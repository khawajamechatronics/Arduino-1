#include <NewSoftSerial.h>
#include <string.h>

int LedJC2pin = 5;
int LedJC3pin = 6;
int LedGreenpin = 7;
int LedJC2state = true;
int SharpLedpin = 3;
int IAQpin = 4;

NewSoftSerial iaqSerial(4, -1,true);
//NewSoftSerial e2v(8, 9);

char IAQBuffer[128] ;
int IAQIndex = 0;


void setup()  
{
  pinMode(LedJC2pin, OUTPUT);     
  pinMode(LedJC3pin, OUTPUT);     
  pinMode(LedGreenpin, OUTPUT);     
  pinMode(3, OUTPUT);     
  Serial.begin(115200);  
  iaqSerial.begin(9600);

}

void loop()                     // run over and over again
{
  ReadIAQ();
//  Serial.println(IAQBuffer);
}

void ReadIAQ()
{

#if 0
  iaqSerial.flush();
  bool OK = false;
  uint8_t idx = 0;
  char c;
  while (!OK)
  {
    do
    {
      while (iaqSerial.available() == 0) ; // wait for a char this causes the blocking
      c = iaqSerial.read();
      IAQBuffer[idx++] = c;
    }
    while (c != '\n' && c != '\r' && idx<127); 
    IAQBuffer[idx++] = '\0';
    int count = 0;
    for (int i=0; i<idx; i++)
      if (IAQBuffer[i] == ' ')
        count++;
    if (count == 10)
        OK = true;
    else
      delay(100);
  }
#else
  if (iaqSerial.available())
  {
    Serial.print((char)iaqSerial.read());
  }
#endif
}
