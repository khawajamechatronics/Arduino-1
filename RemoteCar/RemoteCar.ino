#include <IRremote.h>
#include "keyes.h"

/*OPEN JUMPER L298 Shield*/
// default values by shorting jumpers
// Motor control
 int INA = 4;  // direction motor A
 int PWMA = 5; 
 int INB = 7;  // direction motor B
 int PWMB = 6; 
 int dir = 0;
 int pwmvalue = 0;
 
// IR control
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
// optical encoder stuff
int encoder_pin = 2; // pulse output from the module
unsigned int rpm; // rpm reading
volatile byte pulses; // number of pulses
unsigned long timeold;
// number of pulses per revolution
// based on your encoder disc
unsigned int pulsesperturn = 20;

void counter()
{
   //Update count
   pulses++;
}

 void setup(){
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(115200);
   pinMode(INA,OUTPUT);
   pinMode(INB,OUTPUT);
   pinMode(encoder_pin, INPUT);
   //Interrupt 0 is digital pin 2
   //Triggers on Falling Edge (change from HIGH to LOW)
   attachInterrupt(0, counter, FALLING);
   // Initialize
   pulses = 0;
   rpm = 0;
   timeold = 0;
 }

void loop() {
   int command;
    if (millis() - timeold >= 1000) {
      //Don't process interrupts during calculations
      detachInterrupt(0);
      rpm = (60 * 1000 / pulsesperturn )/ (millis() - timeold)* pulses;
      timeold = millis();
      pulses = 0;
      Serial.print("RPM = ");
      Serial.println(rpm,DEC);
      //Restart the interrupt processing
      attachInterrupt(0, counter, FALLING);
    }
   // read message from IR (if any)
  if (irrecv.decode(&results))
  {
    command = irdecode(results.value);
 //   Serial.println(command);
    switch (command)
    {
      case onoff:
        pwmvalue = 0;
        dir = 0;
        digitalWrite(INA,HIGH);
        digitalWrite(INB,HIGH);          
        analogWrite(PWMA,pwmvalue);
        analogWrite(PWMB,pwmvalue);
        Serial.println("Stop");
        break;
      case plus: // faster
      case minus: // slower
        if (command == plus)
        {
          Serial.println("faster");
          pwmvalue += 10;
          pwmvalue %= 255;          
        }
        else
        {
          pwmvalue -= 10;
          Serial.println("slower");
          if (pwmvalue < 0)
            pwmvalue = 0;
          pwmvalue %= 255;  
        }
        analogWrite(PWMA,pwmvalue);
        analogWrite(PWMB,pwmvalue);
        break;
      case cd:
        dir = 1-dir; // toggle between 0/1
        if (dir == 0)
        {
          Serial.println("forward");
          digitalWrite(INA,HIGH);
          digitalWrite(INB,HIGH);          
        }
        else
        {
          Serial.println("back");
          digitalWrite(INA,LOW);
          digitalWrite(INB,LOW);      
        }
        break;
    }
    irrecv.resume(); // Receive the next value
  }
  delay (100);
}
