#include <Wire.h>
char address = 0x1e;  // HMC5843
byte command[4];

void GetIdent()
{
  Wire.beginTransmission(address);
  Wire.write(10);  // IDENTA
  Wire.endTransmission();
  Wire.requestFrom(address,3);
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read();    // receive a byte as character
    Serial.print(c);         // print the character
  }
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(address);
  command[0] = 2;  // mode
  command[1] = 0;  // continuous
  Wire.write(command,2);  // Mode
  Wire.endTransmission();
}

void loop()
{
  char buf[10];
  char xy[6];
  int i;
  short angle;
  Serial.println("I2C");
  GetIdent();
  Wire.beginTransmission(address);
  Wire.write(10);  // IDENTA
  Wire.endTransmission();
  Wire.requestFrom(address,3);
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read();    // receive a byte as character
    sprintf(buf,"%02X,",c);
    Serial.print(buf);         // print the character
 //     Serial.print(' ');
  }
  Serial.println();
  //---------------------------------
  Serial.println("Config");
  Wire.beginTransmission(address);
  Wire.write(0);  // CONFIGA
  Wire.endTransmission();
  Wire.requestFrom(address,3);
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read();    // receive a byte as character
    sprintf(buf,"%02X,",c);
    Serial.print(buf);         // print the character
 //     Serial.print(' ');
  }
  Serial.println();
  //-----------------------------
  Serial.println("Data");
  Wire.beginTransmission(address);
  Wire.write(3);  // XMSB
  Wire.endTransmission();
  i = 0;
  Wire.requestFrom(address,6);
  while(Wire.available())    // slave may send less than requested
  {
    char c =  Wire.read();    // receive a byte as character
    sprintf(buf,"%d %02X,",i,c);
    Serial.print(buf);
    xy[i++] = c;
  }
  Serial.println();
 #if 1
 // convert MSB LSB to short
  for (i=0; i<3; i++)
  {
    angle = (xy[i*2]<<8) + xy[(i*2)+1];
    sprintf(buf,"%04X %d,",angle,angle);
    Serial.print(buf);
  }
  Serial.println();
 #endif
  delay(1000);
}
