#include <ExtEEPROM.h>
#include "Wire.h"
ExtEEPROM ee = ExtEEPROM();
void setup(void)
{
  Serial.begin(115200);
  pinMode(26,OUTPUT);
  digitalWrite(26,HIGH);

  unsigned int address = 4000;

  ee.begin();
  delay(100);
  ee.EWrite("Frengo0000000041dcadbzgjjfhxkt7e5yehzseqg");
  delay(300);
  Serial.println(String(ee.ERead()));

  /*
   * Wire.begin();
    delay(100);

    for (int i = 0; i < 300; i++) {
    writeEEPROM(i, 76);
    delay(50);
    Serial.print("Writing position: ");
    Serial.println(i);
    }
    delay(2000);
    for (int i = 0; i < 300; i++) {
    char c = (char)(readEEPROM(i));
    Serial.println(c);
    delay(50);
    }*/
}
/*
  void writeEEPROM(unsigned int eeaddress, uint8_t c)
  {
  Wire.beginTransmission(0x50);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(c);
  Wire.endTransmission();

  }

  uint8_t readEEPROM(unsigned int eeaddress)
  {
  uint8_t rdata = (uint8_t) 0xFF;

  Wire.beginTransmission(0x50);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(0x50, 1);

  if (Wire.available()) rdata = Wire.read();
  return rdata;
  }
*/
void loop() {}
