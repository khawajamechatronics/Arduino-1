#include <Wire.h>
#include "MPL3115A2.h" //Pressure sensor
MPL3115A2 myPressure; //Create an instance of the pressure sensor
byte address = 0x60;
byte who_am_i = 0x0c;
const int SENSORADDRESS = 0x60; // address specific to the MPL3115A1, value found in datasheet
float pressure = 0;
byte id;
void setup()
{
  Serial.begin(9600);
  myPressure.begin(); // Get sensor online
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags 
  id = myPressure.whoami();
}
byte reg = 0;
char buffer[20];
void loop()
{
  delay(1000);
  //Calc pressure
  pressure = myPressure.readPressure();
  Serial.println(id,HEX);
  Serial.println(pressure,2);
}


