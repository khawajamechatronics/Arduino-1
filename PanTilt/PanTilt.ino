/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/ 
/*
   A note on currents
   The UNO has a 500mA polyfuse on the 5V circuit. Assuming the board itself takes around
   50mA that leaves a theoretical 450mA that can be drawn from the 5V pin for servos etc.
   Our small 9g servos take 20mA so no problem running 2 from the 5V pin.
   Larger motors however would justify an outside source.
*/
#include <Servo.h> 
 
Servo panServo,tiltServo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
#define PanMaxAngle 180
#define PanMinAngle 90
// Tilt 0 is table horizontal, camera facing up, 90 facing forward
// Pan 0 is right, 90 middle, 180 left
#define TiltMaxAngle 180
#define TiltMinAngle 90
int potpin = 0;  // analog pin used to connect the potentiometer

int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  // must attach servo to a PWM pin
  panServo.attach(9);  // attaches the servo on pin 9 to the servo object 
  tiltServo.attach(10);
} 
 
void loop() 
{ 
#if 0
  for(pos = PanMinAngle; pos <= PanMaxAngle; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    panServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = PanMaxAngle; pos>=PanMinAngle; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    panServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
#endif
#if 0
  for(pos = TiltMinAngle; pos <= TiltMaxAngle; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    tiltServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = TiltMaxAngle; pos>=TiltMinAngle; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    tiltServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
#endif
#if 0
  pos = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  pos = map(pos, 0, 1023, 0, PanMaxAngle);     // scale it to use it with the servo (value between 0 and 180)
  panServo.write(pos);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there#endif
#endif
} 

