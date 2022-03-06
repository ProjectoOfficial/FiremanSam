#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FirebaseESP32.h>
#include "index.h"

#define RESET_PIN 4
#define RED 32
#define GREEN 35
#define BLUE 34

//***********************SD CARD CONFIGURATION********************
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_MISO 19
#define SD_MOSI 23
#define SD_CS 5
#define SD_CLK 18

#define RESET_DELAY 3000 //3 secondi per resettare la SD

const String CONFIG_FILE = "/config.txt";
const String PAIRS_FILE = "/pairs.txt";
const String SENSORS_FILE = "/sensors.txt";
const String ACTUATORS_FILE = "/actuators.txt";

unsigned long start_reset;


// *********************FIREBASE DATABASE CREDENTIALS AND OBJECTS******************

// Insert Firebase project API Key
#define FIREBASE_AUTH "7i1dpSEfmefSNWJ9BZr26QW78gtCuSbbN8vUCAz7"

// Insert RTDB URLefine the RTDB URL */
#define FIREBASE_HOST "https://firemansam-459c0-default-rtdb.europe-west1.firebasedatabase.app/"

//Define Firebase Data objects
FirebaseData fData;
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
  });

  //ERROR PAGE
  server.on("/error", HTTP_GET, [](AsyncWebServerRequest * request) {

    request->send(200, "text/html", setup_html);
  });

  //SUCCESS PAGE
  server.on("/success", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (SD.exists(CONFIG_FILE))
      SD.remove(CONFIG_FILE);
    if (SD.exists(PAIRS_FILE))
      SD.remove(PAIRS_FILE);

    File file = SD.open(CONFIG_FILE, FILE_WRITE);
    if (!file) {
      Serial.println("failed to open file");
      while (1);
    }
    String message = input_SSID + "\n" + input_PASSWORD + "\n" + input_EMAIL + "\n" + input_IP + "\n" + input_ROUTER + "\n" + input_SUBNET + "\n";
    if (!file.print(message)) {
      Serial.println("failed to save file");
      while (1);
    }

    file.close();
    Serial.println("Restarting Gateway");
    ESP.restart();
  });
}

void Gateway() {
  //ROOT PAGE REDIRECT TO SETUP PAGE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", gateway_html);
  });

  // HOME PAGE
  server.on("/gateway", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
}

// **************************SD CARD SETUP****************************
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

  return 1;
}


// **************************SD CARD GET DATA****************************
void SD_get_data() {
  if (!SD.exists(CONFIG_FILE)) {
    Serial.println("Failed to load data, Config File doesn't exist");
    CONFIGURATE = true;
    return;
  }

  File file = SD.open(CONFIG_FILE, FILE_READ);
  if (!file) {
    Serial.println("Failed to load data, cannot open file");
    while (1);
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

  char* GWIP = (char *)malloc(input_IP.length());
  char* RIP = (char *)malloc(input_ROUTER.length());
  char* MASK = (char *)malloc(input_SUBNET.length());

  input_IP.toCharArray(GWIP, input_IP.length());
  input_ROUTER.toCharArray(RIP, input_ROUTER.length());
  input_SUBNET.toCharArray(MASK, input_SUBNET.length());

  if (!Gateway_IP.fromString((const char*)GWIP)) {
    Serial.println("Failed to retrieve Gateway IP Address from SD");
    file.close();
    return;
  }
  if (!Router_IP.fromString((const char*)RIP)) {
    Serial.println("Failed to retrieve Router IP Address from SD");
    file.close();
    return;
  }
  if (!Subnet.fromString((const char*)MASK)) {
    Serial.println("Failed to retrieve Subnet Mask from SD");
    file.close();
    return;
  }

  file.close();
  CONFIGURATE = false;
}

/*                                    *************************************
 ***************************************         RESET MONITOR           **********************************************
 *                                    *************************************
*/

void reset_monitor()
{
  /*
     @brief this function monitors the reset button which
            brings the device back to factory state
  */
  if (digitalRead(RESET_PIN))
  {
    if (millis() - start_reset > RESET_DELAY)
    {
      for (int i = 0; i < 10; i++)
      {
        digitalWrite(BLUE, !digitalRead(BLUE));
        delay(100);
      }
      SD.remove(CONFIG_FILE);
    }
  } else {
    start_reset = millis();
  }
}


// **************************VOID SETUP****************************
void setup() {
  Serial.begin(115200);
  if (SDCard_Setup() < 0) {
    Serial.println("SD Card error, cannot proceed");
    while (1)
    {
      reset_monitor();
    }
  }
  SD_get_data();

  if (CONFIGURATE) {
    if (!SD.exists(CONFIG_FILE)) {
      // ASK USER TO CONFIGURATE SENSOR THROUGH WEB SERVER
      WiFi.softAP(ssid_AP, password_AP);
      if (!WiFi.softAPConfig(local_IP, local_IP, subnet)) {
        Serial.println("STA Failed to configure");
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
    Serial.print("Attempting to connect to ");
    Serial.println(input_SSID);

    WiFi.begin((char *) input_SSID.c_str(), (char *) input_PASSWORD.c_str());

    IPAddress primaryDNS(8, 8, 8, 8);
    IPAddress secondaryDNS(8, 8, 4, 4);
    if (!WiFi.config(Gateway_IP, Router_IP, Subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }

    unsigned long start_time = millis();
    unsigned long dot_time = millis();

    while ((WiFi.status() != WL_CONNECTED) && (start_time + CONNECT_TIME) > millis())
    {
      if (dot_time + 2000 < millis())
      {
        Serial.print(".");
        dot_time = millis();
      }

      if (WiFi.status() == WL_CONNECT_FAILED)
        break;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println(" cannot connect to WiFi!");
      while (true)
      {
        reset_monitor();
      }
    }

    Serial.println("WiFi Connected!");
    Serial.println(WiFi.localIP());

    Gateway();
    server.onNotFound(notFound);
    server.begin();
  }

  //***************************FIREBASE INITIALIZATION************************
  Serial.println("Starting Firebase Communication:\n");
  Serial.println("Firebase.begin:\n");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(fData, 1000 * 60);
  Firebase.setwriteSizeLimit(fData, "tiny");
  delay(1000);

  updateTime = millis();
}


// **************************VOID LOOP****************************
void loop() {
  if (!CONFIGURATE)
  {
    if (millis() - updateTime > 300)
    {
      //tienes el calor
      json.set("/alarm", 1);

      //SCRITTURA
      // Al posto di prova ci va l'attuatore al quale va aggiornato il valore
      //Firebase.updateNode(fData, input_EMAIL+ "/" + "ACTUATORS" + "/"+ "PROVA", json);
      Firebase.updateNode(fData, "Daniel_r/ACTUATORS/Mansarda", json);

      json.set("/score", -1);
      Firebase.updateNode(fData, "Daniel_r/SENSORS/Mansarda", json);


      //LETTURA
      //Firebase.getBool(fData, input_EMAIL + "/" + "ACTUATORS" + "/" + "Mansarda" + "/alarm");
      Firebase.getBool(fData, "Daniel_r/ACTUATORS/Mansarda/alarm");
      bool AlarmValue = fData.to<bool>();

      Serial.println(AlarmValue);

      updateTime = millis();
    }
    reset_monitor();
  }
  else
  {
    //led_blink();
    Serial.println("Gateway Not configured Properly");
  }
}
