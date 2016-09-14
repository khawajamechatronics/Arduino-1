#include <IRremote.h>
#include "remote.h"

/*OPEN JUMPER L298 Shield*/
// default values by shorting jumpers
// Motor control
 int INA = 4;  // direction motor A
 int PWMA = 5; 
 int INB = 7;  // direction motor B
 int PWMB = 6; 
 int dir = 0;
 int pwmvalueleft = 0;
 int pwmvalueright = 0;
 
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
enum eDirection{forward,reverse,left,right};

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

void cardir(enum eDirection d)
{
  switch (d)
  {
    case forward:
      digitalWrite(INA,HIGH);
      digitalWrite(INB,HIGH);          
      break;
    case reverse:
      digitalWrite(INA,LOW);
      digitalWrite(INB,LOW);          
      break;
    case left:
      digitalWrite(INA,HIGH);
      digitalWrite(INB,LOW);          
      break;
    case right:
      digitalWrite(INA,LOW);
      digitalWrite(INB,HIGH);          
      break;    
  }
}
void loop() {
   int command;
   // read message from IR (if any)
  if (irrecv.decode(&results))
  {
    command = irdecode(results.value);
 //   Serial.println(command);
    switch (command)
    {
      case onoff:
        pwmvalueleft = 0;
        pwmvalueright = 0;
        dir = 0;
        digitalWrite(INA,HIGH);
        digitalWrite(INB,HIGH);          
//        analogWrite(PWMA,pwmvalueleft);
//        analogWrite(PWMB,pwmvalueright);
        Serial.println("Stop");
        break;
      case plus: // faster
      case minus: // slower
        if (command == plus)
        {
          Serial.println("faster");
          pwmvalueleft += 10;
          pwmvalueleft %= 255;          
          pwmvalueright += 10;
          pwmvalueright %= 255;          
        }
        else
        {
          pwmvalueleft -= 10;
          pwmvalueright -= 10;
          Serial.println("slower");
          if (pwmvalueleft < 0)
            pwmvalueleft = 0;
          pwmvalueleft %= 255;  
          if (pwmvalueright < 0)
            pwmvalueright = 0;
          pwmvalueright %= 255;  
        }
//        analogWrite(PWMA,pwmvalueleft);
//        analogWrite(PWMB,pwmvalueright);
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
        /*
         *              2 fwd
         *              
         *       left 4 5 6 right
         *       
         *              7 back
         *           
         *           5 stop
         */
       case two:
         cardir(forward);
         Serial.println("forward");
         break;
       case four:
//         cardir(left);
         pwmvalueright = (pwmvalueright * 3)/4;
         Serial.println("left");
         break;
       case six:
//         cardir(right);
         pwmvalueleft = (pwmvalueleft * 3)/4;
         Serial.println("right");
        break;
       case eight:
         cardir(reverse);
        Serial.println("reverse");
         break;
       case ff: // fast forward
         cardir(forward);
         pwmvalueright = pwmvalueleft = 255;
        break;
       case rw: // fast reverse
         cardir(reverse);
         pwmvalueright = pwmvalueleft = 255;
        break;
    }
    irrecv.resume(); // Receive the next value
  }
  delay (100);
  analogWrite(PWMA,pwmvalueleft);
  analogWrite(PWMB,pwmvalueright);
}
