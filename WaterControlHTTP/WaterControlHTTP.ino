/*
 * Components:
 * Hall Effect water flow meter (2 for redundancy)
 * Relay 2 relays working in parallel to provide polarised pulse to solenoid
 * GSM SMS/HTTP based commumication to outside world
 * RGB Led basic status display
 * 
 * See configuration.h ofr GPIO pin allocation and various contants
 * TBD Move configuration stuff to EERPOM
 * 
 */
/*
 * Version 2
 * Add watchdog - note not all boards physically support this optin so check
 * Add 2nd water meter
 * tweak HTTP GET for shorter URLs
 * rename TPL to TPL0 where neededed, add TPL1 in parallel - called tpl[2]
 * drop columns from sql table rawdata (liters,imsi etc) 
 * change E SMS command E? full info, E*  just TPL0,TPL1
 * Versionh 3
 * Place WWWPATH into flash
 * Version 3
 * Add T5403 temperature reading - do this because of board hanging - may be because of temperature
 * sdd homemade watchdog mechanism - only works by resetting from pin 7 - why ?
 * SMS message B -> reset
 * fix bug sending different tpl0,tpl1 called EEPROMGetIndex twice in same statement
 */
#include <stddef.h>
#include "Configuration.h"
#include <Wire.h>
#include "WaterMeter.h"
#include "Relay.h"
#include "RGB.h"
#include "wcEEPROM.h"
#include "Parser.h"
#define USE_BARO
#ifdef USE_BARO
#include <SparkFunT5403.h>
#endif

#include "SIM900.h" //NOTE: Only added here so that Remote.cpp include path set up correctly

// remote control, could be serial or GSM, dont care here
#include "Remote.h"
char Command[SMS_LENGTH];  // messages from remote
char smstime[30];   // timestamp of incoming SMS NOTE: time SENT not time RECEIVED
char *defaultPhone = DEFAULT_HOME_NUMBER;
bool alarmflag = false;
bool ledstate = false;
unsigned long metercount[2] = {0,0}, prevmetercount[2] = {0,0}, prevReportTime = 0;
bool remoteAvailable;
char tempbuf[300];  // for sprintf stuff, longest string is E?
bool SMSout = false;  // save on sending SMS just print to Serial
unsigned MAX_FLOW_TIME = 30; // default secs
//unsigned tpl[2] = {DEFAULT_TICKS_PER_LITER,DEFAULT_TICKS_PER_LITER);
unsigned long REPORT_RATE = DEFAULT_REPORT_RATE;  // report every hour
unsigned PulseWidth = 100;
bool polaritySwitch = true;
char *IMEI = "1234567890123456";
int gprstimeout = DEFAULT_GPRS_TIMEOUT;
uint32_t prevsampletime = 0,prevanalyzetime = 0;
#ifdef USE_BARO
//Create an instance of the object
T5403 barometer(MODE_I2C);
#endif
float temperature_c;
double pressure_abs, pressure_baseline;
/*
 * Analysis methodology
 * Count meter ticks in 10 second blocks and compare this count to the previous block
 * If this block zero no water flowing, GREEN_LED
 * If previous block zero and this block non-zero water is now flowing so record time when water started flowing, BLUE led
 * If previous block non-zero and this block non-zero water still flowing, Compare current time to startflowtime, if greater RED led
 */
unsigned long StartCountingPeriod,CurrentMeterCount[2],PreviousMeterCount[2];
#ifdef ANTENNA_BUG
bool ignoreMeter = false;     // set true while sending emails
#endif

void MeterCount1()
{
//  Serial.print("^");
  if (!ignoreMeter)
  {
    metercount[0]++;
    CurrentMeterCount[0]++;
  }
}

void MeterCount2()
{
//  Serial.print("^");
  if (!ignoreMeter)
  {
    metercount[1]++;
    CurrentMeterCount[1]++;
  }
}

/*
 *   Check both meters
 */
enum eWaterFlow FlowAnalyze(int meter)
{
  enum eWaterFlow wf = FLOW_UNKNOWN;
  if (CurrentMeterCount[meter] == 0) // not flowing
  {
    //RGBActivate(RED_LED,LED_OFF);
   // RGBActivate(BLUE_LED,LED_OFF);
    //RGBActivate(GREEN_LED,LED_ON);
    wf = NOT_FLOWING;
  }
  else  // is flowing
  {
    if (PreviousMeterCount[meter] == 0) // just started
    {
      // If water is flowing and tap supposed to be closed, change the state
      lasttap = TAP_OPEN;
  //    RGBActivate(RED_LED,LED_OFF);
  //    RGBActivate(GREEN_LED,LED_OFF);
   //   RGBActivate(BLUE_LED,LED_ON);
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
volatile unsigned long watchdogcounter = 0;
// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{
  // devise a watchdog like mechanism using TIMER0 - 
  watchdogcounter++;
  if (watchdogcounter > (TEN_SECONDS*6))
  {
    digitalWrite(RESET_TRIGGER,LOW);
    while (true){}
  }
}

void setup()
{
  Serial.begin(115200);
  //InitEepromData();
  PrintEepromData();
  MAX_FLOW_TIME = atoi(EEPROMGetIndex(LIS));
  REPORT_RATE = atol(EEPROMGetIndex(RR));
  gprstimeout = atoi(EEPROMGetIndex(GPRSTO));
  sprintf(tempbuf,"MFT %u TPL0 %u TPL1 %u",MAX_FLOW_TIME,atoi(EEPROMGetIndex(TPL0)),atoi(EEPROMGetIndex(TPL1)));
  Serial.println(tempbuf);
  RGBInit();
  pinMode(RESET_TRIGGER,OUTPUT);
  digitalWrite(RESET_TRIGGER,HIGH);
  pinMode(METER_DRDY_1,INPUT);
  pinMode(METER_DRDY_2,INPUT);
  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,LOW);
// force relay to power off position
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(HIGH,LOW);
  // response for water meter pulses
  attachInterrupt(digitalPinToInterrupt(METER_DRDY_1), MeterCount1, RISING);
  attachInterrupt(digitalPinToInterrupt(METER_DRDY_2), MeterCount2, RISING);
  TapChangeState(TAP_OPEN);
  // start up mpl31152
#ifdef USE_BARO
    barometer.begin();
    // Grab a baseline pressure for delta altitude calculation.
    pressure_baseline = barometer.getPressure(MODE_STANDARD);
#endif
  // startup the GSM , can take time
  remoteAvailable = RemoteInit(GSM_REMOTE);
  Serial.println(remoteAvailable ? "GSM success" : "GSM failure");
  gsm.getIMEI(IMEI);
//  Serial.println(IMEI);
//  strcpy(IMEI,"123");
//  Serial.println(gsm.GetClock());
  Parse("e*");   // force sending eeprom data as web server needs TPL to calculate liters
  delay(5000);   // delay needed between consecutive HTTP GET (why?)
  time_t ep = Sim900ToEpoch(gsm.GetClock());
  sprintf(tempbuf,"%s/RawData.php?AC=powerup&imei=%s&UT=%lu&m0=%lu&m1=%lu&epoch=%lu",EEPROMGetIndex(WWWPATH),
    IMEI,millis()/1000,metercount[0],metercount[0],ep);
 // Serial.println(tempbuf);
//  while (!RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80))
//  { }
  RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
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
    watchdogcounter = 0;
    sprintf(tempbuf,"T %lu M %d %d",prevsampletime,ignoreMeter,metercount);
//    Serial.println(tempbuf);
    alarmflag = false;
    if (ledstate)
      digitalWrite(ONBOARD_LED,HIGH);
    else
      digitalWrite(ONBOARD_LED,LOW);
    ledstate = !ledstate;
// analyze every 10 secs
    if (millis() > (prevanalyzetime + TEN_SECONDS))
    {
#ifdef USE_BARO
      temperature_c = barometer.getTemperature(CELSIUS);
      pressure_abs  = barometer.getPressure(MODE_STANDARD);
 //     Serial.print("Pressure ():");
//      Serial.print(pressure_abs, 2);
//      Serial.print(" Temp(c):");
//      Serial.println(temperature_c, 2);
#endif
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
        sprintf(tempbuf,"%s/RawData.php?AC=leak&imei=%s&UT=%lu&m0=%lu&m1=%lu&tap=%s&epoch=%lu",EEPROMGetIndex(WWWPATH),
          IMEI,millis()/1000,metercount[0],metercount[1],TapToText(),Sim900ToEpoch(gsm.GetClock()));
        TapChangeState(TAP_CLOSE);
        if (RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80))
          Serial.println("alarm sent OK");
        else
           Serial.println("alarm not sent");
        sprintf(tempbuf,"leak uptime=%lu ts=%s",millis()/1000,gsm.GetClock());
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
    sprintf(tempbuf,"%s/RawData.php?AC=report&imei=%s&UT=%lu&m0=%lu&m1=%lu&tap=%s&epoch=%lu&tp=%d",EEPROMGetIndex(WWWPATH),
        IMEI,millis()/1000,metercount[0],metercount[1],TapToText(),Sim900ToEpoch(gsm.GetClock()),(int)temperature_c);
    RemoteSendMessage(EEPROMGetIndex(HTTPs),tempbuf,80);
  }
}

