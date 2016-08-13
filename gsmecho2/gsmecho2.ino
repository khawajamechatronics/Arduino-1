//#define THRU
#ifdef THRU
#include "SoftwareSerial.h"
#else
#include "SIM900.h"
#endif

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.
#define GSM_PIN 8
#ifdef THRU
SoftwareSerial gsm(3,2);
#endif

void setup() 
{
  //Serial connection.
  Serial.begin(115200);
  Serial.println("GSM Shield testing.");
#ifdef THRU
  pinMode(GSM_PIN,OUTPUT); // reset device
  digitalWrite(GSM_PIN,HIGH);
  delay(1200);
  digitalWrite(GSM_PIN,LOW);
  delay(5000);
  gsm.begin(9600);
  Serial.println("GO");
  gsm.print("AT\r");
#else
  if (gsm.begin(9600))
    Serial.println("\nstatus=READY");
  else
    Serial.println("\nstatus=IDLE");
#endif
};

char linebuf[50];
void loop() 
{
  char c;
  while (Serial.available() > 0)
  {
    c = Serial.read();
#ifdef THRU
    gsm.write(c);
#else
    gsm.SimpleWrite(c);
#endif
  }
  delay(10);
  while (gsm.available() > 0)
    Serial.print((char)gsm.read());
};


