#include "SIM900.h"
#include <SoftwareSerial.h>
//#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;
//#define THRU
#define GSM_ON              8 // connect GSM Module turn ON to pin 77 

int numdata;
char inSerial[40];
int i=0;


void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  if (gsm.begin(9600))
  {
    Serial.println("\nstatus=READY");
    if (inet.begin("uinternet"))
      Serial.println("\nConnected\n");
  }
  else
    Serial.println("\nstatus=IDLE");
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
      Serial1.print((char)inSerial[i]);
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
  while (Serial1.available() > 0)
  {
    Serial.print((char)Serial1.read());
  }
#else
  gsm.SimpleRead();
#endif
}
