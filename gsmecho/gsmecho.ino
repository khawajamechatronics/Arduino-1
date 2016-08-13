#define SWS
#define THRU

#ifdef SWS
#include <SoftwareSerial.h>
#else
#include "SIM900.h"
#endif
#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

CallGSM call;
#define GSM_ON              8 
#ifdef THRU
SoftwareSerial mySerial(3,2); // RX, TX  SEE GSM.CPP source
#endif

#ifdef SWS
#define OTHER_SERIAL mySerial
#else
#define OTHER_SERIAL Serial1
#endif

int numdata;
char inSerial[40];
int i=0;
char *mynumber = "0545919886";

void setup() 
{
  //Serial connection.
  Serial.begin(115200);
  Serial.println("GSM Shield testing.");
#ifdef THRU
  OTHER_SERIAL.begin(9600);
#else
if (gsm.begin(9600))
{
    Serial.println("\nstatus=READY");
    call.Call(mynumber);
}
  else
    Serial.println("\nstatus=IDLE");
#endif
};

void loop() 
{
  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
  serialhwread();
  //Read for new byte on NewSoftSerial.
  serialswread();
};

void serialhwread(){
  i=0;
  if (Serial.available() > 0)
  {            
    while (Serial.available() > 0)
    {
      inSerial[i]=(Serial.read());
#ifdef THRU
      Serial.print((char)inSerial[i]);
      OTHER_SERIAL.print((char)inSerial[i]);
#endif
      delay(10);
      i++;      
    }
#ifndef THRU
    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END"))
    {
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWriteln(inSerial);
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST"))
    {
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    } 
    else
    {
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }    
    inSerial[0]='\0';
#endif
  }
}

void serialswread(){
#ifdef THRU
  while (OTHER_SERIAL.available() > 0)
  {
    Serial.print((char)OTHER_SERIAL.read());
  }
#else
  gsm.SimpleRead();
#endif
}
