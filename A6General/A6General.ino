/*
 *   Using GPRS A6 development board, powewr comes from USB so no need to supply from Arduino
 *   RX/TX always connect to a hardware Serial port so we can utilize serialevent
 *   PWR always connected to Arduino VCC, RESET to TRANSISTOR_CONTROL
 *   
 */
#include "A6Services.h"
char buff[30];
char *msg = "GET /WC/last3.php  HTTP/1.0\r\nHost: \"david-henry.dyndns.tv\"\r\nUser-Agent: Arduino\r\n\r\n";

void setup() {
  Serial.begin(115200);
  if (gsm.begin(A6_BAUDRATE))
  {
    Serial.println("GMS up");
    gsm.getCIPstatus();
    gsm.enableDebug = true;
#if 0
    if (gsm.getIMEI(buff))
    {
      Serial.print("\r\nIMEI: ");
      Serial.println(buff);
    }
    else
      Serial.println("getimei failed");
#endif
#if 0
    if (gsm.getCIMI(buff))
    {
      Serial.print("\r\nCIMI: ");
      Serial.println(buff);
    }
    else
      Serial.println("getcimi failed");
#endif
#if 0
    if (gsm.setRTC("10/10/10,10:10:10+02"))
    {
      delay(10000);
      if (gsm.getRTC(buff))
      {
        Serial.print("\r\nRTC: ");
        Serial.println(buff);
      }
      else
        Serial.println("getrtc failed");
    }
    else
      Serial.println("setrtc failed");
#endif
#if 0
    Serial.println(gsm.getCIPstatusString());
#endif
#if 0
    switch (gsm.getPSstate())
    {
      case gsm.DETACHED:
        Serial.println("detached");
        break;
      case gsm.ATTACHED:
        Serial.println("attached");
        gsm.setPSstate(gsm.DETACHED);
        break;
      case gsm.PS_UNKNOWN:
        Serial.println("unknown");
        break;
    }
#endif
#if 1
    if (gsm.startIP("uinternet"))
    {
      Serial.println("IP up");
      if (gsm.getLocalIP(buff))
        Serial.println(buff);
//      if (gsm.stopIP())
  //      Serial.println("IP stopped");
    //  else
      //  Serial.println("IP not stopped");
      if (gsm.connectTCPserver("david-henry.dyndns.tv",80))
      {
        Serial.println("TCP up");
        gsm.sendToServer(msg);
      }
      else
        Serial.println("TCP down");
    }
    else
      Serial.println("IP down");
#endif
    gsm.inSetup = false;
  }
  else
    Serial.println("GSM down");
}
void loop() {
  if (Serial.available())
    gsm.ModemWrite(Serial.read());
}

