#ifndef GENERICGSM_H
#define GENERICGSM_H
#include "config.h"

#ifdef SWSERIAL
#include <SoftwareSerial.h>
#else
#include "hwSerial.h"
#endif
#include "AT.h"
#include "GenericStrings.h"

class GENERICGSM : public virtual AT {

private:
   //  int configandwait(char* pin);
   //  int setPIN(char *pin);
  //   int changeNSIPmode(char);

public:
    GENERICGSM();
    ~GENERICGSM();
 //    int getCCI(char* cci);
     bool gIMEI(char* imei);
	 // phonebook methods
    AT_Phonebook_e GetPhoneNumber(byte position, char *phone_number);
    char PutPhoneNumber(byte position, char *phone_number);
    char DelPhoneNumber(byte position);
    char ComparePhoneNumber(byte position, char *phone_number);
    bool isRegistered(void);
    bool isInitialized(void);
	registration_ret_val_enum checkRegistration(void);
 //    int sendSMS(const char* to, const char* msg);
//     boolean readSMS(char* msg, int msglength, char* number, int nlength);
//     boolean readCall(char* number, int nlength);
//     boolean call(char* number, unsigned int milliseconds);
     char forceON();
     virtual int read(char* result, int resultlength);
     virtual uint8_t read();
     virtual int available();
     int readCellData(int &mcc, int &mnc, long &lac, long &cellid);
     void SimpleRead();
     void WhileSimpleRead();
     void SimpleWrite(char *comm);
     void SimpleWrite(char comm);
     void SimpleWrite(char const *comm);
     void SimpleWrite(int comm);
     void SimpleWrite(const __FlashStringHelper *pgmstr);
     void SimpleWriteln(char *comm);
     void SimpleWriteln(char const *comm);
     void SimpleWriteln(const __FlashStringHelper *pgmstr);
     void SimpleWriteln(int comm);
};

extern GENERICGSM atdevice;

#endif

