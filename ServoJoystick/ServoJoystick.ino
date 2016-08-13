/*
 * Control pan tilt servos from the Funduino joystick
 */

#include <Servo.h> 
int panPin = 9;   // Servos attach to PWM pins
int tiltPin = 10;

Servo panServo,tiltServo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
#define PanMaxAngle 0    // pan right
#define PanMinAngle 180  // pan left
#define PanRange (PanMaxAngle-PanMinAngle)
// Tilt 0 is table horizontal, camera facing up, 90 facing forward
// Pan 0 is right, 90 middle, 180 left
#define TiltMaxAngle 180  // up
#define TiltMinAngle 90   // down
#define TiltRange (TiltMaxAngle-TiltMinAngle)

#define MinAmplitude -100
#define MaxAmplitude 100
#define AmplitudeRange (MaxAmplitude-MinAmplitude)

#define PanFactor ((float)PanRange / (float)AmplitudeRange)
#define TiltFactor ((float)TiltRange / (float)AmplitudeRange)

#include <JoystickShield.h> // include JoystickShield Library
// standard joystick shield uses pins 0-8
JoystickShield joystickShield; // create an instance of JoystickShield object

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // must attach servo to a PWM pin
  panServo.attach(panPin);  // attaches the servo on pin 9 to the servo object 
  tiltServo.attach(tiltPin);
  delay(100);
  // new calibration function
  joystickShield.calibrateJoystick();
    
  // predefined Joystick to Pins 0 and 1.
  // Change it if you are using a different shield
  // setJoystickPins(0, 1);
  
  // predefined buttons to the following pins.
  // change it if you are using a different shield.
  // setButtonPins(pinJoystickButton, pinUp, pinRight, pinDown, pinLeft, pinF, pinE);
  // to deactivate a button use a pin outside of the range of the arduino e.g. 255, but not above
  // setButtonPins(8, 2, 3, 4, 5, 7, 6);
}

/*
 *   Joystick amplitude goes from -100 to + 100 which has to be mapped to
 *   PanTilt max/min angles
 *   Integer arithmetic will introduce errors
 */
int xAmpToXangle(int amplitude)
{
  return PanMinAngle +((amplitude-MinAmplitude)*PanFactor );
}

int yAmpToYangle(int amplitude)
{
  return TiltMinAngle +((amplitude-MinAmplitude)*TiltFactor );
}

void loop() {
  int pos;
  // put your main code here, to run repeatedly:
  joystickShield.processEvents(); // process events
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
#if 1
  Serial.print("Xamp ");Serial.print(joystickShield.xAmplitude());
  Serial.print(" Xang ");
  pos = xAmpToXangle(joystickShield.xAmplitude());
  Serial.println(pos);
  panServo.write(pos);  
  pos = yAmpToYangle(joystickShield.yAmplitude());
  tiltServo.write(pos);  
  delay(100);
#endif
}
