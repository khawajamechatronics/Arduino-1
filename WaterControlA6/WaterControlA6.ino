#include <string.h>
#include <Time.h>
#include <limits.h>
#include "A6Services.h"
#include "A6HTTP.h"
#include "A6CALL.h"
#include "Configuration.h"
#include "RGB.h"
#include "Relay.h"
#include "WCEEPROM.h"
#include "Utilities.h"
#include "TCP.h"

A6GPRS gsm(Serial1,100,2000);
A6HTTP http(gsm);
A6CALL call(gsm);

enum eWaterFlow {FLOW_UNKNOWN,FLOWING,NOT_FLOWING,WATER_LEAK};
void SMSParse(char * s);

// Global Data
#define BIG_BUFF_SIZE 100
static char bigbuff[BIG_BUFF_SIZE];
bool ledstate = false;
bool SMSout = false;  // save on sending SMS just print to Serial
unsigned MAX_FLOW_TIME = DEFAULT_LEAK_IN_SECS;
unsigned long REPORT_RATE = DEFAULT_REPORT_RATE; 
unsigned reboottime = DEFAULT_REBOOT_TIME;
unsigned PulseWidth = 100;
bool polaritySwitch = true;
int gprstimeout = DEFAULT_GPRS_TIMEOUT;
char IMEI[20];  
time_t ep;
unsigned long totalMeterCount[2] = {0,0}, prevReportTime = 0;
unsigned long StartCountingPeriod=UINT_MAX,CurrentMeterCount[2]={0,0},PreviousMeterCount[2]={0,0};
bool tcpActive = false;
bool ignoreMeter = false;
bool smsArrived = false;
uint32_t httptimeout = UINT_MAX;
bool doReport = false;
//int reports[2] = {0,0};  // reports sent, succ completion
//uint32_t last200 = 0;
//int sprintfsize = -1;
uint32_t prevsampletime = 0,prevanalyzetime = 0;
bool sampleFlag;
long lastblink = -1;
long nextSample = INT_MAX;
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

void MeterCount1()
{
    totalMeterCount[0]++;
    CurrentMeterCount[0]++;
}

void MeterCount2()
{
    totalMeterCount[1]++;
    CurrentMeterCount[1]++;
}

void setup() {
//  InitEepromData();
  // set up the various GPIO hardware
  // LEDS
  RGBInit();
  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,LOW);
  // Timeout trigger
  pinMode(RESET_TRIGGER,OUTPUT);
  digitalWrite(RESET_TRIGGER,HIGH);
  // RELAY
// force relay to power off position
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(HIGH,LOW);
  TapChangeState(TAP_OPEN);
  // Hall Effect Meters
  pinMode(METER_DRDY_1,INPUT);
  pinMode(METER_DRDY_2,INPUT);
  // response for water meter pulses
  attachInterrupt(digitalPinToInterrupt(METER_DRDY_1), MeterCount1, RISING);
  attachInterrupt(digitalPinToInterrupt(METER_DRDY_2), MeterCount2, RISING);
  // Serial terminal and modem
  Serial.begin(115200);
  Serial1.begin(115200);
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  PrintEepromData();
  gsm.enableDebug = false;
  if (gsm.begin())
  {
    Serial.println("gsm up");
    gsm.setSmsMode(gsm.SMS_TEXT);
    if (!gsm.getIMEI(IMEI))
      strcpy(IMEI,"fakeimei");
    Serial.println(IMEI);
//    strcpy(IMEI,"999");
    Serial.print("Current Time: ");
    if (gsm.getRTC(bigbuff))
      ep = GSMToEpoch(bigbuff);
    else
      ep = 0;
    Serial.println(ep);
    if (gsm.startGPRS(EEPROMGetIndex(APN)))
    {
      Serial.println("GPRS up");
      if (gsm.getLocalIP(bigbuff))
        Serial.println(bigbuff);
      // announce to the world we're up
      sprintf(bigbuff,"%s/RawData.php?AC=powerup&imei=%s&UT=%lu&m0=%lu&m1=%lu&epoch=%lu",EEPROMGetIndex(WWWPATH),
                IMEI,millis()/1000,totalMeterCount[0],totalMeterCount[1],ep);
      Serial.println(bigbuff);
     // gsm.sendSMS(EEPROMGetIndex(DP),bigbuff);
      tcpActive = HTTPGET(EEPROMGetIndex(HTTPs),bigbuff,80);
    }
    else
    {
      Serial.print("Start ");
      Serial.print(EEPROMGetIndex(APN));
      Serial.println(" failed");
    }
  }
  nextSample = millis();
}

void loop() {
  byte *mm;
  unsigned l;
  char rtctime[30];
  if (sampleFlag)
  {
    sampleFlag = false;
    enum eWaterFlow wf[2];
//    Serial.print("Sampling...");Serial.print(PreviousMeterCount[0]);Serial.print(",");Serial.println(PreviousMeterCount[1]);
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
      if (gsm.getRTC(rtctime))
        ep = GSMToEpoch(rtctime);
      else
        ep = 0;
      sprintf(bigbuff,"%s/RawData.php?AC=leak&imei=%s&UT=%lu&m0=%lu&m1=%lu&tap=%s&epoch=%lu",EEPROMGetIndex(WWWPATH),
        IMEI,millis()/1000,totalMeterCount[0],totalMeterCount[1],TapToText(),ep);
      TapChangeState(TAP_CLOSE);
      tcpActive = HTTPGET(EEPROMGetIndex(HTTPs),bigbuff,80);
      sprintf(bigbuff,"leak uptime=%lu ts=%s",millis()/1000,rtctime);
      gsm.sendSMS(EEPROMGetIndex(DP),bigbuff);
    }
    else
    {
      // what to do if meters dont agree ?
    }
  }
  mm = gsm.Parse(&l);
  if (l != 0)
  {
    if (tcpActive)
      http.Parse(mm,l);
    else
      call.Parse(mm,l);
  }
  if (smsArrived)
  {
    smsArrived = false;
    SMSParse(call.smsbuffer);
  }
  if (doReport)
  {
    doReport = false;
    sprintf(bigbuff,"%s/RawData.php?AC=report&imei=%s&UT=%lu&m0=%lu&m1=%lu&epoch=%lu&tap=%s",EEPROMGetIndex(WWWPATH),
              IMEI,millis()/1000,totalMeterCount[0],totalMeterCount[1],ep,TapToText());
    Serial.println(bigbuff);
    tcpActive = HTTPGET(EEPROMGetIndex(HTTPs),bigbuff,80);
//    doReport = !tcpActive;  // if we want to retry after failure
  }
}


void A6HTTP::OnDataReceived(byte text[],unsigned length)
{
  char *ss;
  int httpresponse;
  text[length]=0;
  Serial.print("Got Data length ");
  Serial.println(length);
  Serial.print((char *)text);
  // check for HTTP response
  if (strncmp(text,"HTTP/1",6) == 0)  // may be HTTP/1.0 or HTTP/1.1
  {
    ss = strchr(text,' ');   // looking for HTTP/1.1 200 OK
    if (ss)
      httpresponse = atoi(ss);
    else
      httpresponse = 0;
  }
  // check if buffer contains </html>
  ss = strrchr(text,'<');  // no way to look for substrings here
  if (ss && *++ss == '/')
  {
    ss++;
    if (strncmp("html>",ss,5) == 0)
    {
      Serial.println("</html>");
      int ips = gsm.getCIPstatus();
      Serial.print("IP status: ");Serial.println(gsm.getCIPstatusString(ips));
      if (ips == gsm.CONNECT_OK)
      {
        if (gsm.disconnectTCPserver())
        {
          tcpActive = false;
          Serial.println("Server down:");      
        }        
      }
    }
  }
}

/*
 * TIMER0 is used to generate the millis counter. Lets use its interrupt
 */

SIGNAL(TIMER0_COMPA_vect) 
{
  long ms = millis()/1000;
  if (ms != lastblink)
  {
  //  Serial.print("t");
    lastblink = ms;
    digitalWrite(ONBOARD_LED,ledstate ? LOW : HIGH);
    ledstate = !ledstate;
  }
  if (millis() > nextSample)
  {
    sampleFlag = true;
    nextSample = millis() + TEN_SECONDS;  // every ten secs
  }
  // force poweron every 2 hours
#ifdef REBOOT
  if (ms > reboottime)
  {
    digitalWrite(RESET_TRIGGER,LOW);
    while (true){}
  }
#endif
  // devise a watchdog like mechanism using TIMER0 - 
//  watchdogcounter++;
//  if (watchdogcounter > (TEN_SECONDS*6))
//  {
//    Serial.println("watchdog");
//    digitalWrite(RESET_TRIGGER,LOW);
 //   while (true){}
//  }
  if (millis() % (REPORT_RATE * 1000) == 0)
    doReport = true;
}

