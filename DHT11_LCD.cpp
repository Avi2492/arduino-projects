#include <LiquidCrystal.h>
LiquidCrystal Lcd(12, 11, 3, 4, 5, 6);
#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTPIN 2    
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
void setup() 
{
 Lcd.begin(16, 2);
 dht.begin();
 Lcd.print("DHT11 TEST!");
}
void loop() 
{
  delay(2000);  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Lcd.setCursor(0, 1);
  Lcd.print("Humidity: \t" );
  Lcd.print(h);
  Lcd.print(" %\n");
  delay(500);
  Lcd.clear();
  Lcd.print("Temperature: \t");
  Lcd.print(t);
  Lcd.print(" *C \n");
  Lcd.clear();
}
