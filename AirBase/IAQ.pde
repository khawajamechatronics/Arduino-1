NewSoftSerial iaqSerial(4, -1,true);
char IAQBuffer[128];
char pbuff[10];

void IAQInit()
{
  Serial.println("IAQ init");
  iaqSerial.begin(9600);
}

void IAQRead()
{
  int spacecount = 0;
  iaqSerial.flush();
  uint8_t idx = 0;
  uint8_t i;
  bool alldec = true;
  char c;
  do
  {
    while (iaqSerial.available() == 0) ; // wait for a char this causes the blocking
    c = iaqSerial.read();
    sprintf(pbuff,"%02X",c);
    Serial.print(pbuff);
    IAQBuffer[idx++] = c;
  }
  while (c != '\n' && c != '\r' && idx<127); 
  Serial.println();
  IAQBuffer[idx] = 0;
  Serial.println(IAQBuffer);
    IAQBuffer[idx-2] = '\0';
  delay(100);
  // sanity check on data, must be 10 numeric fields;
  for (i = 0; i< idx-2; i++)
  {
    if (IAQBuffer[i] == ' ' || IAQBuffer[i] == '\t')
      spacecount++;
    else if (IAQBuffer[i] < '0' || IAQBuffer[i] > '9')
      alldec = false;
  }
  // only check spaces if all decimal
  if (alldec == true)
  {
    if (spacecount == 9)
      IQ100OK = true;
    else
    {
      Serial.print("spaces ");
      Serial.println(spacecount);
    }
  }}

