#include <avr/pgmspace.h>
#include "Arduino.h"
#include <EEPROM.h>
#include "Configuration.h"
#include "WCeeprom.h"
#ifndef DEBUG_SERIAL
#include <SendOnlySoftwareSerial.h>
extern SendOnlySoftwareSerial DEBUG_SERIAL;
#endif


char TempBuf[20];  // buffer in main program
extern unsigned MAX_FLOW_TIME; // default secs
extern unsigned long REPORT_RATE;  // report every hour
extern unsigned TicksPerLiter[];
extern bool SMSout;  // save on sending SMS just print to Serial
extern bool polaritySwitch;
extern unsigned PulseWidth;   // pulse to solenoid
extern byte gprstimeout;

void InitEepromData()
{
  sprintf(TempBuf,"%d",DEFAULT_TICKS_PER_LITER);
  EEPROMSetIndex(TPL,TempBuf);    // ticks per liter
  sprintf(TempBuf,"%d",DEFAULT_LEAK_IN_SECS);
  EEPROMSetIndex(LIS,TempBuf);                // leak time in seconds
  EEPROMSetIndex(DP,DEFAULT_HOME_NUMBER);     // default phone number
  EEPROMSetIndex(SmS,DEFAULT_SMTP_SERVER);    // smtp server
  EEPROMSetIndex(EA,DEFAULT_SENDER_ACCOUNT);  // sender mail account
  EEPROMSetIndex(EP,DEFAULT_SENDER_PASSWORD);              // sender password
  EEPROMSetIndex(ER,DEFAULT_RECEIVER_ACCOUNT);// recipient account
  EEPROMSetIndex(APN,DEFAULT_APN);            // apn
  sprintf(TempBuf,"%d",false);
  EEPROMSetIndex(SENDSMS,TempBuf);            // sendSMS false 
  sprintf(TempBuf,"%lu",DEFAULT_REPORT_RATE);
  EEPROMSetIndex(RR,TempBuf);                 // report rate every 24 hours
  sprintf(TempBuf,"%d",true);
  EEPROMSetIndex(POL,TempBuf);                // solenoid polarity
  sprintf(TempBuf,"%d",DEFAULT_PULSE_WIDTH);
  EEPROMSetIndex(PW,TempBuf);                 // 100 msec pulse
  EEPROMSetIndex(HTTPs,DEFAULT_HTTP_SERVER);  // http server
  sprintf(TempBuf,"%d",false);
  EEPROMSetIndex(useSSL,TempBuf);            // useSSL false 
  sprintf(TempBuf,"%d",DEFAULT_GPRS_TIMEOUT);
  EEPROMSetIndex(GPRSTO,TempBuf);            // time for at+sapbr=1,1 response
  sprintf(TempBuf,"%d",DEFAULT_TICKS_PER_LITER);
  EEPROMSetIndex(TPL0,TempBuf);    // ticks per liter
  sprintf(TempBuf,"%d",DEFAULT_TICKS_PER_LITER);
  EEPROMSetIndex(TPL1,TempBuf);    // ticks per liter
}

void PrintEepromData()
{
  DEBUG_SERIAL.println(EEPROMGetIndex(TPL));
  DEBUG_SERIAL.println(EEPROMGetIndex(LIS));
  DEBUG_SERIAL.println(EEPROMGetIndex(DP));
  DEBUG_SERIAL.println(EEPROMGetIndex(SmS));
  DEBUG_SERIAL.println(EEPROMGetIndex(EA));
  DEBUG_SERIAL.println(EEPROMGetIndex(EP));
  DEBUG_SERIAL.println(EEPROMGetIndex(ER));
  DEBUG_SERIAL.println(EEPROMGetIndex(APN));
  DEBUG_SERIAL.println(EEPROMGetIndex(SENDSMS));
  DEBUG_SERIAL.println(EEPROMGetIndex(RR));
  DEBUG_SERIAL.println(EEPROMGetIndex(POL));
  DEBUG_SERIAL.println(EEPROMGetIndex(PW));
  DEBUG_SERIAL.println(EEPROMGetIndex(HTTPs));
  DEBUG_SERIAL.println(EEPROMGetIndex(useSSL));
  DEBUG_SERIAL.println(EEPROMGetIndex(GPRSTO));
  DEBUG_SERIAL.println(EEPROMGetIndex(TPL0));
  DEBUG_SERIAL.println(EEPROMGetIndex(TPL1));
}

void eepromgetarray(int startaddress,uint8_t *target,int l)
{
  int i;
  for (i=0;i<l;i++)
    *target++ = EEPROM.read(startaddress++);
}

// copy zero terminated atring up to length of target

void eepromgetstring(int startaddress,char *target,int maxlength)
{
  int i;
  char c;
  for (i=0;i<maxlength;i++)
  {
    c = EEPROM.read(startaddress++);
    *target++ = c;
    if (c == 0)
      break;
  }
}

void eepromsetstring(int startaddress,char *source,int maxlength)
{
  int i;
  char c;
  for (i=0;i<maxlength;i++)
  {
    c = *source++;
    EEPROM.write(startaddress++,c);
    if (c == 0)
      break;
  }
}

void eepromsetarray(int startaddress,uint8_t *source,int l)
{
  int i;
  for (i=0;i<l;i++)
    EEPROM.write(startaddress++,*source++);
}

void EEPROMSetIndex(enum eEEPROMIndex i,char * s)
{
  uint32_t stemp;
#if 0
  DEBUG_SERIAL.print(i);
  DEBUG_SERIAL.print(",");
  DEBUG_SERIAL.println(s);
#endif
  switch(i)
  {
    case TPL:
      stemp = atoi(s);
      eepromsetarray(offsetof(struct eEEPROMmap,TicksPerLiter),(uint8_t *)&stemp,sizeof(unsigned));
    //  TicksPerLiter = stemp;
      break;
    case LIS:
      stemp = atol(s);
      eepromsetarray(offsetof(struct eEEPROMmap,LeakInSecs),(uint8_t *)&stemp,sizeof(unsigned));
      MAX_FLOW_TIME = stemp;
      break;
    case DP:
      eepromsetstring(offsetof(struct eEEPROMmap,DefaultPhone),s,sizeof(((struct eEEPROMmap*)0)->DefaultPhone));
      break;
    case SmS:
      eepromsetstring(offsetof(struct eEEPROMmap,SMTPServer),s,sizeof(((struct eEEPROMmap*)0)->SMTPServer));
      break;
    case EA:
      eepromsetstring(offsetof(struct eEEPROMmap,EmailAccount),s,sizeof(((struct eEEPROMmap*)0)->EmailAccount));
      break;
    case EP:
      eepromsetstring(offsetof(struct eEEPROMmap,EmailPassword),s,sizeof(((struct eEEPROMmap*)0)->EmailPassword));
      break;
    case ER:
      eepromsetstring(offsetof(struct eEEPROMmap,EmailRecipient),s,sizeof(((struct eEEPROMmap*)0)->EmailRecipient));
      break;
    case APN:
      eepromsetstring(offsetof(struct eEEPROMmap,Apn),s,sizeof(((struct eEEPROMmap*)0)->Apn));
      break;
    case SENDSMS:
      stemp = atoi(s);
      eepromsetarray(offsetof(struct eEEPROMmap,sendSMS),(uint8_t *)&stemp,sizeof(bool));
      SMSout = stemp;
      break;
    case RR:
      stemp = atol(s);
      eepromsetarray(offsetof(struct eEEPROMmap,reportRate),(uint8_t *)&stemp,sizeof(uint32_t));
      REPORT_RATE = stemp;
      break;
    case POL:
      stemp = atoi(s);
      eepromsetarray(offsetof(struct eEEPROMmap,polarity),(uint8_t *)&stemp,sizeof(bool));
      polaritySwitch = stemp;
      break;
    case PW:
      stemp = atoi(s);
      eepromsetarray(offsetof(struct eEEPROMmap,pulsewidth),(uint8_t *)&stemp,sizeof(unsigned));
      PulseWidth = stemp;
      break;
    case HTTPs:
      eepromsetstring(offsetof(struct eEEPROMmap,HTTPServer),s,sizeof(((struct eEEPROMmap*)0)->HTTPServer));
      break;
    case useSSL:
      stemp = atoi(s);
      eepromsetarray(offsetof(struct eEEPROMmap,useSSL),(uint8_t *)&stemp,sizeof(bool));
      //SMSout = stemp;
      break;
    case GPRSTO:
      stemp = atol(s);
      eepromsetarray(offsetof(struct eEEPROMmap,gprstimeout),(uint8_t *)&stemp,sizeof(int));
      gprstimeout = stemp;
      break;
    case TPL0:
      stemp = atol(s);
      eepromsetarray(offsetof(struct eEEPROMmap,TicksPerLiter0),(uint8_t *)&stemp,sizeof(unsigned));
      TicksPerLiter[0] = stemp;
      break;
    case TPL1:
      stemp = atol(s);
      eepromsetarray(offsetof(struct eEEPROMmap,TicksPerLiter1),(uint8_t *)&stemp,sizeof(unsigned));
      TicksPerLiter[1] = stemp;
      break;
  }
}
static char eeprombuf[30];  // just for eepromgetindex
char * EEPROMGetIndex(enum eEEPROMIndex i)
{
  uint32_t stemp;
  switch(i)
  {
    case TPL:
      eepromgetarray(0,(uint8_t *)&stemp,sizeof(unsigned));
      sprintf(eeprombuf,"%u",stemp);
      break;
    case LIS:
      eepromgetarray(offsetof(struct eEEPROMmap,LeakInSecs),(uint8_t *)&stemp,sizeof(unsigned));
      sprintf(eeprombuf,"%u",stemp);
      break;
    case DP:
      eepromgetstring(offsetof(struct eEEPROMmap,DefaultPhone),eeprombuf,20);
      break;
    case SmS:
      eepromgetstring(offsetof(struct eEEPROMmap,SMTPServer),eeprombuf,30);
      break;
    case EA:
      eepromgetstring(offsetof(struct eEEPROMmap,EmailAccount),eeprombuf,30);
      break;
    case EP:
      eepromgetstring(offsetof(struct eEEPROMmap,EmailPassword),eeprombuf,20);
      break;
    case ER:
      eepromgetstring(offsetof(struct eEEPROMmap,EmailRecipient),eeprombuf,30);
      break;
    case APN:
      eepromgetstring(offsetof(struct eEEPROMmap,Apn),eeprombuf,30);
      break;
    case SENDSMS:
      eepromgetarray(offsetof(struct eEEPROMmap,sendSMS),(uint8_t *)&stemp,sizeof(bool));
      sprintf(eeprombuf,"%d",stemp);
      break;
    case RR:
      eepromgetarray(offsetof(struct eEEPROMmap,reportRate),(uint8_t *)&stemp,sizeof(uint32_t));
      sprintf(eeprombuf,"%lu",stemp);
      break;
    case POL:
      eepromgetarray(offsetof(struct eEEPROMmap,polarity),(uint8_t *)&stemp,sizeof(bool));
      sprintf(eeprombuf,"%d",stemp);
      break;
    case PW:
      eepromgetarray(offsetof(struct eEEPROMmap,pulsewidth),(uint8_t *)&stemp,sizeof(unsigned));
      sprintf(eeprombuf,"%u",stemp);
      break;
    case HTTPs:
      eepromgetstring(offsetof(struct eEEPROMmap,HTTPServer),eeprombuf,30);
      break;
    case useSSL:
      eepromgetarray(offsetof(struct eEEPROMmap,useSSL),(uint8_t *)&stemp,sizeof(bool));
      sprintf(eeprombuf,"%d",stemp);
      break;
    case GPRSTO:
      eepromgetarray(offsetof(struct eEEPROMmap,gprstimeout),(uint8_t *)&stemp,sizeof(int));
      sprintf(eeprombuf,"%d",stemp);
      break;
    case TPL0:
      eepromgetarray(offsetof(struct eEEPROMmap,TicksPerLiter0),(uint8_t *)&stemp,sizeof(unsigned));
      sprintf(eeprombuf,"%u",stemp);
      break;
    case TPL1:
      eepromgetarray(offsetof(struct eEEPROMmap,TicksPerLiter1),(uint8_t *)&stemp,sizeof(unsigned));
      sprintf(eeprombuf,"%u",stemp);
      break;
  }
//  DEBUG_SERIAL.print("GI:");DEBUG_SERIAL.println(eeprombuf);
  return eeprombuf;
}

