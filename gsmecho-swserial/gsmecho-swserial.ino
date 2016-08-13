#include <SoftwareSerial.h>

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

#define GSM_ON              8

SoftwareSerial mySerial(3,2); // RX, TX  SEE GSM.CPP source

void setup() 
{
  //Serial connection.
  Serial.begin(115200);
  Serial.println("GSM Shield testing.");
  mySerial.begin(9600);
  // generate turn on pulse
  digitalWrite(GSM_ON, HIGH);
  delay(1200);
  digitalWrite(GSM_ON, LOW);
  delay(10000);
};

void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

