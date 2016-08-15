#ifdef LOCAL_UDP
byte smcServer[6] = {192, 168, 1, 100,0,0};
#else
byte smcServer[6];
#endif
int SMCDataUdpPort = 11001;

extern unsigned long epoch;
extern char SharpBuffer[];
extern char OZBuffer[];
extern char IAQBuffer[];
extern bool IQ100OK, SharpOK, OZOK;  // only send legitimate data
extern byte macaddress[];

bool AirBaseClientInit()
{
  bool result;
#ifdef LOCAL_UDP
  result = true;
#else
  result = GetIPAddress("sensors.myairbase.com",smcServer);
#endif
  // open a socket for talking to server
  if (result == true)
  {
    SMCDataSocket.begin(4321);  //source port of 0 means one will be auto-picked
    Serial.print("SMC UDP Port open on: ");
    printArray(&Serial, ".", smcServer, 4, 10);
    Serial.print(":");
    Serial.println(SMCDataUdpPort);
    result = true;
  }
}
        
void AirBasePacketSend()
{
  char buf[256];
  SMCDataSocket.beginPacket(smcServer,SMCDataUdpPort);  //Start a new UDP packet for
  sprintf(buf,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<AirBaseSensor>\n<id>%d</id>\n<ver>%d</ver>\n",macaddress[5],VER);
  SMCDataSocket.write(buf);  
  sprintf(buf,"<time>%lu</time>\n",epoch);
  SMCDataSocket.write(buf);  
  if (IQ100OK == true)
  {
    sprintf(buf,"<iQ100>%s</iQ100>\n",IAQBuffer);
    SMCDataSocket.write(buf);  
  }
  else
  {
    sprintf(buf,"IQ100 error %s",IAQBuffer);
    Serial.println(buf);
    SMCDataSocket.write("<iQ100>Error</iQ100>\n");  
  }
  if (OZOK == true)
  {
    sprintf(buf,"<Oz47>%s</Oz47>\n",OZBuffer);
    SMCDataSocket.write(buf); 
  }
  else
  {
    sprintf(buf,"OZ error %s",OZBuffer);
    Serial.println(buf);
    SMCDataSocket.write("<Oz47>Error</Oz47>\n");  
  }
  if (SharpOK == true)
  { 
   sprintf(buf,"<sharp-Gp2y1010>%s</sharp-Gp2y1010>\n",SharpBuffer);
   SMCDataSocket.write(buf); 
  }
  else
  {
    sprintf(buf,"SharpError %s",SharpBuffer);
    Serial.println(buf);
    SMCDataSocket.write("<sharp-Gp2y1010>Error</sharp-Gp2y1010>\n");  
  }
  SMCDataSocket.write("</AirBaseSensor>\n"); 
  SMCDataSocket.endPacket();
}

