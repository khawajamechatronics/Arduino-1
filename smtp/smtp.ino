#include "SIM900.h"
#include <SoftwareSerial.h>
#include "smtp.h"
#include "inetGSM.h"

#define YAHOO
//#define HOTMAIL
//#define GMAIL
//#define ZICKEL

#define USESSL
//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

SMTPGSM smtp;

int i=0;

// Edit the following for your own circumstances
#ifdef YAHOO
char *Sender = "dhdh654321@yahoo.com";
char *ServerURL = "smtp.mail.yahoo.com";
char *Login = "dhdh654321@yahoo.com";
char *Password = "Morris59";
#endif
#ifdef ZICKEL
char *Sender = "davidh@zickel.net";
char *ServerURL = "mail.zickel.net";
char *Login = "davidh@zickel.net";
char *Password = "henryd";
#endif
#ifdef HOTMAIL
char *Sender = "david_henry_il@hotmail.com";
char *ServerURL = "smtp.live.com";
char *Login = "david_henry_il@hotmail.com";
char *Password = "Morris59";
#endif
#ifdef GMAIL
char *Sender = "mgadriver@gmail.com";
char *ServerURL = "aspmx.l.google.com";
char *Login = "mgadriver@gmail.com";
char *Password = "Nubira2001";
#endif
char *APN = "uinternet";
char *SenderNickName = "dave";
char *Recipient = "mgadriver@gmail.com";
char *RecipientNickName = "Fred";
char *Subject = "more stuff ok";
//char Body[50];
char *Body = "TPL 359,LIS 31,PH +972545919886,Srv smtp.mail.yahoo.com,UA dhdh654321@yahoo.com,PW Morris59,RCV mgadriver@gmail.com,APN uinternet";
int pass = 0;
int CID = 1;
#ifdef USESSL
int ServerPort = 465;
#else
int ServerPort = 25;
#endif

void setup() 
{
  //Serial connection.
  Serial.begin(115200);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is recomended to use 4800 or slower.
  if (gsm.begin(9600))
  {
    Serial.println("\nstatus=READY");
  //  smtp.SmtpInit(60);
  //  Serial.println(smtp.SmtpGetBodySize());
    if (smtp.SmtpGprsIsOpen(CID))
      smtp.SmtpCloseGprs(CID);
    Serial.println("init passed");
  }
  else
    Serial.println("\nstatus=IDLE");
}

void loop() 
{
  if (smtp.SmtpOpenGprs(CID,APN))
  {
    smtp.SmtpTimeout(60);
    smtp.SmtpSetCS(smtp.ASCII);
    Serial.println("open gprs passed");
#ifdef USESSL
    smtp.SmtpSetSSL(true);
#else
    smtp.SmtpSetSSL(false);
#endif
    if (smtp.SmtpSetSender(Sender,SenderNickName))
    {
      Serial.println("set sender passed");
      if (smtp.SmtpSetServer(ServerURL,ServerPort))
      {
        Serial.println("set server passed");
        if (smtp.SmtpSetLogin(Login,Password))
        {
          Serial.println("set login passed");
          if (smtp.SmtpSetRecipient(smtp.TO,0,Recipient,RecipientNickName))
          {
       //     smtp.SmtpSetRecipient(smtp.CC,0,"mgtcdriver@gmail.com",RecipientNickName);
            Serial.println("set rcp passed");
            if (smtp.SmtpSetSubject(Subject))
            {
              Serial.println("set subject passed");
          //    sprintf(Body,"hello world %d",pass++);
              if (smtp.SmtpSendBody(Body))
                 Serial.println("send body passed");
              else
                Serial.println("send body failed");
              smtp.SmtpCloseGprs(CID);
//              while (true){}
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
    if (smtp.SmtpCloseGprs(CID))
      Serial.println("closed gprs");
  }
}
