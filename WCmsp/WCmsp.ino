#include "EEPROM.h"
#include "WCmap.h"
char *fn = "Henry";
char *sl = "0545919886,0545289886";
char *apn="uinternet";
char *em = "davidh@zickel.net";
char *sm = "aspmx.l.google.com";
char *pw = "Nubira2001";
char *login = "mgadriver@gmail.com";

char buff[100];
char printbuf[100];

void setnextalarm(long a)
{
  
}
void setup() {
  int i;
  long j;
  // put your setup code here, to run once:
  Serial.begin(115200);
  WCeepromInit();
  Serial.println(WCmapWrite(iFriendlyName,strlen(fn)+1,fn));
  Serial.println(WCmapWrite(iSubscriberList,strlen(sl)+1,sl));
  Serial.println(WCmapWrite(iKeepAlive,24*3600L));
  Serial.println(WCmapWrite(iAlarmIndex,-1)); 
  Serial.println(WCmapWrite(iLeakElapsedTime,29L)); 
  Serial.println(WCmapWrite(iAPN,strlen(apn)+1,apn));
  Serial.println(WCmapWrite(iEmail,strlen(em)+1,em));
  Serial.println(WCmapWrite(iSMTPserver,strlen(sm)+1,sm));
  Serial.println(WCmapWrite(iPW,strlen(pw)+1,pw));
  Serial.println(WCmapWrite(iLogin,strlen(login)+1,login));
  //
 // for (i=0;i<iLeakElapsedTime+4;i++)
//  {
  //  sprintf(printbuf,"%02X",EEPROM.read(i));
 //   Serial.print(printbuf);    
 // }
//  Serial.println();
  i = WCmapRead(iFriendlyName,sizeof(buff),buff);
  sprintf(printbuf,"Friendy %d %s",i,buff);
  Serial.println(printbuf);
  i = WCmapRead(iSubscriberList,sizeof(buff),buff);
  sprintf(printbuf,"Subscribers %d %s",i,buff);
  Serial.println(printbuf);
  i = WCmapRead(iKeepAlive,&j);
  sprintf(printbuf,"KA %d value %ld",i,j);
  Serial.println(printbuf);
  i = WCmapRead(iLeakElapsedTime,&j);
  sprintf(printbuf,"ET %d value %ld",i,j);
  Serial.println(printbuf);
  i = WCmapRead(iAPN,sizeof(buff),buff);
  sprintf(printbuf,"APN %d %s",i,buff);
  Serial.println(printbuf);
  i = WCmapRead(iEmail,sizeof(buff),buff);
  sprintf(printbuf,"email %d %s",i,buff);
  Serial.println(printbuf);
  i = WCmapRead(iSMTPserver,sizeof(buff),buff);
  sprintf(printbuf,"smtp %d %s",i,buff);
  Serial.println(printbuf);
  i = WCmapRead(iPW,sizeof(buff),buff);
  sprintf(printbuf,"pw %d %s",i,buff);
  Serial.println(printbuf);
  i = WCmapRead(iLogin,sizeof(buff),buff);
  sprintf(printbuf,"login %d %s",i,buff);
  Serial.println(printbuf);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
