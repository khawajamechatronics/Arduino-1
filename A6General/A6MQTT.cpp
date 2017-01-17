/*
  MQTT.h - Library for GSM MQTT Client.
  Created by Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016.
  Released into the public domain.
*/
/*
 *   add Reset function -- see begin()
 *   print tcpstatus correctly
 *   Get APN from nain program
 *   Replace all Serial by HW_SERIAL
 *   Set HW serial via macro #define HW_SERIAL Serial1  etc
 *   Replace all serialEvent by SERIALEVENT
 *   Set hw serial event via macro #define SERIALEVENT serialEvent1 etc
 *   
 *   change MQTT_PORT to int
 *   Add USERID, PASSWORD
 *   Replace printMessageType and printConnectAck by printConstString
 *   Bugfix replace 200 by UART_BUFFER_LENGTH in SERIALEVENT
 *   
 *   Changed all mySerial.print to new method DebugPrint. Now user must implement
 *   DebugPrint as he sees fit
 */
#include "Arduino.h"
#include "A6Services.h"
#include "A6MQTT.h"
#include <avr/pgmspace.h>

A6_MQTT::A6_MQTT(unsigned long KeepAlive)
{
  _KeepAliveTimeOut = KeepAlive;
//  gsm.doParsing = false;  // dont parse until connected to server
  modemMessageLength = 0;
  ParseState = GETMM;
  _PingNextMillis = 0xffffffff;
  connectedToServer = false;  // got ACK & it was  CONNECTION_ACCEPTED
  waitingforConnack = false;  // send connection request and waiting for ACK
}

static byte mqttbuffer[50];  // build up message here

uint16_t bswap(uint16_t w)
{
  return ((w&0xff)*256) + (w/256);
}
char *Protocolname = "MQTT";
// Using version 3.1.1  Protocol name MQTT, version 4
bool A6_MQTT::connect(char *ClientIdentifier, char UserNameFlag, char PasswordFlag, char *UserName, char *Password, char CleanSession, char WillFlag, char WillQoS, char WillRetain, char *WillTopic, char *WillMessage)
{
 // ConnectionAcknowledgement = MQ_NO_ACKNOWLEDGEMENT ;
  // calculate overall size of connect headers + payload;
  int connsize = sizeof(struct sFixedHeader)+sizeof(struct sConnectVariableHeader)+sizeof(uint16_t)+strlen(ClientIdentifier);
  if (UserNameFlag)
    connsize+= sizeof(uint16_t) + strlen(UserName);
  if (PasswordFlag)
    connsize+= sizeof(uint16_t) + strlen(Password);
  if (WillFlag)
    connsize+= sizeof(uint16_t) + strlen(WillTopic) + sizeof(uint16_t) + strlen(WillMessage);
  struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
  int bindex = 0;
  pFH->rsv = 0;
  pFH->controlpackettype = MQ_CONNECT;
  pFH->rl = connsize-sizeof(struct sFixedHeader); // do not encode in data
  bindex += sizeof(struct sFixedHeader);
  struct sConnectVariableHeader *pVH = (struct sConnectVariableHeader *)&mqttbuffer[bindex];
  pVH->length = bswap(strlen(Protocolname));
  strcpy(pVH->pname,Protocolname);
  pVH->protocolLevel = 4; // always
  pVH->connectFlags = (CleanSession ? CLEAN_SESSION_FLAG : 0) |
                        (WillFlag ? WILL_FLAG : 0) | (WillFlag ? WillQoS<<3 : 0 ) | (WillFlag ? WILL_RETAIN : 0) |
                        (UserNameFlag ? USERNAME_FLAG : 0) |
                        (PasswordFlag ? USERNAME_FLAG : 0);
  pVH->keepalive = bswap(_KeepAliveTimeOut);
  bindex += sizeof(struct sConnectVariableHeader);
  struct sVariableString *pVS = (struct sVariableString *)&mqttbuffer[bindex];
  pVS->length = bswap(strlen(ClientIdentifier));
  strcpy(pVS->string,ClientIdentifier);
  bindex += sizeof(int16_t) + strlen(ClientIdentifier);
  if (WillFlag)
  {
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(WillTopic));
    strcpy(pVS->string,WillTopic);
    bindex += sizeof(int16_t) + strlen(WillTopic);
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(WillMessage));
    strcpy(pVS->string,WillMessage);
    bindex += sizeof(int16_t) + strlen(WillMessage);
  }
  if (UserNameFlag)
  {
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(UserName));
    strcpy(pVS->string,UserName);
    bindex += sizeof(int16_t) + strlen(UserName);
  }
  if (PasswordFlag)
  {
    pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(Password));
    strcpy(pVS->string,Password);
    bindex += sizeof(int16_t) + strlen(Password);
  }
  return gsm.sendToServer(mqttbuffer,connsize);
}

bool A6_MQTT::subscribe(unsigned int MessageID, char *SubTopic, char SubQoS)
{
  bool rc = false;
  if (connectedToServer)
  {
    int connsize = sizeof(struct sFixedHeader)+sizeof(struct sSubscribeVariableHeader)+sizeof(uint16_t)+strlen(SubTopic) + 1; // includes QOS byte
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    int bindex = 0;
    pFH->rsv = 0x02;   // see documentation
    pFH->controlpackettype = MQ_SUBSCRIBE;
    pFH->rl = connsize-sizeof(struct sFixedHeader); // do not encode in data
    bindex += sizeof(struct sFixedHeader);
    struct sSubscribeVariableHeader *pVH = (struct sSubscribeVariableHeader *)&mqttbuffer[bindex];
    pVH->packetid = bswap(MessageID);
    bindex += sizeof(struct sSubscribeVariableHeader);
    struct sVariableString *pVS = (struct sVariableString *)&mqttbuffer[bindex];
    pVS->length = bswap(strlen(SubTopic));
    strcpy(pVS->string,SubTopic);
    bindex += sizeof(int16_t) + strlen(SubTopic);
    *(uint8_t *)bindex = SubQoS; 
    rc = gsm.sendToServer(mqttbuffer,connsize);
  }
  return rc;
}

/*
 *  Serial input is a mixture of modem unsolicited messages e.g. +CGREG: 1, expected messages
 *  suxh as the precursor to data +CIPRCV:n, and data from the broker
 *  We look for complete modem messages & react to +CIPRCV, tgr4ansfer n bytes tp mqtt parser
 *  wait for cr/lf cr , as terminators 
 */
//const char *rcv = "+CIPRCV:";
//const char rcv[] PROGMEM = "+CIPRCV:";
//const char *tcpclosed PROGMEM = "+TCPCLOSED:0";
void A6_MQTT::serialparse()
{
  char c = gsm.pop();
  while (c != -1)
  {
    switch (ParseState)
    {
      case GETMM:
        modemmessage[modemMessageLength++] = c;
        if (c==0x0a || c== 0x0d || c== ':') // expected delimiter
        {
          if (modemMessageLength == strlen("+CIPRCV:") && strncmp(modemmessage,"+CIPRCV:",8) == 0)
          {
            ParseState = GETLENGTH;
            modemMessageLength = 0;
          }
          else if (modemMessageLength == strlen("+TCPCLOSED:") && strncmp(modemmessage,"+TCPCLOSED:",11) == 0)
          {
            gsm.DebugWrite(F("Server closed connection\r\n"));
            connectedToServer = false;
            gsm.stopIP();
            gsm.getCIPstatus();
            AutoConnect();
          }
          else
            modemMessageLength = 0; // just discard      
        }
        break;
      case GETLENGTH:
        modemmessage[modemMessageLength++] = c;
        if (c == ',')
        {
          modemmessage[modemMessageLength] = 0;
          mqttmsglength = atoi(modemmessage);
          modemMessageLength = 0;
          ParseState = GETDATA;
        }
        break;
      case GETDATA:
        modemmessage[modemMessageLength++] = c;
        if (modemMessageLength == mqttmsglength)
        {
          ParseState = GETMM;
          modemMessageLength = 0;
          mqttparse();
        }
        break;
    }
    c = gsm.pop();
  }
}

/*
 * buffer modemmessage contains mqttmsglength bytes
 */
void A6_MQTT::mqttparse()
{
  struct sFixedHeader *pFH = (struct sFixedHeader *)modemmessage;
  struct sSubackVariableHeader *pSVH;
  struct sVariableString *pVS;
  int16_t slength;
  uint16_t *pW;
  Serial.print("<<");
  for (int ii=0;ii<pFH->rl+2;ii++)
  {
    Serial.print(modemmessage[ii],HEX);
    Serial.print(',');
  }
  Serial.println();
  switch (pFH->controlpackettype)
  {
    case MQ_CONNACK:
      if (pFH->rl == 2)
      {
        // skip to next part of message
        struct sConnackVariableHeader *pCAVH = (struct sConnackVariableHeader *)&modemmessage[sizeof(struct sFixedHeader)];
        connectedToServer = pCAVH->returncode == CONNECT_RC_ACCEPTED;
        waitingforConnack = false;
        OnConnect(pCAVH->returncode);
      }
      break;
    case MQ_SUBACK:
      pSVH = (struct sSubackVariableHeader *)&modemmessage[sizeof(struct sFixedHeader)];
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
      OnSubscribe(bswap(pSVH->packetid));
      break;
    case MQ_PUBLISH:
      pVS = (struct sVariableString *)&modemmessage[sizeof(struct sFixedHeader)];
      slength = bswap(pVS->length);
      memcpy(Topic,pVS->string,slength);  // copy out topic
      Topic[slength] = 0;  // add end marker
      memcpy(Message,pVS->string+slength,pFH->rl-slength-sizeof(int16_t));
      Message[pFH->rl-slength-sizeof(int16_t)] = 0;
      OnMessage(Topic,Message);
      break;
    case MQ_PINGRESP:
      gsm.DebugWrite(F("ping response\r\n"));
      _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
      break;
    case MQ_PUBACK:
      pW = (uint16_t *)&modemmessage[2];
      gsm.DebugWrite("puback: ");
      gsm.DebugWrite(*pW);
      gsm.DebugWrite("\r\n");
      break;
    case MQ_UNSUBACK:
      pW = (uint16_t *)&modemmessage[2];
      gsm.DebugWrite("unsuback: ");
      gsm.DebugWrite(*pW);
      gsm.DebugWrite("\r\n");
      break;
  }
}

bool A6_MQTT::ping()
{
  bool rc = false;
  if (connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PINGREQ;
    pFH->rsv = 0;
    pFH->rl = 0;
    // stop spiunning in ping loop
    _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
    // no variable header or payload
    rc =  gsm.sendToServer(mqttbuffer,2);
  }
  return rc;
}

bool A6_MQTT::publish(char *Topic, char *Message)
{
  bool rc = false;
  if (connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_PUBLISH;
    pFH->rsv = 0;
    pFH->rl = strlen(Topic) + strlen(Message) + sizeof(uint16_t);  // QOS 0 no messageid
    // copy topic as variable string
    struct sVariableString *pVS = (struct sVariableString *)&mqttbuffer[2];
    pVS->length = bswap(strlen(Topic));
    strcpy(pVS->string,Topic);
    // copy message after topic
    char *pS = (char*)((int)pVS + sizeof(uint16_t) + strlen(Topic));
    strcpy(pS,Message);
    _PingNextMillis = millis() + (_KeepAliveTimeOut*1000) - 2000;
    rc = gsm.sendToServer(mqttbuffer,pFH->rl + 2);
  }
  return rc;
}

bool A6_MQTT::disconnect()
{
  bool rc = false;
  if (connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_DISCONNECT;
    pFH->rsv = 0;
    pFH->rl = 0;
    connectedToServer = false;
    rc = gsm.sendToServer(mqttbuffer,2);
  }
  return rc;  
}

 bool A6_MQTT::unsubscribe(unsigned int MessageID, char *SubTopic)
 {
  bool rc = false;
  if (connectedToServer)
  {
    struct sFixedHeader *pFH = (struct sFixedHeader *)mqttbuffer;
    pFH->controlpackettype = MQ_UNSUBSCRIBE;
    pFH->rsv = 2;
    pFH->rl = (2*sizeof(int16_t)) +strlen(SubTopic); // messageid + length + string
    int16_t *pW = (int16_t *)&mqttbuffer[2];
    *pW++ = bswap(MessageID);
    struct sVariableString *pVS = (struct sVariableString *)pW;
    pVS->length = bswap(strlen(SubTopic));
    strcpy(pVS->string,SubTopic);
    rc = gsm.sendToServer(mqttbuffer,pFH->rl + 2);  
  }
  return rc;
 }

