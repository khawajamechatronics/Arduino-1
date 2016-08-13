#define DefaultHouse  "+972545919886"

#define REMOTE_SERIAL Serial1   // when using MEGA option of GSM library
#define REMOTE_BAUD 115200

enum eRemoteType {GSM_REMOTE,SERIAL_REMOTE};
bool RemoteInit(enum eRemoteType);
bool RemoteMessageAvailable(char *msg,char *timestamp);
bool RemoteSendMessage(char * msg);  // to default subscriber or last sender
bool RemoteSendMessage(char * msg, char *sub);  // to particular subscriber
bool RemoteSetClock(char *);
char *RemoteGetClock(void);

