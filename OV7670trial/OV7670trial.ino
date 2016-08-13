
#include <avr/pgmspace.h>
#include <CMOSCamera.h>

/*
 * Current:
 *  OV7670 draws 60mA in normal use and max current allowable from
 *  UNO 3.3 supply is 50mA so we need an external source
 * I2C:
 *   UNO SIOC to A5, SIOD to A4. No external pull-ups used
 * Reset:
 *   Tie PWDN to GND, Reset to 3.3V
 * XCLK:
 *  Arduino PWM cannot supply more than 2MHz so I used an external 25MHz oscillator as XCLK source
 */

#include <CMOSCamera.h>
#include <UTFT.h>

#define VGA_WIDTH 640
#define QVGA_WIDTH (VGA_WIDTH/2)
#define QQVGA_WIDTH (VGA_WIDTH/4)
#define VGA_HEIGHT 480
#define QVGA_HEIGHT (VGA_HEIGHT/2)
#define QQVGA_HEIGHT (VGA_HEIGHT/4)

#define VSYNC_PIN 18  // PE4
#define HSYNC_PIN 19  // PE5
#define PCLK_PIN  20  // PG5

uint16_t qqvgabuf[1/*QQVGA_WIDTH*QQVGA_HEIGHT*/];
CMOSCamera CCC;
UTFT myGLCD(ITDB32S,38,39,40,41);
bool firstloop;
void setup() {
  int x,y;
  uint8_t a,b,c,rc;
  // put your setup code here, to run once:
  Serial.begin(9600);   
  pinMode(VSYNC_PIN, INPUT); 
  pinMode(HSYNC_PIN, INPUT); 
  pinMode(PCLK_PIN, INPUT); 
  CCC.begin();  // also sets up TWI so dont use Wire library
  if (CCC.WhoAmI())
  {
    Serial.println("WHO_AM_I OK");
    CCC.initCam(BAYER_RGB);
 //   CCC.initCam(RGB565);
    CCC.setRes(QVGA);  // set to a small picture 
    CCC.readRegs(REG_COM14,&a,1);
    Serial.println(a,HEX);
//    b = (a & 0xf8);  // pclk divider to 1
//    b = (a & 0xf8) | 1;  // pclk divider to 2
    b = (a & 0xf8) | 2;  // pclk divider to 4
    Serial.print("write ");Serial.println(b,HEX);
    CCC.wrReg(REG_COM14,b);
    CCC.readRegs(REG_COM14,&a,1);
    Serial.println(a,HEX);
  }
  else
    Serial.println("???");
  firstloop = true;
#if 1
// Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setColor(VGA_GREEN);
  for (y=0;y<QVGA_HEIGHT;y++)
  {
    for (x=0;x<QVGA_WIDTH;x++)
    //  myGLCD.drawPixel(x,y);
      myGLCD.setPixel(VGA_RED);
  }
#endif
}
unsigned long vcount = 0;
unsigned long hcount = 0;
#define FPS 30   // VSYNC should be at around 30fpd
#define TEST_TIME 5
void loop() {
  // count PCLK transitions against VSYNC
  if (firstloop)
  {
    while (!(PINE & (1<<4))){}  // wait for PE4 to go high
    firstloop = false;
  }
  while (PINE & (1<<4)){}  // wait for PE4 to go low  
  vcount++;
#if 1
  while (!(PINE & (1<<4))) // while PE4 is low
  {
    if (PINE & (1<<5)){}  // wait for PE5 to go high
    {
      while (PINE & (1<<5)){} // wait for PE5 to go low
      hcount++;
    }
  } 
#endif  
  if (vcount == (FPS*TEST_TIME))
  {
    Serial.print(vcount);  // should appear every TEST_TIME seconds
    Serial.print(",");  // should appear every TEST_TIME seconds
    Serial.println(hcount);  // should appear every TEST_TIME seconds
    vcount = 0;
    hcount = 0;
  }
}


