#include <Arduino.h>
#include "EEPROM.h"
#include "WCmap.h"

int WCmapWrite(unsigned offset,unsigned buflength,char *s)
{
    const byte* p = (const byte*)s;
    unsigned int i;
    for (i = 0; *p && i < buflength; i++)  // stop on null byte
          EEPROM.write(offset++, *p++);
    EEPROM.write(offset, 0);  // add null byte
    return i;
}
int WCmapWrite(unsigned offset,long n)
{
    byte* p = (byte*)&n;
    unsigned int i;
    for (i = 0; i < sizeof(long); i++)
          EEPROM.write(offset++, *p++);
    return i;  
}
int WCmapRead(unsigned offset,unsigned buflength,char *s)
{
  byte* p = (byte*)s;
  byte temp = 0xff;
    unsigned int i;
    for (i = 0; temp && i < buflength; i++) // stop on null byte
    {
      temp = EEPROM.read(offset++);
      *p++ = temp;
    }
    return i;
}
int WCmapRead(unsigned offset,long *n)
{
  byte* p = (byte*)n;
    unsigned int i;
    for (i = 0; i < sizeof(long); i++)
          *p++ = EEPROM.read(offset++);
    return i;
}

void WCeepromInit()
{
  // write zero length strings or default long values
  EEPROM.write(iFriendlyName,0);
  EEPROM.write(iSubscriberList,0);
  EEPROM.write(iKeepAlive,0);
  EEPROM.write(iAlarmIndex,0xff);
  EEPROM.write(iFriendlyName,0);
  EEPROM.write(iFriendlyName,0);
  EEPROM.write(iFriendlyName,0);
  EEPROM.write(iLeakElapsedTime,30);
  EEPROM.write(iAPN,0);
  EEPROM.write(iEmail,0);
}

