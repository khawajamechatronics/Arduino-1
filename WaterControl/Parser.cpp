#include "Configuration.h"
#include "Arduino.h"
#include "Remote.h"
#include "wcEEPROM.h"
#include "Relay.h"

extern unsigned TicksPerLiter;
extern unsigned long metercount, prevmetercount,CurrentMeterCount,PreviousMeterCount,REPORT_RATE;
extern char *tempbuf;
#ifdef ANTENNA_BUG
extern bool ignoreMeter;     // set true while sending emails
#endif

void Parse(char * s)
{
  unsigned long secs;
  Serial.println(s);
  switch(*s)
  {
    case 'c':
    case 'C':
      //TapClose();
      TapChangeState(TAP_CLOSE);
      sprintf(tempbuf,"%s: Tap closed",RemoteGetClock());
      RemoteSendMessage(tempbuf);
      break;
    case 'o':
    case 'O':
      //TapOpen();
      TapChangeState(TAP_OPEN);
      sprintf(tempbuf,"%s: Tap opened",RemoteGetClock());
      RemoteSendMessage(tempbuf);
      break;
    case 'r':
    case 'R':
      PreviousMeterCount = CurrentMeterCount = 0;
      InitEepromData();
      //TapOpen();
      TapChangeState(TAP_OPEN);
#ifdef ANTENNA_BUG
      ignoreMeter = false;
#endif
      sprintf(tempbuf,"%s: Reset",RemoteGetClock());
      RemoteSendMessage(tempbuf);
      break;
      /*
       * unsigned long secs = millis()/1000;
  secs *= 1000;  // do a day in 86.4 secs
  unsigned long days = secs / (24*3600L);
  unsigned long hours = (secs / 3600)%24;
  unsigned long mins = (secs/60)%60;
  unsigned long s = secs%60;
       */
    case 'p':
    case 'P':
      secs = millis()/1000;
      sprintf(tempbuf,"%s: Tap %s Liters %u RR %lu Uptime Days %lu %02lu:%02lu:%02lu",
        RemoteGetClock(),
        lasttap == TAP_OPEN ? "open" : "closed",
        metercount/TicksPerLiter,
        REPORT_RATE,
        secs/86400L,(secs/3600)%24, (secs/60)%60, secs%60);
      if (RemoteSendMessage(tempbuf))
        Serial.println("SMS sent OK");
      else
        Serial.println("SMS not sent");
      break;
    case 'e':
    case 'E':
    // E?
    // En,v   n 0-9 v value
      switch(*++s)
      {
        case '?':
          sprintf(tempbuf,"TPL %s",EEPROMGetIndex(TPL));
          strcat(tempbuf,",LIS ");
          strcat(tempbuf,EEPROMGetIndex(LIS));
          strcat(tempbuf,",PH ");
          strcat(tempbuf,EEPROMGetIndex(DP));
          strcat(tempbuf,",Srv ");
          strcat(tempbuf,EEPROMGetIndex(SmS));
          strcat(tempbuf,",UA ");
          strcat(tempbuf,EEPROMGetIndex(EA));
          strcat(tempbuf,",PW ");
          strcat(tempbuf,EEPROMGetIndex(EP));
          strcat(tempbuf,",RCV ");
          strcat(tempbuf,EEPROMGetIndex(ER));
          strcat(tempbuf,",APN ");
          strcat(tempbuf,EEPROMGetIndex(APN));
          strcat(tempbuf,",SMS ");
          strcat(tempbuf,EEPROMGetIndex(SENDSMS));
          strcat(tempbuf,",RR ");
          strcat(tempbuf,EEPROMGetIndex(RR));
          strcat(tempbuf,",POL ");
          strcat(tempbuf,EEPROMGetIndex(POL));
          strcat(tempbuf,",PW ");
          strcat(tempbuf,EEPROMGetIndex(PW));
          RemoteSendMessage(tempbuf);
          break;
        case '0': // '0' -> 0
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          EEPROMSetIndex((eEEPROMIndex)(*s-'0'),s+2);   
          break;
        case 'a': // 'a'->10
        case 'b':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x57),s+2);  
          break;
        case 'A': // 'A'->10
        case 'B':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x37),s+2);   
          break;
      }
      break;
  }
}

