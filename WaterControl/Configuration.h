/*
 * Hardware Pins
*/
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define HARDWARE_SPI // forces use of HW CLK,MOSI,MISO
//#define USE_DS3234    // comment out if not using DS3234
//#define USE_5100  // comment out if not using Nokia 5100

#define ANTENNA_BUG
#ifdef USE_DS3234
const int DS3234_CS = 10; //RTC chip select 
const int DS3234_DRDY = 3; // alarm
#endif
#ifdef USE_5100
const int LCD_RST = 24; // Nokia 5100 reset
const int LCD_CS = 23; // Nokia 5100 chip select
const int LCD_DC = 25; // Nokia 5100 DC
#endif
const int METER_DRDY = 21; // alarm must be an interrupt pin
const int ONBOARD_LED = 13;
const int RED_LED_PIN = 9; //41;
const int GREEN_LED_PIN = 10; //42;
const int BLUE_LED_PIN = 11; //43;
const int IN1 = 6; // relay pins
const int IN2 = 7;
// pin 8 used by gsmshield
//const int BUTTON = 20; // must be an interrupt pin
// water meter observer calibration

// MEGA MISO 50 MOSI 51  SCK 52
// UNO MISO 12 MOSI 11  SCK 13

// Mega 2560 interrupt pins are 2,3,18,19,20,21
/*
 *   ICSP   MISO (1)  VCC
 *          SCK       MOSI
 *          RESET     GND
 */

#define DEFAULT_HOME_NUMBER "+972545919886"
#define DEFAULT_SMTP_SERVER "smtp.mail.yahoo.com"
#define DEFAULT_SENDER_ACCOUNT  "dhdh654321@yahoo.com"
#define DEFAULT_SENDER_PASSWORD "Morris59"
#define DEFAULT_RECEIVER_ACCOUNT "mgadriver@gmail.com"
#define DEFAULT_APN "uinternet"
#define DEFAULT_PULSE_WIDTH 100
#define DEFAULT_REPORT_RATE (24*3600L)
#define DEFAULT_LEAK_IN_SECS 29
#define DEFAULT_TICKS_PER_LITER 250
// Useful universal macros
#define SMS_LENGTH 160
#define TICKS_PER_SEC 1000
#define ONE_SECOND TICKS_PER_SEC
#define TEN_SECONDS (10*TICKS_PER_SEC)

#endif
