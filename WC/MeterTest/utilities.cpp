#include "utilities.h"
#include "Arduino.h"

extern enum tapState  lasttap;
void TapChangeState(enum tapState t)
{
  lasttap = t;
}

extern enum rgbstate redled;
extern enum rgbstate blueled;
extern enum rgbstate greenled;
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
      Serial.print("Unknown led :"),Serial.println(led);
      return;
  }
  //DEBUG_SERIAL.print(pin);DEBUG_SERIAL.print(" ");DEBUG_SERIAL.println(onoff);
  if (onoff == LED_ON)
    digitalWrite(pin,HIGH);
  else
    digitalWrite(pin,LOW);
}

char *TapToText()
{
  char *c;
  switch (lasttap)
  {
    case TAP_UNKNOWN:
      c = "unknown";
      break;
    case TAP_OPEN:
      c = "tapopen";
      break;
    case TAP_CLOSE:
      c = "tapclose";
      break;
  }
  return c;
}

unsigned long Sim900ToEpoch()
{
  return millis()*1000L;
}

extern unsigned long CurrentMeterCount[],PreviousMeterCount[];
extern unsigned long StartCountingPeriod;
void ResetAll()
{
  TapChangeState(TAP_OPEN);
  CurrentMeterCount[0] = CurrentMeterCount[1] = 0;
  PreviousMeterCount[1] = PreviousMeterCount[1] = 0;
  StartCountingPeriod = millis()/ONE_SECOND;
}

