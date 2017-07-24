/*
 *  Water control uses EEPROM to store operational data
 *  Amount varies with model, we assume 1024 bytes which is avaliable on all models
 */
#ifndef WCEEPROM_H
#define ECEEPROM_H

 struct eEEPROMmap 
 {
  unsigned Tpl0;          // 0
  unsigned LeakInSecs;    // 1
  char DefaultPhone[20];  // 2
  char SMTPServer[30];    // 3
  char EmailAccount[30];  // 4
  char EmailPassword[20]; // 5 yeah I know its unsafe
  char EmailRecipient[30];// 6
  char Apn[20];           // 7
  bool sendSMS;           // 8
  uint32_t reportRate;    // 9 send report every N seconds;
  bool polarity;          // a
  unsigned pulsewidth;    // b
  char HTTPServer[30];    // c
  bool useSSL;            // d
  int gprstimeout;        // e
  unsigned Tpl1;          // f
  char wwwpath[20];      // g
//  unsigned reboottime;    //h
 };

//enum eEEPROMIndex {TPL0,LIS,DP,SmS,EA,EP,ER,APN,SENDSMS,RR,POL,PW,HTTPs,useSSL,GPRSTO,TPL1,WWWPATH,REBOOT};
enum eEEPROMIndex {TPL0,LIS,DP,SmS,EA,EP,ER,APN,SENDSMS,RR,POL,PW,HTTPs,useSSL,GPRSTO,TPL1,WWWPATH};
void EEPROMSetIndex(enum eEEPROMIndex i,char * s);
char *EEPROMGetIndex(enum eEEPROMIndex i);

void InitEepromData(void);
void PrintEepromData(void);
char * EEPROMGetIndex(enum eEEPROMIndex i);
void EEPROMSetIndex(enum eEEPROMIndex i,char * s);
void eepromgetarray(int startaddress,uint8_t *target,int l);
void eepromsetarray(int startaddress,uint8_t *source,int l);
void eepromgetstring(int startaddress,char *target,int maxlength);
void eepromsetstring(int startaddress,char *source,int maxlength);
#endif

