/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-microsd-card-arduino/
  
  This sketch was mofidied from: Examples > SD(esp32) > SD_Test
*/
//include for the microsd slot adapter
#include "SPI.h"

//include for dht11 ans ccs811 sensors
#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <CCS811.h>

//sd slot adapter pins
#define SCK  17
#define MISO  13
#define MOSI  12
#define CS  18

//dht11 data pin
#define DHTPIN 23
#define DHTTYPE DHT11

// initializing the sensors:
DHT dht(DHTPIN, DHTTYPE);
CCS811 sensor;

String class0 = String(0);
String class1 = String(1);
char message[40];

float weights[] = {-0.004163, 0.005974, 005523, 0.005606};
float bias = -0.007646;
float means[] = {54.023747, 25.116755, 1655.704485, 1035.536939};
float devs[] = {12.707672, 4.145223, 1496.357791, 2692.752151};

int led = 2;

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
  pinMode(led, OUTPUT);
}

void humidity_and_temperature(){
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    //return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}

void co2_and_tvoc(){
    if(sensor.checkDataReady() == true){
        Serial.print("CO2: ");
        Serial.print(sensor.getCO2PPM());
        Serial.print("ppm, TVOC: ");
        Serial.print(sensor.getTVOCPPB());
        Serial.println("ppb");
        
    } else {
        Serial.println("Data is not ready!");
    }
    /*!
     * @brief Set baseline
     * @param get from getBaseline.ino
     */
    sensor.writeBaseLine(0x847B);
    //delay cannot be less than measurement cycle
    //delay(1000);
}

float read_humidity(){
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  return dht.readHumidity();
}

float read_temperature(){
  // Read temperature as Celsius (the default)
  return dht.readTemperature();
}

int read_co2(){
  return sensor.getCO2PPM();
}

int read_tvoc(){
  return sensor.getTVOCPPB();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  humidity_and_temperature();
  co2_and_tvoc();
  float humidity = read_humidity();
  float temperature = read_temperature();
  int co2 = read_co2();
  int tvoc = read_tvoc();
  float datas[] = {humidity, temperature, co2, tvoc};
  float score = 0;
  for(int i=0; i<4; i++){
    score += (datas[i] - means[i])/devs[i] * weights[i];
  }
  score += bias;
  Serial.print("Score: ");
  Serial.print(score);
  Serial.print("\n");
  if(score > 0){
    Serial.println("Occhio che c'è un incendio!");
    digitalWrite(led, HIGH);
  }
  else {
    Serial.println("Tutto a posto.");
    digitalWrite(led, LOW);
  }

  
  Serial.print("Score: ");
  Serial.print(score);
  if(score > 0){
    Serial.println("Occhio che c'è un incendio Johnny!");
    digitalWrite(led, HIGH);
  }
  else {
    Serial.println("Tutto a posto Johnny.");
    digitalWrite(led, LOW);
  }
}
