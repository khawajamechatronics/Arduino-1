/*
    global configuration
	Here's where you define the behaviour of your setup
*/

/*
	Define the serial channel to the GSM's UART
	Uncomment just 1 of the 5 choices
*/
//#define SWSERIAL 
//#define HWSERIAL Serial
//#define HWSERIAL Serial1
#define HWSERIAL Serial2
//#define HWSERIAL Serial3

#if defined(SWSERIAL) && defined(HWSERIAL)
#error You can only define 1 serial channel
#elif !defined(SWSERIAL) && !defined(HWSERIAL)
#error You must define 1 serial channel
#endif

//#define AT_DEVICE_ON_PIN 8

//#define AT_DEVICE_RESET_PIN 7
#define AT_DEVICE_ACTIVE_LOW
//#define AT_DEVICE_ACTIVE_HIGH
#define AT_DEVICE_ACTIVE_TIME 10 // millisec

/*
	Default baud rate of the GSM device. begin() is a lot quicker 
	if you define this correctly
*/
#define DEFAULT_BAUD_RATE 115200
 
/*
 Define variation of the base GSM module (GPRS A6)
 modify GenericStrings.h as necessary
 
 Uncomment the appropriate GSM_MODULE line to trigger redefinition of 
 the desired strings (See GenericStrings.h)
 
 If you create a new module, please donate the changes to my GitHub repository
 
*/
#define SIM900_MODULE 1
#define YET_ANOTHER_MODULE 2
//#define GSM_MODULE SIM900_MODULE
//#define GSM_MODULE YET_ANOTHER_MODULE

/*
    Features not available in all GMS devices
	e.g. the A6 has no flash storage so the AT commands AT&W and AT&V are not supported
	DTMF support varies widely
	Uncomment the appropriate line if your device supports that optional feature
*/
//#define DIAL_NUMBER_FROM_STORAGE // uncomment out if dial from storage (ATD><n>) supported