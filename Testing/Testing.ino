#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <CCS811.h>
#include <WiFi.h>

#define DHTPIN 15
#define DHTTYPE DHT11
#define LED 16

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

DHT dht(DHTPIN, DHTTYPE);
CCS811 sensor;

float weights[] = { -0.004163, 0.005974, 0.005523, 0.005606};
float bias = -0.007646;
float means[] = {54.023747, 25.116755, 1655.704485, 1035.536939};
float devs[] = {12.707672, 4.145223, 1496.357791, 2692.752151};

size_t updateTime = 0;

/**********************VOID SETUP********************************/
void setup() {
  Serial.begin(115200);

  while (sensor.begin() != 0) {
    Serial.println("failed to init chip, please check if the chip connection is fine");
    delay(1000);
  }

  dht.begin();
  sensor.setMeasCycle(sensor.eCycle_250ms);
  pinMode(LED, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  size_t start_time = millis();
  size_t connection_time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start_time > 500) {
      Serial.print(".");
      start_time = millis();
    }
    if (millis() - connection_time > 3000) {
      Serial.println("Connection aborted");
      while (true);
    }
  }



  Serial.println("Connessione");
  Serial.println();
  Serial.print("WiFi Connesso, indirizzo IP:");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Fatto! WiFi connesso!");
  updateTime = millis();
}

/**********************DHT11 READ********************************/
void humidity_and_temperature(float *h, float *t) {
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
void co2_and_tvoc(int *co2, int *tvoc) {
  if (sensor.checkDataReady() == true) {
    *co2 = sensor.getCO2PPM();
    *tvoc = sensor.getTVOCPPB();
    Serial.print("CO2: ");
    Serial.print(*co2);
    Serial.print("ppm, TVOC: ");
    Serial.print(*tvoc);
    Serial.println("ppb");

  } else {
    Serial.println("Data is not ready!");
  }
  sensor.writeBaseLine(0x847B);
}

/**********************VOID LOOP********************************/
void loop() {
  if (millis() - updateTime > 500) {
    float humidity, temperature ;
    int co2 = 0, tvoc = 0;
    humidity_and_temperature(&humidity, &temperature);
    co2_and_tvoc(&co2, &tvoc);

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
