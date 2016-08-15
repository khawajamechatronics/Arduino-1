#include <NewSoftSerial.h>
#include <string.h>

int LedJC2pin = 5;
int LedJC3pin = 6;
int LedGreenpin = 7;
int LedJC2state = true;
int SharpLedpin = 3;
int IAQpin = 4;

NewSoftSerial e2v(8, 9);

void setup()  
{
  pinMode(LedJC2pin, OUTPUT);     
  pinMode(LedJC3pin, OUTPUT);     
  pinMode(LedGreenpin, OUTPUT);     
  pinMode(3, OUTPUT);     
  Serial.begin(115200);  
  e2v.begin(19200);
}

void loop()                     // run over and over again
{
  if (e2v.available()) 
    Serial.print((char)e2v.read());
  if (Serial.available())
      e2v.print((char)Serial.read());    
}


