#define iFriendlyName 0  // up tp 20 characters
#define iSubscriberList 20 // Up to 60 chars including commas
#define iKeepAlive 80
#define iAlarmIndex 84 // index to next alarmarray, -1 if empty
#define iAlarmSize 5
#define iAlarmArray (iAlarmIndex+sizeof(long)) // 88 array of 5 longs
#define iLeakElapsedTime (iAlarmArray+(5*sizeof(long))) // in seconds 108
#define iAPN (iLeakElapsedTime+sizeof(long))
#define iEmail (iAPN+20)
#define iSMTPserver (iEmail+30)
#define iPW (iSMTPserver+30)
#define iLogin (iPW+30)

int WCmapWrite(unsigned offset,unsigned buflength,char *s);
int WCmapWrite(unsigned offset,long n);
int WCmapRead(unsigned offset,unsigned buflength,char *s);
int WCmapRead(unsigned offset,long *n);
void WCeepromInit();
int WCsetAlarm(long t);
long WCgetAlarm();
long WCgetAlarm(int);

