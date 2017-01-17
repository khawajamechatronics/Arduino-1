#include "Arduino.h"
#include "A6Services.h"
#include <ctype.h>

GPRSA6 gsm;
GPRSA6::GPRSA6(){
  cid = 1;
  CIPstatus = IP_STATUS_UNKNOWN;
};
GPRSA6::~GPRSA6(){};

static char *statusnames[] = {"IP INITIAL","IP START","IP CONFIG","IP IND","IP GPRSACT","IP STATUS","TCP/UDP CONNECTING","IP CLOSE","CONNECT OK"};
//const char* const status_names[] PROGMEM = {"IP INITIAL","IP START","IP CONFIG","IP IND","IP GPRSACT","IP STATUS","TCP/UDP CONNECTING","IP CLOSE","CONNECT OK"};

char tempbuf[100];

void HW_SERIAL_EVENT() {
  while (HW_SERIAL.available())
    gsm.push(HW_SERIAL.read());
}

bool GPRSA6::getIMEI(char* imei)
{
  bool rc;
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+EGMR=2,7\r"));
  rc = GetLineWithPrefix("+EGMR:",imei,20,500);
  waitresp("OK\r\n",500);
  return rc;
}

bool GPRSA6::getCIMI(char* cimi)
{
  bool rc;
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+CIMI\r"));
  waitresp("\r\n",500);
  rc = GetLineWithPrefix(NULL,cimi,20,500);
  waitresp("OK\r\n",500);
  return rc;
}
bool GPRSA6::getRTC(char* rtc)
{
  bool rc;
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+CCLK\r"));
  waitresp("\r\n",500);
  rc = GetLineWithPrefix("+CCLK:",rtc,30,500);
  waitresp("OK\r\n",500);
  return rc;
}
bool GPRSA6::setRTC(char* rtc)
{
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+CCLK=\""));
  HW_SERIAL.print(rtc);
  HW_SERIAL.print("\"\r");
  return waitresp("OK\r\n",500);
}

enum GPRSA6::eCIPstatus GPRSA6::getCIPstatus()
{
  enum eCIPstatus es = IP_STATUS_UNKNOWN;
  gsm.RXFlush();
  HW_SERIAL.print("AT+CIPSTATUS\r");
  if (GetLineWithPrefix("+IPSTATUS:",tempbuf,50,1000))
  {
    char *s = tempbuf;  // skip over whitespace
    while (isspace(*s))
      s++;
    int i;
    for (i=0;i<9;i++)
      if (strncmp(s,statusnames[i],strlen(statusnames[i])) == 0)
      {
        es = i;
        break;
      }
  }
  waitresp("OK\r\n",2000);  // trailing stuff
  CIPstatus = es;
  return es;
}

char *GPRSA6::getCIPstatusString(enum eCIPstatus i)
{
  return statusnames[i];
}

char *GPRSA6::getCIPstatusString()
{
  return statusnames[getCIPstatus()];
}

bool GPRSA6::startIP(char *apn,char*user,char *pwd)  // apn, username, password
{
  bool rc = false;
  cid = 1; //gsm.getcid();
  gsm.RXFlush();
  if (CIPstatus != IP_INITIAL)
  {
    HW_SERIAL.print(F("AT+CIPCLOSE\r"));
    waitresp("OK\r\n",2000);
  }
//  DebugWrite("\r\n2:");
//  DebugWrite(getCIPstatusString());
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+CGATT=1\r"));
  if (waitresp("OK\r\n",10000))
  {
//    DebugWrite("\r\n3:");
//    DebugWrite(getCIPstatusString());
    gsm.RXFlush();
    sprintf(tempbuf,"AT+CGDCONT=%d,\"IP\",\"%s\"\r",cid,apn);
    DebugWrite(tempbuf);
    HW_SERIAL.print(tempbuf);
    if (waitresp("OK\r\n",1000))
    {
 //     DebugWrite("\r\n4:");
//      DebugWrite(getCIPstatusString());
      gsm.RXFlush();
      sprintf(tempbuf,"AT+CGACT=1,%d\r",cid);
      HW_SERIAL.print(tempbuf);
      if (waitresp("OK\r\n",1000))
      {
//        DebugWrite("\r\n5:");
 //       DebugWrite(getCIPstatusString());
        gsm.RXFlush();
        rc = true;
      }
    }
  }
//  DebugWrite("\r\n6:");
//  DebugWrite(getCIPstatusString());
  gsm.RXFlush();
  return rc;
}

bool GPRSA6::startIP(char *apn)  // apn
{
  return startIP(apn,"","");
}

bool GPRSA6::stopIP()
{
  bool rc = false;
  gsm.RXFlush();
  HW_SERIAL.print("AT+CIPCLOSE\r");
  rc = waitresp("OK\r\n",1000);
  return rc;
}

GPRSA6::ePSstate GPRSA6::getPSstate()
{
  ePSstate eps = PS_UNKNOWN;
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+CGATT?\r"));  
  if (GetLineWithPrefix("+CGATT:",tempbuf,50,1000))
  {
    if (*tempbuf == '0')
      eps = DETACHED;
    else if (*tempbuf == '1')
      eps = ATTACHED;
  }
  return eps;
}

bool GPRSA6::setPSstate(GPRSA6::ePSstate eps)
{
  bool rc = false;
  gsm.RXFlush();
  switch (eps)
  {
    case DETACHED:
      HW_SERIAL.print(F("AT+CGATT=0\r"));
      break;
    case ATTACHED:
      HW_SERIAL.print(F("AT+CGATT=1\r"));
      break;
  }
  return waitresp("OK\r\n",2000);
}

bool GPRSA6::getLocalIP(char *ip)
{
  bool rc = false;
  gsm.RXFlush();
  HW_SERIAL.print(F("AT+CIFSR\r"));
  GetLineWithPrefix(NULL,ip,20,2000);
  return waitresp("OK\r\n",2000);
}
bool GPRSA6::connectTCPserver(char*path,int port)
{
  bool rc = false;
  CIPstatus = getCIPstatus();
  if (CIPstatus == IP_CLOSE || CIPstatus == IP_GPRSACT)
  {
    sprintf(tempbuf,"AT+CIPSTART=\"TCP\",\"%s\",%d\r",path,port);
    DebugWrite(tempbuf);
    HW_SERIAL.print(tempbuf);
    if (waitresp("CONNECT OK",10000))
    {
      DebugWrite(">>");
      waitresp("OK\r\n",10000);
      rc = true;
    }
  }
  return rc;
}
bool GPRSA6::sendToServer(char*msg)
{
  bool rc = false;
  getCIPstatus();
  if (CIPstatus == CONNECT_OK)
  {
    HW_SERIAL.print(F("AT+CIPSEND\r"));
    if (waitresp(">",100))
    {
      HW_SERIAL.print(msg);
      HW_SERIAL.write(0x1a);
      waitresp("OK\r\n",1000);
      rc = true;
    }
  }
  return rc;
}
bool GPRSA6::sendToServer(char*msg,int length)
{
  bool rc = false;
  getCIPstatus();
  if (CIPstatus == CONNECT_OK)
  {
    HW_SERIAL.print(F("AT+CIPSEND="));
    HW_SERIAL.print(length);
    HW_SERIAL.print("\r");
    if (waitresp(">",100))
    {
      HW_SERIAL.print(msg);
      waitresp("OK\r\n",1000);
      rc = true;
    }
  }
  return rc;
}

bool GPRSA6::sendToServer(byte*msg,int length)
{
  bool rc = false;
  char buff[10];
  getCIPstatus();
  if (CIPstatus == CONNECT_OK)
  {
    HW_SERIAL.print(F("AT+CIPSEND="));
    HW_SERIAL.print(length);
    HW_SERIAL.print("\r");
    if (waitresp(">",100))
    {
      for (int i=0;i<length;i++)
      {
        sprintf(buff,"%02X,",msg[i]);
        DebugWrite(buff);
        HW_SERIAL.write(msg[i]);
      }
      waitresp("OK\r\n",1000);
      rc = true;
    }
  }
  return rc;
}

