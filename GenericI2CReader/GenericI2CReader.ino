// read from HMC5883L

#include <Wire.h>
byte x[10];
char string[10];
int i = 0;
byte commands[1];
int main()
{
  int j;
  char address =  0x1e;  // HMC5843
  int maxreg = 13;
  PORTC = 0b00110000; // Use the internal pull up resistors P4 P5
  Serial.begin(11500);
  Wire.begin();
 // Wire.onRequest(rcvHandler);
 // delay(10);
  Serial.println("hello I2C");
  // read ID regs
  Wire.beginTransmission(address);  // HMC5883L
  Serial.println("1");
  commands[0] = 0;
  Wire.write(commands,1);  // register 0
  Serial.println("2");
  i = Wire.endTransmission();
  Serial.println("3");
  Serial.print("endt ");
  string[0] = i+ '0';
  string[1] = 0;
//  serial.println(string);
  Serial.println(); 
  Wire.beginTransmission(address);
  Wire.requestFrom(address,maxreg);
  while (i<maxreg)
 {
   char c = Wire.read();
   Serial.println(c);
 }
  return 1;
}

void rcvHandler()
{
  Serial.println('x');
}

