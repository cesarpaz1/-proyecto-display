//
//    FILE: dht11_test.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: DHT library test sketch for DHT11 && Arduino
//     URL: https://github.com/RobTillaart/DHTstable



#include "DHTStable.h"
#include <LiquidCrystal.h>

DHTStable DHT;

#define DHT11_PIN       22
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHTSTABLE_LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}


void loop()
{
  // READ DATA
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
      Serial.print("OK,\t"); 
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      Serial.print("Checksum error,\t"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      Serial.print("Time out error,\t"); 
      break;
    default: 
      Serial.print("Unknown error,\t"); 
      break;
  }
  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  lcd.print(DHT.getTemperature());
  // DISPLAY DATA
  //Serial.print(DHT.getHumidity(), 1);
  //Serial.print(",\t");
  lcd.setCursor(0, 1);
  //Serial.println(DHT.getTemperature(), 1);
  lcd.print(DHT.getHumidity());

 if (DHT.getTemperature() > 29) {
    Serial.println("LED_RED");
 }else if (DHT.getTemperature() < 26) {
    Serial.println("LED_BLUE");
 }else{
    Serial.println("LED_GREEN");
 }
  delay(2000);
}


// -- END OF FILE --
