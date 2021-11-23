#include <ExtEEPROM.h>
#include "Wire.h"
ExtEEPROM ee = ExtEEPROM();
void setup(void)
{
  Serial.begin(115200);

  unsigned int address = 0;
  ee.begin();
  ee.EWrite(address, "Frengo");
  delay(4);
  Serial.println(String(ee.ERead(address)));
}

void loop() {}
