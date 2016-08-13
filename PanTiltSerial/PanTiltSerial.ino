/*
 *   control pan/tilt from serial line
 */
#include <Servo.h> 
 
Servo panServo,tiltServo;  // create servo object to control a servo 
#define PanPin 9    // must be a PWM pin
#define TiltPin 10

String s;
char xxx[50];

unsigned device,angle;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(60000); // allow a minute to enter a value
  panServo.attach(PanPin);  // attaches the servo on pin 9 to the servo object 
  tiltServo.attach(TiltPin);
  memset(xxx,'c',50);
}

void getline()
{
  char c;
  s = "";
  while (true) // exit on cr, ignore lf
  {
    if (Serial.available()> 0)
    {
      c = Serial.read();
   //   Serial.println(c);
      if (c != 0x0a)
      {
        s += c;
      }
      if (c == 0x0d)
      {
        return;
      }
    }
  }
}
void loop() {
  int n,m;
  // get a xxx and execute it
  Serial.println("Enter n,m where");
  Serial.println(" n is 0 for pan, 1 for tilt, m is angle 0-360");
  getline();
//  Serial.println(s);
  s.toCharArray(xxx,s.length());
  // decode xxx and check for legality
  if ((n = sscanf(xxx,"%u,%u",&device,&angle)) == 2)
  {
    Serial.print("device ");Serial.print(device);Serial.print(" angle ");Serial.println(angle);
    if (device <= 1 && angle <=360)
    {
      if (device == 0)
        panServo.write(angle);
      else
        tiltServo.write(angle);
    }
    else
    {
      Serial.println("Illegal values");
    }
  }
  else
  {
    Serial.print(n);Serial.println(" fields");
    Serial.println("Wrong format");
  }
}
