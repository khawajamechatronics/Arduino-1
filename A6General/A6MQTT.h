/*
  MQTT.h - Library for GSM MQTT Client.
  Created by Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016.
  Released into the public domain.
*/
#ifndef A6_MQTT_H_
#define A6_MQTT_H_

//#define UART_BUFFER_LENGTH 300    //Maximum length allowed for UART data
#define TOPIC_BUFFER_LENGTH 50    //Maximum length allowed Topic
#define MESSAGE_BUFFER_LENGTH 50  //Maximum length allowed data
#define MODEM_MESSAGE_LENGTH (TOPIC_BUFFER_LENGTH+MESSAGE_BUFFER_LENGTH+10) // max expected length of modem input

// ######################################################################################################################
#define MQ_CONNECT     1   //Client request to connect to Server                Client          Server
#define MQ_CONNACK     2   //Connect Acknowledgment                             Server/Client   Server/Client
#define MQ_PUBLISH     3   //Publish message                                    Server/Client   Server/Client
#define MQ_PUBACK      4   //Publish Acknowledgment                             Server/Client   Server/Client
#define MQ_PUBREC      5   //Publish Received (assured delivery part 1)         Server/Client   Server/Client
#define MQ_PUBREL      6   //Publish Release (assured delivery part 2)          Server/Client   Server/Client
#define MQ_PUBCOMP     7   //Publish Complete (assured delivery part 3)         Server/Client   Server/Client
#define MQ_SUBSCRIBE   8   //Client Subscribe request                           Client          Server
#define MQ_SUBACK      9   //Subscribe Acknowledgment                           Server          Client
#define MQ_UNSUBSCRIBE 10  //Client Unsubscribe request                         Client          Server
#define MQ_UNSUBACK    11  //Unsubscribe Acknowledgment                         Server          Client
#define MQ_PINGREQ     12  //PING Request                                       Client          Server
#define MQ_PINGRESP    13  //PING Response                                      Server          Client
#define MQ_DISCONNECT  14  //Client is Disconnecting                            Client          Server

// QoS value bit 2 bit 1 Description
//   0       0       0   At most once    Fire and Forget         <=1
//   1       0       1   At least once   Acknowledged delivery   >=1
//   2       1       0   Exactly once    Assured delivery        =1
//   3       1       1   Reserved
#define DUP_Mask      8   // Duplicate delivery   Only for QoS>0
#define QoS_Mask      6   // Quality of Service
#define QoS_Scale     2   // (()&QoS)/QoS_Scale
#define RETAIN_Mask   1   // RETAIN flag

//#define User_Name_Flag_Mask  128
//#define Password_Flag_Mask   64
//#define Will_Retain_Mask     32
//#define Will_QoS_Mask        24
//#define Will_QoS_Scale       8
//#define Will_Flag_Mask       4
//#define Clean_Session_Mask   2

#define MQ_DISCONNECTED          0
#define MQ_CONNECTED             1
#define MQ_NO_ACKNOWLEDGEMENT  255

enum eParseState {GETMM,GETLENGTH,GETDATA};
struct sFixedHeader {
  byte retain:1;
  byte qos:2;
  byte dup:1;
  byte controlpackettype:4;
  byte rl;
};
struct sConnectVariableHeader {
  uint16_t length;
  byte pname[4];  // always MQIsdp
  byte protocolLevel;
  byte connectFlags;
  uint16_t keepalive;
};

struct sConnackVariableHeader {
  byte flags;
  byte returncode;
};

struct sSubscribeVariableHeader {
  uint16_t packetid;
};

struct sSubackVariableHeader {
  uint16_t packetid; 
};

struct sSubackPayload {
  byte rc[1];
};

struct sVariableString {
  uint16_t length;
  char string[1];
};
// connectFlags masks
#define CLEAN_SESSION_FLAG (1<<1)
#define WILL_FLAG (1<<2)
#define WILL_QOS (3<<3)
#define WILL_RETAIN (1<<5)
#define PASSWORD_FLAG (1<<6)
#define USERNAME_FLAG (1<<7)

class A6_MQTT
{
  public:
      enum eConnectRC {CONNECT_RC_ACCEPTED, CONNECT_RC_REFUSED_PROTOCOL,CONNECT_RC_REFUSED_IDENTIFIER};
      enum eQOS {QOS_0,QOS_1,QOS_2};
 //   volatile bool TCP_Flag = false;
 //   volatile char GSM_ReplyFlag;
 //   char reply[20];
    volatile bool pingFlag = false;
 //   volatile char tcpATerrorcount = 0;
 //   volatile bool MQTT_Flag = false;
 //   volatile int ConnectionAcknowledgement = MQ_NO_ACKNOWLEDGEMENT ;
 //   volatile int PublishIndex = 0;
 //   volatile int MessageFlag = false;
 //   volatile int modemStatus = 0;
    volatile uint32_t index = 0;
    volatile uint32_t length = 0, lengthLocal = 0;
    bool connectedToServer;
    bool waitingforConnack;
  //  volatile bool waitingForAck = false;
 //   char inputString[UART_BUFFER_LENGTH]; 
    A6_MQTT(unsigned long KeepAlive);
    bool connect(char *ClientIdentifier, char UserNameFlag, char PasswordFlag, char *UserName, char *Password, char CleanSession, char WillFlag, char WillQoS, char WillRetain, char *WillTopic, char *WillMessage);
    bool publish(char *Topic, char *Message); // QOS 0 no dup no retain
    bool publish(char *Topic, char *Message, bool , bool ); // dup,retain,qos=0
    bool publish(char *Topic, char *Message, bool , bool, eQOS,uint16_t packetid ); // dup,retain,qos
    bool subscribe(unsigned int MessageID, char *SubTopic, char SubQoS);
    bool unsubscribe(unsigned int MessageID, char *SubTopic);
    bool disconnect(void);
 //   void processing(void);
    bool available(void);
    bool ping(void);
    
    void AutoConnect(void);
    void OnConnect(eConnectRC) __attribute__((weak));
    void OnSubscribe(uint16_t)  __attribute__((weak));
    void OnMessage(char *Topic,char *Message,bool,bool,eQOS)   __attribute__((weak));
    void OnPubAck(uint16_t messageid) __attribute__((weak));

    void serialparse();
    void mqttparse();

    volatile unsigned long _PingNextMillis = 0;
  private:
    volatile unsigned int _LastMessaseID = 0;
    volatile int _ProtocolVersion = 3;
    volatile int  _tcpStatus = 0;
    volatile int _tcpStatusPrev = 0;
    volatile unsigned long _KeepAliveTimeOut;
    enum eParseState ParseState;
    int modemMessageLength;
    byte modemmessage[MODEM_MESSAGE_LENGTH];
    int mqttmsglength;
    char Topic[TOPIC_BUFFER_LENGTH];
    volatile int TopicLength = 0;
    char Message[MESSAGE_BUFFER_LENGTH];
    volatile int MessageLength = 0;
 //   void _sendUTFString(char *string);
 //   void _sendLength(int len);
    void _ping(void);
 //   void _tcpInit(void);
 //   char _sendAT(char *command, unsigned long waitms);
 //   unsigned int _generateMessageID(void);
};
#endif /* A6_MQTT_H_ */

