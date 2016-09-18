#include "GenericGSM.h"
#include "Streaming.h"

#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

//#define RESETPIN 7

GENERICGSM atdevice;
GENERICGSM::GENERICGSM() {};
GENERICGSM::~GENERICGSM() {};

/**********************************************************
Function: 	This function premits to wake up the module
			(only for SIM908) when it goes in energy saving
			mode.

Author:		Marco Martines
Created:	unknown
Modified:	18/02/2014

Output:		none

Comments:	It would be nice to call this function
 			automatically when atdevice.begin is called (of course
 			only if a SIM908 is used). 
**********************************************************/

char GENERICGSM::forceON()
{
	 char ret_val=0;
     char *p_char;
     char *p_char1;

     SimpleWriteln(F("AT+CREG?"));
     WaitResp(5000, 100, str_ok);
     if(IsStringReceived(str_ok)) {
          ret_val=1;
     }
     
     p_char = strchr((char *)(atdevice.comm_buf),',');
     p_char1 = p_char+1;
     *(p_char1+2)=0;
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }

     if((*p_char1)=='4') {
#ifdef AT_DEVICE_ON_PIN         
			deviceon(AT_DEVICE_ON_PIN);
#endif
          ret_val=2;
     }

     return ret_val;
}

#if 0
int GENERICGSM::configandwait(char* pin)
{
     int connCode;
     //_tf.setTimeout(_GSM_CONNECTION_TOUT_);

     if(pin) setPIN(pin); //syv

     // Try 10 times to register in the network. Note this can take some time!
     for(int i=0; i<10; i++) {
          //Ask for register status to GPRS network.
          SimpleWriteln(F("AT+CGREG?"));

          //Se espera la unsolicited response de registered to network.
          while(atdevice.WaitResp(5000, 50, "+CGREG: 0,")!=RX_FINISHED_STR_RECV)
               //while (_tf.find("+CGREG: 0,"))  // CHANGE!!!!
          {
               //connCode=_tf.getValue();
               connCode=_cell.read();
               if((connCode==1)||(connCode==5)) {
                    setStatus(READY);

                    SimpleWriteln(F("AT+CMGF=1")); //SMS text mode.
                    delay(200);
                    // Buah, we should take this to readCall()
                    SimpleWriteln(F("AT+CLIP=1")); //SMS text mode.
                    delay(200);
                    //_cell << "AT+QIDEACT" <<  _DEC(cr) << endl; //To make sure not pending connection.
                    //delay(1000);

                    return 1;
               }
          }
     }
     return 0;
}
#endif

/**
 * GENERICGSM::read(char* buffer, int buffersize)
 *
 * Waits for data to be readable from the atdevice module, reads data until
 * no more is available or the buffer has been filled
 *
 * returns number of bytes read
 *
 */
int GENERICGSM::read(char* result, int resultlength)
{
     char temp;
     int i=0;

#ifdef DEBUG_ON
     Serial.print(F("Starting read..\nWaiting for Data.."));
#endif
     // Wait until we start receiving data
     while(atdevice.available()<1) {
          delay(100);
#ifdef DEBUG_ON
          Serial.print(F("."));
#endif
     }

     while(atdevice.available()>0 && i<(resultlength-1)) {
          temp=_cell.read();
          if(temp>0) {
#ifdef DEBUG_ON
               Serial.print(temp);
#endif
               result[i]=temp;
               i++;
          }
          delay(1);
     }

     // Terminate the string
     result[resultlength-1]='\0';

#ifdef DEBUG_ON
     Serial.println(F("\nDone.."));
#endif
     return i;
}

int GENERICGSM::readCellData(int &mcc, int &mnc, long &lac, long &cellid)
{
     if (getStatus()==IDLE)
          return 0;

     //_tf.setTimeout(_GSM_DATA_TOUT_);
     //_cell.flush();
     SimpleWriteln(F("AT+QENG=1,0"));
     SimpleWriteln(F("AT+QENG?"));
     if(atdevice.WaitResp(5000, 50, "+QENG")!=RX_FINISHED_STR_NOT_RECV)
          return 0;

     //mcc=_tf.getValue(); // The first one is 0
     mcc=_cell.read();
     //mcc=_tf.getValue();
     mcc=_cell.read();
     //mnc=_tf.getValue();
     mnc=_cell.read();
     //lac=_tf.getValue();
     lac=_cell.read();
     //cellid=_tf.getValue();
     cellid=_cell.read();

     atdevice.WaitResp(5000, 50, "+OK");
     SimpleWriteln(F("AT+QENG=1,0"));
     atdevice.WaitResp(5000, 50, "+OK");
     return 1;
}

#if 0
boolean GENERICGSM::readSMS(char* msg, int msglength, char* number, int nlength)
{
     Serial.println(F("This method is deprecated! Please use GetSMS in the SMS class."));
     long index;
     char *p_char;
     char *p_char1;

     /*
     if (getStatus()==IDLE)
       return false;
     */
#ifdef UNO
     _tf.setTimeout(_GSM_DATA_TOUT_);
#endif
     //_cell.flush();
     WaitResp(500, 500);
     SimpleWriteln(F("AT+CMGL=\"REC UNREAD\",1"));

     WaitResp(5000, 500);
     if(atdevice.IsStringReceived("+CMGL")) {

          //index
          p_char = strchr((char *)(atdevice.comm_buf),'+CMGL');
          p_char1 = p_char+3;  //we are on the first char of string
          p_char = p_char1+1;
          *p_char = 0;
          index=atoi(p_char1);

          p_char1 = p_char+1;
          p_char = strstr((char *)(p_char1), "\",\"");
          p_char1 = p_char+3;
          p_char = strstr((char *)(p_char1), "\",\"");
          if (p_char != NULL) {
               *p_char = 0;
          }
          strcpy(number, (char *)(p_char1));
          //////

          p_char1 = p_char+3;
          p_char = strstr((char *)(p_char1), "\",\"");
          p_char1 = p_char+3;

          p_char = strstr((char *)(p_char1), "\n");
          p_char1 = p_char+1;
          p_char = strstr((char *)(p_char1), "\n");
          if (p_char != NULL) {
               *p_char = 0;
          }
          strcpy(msg, (char *)(p_char1));

          // #ifdef UNO
          // index=_tf.getValue();
          // #endif
          // #ifdef MEGA
          //index=_cell.read();
          // #endif
          // Serial.println("DEBUG");
          // #ifdef UNO
          // _tf.getString("\",\"", "\"", number, nlength);
          // #endif
          // Serial.println("PRIMA");
          // #ifdef MEGA
          // _cell.getString("\",\"", "\"", number, nlength);
          // #endif
          // Serial.println("DEBUG");
          // #ifdef UNO
          // _tf.getString("\n", "\nOK", msg, msglength);
          // #endif
          // #ifdef MEGA
          // _cell.getString("\n", "\nOK", msg, msglength);
          // #endif

          SimpleWrite(F("AT+CMGD="));
          SimpleWriteln(index);
          // Serial.print("VAL= ");
          // Serial.println(index);
          atdevice.WaitResp(5000, 50, str_ok);
          return true;
     };
     return false;
};
#endif

#if 0
boolean GENERICGSM::readCall(char* number, int nlength)
{
     int index;

     if (getStatus()==IDLE)
          return false;

     //_tf.setTimeout(_GSM_DATA_TOUT_);
     if(atdevice.WaitResp(5000, 50, "+CLIP: \"")!=RX_FINISHED_STR_RECV)
          //if(_tf.find("+CLIP: \""))
     {
#ifdef SWSERIAL
          _tf.getString("", "\"", number, nlength);
#else
		  _cell.getString("", "\"", number, nlength);
#endif
          SimpleWriteln(F("ATH"));
          delay(1000);
          //_cell.flush();
          return true;
     };
     return false;
};
#endif

#if 0
boolean GENERICGSM::call(char* number, unsigned int milliseconds)
{
     if (getStatus()==IDLE)
          return false;

     //_tf.setTimeout(_GSM_DATA_TOUT_);

     SimpleWrite(F("ATD"));
     SimpleWrite(number);
     SimpleWriteln(F(";"));
     delay(milliseconds);
     SimpleWriteln(F("ATH"));

     return true;

}
#endif

#if 0
int GENERICGSM::setPIN(char *pin)
{
     //Status = READY or ATTACHED.
     if((getStatus() != IDLE))
          return 2;

     //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

     //_cell.flush();

     //AT command to set PIN.
     SimpleWrite(F("AT+CPIN="));
     SimpleWriteln(pin);

     //Expect str_ok.

     if(atdevice.WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV)
          return 0;
     else
          return 1;
}
#endif
#if 0
int GENERICGSM::changeNSIPmode(char mode)
{
     //_tf.setTimeout(_TCP_CONNECTION_TOUT_);

     //if (getStatus()!=ATTACHED)
     //    return 0;

     //_cell.flush();

     SimpleWrite(F("AT+QIDNSIP="));
     SimpleWriteln(mode);
     if(atdevice.WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV) return 0;
     //if(!_tf.find(str_ok)) return 0;

     return 1;
}
#endif
#if 0
int GENERICGSM::getCCI(char *cci)
{
     //Status must be READY
     if((getStatus() != READY))
          return 2;

     //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

     //_cell.flush();

     //AT command to get CCID.
     SimpleWriteln(F("AT+QCCID"));

     //Read response from modem
#ifdef SWSERIAL
     _tf.getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);
#else
    _cell.getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);
#endif

     //Expect str_ok.
     if(atdevice.WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV)
          return 0;
     else
          return 1;
}
#endif

bool GENERICGSM::gIMEI(char *imei)
{

     //_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

     //_cell.flush();

     //AT command to get IMEI.
     SimpleWriteln(S_GET_IMEI);

     //Read response from modem
#if 0
#ifdef SWSERIAL
     _tf.getString("\r\n","\r\n",imei, 16);
#else
     _cell.getString("\r\n","\r\n",imei, 16);
#endif
#endif
     //Expect str_ok.
     if(atdevice.WaitResp(5000, 50, str_ok)!=RX_FINISHED_STR_NOT_RECV){
     	memcpy(imei,atdevice.comm_buf+2,15);
     	imei[15]='\0';
          return true;
     }else
          return false;
}
int GENERICGSM::available()
{
     return _cell.available();
}

uint8_t GENERICGSM::read()
{
     return _cell.read();
}

void GENERICGSM::SimpleRead()
{
     char datain;
     if(_cell.available()>0) {
          datain=_cell.read();
          if(datain>0) {
               Serial.print(datain);
          }
     }
}

void GENERICGSM::SimpleWrite(char *comm)
{
     _cell.print(comm);
}

void GENERICGSM::SimpleWrite(const char *comm)
{
     _cell.print(comm);
}

void GENERICGSM::SimpleWrite(char comm)
{
     _cell.print(comm);
}
void GENERICGSM::SimpleWrite(int comm)
{
     _cell.print(comm);
}

void GENERICGSM::SimpleWrite(const __FlashStringHelper *pgmstr)
{
     _cell.print(pgmstr);
}

void GENERICGSM::SimpleWriteln(char *comm)
{
     _cell.println(comm);
}

void GENERICGSM::SimpleWriteln(const __FlashStringHelper *pgmstr)
{
     _cell.println(pgmstr);
}

void GENERICGSM::SimpleWriteln(char const *comm)
{
     _cell.println(comm);
}

void GENERICGSM::SimpleWriteln(int comm)
{
     _cell.println(comm);
}

void GENERICGSM::WhileSimpleRead()
{
     char datain;
     while(_cell.available()>0) {
          datain=_cell.read();
          if(datain>0) {
               Serial.print(datain);
          }
     }
}

//---------------------------------------------
/**********************************************************
Turns on/off the speaker

off_on: 0 - off
        1 - on
**********************************************************/
#if 0
void GENERICGSM::SetSpeaker(byte off_on)
{
     if (CLS_FREE != GetCommLineStatus()) return;
     SetCommLineStatus(CLS_ATCMD);
     if (off_on) {
          //SendATCmdWaitResp("AT#GPIO=5,1,2", 500, 50, "#GPIO:", 1);
     } else {
          //SendATCmdWaitResp("AT#GPIO=5,0,2", 500, 50, "#GPIO:", 1);
     }
     SetCommLineStatus(CLS_FREE);
}
#endif


bool GENERICGSM::isRegistered(void)
{
     return (module_status & STATUS_REGISTERED) == STATUS_REGISTERED;
}
bool GENERICGSM::isInitialized(void)
{
     return (module_status & STATUS_INITIALIZED) == STATUS_INITIALIZED;
}

/**********************************************************
Method checks if the GENERICGSM module is registered in the GENERICGSM net
- this method communicates directly with the GENERICGSM module
  in contrast to the method IsRegistered() which reads the
  flag from the module_status (this flag is set inside this method)

- must be called regularly - from 1sec. to cca. 10 sec.

return values:
      REG_NOT_REGISTERED  - not registered
      REG_REGISTERED      - GENERICGSM module is registered
      REG_NO_RESPONSE     - GENERICGSM doesn't response
      REG_COMM_LINE_BUSY  - comm line between GENERICGSM module and Arduino is not free
                            for communication
**********************************************************/
registration_ret_val_enum GENERICGSM::checkRegistration(void)
{
     rx_state_enum status;
     registration_ret_val_enum ret_val = REG_NOT_REGISTERED;

     if (CLS_FREE != GetCommLineStatus()) return (REG_COMM_LINE_BUSY);
     SetCommLineStatus(CLS_ATCMD);
     _cell.println(F("AT+CREG?"));
     // 5 sec. for initial comm tmout
     // 50 msec. for inter character timeout
     status = WaitResp(5000, 50);

     if (status == RX_FINISHED) {
          // something was received but what was received?
          // ---------------------------------------------
          if(IsStringReceived("+CREG: 0,1")
                || IsStringReceived("+CREG: 0,5")
				|| IsStringReceived("+CREG: 1,1")
				|| IsStringReceived("+CREG: 1,5")) {
               // it means module is registered
               // ----------------------------
               module_status |= STATUS_REGISTERED;


               // in case GENERICGSM module is registered first time after reset
               // sets flag STATUS_INITIALIZED
               // it is used for sending some init commands which
               // must be sent only after registration
               // --------------------------------------------
               if (!isInitialized()) {
                    module_status |= STATUS_INITIALIZED;
                    SetCommLineStatus(CLS_FREE);
                //    InitParam(PARAM_SET_1);
               }
               ret_val = REG_REGISTERED;
          } else {
               // NOT registered
               // --------------
               module_status &= ~STATUS_REGISTERED;
               ret_val = REG_NOT_REGISTERED;
          }
     } else {
          // nothing was received
          // --------------------
          ret_val = REG_NO_RESPONSE;
     }
     SetCommLineStatus(CLS_FREE);


     return (ret_val);
}
/**********************************************************
Method sets speaker volume

speaker_volume: volume in range 0..14

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module did not answer in timeout
        -3 - GENERICGSM module has answered "ERROR" string

        OK ret val:
        -----------
        0..14 current speaker volume
**********************************************************/
/*
char GENERICGSM::SetSpeakerVolume(byte speaker_volume)
{

  char ret_val = -1;

  if (CLS_FREE != GetCommLineStatus()) return (ret_val);
  SetCommLineStatus(CLS_ATCMD);
  // remember set value as last value
  if (speaker_volume > 14) speaker_volume = 14;
  // select speaker volume (0 to 14)
  // AT+CLVL=X<CR>   X<0..14>
  _cell.print("AT+CLVL=");
  _cell.print((int)speaker_volume);
  _cell.print("\r"); // send <CR>
  // 10 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  if (RX_TMOUT_ERR == WaitResp(10000, 50)) {
    ret_val = -2; // ERROR
  }
  else {
    if(IsStringReceived(str_ok)) {
      last_speaker_volume = speaker_volume;
      ret_val = last_speaker_volume; // OK
    }
    else ret_val = -3; // ERROR
  }

  SetCommLineStatus(CLS_FREE);
  return (ret_val);
}
*/
/**********************************************************
Method increases speaker volume

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module did not answer in timeout
        -3 - GENERICGSM module has answered "ERROR" string

        OK ret val:
        -----------
        0..14 current speaker volume
**********************************************************/
/*
char GENERICGSM::IncSpeakerVolume(void)
{
  char ret_val;
  byte current_speaker_value;

  current_speaker_value = last_speaker_volume;
  if (current_speaker_value < 14) {
    current_speaker_value++;
    ret_val = SetSpeakerVolume(current_speaker_value);
  }
  else ret_val = 14;

  return (ret_val);
}
*/
/**********************************************************
Method decreases speaker volume

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module did not answer in timeout
        -3 - GENERICGSM module has answered "ERROR" string

        OK ret val:
        -----------
        0..14 current speaker volume
**********************************************************/
/*
char GENERICGSM::DecSpeakerVolume(void)
{
  char ret_val;
  byte current_speaker_value;

  current_speaker_value = last_speaker_volume;
  if (current_speaker_value > 0) {
    current_speaker_value--;
    ret_val = SetSpeakerVolume(current_speaker_value);
  }
  else ret_val = 0;

  return (ret_val);
}
*/

/**********************************************************
Method sends DTMF signal
This function only works when call is in progress

dtmf_tone: tone to send 0..15

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module didn't answer in timeout
        -3 - GENERICGSM module has answered "ERROR" string

        OK ret val:
        -----------
        0.. tone
**********************************************************/
/*
char GENERICGSM::SendDTMFSignal(byte dtmf_tone)
{
  char ret_val = -1;

  if (CLS_FREE != GetCommLineStatus()) return (ret_val);
  SetCommLineStatus(CLS_ATCMD);
  // e.g. AT+VTS=5<CR>
  _cell.print("AT+VTS=");
  _cell.print((int)dtmf_tone);
  _cell.print("\r");
  // 1 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  if (RX_TMOUT_ERR == WaitResp(1000, 50)) {
    ret_val = -2; // ERROR
  }
  else {
    if(IsStringReceived(str_ok)) {
      ret_val = dtmf_tone; // OK
    }
    else ret_val = -3; // ERROR
  }

  SetCommLineStatus(CLS_FREE);
  return (ret_val);
}
*/

/**********************************************************
Method returns state of user button


return: 0 - not pushed = released
        1 - pushed
**********************************************************/
#if 0
byte GENERICGSM::IsUserButtonPushed(void)
{
     byte ret_val = 0;
     if (CLS_FREE != GetCommLineStatus()) return(0);
     SetCommLineStatus(CLS_ATCMD);
     //if (AT_RESP_OK == SendATCmdWaitResp("AT#GPIO=9,2", 500, 50, "#GPIO: 0,0", 1)) {
     // user button is pushed
     //  ret_val = 1;
     //}
     //else ret_val = 0;
     //SetCommLineStatus(CLS_FREE);
     //return (ret_val);
}
#endif

/**********************************************************
Method reads phone number string from specified SIM position

position:     SMS position <1..20>

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module didn't answer in timeout
        -3 - position must be > 0
        phone_number is empty string

        OK ret val:
        -----------
        0 - there is no phone number on the position
        1 - phone number was found
        phone_number is filled by the phone number string finished by 0x00
                     so it is necessary to define string with at least
                     15 bytes(including also 0x00 termination character)

an example of usage:
        GENERICGSM atdevice;
        char phone_num[20]; // array for the phone number string

        if (1 == atdevice.GetPhoneNumber(1, phone_num)) {
          // valid phone number on SIM pos. #1
          // phone number string is copied to the phone_num array
          #ifdef DEBUG_PRINT
            atdevice.DebugPrint("DEBUG phone number: ", 0);
            atdevice.DebugPrint(phone_num, 1);
          #endif
        }
        else {
          // there is not valid phone number on the SIM pos.#1
          #ifdef DEBUG_PRINT
            atdevice.DebugPrint("DEBUG there is no phone number", 1);
          #endif
        }
**********************************************************/


AT_Phonebook_e GENERICGSM::getPhoneNumber(byte position, char *phone_number)
{
     AT_Phonebook_e ret_val = COMM_BUSY;

     char *p_char;
     char *p_char1;

     if (position == 0)
		 return (INVALID_INDEX);
     if (CLS_FREE != GetCommLineStatus())
		 return (ret_val);
     SetCommLineStatus(CLS_ATCMD);
     ret_val = NUMBER_NOT_PRESENT; // not found yet
     phone_number[0] = 0; // phone number not found yet => empty string

     //send "AT+CPBR=XY" - where XY = position
     _cell.print(S_READ_PHONEBOOK_COMMAND);
     _cell.print((int)position);
     _cell.print("\r");

     // 5000 msec. for initial comm tmout
     // 50 msec. for inter character timeout
     switch (WaitResp(5000, 50, S_READ_PHONEBOOK_REPLY))
	 {
		 case RX_TMOUT_ERR:
			  // response was not received in specific time
			  ret_val = MODULE_TIMEOUT;
			  break;

		 case RX_FINISHED_STR_RECV:
			  // response in case valid phone number stored:
			  // <CR><LF>+CPBR: <index>,<number>,<type>,<text><CR><LF>
			  // <CR><LF>OK<CR><LF>

			  // response in case there is not phone number:
			  // <CR><LF>OK<CR><LF>
			  p_char = strstr((char *)(comm_buf),",\"");
			  if (p_char != NULL) {
				   p_char++;
				   p_char++;       // we are on the first phone number character
				   // find out '"' as finish character of phone number string
				   p_char1 = strchr((char *)(p_char),'"');
				   if (p_char1 != NULL) {
						*p_char1 = 0; // end of string
				   }
				   // extract phone number string
				   strcpy(phone_number, (char *)(p_char));
				   // output value = we have found out phone number string
				   ret_val = NUMBER_FOUND;
			  }
			  break;

		 case RX_FINISHED_STR_NOT_RECV:
			  // only OK or ERROR => no phone number
			  ret_val = NUMBER_NOT_PRESENT;
			  break;
     }

     SetCommLineStatus(CLS_FREE);
     return (ret_val);
}

/**********************************************************
Method writes phone number string to the specified SIM position

position:     SMS position <1..20>
phone_number: phone number string for the writing

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module didn't answer in timeout
        -3 - position must be > 0

        OK ret val:
        -----------
        0 - phone number was not written
        1 - phone number was written
**********************************************************/
char GENERICGSM::putPhoneNumber(byte position, char *phone_number)
{
     char ret_val = -1;

     if (position == 0) return (-3);
     if (CLS_FREE != GetCommLineStatus())
		 return (ret_val);
     SetCommLineStatus(CLS_ATCMD);
     ret_val = 0; // phone number was not written yet

     //send: AT+CPBW=XY,"00420123456789"
     // where XY = position,
     //       "00420123456789" = phone number string
     _cell.print(S_WRITE_PHONEBOOK);
     _cell.print((int)position);
     _cell.print(S_COMMA_STRING);
     _cell.print(phone_number);
     _cell.print(S_CARRIAGE_RETURN_STRING);

     // 5000 msec. for initial comm tmout
     // 50 msec. for inter character timeout
     switch (WaitResp(5000, 50, str_ok))
	 {
		 case RX_TMOUT_ERR:
			  // response was not received in specific time
			  break;

		 case RX_FINISHED_STR_RECV:
			  // response is OK = has been written
			  ret_val = 1;
			  break;

		 case RX_FINISHED_STR_NOT_RECV:
			  // other response: e.g. ERROR
			  break;
     }

     SetCommLineStatus(CLS_FREE);
     return (ret_val);
}

/**********************************************************
Method del phone number from the specified SIM position

position:     SMS position <1..20>

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module didn't answer in timeout
        -3 - position must be > 0

        OK ret val:
        -----------
        0 - phone number was not deleted
        1 - phone number was deleted
**********************************************************/
char GENERICGSM::delPhoneNumber(byte position)
{
     char ret_val = -1;

     if (position == 0) return (-3);
     if (CLS_FREE != GetCommLineStatus()) return (ret_val);
     SetCommLineStatus(CLS_ATCMD);
     ret_val = 0; // phone number was not written yet

     //send: AT+CPBW=XY
     // where XY = position
     _cell.print(S_DEL_PHONEBOOK_COMMAND);
     _cell.print((int)position);
     _cell.print(F("\r"));

     // 5000 msec. for initial comm tmout
     // 50 msec. for inter character timeout
     switch (WaitResp(5000, 50, str_ok)) {
     case RX_TMOUT_ERR:
          // response was not received in specific time
          break;

     case RX_FINISHED_STR_RECV:
          // response is OK = has been written
          ret_val = 1;
          break;

     case RX_FINISHED_STR_NOT_RECV:
          // other response: e.g. ERROR
          break;
     }

     SetCommLineStatus(CLS_FREE);
     return (ret_val);
}

/**********************************************************
Function compares specified phone number string
with phone number stored at the specified SIM position

position:       SMS position <1..20>
phone_number:   phone number string which should be compare

return:
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GENERICGSM module is not free
        -2 - GENERICGSM module didn't answer in timeout
        -3 - position must be > 0

        OK ret val:
        -----------
        0 - phone numbers are different
        1 - phone numbers are the same


an example of usage:
        if (1 == atdevice.ComparePhoneNumber(1, "123456789")) {
          // the phone num. "123456789" is stored on the SIM pos. #1
          // phone number string is copied to the phone_num array
          #ifdef DEBUG_PRINT
            atdevice.DebugPrint("DEBUG phone numbers are the same", 1);
          #endif
        }
        else {
          #ifdef DEBUG_PRINT
            atdevice.DebugPrint("DEBUG phone numbers are different", 1);
          #endif
        }
**********************************************************/
char GENERICGSM::comparePhoneNumber(byte position, char *phone_number)
{
     char ret_val = -1;
     char sim_phone_number[20];


     ret_val = 0; // numbers are not the same so far
     if (position == 0) return (-3);
     if (1 == getPhoneNumber(position, sim_phone_number)) {
          //Serial.print("CHIAMANTE ");
          //Serial.println(phone_number);
          //Serial.print("SALVATO ");
          //Serial.println(sim_phone_number);

          // there is a valid number at the spec. SIM position
          // -------------------------------------------------
          if (0 == strcmp(phone_number, sim_phone_number)) {
               // phone numbers are the same
               // --------------------------
               ret_val = 1;
          }
     }
     return (ret_val);
}

//-----------------------------------------------------
