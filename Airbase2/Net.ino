#include <Dns.h>  
byte dnsbuffer[6];  // need to save this
DNSClient Dns;

// Return TRUE for success, false could be no DHCP or DNS
bool NetInit()
{
  Serial.print("getting ip for ");
  printArray(&Serial,".",macaddress,6,16);
  Serial.println();
  int result = Ethernet.begin(macaddress);
  if(result != 0)
  {
    bool gotip;
   //ipAcquired = true;
    Serial.println("ip acquired...");
    Serial.print("ip address: ");
    printIP(Ethernet.localIP());
    Serial.print("subnet mask: ");
    printIP(Ethernet.subnetMask());
    Serial.print("gateway ip: ");
    printIP(Ethernet.gatewayIP());
    Serial.print("dns server ip: ");
    printIP(Ethernet.dnsServerIP());
  }
  return (result == 1); 
}

bool GetIPAddress(char * name, byte *address)
{
  Dns.begin(Ethernet.dnsServerIP());
  IPAddress ip;
  int result = Dns.getHostByName(name, ip);
  if (result == 1)
  {
    address[0] = (ip) & 0xFF;
    address[1] = (ip>>8) & 0xFF;
    address[2] = (ip>>16) & 0xFF;
    address[3] = (ip>>24) & 0xFF;
    return true;
  }
  else
    return false;
}


/*
bool GetIPAddress(char * name, byte *address)
{
  int result;
  Dns.init(name, dnsbuffer);  //Buffer contains the IP address of the DNS server
  Dns.resolve();   
  while(!(result=Dns.finished())) ;  //wait for DNS to resolve the name
  if(result != 1)
  {
    Serial.print("DNS Error code: ");
    Serial.println(result,DEC);
  }
  else
  {
    Dns.getIP(address);
    Serial.print(name);
    Serial.print(" address: ");
    printArray(&Serial, ".", address, 4, 10);
    Serial.println();
  }
  return (result==1);
}
*/
