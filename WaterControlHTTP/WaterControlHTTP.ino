/*
 * Components:
 * Hall Effect water flow meter
 * Relay 2 relays working in parallel to provide polarised pulse to solenoid
 * GSM SMS based commumication to outside owrld
 * RGB Led basic status display
 * GSM shield provides calendar time/clock triggered by incoming SMS
 * 
 * See configuration.h ofr GPIO pin allocation and various contants
 * TBD Move configuration stuff to EERPOM
 *    
 *  Changes V2
 *  Change base URL address from WaterControl to WC
 *  Send timestyamp ar time_t epoch rather than string
 *  Shorten some field names RawData.php URL   action to AC uptime to UT
 *  Stop sending liters field in RawData URL
 *  Add ID string to EEPROM and send ID in place of IMIE
 *  Add 2nd water meter interrupt pin 
 *  change all watermeter stuff from single to double
 *    metercount , prevmetercount , CurrentMeterCount,PreviousMeterCount, TicksPerLiter
 *    add 2 values of tpl to eeprom
 *    at startup, send tpl0,tpl1 eeprom message
 *    on eeprom change, send report back to server
 *    New leak detect policy
 *    Move to UNO by adding SendOnlySoftwareSerial - software OK but failed as 
 *    UNO has only 2K ram which is not enough, MEGA has 8K

 * TBD
 * Add watchdog - note not all boards physically support this optin so check
 */
#include <stddef.h>
#include <Time.h>
//#include <TimeLib.h>
#include "Configuration.h"
#include <SPI.h>
#include "WaterMeter.h"
#include "Relay.h"
#include "RGB.h"
#include "wcEEPROM.h"
#include "Parser.h"

#ifndef DEBUG_SERIAL
#include <SendOnlySoftwareSerial.h>
SendOnlySoftwareSerial DEBUG_SERIAL(3);
#endif

#include "SIM900.h" //NOTE: Only added here so that Remote.cpp include path set up correctly

// remote control, could be serial or GSM, dont care here
#include "Remote.h"
char Command[SMS_LENGTH];  // messages from remote
char smstime[30];   // timestamp of incoming SMS NOTE: time SENT not time RECEIVED
char *defaultPhone = DEFAULT_HOME_NUMBER;
bool alarmflag = false;
bool ledstate = false;
unsigned long StartCountingPeriod = 0, prevReportTime = 0;
bool remoteAvailable;
char tempbuf[300];  // for sprintf stuff, longest string is E?
bool SMSout = false;  // save on sending SMS just print to Serial
unsigned MAX_FLOW_TIME = 30; // default secs
unsigned long REPORT_RATE = DEFAULT_REPORT_RATE;  // report every hour
unsigned PulseWidth = 100;
bool polaritySwitch = true;

char IMEI[] = "1234567890123456";  // allow 16 chars
int gprstimeout = DEFAULT_GPRS_TIMEOUT;

uint32_t prevsampletime = 0,prevanalyzetime = 0;


/*
 * Analysis methodology
 * Count meter ticks in 10 second blocks and compare this count to the previous block
 * If this block zero no water flowing, GREEN_LED
 * If previous block zero and this block non-zero water is now flowing so record time when water started flowing, BLUE led
 * If previous block non-zero and this block non-zero water still flowing, Compare current time to startflowtime, if greater RED led
 */
unsigned long CurrentMeterCount[2],PreviousMeterCount[2],TotalMeterCount[2] = {0,0};
unsigned TicksPerLiter[2] = {DEFAULT_TICKS_PER_LITER,DEFAULT_TICKS_PER_LITER};
#ifdef ANTENNA_BUG
bool ignoreMeter = false;     // set true while sending emails
#endif

void MeterCounter(int n)
{
  if (!ignoreMeter)
  {
    TotalMeterCount[n]++;
    CurrentMeterCount[n]++;
  }
}
void Meter0Count ()
{
  MeterCounter(0);
}
void Meter1Count ()
{
  MeterCounter(1);
}

/*
 *   Check both meters
 */
enum eWaterFlow FlowAnalyze(int meter)
{
  enum eWaterFlow wf = FLOW_UNKNOWN;
  if (CurrentMeterCount[meter] == 0) // not flowing
  {
    wf = NOT_FLOWING;
  }
  else  // is flowing
  {
    if (PreviousMeterCount[meter] == 0) // just started
    {
      // If water is flowing and tap supposed to be closed, change the state
      lasttap = TAP_OPEN;
      wf = FLOWING;
      StartCountingPeriod = millis()/ONE_SECOND;
    }
    else
    {
      if ((millis()/ONE_SECOND - StartCountingPeriod) >= MAX_FLOW_TIME)  // running for a minute or more)
      {
        wf = WATER_LEAK;
      }
    }
  }
  PreviousMeterCount[meter] = CurrentMeterCount[meter];
  CurrentMeterCount[meter] = 0;
  return wf;
}

void setup()
{
  DEBUG_SERIAL.begin(DEBUG_BAUD_RATE);
  //InitEepromData();
  PrintEepromData();
  MAX_FLOW_TIME = atoi(EEPROMGetIndex(LIS));
  TicksPerLiter[0] = atoi(EEPROMGetIndex(TPL0));
  TicksPerLiter[1] = atoi(EEPROMGetIndex(TPL1));
  REPORT_RATE = atol(EEPROMGetIndex(RR));
  gprstimeout = atoi(EEPROMGetIndex(GPRSTO));
  sprintf(tempbuf,"MFT %u TPL0 %u TPL1 %u",MAX_FLOW_TIME,TicksPerLiter[0],TicksPerLiter[1]);
  DEBUG_SERIAL.println(tempbuf);
  RGBInit();
  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,LOW);
// force relay to power off position
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(HIGH,LOW);
  // response for water meter pulses
  pinMode(METER_0_DRDY,INPUT);
  attachInterrupt(digitalPinToInterrupt(METER_0_DRDY), Meter0Count, RISING);
  pinMode(METER_1_DRDY,INPUT);
  attachInterrupt(digitalPinToInterrupt(METER_1_DRDY), Meter1Count, RISING);
  TapChangeState(TAP_OPEN);
  // startup the GSM , can take time
  remoteAvailable = RemoteInit(GSM_REMOTE);
#if 0
  if (gsm.getIMEI(IMEI)== 0)
  {
    DEBUG_SERIAL.println("Didnt get IMEI");
  }
  else
    DEBUG_SERIAL.println(IMEI);
#endif
  if (remoteAvailable)
  {
    DEBUG_SERIAL.println("GSM success");
    strcpy(IMEI,"123");
    sprintf(tempbuf,"/WC/RawData.php?AC=powerup&imei=%s&UT=%lu&m0=%lu&m1=%lu&epoch=%lu",
      IMEI,millis()/1000,TotalMeterCount[0],TotalMeterCount[1],Sim900ToEpoch());
  // DEBUG_SERIAL.println(tempbuf);
    RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
    delay(5000); 
  // report ticks per liter -- need delay between 2 http get, not sure why
    int items2[] = {TPL0,TPL1};  //
    EEPROMReport(items2,2);  //
  }
  else
  {
    DEBUG_SERIAL.println("GSM failure");
    // turn all leds on and hang in a loop
    RGBActivate(RED_LED,LED_ON);
    RGBActivate(BLUE_LED,LED_ON);
    RGBActivate(GREEN_LED,LED_ON);
  }
}

void loop()
{
  String dt;
  uint32_t utime;
 #define BUFF_MAX 32
  char buff[BUFF_MAX];
  int retries;
  if (remoteAvailable)
  {
    if (RemoteMessageAvailable(Command,smstime))
    {
      // use smstime to recalibrate RTC
  // also update GSM modem so that emails have correct time
      RemoteSetClock(smstime);
      // parse command and act on it
      Parse(Command);
    }
    if ((millis() - prevsampletime) >= ONE_SECOND)
    {
      alarmflag = true;
      prevsampletime = millis();
    }
    if (alarmflag)
    {
      sprintf(tempbuf,"T %lu M %d %lu %lu",prevsampletime,ignoreMeter,TotalMeterCount[0],TotalMeterCount[1]);
   //   DEBUG_SERIAL.println(tempbuf);
      alarmflag = false;
      if (ledstate)
        digitalWrite(ONBOARD_LED,HIGH);
      else
        digitalWrite(ONBOARD_LED,LOW);
      ledstate = !ledstate;
  // analyze every 10 secs
      if (millis() > (prevanalyzetime + TEN_SECONDS))
      {
        enum eWaterFlow wf[2];
        prevanalyzetime = millis();
        // check both meters and decide how to proceed
        wf[0] = FlowAnalyze(0);
        wf[1] = FlowAnalyze(1);
        if (wf[0] == NOT_FLOWING && wf[1] == NOT_FLOWING)  // both report not flowing
        {
          RGBActivate(RED_LED,LED_OFF);
          RGBActivate(BLUE_LED,LED_OFF);
          RGBActivate(GREEN_LED,LED_ON);
        }
        else if (wf[0] == FLOWING && wf[1] == FLOWING)  // both report flowing
        {
          RGBActivate(RED_LED,LED_OFF);
          RGBActivate(BLUE_LED,LED_ON);
          RGBActivate(GREEN_LED,LED_OFF);
        }
        else if (wf[0] == WATER_LEAK || wf[1] == WATER_LEAK)  // either report flowing
        {
          RGBActivate(RED_LED,LED_ON);
          RGBActivate(BLUE_LED,LED_OFF);
          RGBActivate(GREEN_LED,LED_OFF);
          // send message
          sprintf(tempbuf,"/WC/RawData.php?action=leak&imei=%s&uptime=%lu&m1=%lu&m2=%lu&tap=%s&epoch=%lu",
            IMEI,millis()/1000,TotalMeterCount[0],TotalMeterCount[1],TapToText(),Sim900ToEpoch());
          TapChangeState(TAP_CLOSE);
          retries = 5;
          while (retries > 0)
          {
            if (RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80))
              retries = 0;
            else
              retries--;
          }
          sprintf(tempbuf,"leak uptime=%lu ts=%s",millis()/1000,RemoteGetClock());
          if (SMSout)
            RemoteSendSMS(tempbuf);
        }
        else
        {
          // what to do if meters dont agree ?
        }
      }
    }
    if (millis()/ONE_SECOND > (prevReportTime + REPORT_RATE))
    {
      // time for a regular report
      prevReportTime = millis()/ONE_SECOND;
      sprintf(tempbuf,"/WC/RawData.php?AC=report&imei=%s&UT=%lu&m0=%lu&m1=%lu&tap=%s&epoch=%lu",
          IMEI,millis()/1000,TotalMeterCount[0],TotalMeterCount[1],TapToText(),Sim900ToEpoch());
      RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
    }
  }
}

