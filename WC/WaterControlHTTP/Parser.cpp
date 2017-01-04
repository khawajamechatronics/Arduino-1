#include "Configuration.h"
#include <Time.h>
//#include <TimeLib.h>
#include "Arduino.h"
#include "Remote.h"
#include "wcEEPROM.h"
#include "Relay.h"
#include "SoftReset.h"
#ifndef DEBUG_SERIAL
#include <SendOnlySoftwareSerial.h>
extern SendOnlySoftwareSerial DEBUG_SERIAL;
#endif

extern unsigned TicksPerLiter[];
extern unsigned long TotalMeterCount[], CurrentMeterCount[],PreviousMeterCount[],REPORT_RATE;
extern char tempbuf[];
extern char IMEI[];  // why ? *IMEI causes problems
extern char cooked[];

#ifdef ANTENNA_BUG
extern bool ignoreMeter;     // set true while sending emails
#endif
char taptemp[10];
char *FieldNames[] = {"tpl","lis","ph","smtps","ea","pword","rcvr","apn","smsflag",
                   "rr","pol","plw","https","ssl","gpto","tpl0","tpl1"};
void EEPROMReport(int indices[],int numindices)
{
  char singleitem[50];
  sprintf(singleitem,"imei=%s&epoch=%lu",IMEI,Sim900ToEpoch());
  strcpy(tempbuf,singleitem);
// tempbuf[0] = 0;
  for (int i=0;i<numindices;i++)
  {
//    DEBUG_SERIAL.print("Index ");DEBUG_SERIAL.print(indices[i]);DEBUG_SERIAL.print(" ");DEBUG_SERIAL.println(FieldNames[indices[i]])
    sprintf(singleitem,"&%s=%s",FieldNames[indices[i]],EEPROMGetIndex(indices[i]));
//    DEBUG_SERIAL.println(singleitem);
    strcat(tempbuf,singleitem);
  //  DEBUG_SERIAL.println(tempbuf);
  }
  urlencode(tempbuf,cooked);
  strcpy(tempbuf,"/WC/eeprom.php?");  // do not do url encode on this bit
  strcat(tempbuf,cooked);          
//  DEBUG_SERIAL.println(tempbuf);
  int retries = 3;
  while (retries > 0)
  {
    if (RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80))
      retries = 0;
    else
      retries--;
  }
}
void Parse(char * s)
{
  static int AllItems [] = {TPL,LIS,DP,SmS,EA,EP,ER,APN,SENDSMS,RR,POL,PW,HTTPs,useSSL,GPRSTO,TPL0,TPL1};
  unsigned long secs;
  int indices[1];
  DEBUG_SERIAL.print("Parse:");
  DEBUG_SERIAL.println(s);
  int retries; // for httpget
  switch(*s)
  {
    case 'b':
    case 'B':
      soft_restart();
      break;
    case 'c':
    case 'C':
      TapChangeState(TAP_CLOSE);
      sprintf(tempbuf,"/WC/RawData.php?AC=tapchange&imei=%s&UT=%lu&tap=tapclose&epoch=%lu",IMEI,millis()/1000,Sim900ToEpoch());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'o':
    case 'O':
      TapChangeState(TAP_OPEN);
      sprintf(tempbuf,"/WC/RawData.php?AC=tapchange&imei=%s&UT=%lu&tap=tapopen&epoch=%lu",IMEI,millis()/1000,Sim900ToEpoch());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
      break;
    case 'r':
    case 'R':
      PreviousMeterCount[0] = CurrentMeterCount[0] = PreviousMeterCount[1] = CurrentMeterCount[1] = 0;
      InitEepromData();
#ifdef ANTENNA_BUG
      ignoreMeter = false;
#endif
      sprintf(tempbuf,"/WC/RawData.php?AC=reset&imei=%s&UT=%lu&epoch=%lu",IMEI,millis()/1000,Sim900ToEpoch());
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
      sprintf(tempbuf,"/WC/RawData.php?AC=ping&imei=%s&UT=%lu&m0=%lu&m1=%lu&tap=%s&epoch=%lu",
        IMEI,millis()/1000,TotalMeterCount[0],TotalMeterCount[1],TapToText(),Sim900ToEpoch());
      retries = 3;
      while (retries > 0)
      {
        if (RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80))
          retries = 0;
        else
          retries--;
      }
      break;
    case 'e':
    case 'E':
    // E?
    // En,v   n 0-9 v value
      switch(toupper(*++s))
      {
        case '?':
          EEPROMReport(AllItems,TPL1+1);
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
          indices[0] = *s-'0';
          EEPROMReport(indices,1);
          break;
        case 'A': // 'A'->10
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
          EEPROMSetIndex((eEEPROMIndex)(*s-0x37),s+2);   
          indices[0] = *s-0x37;
          EEPROMReport(indices,1);
          break;
      }
      break;
  }
}

