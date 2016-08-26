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
#include "inetGSM.h"
#include "wcEEPROM.h"
#include "DS3234.h"


char * EEPROMGetIndex(enum eEEPROMIndex);

SMSGSM sms;
SMTPGSM smtp;
InetGSM inet;

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
char cooked[300];

void Connect(int cid)
{
  bool rc = false;
   //GPRS attach, put in order APN, username and password.
  if (inet.CloseGprs(HTTPCID)) // ignore if error
    Serial.println("status=DETACHED");
  else
    Serial.println("status=NOT DETACHED");
//  Serial.println(EEPROMGetIndex(APN));
  while (!inet.OpenGprs(cid,EEPROMGetIndex(APN),20))
//  while (!inet.OpenGprs(HTTPCID,"uinternet",20))
    Serial.println("status=NOT ATTACHED");
  Serial.println("status=ATTACHED");
}

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
      if (success)
      {
        // open up socket & keep it forever
        Connect(HTTPCID); // loops until done
      }
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
#if 1
      Serial.print("Incoming from: ");
      Serial.println("--");
      Serial.println(IncomingPhone);
      Serial.println("--");
      Serial.println(SMStime);
      Serial.println("--");
      Serial.println(smsbuffer);
#endif
      strcpy(LastIncomingPhone,"0545919886"/*IncomingPhone*/);
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
  return true;
  bool rc = false;
  char *cp,cpw[20],cuser[30];
#ifdef ANTENNA_BUG
  ignoreMeter = true;
#endif
  if (smtp.SmtpOpenGprs(SMTPCID,EEPROMGetIndex(APN)))
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
              smtp.SmtpCloseGprs(SMTPCID);
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
  if (smtp.SmtpCloseGprs(SMTPCID))
    Serial.println("closed gprs");
#ifdef ANTENNA_BUG
  ignoreMeter = false;
#endif
  return rc; 
}
bool RemoteSetClock(char *s)
{
  return smtp.SmtpSetClock(s);
  //return true;
}

char *RemoteGetClock()
{
  char *cs,*ce;
  //return smtp.SmtpGetClock();
  //return "16/12/1 2:3:4+12";
  // string includes quotes and spaces so trim those
  cs = strchr(smtp.SmtpGetClock(),'\"');
  cs++;
  ce = strchr(cs,'+');
  *ce = 0;
//  Serial.println(cs);
//  Serial.println(urlencode(cs,cooked));
//  Serial.println(cooked);
  urlencode(cs,cooked);
  return cooked;
}

int urlencode(char *src, char *tgt)
{
  int offset = 0;
//  const char rfc3986[] = " !*'();:@&=+$,/?#[]";
  const char rfc3986[] = " !*'();:@+$,/?#[]";  // without & = 
  while (*src)
  {
    if (strchr(rfc3986,*src))
    {
      sprintf(&tgt[offset],"%%%02X",*src);
      offset += 3;
    }
    else
      tgt[offset++] = *src;
    src++;
  }
  tgt[offset] = 0;
  return offset;
}

int HTTPGet(char *server,char *url,char *buf, int buflen)
{
  char *c;
  int rc;
  int CR = inet.httpGET(server, 80, url, buf, buflen);
  Serial.print("Length of data received: ");
  Serial.println(CR);  
    // extract return code from Nth line HTTP/1.1 200 OK
  Serial.println("--------------------------------");
  if (CR > 0)
  {
    c = strchr(buf,'H');
    Serial.println(c);
    c = strchr(c,' ');
    sscanf(c,"%d",&rc);
    return rc;    
  }
  else
    return 0;
}

bool RemoteSendMessage(char * srv, char *url, int port) // to http server
{
  int retries = 3;
  char SRV[30];
  bool success = false;
  int rc;
  memcpy(SRV,srv,30);
  Serial.println(SRV);
  Serial.println(url);
  // check if connection still open, if not re-open
 // check IP address
  char *c = inet.GetGprsIP(HTTPCID);
  Serial.println(c);
  if (strcmp(c,"0.0.0.0") == 0)
  {
    // restart GPRS
    Serial.println("Restart GPRS");
    gsm.Echo(0);
    gsm.RxInit(15000, 2000); 
    Connect(HTTPCID);
    c = inet.GetGprsIP(HTTPCID);
    Serial.println(c);
  }
  retries = 3;
  success = false;
  while (retries-- > 0 && !success)
  {
    rc = HTTPGet(SRV,url,smsbuffer,sizeof(smsbuffer));
    Serial.println(rc);
    success = (rc == 200);
  }
  return success;
}

