#include <MANCHESTER.h>

#define TxPin 4  //the digital pin to use to transmit data

unsigned int Tdata = 0;  //the 16 bits to send
unsigned int count = 0;
bool ledon = false;

void setup() 
{                
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  MANCHESTER.SetTxPin(TxPin);      // sets the digital pin as output default 4
}//end of setup

void loop() 
{
 Tdata +=1;
 MANCHESTER.Transmit(Tdata);
 if ((++count % 5) == 0)
 {
   count = 0;
   if (ledon)
    digitalWrite(13, LOW);
   else
     digitalWrite(13, HIGH);
   ledon = !ledon;     
  }
 delay(100);
}//end of loop


