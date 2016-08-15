int SharpLedpin = 3;
char SharpBuffer[64];
int delayTime=280;
int delayTime2=40;
float offTime=9680;

#define SHARP_LOOPS 10  // number of times to sample dust// read 10 times with 100millsec delay, report min, max, average values
void SharpInit()
{
  Serial.println("Sharp init");
  pinMode(SharpLedpin, OUTPUT);     
}

void SharpRead()
{
  int n,readings[SHARP_LOOPS],sum;
  SharpOK = true;
  for (n = 0; n < SHARP_LOOPS; n++)
  {
    delay(100);
      // ledPower is any digital pin on the arduino connected to Pin 3 on the sensor
    digitalWrite(SharpLedpin,LOW); // power on the LED
    delayMicroseconds(delayTime);
    readings[n] = analogRead(SharpLedpin); // read the dust value via pin 3 on the sensor
    delayMicroseconds(delayTime2);
    digitalWrite(SharpLedpin,HIGH); // turn the LED off
    delayMicroseconds(offTime);
    delay(100);
  }
 SharpBuffer[0] = 0;
 for (n=0; n<SHARP_LOOPS; n++)
 {
 //  char temp[10];
   sprintf(pbuff,"%d,",readings[n]);
   strcat(SharpBuffer,pbuff);
 }
 Serial.println(SharpBuffer);
}

