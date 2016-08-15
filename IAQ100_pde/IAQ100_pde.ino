#include <icrmacros.h>
#include <SoftwareSerial.h>
#define IaqPin  4
SoftwareSerial iaqSerial(IaqPin, -1,true);
#define BUFSIZE 80
char IAQBuffer[BUFSIZE];
#define LOOP_CYCLE_TIME 5  // do work every 5 secs
unsigned long lastMillis;

enum estate { UNKNOWN,NUM,BLANK,CR,LF };
estate state,endstate;

char printbuf[10];
void setup()
{
  Serial.println("IAQ init");
  iaqSerial.begin(9600);
  Serial.begin(115200);
  state = UNKNOWN;
}

void loop()
{
  int spacecount = 0;
  uint8_t idx = 0;
  uint8_t i;
  bool alldec = true;
  bool success = false;
  int trycount = 10;
  char c;
  if ((millis() - lastMillis)/1000 >= LOOP_CYCLE_TIME )
  {
    Serial.println("IAQ read");
    iaqSerial.listen();
    iaqSerial.flush();
    lastMillis = millis();
    while (success == false /*&& trycount > 0*/)
    {
      Serial.println("retry");
      spacecount = idx = 0;
      alldec = true;
      state = UNKNOWN;
      Serial.println(millis());
      // lines finish with sequence blank cr lf
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
      // trim trailing blanks, if any
      idx -= 3;
      Serial.println(' ');
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
      trycount--;
      success = (spacecount == 9) && alldec;
    }
  }
}

