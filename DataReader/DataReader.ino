#include "oscup.h"
#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <CCS811.h>
#include <stdlib.h>

#define DHTPIN 15
#define DHTTYPE DHT11
#define BUTTON 35

DHT dht(DHTPIN, DHTTYPE);
CCS811 sensor;

uint8_t id = 0x5D;

Oscup oscup = Oscup(id, 115200);
void setup() {
  oscup.begin();
  dht.begin();
  while (sensor.begin() != 0);
  sensor.setMeasCycle(sensor.eCycle_250ms);

  pinMode(BUTTON, INPUT);
}

void loop() {
  char arr[16] = {0};
  
  if (sensor.checkDataReady() == true) {
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    float temperature = dht.computeHeatIndex(temp, humidity, false);
    int co2 = sensor.getCO2PPM();
    int tvoc = sensor.getTVOCPPB();

    char *fbuff = (char *)malloc(sizeof(float));
    float2Bytes(fbuff, temperature);
    for (int i = 0; i < sizeof(float); i++) arr[i] = fbuff[i];
    free(fbuff);
    
    char *fbuff2 = (char *)malloc(sizeof(float));
    float2Bytes(fbuff2, humidity);
    for (int i = sizeof(float); i < sizeof(float) * 2; i++) arr[i] = fbuff2[i - sizeof(float)];
    free(fbuff2);
    
    unsigned char *buff = int_toBytes(co2);
    for (int i = sizeof(float) * 2; i < sizeof(float) * 2 + sizeof(int); i++) arr[i] = buff[i - sizeof(float) * 2];
    free(buff);

    unsigned char *buff2 = int_toBytes(tvoc);
    for (int i = sizeof(float) * 2 + sizeof(int); i < sizeof(float) * 2 + sizeof(int) * 2; i++) arr[i] = buff2[i - sizeof(float) * 2 - sizeof(int)];
    free(buff2);

    uint8_t errore;
    if (digitalRead(BUTTON))
      errore = oscup.write((uint8_t)TxCommands::CONFIRM, 16, arr);
    else
      errore = oscup.write((uint8_t)TxCommands::SHARE, 16, arr);
      
    if (errore)
      errore = oscup.write((uint8_t)TxCommands::SHARE, 1, (char *)&errore);
  }

  sensor.writeBaseLine(0x847B);
  delay(300);
}

void float2Bytes(char bytes_temp[4],float float_variable){ 
  memcpy(bytes_temp, (unsigned char*) (&float_variable), 4);
}

unsigned char *int_toBytes(uint32_t number) {
  size_t dim = sizeof(uint32_t);
  unsigned char *buff = (unsigned char *)calloc(dim, sizeof(unsigned char));
  for (int i = 0 ; i < dim; i++) {
    buff[i] = (number >> (8 * i)) & 0xFF;
  }
  return buff;
}
