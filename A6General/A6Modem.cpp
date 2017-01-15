/*
 * This source file provides services for the physical manipulation of the serial stream
 * to and from the modem
 */
#include "Arduino.h"
#include "A6Modem.h"
#include <TimerOne.h>

void HW_SERIAL_EVENT();
//bool availableCID[] = {true,true,true,true,true,true,true};
bool GPRSA6Device::begin(long baudrate)
{
  bool rc = false;
  HW_SERIAL.begin(A6_BAUDRATE);
  inlevel = outlevel = 0;
  //inSetup = true;
  enableDebug = false;
  HWReset();
  if (waitresp("+CREG: 1\r\n",20000))
  {
    HW_SERIAL.print("ATE0\r");
    rc = waitresp("OK\r\n",1000);
  }
  return rc;
}
// ATV1
GPRSA6Device::GPRSA6Device(){}

void GPRSA6Device::push(char c)
{
  DebugWrite(c);
  comm_buf[inlevel++] = c;
  if (inlevel == COMM_BUF_LEN)  // handle wrap around
    inlevel = 0;
}

char GPRSA6Device::pop()
{
  char c;
  if (inlevel == outlevel)
    c = -1;
  else
  {
    c = comm_buf[outlevel++];
    if (outlevel == COMM_BUF_LEN)  // handle wrap around
      outlevel = 0;
    if (inlevel == outlevel)
      inlevel = outlevel = 0;
  }
  return c;
}

void GPRSA6Device::ModemWrite(char c)
{
  HW_SERIAL.write(c);
}

/*
 * Check incoming buffer for desired string, return true if found in time else false
 * Only use this version during setup time as serialEvent not yet available
 */
bool GPRSA6Device::waitresp(char const *response_string,int32_t timeout)
{
  int lengthtotest = strlen(response_string);
  char *nextChar = response_string;
  bool started = false;
  uint32_t TimeOut = millis() + timeout;
  while (TimeOut > millis() && lengthtotest>0)
  {
//    if (inSetup && HW_SERIAL.available())
  //    push(HW_SERIAL.read());
    HW_SERIAL_EVENT();
    // get next char from buffer, if no match discard, if match decrement lengthtotest & get next character
    char c = pop();
    if (c != -1)
    {
      if (c == *nextChar)
      {
 //       DebugWrite('[');
        lengthtotest--;
        nextChar++;
        started = true;
      }
      else if (started)
      {
 //       DebugWrite(']');
        lengthtotest = strlen(response_string);
        nextChar = response_string;
        started = false;
      }
    }
  }
  return TimeOut > millis();  // finished before time is up
}

bool GPRSA6Device::GetLineWithPrefix(char const *px,char *outbuf, int bufsize,int32_t timeout)
{
  int lengthtotest = strlen(px);
  char *nextChar = px;
  uint32_t TimeOut = millis() + timeout;
  enum epx {BEFORE_PREFIX,IN_PREFIX,AFTER_PREFIX} eepx = BEFORE_PREFIX;
  bool alldone = false;
  if (px == NULL)
  {
    eepx = AFTER_PREFIX;
  }
  while (TimeOut > millis() && !alldone)
  {
//    if (inSetup && HW_SERIAL.available())
//      push(HW_SERIAL.read());
    HW_SERIAL_EVENT();
    // get next char from buffer, if no match discard, if match decrement lengthtotest & get next character
    char c = pop();
    if (c!= -1)
    {
      switch (eepx)
      {
        case BEFORE_PREFIX:
          if (c == *nextChar)
          {
            eepx = IN_PREFIX;
            lengthtotest--;
            nextChar++;
          }
          break;
        case IN_PREFIX:
          if (c == *nextChar)
          {
            lengthtotest--;
            nextChar++;
            if (lengthtotest == 0)
              eepx = AFTER_PREFIX;
          }
          else
          {
            eepx = BEFORE_PREFIX;
            nextChar = px;
            lengthtotest = strlen(px);
          }
          break;
        case AFTER_PREFIX:
 //         Serial.print('[');
          *outbuf++ = c;
          bufsize--;
          if (c == 0x0d || c == 0x0a || c == 0 || bufsize == 0)
          {
 //           Serial.print(']');
            *outbuf = 0; // add end marker
            alldone = true;
          }
          break;
      }
    }
  } 
  return TimeOut > millis();  // still have time left 
}

void GPRSA6Device::RXFlush()
{
  volatile char c;
  while (HW_SERIAL.available())
    c = HW_SERIAL.read();
  inlevel = outlevel = 0;
}

#if 0
int GPRSA6Device::getcid()
{
  int cid = -1;
  for (int i=0;i<7;i++)
    if (availableCID[i])
    {
      availableCID[i] = false;
      cid = i+1;  // counted from 1, not 0
      break;
    }
  return cid;
}

void GPRSA6Device::freecid(int cid)
{
  if (cid>=1 && cid<=7)
    availableCID[cid-1] = true;
}
#endif

