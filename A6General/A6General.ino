/*
 *   Using GPRS A6 development board, powewr comes from USB so no need to supply from Arduino
 *   RX/TX always connect to a hardware Serial port so we can utilize serialevent
 *   PWR always connected to Arduino VCC, RESET to TRANSISTOR_CONTROL
 *   
 */
#include "A6Services.h"
#include "A6MQTT.h"

char buff[30];

A6_MQTT MQTT(30);
uint32_t nextpublish;
#define PUB_DELTA 20000 // publish every 20 secs
bool unsubscribed = false;
void setup() {
  Serial.begin(115200);
  if (gsm.begin())
  {
    Serial.println("GSM up");
    gsm.getCIPstatus();
    gsm.enableDebug = false;
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
      MQTT.AutoConnect();
     //   gsm.sendToServer(msg);
    //    nextpublish = millis()+PUB_DELTA;
    }
    else
      Serial.println("IP down");
#endif
  //  gsm.inSetup = false;
  }
  else
    Serial.println("GSM down");
}
uint16_t PACKET_ID = 0;   // for qos 1
void loop() {
  if (MQTT.connectedToServer || MQTT.waitingforConnack)
  {
    if (MQTT._PingNextMillis < millis())
      MQTT.ping();
#if 1
    if (nextpublish < millis())
    {
      nextpublish = millis()+PUB_DELTA;
      sprintf(buff,"%lu",millis());
      Serial.print("Publish ");
      Serial.println(PACKET_ID,HEX);
      MQTT.publish("/blah",buff,false,true,MQTT.QOS_0,PACKET_ID); // dup false, retain false, QOS 
      PACKET_ID += 0x200;
    }
#endif
    MQTT.Parse();
  }
  if (Serial.available())
    gsm.ModemWrite(Serial.read());
}

void A6_MQTT::AutoConnect()
{
  if (gsm.connectTCPserver("david-henry.dyndns.tv",1883))
  {
    Serial.println("TCP up");
    MQTT.waitingforConnack = connect("a", 0, 0, "", "", 1, 0, 0, 0, "", "");
  }
  else
    Serial.println("TCP down");
}

void A6_MQTT::OnConnect(enum eConnectRC rc)
{
  switch (rc)
  {
    case MQTT.CONNECT_RC_ACCEPTED:
      Serial.println("Connected");
      MQTT._PingNextMillis = millis() + (MQTT._KeepAliveTimeOut*1000) - 2000;
      MQTT.subscribe(1234,"/blah",1);
     break;
    case MQTT.CONNECT_RC_REFUSED_PROTOCOL:
      Serial.println("Protocol error");
      break;
    case MQTT.CONNECT_RC_REFUSED_IDENTIFIER:
      Serial.println("Identity error");
      break;
  }
}

void A6_MQTT::OnSubscribe(uint16_t pi)
{
  Serial.print("Subscribed to ");
  Serial.println(pi);
}

void A6_MQTT::OnMessage(char *topic,char *message,bool dup, bool ret,A6_MQTT::eQOS qos)
{
  if (dup)
    Serial.print("DUP ");
  if (ret)
    Serial.print("RET ");
  Serial.print("QOS ");
  Serial.println(qos);
  Serial.print("Topic: ");Serial.println(topic);
  Serial.print("Message: ");Serial.println(message);
}

void A6_MQTT::OnPubAck(uint16_t messageid)
{
  Serial.print("Packet ");
  Serial.print(messageid,HEX);
  Serial.println(" Acknowledged");
}

void A6_MQTT::OnDisconnect()
{
  Serial.println("Server disconnected");
  MQTT.AutoConnect();
}

