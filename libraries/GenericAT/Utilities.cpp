/*
    device specific hardware routines
*/


#include "Arduino.h"
#include "config.h"
void deviceon(int pin)
{
}
void devicereset()
{
#ifdef AT_DEVICE_RESET_PIN
	#ifdef AT_DEVICE_ACTIVE_LOW
		digitalWrite(AT_DEVICE_RESET_PIN,LOW);
		delay(AT_DEVICE_ACTIVE_TIME);
		digitalWrite(AT_DEVICE_RESET_PIN,HIGH);
	#else
		digitalWrite(AT_DEVICE_RESET_PIN,HIGH);
		delay(AT_DEVICE_ACTIVE_TIME);
		digitalWrite(AT_DEVICE_RESET_PIN,LOW);
	#endif
#endif
}
