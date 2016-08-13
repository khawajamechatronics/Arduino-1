/*
  UDPSendReceive.pde:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and received messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
unsigned int localPort = 8888;      // local port to listen on
// buffers for receiving and sending data
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
IPAddress ServerIP(10, 0, 0, 8);
int ServerPort = 8000;
char *data = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
void setup() {
  // start the Ethernet and UDP:
  Serial.begin(115200);
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print your local IP address:
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
}

void loop() {
    Udp.beginPacket(ServerIP, ServerPort);
    Udp.println(data);
    Serial.println(data);
    data[0]++;
    if (data[0] > 0x7f)
      data[0] = 0x20;
    Udp.endPacket();
    delay(100);
}

