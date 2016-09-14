/*
 * Components:
 * DS3234 provides datetime and interrupt every second
 * Hall Effect water flow meter
 * Relay 2 relays working in parallel to provide polarised pulse to solenoid
 * GSM SMS based commumication to outside owrld
 * RGB Led basic status display
 * Nokia5100 Information display
 * 
 * See configuration.h ofr GPIO pin allocation and various contants
 * TBD Move configuration stuff to EERPOM
 * 
 */

#include <stddef.h>
#include "Configuration.h"
#include <SPI.h>
#include "WaterMeter.h"
#include "Relay.h"
#include "RGB.h"
#include "wcEEPROM.h"
#include "Parser.h"

#include "SIM900.h" //NOTE: Only added here so that Remote.cpp include path set up correctly

// remote control, could be serial or GSM, dont care here
#include "Remote.h"
char Command[SMS_LENGTH];  // messages from remote
char smstime[30];   // timestamp of incoming SMS NOTE: time SENT not time RECEIVED
char *defaultPhone = DEFAULT_HOME_NUMBER;
bool alarmflag = false;
bool ledstate = false;
unsigned long metercount = 0, prevmetercount = 0, prevReportTime = 0;
bool remoteAvailable;
char tempbuf[300];  // for sprintf stuff, longest string is E?
bool SMSout = false;  // save on sending SMS just print to Serial
unsigned MAX_FLOW_TIME = 30; // default secs
unsigned TicksPerLiter = DEFAULT_TICKS_PER_LITER;
unsigned long REPORT_RATE = DEFAULT_REPORT_RATE;  // report every hour
unsigned PulseWidth = 100;
bool polaritySwitch = true;
char *IMEI = "1234567890123456";
int gprstimeout = DEFAULT_GPRS_TIMEOUT;

uint32_t prevsampletime = 0,prevanalyzetime = 0;
/*
 * Analysis methodology
 * Count meter ticks in 10 second blocks and compare this count to the previous block
 * If this block zero no water flowing, GREEN_LED
 * If previous block zero and this block non-zero water is now flowing so record time when water started flowing, BLUE led
 * If previous block non-zero and this block non-zero water still flowing, Compare current time to startflowtime, if greater RED led
 */
unsigned long StartCountingPeriod,CurrentMeterCount,PreviousMeterCount;
#ifdef ANTENNA_BUG
bool ignoreMeter = false;     // set true while sending emails
#endif

void MeterCount()
{
//  Serial.print("^");
  if (!ignoreMeter)
  {
    metercount++;
    CurrentMeterCount++;
  }
}


void Analyze()
{
#if 0
  Serial.print(lasttap == TAP_OPEN ? "tap open, " : "tap closed, ");
  Serial.print("Meter counts ");
  Serial.print(CurrentMeterCount);
  Serial.print(" ");
  Serial.println(PreviousMeterCount);
#endif
  if (CurrentMeterCount == 0) // not flowing
  {
    RGBActivate(RED_LED,LED_OFF);
    RGBActivate(BLUE_LED,LED_OFF);
    RGBActivate(GREEN_LED,LED_ON);
  }
  else  // is flowing
  {
    if (PreviousMeterCount == 0) // just started
    {
      // If water is flowing and tap supposed to be closed, change the state
 //     lasttap = TAP_OPEN;
      RGBActivate(RED_LED,LED_OFF);
      RGBActivate(GREEN_LED,LED_OFF);
      RGBActivate(BLUE_LED,LED_ON);
      StartCountingPeriod = millis()/ONE_SECOND;
    }
    else
    {
      if ((millis()/ONE_SECOND - StartCountingPeriod) >= MAX_FLOW_TIME)  // running for a minute or more)
      {
        RGBActivate(BLUE_LED,LED_OFF);
        RGBActivate(GREEN_LED,LED_OFF);
        RGBActivate(RED_LED,LED_ON);
        // sanity check, leak detected and tap open or closed
        // send message
        sprintf(tempbuf,"/WaterControl/RawData.php?action=leak&imei=%s&uptime=%lu&meterticks=%lu&liters=%lu&tap=%s&rawts=%s",
          IMEI,millis()/1000,metercount,metercount/TicksPerLiter,TapToText(),RemoteGetClock());
        TapChangeState(TAP_CLOSE);
        if (RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80))
          Serial.println("alarm sent OK");
        else
           Serial.println("alarm not sent");
        sprintf(tempbuf,"leak uptime=%lu ts=%s",millis()/1000,RemoteGetClock());
        if (SMSout)
          RemoteSendSMS(tempbuf);
      }
    }
  }
  PreviousMeterCount = CurrentMeterCount;
  CurrentMeterCount = 0;
}

void setup()
{
  Serial.begin(115200);
//  InitEepromData();
  PrintEepromData();
  MAX_FLOW_TIME = atoi(EEPROMGetIndex(LIS));
  TicksPerLiter = atoi(EEPROMGetIndex(TPL));
  REPORT_RATE = atol(EEPROMGetIndex(RR));
  gprstimeout = atoi(EEPROMGetIndex(GPRSTO));
  sprintf(tempbuf,"MFT %u TPL %u",MAX_FLOW_TIME,TicksPerLiter);
  Serial.println(tempbuf);
  RGBInit();
  pinMode(METER_DRDY,INPUT);
  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,LOW);
// force relay to power off position
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(HIGH,LOW);
  // response for water meter pulses
  attachInterrupt(digitalPinToInterrupt(METER_DRDY), MeterCount, RISING);
  TapChangeState(TAP_OPEN);
  // startup the GSM , can take time
  remoteAvailable = RemoteInit(GSM_REMOTE);
  Serial.println(remoteAvailable ? "GSM success" : "GSM failure");
//  Serial.println(gsm.getIMEI(IMEI));
//  Serial.println(IMEI);
  strcpy(IMEI,"123");
  sprintf(tempbuf,"/WaterControl/RawData.php?action=powerup&imei=%s&uptime=%lu&meterticks=%lu&rawts=%s",
    IMEI,millis()/1000,metercount,RemoteGetClock());
 // Serial.println(tempbuf);
  RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
}

void loop()
{
  String dt;
  uint32_t utime;
 #define BUFF_MAX 32
  char buff[BUFF_MAX];
  if (remoteAvailable && RemoteMessageAvailable(Command,smstime))
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
    sprintf(tempbuf,"T %lu M %d %d",prevsampletime,ignoreMeter,metercount);
 //   Serial.println(tempbuf);
    alarmflag = false;
    if (ledstate)
      digitalWrite(ONBOARD_LED,HIGH);
    else
      digitalWrite(ONBOARD_LED,LOW);
    ledstate = !ledstate;
// analyze every 10 secs
    if (millis() > (prevanalyzetime + TEN_SECONDS))
    {
  //    Serial.println(IMEI);
      prevanalyzetime = millis();
      Analyze();
    }
  }
  if (millis()/ONE_SECOND > (prevReportTime + REPORT_RATE))
  {
    // time for a regular report
    prevReportTime = millis()/ONE_SECOND;
    sprintf(tempbuf,"/WaterControl/RawData.php?action=report&imei=%s&uptime=%lu&meterticks=%lu&liters=%lu&tap=%s&rawts=%s",
        IMEI,millis()/1000,metercount,metercount/TicksPerLiter,TapToText(),RemoteGetClock());
    RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
  }
}

