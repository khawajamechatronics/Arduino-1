SoftwareSerial iaqSerial(IaqPin, -1,false);
#define BUFSIZE 128
char IAQBuffer[BUFSIZE];
enum estate { UNKNOWN,NUM,BLANK,CR,LF };
char printbuf[10];

void IAQInit()
{
  Serial.println("IAQ init");
  iaqSerial.begin(9600);
}
void IAQClose()
{
  iaqSerial.flush();
  iaqSerial.end();
}

void IAQRead()
{
  int spacecount = 0;
  iaqSerial.listen();
  iaqSerial.flush();
  uint8_t idx = 0;
  uint8_t i;
  bool alldec = true;
  char c;
  estate state = UNKNOWN;
  // data should be an arraey of 10 numbers separated by blank with trailing blank/cr/lf
  // however not always well behaved so do a sanity check
  do
  {
    while (iaqSerial.available() == 0) ; // wait for a char this causes the blocking
    c = iaqSerial.read();
    IAQBuffer[idx++] = c;
    idx %= BUFSIZE;  // avoid overflow
    sprintf(printbuf,"%02X",c);
    Serial.print(printbuf);
    if (c == ' ')
      state = BLANK;
    else if (c == 0x0d && state == BLANK)
      state = CR;
    else if (c == 0x0a && state == CR)
      state = LF; 
    else
      state = UNKNOWN;
  } while (state != LF /*&& idx<127*/);
  // Now do sanity check
  // trim trailing blanks
  Serial.println();
  idx -= 3;
  // marke end of string
  IAQBuffer[idx] = 0;
  for (i=0; i< idx; i++)
  {
    Serial.print(IAQBuffer[i]);
    if (IAQBuffer[i] == 0x20)
      spacecount++;
    else if (IAQBuffer[i] < '0' || IAQBuffer[i] > '9')
      alldec = false;
  }
  Serial.print(' ');
  Serial.print(spacecount);
  Serial.print(",");
  Serial.println(alldec);
  IQ100OK = alldec && (spacecount == 9);
  if (IQ100OK == false)
    Serial.println("IAQ1000 badly formed");
}

