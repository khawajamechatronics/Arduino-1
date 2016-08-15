#include <Time.h>

#include <MsTimer2.h>
#include <AEthernet.h>
#include "Dhcp.h"
#include "Dns.h"
#include "Ntp.h"

#include <string.h>

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
boolean ipAcquired = false;
boolean ntpacquired = false;
DnsClass Dns;
NtpClass Ntp;

byte ntpServer[6];

// Switch on LED on pin 13 each second

unsigned long epoch;
void flash() {
  static boolean output = HIGH;
  digitalWrite(13, output);
  output = !output;
  epoch++;
  //Serial.println(epoch);

  int d = day(epoch);
  int m = month(epoch);
  int y = year(epoch);
  int h = hour(epoch);
  int mn = minute(epoch);
  int s = second(epoch);
  Serial.print(d);
  Serial.print("/");
  Serial.print(m);
  Serial.print("/");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(h);
  Serial.print(":");
  Serial.print(mn);
  Serial.print(":");
  Serial.println(s);

}
void printArray(Print *output, char* delimeter, byte* data, int len, int base)
{
  char buf[10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0  };

  for(int i = 0; i < len; i++)
  {
    if(i != 0)
      output->print(delimeter);

    output->print(itoa(data[i], buf, base));
  }

  output->println();
}

void setup()
{
  int result = Dhcp.beginWithDHCP(mac);
  byte buffer[6];
  Serial.begin(9600);

  if(result == 1)
  {
    ipAcquired = true;
    Serial.println("ip acquired...");
    Dhcp.getMacAddress(buffer);
    Serial.print("mac address: ");
    printArray(&Serial, ":", buffer, 6, 16);

    Dhcp.getLocalIp(buffer);
    Serial.print("ip address: ");
    printArray(&Serial, ".", buffer, 4, 10);

    Dhcp.getSubnetMask(buffer);
    Serial.print("subnet mask: ");
    printArray(&Serial, ".", buffer, 4, 10);

    Dhcp.getGatewayIp(buffer);
    Serial.print("gateway ip: ");
    printArray(&Serial, ".", buffer, 4, 10);

    Dhcp.getDhcpServerIp(buffer);
    Serial.print("dhcp server ip: ");
    printArray(&Serial, ".", buffer, 4, 10);

    Dhcp.getDnsServerIp(buffer);
    Serial.print("dns server ip: ");
    printArray(&Serial, ".", buffer, 4, 10);
  }
  else
  {
    Serial.println("No IP address");
  }

  while (ntpacquired == false)
  { 
    // Do DNS Lookup for the NTP server    
    Dns.init("pool.ntp.org", buffer);  //Buffer contains the IP address of the DNS server
    Dns.resolve();   

    while(!(result=Dns.finished())) ;  //wait for DNS to resolve the name
    if(result != 1)
    {
      Serial.print("DNS Error code: ");
      Serial.println(result,DEC);
    }    
    else
    {
      Dns.getIP(ntpServer);  //buffer now contains the IP address for google.com
      Serial.print("NTP Server IP address: ");
      printArray(&Serial, ".", ntpServer, 4, 10);
      result = COMPLETE_OK + 1;
      while (result != COMPLETE_OK)
      {
        // Do NTP enquiry
        Ntp.init(ntpServer);
        Ntp.request();

        while(!(result=Ntp.finished())); //wait for NTP to complete

        if (result == COMPLETE_OK)
        {
          //get the time data
          Ntp.getTimestamp(&epoch);

          //print the number of seconds since Jan 1 1970 (Unix Epoch)
          Serial.println();
          Serial.print("Seconds since Jan 1 1970: ");
          Serial.println(epoch);
          ntpacquired = true;
        }
        else 
        {
          switch(result)
          {
          case TIMEOUT_EXPIRED:
            Serial.println("Timeout");
            break;
          case NO_DATA:
            Serial.println("No Data");
            break;
          case COMPLETE_FAIL:
            Serial.println("Failed");
            break;
          }
        }
      }
    }
  }

  pinMode(13, OUTPUT);

  MsTimer2::set(1000, flash); // 500ms period
  MsTimer2::start();
}

void loop()
{
}

