#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//#define USE_TCP  // if commented out uses UDP
//#define DUMMY_DATA
#define TRANSPARENT
//Simple sketch to start a connection as client.
char *APN = "uinternet"; // orange
char *ServerName = "david-henry.dyndns.tv";
//char *ServerName = "79.177.199.194";
int ServerPort = 8001;
char IMEIbuff[17];

//char *Data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";  // ends in ctrl
/*
 * XML stuff
 */
const char *xmlformat1="<C I=\"%u\",D=\"%u\",E=\"%c\",T=\"%s\"";  // only needs to be done once
const char *xmlformat2=",L=\"%u\" />";  // do this for each block
char *pXML;
// serial ports and baud rates

#define GPSSERIAL Serial2
#define GSMSERIAL Serial1
#define GPS_BAUD 38400
#define GSM_BAUD 115200
#define DEBUG_BAUD 38400

unsigned ID=237,DD=23;
const char Endian='T';  // text, neither big nor little
const char *T="NMEA";
char xmlheader[80];
unsigned xmlfixedheaderlength;


InetGSM inet;
//CallGSM call;
//SMSGSM sms;

char msg[50];
int numdata;
char inSerial[50];
int i=0;
boolean started=false;
boolean apned = false; 
boolean serverOK = false;
#define BUFF_SIZE 64
char buff[BUFF_SIZE+1];  // size of Serial buffer;

void setup() 
{
    xmlfixedheaderlength = sprintf(xmlheader,xmlformat1,ID,DD,Endian,T);
    pXML = &xmlheader[xmlfixedheaderlength];
    GPSSERIAL.begin(GPS_BAUD); 
  //Serial connection.
  Serial.begin(DEBUG_BAUD);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is recommended to use 4800 or slower.
  GSMSERIAL.begin(GSM_BAUD);
  
  delay(1000);
  GSMSERIAL.print("+++");  // may still be in data mode
  delay(1000);
  if (gsm.begin(GSM_BAUD))
  {
    Serial.println("\nstatus=READY");
 //   gsm.getIMEI(IMEIbuff);
//    IMEIbuff[16] = 0;
 //   Serial.println(IMEIbuff);
    started=true;  
  }
  else
    Serial.println("\nstatus=IDLE");
  if (started)
  {
#ifdef TRANSPARENT
    // Enter transparent mode
    gsm.SimpleWriteln("AT+CIPMODE=1");
    gsm.WaitResp(5000, 200, "OK");
    gsm.SimpleWriteln("AT+CIPCCFG=3,2,64,1,0,50,20");
    gsm.WaitResp(5000, 200, "OK");
#else
    gsm.SimpleWriteln("AT+CIPMODE=0");
#endif
    gsm.WaitResp(5000, 200, "OK");
    // connect to APN
    if (inet.attachGPRS(APN, "", ""))
    {
      Serial.println("status=ATTACHED");
      apned = true;
    }
    else
      Serial.println("status=ERROR");
    if (apned)
    {
        //Read IP address.
  //      gsm.SimpleWriteln("AT+CIFSR");
    //    delay(5000);
        //Read until serial buffer is empty.
        gsm.WhileSimpleRead();
        // connect to my server
#ifdef USE_TCP
        if (inet.connectTCP(ServerName,ServerPort) == 1)
#else
        if (inet.connectUDP(ServerName,ServerPort) == 1)
#endif
        {
          Serial.println("Connected to server");
          serverOK = true;
        }
        else
        {
          Serial.println("Not connected to server");
                    serverOK = true;
        }
    }
  }
};

int ServerSend(char *data,int length)
{
  char endb[2] = {0x1a,0};
  data[length] = 0; //add end of string
  gsm.SimpleWriteln("AT+CIPSEND");
  switch(gsm.WaitResp(5000, 200, ">"))
  {
    case RX_TMOUT_ERR: 
      return 1;
      break;
    case RX_FINISHED_STR_NOT_RECV: 
      return 2; 
      break;
  }
  gsm.SimpleWrite(data);
  gsm.SimpleWrite(endb);
  switch(gsm.WaitResp(15000, 200, "SEND OK"))
  {
    case RX_TMOUT_ERR: 
      return 3;
      break;
    case RX_FINISHED_STR_NOT_RECV: 
      return 4; 
      break;
  }
  return 0;
}
int totallength = 0;
char *sample = "abcdefghijklmnopqrstuvyxyz1234567890abcdefghijklmnopqrstuvyxyz\r\n";
void loop() 
{
  int count = 0;
  int linelength;
  int start,delta;
  if (GPSSERIAL.available() > 0)
  {
    start= millis();
    count = GPSSERIAL.readBytes(buff,BUFF_SIZE);
    linelength = sprintf(pXML,xmlformat2,count);
    linelength += xmlfixedheaderlength;  // total length of XML
    if (serverOK)
    {
#ifdef TRANSPARENT
      gsm.SimpleWrite(xmlheader);
      buff[count] = 0;
      gsm.SimpleWrite(buff);
#else
      ServerSend(xmlheader,linelength);
      ServerSend(buff,count);
#endif
    } 
    delta = millis()-start;
    Serial.println(delta);      
  } 
}
