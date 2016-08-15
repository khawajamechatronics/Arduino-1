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
#include "icmp.h"
#include "utility.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
SOCKET _sock;
bool havesock,done;
char printbuf[80];

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");
  havesock = false;
  done = false;
  Ethernet.begin(mac);  // ip address will be obtained from DHCP
  sprintf(printbuf,"IP %u.%u.%u.%u",Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3]);
  Serial.println(printbuf);
  // open up a raw socket
  for (int i = 0; i < MAX_SOCK_NUM; i++)
  {
    uint8_t s = W5100.readSnSR(i);
    if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
      _sock = i;
      break;
    }
  }
  Serial.print("Socket number ");
  Serial.println(_sock);
  // if we have a valid socket, carry on
  if (_sock != MAX_SOCK_NUM)
  {
    // ICMP doesnt listen on a port but receives all messages
    if (socket(_sock,SnMR::IPRAW,0,0) == 1)
    {
      W5100.execCmdSn(_sock, Sock_CLOSE);
      W5100.writeSnIR(_sock, 0xFF);
      W5100.writeSnPROTO(_sock, IPPROTO::ICMP);
      W5100.writeSnPORT(_sock, 0);
      W5100.execCmdSn(_sock, Sock_OPEN);
      havesock = true;
    }
  }
  if (havesock)
    Serial.println("Have socket");
  else
    Serial.println("Dont have socket");
}

void checksum(struct eEchoReply *er)
{
  er->header.checksum = 0;
  int nleft = sizeof(struct eEchoReply);
  unsigned long sum = 0;
  uint16_t *w = (uint16_t *)er;
  while(nleft > 1)  
  {
    sum += *w++;
    nleft -= 2;
  }
  if(nleft)
  {
    uint16_t u = 0;
    *(uint8_t *)(&u) = *(uint8_t *)w;
    sum += u;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  er->header.checksum = ~sum;
}
void loop()
{
  uint16_t sz,temp16;
  uint8_t buffer;
  struct sIPHeader header;	// IP header i.e. who sent it
  struct eEchoReply echoreply;
  int length;
  if (havesock )
  {
    // wait for a packet to come in
    sz = W5100.getRXReceivedSize(_sock);
    if (sz != 0)
    {
      Serial.println(sz);
      // get the read pointer
      buffer = W5100.readSnRX_RD(_sock);
      // read the IP header
      W5100.read_data(_sock, (uint8_t *)buffer, (uint8_t *)&header, sizeof(struct sIPHeader));
      // swap endianness
      temp16 = swapb((uint8_t *)&header.length);
      sprintf(printbuf,"IP %u.%u.%u.%u Length %u",header.ip[0],header.ip[1],header.ip[2],header.ip[3],temp16);
      Serial.println(printbuf);
      length = temp16;
      buffer += sizeof(struct sIPHeader);
      // read the ICMP message
      W5100.read_data(_sock, (uint8_t *)buffer, (uint8_t *)&echoreply.header, sizeof(struct sICMP));
      temp16 = swapb((uint8_t *)echoreply.header.checksum);
      sprintf(printbuf,"ICMP type %u code %u checksum %u ",echoreply.header.type, echoreply.header.code,temp16);
      Serial.println(printbuf);
      if (echoreply.header.type == ECHO_REQUEST && echoreply.header.code == 0)
      {
        // do checksum if you're feeling pedantic
        temp16 = swapb((uint8_t *)&echoreply.header.remainder.echo.id);
        sprintf(printbuf,"ID %u ",temp16);
        Serial.print(printbuf);      
        temp16 = swapb((uint8_t *)&echoreply.header.remainder.echo.seq);
        sprintf(printbuf,"Seq %u",temp16);
        Serial.println(printbuf);      
      }
      buffer += sizeof(struct sICMP);
      // calculate size of data section
      length -= sizeof(struct sICMP);
      Serial.print("DATA ");
      for (int j=0; j< length; j++)
      {
        W5100.read_data(_sock, (uint8_t *)buffer++, &echoreply.data[j], 1);
        sprintf(printbuf,"%02X,",echoreply.data[j]);
        Serial.print(printbuf);
      }
      Serial.println();
      // send reply back
      // change type
      echoreply.header.type = ECHO_REPLY;
      // change data
      echoreply.data[0] = 'S';
      echoreply.data[1] = 'D';
      echoreply.data[2] = 'S';
      echoreply.data[3] = 9;
      // redo checksum
      checksum(&echoreply);
      W5100.writeSnDIPR(_sock, header.ip);
      W5100.writeSnDPORT(_sock, 0);
      W5100.send_data_processing(_sock, (uint8_t *)&echoreply, sizeof(struct eEchoReply));
      W5100.execCmdSn(_sock, Sock_SEND);
      while ((W5100.readSnIR(_sock) & SnIR::SEND_OK) != SnIR::SEND_OK) 
      {
        if (W5100.readSnIR(_sock) & SnIR::TIMEOUT)
        {
          W5100.writeSnIR(_sock, (SnIR::SEND_OK | SnIR::TIMEOUT));
        }
      }
      W5100.writeSnIR(_sock, SnIR::SEND_OK);
      done = true;
    }
  }
  else
    Serial.println("No socket");
}
