/*
UDP Echo client
*/
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

#include "utility/w5100.h"
#include "utility/socket.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
unsigned int localPort = 7;      // ECHO
  
// A UDP instance to let us send and receive packets over UDP
char printbuf[80];
EthernetUDP Udp;
EthernetClient tcpclient;
byte inPacketBuffer[20]; //buffer to hold incoming and outgoing packets 
char outPacketBuffer[] = "SDS432199";
void setup()
{
    Serial.begin(115200);
// start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print local IP address
  sprintf(printbuf,"IP %u.%u.%u.%u",Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3]);
  Serial.println(printbuf);
  Udp.begin(localPort);
}

void loop()
{
  int length,j,serverport;
  delay(1000);
  if ( Udp.parsePacket() )
  {
    // We've received a packet, read the data from it
    IPAddress remote = Udp.remoteIP();
    Serial.print("From ");
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    uint16_t port = Udp.remotePort();
    Serial.print(":");
    Serial.print(port);
    Serial.print(" Data ");
    length = Udp.read(inPacketBuffer,20);  // read the packet into the buffer
    for (j=0;j<length;j++)
    {
      Serial.print(inPacketBuffer[j]);
      Serial.print(",");
    }
    Serial.println();
    // build reply SDS432199
    Udp.beginPacket(Udp.remoteIP(),Udp.remotePort());
    Udp.write(outPacketBuffer);
    Udp.endPacket();
    // now connect to the TCP server
    serverport = 0;
    for (j=7;j<length;j++)
      serverport = (serverport*10) + inPacketBuffer[j] - '0';
    Serial.print("Connecting to ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(serverport);
    if (tcpclient.connect(Udp.remoteIP(),serverport))
      Serial.println("Client connected");
    else
      Serial.println("Client not connected");
  }
}
