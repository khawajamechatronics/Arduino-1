byte dnsbuffer[6];  // need to save this
DnsClass Dns;

// Return TRUE for success, false could be no DHCP or DNS
bool NetInit()
{
  Serial.println("getting ip...");
  int result = Dhcp.beginWithDHCP(macaddress);
  if(result == 1)
  {
    bool gotip;
   //ipAcquired = true;
    Serial.println("ip acquired...");
    Dhcp.getMacAddress(dnsbuffer);
    Serial.print("mac address: ");
    printArray(&Serial, ":", dnsbuffer, 6, 16);
    Serial.println();
    Dhcp.getLocalIp(dnsbuffer);
    Serial.print("ip address: ");
    printArray(&Serial, ".", dnsbuffer, 4, 10);
    Serial.println();
    Dhcp.getSubnetMask(dnsbuffer);
    Serial.print("subnet mask: ");
    printArray(&Serial, ".", dnsbuffer, 4, 10);
    Serial.println();
    Dhcp.getGatewayIp(dnsbuffer);
    Serial.print("gateway ip: ");
    printArray(&Serial, ".", dnsbuffer, 4, 10);
    Serial.println();
    Dhcp.getDhcpServerIp(dnsbuffer);
    Serial.print("dhcp server ip: ");
    printArray(&Serial, ".", dnsbuffer, 4, 10);
    Serial.println();
    Dhcp.getDnsServerIp(dnsbuffer);
    Serial.print("dns server ip: ");
    printArray(&Serial, ".", dnsbuffer, 4, 10);
    Serial.println();
  }
  return (result == 1); 
}

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

