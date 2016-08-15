int OZControlUdpPort = 6655;
char OZBuffer[64];
UDP OZControlSocket;
SoftwareSerial ozSerial(OzPinTX, OzPinRX);
//extern byte smcServer[];

void OzInit()
{
#ifdef NET_ACTIVE
  Serial.print("OZ Control UDP Port open on: ");
  // Open UDP Socket
  OZControlSocket.begin(5566);  //source port of 0 means one will be auto-picked
  OZControlSocket.beginPacket(smcServer,OZControlUdpPort);  //Start a new UDP packet for
  Serial.println(OZControlUdpPort);
#endif
  ozSerial.begin(19200);
  ozSerial.print("{S}");
}

void OZRead()
{
  ozSerial.listen();
  ozSerial.flush();
  uint8_t idx = 0;
  int8_t sindex = -1;  // {
  int8_t eindex = -1; // }
  ozSerial.print("{M}");
  delay(100);
  char c;
  while (ozSerial.available() && idx<sizeof(OZBuffer)) 
  {
    c = ozSerial.read();
    if (c == '{')
      sindex = idx;
    if (c == '}')
      eindex = idx;
    OZBuffer[idx++] = c;
  }
  // truncate past } if any
  if (eindex >= 0)  
    OZBuffer[eindex+1] = '\0';
  else
    OZBuffer[idx] = '\0';
  // shift left so that {, if any, is 1st character
  if (sindex >= 0)
    for (int i=sindex; i<idx; i++)
      OZBuffer[i-sindex] =OZBuffer[i];
  // sanity check
  if (eindex >= 0 && sindex >= 0)
    OZOK = true;
}

