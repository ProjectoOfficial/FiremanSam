#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <CCS811.h>

#define CONNECT_TIME 10000
#define TIMEOUTTIME 5000

/*
   SENSOR CONFIGURATION
*/

#define DHTPIN 15
#define DHTTYPE DHT11
#define LED 16

DHT dht(DHTPIN, DHTTYPE);
CCS811 cc811;

float weights[] = { -0.004163, 0.005974, 005523, 0.005606};
float bias = -0.007646;
float means[] = {54.023747, 25.116755, 1655.704485, 1035.536939};
float devs[] = {12.707672, 4.145223, 1496.357791, 2692.752151};

size_t updateTime = 0;

/*
   WEB SERVER CONFIGURATION
*/
AsyncWebServer server(80);

bool CONFIGURATE = true;

const char* ssid_AP     = "FiremanSam-Sensor";
const char* password_AP = "12345678";

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "password";
const char* EMAIL_ADDRESS = "Email";

String input_SSID = "";
String input_PASSWORD = "";
String input_EMAIL = "";

String error = "";

IPAddress local_IP(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

/**********************VOID CONFIGURE********************************/
void configure() {

  //ROOT PAGE REDIRECT TO SETUP PAGE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", setup_html);
  });

  //SETUP PAGE
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest * request) {
    error = "";

    if (request->getParam(WIFI_SSID)->value() != "")
      input_SSID = request->getParam(WIFI_SSID)->value();
    else
      error += "<li>WiFi SSID not configured</li>";

    if (request->getParam(WIFI_PASSWORD)->value() != "")
      input_PASSWORD = request->getParam(WIFI_PASSWORD)->value();
    else
      error += "<li>WiFi Password not configured</li>";

    if (request->getParam(EMAIL_ADDRESS)->value() != "") {
      input_EMAIL = request->getParam(EMAIL_ADDRESS)->value();
    }
    else
      error += "<li>Email Address not configured</li>";


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
    ESP.restart();
  });
}

/**********************VOID SETUP********************************/
void setup() {
  /************************************
        LEGGERE I DATI SULLA EEPROM
        se non ci sono procedere con la configurazione
  */

  pinMode(LED, OUTPUT);

  if (CONFIGURATE) {
    // ASK USER TO CONFIGURATE SENSOR THROUGH WEB SERVER
    WiFi.softAP(ssid_AP, password_AP);
    if (!WiFi.softAPConfig(local_IP, local_IP, subnet)) {
      Serial.println("STA Failed to configure");
    }
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Access Point IP address: ");
    Serial.println(IP);

    configure();
    /************************************
        SALVARE I DATI SULLA EEPROM
    */

    server.onNotFound(notFound);
    server.begin();
  }
  else {
    /*  
     *   LEGGERE DATI DALLA EEPROM
     */
    // LET SENSOR START ITS JOB
    Serial.print("Attempting to connect to ");
    WiFi.begin((char *)&input_SSID, (char *)&input_PASSWORD);

    size_t start_time = millis();
    size_t dot_time = millis();
    while ((WiFi.status() != WL_CONNECTED) && (start_time + CONNECT_TIME) > millis()) {
      if (dot_time + 250 < millis()) {
        Serial.print(".");
        dot_time = millis();
      }
    }

    while (cc811.begin() != 0)
      delay(100);
    dht.begin();
    cc811.setMeasCycle(cc811.eCycle_250ms);
  }
}

/**********************DHT11 READ********************************/
void readDHT11(float *h, float *t) {
  *h = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(*h) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  *t = dht.computeHeatIndex(temp, *h, false);

  Serial.print(F("Humidity: "));
  Serial.print(*h);
  Serial.print(F("%  Temperature: "));
  Serial.print(*t);
  Serial.print(F("°C "));
}

/**********************CCS811 READ********************************/
void readCC811(int *co2, int *tvoc) {
  if (cc811.checkDataReady() == true) {
    *co2 = cc811.getCO2PPM();
    *tvoc = cc811.getTVOCPPB();
    Serial.print("CO2: ");
    Serial.print(*co2);
    Serial.print("ppm, TVOC: ");
    Serial.print(*tvoc);
    Serial.println("ppb");

  } else {
    Serial.println("Data is not ready!");
  }
  cc811.writeBaseLine(0x847B);
}

/**********************VOID LOOP********************************/
void loop() {
  if (!CONFIGURATE) {
    if (millis() - updateTime > 500) {
      float humidity, temperature ;
      int co2 = 0, tvoc = 0;
      readDHT11(&humidity, &temperature);
      readCC811(&co2, &tvoc);

      if (co2 != 0) {
        float datas[] = {humidity, temperature, co2, tvoc};
        float score = 0;
        for (int i = 0; i < 4; i++) {
          score += (datas[i] - means[i]) / devs[i] * weights[i];
        }
        score += bias;
        Serial.print("Score: ");
        Serial.print(score);

        if (score > 0) {
          Serial.println("\nFire detected!");
          digitalWrite(LED, HIGH);
        }
        else {
          Serial.println("\nNothing");
          digitalWrite(LED, LOW);
        }
        Serial.print("\n");
        updateTime = millis();
      }
    }
  }
}
