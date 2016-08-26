#include "Configuration.h"
#include "Arduino.h"
#include "Remote.h"
#include "wcEEPROM.h"
#include "Relay.h"
#include "SoftReset.h"

extern unsigned TicksPerLiter;
extern unsigned long metercount, prevmetercount,CurrentMeterCount,PreviousMeterCount,REPORT_RATE;
extern char *tempbuf;
extern char *IMEI;  // why ? *IMEI causes problems
extern char cooked[];

#ifdef ANTENNA_BUG
extern bool ignoreMeter;     // set true while sending emails
#endif
char taptemp[10];

void Parse(char * s)
{
  unsigned long secs;
  Serial.print("Parse:");
  Serial.println(s);
  switch(*s)
  {
    case 'b':
    case 'B':
      soft_restart();
      break;
    case 'c':
    case 'C':
      TapChangeState(TAP_CLOSE);
      sprintf(tempbuf,"/WaterControl/RawData.php?action=tapchange&imei=%s&uptime=%lu&tap=tapclose&rawts=%s",IMEI,millis()/1000,RemoteGetClock());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'o':
    case 'O':
      TapChangeState(TAP_OPEN);
      sprintf(tempbuf,"/WaterControl/RawData.php?action=tapchange&imei=%s&uptime=%lu&tap=tapopen&rawts=%s",IMEI,millis()/1000,RemoteGetClock());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'r':
    case 'R':
      PreviousMeterCount = CurrentMeterCount = 0;
      InitEepromData();
#ifdef ANTENNA_BUG
      ignoreMeter = false;
#endif
      sprintf(tempbuf,"/WaterControl/RawData.php?action=reset&imei=%s&uptime=%lu&rawts=%s",IMEI,millis()/1000,RemoteGetClock());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
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
      // this string about 170 long
      sprintf(tempbuf,"/WaterControl/RawData.php?action=ping&imei=%s&uptime=%lu&meterticks=%lu&liters=%lu&tap=%s&rawts=%s",
        IMEI,millis()/1000,metercount,metercount/TicksPerLiter,TapToText(),RemoteGetClock());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'e':
    case 'E':
    // E?
    // En,v   n 0-9 v value
      switch(*++s)
      {
        case '?':
          strcpy(tempbuf,"/WaterControl/eeprom.php?");  // do not do url encode on this bit
          tempbuf[0] = 0;
          sprintf(tempbuf,"%stpl=%s",tempbuf,EEPROMGetIndex(TPL));
          sprintf(tempbuf,"%s&lis=%s",tempbuf,EEPROMGetIndex(LIS));
          sprintf(tempbuf,"%s&ph=%s",tempbuf,EEPROMGetIndex(DP));
          sprintf(tempbuf,"%s&smtps=%s",tempbuf,EEPROMGetIndex(SmS));
          sprintf(tempbuf,"%s&ea=%s",tempbuf,EEPROMGetIndex(EA));
          sprintf(tempbuf,"%s&pword=%s",tempbuf,EEPROMGetIndex(EP));
          sprintf(tempbuf,"%s&rcvr=%s",tempbuf,EEPROMGetIndex(ER));
          sprintf(tempbuf,"%s&apn=%s",tempbuf,EEPROMGetIndex(APN));
          sprintf(tempbuf,"%s&smsflag=%s",tempbuf,EEPROMGetIndex(SENDSMS));
          sprintf(tempbuf,"%s&rr=%s",tempbuf,EEPROMGetIndex(RR));
          sprintf(tempbuf,"%s&pol=%s",tempbuf,EEPROMGetIndex(POL));
          sprintf(tempbuf,"%s&plw=%s",tempbuf,EEPROMGetIndex(PW));
          sprintf(tempbuf,"%s&https=%s",tempbuf,EEPROMGetIndex(HTTPs));
          sprintf(tempbuf,"%s&gpto=%s",tempbuf,EEPROMGetIndex(GPRSTO));
          urlencode(tempbuf,cooked);
          strcpy(tempbuf,"/WaterControl/eeprom.php?");  // do not do url encode on this bit
          strcat(tempbuf,cooked);          
          Serial.println(tempbuf);
          RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
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
        case 'c':
        case 'd':
        case 'e':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x57),s+2);  
          break;
        case 'A': // 'A'->10
        case 'B':
        case 'C':
        case 'D':
        case 'E':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x37),s+2);   
          break;
      }
      break;
  }
}

