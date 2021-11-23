#include <Wire.h>

#define EADDRESS 0x50    //Address of 24LC256 eeprom chip
#define STOPBYTE 255
#define MAXITERATIONS 90

void setup(void)
{
  Serial.begin(115200);
  Wire.begin();

  unsigned int address = 0;
  
  EWrite(address, "Frengo");
  Serial.println(ERead(address));
}

void loop() {}

void EWrite(unsigned int startaddress, String data) {

  int len = data.length();

  char *cdata = (char *)calloc(len, sizeof(char));
  data.toCharArray(cdata, len + 1);
  
  for (unsigned int i = startaddress; i < len + startaddress ; i++){
    writeEEPROM(i, cdata[i - startaddress]);
  }
    
    writeEEPROM(len + startaddress, STOPBYTE);
}

String ERead(unsigned int startaddress) {
  char *cdata = (char *) calloc(1, sizeof(char));
  unsigned int i = startaddress;
  
  byte rdata = readEEPROM(i);
  while (rdata != STOPBYTE && i < MAXITERATIONS) {
    cdata = (char *)realloc(cdata, i - startaddress + 2);
    cdata[i - startaddress] = rdata;
    cdata[i - startaddress + 1] = '\0';
    i++;
    
    Serial.println(rdata);
    rdata = readEEPROM(i);
  }

  return String(cdata);
}

void writeEEPROM(unsigned int eeaddress, byte c)
{
  Wire.beginTransmission(EADDRESS);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(c);
  Wire.endTransmission();

  delay(10);
}

byte readEEPROM(unsigned int eeaddress)
{
  byte rdata = 0xFF;

  Wire.beginTransmission(EADDRESS);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(EADDRESS, 1);

  if (Wire.available()) rdata = Wire.read();
  delay(10);
  return rdata;
}
