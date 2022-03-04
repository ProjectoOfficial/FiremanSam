#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FirebaseESP32.h>
#include "index.h"

#include "ExtEEPROM.h"
#include "Wire.h"

#define CONNECT_TIME 20000
#define TIMEOUTTIME 5000
#define DELAYTIME 1000



/*                                    *************************************
 ***************************************      FIREBASE CONFIGURATION      **********************************************
 *                                    *************************************
*/

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

FirebaseData fData;
FirebaseJson json;

unsigned long updateTime = 0;

/*                                    *************************************
 ***************************************    WEBSERVER CONFIGURATION      **********************************************
 *                                    *************************************
*/
#define BLINK_TIME 500
#define MAX_STRING_LENGTH 80 //limiting ssid, psw, email, dev length
#define STRING_SEPARATOR 0xFD
AsyncWebServer server(80);

bool CONFIGURATE = true;
unsigned long timeBlink = millis();

const char* ssid_AP     = "FiremanSam-Sensor";
const char* password_AP = "12345678";

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "password";
const char* EMAIL_ADDRESS = "email";
const char* DEVICE_NAME = "device";

String input_SSID = "";
String input_PASSWORD = "";
String input_EMAIL = "";
String input_DEVICE = "";

String error = "";

IPAddress IPAddress_AP(192, 168, 1, 1);
IPAddress subnet_AP(255, 255, 255, 0);

void led_blink() {
  /*
     @brief it is used for showing to the user that the device
            is in configuration mode
  */
  if (millis() - timeBlink > BLINK_TIME) {
    digitalWrite(LED, !digitalRead(LED));
    timeBlink = millis();
  }
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void configure() {
  /*
     @brief implements the configuration webserver. It is used for
            configuring the FiremanSensor for user's local network
  */

  //ROOT PAGE REDIRECT TO SETUP PAGE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", setup_html);
  });

  //SETUP PAGE
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest * request) {
    error = "";

    //WiFi SSID
    if (request->getParam(WIFI_SSID)->value() != "")
      input_SSID = request->getParam(WIFI_SSID)->value();
    else
      error += "<li>WiFi SSID not configured</li>";

    //WiFi Password
    if (request->getParam(WIFI_PASSWORD)->value() != "")
      input_PASSWORD = request->getParam(WIFI_PASSWORD)->value();
    else
      error += "<li>WiFi Password not configured</li>";

    //Email Address
    if (request->getParam(EMAIL_ADDRESS)->value() != "")
      input_EMAIL = request->getParam(EMAIL_ADDRESS)->value();
    else
      error += "<li>Email Address not configured</li>";

    //Device Name
    if (request->getParam(DEVICE_NAME)->value() != "")
      input_DEVICE = request->getParam(DEVICE_NAME)->value();
    else
      error += "<li>Device name not configured</li>";

    if (input_SSID.length() > MAX_STRING_LENGTH) error += "<li>SSID too Long!</li>";
    if (input_PASSWORD.length() > MAX_STRING_LENGTH) error += "<li>Password too Long!</li>";
    if (input_EMAIL.length() > MAX_STRING_LENGTH) error += "<li>Email too Long!</li>";
    if (input_DEVICE.length() > MAX_STRING_LENGTH) error += "<li>Device name too Long!</li>";

    if (error != "")
      request->send(200, "text/html", fail_html1 + error + fail_html2);
    else {
      store(input_SSID, input_PASSWORD, input_EMAIL, input_DEVICE);
      request->send(200, "text/html", success_html);
    }
  });

  //ERROR PAGE
  server.on("/error", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", setup_html);
  });

  //SUCCESS PAGE
  server.on("/success", HTTP_GET, [](AsyncWebServerRequest * request) {
    ESP.restart();
  });
}

/*                                    *************************************
 ***************************************      EEPROM CONFIGURATION       **********************************************
 *                                    *************************************
*/
ExtEEPROM ee = ExtEEPROM();
#define EEPROM_PIN 26

#define CONFIGURED_EEPROM 0
#define SSID_EEPROM       100
#define PASSWORD_EEPROM   (SSID_EEPROM + MAX_STRING_LENGTH + 1)
#define EMAIL_EEPROM      (PASSWORD_EEPROM + MAX_STRING_LENGTH + 1)
#define DEVICE_EEPROM     (EMAIL_EEPROM + MAX_STRING_LENGTH + 1)

#define STORE_DELAY 2000
#define READ_DELAY  5

#define RESET_DELAY 3000 //3 secondi per resettare la EEPROM
unsigned long start_reset;

void store(String ssid, String password, String email, String device) 
{
  String sep = String((char)STRING_SEPARATOR);
  String buff = String(1) + sep + ssid + sep + password + sep + email + sep + device;
  digitalWrite(EEPROM_PIN, HIGH);
  ee.EWrite(buff);
  delay(STORE_DELAY);
  digitalWrite(EEPROM_PIN, LOW);
}

void load() {
  digitalWrite(EEPROM_PIN, HIGH);
  String sep = String((char)STRING_SEPARATOR);
  String buff = String(ee.ERead());
  digitalWrite(EEPROM_PIN, LOW);

  Serial.println(buff);

  String conf = buff.substring(0, 1);

  buff.remove(0, 2);
  input_SSID = buff.substring(0, buff.indexOf(sep, 1));

  buff.remove(0, input_SSID.length() + 1);
  input_PASSWORD = buff.substring(0, buff.indexOf(sep, 1));

  buff.remove(0, input_PASSWORD.length() + 1);
  input_EMAIL = buff.substring(0, buff.indexOf(sep, 1));

  buff.remove(0, input_EMAIL.length() + 1);
  input_DEVICE = buff;

  Serial.println(conf);
  Serial.println(input_SSID);
  Serial.println(input_PASSWORD);
  Serial.println(input_EMAIL);
  Serial.println(input_DEVICE);

  if (conf == String(1))
    CONFIGURATE = false;
}

void reset_eeprom() {
  digitalWrite(EEPROM_PIN, HIGH);
  ee.writeEEPROM(0, (uint8_t) 255);
  delay(10);
  digitalWrite(EEPROM_PIN, LOW);
  ESP.restart();
}

/*                                    *************************************
 ***************************************           VOID SETUP            **********************************************
 *                                    *************************************
*/
void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(RESET_PIN, INPUT);
  pinMode(EEPROM_PIN, OUTPUT);

  ee.begin();
  load();
  delay(DELAYTIME);

  if (CONFIGURATE) { // ASK USER TO CONFIGURATE SENSOR THROUGH WEB SERVER
    if (!WiFi.softAPConfig(IPAddress_AP, IPAddress_AP, subnet_AP)) {
      Serial.println("STA Failed to configure");
    }

    WiFi.softAP(ssid_AP, password_AP);
    Serial.print("Access Point IP address: ");
    Serial.println(WiFi.softAPIP());
    delay(DELAYTIME);

    configure();

    server.onNotFound(notFound);
    server.begin();
  }
  else { // LET SENSOR START ITS JOB
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.print("Attempting to connect to ");
    Serial.print(input_SSID);
    WiFi.begin((char *)&input_SSID, (char *)&input_PASSWORD);

    size_t start_time = millis();
    size_t dot_time = millis();

    while ((WiFi.status() != WL_CONNECTED) && (start_time + CONNECT_TIME) > millis()) {
      if (dot_time + 2000 < millis()) {
        Serial.print(".");
        dot_time = millis();
      }

      if (WiFi.status() == WL_CONNECT_FAILED)
        break;
    }

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println(" cannot connect to WiFi!");
      while (true)
        reset_monitor();
    }


    delay(DELAYTIME);
    Serial.println();
    Serial.print("Device IP: ");
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    Firebase.setReadTimeout(fData, 1000 * 60);
    Firebase.setwriteSizeLimit(fData, "tiny");

    updateTime = millis();
  }
  delay(DELAYTIME);
}

/*                                    *************************************
 ***************************************         RESET MONITOR           **********************************************
 *                                    *************************************
*/

void reset_monitor() {
  if (digitalRead(RESET_PIN)) {
    if (millis() - start_reset > RESET_DELAY) {
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED, !digitalRead(LED));
        delay(100);
      }
      reset_eeprom();
    }
  } else {
    start_reset = millis();
  }
}

/*                                    *************************************
 ***************************************            VOID LOOP            **********************************************
 *                                    *************************************
*/
void loop() {
  if (!CONFIGURATE) {
    

    reset_monitor();
  } 
  else 
  {
    led_blink();
  }
}
