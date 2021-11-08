#include "SPI.h"

#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <CCS811.h>

//sd slot adapter pins
#define SCK  17
#define MISO  13
#define MOSI  12
#define CS  18

#define DHTPIN 23
#define DHTTYPE DHT11
#define LED 5

DHT dht(DHTPIN, DHTTYPE);
CCS811 sensor;

String class0 = String(0);
String class1 = String(1);
char message[40];

float weights[] = {-0.004163, 0.005974, 005523, 0.005606};
float bias = -0.007646;
float means[] = {54.023747, 25.116755, 1655.704485, 1035.536939};
float devs[] = {12.707672, 4.145223, 1496.357791, 2692.752151};


void setup(){
  Serial.begin(115200);
  SPIClass spi = SPIClass(HSPI);
  spi.begin(SCK, MISO, MOSI, CS);

  while(sensor.begin() != 0){
        Serial.println("failed to init chip, please check if the chip connection is fine");
        delay(1000);
  }

  dht.begin();
  sensor.setMeasCycle(sensor.eCycle_250ms);
  pinMode(LED, OUTPUT);
}

void humidity_and_temperature(float *h, float *t){
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

void co2_and_tvoc(int *co2, int *tvoc){
    if(sensor.checkDataReady() == true){
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
    /*!
     * @brief Set baseline
     * @param get from getBaseline.ino
     */
    sensor.writeBaseLine(0x847B);
}

void loop() {
  delay(2000);
  float humidity,temperature ; 
  int co2, tvoc;
  humidity_and_temperature(&humidity, &temperature);
  co2_and_tvoc(&co2, &tvoc);
  float datas[] = {humidity, temperature, co2, tvoc};
  float score = 0;
  for(int i=0; i<4; i++){
    score += (datas[i] - means[i])/devs[i] * weights[i];
  }
  score += bias;
  Serial.print("Score: ");
  Serial.print(score);
  
  if(score > 0){
    Serial.println("\nFire detected!");
    digitalWrite(LED, HIGH);
  }
  else {
    Serial.println("\nNothing");
    digitalWrite(LED, LOW);
  }
  Serial.print("\n");
}
