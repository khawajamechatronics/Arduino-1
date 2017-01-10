/*
 *   Following functions are implemented on behalf of the main class
 *   A6 baud rate always 115200, commands may be in lower case
 *   After reset modem does a set of CINIT CTZV and CIEV commands - all undocumented
 *   Finally should see +CREG: 1
 *   Allow > 12 secs for reset
 *   DEBUG_SERIAL and HW_SERIAL defined in A^Modem.h
 */
#include <Arduino.h>
#include "A6Modem.h"

#define TRANSISTOR_CONTROL 7  // connect to base of transistor
#define A6_RESET_TIME 50 // ms

void GPRSA6Device::HWReset()
{
  // put your setup code here, to run once:
  pinMode(TRANSISTOR_CONTROL,OUTPUT);
  digitalWrite(TRANSISTOR_CONTROL,LOW);
  digitalWrite(TRANSISTOR_CONTROL,HIGH);
  delay(A6_RESET_TIME);
  digitalWrite(TRANSISTOR_CONTROL,LOW);  
}

#if 1
void GPRSA6Device::DebugWrite(int c)
{
  if (enableDebug)
    Serial.print(c);
}
void GPRSA6Device::DebugWrite(char c)
{
  if (enableDebug)
    Serial.write(c);
}
void GPRSA6Device::DebugWrite(char *s)
{
  if (enableDebug)
    Serial.print(s);
}
#endif

