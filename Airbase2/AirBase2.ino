#define NET_ACTIVE
#define IAQ_ACTIVE
#define SHARP_ACTIVE
#define OZ_ACTIVE
#define WATCHDOG_ACTIVE
#define WATCHDOG_BARK 180000    // time in millsec before reset
#include <icrmacros.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Udp.h>  
#ifdef WATCHDOG_ACTIVE
#include <avr/io.h>
#include <avr/wdt.h>
#endif

//#define LOCAL_UDP
#define NTP_TIMEOUT 43200 // 12 hours
#define LOOP_CYCLE_TIME 5  // do work every 5 secs
// pins 0, 1 UART
#define EthernetResetPin  2
#define SharpLedpin  3
#define IaqPin  4
#define LedJC2pin  5
#define LedJC3pin  6
#define LedGreenpin  7
#define OzPinTX 8
#define OzPinRX 9
bool LedJC2state = true;

// global data
#define VER 004
byte macaddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0b};
bool IQ100OK, SharpOK, OZOK;  // only send legitimate data
extern unsigned long epoch;
extern UDP OZControlSocket;
UDP SMCDataSocket;

// local data
unsigned long lastMillis;
unsigned long lastGoodSet;

int task;
bool taskOK;

void setup()  
{
  delay(1000);
#ifdef NET_ACTIVE
  pinMode(EthernetResetPin, INPUT); // ethernet reset     
#endif
  pinMode(LedJC2pin, OUTPUT);     
  pinMode(LedJC3pin, OUTPUT);     
  pinMode(LedGreenpin, OUTPUT); 
  Serial.begin(115200); 
 // initialize network elements 
#ifdef NET_ACTIVE
  while (NetInit() == false);  // loop until we get local IP address
  digitalWrite(LedGreenpin,HIGH);  // see we got IP address
  while (NTPInit() == false);  // loop until we contact time server
  while (AirBaseClientInit() == false); // loop until we contact airbase server
  digitalWrite(LedJC3pin,HIGH);  // see we server addresses
  //while (NTPenquiry() == false);  // loop until we get TOD
#endif
// Initialize sensors
#ifdef SHARP_ACTIVE
  SharpInit();  
#endif
#ifdef OZ_ACTIVE
  OzInit();
#endif
#ifdef IAQ_ACTIVE
  IAQInit();
#endif
#ifdef WATCHDOG_ACTIVE
  lastGoodSet = millis();
  wdt_enable(WDTO_8S);
#endif
}

void loop()
{
#if defined(NET_ACTIVE) && defined(OZ_ACTIVE)
  // check if server tried to config this node
  //if (OZControlSocket.available())
  //{
  //}
#endif
  // Action every 5 seconds
  if ((millis() - lastMillis)/1000 >= LOOP_CYCLE_TIME )
  {
    epoch+= (millis()-lastMillis)/1000;
    lastMillis = millis();
#ifdef NET_ACTIVE
    // refresh time from NTP server
    unsigned long ElapsedTime = (millis()/1000)%NTP_TIMEOUT;
 //   Serial.println(ElapsedTime);
    // do once at beginning of new timeout period
    if ( ElapsedTime < LOOP_CYCLE_TIME) 
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
      case 0:
#ifdef SHARP_ACTIVE
        SharpRead();
#endif
      break;
      case 1:
        IQ100OK = false;
#ifdef IAQ_ACTIVE
        count = 10;
        while (IQ100OK == false && count-- > 0)    
          IAQRead();
#endif
        break;
      case 2:
        OZOK = false;
#ifdef OZ_ACTIVE
        count = 10;
        while (OZOK == false && count-- > 0)
          OZRead();
#endif
      break;
      case 3:
        PrintMessages();
#ifdef NET_ACTIVE
        AirBasePacketSend();
        digitalWrite(LedJC3pin,LOW);  // turn off
        digitalWrite(LedGreenpin,LOW);  // turn off
#endif
        break;        
    }
    taskOK = true;
#ifdef WATCHDOG_ACTIVE
    // Watchdog set for 8 secs, we come here every 5 secs
    // Record when we last had a good set of data
    if (IQ100OK == true && OZOK == true)
      lastGoodSet = millis();
    // if < 3 minutes since last good set, reset watchdog
    if ((millis() - lastGoodSet) <= WATCHDOG_BARK)
      wdt_reset();
#endif
  }
}
