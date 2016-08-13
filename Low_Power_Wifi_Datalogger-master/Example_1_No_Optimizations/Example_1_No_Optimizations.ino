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
/* 
 modified for persistent connection
 added seriel number
 delay in millsec
 95 byte message (MID41)
*/
//#define CC3000_TINY_DRIVER
#include <Adafruit_CC3000.h>
#include <SPI.h>

// CC3000 configuration.
#define ADAFRUIT_CC3000_IRQ    3
#define ADAFRUIT_CC3000_VBAT   5
#define ADAFRUIT_CC3000_CS     10

// Wifi network configuration.
#define WLAN_SSID              "Henry"
#define WLAN_PASS              "9876543210"
#define WLAN_SECURITY          WLAN_SEC_WPA2
char *serverName = "david-henry.dyndns.tv";
int serialnumber = 0;
// Data logging configuration.
#define LOGGING_FREQ_SECONDS   1       // Seconds to wait before a new sensor reading is logged.
#define LOGGING_FREQ_MSECONDS   200       // milliSeconds to wait before a new sensor reading is logged.

#define SENSOR_PIN             4        // Analog pin to read sensor values from (for example
                                        // from a photocell or other resistive sensor).

#define SERVER_IP              10, 0, 0, 8    // Logging server IP address.  Note that this
                                                   // should be separated with commas and NOT periods!

#define SERVER_PORT            8000                // Logging server listening port.

// Internal state used by the sketch.
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT);
uint32_t ip;
unsigned long lastSend = 0;
Adafruit_CC3000_Client server;
char MID41[80];  // known problem in library for packets > 95 bytes

// Take a sensor reading and send it to the server.
void logSensorReading() {
  // Take a sensor reading
//  int reading = analogRead(SENSOR_PIN);
  // Connect to the server and send the reading.
 // Serial.print(F("> ")); Serial.println(reading, DEC);
//  Adafruit_CC3000_Client server = cc3000.connectTCP(ip, SERVER_PORT);
  if (server.connected()) {
#if 0
    server.print(serialnumber++);
    server.print(",");
    server.println(reading);
#else
  Serial.println(serialnumber);
    server.print(serialnumber++);
    server.print(",");
    server.write(MID41,sizeof(MID41));
#endif
}
  else {
    Serial.println(F("Error sending measurement!"));
  }

  // Wait a small period of time before closing the connection
  // so the message is sent to the server.
  delay(100);
  
  // Close the connection to the server.
//  server.close();
}

void setup(void)
{  
  Serial.begin(9600);
  
  // Initialize the CC3000.
  Serial.println(F("\nInitializing CC3000..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }

  // Connect to AP.
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  Serial.println(F("Connected!"));
  
  // Wait for DHCP to be complete.
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
  // get IP address of remote server
  if  (!  cc3000.getHostByName(serverName, &ip))
  {
      Serial.println(F("Couldn't resolve!"));
  }
  else
  {
      Serial.print(serverName);Serial.print(": ");
      cc3000.printIPdotsRev(ip);
      Serial.println("");
  }
  // Store the IP of the server.
  ip = cc3000.IP2U32(SERVER_IP);
 
  Serial.println(F("Setup complete."));
  // persistent server
//  server = cc3000.connectTCP(ip, SERVER_PORT);
  memset(MID41,'A',sizeof(MID41));
  MID41[sizeof(MID41)-2] = 0x0d;
  MID41[sizeof(MID41)-1] = 0x0a;
  server = cc3000.connectUDP(ip, SERVER_PORT);
  delay(1000);
}

void loop(void)
{
  unsigned long time = millis();
//  if (time - lastSend >= (1000 * (unsigned long)LOGGING_FREQ_SECONDS)) {
  if (time - lastSend >= LOGGING_FREQ_MSECONDS) {
    logSensorReading();
    lastSend = time;
  }
}

