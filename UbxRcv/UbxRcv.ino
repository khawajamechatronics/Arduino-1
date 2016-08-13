/*
 * Read incoming data, identify UBX header 0xB2 0x62 and toggle led

 */

//#define MEGA

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

byte buff[64];  // size of Serial buffer;
// WIFI connection
// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed but DI

#define WLAN_SSID       "Henry"        // cannot be longer than 32 characters!
#define WLAN_PASS       "9876543210"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

//
//  UDP server stuff
//
char *serverName = "david-henry.dyndns.tv";
#define SERVER_IP              10, 0, 0, 8    // Logging server IP address.  Note that this
                                                   // should be separated with commas and NOT periods!
#define SERVER_PORT            8000                // Logging server listening port.
uint32_t ip;
unsigned long lastSend = 0;
Adafruit_CC3000_Client server;

void setup() {
  // GPS set to UBX, 38400 baud
  Serial.begin(9600);
#ifdef MEGA
  Serial1.begin(9600);
#endif
  if (!cc3000.begin(0,false,"WiFiShield"))
  {
#ifdef MEGA
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
#endif
    while(1);
  }
#ifdef MEGA
  /* Delete any old connection data on the module */
  Serial.println(F("\nDeleting old connection profiles"));
#endif
if (!cc3000.deleteProfiles()) {
#ifdef MEGA
    Serial.println(F("Failed!"));
#endif
    while(1);
  }
  /* Attempt to connect to an access point */
  char *ssid = WLAN_SSID;             /* Max 32 chars */
#ifdef MEGA
  Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);
#endif
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
#ifdef MEGA
    Serial.println(F("Failed!"));
#endif
    while(1);
  }
  /* Wait for DHCP to complete */
#ifdef MEGA
  Serial.println(F("Request DHCP"));
#endif
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }
   
#ifdef MEGA
  Serial.println(F("Connected!"));
#endif
/* now connect to UDP server */
  server = cc3000.connectUDP(ip, SERVER_PORT);
  delay(1000);

}
void loop() {
  int n = Serial.readBytes(buff,64);
#ifdef MEGA
  Serial.write(buff,n);
#endif
  if (server.connected())
    server.write(buff,n);
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
#ifdef MEGA
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
#endif
    return false;
  }
  else
  {
#ifdef MEGA
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
#endif
    return true;
  }
}

