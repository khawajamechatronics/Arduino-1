#define USE_NET
#define USE_OZ
#define USE_IAQ
#define USE_SHARP

#include <NewSoftSerial.h>
#include <AClient.h>
#include <AEthernet.h>
#include <AServer.h>
#include <ASocket.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ntp.h>

#define LOCAL_UDP

int LedJC2pin = 5;
int LedJC3pin = 6;
int LedGreenpin = 7;
bool LedJC2state = true;

// global data
#define VER 001
byte macaddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0E};
bool IQ100OK, SharpOK, OZOK;  // only send legitimate data
extern unsigned long epoch;
extern ASocket OZControlSocket;
// local data
unsigned long lastMillis;
int task;
bool taskOK;

void setup()  
{
  pinMode(LedJC2pin, OUTPUT);     
  pinMode(LedJC3pin, OUTPUT);     
  pinMode(LedGreenpin, OUTPUT); 
  Serial.begin(115200); 
 // initialize network elements 
#ifdef USE_NET
  while (NetInit() == false);  // loop until we get local IP address
  while (NTPInit() == false);  // loop until we contact time server
  while (AirBaseClientInit() == false); // loop until we contact airbase server
  while (NTPenquiry() == false);  // loop until we get TOD
#endif
// Initialize sensors
#ifdef USE_SHARP
  SharpInit();  
#endif
#ifdef USE_OZ
  OzInit();
#endif
#ifdef USE_IAQ
  IAQInit();
#endif
}

void loop()
{
#ifdef USE_NET
  // check if server tried to config this node
  if (OZControlSocket.available())
  {
  }
#endif
  // Action every 5 seconds
  if ((millis() - lastMillis)/1000 >= 5 )
  {
    lastMillis = millis();
    epoch+= (millis()-lastMillis)/1000;
#ifdef USE_NET
  // refresh TOD every 12 hours
    // refer to time server every 12 hours
    unsigned long secsper12hours = 43200L; //12 * 60 * 60;
    unsigned long ElapsedTime = (lastMillis/1000) / secsper12hours;
    ElapsedTime *= secsper12hours;
    ElapsedTime = (lastMillis/1000) - ElapsedTime;
 //   Serial.println(ElapsedTime);
    if ( ElapsedTime < 5) 
      NTPenquiry(); 
#endif
    LedJC2state = !LedJC2state;
    digitalWrite(LedJC2pin,LedJC2state);  // toggle pin to show activity
    task++;
    task = task % 4;
    taskOK = false;
  }
  if (!taskOK)
  {
    switch (task)
    {
      int count;
      case 1:
        SharpOK = true;
#ifdef USE_SHARP
        SharpRead();
#endif
        break;
      case 0:
        IQ100OK = false;
#ifdef USE_IAQ
        count = 10;
        while (IQ100OK == false && count-- > 0)    
          IAQRead();
#endif
        break;
      case 2:
        OZOK = false;
#ifdef USE_OZ
        count = 10;
        while (OZOK == false && count-- > 0)
          OZRead();
#endif
        break;
      case 3:
        PrintMessages();
#ifdef USE_NET
        AirBasePacketSend();
#endif
        break;        
    }
    taskOK = true;
  }}
