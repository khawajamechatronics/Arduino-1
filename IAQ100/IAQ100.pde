/*
  Software driver using pin 4 for Rx. No Tx necessary
  Device (IAQ-100) works at 9600 baud
  Echo all input so standard Serial port
*/

#define bit9600Delay 84  
#define halfBit9600Delay 42
byte rx = 4;    // pin 4 used for Rx
byte SWval;
byte led;
void setup() {
  pinMode(rx,INPUT);
  led = 1;
  digitalWrite(13,HIGH); //turn on debugging LED
  Serial.begin(9600);
}

int SWread()
{
  byte val = 0;
  while (digitalRead(rx));
  //wait for start bit
  if (digitalRead(rx) == LOW) {
    delayMicroseconds(halfBit9600Delay);
    for (int offset = 0; offset < 8; offset++) {
     delayMicroseconds(bit9600Delay);
     val |= digitalRead(rx) << offset;
    }
    //wait for stop bit + extra
    delayMicroseconds(bit9600Delay); 
    delayMicroseconds(bit9600Delay);
    if (led == 1)
    {
      led = 0;
      digitalWrite(13,LOW); //turn off debugging LED
    }
    else
    {
      led = 1;
      digitalWrite(13,HIGH); //turn on debugging LED
    }
    return val;
  }
}

void loop()
{
    SWval = SWread(); 
    Serial.print(SWval,HEX);
}

