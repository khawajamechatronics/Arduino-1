#include <avr/pgmspace.h>
#include "Arduino.h"
#include "Configuration.h"
#include "Remote.h"
// GSM shield, use SMS for control
// NOTE the following includes MUST be in the main INO file else the include path not setup correctly
// added messaging via email as that is cheaper than SMS. 
#include "SIM900.h"
#include "sms.h"
#include "smtp.h"
//#include "inetGSM.h"
#include "wcEEPROM.h"
#include "DS3234.h"


char * EEPROMGetIndex(enum eEEPROMIndex);

SMSGSM sms;
SMTPGSM smtp;
int CID = 1;
#define USESSL
#ifdef USESSL
int ServerPort = 465;
#else
int ServerPort = 25;
#endif

char LastIncomingPhone[20];
boolean started=false;
char smsbuffer[SMS_LENGTH];
char IncomingPhone[20];  // phone number
char SMStime[20];  // timestamp
extern bool SMSout;  // if true send SMS else print to Serial
#ifdef ANTENNA_BUG
extern bool ignoreMeter;     // set true while sending emails
#endif

bool RemoteInit(enum eRemoteType type)
{
  bool success = false;
  strcpy(LastIncomingPhone,DefaultHouse);
  // set up serial line and gsm if relevant
  REMOTE_SERIAL.begin(REMOTE_BAUD);
  switch (type)
  {
    case SERIAL_REMOTE:
      success = true;
      break;
    case GSM_REMOTE:
      success = gsm.begin(REMOTE_BAUD);
      if (smtp.SmtpGprsIsOpen(CID))
        smtp.SmtpCloseGprs(CID);
      break;
  }
  return success;
}

// if a message as available, pass it to caller
// delete from SIM when read
bool RemoteMessageAvailable(char *msg,char *timestamp)
{
    int i = sms.IsSMSPresent(SMS_UNREAD);
    if (i > 0)
    {
      // modified GetSMS to extract timestamp
      sms.GetSMS(i,IncomingPhone,SMStime,smsbuffer,SMS_LENGTH);
//      Serial.print("Incoming from: ");
//      Serial.println("--");
//      Serial.println(IncomingPhone);
//      Serial.println("--");
//      Serial.println(SMStime);
//      Serial.println("--");
//      Serial.println(smsbuffer);
      strcpy(LastIncomingPhone,IncomingPhone);
      strcpy(timestamp,SMStime);
      strcpy(msg,smsbuffer);
      sms.DeleteSMS(i);
      return true;
    } 
    else
      return false; 
}

bool RemoteSendEmail(char *msg)
{
  bool rc = false;
  char *cp,cpw[20],cuser[30];
#ifdef ANTENNA_BUG
  ignoreMeter = true;
#endif
  if (smtp.SmtpOpenGprs(CID,EEPROMGetIndex(APN)))
  {
    smtp.SmtpTimeout(60);
    smtp.SmtpSetCS(smtp.ASCII);
    Serial.println("open gprs passed");
#ifdef USESSL
    smtp.SmtpSetSSL(true);
#else
    smtp.SmtpSetSSL(false);
#endif
    cp = EEPROMGetIndex(EA);
    Serial.print("Sender ");
    Serial.println(cp);
    if (smtp.SmtpSetSender(cp,"Water Meter"))
    {
      Serial.println("set sender passed");
      cp = EEPROMGetIndex(SmS);
      Serial.print("Server ");
      Serial.println(cp);
      if (smtp.SmtpSetServer(cp,ServerPort))
      {
        Serial.println("set server passed");
        strcpy(cuser,EEPROMGetIndex(EA));
        Serial.print("Login ");
        Serial.println(cuser);
        strcpy(cpw,EEPROMGetIndex(EP));
        Serial.print("PW ");
        Serial.println(cpw);
        if (smtp.SmtpSetLogin(cuser,cpw))
        {
          Serial.println("set login passed");
          cp = EEPROMGetIndex(ER);
          Serial.print("Receiver ");
          Serial.println(cp);
          if (smtp.SmtpSetRecipient(smtp.TO,0,cp,"anyone"))
          {
            Serial.println("set rcp passed");
            if (smtp.SmtpSetSubject("Water Meter Message"))
            {
              Serial.println("set subject passed");
              if (smtp.SmtpSendBody(msg))
              {
                 Serial.println("send body passed");
                 rc = true;
              }
              else
                Serial.println("send body failed");
              smtp.SmtpCloseGprs(CID);
            }
            else
              Serial.println("set subject failed");
          }
          else
            Serial.println("set rcp failed");
        }
        else
          Serial.println("set sender failed");
      }
      else
        Serial.println("set login failed");
    }
    else
      Serial.println("set server failed");
  }
  else
  {
    Serial.println("open gprs failed");
  } 
  if (smtp.SmtpCloseGprs(CID))
    Serial.println("closed gprs");
#ifdef ANTENNA_BUG
  ignoreMeter = false;
#endif
  return rc; 
}
bool RemoteSendMessage(char * msg)
{
  Serial.println(msg);
  if (SMSout != 0)
  {
    Serial.println("SMS");
    return sms.SendSMS(DefaultHouse/*LastIncomingPhone*/, msg);
  }
  else
  {
    int i = 5;  // retries until success
    bool rc = false;
    Serial.println("email");
    while (!rc && i--)
      rc = RemoteSendEmail(msg); 
    return rc;   
  }
}

bool RemoteSendMessage(char * msg, char *sub)
{
  Serial.println(msg);
  if (SMSout)
    return sms.SendSMS(LastIncomingPhone, msg);
  else
  {
  int i = 5;  // retries until success
  bool rc = false;
  while (!rc && i--)
    rc = RemoteSendEmail(msg);    
  }
}

bool RemoteSetClock(char *s)
{
  return smtp.SmtpSetClock(s);
}

char *RemoteGetClock()
{
  return smtp.SmtpGetClock();
}

