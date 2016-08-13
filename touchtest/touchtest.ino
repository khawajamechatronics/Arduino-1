#include <UTFT.h>
#include <UTouch.h>

UTFT    myGLCD(ITDB32S,38,39,40,41);
UTouch  myTouch( 6, 5, 4, 3, 2);
// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t vga8x12[];
extern uint8_t vga16x16[];
//extern uint8_t hebrewlarge[];
int YSize,YFontSize,YTopLine;
char linebuf[17] = {0};
char xchar = 0x20;
int linenumber = 0;
void setup()
{
  int j;
  Serial.begin(9600);
  Serial.println("UTouch test");
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.setFont(vga16x16);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.setColor(VGA_LIME);
  myGLCD.clrScr();
  myTouch.InitTouch(LANDSCAPE);
  YSize = myGLCD.getDisplayYSize();
  YFontSize = myGLCD.getFontYsize();
  YTopLine = (YSize - YFontSize)/2;
#if 1
for (j=0;j<6;j++)
  {
    // fill the line buffer
    int k;
    for (k=0;k<16;k++)
      linebuf[k]=xchar++;
    myGLCD.print(linebuf,LEFT,linenumber++ * YFontSize);
  }
#else
  myGLCD.print("Touch Test",CENTER,YTopLine);
#endif
}
void loop()
{
  char buffer[30];
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    sprintf(buffer,"X %d Y %d",myTouch.getX(),myTouch.getY());
    Serial.println(buffer);
    myGLCD.clrScr();
    myGLCD.print(buffer,CENTER,YTopLine);
  }
}
