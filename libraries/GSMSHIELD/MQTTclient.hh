#ifndef _MQTT_
#define _MQTT_

#include "SIM900.h"
#include "inetGSM.h"
//#define SMTP_DEBUG_ON  // comment out if not needed
#define MAX_TEXT_LENGTH 100   // maximum length of a body text
#define MAX_COMMAND_LENGTH 20  // maximum length of a raw AT command format

#define MQTT_LIB_VERSION 100 // initial version, uses gsm object already created

class MQTTGSM
{
	public:
    // MQTT's methods 
    enum eConnAck MqttSetBroker(char *broker_url,int keepalive);	//default port 1883
    enum eConnAck MqttSetBroker(char *broker_url,int keepalive,int port);
	private:
	char printbuf[MAX_TEXT_LENGTH+MAX_COMMAND_LENGTH];
};

// Control Packet Type
enum eControlType {CONNECT=1,CONNACK,PUBLISH,PUBACK,PUBREC,PUBREL,PUBCOMP,
SUBSCRIBE,SUBACK,UNSUBSCRIBE,NSUBACK,PINGREQ,PINGRESP,DISCONNECT};

enum eConnAck {CONN_OK,UNACC_PROTOCOL,IDENTIFIER_REJECTED,CONN_ERROR};
// flag bits masks
#define RETAIN_MASK 0B0011
#define QOS_MASK 	0B0110
#define DUP_MASK	0B1000
// data structures
struct mqttFixedHeader {
	unsigned flagbits:4;
	enum eControlType controlType:4;
	unsigned char remainingLengthb0;  // handles lengths up to 127
};

struct mqttConnectVariableHeader {
	unsigned short length; //MSB
	char m[4] = {'M','Q','T','T'};
	char level = 0x04;		// corresponds to MQTT V3.1.1
	unsigned char flags;
	unsigned short keepalive; // MSB
};
#endif
