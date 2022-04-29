#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FirebaseESP32.h>
#include "index.h"

#include <ExtEEPROM.h>
#include "Wire.h"

#define CONNECT_TIME 20000
#define DELAYTIME 100

#define LED 12
#define RESET_PIN 14
#define BUZZER 4

/*                                    *************************************
 ***************************************      FIREBASE CONFIGURATION      **********************************************
 *                                    *************************************
*/

#define RESET_ALARM 500

#define FIREBASE_HOST "https://firemansam-459c0-default-rtdb.europe-west1.firebasedatabase.app/"
#define FIREBASE_AUTH "7i1dpSEfmefSNWJ9BZr26QW78gtCuSbbN8vUCAz7"

FirebaseData fData;
FirebaseJson json;

unsigned long updateTime = 0;
unsigned long reset_alarmTime = 0;

/*                                    *************************************
 ***************************************    WEBSERVER CONFIGURATION      **********************************************
 *                                    *************************************
*/
#define BLINK_TIME 500
#define BUZZ_TIME 200

#define MAX_STRING_LENGTH 80 //limiting ssid, psw, email, dev length
#define STRING_SEPARATOR 0xFD
AsyncWebServer server(80);

bool CONFIGURATE = true;
unsigned long timeBlink = millis();
unsigned long timeBuzz = millis();

const char* ssid_AP     = "FiremanSam-Actuator";
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

void buzzer_buzz() {
  /*
     @brief it is used to warn in case of fire
  */
  if (millis() - timeBuzz > BUZZ_TIME) {
    digitalWrite(BUZZER, !digitalRead(BUZZER));
    timeBuzz = millis();
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
#define EEPROM_PIN 19

#define STORE_DELAY 300
#define LOAD_DELAY 50

#define RESET_DELAY 3000 //3 secondi per resettare la EEPROM
unsigned long start_reset;

void store(const String ssid, const String password, const String email, const String device) {
  String sep = String((char)STRING_SEPARATOR);
  String buff = String(1) + sep + ssid + sep + password + sep + email + sep + device;

  digitalWrite(EEPROM_PIN, HIGH);
  delay(10);
  ee.begin();
  delay(STORE_DELAY);
  ee.EWrite(buff);
  delay(STORE_DELAY);
  digitalWrite(EEPROM_PIN, LOW);
}

void load() {
  String sep = String((char)STRING_SEPARATOR);

  digitalWrite(EEPROM_PIN, HIGH);
  delay(10);
  ee.begin();
  delay(LOAD_DELAY);
  String buff = String(ee.ERead());
  delay(LOAD_DELAY);
  digitalWrite(EEPROM_PIN, LOW);

  String conf = buff.substring(0, 1);

  buff.remove(0, 2);
  input_SSID = buff.substring(0, buff.indexOf(sep, 1));

  buff.remove(0, input_SSID.length() + 1);
  input_PASSWORD = buff.substring(0, buff.indexOf(sep, 1));

  buff.remove(0, input_PASSWORD.length() + 1);
  input_EMAIL = buff.substring(0, buff.indexOf(sep, 1));

  buff.remove(0, input_EMAIL.length() + 1);
  input_DEVICE = buff;

  if (conf == String(1))
    CONFIGURATE = false;
}

void reset_eeprom() {
  digitalWrite(EEPROM_PIN, HIGH);
  delay(10);
  ee.begin();
  delay(10);
  ee.writeEEPROM(0, (uint8_t) 255);
  delay(10);
  digitalWrite(EEPROM_PIN, LOW);
  ESP.restart();
}

const String splitString(const String str) {
  String buff = str;
  return buff.substring(0, buff.indexOf('@'));
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
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);

  load();

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

    json.set("/alarm", 0);
    Firebase.updateNode(fData, input_EMAIL + "/" + "ACTUATORS" + "/" + input_DEVICE, json);

    updateTime = millis();
  }
  delay(DELAYTIME);
}

void reset_alarm(){
  if (digitalRead(RESET_PIN)) {
    if (millis() - reset_alarmTime < RESET_ALARM) {
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED, !digitalRead(LED));
        delay(300);
      }
      json.set("/alarm", 0);
      Firebase.updateNode(fData, input_EMAIL + "/" + "ACTUATORS" + "/" + input_DEVICE, json);
    }
  } else {
    reset_alarmTime = millis();
  }
}

/*                                    *************************************
 ***************************************         RESET MONITOR           **********************************************
 *                                    *************************************
*/

void reset_monitor() {
  /*
     @brief this function monitors the reset button which
            brings the device back to factory state if hold for at least 3 seconds
  */
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
void loop()
{
  if (!CONFIGURATE) {
    if (millis() - updateTime > 300) {
      Firebase.getInt(fData, splitString(input_EMAIL) + "/" + "ACTUATORS" + "/" + input_DEVICE + "/alarm");
      int AlarmValue = fData.to<int>();

      Serial.println(AlarmValue);

      if (AlarmValue) {
        Serial.println("Fire detected!");
        buzzer_buzz();
        if (!digitalRead(LED))
          digitalWrite(LED, HIGH);
      }
      else {
        if (digitalRead(BUZZER))
          digitalWrite(BUZZER, LOW);

        if (digitalRead(LED))
          digitalWrite(LED, LOW);
      }
      updateTime = millis();
    }
    reset_alarm();
    reset_monitor();
  }
  else {
    led_blink();
  }
}
