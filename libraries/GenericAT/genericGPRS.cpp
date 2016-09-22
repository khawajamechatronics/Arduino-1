#include "genericGPRS.h"

#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10
static const char *OK = "OK";
bool writewithtimeout(char *buf,const char *response,unsigned long start,unsigned long interchar)
{
	bool success = false;
	byte status;
#ifdef DEBUG_ON
	DebugPrintln(buf);
#endif
	atdevice.SimpleWrite(buf);
	// 1 sec. for initial comm tmout
    // and max. 150 msec. for inter character timeout
	atdevice.RxInit(start, interchar); 
	// wait response is finished
	do
	{
		if (atdevice.IsStringReceived(response))
		{ 
			// perfect - we have some response, but what:
			status = RX_FINISHED;
			success = true;
			break; // so finish receiving immediately and let's go to 
             // to check response 
		}
		status = atdevice.IsRxFinished();
	} while (status == RX_NOT_FINISHED);
	return success;
}

int inetGSM::httpGET(const char* server, int port, const char* path, char* result, int resultlength)
{
  boolean connected=false;
  int n_of_at=0;
  int length_write;
  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';
	int res;
  /*
  Status = ATTACHED.
  if(atdevice.getStatus()!=GSM::ATTACHED)
    return 0;
  */
  while(n_of_at<3){
	  if(!connectTCP(server, port)){
	  	#ifdef DEBUG_ON
			DebugPrintln("DB:NOT CONN");
		#endif	
	    	n_of_at++;
	  }
	  else{
		connected=true;
		n_of_at=3;
	}
  }

  if(!connected) return 0;
	
  atdevice.SimpleWrite("GET ");
  atdevice.SimpleWrite(path);
  atdevice.SimpleWrite(" HTTP/1.0\nHost: ");
  atdevice.SimpleWrite(server);
  atdevice.SimpleWrite("\n");
  atdevice.SimpleWrite("User-Agent: Arduino");
  atdevice.SimpleWrite("\n\n");
  atdevice.SimpleWrite(end_c);

  switch(atdevice.WaitResp(10000, 10, "SEND OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  
  
 //delay(2000);
  	#ifdef DEBUG_ON
		DebugPrintln("DB:SENT");
	#endif	
#if 0
  int res= atdevice.read(result, resultlength);
#else
	byte status;
	bool success = false;
	// 1 sec. for initial comm tmout (some PHP mailer takes 5 secs)
    // and max. 150 msec. for inter character timeout
	atdevice.RxInit(15000, 2000); 
	// wait response is finished
	do
	{
		if (atdevice.IsStringReceived("OK"))
		{ 
			// perfect - we have some response, but what:
			status = RX_FINISHED;
			success = true;
			break; // so finish receiving immediately and let's go to 
             // to check response 
		}
		status = atdevice.IsRxFinished();
	} while (status == RX_NOT_FINISHED);
	if (success)
	{
		memcpy(result,atdevice.comm_buf,resultlength);
		res = 30; // covers 1st line
	}
	else
		res = 0;
#endif
  //atdevice.disconnectTCP();
  
  //int res=1;
  return res;
}

int inetGSM::httpPOST(const char* server, int port, const char* path, const char* parameters, char* result, int resultlength)
{
  boolean connected=false;
  int n_of_at=0;
  char itoaBuffer[8];
  int num_char;
  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';

  while(n_of_at<3){
	  if(!connectTCP(server, port)){
	  	#ifdef DEBUG_ON
			DebugPrintln("DB:NOT CONN");
		#endif	
	    	n_of_at++;
	  }
	  else{
		connected=true;
		n_of_at=3;
	}
  }

  if(!connected) return 0;
	
  atdevice.SimpleWrite("POST ");
  atdevice.SimpleWrite(path);
  atdevice.SimpleWrite(" HTTP/1.1\nHost: ");
  atdevice.SimpleWrite(server);
  atdevice.SimpleWrite("\n");
  atdevice.SimpleWrite("User-Agent: Arduino\n");
  atdevice.SimpleWrite("Content-Type: application/x-www-form-urlencoded\n");
  atdevice.SimpleWrite("Content-Length: ");
  itoa(strlen(parameters),itoaBuffer,10);
  atdevice.SimpleWrite(itoaBuffer);
  atdevice.SimpleWrite("\n\n");
  atdevice.SimpleWrite(parameters);
  atdevice.SimpleWrite("\n\n");
  atdevice.SimpleWrite(end_c);
 
  switch(atdevice.WaitResp(10000, 10, "SEND OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

 delay(50);
	#ifdef DEBUG_ON
		DebugPrintln("DB:SENT");
	#endif	

  int res= atdevice.read(result, resultlength);
  //atdevice.disconnectTCP();
  return res;
}

int inetGSM::openmail(char* server, char* loginbase64, char* passbase64, char* from, char* to, char* subj)
{
  boolean connected=false;
  int n_of_at=0;
  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';
	
  while(n_of_at<3){
	  if(!connectTCP(server, 25)){
	  	#ifdef DEBUG_ON
			DebugPrintln("DB:NOT CONN");
		#endif	
	    	n_of_at++;
	  }
	  else{
		connected=true;
		n_of_at=3;
	}
  }

  if(!connected) return 0;  
  
	delay(100);   
    atdevice.SimpleWrite("HELO ");  atdevice.SimpleWrite(server);  atdevice.SimpleWrite("\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("SEND OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100);
	
	delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}
    atdevice.SimpleWrite("AUTH LOGIN\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100);
		
	delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}
    atdevice.SimpleWrite(loginbase64);atdevice.SimpleWrite("\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100);

	delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}	
    atdevice.SimpleWrite(passbase64);atdevice.SimpleWrite("\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100);
	

	delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}
    atdevice.SimpleWrite("MAIL From: <");atdevice.SimpleWrite(from);atdevice.SimpleWrite(">\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100, "");
	
	delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}
    atdevice.SimpleWrite("RCPT TO: <");atdevice.SimpleWrite(to);atdevice.SimpleWrite(">\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100, "");

		delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}
    atdevice.SimpleWrite("Data\n");
	atdevice.SimpleWrite(end_c);
	atdevice.WaitResp(5000, 100, "OK");
	if(!atdevice.IsStringReceived("OK"))
		return 0;
    delay(500);
	atdevice.WaitResp(5000, 100, "");

	delay(100);   
	atdevice.SimpleWriteln("AT+CIPSEND");
	switch(atdevice.WaitResp(5000, 200, ">")){
		case RX_TMOUT_ERR: 
			return 0;
			break;
		case RX_FINISHED_STR_NOT_RECV: 
			return 0; 
			break;
	}
    atdevice.SimpleWrite("Subject: ");atdevice.SimpleWrite(subj);atdevice.SimpleWrite("\n\n");
	
    return 1;
}
int inetGSM::closemail()
{
	char end_c[2];
	end_c[0]=0x1a;
	end_c[1]='\0';
  
	atdevice.SimpleWrite("\n.\n");
	atdevice.SimpleWrite(end_c);
	disconnectTCP();
	return 1;
}
 

int inetGSM::attachGPRS(char* domain, char* dom1, char* dom2)
{
   int i=0;
   delay(5000);
   
  //atdevice._tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.
  atdevice.WaitResp(50, 50);
  atdevice.SimpleWriteln("AT+CIFSR");
  if(atdevice.WaitResp(5000, 50, "ERROR")!=RX_FINISHED_STR_RECV){
  	#ifdef DEBUG_ON
		DebugPrintln("DB:ALREADY HAVE AN IP");
	#endif
	  atdevice.SimpleWriteln("AT+CIPCLOSE");
	atdevice.WaitResp(5000, 50, "ERROR");
	delay(2000);
	atdevice.SimpleWriteln("AT+CIPSERVER=0");
	atdevice.WaitResp(5000, 50, "ERROR");
	return 1;
  }
  else{

	#ifdef DEBUG_ON
		DebugPrintln("DB:STARTING NEW CONNECTION");
	#endif
  
  atdevice.SimpleWriteln("AT+CIPSHUT");
  
  switch(atdevice.WaitResp(500, 50, "SHUT OK")){

	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
	#ifdef DEBUG_ON
		DebugPrintln("DB:SHUTTED OK");
	#endif
	 delay(1000);
	 
  atdevice.SimpleWrite("AT+CSTT=\"");
  atdevice.SimpleWrite(domain);
  atdevice.SimpleWrite("\",\"");
  atdevice.SimpleWrite(dom1);
  atdevice.SimpleWrite("\",\"");
  atdevice.SimpleWrite(dom2);
  atdevice.SimpleWrite("\"\r");  

  
  switch(atdevice.WaitResp(500, 50, "OK")){

	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
	#ifdef DEBUG_ON
		DebugPrintln("DB:APN OK");
	#endif
	 delay(5000);
	  
	atdevice.SimpleWriteln("AT+CIICR");  

  switch(atdevice.WaitResp(10000, 50, "OK")){
	case RX_TMOUT_ERR: 
		return 0; 
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  	#ifdef DEBUG_ON
		DebugPrintln("DB:CONNECTION OK");
	#endif

  delay(1000);


 atdevice.SimpleWriteln("AT+CIFSR");
 if(atdevice.WaitResp(5000, 50, "ERROR")!=RX_FINISHED_STR_RECV){
	#ifdef DEBUG_ON
		DebugPrintln("DB:ASSIGNED AN IP");
	#endif
	atdevice.setStatus(atdevice.ATTACHED);
	return 1;
}
	#ifdef DEBUG_ON
		DebugPrintln("DB:NO IP AFTER CONNECTION");
	#endif
 return 0;
 }
}

int inetGSM::detachGPRS()
{
  if (atdevice.getStatus()==atdevice.IDLE) return 0;
   
  //atdevice._tf.setTimeout(_GSM_CONNECTION_TOUT_);

  //_cell.flush();

  //GPRS dettachment.
  atdevice.SimpleWriteln("AT+CGATT=0");
  if(atdevice.WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV)
  {
    atdevice.setStatus(atdevice.ERROR);
    return 0;
  }
  delay(500);
  
  // Commented in initial trial code!!
  //Stop IP stack.
  //_cell << "AT+WIPCFG=0" <<  _DEC(cr) << endl;
  //	if(!atdevice._tf.find("OK")) return 0;
  //Close GPRS bearer.
  //_cell << "AT+WIPBR=0,6" <<  _DEC(cr) << endl;

  atdevice.setStatus(atdevice.READY);
  return 1;
}

int inetGSM::connectTCP(const char* server, int port)
{
  //atdevice._tf.setTimeout(_TCP_CONNECTION_TOUT_);

  //Status = ATTACHED.
  //if (getStatus()!=ATTACHED)
    //return 0;

  //_cell.flush();
  
  //Visit the remote TCP server.
   atdevice.SimpleWrite("AT+CIPSTART=\"TCP\",\"");
   atdevice.SimpleWrite(server);
   atdevice.SimpleWrite("\",");
   atdevice.SimpleWriteln(port);
  
  switch(atdevice.WaitResp(1000, 200, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  #ifdef DEBUG_ON
	DebugPrintln("DB:RECVD CMD");
  #endif	
  if (!atdevice.IsStringReceived("CONNECT OK")) {
    switch(atdevice.WaitResp(15000, 200, "OK")) {
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
    }
  }

  #ifdef DEBUG_ON
	DebugPrintln("DB:OK TCP");
  #endif

  delay(3000);
  atdevice.SimpleWriteln("AT+CIPSEND");
  switch(atdevice.WaitResp(5000, 200, ">")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  #ifdef DEBUG_ON
	DebugPrintln("DB:>");
  #endif
  delay(4000);
  return 1;
}

int inetGSM::connectUDP(const char* server, int port)
{
  
  //Visit the remote TCP server.
   atdevice.SimpleWrite("AT+CIPSTART=\"UDP\",\"");
   atdevice.SimpleWrite(server);
   atdevice.SimpleWrite("\",");
   atdevice.SimpleWriteln(port);
  
  switch(atdevice.WaitResp(1000, 200, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  #ifdef DEBUG_ON
	DebugPrintln("DB:RECVD CMD");
  #endif	
  if (!atdevice.IsStringReceived("CONNECT OK")) {
    switch(atdevice.WaitResp(15000, 200, "OK")) {
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
    }
  }

  #ifdef DEBUG_ON
	DebugPrintln("DB:OK UDP");
  #endif

#if 0   // let app do encapsulating
  // this is for sending ASCII with ^Z as terminating character
  delay(3000);
  atdevice.SimpleWriteln("AT+CIPSEND");
  switch(atdevice.WaitResp(5000, 200, ">")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  #ifdef DEBUG_ON
	DebugPrintln("DB:>");
  #endif
  delay(4000);
#else
	// select normal data send mode
  atdevice.SimpleWriteln("AT+CIPQSEND=0");
  switch(atdevice.WaitResp(5000, 200, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
#endif
  return 1;
}


int inetGSM::disconnectTCP()
{
  //Close TCP/UDP client and deact.
  atdevice.SimpleWriteln("AT+CIPCLOSE");
  if(atdevice.getStatus()==atdevice.TCPCONNECTEDCLIENT)
      	atdevice.setStatus(atdevice.ATTACHED);
   else
        atdevice.setStatus(atdevice.TCPSERVERWAIT);   
    return 1;
}

int inetGSM::connectTCPServer(int port)
{
/*
  if (getStatus()!=ATTACHED)
     return 0;
*/
  //atdevice._tf.setTimeout(_GSM_CONNECTION_TOUT_);

  //_cell.flush();

  // Set port
  
  atdevice.SimpleWrite("AT+CIPSERVER=1,");
  atdevice.SimpleWriteln(port);
/*
  switch(atdevice.WaitResp(5000, 50, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  switch(atdevice.WaitResp(5000, 50, "SERVER")){ //Try SERVER OK
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
*/
  //delay(200);  

  return 1;

}

boolean inetGSM::connectedClient()
{
  /*
  if (getStatus()!=TCPSERVERWAIT)
     return 0;
  */
  
   atdevice.SimpleWriteln("AT+CIPSTATUS");
  // Alternative: AT+QISTAT, although it may be necessary to call an AT 
  // command every second,which is not wise
  /*
  switch(atdevice.WaitResp(1000, 200, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }*/
  //atdevice._tf.setTimeout(1);
  if(atdevice.WaitResp(5000, 50, "CONNECT OK")!=RX_FINISHED_STR_RECV)
  {
    atdevice.setStatus(atdevice.TCPCONNECTEDSERVER);
    return true;
  }
  else
    return false;
 }
bool inetGSM::OpenGprs(int CID,char *apn,int responsetime)
{
	bool success = false;
	cid = CID;
	//AT+CGATT=1
	//sprintf(printbuf,"AT+SAPBR=3,%d,\"CONTYPE\",\"GPRS\"\r",cid);
	strcpy(printbuf,"AT+CGATT=1\r");
#ifdef DEBUG_ON
	DebugPrintln("DEBUG:Inet OPENGPRS");
	DebugPrintln(printbuf);
#endif
	if (writewithtimeout(printbuf,OK,10000,1000))
	{
		delay(2000);
		//AT+CGDCONT=1,"IP","uinternet"
	//	sprintf(printbuf,"AT+SAPBR=3,%d,\"APN\",\"%s\"\r",cid,apn);
		sprintf(printbuf,"AT+CGDCONT=%d,\"IP\",\"%s\"\r",cid,apn);
#ifdef DEBUG_ON
		DebugPrintln("DEBUG:Inet OPENGPRS");
		DebugPrintln(printbuf);
#endif
		if (writewithtimeout(printbuf,OK,10000,150))
		{
			delay(2000);
			//AT+CGACT=1,1
//			sprintf(printbuf,"AT+SAPBR=1,%d\r",cid);
			sprintf(printbuf,"AT+CGACT=1,%d\r",cid);
#ifdef DEBUG_ON
			DebugPrintln(printbuf);
			DebugPrintln(responsetime);
#endif
			success = (writewithtimeout(printbuf,OK,responsetime*1000,responsetime*1000));
		}
	}
	return success;
}

bool inetGSM::CloseGprs(int CID)
{
//	sprintf(printbuf,"AT+SAPBR=0,%d\r",CID);
	sprintf(printbuf,"AT+CGACT=0,%d\r",CID);
#ifdef DEBUG_ON
	DebugPrintln("DEBUG:Inet CLOSEGPRS");
	DebugPrintln(printbuf);
#endif
	return writewithtimeout(printbuf,"+CGREG: 1",1000,150);
}

char *inetGSM::GetGprsIP(int CID)
{
	char *cs,*ce;
	sprintf(printbuf,"AT+SAPBR=2,%d\r",CID);
#ifdef DEBUG_ON
	DebugPrintln("DEBUG:Inet GetGPRSIP");
	DebugPrintln(printbuf);
#endif
	if  (writewithtimeout(printbuf,OK,1000,150))
	{
		// reply like this +SAPBR: 1,1,"10.173.225.36"
#ifdef DEBUG_ON
		DebugPrintln("DEBUG:Inet reply");
		DebugPrintln((char *)atdevice.comm_buf);
#endif
		cs = strchr((char *)atdevice.comm_buf,'\"');
		ce = strchr(++cs,'\"');
		*ce = 0;
		return cs;
	}
	else
		return 0;
}
