#include "GenericCall.h"

/**********************************************************
Method checks status of call

return: 
      CALL_NONE         - no call activity
      CALL_INCOM_VOICE  - incoming voice
      CALL_ACTIVE_VOICE - active voice
      CALL_NO_RESPONSE  - no response to the AT command 
      CALL_COMM_LINE_BUSY - comm line is not free
**********************************************************/
call_ret_val_enum callGSM::CallStatus(void)
{
  call_ret_val_enum ret_val = CALL_NONE;

  if (CLS_FREE != atdevice.GetCommLineStatus()) return (CALL_COMM_LINE_BUSY);
  atdevice.SetCommLineStatus(CLS_ATCMD);
  atdevice.SimpleWriteln(S_PHONE_ACTIVITY_STATUS);

  // 5 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  if (RX_TMOUT_ERR == atdevice.WaitResp(5000, 50)) {
    // nothing was received (RX_TMOUT_ERR)
    // -----------------------------------
    ret_val = CALL_NO_RESPONSE;
  }
  else {
    // something was received but what was received?
    // ---------------------------------------------
    // ready (device allows commands from TA/TE)
    // <CR><LF>+CPAS: 0<CR><LF> <CR><LF>OK<CR><LF>
    // unavailable (device does not allow commands from TA/TE)
    // <CR><LF>+CPAS: 1<CR><LF> <CR><LF>OK<CR><LF> 
    // unknown (device is not guaranteed to respond to instructions)
    // <CR><LF>+CPAS: 2<CR><LF> <CR><LF>OK<CR><LF> - NO CALL
    // ringing
    // <CR><LF>+CPAS: 3<CR><LF> <CR><LF>OK<CR><LF> - NO CALL
    // call in progress
    // <CR><LF>+CPAS: 4<CR><LF> <CR><LF>OK<CR><LF> - NO CALL
    if(atdevice.IsStringReceived(S_NO_CALL)) { 
      // ready - there is no call
      // ------------------------
      ret_val = CALL_NONE;
    }
    else if(atdevice.IsStringReceived(S_INCOM_VOICE)) { 
      // incoming call
      // --------------
      ret_val = CALL_INCOM_VOICE;
    }
    else if(atdevice.IsStringReceived(S_ACTIVE_VOICE)) { 
      // active call
      // -----------
      ret_val = CALL_ACTIVE_VOICE;
    }
  }

  atdevice.SetCommLineStatus(CLS_FREE);
  return (ret_val);

}

/**********************************************************
Method checks status of call(incoming or active) 
and makes authorization with specified SIM positions range

phone_number: a pointer where the tel. number string of current call will be placed
              so the space for the phone number string must be reserved - see example
first_authorized_pos: initial SIM phonebook position where the authorization process
                      starts
last_authorized_pos:  last SIM phonebook position where the authorization process
                      finishes

                      Note(important):
                      ================
                      In case first_authorized_pos=0 and also last_authorized_pos=0
                      the received incoming phone number is NOT authorized at all, so every
                      incoming is considered as authorized (CALL_INCOM_VOICE_NOT_AUTH is returned)

return: 
      CALL_NONE                   - no call activity
      CALL_INCOM_VOICE_AUTH       - incoming voice - authorized
      CALL_INCOM_VOICE_NOT_AUTH   - incoming voice - not authorized
      CALL_ACTIVE_VOICE           - active voice
      CALL_INCOM_DATA_AUTH        - incoming data call - authorized
      CALL_INCOM_DATA_NOT_AUTH    - incoming data call - not authorized  
      CALL_ACTIVE_DATA            - active data call
      CALL_NO_RESPONSE            - no response to the AT command 
      CALL_COMM_LINE_BUSY         - comm line is not free
**********************************************************/
call_ret_val_enum callGSM::CallStatusWithAuth(char *phone_number,
                             byte first_authorized_pos, byte last_authorized_pos)
{
  call_ret_val_enum ret_val = CALL_NONE;
  byte search_phone_num = 0;
  byte i;
  byte status;
  char *p_char; 
  char *p_char1;

  phone_number[0] = 0x00;  // no phonr number so far
  if (CLS_FREE != atdevice.GetCommLineStatus()) return (CALL_COMM_LINE_BUSY);
  atdevice.SetCommLineStatus(CLS_ATCMD);
  atdevice.SimpleWriteln(F("AT+CLCC"));

  // 5 sec. for initial comm tmout
  // and max. 1500 msec. for inter character timeout
  atdevice.RxInit(5000, 1500); 
  // wait response is finished
  do {
    if (atdevice.IsStringReceived("OK\r\n")) { 
      // perfect - we have some response, but what:

      // there is either NO call:
      // <CR><LF>OK<CR><LF>

      // or there is at least 1 call
      // +CLCC: 1,1,4,0,0,"+420XXXXXXXXX",145<CR><LF>
      // <CR><LF>OK<CR><LF>
      status = RX_FINISHED;
      break; // so finish receiving immediately and let's go to 
             // to check response 
    }
    status = atdevice.IsRxFinished();
  } while (status == RX_NOT_FINISHED);

  // generate tmout 30msec. before next AT command
  delay(30);

  if (status == RX_FINISHED) {
    // something was received but what was received?
    // example: //+CLCC: 1,1,4,0,0,"+420XXXXXXXXX",145
    // ---------------------------------------------
    if(atdevice.IsStringReceived("+CLCC: 1,1,4,0,0")) { 
      // incoming VOICE call - not authorized so far
      // -------------------------------------------
      search_phone_num = 1;
      ret_val = CALL_INCOM_VOICE_NOT_AUTH;
    }
    else if(atdevice.IsStringReceived("+CLCC: 1,1,4,1,0")) { 
      // incoming DATA call - not authorized so far
      // ------------------------------------------
      search_phone_num = 1;
      ret_val = CALL_INCOM_DATA_NOT_AUTH;
    }
    else if(atdevice.IsStringReceived("+CLCC: 1,0,0,0,0")) { 
      // active VOICE call - GSM is caller
      // ----------------------------------
      search_phone_num = 1;
      ret_val = CALL_ACTIVE_VOICE;
    }
    else if(atdevice.IsStringReceived("+CLCC: 1,1,0,0,0")) { 
      // active VOICE call - GSM is listener
      // -----------------------------------
      search_phone_num = 1;
      ret_val = CALL_ACTIVE_VOICE;
    }
    else if(atdevice.IsStringReceived("+CLCC: 1,1,0,1,0")) { 
      // active DATA call - GSM is listener
      // ----------------------------------
      search_phone_num = 1;
      ret_val = CALL_ACTIVE_DATA;
    }
    else if(atdevice.IsStringReceived("+CLCC:")){ 
      // other string is not important for us - e.g. GSM module activate call
      // etc.
      // IMPORTANT - each +CLCC:xx response has also at the end
      // string <CR><LF>OK<CR><LF>
      ret_val = CALL_OTHERS;
    }
    else if(atdevice.IsStringReceived("OK")){ 
      // only "OK" => there is NO call activity
      // --------------------------------------
      ret_val = CALL_NONE;
    }

    
    // now we will search phone num string
    if (search_phone_num) {
      // extract phone number string
      // ---------------------------
      p_char = strchr((char *)(atdevice.comm_buf),'"');
      p_char1 = p_char+1; // we are on the first phone number character
      p_char = strchr((char *)(p_char1),'"');
      if (p_char != NULL) {
        *p_char = 0; // end of string
        strcpy(phone_number, (char *)(p_char1));
		DebugPrint("ATTESO: ");
		DebugPrintln(phone_number);
      }
	  else
		//Serial.println(atdevice.comm_buf);
		DebugPrintln("NULL");
      
      if ( (ret_val == CALL_INCOM_VOICE_NOT_AUTH) 
           || (ret_val == CALL_INCOM_DATA_NOT_AUTH)) {

        if ((first_authorized_pos == 0) && (last_authorized_pos == 0)) {
          // authorization is not required => it means authorization is OK
          // -------------------------------------------------------------
          if (ret_val == CALL_INCOM_VOICE_NOT_AUTH) ret_val = CALL_INCOM_VOICE_AUTH;
          else ret_val = CALL_INCOM_DATA_AUTH;
        }
        else {
          // make authorization
          // ------------------
          atdevice.SetCommLineStatus(CLS_FREE);
          for (i = first_authorized_pos; i <= last_authorized_pos; i++) {
            if (atdevice.ComparePhoneNumber(i, phone_number)) {
              // phone numbers are identical
              // authorization is OK
              // ---------------------------
              if (ret_val == CALL_INCOM_VOICE_NOT_AUTH) ret_val = CALL_INCOM_VOICE_AUTH;
              else ret_val = CALL_INCOM_DATA_AUTH;
              break;  // and finish authorization
            }
          }
        }
      }
    }
    
  }
  else {
    // nothing was received (RX_TMOUT_ERR)
    // -----------------------------------
    ret_val = CALL_NO_RESPONSE;
  }

  atdevice.SetCommLineStatus(CLS_FREE);
  return (ret_val);
}

/**********************************************************
Method picks up an incoming call

return: 
**********************************************************/
void callGSM::PickUp(void)
{
  //if (CLS_FREE != atdevice.GetCommLineStatus()) return;
  //atdevice.SetCommLineStatus(CLS_ATCMD);
  atdevice.SendATCmdWaitResp(S_PICKUP, 10, 10, "OK", 3);
  atdevice.SimpleWriteln(S_PICKUP);
  //atdevice.SetCommLineStatus(CLS_FREE);
}

/**********************************************************
Method hangs up incoming or active call

return: 
**********************************************************/
void callGSM::HangUp(void)
{
  //if (CLS_FREE != atdevice.GetCommLineStatus()) return;
  //atdevice.SetCommLineStatus(CLS_ATCMD);
  atdevice.SendATCmdWaitResp(S_HANGUP, 500, 100, "OK", 5);
  //atdevice.SetCommLineStatus(CLS_FREE);
}

/**********************************************************
Method calls the specific number

number_string: pointer to the phone number string 
               e.g. atdevice.Call("+420123456789"); 

**********************************************************/
void callGSM::Call(char *number_string)
{
  if (CLS_FREE != atdevice.GetCommLineStatus()) return;
  atdevice.SetCommLineStatus(CLS_ATCMD);
  // ATDxxxxxx;<CR>
  atdevice.SimpleWrite(S_DIAL);
  atdevice.SimpleWrite(number_string);    
 // atdevice.SimpleWriteln(F(";"));
  // 10 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  atdevice.WaitResp(10000, 50);
  atdevice.SetCommLineStatus(CLS_FREE);
}

/**********************************************************
Method calls the number stored at the specified SIM position

sim_position: position in the SIM <1...>
              e.g. atdevice.Call(1);
**********************************************************/
#ifdef DIAL_NUMBER_FROM_STORAGE
void callGSM::Call(int sim_position)
{
  if (CLS_FREE != atdevice.GetCommLineStatus())
	  return;
  atdevice.SetCommLineStatus(CLS_ATCMD);
  // ATD>"SM" 1;<CR>
  atdevice.SimpleWrite(F("ATD>\"SM\" "));
  atdevice.SimpleWrite(sim_position);
  atdevice.SimpleWriteln(F(";"));

  // 10 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  atdevice.WaitResp(10000, 50);

  atdevice.SetCommLineStatus(CLS_FREE);
}
#endif

void callGSM::SendDTMF(char *number_string, int time)
{
  if (CLS_FREE != atdevice.GetCommLineStatus()) return;
  atdevice.SetCommLineStatus(CLS_ATCMD);

  atdevice.SimpleWrite(F("AT+VTD="));
  atdevice.SimpleWriteln(time);
  atdevice.WaitResp(1000, 100, "OK");

  atdevice.SimpleWrite(F("AT+VTS=\""));
  atdevice.SimpleWrite(number_string);
  atdevice.SimpleWriteln(F("\""));
  
  atdevice.WaitResp(5000, 100, "OK");
  atdevice.SetCommLineStatus(CLS_FREE);
}

void callGSM::SetDTMF(int DTMF_status)
{
	if(DTMF_status==1)
		atdevice.SendATCmdWaitResp("AT+DDET=1", 500, 50, "OK", 5);
	else
		atdevice.SendATCmdWaitResp("AT+DDET=0", 500, 50, "OK", 5);
}


char callGSM::DetDTMF()
{
	char *p_char; 
	char *p_char1;
	char dtmf_char='-';
	atdevice.WaitResp(1000, 500);
	{
		//Serial.print("BUF: ");
		//Serial.println((char *)atdevice.comm_buf);
		//Serial.println("end");
		p_char = strstr((char *)(atdevice.comm_buf),"+DTMF:");
		if (p_char != NULL) {
			p_char1 = p_char+6;  //we are on the first char of BCS
			dtmf_char = *p_char1;
		}
	}
	return dtmf_char;
}