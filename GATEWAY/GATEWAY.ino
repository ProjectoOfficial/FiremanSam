#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FirebaseESP32.h>

#include "index.h"
#include "gateway.h"

#define RESET_PIN 4
#define RED 27
#define GREEN 26
#define BLUE 32

#define BLINK_TIME 500
unsigned long timeBlink = millis();

void led_blink(const int pin)
{
  /*
     @brief it is used for showing to the user that the device
            is in configuration mode
  */
  if (millis() - timeBlink > BLINK_TIME)
  {
    if(pin != BLUE) digitalWrite(BLUE, LOW);
    if(pin != RED) digitalWrite(RED, LOW);
    if(pin != GREEN) digitalWrite(GREEN, LOW);

    digitalWrite(pin, !digitalRead(pin));
    timeBlink = millis();
  }
}

void light_led(const int pin){
  /*
   * @brief turns on one led color channel per time
   * 
   * @param pin the led pin to be turned on 
  */
  if(pin == BLUE){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
  }

  if(pin == RED){
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
  }

  if(pin == GREEN){
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, HIGH);
  }
}

/*                                    *************************************
 ***************************************            SD CARD             **********************************************
 *                                    *************************************
*/
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_MISO 19
#define SD_MOSI 23
#define SD_CS 5
#define SD_CLK 18

#define RESET_DELAY 3000 //3 secondi per resettare la SD

const String CONFIG_FILE = "/config.txt";
const String SENSORS_FILE = "/sensors.txt";
const String ACTUATORS_FILE = "/actuators.txt";

unsigned long start_reset;


/*                                    *************************************
 ***************************************         RESET MONITOR           **********************************************
 *                                    *************************************
*/

void reset_monitor()
{
  /*
   * @brief this function monitors the reset button which brings the device back to factory state
  */
  if (digitalRead(RESET_PIN)) {
    if (millis() - start_reset > RESET_DELAY) {
      for (int i = 0; i < 10; i++) {
        digitalWrite(BLUE, !digitalRead(BLUE));
        delay(100);
      }
      Serial.println("Gateway reset successfully");
      SD.remove(CONFIG_FILE);
    }
  } else
    start_reset = millis();
}

/*                                    *************************************
 ***************************************         STRING SPLIT           **********************************************
 *                                    *************************************
*/

const String splitString(const String str) {
  /*
   * @brief it is used for removing the @ from the inserted email (@ creates problems with firebase) 
  */
  String buff = str;
  return buff.substring(0, buff.indexOf('@'));
}


/*                                    *************************************
 ***************************************             FIREBASE           **********************************************
 *                                    *************************************
*/

#define FIREBASE_HOST "https://firemansam-459c0-default-rtdb.europe-west1.firebasedatabase.app/"
#define FIREBASE_AUTH "7i1dpSEfmefSNWJ9BZr26QW78gtCuSbbN8vUCAz7"

//Define Firebase Data objects
FirebaseData fbdo;
FirebaseJson json;

unsigned long updateTime = 0;
/*                                    *************************************
 ***************************************    WEBSERVER CONFIGURATION      **********************************************
 *                                    *************************************
*/

#define CONNECT_TIME 10000
#define TIMEOUTTIME 5000

bool CONFIGURATE = true;

AsyncWebServer server(80);

const char* ssid_AP     = "FiremanSam-Gateway";
const char* password_AP = "12345678";

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "password";
const char* EMAIL = "email";
const char* IP_ADDRESS = "IPAddress";
const char* IP_ROUTER = "IPRouter";
const char* SUBNET_MASK = "Subnet";

String input_SSID = "";
String input_PASSWORD = "";
String input_EMAIL = "";
String input_IP = "";
String input_ROUTER = "";
String input_SUBNET = "";

String error = "";

IPAddress local_IP(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

IPAddress Gateway_IP;
IPAddress Router_IP;
IPAddress Subnet;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void configure() {

  //ROOT PAGE REDIRECT TO SETUP PAGE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", setup_html);
  });

  //SETUP PAGE
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest * request) {
    error = "";

    //WIFI SSID
    if (request->getParam(WIFI_SSID)->value() != "")
      input_SSID = request->getParam(WIFI_SSID)->value();
    else
      error += "<li>WiFi SSID not configured</li>";

    //WIFI PASSWORD
    if (request->getParam(WIFI_PASSWORD)->value() != "")
      input_PASSWORD = request->getParam(WIFI_PASSWORD)->value();
    else
      error += "<li>WiFi Password not configured</li>";

    //EMAIL
    if (request->getParam(EMAIL)->value() != "")
      input_EMAIL = request->getParam(EMAIL)->value();
    else
      error += "<li>Forgot to enter your email</li>";

    //GATEWAY STATIC IP ADDRESS
    if (request->getParam(IP_ADDRESS)->value() != "") {
      input_IP = request->getParam(IP_ADDRESS)->value();
      if (!Gateway_IP.fromString(input_IP))
        error += "<li>IP Address invalid</li>";
    } else
      error += "<li>Gateway IP Address not configured</li>";

    //ROUTER IP ADDRESS
    if (request->getParam(IP_ROUTER)->value() != "") {
      input_ROUTER = request->getParam(IP_ROUTER)->value();
      if (!Router_IP.fromString(input_ROUTER))
        error += "<li>Router IP invalid</li>";
    } else
      error += "<li>Router IP Address not configured</li>";

    //SUBNET MASK
    if (request->getParam(SUBNET_MASK)->value() != "") {
      input_SUBNET = request->getParam(SUBNET_MASK)->value();
      if (!Subnet.fromString(input_SUBNET))
        error += "<li>Subnet Mask invalid</li>";
    } else
      error += "<li>Subnet Mask not configured</li>";

    if (error != "")
      request->send(200, "text/html", fail_html1 + error + fail_html2);
    else
      request->send(200, "text/html", success_html);

    if (SD.exists(CONFIG_FILE))
      SD.remove(CONFIG_FILE);

    File file = SD.open(CONFIG_FILE, FILE_WRITE);

    if (!file) {
      Serial.println("failed to create the CONFIG file");
      while (1);
    }

    input_EMAIL = splitString(input_EMAIL);
    String message = input_SSID + "\n" + input_PASSWORD + "\n" + input_EMAIL + "\n" + input_IP + "\n" + input_ROUTER + "\n" + input_SUBNET + "\n";
    if (!file.print(message)) {
      Serial.println("failed to save CONFIG file");
      while (1)
      {
        reset_monitor();
      }
    }
    file.close();
  });

  //ERROR PAGE
  server.on("/error", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", setup_html);
  });

  //SUCCESS PAGE
  server.on("/success", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Restarting Gateway");
    ESP.restart();
  });
}

/*                                    *************************************
 ***************************************             GATEWAY            **********************************************
 *                                    *************************************
*/
String populate_dropdown(const String filename) {
  /*
   * @brief populates the dropdownlist for devices pairing
   *
   * @param filename the file used for extracting the devices 
  */
  File fs = SD.open(filename, FILE_READ);

  if (!fs) {
    Serial.print("failed to open ");
    Serial.println(filename);
    return "";
  }

  String buff = "";
  while (fs.available()) {
    String dt = fs.readStringUntil('\n');
    buff += "<option value=\"" + dt + "\">" + dt + "</option>";
  }
  fs.close();
  return buff;
}

void update_sensor_add(const String sensor_name, const String actuator_name) {
  /*
   * @brief adds an actuator to a specific sensor file
   *
   * @param sensor_name the sensor to pair with the actuator, it should have its own file
   * @param actuator_name the name of the actuator to be inserted into sensor file for pairing
  */
  String fname = "/" + sensor_name + ".txt";
  File fs = SD.open(fname, FILE_READ);

  if (!fs) {
    Serial.print(sensor_name);
    Serial.println(" does not exists");
  } 
  else {
    while (fs.available()) {
      if (fs.readStringUntil('\n') == actuator_name)
        return;
    }
    fs.close();
  }

  File fd = SD.open(fname, FILE_WRITE);

  if (!fd) {
    Serial.print("failed to create/open ");
    Serial.print(sensor_name);
    Serial.println(".txt");
    return;
  }
  fd.println(actuator_name);
  fd.close();
}

void update_sensor_remove(const String sensor_name, const String actuator_name){
  /*
   * @brief removes an actuator to a specific sensor file
   *
   * @param sensor_name the sensor to unpair with the actuator, it should have its own file
   * @param actuator_name the name of the actuator to be removed from sensor file 
  */

  String fname = "/" + sensor_name + ".txt";
  String tname = "/temp.txt";

  File fs = SD.open(fname, FILE_READ);
  File tmp = SD.open(tname, FILE_WRITE);

  if (!fs) {
    Serial.print(sensor_name);
    Serial.println(" file open error");
    return;
  } 

  if(!tmp){
    Serial.println("error during tmp file creation");
    return;
  }

  while (fs.available()) {
    String line = fs.readStringUntil('\n');
    if (line != actuator_name)
      tmp.println(line);
  }
  
  fs.close();
  SD.remove(fname);

  if(tmp.size() == 0){
    tmp.close();
    SD.remove(tname);
    return;
  }
  tmp.close();

  File fd = SD.open(fname, FILE_WRITE);
  File tmp2 = SD.open(tname, FILE_READ);

   if (!fs) {
    Serial.print(sensor_name);
    Serial.println(" file open error");
    return;
  } 

  if(!tmp2){
    Serial.println("error during tmp2 file open");
    return;
  }

  while (tmp2.available()) 
    fd.println(tmp2.readStringUntil('\n'));

  fd.close();
  tmp2.close();
  SD.remove(tname);
  
}

String get_sensor_files(){
  /*
   * @brief returns a string containing only sensor filenames separated by ;
  */
  String buff = "";
  String separator = ";";

  File root = SD.open("/");

  if(!root){
    return buff;
  }

  while(true){
    File entry = root.openNextFile();

    if(!entry)
      break;

    if(!entry.isDirectory()){
      String fname = entry.name();
      if(fname == CONFIG_FILE || fname == SENSORS_FILE || fname == ACTUATORS_FILE)
        continue;

      buff += fname + separator;
    }
    entry.close();
  }

  root.close();
  return buff;
}

String get_pair_box(String files){
  /*
   * @brief prepares an html table for each sensor paired with one or more actuators, to be displayed on the webserver
   *
   * @param files contains each sensor file which is used for actuator name retrieving
  */
  int last_index = 0;
  String out = "";

  while(true){
    int index = files.indexOf(";", last_index);
    
    if(index == -1)
      break;

    String file = files.substring(last_index, index);

    File fs = SD.open(file, FILE_READ);

    if(fs){
      uint8_t n_act = 0;
      while(fs.available()){
        String dt = fs.readStringUntil('\n');

        if(n_act == 0)
          out += sensor_html + file + sensor2_html + dt + sensor3_html;
        else
          out += sensor4_html + dt + sensor5_html;
        ++n_act;
      }

      if(n_act == 1)
        out += sensornomore_html;

      fs.close(); 
    }    
    last_index = index + 1;
  } 
  return out;
}

void Gateway() {
  /*
   * @brief manages the gateway webserver 
  */
 
 // REDIRECT
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String files = get_sensor_files();
    Serial.println(files);
    String gw = get_pair_box(files);
    request->send(200, "text/html", gateway_html + gw + gateway2_html);
  });

  // PAIRING PAGE
  server.on("/pair", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("home")) {
      Serial.println("routing to home...");
      
      String files = get_sensor_files();
      Serial.println(files);
      String gw = get_pair_box(files);
      request->send(200, "text/html", gateway_html + gw + gateway2_html);
    }

    if (request->hasParam("pair")) {
      String sensor = "";
      String actuator = "";
      if (request->hasParam("sensori"))
        sensor = request->getParam("sensori")->value();
      if (request->hasParam("attuatori"))
        actuator = request->getParam("attuatori")->value();

      if (sensor != "" && actuator != "")
        update_sensor_add(sensor, actuator);
    }

    if (request->hasParam("unpair")) {
      String sensor = "";
      String actuator = "";
      if (request->hasParam("sensori"))
        sensor = request->getParam("sensori")->value();
      if (request->hasParam("attuatori"))
        actuator = request->getParam("attuatori")->value();
        
      if (sensor != "" && actuator != "")
        update_sensor_remove(sensor, actuator);

    }
    
    String files = get_sensor_files();
    Serial.println(files);
    String gw = get_pair_box(files);
    request->send(200, "text/html", gateway_html + gw + gateway2_html);
  });

  // HOME PAGE
  server.on("/gateway", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("reset")) {
      Serial.println("Restarting Gateway ...");
      ESP.restart();
    }

    if (request->hasParam("pair")) {
      Serial.println("routing to accoppia...");
      String page = pair_html + populate_dropdown(SENSORS_FILE) + pair_html2 + populate_dropdown(ACTUATORS_FILE) + pair_html3;
      request->send(200, "text/html", page);
    }
  });
}

/*                                    *************************************
 ***************************************             SD CARD            **********************************************
 *                                    *************************************
*/
int SDCard_Setup() {
  Serial.println("SD CARD SETUP");
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return -1;
  } else
    Serial.println("SD Card Found!");

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return -1;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  double cardSize = SD.cardSize() / (1024 * 1024);
  Serial.print("SD Card Size: ");
  Serial.print(cardSize, 3);
  Serial.println("MB");

  CONFIGURATE = false;
  return 1;
}

int SD_get_data() {
  if (!SD.exists(CONFIG_FILE)) {
    Serial.println("Failed to load data, Config File doesn't exist");
    return -1;
  }

  File file = SD.open(CONFIG_FILE, FILE_READ);
  if (!file) {
    Serial.println("Failed to load data, cannot open file");
    return -1;
  }

  String Buffer = "";
  uint8_t cont = 0;
  char c;
  while (file.available()) {
    c = file.read();
    String s = (String)c;
    if (s != "\n")
      Buffer += s;
    else {
      switch (cont) {
        case 0: input_SSID = Buffer; break;
        case 1: input_PASSWORD = Buffer; break;
        case 2: input_EMAIL = Buffer; break;
        case 3: input_IP = Buffer; break;
        case 4: input_ROUTER = Buffer; break;
        case 5: input_SUBNET = Buffer; break;
      }
      Buffer = "";
      cont++;
    }
  }

  if (!Gateway_IP.fromString(input_IP.c_str())) {
    Serial.println("Failed to retrieve Gateway IP Address from SD");
    file.close();
    return -1;
  }
  if (!Router_IP.fromString(input_ROUTER.c_str())) {
    Serial.println("Failed to retrieve Router IP Address from SD");
    file.close();
    return -1;
  }
  if (!Subnet.fromString(input_SUBNET.c_str())) {
    Serial.println("Failed to retrieve Subnet Mask from SD");
    file.close();
    return -1;
  }

  file.close();
  return 1;
}

/*                                    *************************************
 ***************************************   SENSORS STRING EXCTRACTOR    **********************************************
 *                                    *************************************
*/

void devices_extractor(const String str, const String filename) {
  /*
     @brief extracts sensors from a firebase request and saves them on sensors.txt file

     @param str the firebase request
  */

  int last_index = 0;
  String all_devices = "";

  while (true) {
    int index = str.indexOf(',', last_index + 1);
    String substr = "";
    if (index == -1) {
      substr = str.substring(last_index);
    } else
      substr = str.substring(last_index, index);

    int fapex = substr.indexOf('"');
    int lapex = substr.indexOf('"', fapex + 1);
    all_devices += substr.substring(fapex + 1, lapex) + '\n';

    if (index == -1)
      break;
    else
      last_index = index;
  }

  File file = SD.open(filename, FILE_WRITE);
  if (!file.print(all_devices))
    Serial.println("failed to save on device file");
  file.close();
}


/*                                    *************************************
 ***************************************       DOWNLOAD SENSORS          **********************************************
 *                                    *************************************
*/

void download_devices() {
  /*
   * @brief retrieves sensors and actuators of a specific user and saves them on two different files
  */
  Firebase.getString(fbdo, input_EMAIL + "/SENSORS/");
  String str = fbdo.to<String>();
  devices_extractor(str, SENSORS_FILE);
  Serial.println(str);

  Firebase.getString(fbdo, input_EMAIL + "/ACTUATORS/");
  str = fbdo.to<String>();
  devices_extractor(str, ACTUATORS_FILE);
  Serial.println(str);
}



/*                                    *************************************
 ***************************************       SENSOR TO ACTUATOR       **********************************************
 *                                    *************************************
*/

void check_fire(){
  String files = get_sensor_files();

  int last_index = 0;
  String out = "";

  while(true){
    int index = files.indexOf(";", last_index);
    
    if(index == -1)
      break;

    String file = files.substring(last_index, index);
    
    int bracket_idx = file.indexOf("/");
    int dot_idx = file.indexOf(".");
    String sensor = file.substring(bracket_idx + 1, dot_idx);

    File fs = SD.open(file, FILE_READ);

    if(fs){
      Firebase.getInt(fbdo,  input_EMAIL + "/SENSORS/" + sensor + "/score");
      int score = fbdo.to<int>();

      while(fs.available()){
        String actuator = fs.readStringUntil('\n');
        
        if(score > 0){
          String updateStr = input_EMAIL + "/ACTUATORS/" + actuator.substring(0, actuator.indexOf("\r")) + "/alarm";
          Firebase.setInt(fbdo, updateStr, 1);
        }
      }
      fs.close();
    }    
    last_index = index + 1;
  } 
}

/*                                    *************************************
 ***************************************          VOID SETUP            **********************************************
 *                                    *************************************
*/
void setup() {
  Serial.begin(115200);
  pinMode(RESET_PIN, INPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);

  if (SDCard_Setup() < 0) {
    Serial.println("SD Card error, cannot proceed");
    light_led(RED);
    while (1)
      reset_monitor();
  }

  if (SD_get_data() < 0)
    CONFIGURATE = true;

  if (CONFIGURATE) {
    if (!SD.exists(CONFIG_FILE)) {
      // ASK USER TO CONFIGURATE SENSOR THROUGH WEB SERVER
      WiFi.softAP(ssid_AP, password_AP);
      if (!WiFi.softAPConfig(local_IP, local_IP, subnet)) {
        Serial.println("STA Failed to configure");
        light_led(RED);
        while(1)
          reset_monitor();
      }
      IPAddress IP = WiFi.softAPIP();
      Serial.print("Access Point IP address: ");
      Serial.println(IP);

      configure();
      server.onNotFound(notFound);
      server.begin();
    }
  }
  else {
    // LET GATEWAY START ITS JOB
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.print("Attempting to connect to ");
    Serial.print(input_SSID);

    if (!WiFi.config(Gateway_IP, Router_IP, Subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
      light_led(RED);
      while(1)
        reset_monitor();
    }

    WiFi.begin((char *) input_SSID.c_str(), (char *) input_PASSWORD.c_str());

    unsigned long start_time = millis();
    unsigned long dot_time = millis();

    while ((WiFi.status() != WL_CONNECTED) && (start_time + CONNECT_TIME) > millis()) {
      if (dot_time + 2000 < millis()) {
        Serial.print(".");
        dot_time = millis();
      }

      if (WiFi.status() == WL_CONNECT_FAILED)
        break;
    }
    Serial.println("");

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println(" cannot connect to WiFi!");
        light_led(RED);
      while (1)
        reset_monitor();
    }

    Serial.println("WiFi Connected!");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.subnetMask());
    Serial.println(WiFi.gatewayIP());

    Gateway();
    server.onNotFound(notFound);
    server.begin();

    //***************************FIREBASE INITIALIZATION************************

    Serial.println("Starting Firebase Communication:\n");
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    Firebase.setReadTimeout(fbdo, 1000 * 60);
    Firebase.setwriteSizeLimit(fbdo, "tiny");

    download_devices();
    updateTime = millis();
    light_led(GREEN);
  }
}


/*                                    *************************************
 ***************************************            VOID LOOP           **********************************************
 *                                    *************************************
*/
void loop() {
  if (!CONFIGURATE)
  {
    if (millis() - updateTime > 300)
    {  
      check_fire();
      updateTime = millis();
    }
    reset_monitor();
  }
  else
    led_blink(BLUE);
}
