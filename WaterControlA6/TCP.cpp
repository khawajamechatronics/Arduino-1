#include "Arduino.h"
#include "A6Services.h"
#include "A6HTTP.h"
#include "TCP.h"

extern A6GPRS gsm;
extern A6HTTP http;
//#define HTTP_TIMEOUT 30 // seconds

extern bool tcpActive;
extern uint32_t httptimeout;
extern int gprstimeout;

bool HTTPGET(char *server,char *url,int port)
{
  bool rc = false;
  if (gsm.connectTCPserver(server,port))
  {
    Serial.print("Server up at ");
    Serial.println(millis());
    tcpActive = true;
    if (http.get(url))
      rc = true;
    else
      gsm.disconnectTCPserver();
  }
  else  
      Serial.println("Server not up");
  return rc;
}

