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
#ifdef USE_DS3234
#include "DS3234.h"
#endif
#include "WaterMeter.h"
#include "Relay.h"
#include "RGB.h"
#include "wcEEPROM.h"
#include "Parser.h"
#ifdef USE_5100
// Nokia5100 display
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#endif

#include "SIM900.h" //NOTE: Only added here so that Remote.cpp include path set up correctly

// remote control, could be serial or GSM, dont care here
#include "Remote.h"
char Command[SMS_LENGTH];  // messages from remote
char smstime[30];   // timestamp of incoming SMS NOTE: time SENT not time RECEIVED
char *defaultPhone = DEFAULT_HOME_NUMBER;
#ifdef USE_5100
Adafruit_PCD8544 display = Adafruit_PCD8544(LCD_DC, LCD_CS, LCD_RST);
#endif
bool alarmflag = false;
bool ledstate = false;
unsigned long metercount = 0, prevmetercount = 0, prevReportTime = 0;
bool remoteAvailable;
char tempbuf[200];  // for sprintf stuff
bool SMSout = false;  // save on sending SMS just print to Serial
unsigned MAX_FLOW_TIME = 30; // default secs
unsigned TicksPerLiter = DEFAULT_TICKS_PER_LITER;
unsigned long REPORT_RATE = 3600;  // report every hour
unsigned PulseWidth = 100;
bool polaritySwitch = true;

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

#ifdef USE_DS3234
// called every alarm second
void AlarmFunc()
{
  alarmflag = true;
}
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

#ifdef USE_DS3234
struct ts TS;  // used by DS3234, convert datetime to unix time for easier comparison
#endif

#ifdef BUTTON   // just used for development
void TapOnOff()
{
  if (digitalRead(BUTTON) == HIGH)
    //TapOpen()
    TapChangeState(TAP_OPEN);
  else
    //TapClose();
    TapChangeState(TAP_CLOSE);
}
#endif

void Analyze()
{
  Serial.print(lasttap == TAP_OPEN ? "tap open, " : "tap closed, ");
  Serial.print("Meter counts ");
  Serial.print(CurrentMeterCount);
  Serial.print(" ");
  Serial.println(PreviousMeterCount);
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
#ifdef USE_DS3234
      StartCountingPeriod = TS.unixtime;
#else
      StartCountingPeriod = millis()/ONE_SECOND;
#endif
    }
    else
    {
#ifdef USE_DS3234
      if ((TS.unixtime - StartCountingPeriod) >= MAX_FLOW_TIME)  // running for a minute or more
#else
      if ((millis()/ONE_SECOND - StartCountingPeriod) >= MAX_FLOW_TIME)  // running for a minute or more)
#endif
      {
        RGBActivate(BLUE_LED,LED_OFF);
        RGBActivate(GREEN_LED,LED_OFF);
        RGBActivate(RED_LED,LED_ON);
        // sanity check, leak detected and tap open or closed
        // send message
#ifdef USE_DS3234
        sprintf(tempbuf,"Water Leak %02d/%02d/%02d %02d:%02d:%02d", TS.mday,
           TS.mon, TS.year, TS.hour, TS.min, TS.sec);
#else
        sprintf(tempbuf,"Tap State: %s.Water Leak %s",lasttap == TAP_CLOSE ? "Closed" : "Open", RemoteGetClock());
#endif
        //TapClose();
        TapChangeState(TAP_CLOSE);
        if (RemoteSendMessage(tempbuf))
          Serial.println("alarm sent OK");
        else
           Serial.println("alarm not sent");
      }
    }
  }
  PreviousMeterCount = CurrentMeterCount;
  CurrentMeterCount = 0;
}

void setup() {
  Serial.begin(115200);
  MAX_FLOW_TIME = atoi(EEPROMGetIndex(LIS));
  TicksPerLiter = atoi(EEPROMGetIndex(TPL));
  sprintf(tempbuf,"MFT %u TPL %u",MAX_FLOW_TIME,TicksPerLiter);
  Serial.println(tempbuf);
  RGBInit();
  pinMode(METER_DRDY,INPUT);
  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,LOW);
  // deselect all SPI devices
#ifdef USE_DS3234
  pinMode(DS3234_DRDY,INPUT);
  pinMode(DS3234_CS,OUTPUT);
  digitalWrite(DS3234_CS,HIGH);
#endif
#ifdef USE_5100
  pinMode(LCD_CS,OUTPUT);
  digitalWrite(LCD_CS,HIGH);
#endif
  SPI.begin();
  SPI.setBitOrder(MSBFIRST); 
// force relay to power off position
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(HIGH,LOW);
  // response for water meter pulses
  attachInterrupt(digitalPinToInterrupt(METER_DRDY), MeterCount, RISING);
  // RTC is MODE3, Nokia MODE0 
#ifdef USE_DS3234
  SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work but only works properly with interrupt
  RS3234Init();
  //day(1-31), month(1-12), year , hour(0-23), minute(0-59), second(0-59)
  //SetTimeDate(13,4,2016,19,54,0); 
  SetEverySecondAlarm();
  // set up RTC alarm every seconf
  //pinMode(DS3234_DRDY,INPUT);
  attachInterrupt(digitalPinToInterrupt(DS3234_DRDY),AlarmFunc, FALLING); // SQK is active LOW
#endif
#ifdef USE_5100
  // set up Nokia display
  display.begin();
  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);
  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(BLACK);
#endif
// well use a button to turn on/off
#ifdef BUTTON
  pinMode(BUTTON,INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON),TapOnOff, CHANGE);
#endif
  //TapOpen();
  TapChangeState(TAP_OPEN);
  // startup the GSM , can take time
  remoteAvailable = RemoteInit(GSM_REMOTE);
  Serial.println(remoteAvailable ? "GSM success" : "GSM failure");
  PrintEepromData();
  RemoteSendMessage("Water Control Started");
}


void loop() {
  String dt;
  uint32_t utime;
 #define BUFF_MAX 32
  char buff[BUFF_MAX];
  if (remoteAvailable && RemoteMessageAvailable(Command,smstime))
  {
    // use smstime to recalibrate RTC
#ifdef USE_DS3234
    DS3234_SMS_get(smstime,&TS);
    SetTimeDate(TS.mday,TS.mon,TS.year,TS.hour,TS.min,TS.sec);
#endif
// also updte GSM modem so that emails have correct time
    RemoteSetClock(smstime);
    // parse command and act on it
    Parse(Command);
  }
#ifndef USE_DS3234
  if ((millis() - prevsampletime) >= ONE_SECOND)
  {
    alarmflag = true;
    prevsampletime = millis();
  }
#endif
  if (alarmflag)
  {
    sprintf(tempbuf,"T %lu M %d %d",prevsampletime,ignoreMeter,metercount);
    Serial.println(tempbuf);
    alarmflag = false;
    if (ledstate)
      digitalWrite(ONBOARD_LED,HIGH);
    else
      digitalWrite(ONBOARD_LED,LOW);
    ledstate = !ledstate;
#ifdef USE_DS3234
    SPI.setDataMode(SPI_MODE3);
    DS3234_get(&TS);
    ClearAlarm();
#endif
#ifdef USE_5100
    // display on screen
    display.setCursor(0,0);
    display.clearDisplay();
    display.println(dt);
    if (prevmetercount != metercount)
    {
      prevmetercount = metercount;
    }
    display.println(prevmetercount/TicksPerLiter,DEC);
    display.print("TAP ");display.print(lasttap);
    display.display();
#endif
// analyze every 10 secs
#ifdef USE_DS3234
    if ((TS.unixtime % 10) == 0)
#else
    if (millis() > (prevanalyzetime + TEN_SECONDS))
#endif
    {
      prevanalyzetime = millis();
      Analyze();
    }
  }
  if (millis()/ONE_SECOND > (prevReportTime + REPORT_RATE))
  {
    // time for a regular report
    prevReportTime = millis()/ONE_SECOND;
    sprintf(tempbuf,"%s: Tap %s Liters %u",RemoteGetClock(),lasttap == TAP_OPEN ? "open" : "closed",metercount/TicksPerLiter);
    RemoteSendMessage(tempbuf);
  }
}



