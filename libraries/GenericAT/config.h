/*
    global configuration
*/

//#define SWSERIAL  // uncomment if using hardware serial
#ifndef SWSERIAL
  // uncomment just 1 serial device
//#define HWSERIAL Serial
//#define HWSERIAL Serial1
#define HWSERIAL Serial2
//#define HWSERIAL Serial3
#endif
//#define AT_DEVICE_ON_PIN 8
//#define AT_DEVICE_RESET_PIN 7
#define AT_DEVICE_ACTIVE_LOW
//#define AT_DEVICE_ACTIVE_HIGH
#define AT_DEVICE_ACTIVE_TIME 10 // millisec
#define DEFAULT_BAUD_RATE 115200

// Define variation of GSM module
// Base module is the e-thinker GPRS A6
// modify GenericStrings.h as necessary
#define SIM900_MODULE 1
//#define GSM_MODULE SIM900_MODULE

//#define DIAL_NUMBER_FROM_STORAGE // uncomment out if dial from storage (ATD><n>) supported