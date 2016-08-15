/*
  Single motor and ROB-11670 breakout board
  Also ran 2 motors on ROB-11669 4-way board, works but not sure 
  if OK to power all through Arduino
  WARNING check flat cables pressed in correct direction
  Power 3 - 12 V
  pins 2,3 CHA,CHB encoder, must be interrupt pins
    wheel has 32 black & 32 white markers
  pins 8,9 In1, In2
  pin 10 PWM
  
  Command Language
  [N][C][V]
  N motor number 0-3
  C Command
    B brake mode Value 0,1
    D set direction Value 0,1
    d get direction
  S Stop
  P Set power Value -255 to +255
  p Get power
*/
#include <HUBeeBMDWheel.h>
#include <Encoder.h>

HUBeeBMDWheel myWheel(8,9,10);
Encoder myEnc(2,3);
int Direction = 0;
long oldPosition  = -999;
int loops = 0;
byte buffer[50];  // more than enough for a command
int bufferindex = 0;

void setup()
{
  Serial.begin(9600);
  myWheel.setDirectionMode(Direction);
  myWheel.setMotorPower(50);
  Serial.println("Hello Wheel");
}

void CommandParse()
{
  int motor = buffer[0] - '0';
  Serial.print(motor);
  Serial.print(',');
  byte command = buffer[1];
  Serial.print(command);
  Serial.print(',');
  int value = 0;
  int index = 2;
  int sign = 1;
  if (buffer[index] == '-')
  {
    index = 3;
    sign = -1;
  }
  while (buffer[index] != 0x0a)
  {
    value = (value*10) + (buffer[index] - '0');
    index++;
  }
  if (value != 0)
    value = value * sign;
  Serial.println(value);
  //sprintf(pbuf,"Motor %d Command %c Arg %d",motor,command,value);
  //Serial.println(pbuf);
  switch(command)
  {
    case 'B':
      myWheel.setBrakeMode(value == 0 ? false : true);
      break;
    case 'D':
      myWheel.setDirectionMode(value == 0? 0 : 1);
      break;
    case 'P':
      myWheel.setMotorPower(value);
      break;
    case 'S':
      myWheel.stopMotor();
      break;
    default:
      Serial.print("Unknown ");
      Serial.println(command);
      break;    
  }
}
void loop()
{
#if 1
  long newPosition = myEnc.read();
  if (newPosition != oldPosition)
  {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
#endif
  // wait for a line of command then decode it
  if (Serial.available())
  {
    buffer[bufferindex] = Serial.read();
    if (buffer[bufferindex] == 0x0a)
    {
      // complete command, process it
      bufferindex = 0;
      CommandParse();
    }
    else
      bufferindex++;
  }
}

