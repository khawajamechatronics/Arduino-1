#include "Configuration.h"
#include "Arduino.h"
#include "Remote.h"
#include "wcEEPROM.h"
#include "Relay.h"
#include "SoftReset.h"
#include "Sim900.h"

//extern unsigned tpl[]
extern unsigned long metercount[], prevmetercount[],CurrentMeterCount[],PreviousMeterCount[],REPORT_RATE;
extern char tempbuf[];
extern char *IMEI;  // why ? *IMEI causes problems
extern char cooked[];

#ifdef ANTENNA_BUG
extern bool ignoreMeter;     // set true while sending emails
#endif
char taptemp[10];

// EEPROM message long so send in parts
void SendLongMessage ()
{
//  Serial.println(tempbuf);
  urlencode(tempbuf,cooked);
  strcpy(tempbuf,EEPROMGetIndex(WWWPATH));
  strcat(tempbuf,"/eeprom.php?");  // do not do url encode on this bit
  strcat(tempbuf,cooked);          
//  Serial.println(tempbuf);
  RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
}

void Parse(char * s)
{
  unsigned long secs;
  char tpltemp[10];
  Serial.print("Parse:");
  Serial.println(s);
 // Serial.println(gsm.GetClock());
  time_t ep = Sim900ToEpoch(gsm.GetClock());
 // Serial.println(ep);
  switch(*s)
  {
    case 'b':
    case 'B':
      // soft_restart();
      digitalWrite(RESET_TRIGGER,LOW);
      break;
    case 'c':
    case 'C':
      TapChangeState(TAP_CLOSE);
      sprintf(tempbuf,"%s/RawData.php?AC=tapchange&imei=%s&UT=%lu&tap=tapclose&epoch=%lu&m0=%lu&m1=%lu",EEPROMGetIndex(WWWPATH),
        IMEI,millis()/1000,ep,metercount[0],metercount[1]);
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'o':
    case 'O':
      TapChangeState(TAP_OPEN);
      sprintf(tempbuf,"%s/RawData.php?AC=tapchange&imei=%s&UT=%lu&tap=tapopen&epoch=%lu&m0=%lu&m1=%lu",EEPROMGetIndex(WWWPATH),
        IMEI,millis()/1000,ep,metercount[0],metercount[1]);
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'r':
    case 'R':
      PreviousMeterCount[0] = CurrentMeterCount[0] = PreviousMeterCount[1] = CurrentMeterCount[1]= 0;
      InitEepromData();
#ifdef ANTENNA_BUG
      ignoreMeter = false;
#endif
      sprintf(tempbuf,"%s/RawData.php?AC=reset&imei=%s&UT=%lu&epoch=%lu",EEPROMGetIndex(WWWPATH),IMEI,millis()/1000,ep);
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
      sprintf(tempbuf,"%s/RawData.php?AC=ping&imei=%s&UT=%lu&m0=%lu&m1=%lu&tap=%s&epoch=%lu",EEPROMGetIndex(WWWPATH),
        IMEI,millis()/1000,metercount[0],metercount[1],TapToText(),ep);
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'e':
    case 'E':
    // E?
    // En,v   n 0-9 v value
      switch(*++s)
      {
        case '*':  // truncated EEPROM data
          // bug fix, cannot call EEPROMGetIndex twice as result overidden
          strcpy(tpltemp,EEPROMGetIndex(TPL0));
          sprintf(tempbuf,"imei=%s&epoch=%lu&tpl0=%s&tpl1=%s",IMEI,ep,tpltemp,EEPROMGetIndex(TPL1));
          SendLongMessage();
          break;
        case '?':   // full EEPROM data
          sprintf(tempbuf,"imei=%s",IMEI);
          sprintf(tempbuf,"%s&epoch=%lu",tempbuf,ep);
          sprintf(tempbuf,"%s&tpl0=%s",tempbuf,EEPROMGetIndex(TPL0));
          sprintf(tempbuf,"%s&tpl1=%s",tempbuf,EEPROMGetIndex(TPL1));
          sprintf(tempbuf,"%s&lis=%s",tempbuf,EEPROMGetIndex(LIS));
          sprintf(tempbuf,"%s&ph=%s",tempbuf,EEPROMGetIndex(DP));
          SendLongMessage();
          sprintf(tempbuf,"imei=%s",IMEI);
          sprintf(tempbuf,"%s&epoch=%lu",tempbuf,ep);
          sprintf(tempbuf,"%s&smtps=%s",tempbuf,EEPROMGetIndex(SmS));
          sprintf(tempbuf,"%s&ea=%s",tempbuf,EEPROMGetIndex(EA));
          SendLongMessage();
          sprintf(tempbuf,"imei=%s",IMEI);
          sprintf(tempbuf,"%s&epoch=%lu",tempbuf,ep);
          sprintf(tempbuf,"%s&pword=%s",tempbuf,EEPROMGetIndex(EP));
          sprintf(tempbuf,"%s&rcvr=%s",tempbuf,EEPROMGetIndex(ER));
          sprintf(tempbuf,"%s&apn=%s",tempbuf,EEPROMGetIndex(APN));
          sprintf(tempbuf,"%s&smsflag=%s",tempbuf,EEPROMGetIndex(SENDSMS));
          SendLongMessage();
          sprintf(tempbuf,"imei=%s",IMEI);
          sprintf(tempbuf,"%s&epoch=%lu",tempbuf,ep);
          sprintf(tempbuf,"%s&rr=%s",tempbuf,EEPROMGetIndex(RR));
          sprintf(tempbuf,"%s&pol=%s",tempbuf,EEPROMGetIndex(POL));
          sprintf(tempbuf,"%s&plw=%s",tempbuf,EEPROMGetIndex(PW));
          sprintf(tempbuf,"%s&https=%s",tempbuf,EEPROMGetIndex(HTTPs));
          sprintf(tempbuf,"%s&gpto=%s",tempbuf,EEPROMGetIndex(GPRSTO));
          SendLongMessage();
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
        case 'f':
        case 'g':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x57),s+2);  
          break;
        case 'A': // 'A'->10
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x37),s+2);   
          break;
      }
      break;
  }
}

