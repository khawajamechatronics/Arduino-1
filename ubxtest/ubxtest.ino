#include <ubx.h>

#define HOT_START
#define M8
//#define M6

#define INI_OFFSET 0
#define ALM_COUNT 60 // count of alm blocks
#define ALM_START_OFFSET 70  // wil

// values for MAG_INI_TIMEUTC
unsigned INI_YEAR = 2016;
unsigned INI_MONTH =5;
unsigned INI_DAY = 15;
unsigned INI_HOUR = 8;
unsigned INI_MINUTE = 59;
unsigned INI_SECOND = 0;

struct UbxHeader *pH;
struct sUBX_NAV_STATUS *pUNStat;
struct sUBX_NAV_TIMEUTC *pUNTuc;
struct sUBX_NAV_POSLLH *pUNllh;
struct sUBX_NAV_AOP_STATUS *pUNOS;
struct sAID_POLL GeneralPoll = {{SYNC1,SYNC2,0,0,0},{0,0}};
struct sAID_INI_DATA *pAId;
struct sAID_ALM_DATA_1 *pAAd1;
struct sAID_ALM_DATA_8 *pAAd8;
struct sAID_ALM_DATA_40 *pAAd40;
struct sMGA_INI_TIME_UTC MITU = {{SYNC1,SYNC2,MGA,MGA_INI,24},0x10,0,0,-128};
struct sMGA_INI_ACK *pMIa;

UBX ubx;
byte buffer[200];  // work area
byte lastfix = 0xff;
char pbuf[50];
int almoffset = ALM_START_OFFSET;
volatile byte almcount = 10;
bool assitnowsent = false;
bool aopidle = false; //assume the worst
//SoftwareSerial mySerial(2, 3); // RX, TX
#define OTHER_SERIAL Serial1 

void setup() {
  char cc[20];
  // put your setup code here, to run once:
  Serial.begin(115200);
  OTHER_SERIAL.begin(9600);
  ubx.begin(buffer,sizeof(buffer));
#if 0
  for (int i=0; i<sizeof(msg1);i++)
    if (ubx.parse(msg1[i]))
      Serial.println("Complete message");
  Serial.println("No more data");
  // now create a checksum from a message
  ubx.Checksum(&msg1[2],sizeof(msg1)-4);
  sprintf(cc,"%02X %02X",ubx.CK_A,ubx.CK_B);
  Serial.println(cc);
#endif
 //   EEPROM.put(ALM_COUNT,almcount);
}

unsigned long latrtcupdate = 0;
void MyRTC()
{
    // adjust by amount of time since start up
  // allow for rollover at midnight but not rollover at end of month
  int adjust = ((latrtcupdate - millis())+500)/1000;
  latrtcupdate = millis();
  INI_SECOND += adjust % 60;
  if (INI_SECOND > 59)
  {
    INI_MINUTE++;
    INI_SECOND %= 60;
  }
  INI_MINUTE += adjust/60;
  if (INI_MINUTE > 59)
  {
    INI_HOUR++;
    INI_MINUTE %= 60;
  }
  if (INI_HOUR > 23)
  {
    INI_DAY++;
    INI_HOUR %= 24;
  }
  sprintf(pbuf,"MYRTC %02u/%02u/%04u %02u:%02u:%02u",
    INI_MONTH,INI_DAY,INI_YEAR,INI_HOUR,INI_MINUTE,INI_SECOND);
  Serial.println(pbuf);
}
void PollAnyMessage(uint8_t c, uint8_t i)
{
  GeneralPoll.header.classx = c;
  GeneralPoll.header.id = i;
  ubx.Checksum((byte *)&GeneralPoll.header.classx,sizeof(GeneralPoll)-4);
  GeneralPoll.cs.ck_a = ubx.CK_A;
  GeneralPoll.cs.ck_b = ubx.CK_B;
  OTHER_SERIAL.write((byte *)&GeneralPoll,sizeof(GeneralPoll));
}
bool AssistNowAutonomous(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second,unsigned accuracy)
{
  if (!aopidle)
  {
    // poll aopstatus until idle
    PollAnyMessage(NAV,NAV_AOPSTATUS);
    return false;
  }
  else
  {
#ifdef M6
  // data was saved at 19:07:02 GMT so we have to bump up AId.tow by correct amount
    EEPROM.get(ALM_OFFSET,AAd);
    sprintf(pbuf,"%02x %02x %02x %02x %u",AAd.header.sync1,AAd.header.sync2,AAd.header.classx,AAd.header.id,AAd.header.plength);
    Serial.println(pbuf);
    ubx.Checksum((byte *)&AAd.header.classx,sizeof(AAd)-4);
    AAd.cs.ck_a = ubx.CK_A;
    AAd.cs.ck_b = ubx.CK_B;
    OTHER_SERIAL.write((byte *)&AAd,sizeof(AAd));
    EEPROM.get(INI_OFFSET,AId);
    sprintf(pbuf,"%02x %02x %02x %02x %u",AId.header.sync1,AId.header.sync2,AId.header.classx,AId.header.id,AId.header.plength);
    Serial.println(pbuf);
    // run this app at 19:30:02 i.e 23*60*1000 msecs later
    AId.tow += (23*60*1000);
    ubx.Checksum((byte *)&AId.header.classx,sizeof(AId)-4);
    AId.cs.ck_a = ubx.CK_A;
    AId.cs.ck_b = ubx.CK_B;
    OTHER_SERIAL.write((byte *)&AId,sizeof(AId));
#endif
#ifdef M8
    // final version gets current time from NTP server
    MITU.year = year;
    MITU.month = month;
    MITU.day = day;
    MITU.hour = hour;
    MITU.minute = minute;
    MITU.second = second;
    MITU.ns = 0;
    MITU.tAccS = accuracy;
    ubx.Checksum((byte *)&MITU.header.classx,sizeof(MITU)-4);
    MITU.cs.ck_a = ubx.CK_A;
    MITU.cs.ck_b = ubx.CK_B;
    OTHER_SERIAL.write((byte *)&MITU,sizeof(MITU));
#endif
    return true;
  }
}
void NAVhandler()
{
  switch (pH->id)
  {
    case NAV_POSLLH:
      pUNllh = (struct sUBX_NAV_POSLLH *)buffer;
      sprintf(pbuf,"lat %ld.%05lu long %ld.%05lu alt %ld",
        pUNllh->latitude/10000000,pUNllh->latitude%10000000,
        pUNllh->longitude/10000000,pUNllh->longitude%10000000,
        pUNllh->height/1000);
      Serial.println(pbuf);
      break;
    case NAV_STATUS:
      pUNStat = (struct sUBX_NAV_STATUS *)buffer;
      sprintf(pbuf,"itow %lu fix %d fl %02X",pUNStat->iTOW,pUNStat->gpsFix,pUNStat->flags);
      Serial.println(pbuf);
#if 0
      if (lastfix != 3 && pUNStat->gpsFix == 3)
      {
        // get ALM data by polling
        PollAnyMessage(AID,INI);
      }
#endif
      if (!assitnowsent && pUNStat->gpsFix < 2)  // we need help
      {
        assitnowsent = AssistNowAutonomous(INI_YEAR,INI_MONTH,INI_DAY,
        INI_HOUR,INI_MINUTE,INI_SECOND,10);
      }
      break;
    case NAV_TIMEUTC:
      pUNTuc = (struct sUBX_NAV_TIMEUTC *)buffer;
      if (true /*(pUNTuc->valid & validUTC) == validUTC*/)
      {
        sprintf(pbuf,"V %02X %02u/%02u/%04u %02u:%02u:%02u",pUNTuc->valid,
          pUNTuc->day,pUNTuc->month,pUNTuc->year,
          pUNTuc->hour,pUNTuc->minute,pUNTuc->second);                
        Serial.println(pbuf);
      }
      MyRTC();
      break;
    case NAV_AOPSTATUS:
      pUNOS = (struct sUBX_NAV_AOP_STATUS *)buffer;
      aopidle = (pUNOS->status == 0);
      sprintf(pbuf,"AOP %s",aopidle ? "idle" : "busy");
      Serial.println(pbuf);
      break;
    default:
      sprintf(pbuf,"NAV? %u",pH->id);
      Serial.println(pbuf);
      break;

  }
}
void AIDhandler()
{
  switch(pH->id)
  {
    case AID_INI:
      lastfix = 3; // stop getting this message
      pAId = (struct sAID_INI_DATA *)buffer;
      if ((pAId->flags & 0x10) != 0) // LLA format
        sprintf(pbuf,"lat %ld.%05lu long %ld.%05lu alt %ld posacc %ld",
          pAId->ecefXOrLat/10000000,pAId->ecefXOrLat%10000000,
          pAId->ecefYOrLon/10000000,pAId->ecefYOrLon%10000000,
          pAId->ecefZOrAlt/100,pAId->posAcc);
      else // ECEF
        sprintf(pbuf,"lat %ld.%02lu long %ld.%02lu alt %ld posacc %ld",
          pAId->ecefXOrLat/100,pAId->ecefXOrLat%100,
          pAId->ecefYOrLon/100,pAId->ecefYOrLon%100,
          pAId->ecefZOrAlt/100,pAId->posAcc);
      Serial.println(pbuf);
      // write to EEPROM
//      EEPROM.put(INI_OFFSET,*pAId);
      // now poll for ALM
      // 1 poll will result in many replies
      PollAnyMessage(AID,AID_ALM);
      break;
    case AID_ALM:
      // which block do we have?
      switch (pH->plength)
      {
        case 1:
          pAAd1 = (struct sAID_ALM_DATA_1 *)buffer;
          sprintf(pbuf,"l %u svid %u",pAAd1->header.plength,pAAd1->svid);
          Serial.println(pbuf);
          // write to EEPROM
//          EEPROM.put(almoffset,*pAAd1);
          almoffset += sizeof(struct sAID_ALM_DATA_1);
          break;
        case 8:
          pAAd8 = (struct sAID_ALM_DATA_8 *)buffer;
          sprintf(pbuf,"l %u svid %u",pAAd8->header.plength,pAAd8->svid);
          Serial.println(pbuf);
          // write to EEPROM
//            EEPROM.put(almoffset,*pAAd8);
          almoffset += sizeof(struct sAID_ALM_DATA_8);
          break;
        case 40:
          pAAd40 = (struct sAID_ALM_DATA_40 *)buffer;
          sprintf(pbuf,"l %u svid %u",pAAd40->header.plength,pAAd40->svid);
          Serial.println(pbuf);
          // write to EEPROM
//           EEPROM.put(almoffset,*pAAd40);
          almoffset += sizeof(struct sAID_ALM_DATA_40);
          break;
      }
      almcount++;
      sprintf(pbuf,"A %u",almcount);
      Serial.println(pbuf);
//        EEPROM.put(ALM_COUNT,almcount);
      break;
    default:
      sprintf(pbuf,"AID? %u",pH->id);
      Serial.println(pbuf);
      break;

  }
}

void MGAhandler()
{
  switch (pH->id)
  {
    case MGA_ACK:
      pMIa = (struct sMGA_INI_ACK *)buffer;
      sprintf(pbuf,"MGA_ACK t %u c %u i %u",pMIa->type,pMIa->errorCode,pMIa->msgId);
      Serial.println(pbuf);
      break;
    default:
      sprintf(pbuf,"MGA? %u",pH->id);
      Serial.println(pbuf);
      break;
  }
}
void loop() {
//  byte b;
  // put your main code here, to run repeatedly:
  if (OTHER_SERIAL.available())
  {
 //   b = OTHER_SERIAL.read();
    if (ubx.parse(OTHER_SERIAL.read()))
    {
      pH = (struct UbxHeader *)buffer;
      sprintf(pbuf,"rcv C %02X I %02X",pH->classx,pH->id);
      Serial.println(pbuf);
      switch(pH->classx)
      {
        case AID:
          AIDhandler();
          break;
        case NAV:
          NAVhandler();
          break;
        case MGA:
          MGAhandler();
          break;
      }
    }
  }
  if (Serial.available())
    OTHER_SERIAL.write(Serial.read());
}
