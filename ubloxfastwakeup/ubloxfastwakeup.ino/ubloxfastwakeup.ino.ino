/*
 * Get time from NTP via ethernet
 * Create UBX-MGA-INI-TIME_UTC message and transmit to uBlox via SoftwareSerial
 * GPS lock is decided by analysing incoming GPRMC for validation field changing from V to A
 */
 
#include <TimeLib.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <string.h>

#define RED_LED 9
#define YELLOW_LED 8
// UNO uses pins 11,12,13 for SPI, only 2,3 have interrupt capability
SoftwareSerial mySerial(2, 3); // RX, TX
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
// NTP Servers:
char timeServer[] = "time.nist.gov"; // time-a.timefreq.bldrdoc.gov
const int timeZone = 0;     // UTC
//const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)
bool sentUTC = false;


EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

#pragma pack(1)
struct sUBX_MGA_INI_TIME_UTC_payload {
  uint8_t type = 0x10;
  uint8_t r1 = 0;
  uint8_t ref = 0;
  uint8_t leapsec= -128;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t r2;
  uint32_t ns = 0;
  uint16_t taccs = 0;
  uint16_t r3;
  uint32_t taccns = 0;
};
struct sUBX_MGA_INI_TIME_UTC
{
  uint8_t header0 = 0xb5;
  uint8_t header1 = 0x62;
  uint8_t classx = 0x13;
  uint8_t id = 0x40;
  uint16_t length = sizeof(struct sUBX_MGA_INI_TIME_UTC_payload);
  struct sUBX_MGA_INI_TIME_UTC_payload payload;
  uint8_t cka;
  uint8_t ckb;
} ubxmessage;
#pragma pack(0)

// adapted from ubloxM8 receiver description UBX checksum
#define CK_A Buffer[length-2]
#define CK_B Buffer[length-1]
void UBXchecksum(uint8_t *Buffer,int length)
{
  CK_A = 0;
  CK_B = 0;
  for (int I = 2; I< length-2;I++)
  {
    CK_A = CK_A + Buffer[I];
    CK_B = CK_B + CK_A;
  }
}

char NMEAbuffer[100];
int NMEAindex = 0;
bool NMEAstarted = false;
bool ValidityCheck(char c)
{
  bool rc = false;
  if (NMEAstarted)
  {
    // if reached end of sentence, start again
    if (c == '*' || c == 0x0d || c == 0x0a)
    {
      NMEAstarted = false;
      digitalWrite(RED_LED, LOW);
    }
    else
    {
      // add char to sentence
      NMEAbuffer[NMEAindex] = c;
      if (NMEAindex >= 5 && strncmp(NMEAbuffer,"$GPRMC",6) == 0)
      {
        // we have correct header, now check the reset
        // $GPRMC,tttttt.00,A
        rc = (NMEAindex == 17 && c == 'A');
      }
      NMEAindex++;
    }
  }
  else
  {
    if (c == '$')
    {
      NMEAstarted = true;
      NMEAindex = 1;
      NMEAbuffer[0] = c;
      digitalWrite(RED_LED, HIGH); 
    }
  }
  return rc;
}
void setup() 
{
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH); 
  pinMode(YELLOW_LED, OUTPUT);
  digitalWrite(YELLOW_LED, LOW); 
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  Serial.println("TimeNTP Example");
  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    while (1) {
      Serial.println("Failed to configure Ethernet using DHCP");
      delay(10000);
    }
  }
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
//  mySerial.println("Hello, world?");
}

time_t prevDisplay = 0; // when the digital clock was displayed
unsigned long fractionalSec;

void loop()
{  
  char c;
  if (timeStatus() != timeNotSet) 
  {
    if (!sentUTC)
    {
      sentUTC = true;
      digitalWrite(YELLOW_LED,HIGH);
      digitalWrite(RED_LED,LOW);
      digitalClockDisplay(); 
      ubxmessage.payload.year = year();
      ubxmessage.payload.month = month();
      ubxmessage.payload.day = day();
      ubxmessage.payload.hour = hour();
      ubxmessage.payload.minute = minute();
      ubxmessage.payload.second = second();
      ubxmessage.payload.ns = fractionalSec;
      UBXchecksum((uint8_t *)&ubxmessage, sizeof(ubxmessage));
      mySerial.write((uint8_t *)&ubxmessage, sizeof(ubxmessage));
    }
  }
  if (mySerial.available())
  {
    c = mySerial.read();
    if (ValidityCheck(c))
       digitalWrite(YELLOW_LED, LOW); 
    Serial.write(c);
  }
  if (Serial.available())
    mySerial.write(Serial.read());
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      // convert four bytes starting at location 40 to a long integer
      fractionalSec =  (unsigned long)packetBuffer[44] << 24;
      fractionalSec |= (unsigned long)packetBuffer[45] << 16;
      fractionalSec |= (unsigned long)packetBuffer[46] << 8;
      fractionalSec |= (unsigned long)packetBuffer[47];
 //     digitalWrite(YELLOW_LED,HIGH);
   //   digitalWrite(RED_LED,LOW);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
 // digitalWrite(YELLOW_LED,LOW);
  //digitalWrite(RED_LED,HIGH);
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
//void sendNTPpacket(IPAddress &address)
void sendNTPpacket(char* address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

