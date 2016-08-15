#include <GadgetShield.h>
/*
0,0,0 off
0,0,1 blue
0,1,0 green
0,1,1 cyan
1,0,0 red
1,0,1 pink
1,1,0 yellow
1,1,1 white
*/
#define LAMP_LEVEL 8
#define RED 8,0,0
#define GREEN 0,8,0
#define BLUE 0,0,8
#define CYAN 0,8,8
int colorchange;
int colorbase[3];
uint8_t lastpb[2];
int8_t factor = 1;

char buffer[20];
char inbuf[40];
char inIndex = 0;
void setup()
{
  GS.Setup();
  Serial.begin(115200);
}

void loop()
{
  // no interrupt on buttons, so manage a state machine
  for (int j=0; j<2;j++)
  {
    if (GS.IsSwitch(j))
    {
      if (lastpb[j] == 0)
      {
        lastpb[j] = 1;
        Serial.print("PB");
        Serial.println(j);
        switch (j)
        {
          case 0:
          // increment color count
            colorchange++;
            colorchange %= 3;
            Serial.print("CIndex ");
            Serial.println(colorchange);
            break;
          case 1:
            break;
        }
      }
    }
    else
      lastpb[j] = 0;
  }
 // sprintf(buffer,"%d,%d,%d",colorbase[0],colorbase[1],colorbase[2]);
 // Serial.println(buffer);
  // look at potentiometer values
  if (Serial.available())
  {
    int newbyte = Serial.read();
    inbuf[inIndex++] = newbyte;
    if (newbyte == 0x0d)
    {
      inbuf[inIndex] = 0;
      sscanf(inbuf,"%d,%d,%d",&colorbase[0],&colorbase[1],&colorbase[2]);
      inIndex = 0;
    }
  }
 // colorbase[colorchange] = GS.Pot();
  // use value to colour the RGB led
  GS.RGB(colorbase[0],colorbase[1],colorbase[2]);
}


