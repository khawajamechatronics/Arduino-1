/*
    Generic AT commands, can be overriden for specific cases
*/
#define S_GET_IMEI F("AT+GSN")
#define S_WRITE_PHONEBOOK F("AT+CPBW=")
#define S_READ_PHONEBOOK_COMMAND F("AT+CPBR=")
#define S_READ_PHONEBOOK_REPLY "+CPBR"   // cannot use flash here
#define S_DEL_PHONEBOOK_COMMAND F("AT+CPBW=")
#define S_COMMA_STRING F(",\"")
#define S_CARRIAGE_RETURN_STRING F("\"\r")

#define S_PHONE_ACTIVITY_STATUS F("AT+CPAS")
#define S_NO_CALL "+CPAS:0"
#define S_INCOM_VOICE "+CPAS:3"
#define S_ACTIVE_VOICE "+CPAS:4"

#define S_SMS_TEXT_MODE F("AT+CMGF=1")
#define S_SMS_PDU_MODE F("AT+CMGF=0")
#define S_SMS_SEND F("AT+CMGS=\"")
#define S_SMS_SEND_RESPONSE "+CMGS"

#define S_PICKUP F("ATA")
#define S_HANGUP F("ATH")
#define S_DIAL F("ATD")
/*
    Put variations here, undef base version and define new version
*/
#ifdef GSM_MODULE
	#if GSM_MODULE == SIM900_MODULE
	#undef S_NO_CALL
	#define S_NO_CALL "+CPAS: 0"
	#undef S_INCOM_VOICE
	#define S_INCOM_VOICE "+CPAS: 3"
	#undef S_ACTIVE_VOICE
	#define S_ACTIVE_VOICE "+CPAS: 4"
	#endif
#endif
