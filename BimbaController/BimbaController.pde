/*
  This has to mimic Ofirs Fez based controller with joystick
  
  Protocol ABxxyycc   xx,yy signed byte cc checksum
  
  Scale to mimic Fez range from 0xAC max 0x85 level 0x5C min
*/

//#define DEBUG  // undefine for production
#include <GadgetShield.h>

// How often to take accelerometer readings
//#define SAMPLE_RATE 100  // milliseconds
// The time at which to take the next accelerometer reading
//unsigned long sampleTime;
byte packet[5];

void setup() {
  GS.Setup();
  packet[0] ='A';
  packet[1] ='B';
//  sampleTime = millis() + SAMPLE_RATE;
  Serial.begin(9600);
  // Strong filtering to get smooth transitions
//  GS.AccelSetFilterX(252);
//  GS.AccelSetFilterY(252);
//  GS.AccelSetFilterZ(252);
}

#if 0
void Checksum(byte p[])
{
  byte c = 0;
  for (int i=0;i<4;i++)
    c ^= p[i];
  p[4] = c;
}
#endif
void loop() {
  uint8_t x,y,z;
//  if ((long)(millis() - sampleTime) >= 0) {
 //   sampleTime += SAMPLE_RATE;
    // Read the 3 axes
    GS.AccelSample();

    x = GS.AccelResultX();
    y = GS.AccelResultY();
    z = GS.AccelResultZ();
    packet[2] = map(x,11,53,0x5c, 0xac);  // play with these to get correct orientation
    packet[3] = map(y,11,53,0x5c, 0xac);
    packet[4] = 0;
    for (int j=0; j<4; j++)
      packet[4] ^= packet[j];
//    Checksum(packet);
#ifdef DEBUG
    Serial.print((unsigned)x); Serial.print(" ");
    Serial.print((unsigned)y); Serial.print(" ");
    Serial.print((unsigned)packet[2]); Serial.print(" ");
    Serial.print((unsigned)packet[3]); Serial.print(" ");
//    Serial.print((unsigned)z); Serial.print(" ");
    // orientation
//    Serial.print((unsigned)GS.AccelPoLa()); Serial.print(" ");
//    Serial.print((unsigned)GS.AccelBaFro()); Serial.print(" ");
    Serial.println();
#else
    Serial.write(packet,5);
    delay(100);
#endif
//  }
}
