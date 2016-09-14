#include <ATdevice.h>
#include <DeviceInterface.h>
#include <SerialN.h>


/*
 * The rationale behind this project is to create a parameter driven AT command handler that can be 
 * easily adapated to any suitable device e.g. the various GSM modems out there
 * It was developed opposite the GPRS A6 modem
 * 
 * Assumptions:
 *  1. Any serial interface except Serial may be used (Serial1,Serial2, SoftwareSerial etc). Serial is assumed to
 *     be reserved for debugging
 *  2. The serial interface used is defined in atdevice.h
 *  3. Model specific hardware handling e.g. toggling reset pin outside the scope of this library
 *  
 * Classes: ATdevice - the device receiving and reacting to AT commands
 * 
 * Credits: GSMSHIELD library

 
 */
#define A6_PWR_PIN 4  // connect to RST/PWR_KEY 1 bottom row
   // GND 2 bottom row 2 top row
   // VCC 1 top row
   // TX 8 top row
   // RX 9 top row
#define A6_RESET_TIME 5000 // ms

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(A6_PWR_PIN,OUTPUT);
  digitalWrite(A6_PWR_PIN,HIGH);
  delay(A6_RESET_TIME);
  digitalWrite(A6_PWR_PIN,LOW);
  if (/*_ATdevice.begin(9600)*/true)
  {
    Serial.println("Init succeeded");
  }
  else
    Serial.println("Init failed");
    while (true){}
}

void loop() {
  // put your main code here, to run repeatedly:

}
