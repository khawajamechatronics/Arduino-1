#include "Arduino.h"
#include "Configuration.h"
#include "RGB.h"
#ifndef DEBUG_SERIAL
#include <SendOnlySoftwareSerial.h>
extern SendOnlySoftwareSerial DEBUG_SERIAL;
#endif


enum rgbstate redled;
enum rgbstate blueled;
enum rgbstate greenled;

void RGBActivate(enum rgbled led ,enum rgbstate onoff)
{
  int pin;
  //DEBUG_SERIAL.print(led);DEBUG_SERIAL.print(" ");DEBUG_SERIAL.println(onoff);
  switch(led)
  {
    case RED_LED:
      pin = RED_LED_PIN;
      redled = onoff;
      break;
    case BLUE_LED:
      pin = BLUE_LED_PIN;
      blueled = onoff;
      break;
    case GREEN_LED:
      pin = GREEN_LED_PIN;
      greenled = onoff;
      break;
    default:
      DEBUG_SERIAL.print("Unknown led :"),DEBUG_SERIAL.println(led);
      return;
  }
  //DEBUG_SERIAL.print(pin);DEBUG_SERIAL.print(" ");DEBUG_SERIAL.println(onoff);
  if (onoff == LED_ON)
    digitalWrite(pin,HIGH);
  else
    digitalWrite(pin,LOW);
}
void  RGBToggle(enum rgbled led)
{
  int pin;
  enum rgbstate newstate;
  switch(led)
  {
    case RED_LED:
      newstate = redled == LED_ON ? LED_OFF : LED_ON;
      break;
    case BLUE_LED:
      newstate = blueled == LED_ON ? LED_OFF : LED_ON;
      break;
    case GREEN_LED:
      newstate = greenled == LED_ON ? LED_OFF : LED_ON;
      break;
    default:
      return;
  }
  RGBActivate(led,newstate);
}

void RGBInit()
{
  pinMode(RED_LED_PIN,OUTPUT);
  RGBActivate(RED_LED,LED_ON);
  RGBToggle(RED_LED);
  pinMode(BLUE_LED_PIN,OUTPUT);
  RGBActivate(BLUE_LED,LED_ON);
  RGBToggle(BLUE_LED);
  pinMode(GREEN_LED_PIN,OUTPUT);
  RGBActivate(GREEN_LED,LED_ON);
  RGBToggle(GREEN_LED);
}

