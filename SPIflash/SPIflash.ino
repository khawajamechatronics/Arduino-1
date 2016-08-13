#include <SPI.h>
const int tp_cs = 2; // touch
const int f_cs = 3;  //flash cs
const int sd_cs = 4; // disk

const byte WHO_AM_I = 0x9f;
const byte DUMMY = 0x00;
// flash id 0x9f
// touch id ads7843
void setup()
{
  pinMode(tp_cs,OUTPUT);
  digitalWrite(tp_cs,HIGH); // deselect
  pinMode(sd_cs,OUTPUT);
  digitalWrite(sd_cs,HIGH); // deselect
  pinMode(f_cs,OUTPUT);
  digitalWrite(f_cs,LOW); // select
  SPI.begin();
  SPI.setBitOrder(MSBFIRST); 
  SPI.setDataMode(SPI_MODE0); 
  Serial.begin(115200);
}

void loop()
{
  SPI.transfer(WHO_AM_I);  // who am i
  unsigned int n = SPI.transfer(DUMMY);
  Serial.println(n);
  n = SPI.transfer(DUMMY);
  Serial.println(n);
  n = SPI.transfer(DUMMY);
  Serial.println(n);
  delay(1000);
}
