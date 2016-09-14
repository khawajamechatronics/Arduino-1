#include <SoftwareSerial.h>

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.


// GPRS a6 PWR_KEY 1 LOWER, RST 7 LOWER
#define TRANSISTOR_BASE  7  // RST
#define START_BAUD 115200
#define FINAL_BAUD 9600
SoftwareSerial mySerial(2,3); // RX must be an interrupt pin TX

void hwreset()
{
  Serial.println("\r\nReset");
  digitalWrite(TRANSISTOR_BASE,HIGH);
  delay(10);
  digitalWrite(TRANSISTOR_BASE,LOW);  
}
void setup() 
{
  //Serial connection.
  Serial.begin(115200);
  Serial.println("Start");
  mySerial.begin(START_BAUD);
  pinMode(TRANSISTOR_BASE,OUTPUT);
  digitalWrite(TRANSISTOR_BASE,LOW);
  hwreset();
};

int brate = 0;
void loop() { // run over and over
  char c;
  if (brate == 0 && millis() > 15000)
  {
    brate = 1;
    mySerial.print("AT+IPR=");
    mySerial.println(FINAL_BAUD);
    mySerial.end();
    mySerial.begin(FINAL_BAUD);
    hwreset();
  }
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    c = Serial.read();
    if (c == ']')
      hwreset();
    else
      mySerial.write(c);
  }
}
/*
 * AT+IPR?
 * AT+GSN
 */
