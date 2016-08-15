/*************************************************** 
  CC3000 Low Power Datalogging
  
  Example 1: No Low Power Optimizations
  
  Created by Tony DiCola (tony@tonydicola.com)

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Based on CC3000 examples written by Limor Fried & Kevin Townsend 
  for Adafruit Industries and released under a BSD license.
  All text above must be included in any redistribution.
  
 ****************************************************/
#define HAVE_SERIAL1  // use Serial for debugger, Serial1 for GPS
//#define CC3000_TINY_DRIVER
#include <Adafruit_CC3000.h>
#include <SPI.h>

// toggle led when PPS rises
#define PPS_PIN 2
#define LED_PIN 13
int ledstate = 0;

// CC3000 configuration.
#define ADAFRUIT_CC3000_IRQ    3
#define ADAFRUIT_CC3000_VBAT   5
#define ADAFRUIT_CC3000_CS     10

#if 1
#define WLAN_SSID              "Henry"
//#define WLAN_SSID              "HenryAP"
#define SERVER_IP              10, 0, 0, 8    // Logging server IP address.  Note commas
#else
#define WLAN_SSID              "HenryMobile"
#define SERVER_IP              79, 181, 153, 193 
#endif

// Wifi network configuration.
#define WLAN_PASS              "9876543210"
#define WLAN_SECURITY          WLAN_SEC_WPA2
char *serverName = "david-henry.dyndns.tv";

const char *xmlformat1="<C I=\"%u\",D=\"%u\",E=\"%c\",T=\"%s\"";  // only needs to be done once
const char *xmlformat2=",L=\"%u\" />";  // do this for each block
char *pXML;

unsigned ID=237,DD=23;
const char Endian='T';  // text, neither big nor little
const char *T="NMEA";
char xmlheader[80];
unsigned xmlfixedheaderlength;

#define SERVER_PORT            8000                // Logging server listening port.

// Internal state used by the sketch.
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT);
uint32_t ip;
Adafruit_CC3000_Client server;
byte buff[64];  // size of Serial buffer;

void blink()
{
  ledstate = !ledstate;
}

void setup(void)
{  
  xmlfixedheaderlength = sprintf(xmlheader,xmlformat1,ID,DD,Endian,T);
  pXML = &xmlheader[xmlfixedheaderlength];
  pinMode(PPS_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), blink, RISING);
#ifdef HAVE_SERIAL1
  Serial.begin(115200);  // debug
  Serial1.begin(9600);  // gps
  Serial.print("CC3300 Init ");
#else
  Serial.begin(9600); // gps
#endif
if (!cc3000.begin(0,false,"WiFiShield"))
  {
#ifdef HAVE_SERIAL1
    Serial.println("fail");
#endif
    while(1);
  }
#ifdef HAVE_SERIAL1
  else
    Serial.println("pass");
#endif

  /* Optional: Get the SSID list (not available in 'tiny' mode) */
#ifndef CC3000_TINY_DRIVER
  #ifdef HAVE_SERIAL1
  listSSIDResults();
  #endif
#endif
  
#if 0
/* Delete any old connection data on the module */
#ifdef HAVE_SERIAL1
  Serial.println(F("\nDeleting old connection profiles"));
#endif
  if (!cc3000.deleteProfiles()) {
#ifdef HAVE_SERIAL1
    Serial.println(F("Failed!"));
#endif
    while(1);
  }
#endif
  
  // Connect to AP.
#ifdef HAVE_SERIAL1
  Serial.print("CC3300 connecting to ");Serial.print(WLAN_SSID);
#endif
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    while(1);
#ifdef HAVE_SERIAL1
    Serial.println(" fail");
#endif
  }
#ifdef HAVE_SERIAL1
  else
    Serial.println(" pass");
#endif


// Wait for DHCP to be complete.
#ifdef HAVE_SERIAL1
  Serial.println("Get DHCP");
#endif
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
#ifdef HAVE_SERIAL1
  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }
#endif

  // get IP address of remote server
  if  (cc3000.getHostByName(serverName, &ip) == 0)
  {
#ifdef HAVE_SERIAL1
    Serial.print("Could not resolve ");Serial.println(serverName);
#endif
    while (1);
  }
#ifdef HAVE_SERIAL1
  else
  {
    Serial.print(serverName);Serial.print(" at ");cc3000.printIPdotsRev(ip);Serial.println();
  }
#endif
// Store the IP of the server.
  if (ip == 0)
    ip = cc3000.IP2U32(SERVER_IP);
  // persistent server
#ifdef HAVE_SERIAL1
  Serial.print("Connect to UDP server at ");cc3000.printIPdotsRev(ip);Serial.println();
#endif
  server = cc3000.connectUDP(ip, SERVER_PORT);
  delay(1000);
}

void loop(void)
{
  int count = 0;
  int ll;
  digitalWrite(LED_PIN, ledstate);
#ifdef HAVE_SERIAL1
  if (Serial1.available() > 0)
#else
  if (Serial.available() > 0)
#endif
  {
#ifdef HAVE_SERIAL1
    count = Serial1.readBytes(buff,sizeof(buff));
#else
    count = Serial.readBytes(buff,sizeof(buff));
#endif
    ll = sprintf(pXML,xmlformat2,count);
    ll += xmlfixedheaderlength;  // total length of XML
    if (server.connected())
    {
      server.write(xmlheader,ll);
      server.write(buff,count);
    }
#ifdef HAVE_SERIAL1
    Serial.write(xmlheader,ll);
    Serial.write(buff,count);
#endif
}
}
#ifdef HAVE_SERIAL1
/*********************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint32_t index;
  uint8_t valid, rssi, sec;
  char ssidname[33]; 

  if (!cc3000.startSSIDscan(&index)) {
    Serial.println(F("SSID scan failed!"));
    return;
  }

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}
#endif

