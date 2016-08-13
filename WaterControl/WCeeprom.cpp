#include <avr/pgmspace.h>
#include "Arduino.h"
#include <EEPROM.h>
#include "Configuration.h"
#include "WCeeprom.h"

extern char*tempbuf;  // buffer in main program
extern unsigned MAX_FLOW_TIME; // default secs
extern unsigned long REPORT_RATE;  // report every hour
extern unsigned TicksPerLiter;
extern bool SMSout;  // save on sending SMS just print to Serial
extern bool polaritySwitch;
extern unsigned PulseWidth;   // pulse to solenoid

void InitEepromData()
{
  sprintf(tempbuf,"%d",DEFAULT_TICKS_PER_LITER);
  EEPROMSetIndex(TPL,tempbuf);    // ticks per liter
  sprintf(tempbuf,"%d",DEFAULT_LEAK_IN_SECS);
  EEPROMSetIndex(LIS,tempbuf);                // leak time in seconds
  EEPROMSetIndex(DP,DEFAULT_HOME_NUMBER);     // default phone number
  EEPROMSetIndex(SmS,DEFAULT_SMTP_SERVER);    // smtp server
  EEPROMSetIndex(EA,DEFAULT_SENDER_ACCOUNT);  // sender mail account
  EEPROMSetIndex(EP,DEFAULT_SENDER_PASSWORD);              // sender password
  EEPROMSetIndex(ER,DEFAULT_RECEIVER_ACCOUNT);// recipient account
  EEPROMSetIndex(APN,DEFAULT_APN);            // apn
  sprintf(tempbuf,"%d",false);
  EEPROMSetIndex(SENDSMS,tempbuf);            // sendSMS false 
  sprintf(tempbuf,"%lu",DEFAULT_REPORT_RATE);
  EEPROMSetIndex(RR,tempbuf);                 // report rate every 24 hours
  sprintf(tempbuf,"%d",true);
  EEPROMSetIndex(POL,tempbuf);                // solenoid polarity
  sprintf(tempbuf,"%d",DEFAULT_PULSE_WIDTH);
  EEPROMSetIndex(PW,tempbuf);                 // 100 msec pulse
}

void PrintEepromData()
{
 ;
  Serial.println(EEPROMGetIndex(TPL));
  Serial.println(EEPROMGetIndex(LIS));
  Serial.println(EEPROMGetIndex(DP));
  Serial.println(EEPROMGetIndex(SmS));
  Serial.println(EEPROMGetIndex(EA));
  Serial.println(EEPROMGetIndex(EP));
  Serial.println(EEPROMGetIndex(ER));
  Serial.println(EEPROMGetIndex(APN));
  Serial.println(EEPROMGetIndex(SENDSMS));
  Serial.println(EEPROMGetIndex(RR));
  Serial.println(EEPROMGetIndex(POL));
  Serial.println(EEPROMGetIndex(PW));
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
  Serial.print(i);
  Serial.print(",");
  Serial.println(s);
  switch(i)
  {
    case TPL:
      stemp = atoi(s);
      eepromsetarray(0,(uint8_t *)&stemp,sizeof(unsigned));
      TicksPerLiter = stemp;
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
      sprintf(eeprombuf,"%d",stemp);
      break;
    case LIS:
      eepromgetarray(offsetof(struct eEEPROMmap,LeakInSecs),(uint8_t *)&stemp,sizeof(unsigned));
      sprintf(eeprombuf,"%d",stemp);
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
      sprintf(eeprombuf,"%d",stemp);
      break;
  }
  return eeprombuf;
}

