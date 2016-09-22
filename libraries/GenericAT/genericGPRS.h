#ifndef _INETGSM_H_
#define _INETGSM_H_

#define BUFFERSIZE 1

#include "GenericGSM.h"

class inetGSM
{
  private:
    char _buffer[BUFFERSIZE];
	int cid;
	char printbuf[30];
    
  public:
    int httpGET(const char* server, int port, const char* path, char* result, int resultlength);
    int httpPOST(const char* server, int port, const char* path, const char* parameters, char* result, int resultlength);
    
    // Fast and dirty solution. Should make a "mail" object. And by the moment it does not run.
    int openmail(char* server, char* loginbase64, char* passbase64, char* from, char* to, char* subj);
    int closemail();
    int attachGPRS(char* domain, char* dom1, char* dom2);
    int detachGPRS();
    int connectTCP(const char* server, int port);
    int connectUDP(const char* server, int port);
    int disconnectTCP();
    int disconnectUDP();
    int connectTCPServer(int port);
    boolean connectedClient();
	bool OpenGprs(int cid,char *apn,int responsetime);
	bool CloseGprs(int cid);
    // This runs, yes
    //int tweet(const char* token, const char* msg);
	char *GetGprsIP(int cid);
};

#endif
