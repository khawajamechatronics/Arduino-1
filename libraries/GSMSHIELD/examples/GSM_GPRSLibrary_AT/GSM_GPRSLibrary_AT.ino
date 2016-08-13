#include "SIM900.h"
#include <SoftwareSerial.h>
#include "smtp.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

//InetGSM inet;
//CallGSM call;
//SMSGSM sms;
SMTPGSM smtp;

int numdata;
char inSerial[40];
int i=0;


void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is recomended to use 4800 or slower.
  if (gsm.begin(9600))
  {
    Serial.println("\nstatus=READY");
    smtp.SmtpSetCS(smtp.ASCII);
  }
  else
  Serial.println("\nstatus=IDLE");
}

void loop() 
{
  if (smtp.SmtpInit(30))
  {
    Serial.println("init passed");
    if (smtp.SmtpOpenGprs("uinternet"))
    {
      Serial.println("open gprs passed");
      if (smtp.SmtpSetServer("smtp.mail.yahoo.com",465))
      {
        Serial.println("set server passed");
        if (smtp.SmtpSetLogin("david_henry.geo@yahoo.com","mga1958"))
        {
          Serial.println("set login passed");
          if (smtp.SmtpSetSender("david_henry.geo@yahoo.com","fred"))
          {
            Serial.println("set sender passed");
            if (smtp.SmtpSetRecipient(smtp.TO,"mgadriver@gmail.com","house"))
            {
              Serial.println("set rcp passed");
              if (smtp.SmtpSetSubject("blah"))
              {
                Serial.println("set subject passed");
                if (smtp.SmtpBody("hello world"))
                   Serial.println("send body passeded");
                else
                  Serial.println("send body failed");
                smtp.SmtpCloseGprs();
                while (true){}
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
      if (smtp.SmtpCloseGprs())
        Serial.println("closed gprs");
    }
  }
  else
    Serial.println("init failed");
  delay(2000);
}
