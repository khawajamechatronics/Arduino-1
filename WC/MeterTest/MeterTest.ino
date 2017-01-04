/*
 *   Pin
 *   RED earth GREEN BLUE
 */
#include "utilities.h"

enum tapState  lasttap;
enum rgbstate redled;
enum rgbstate blueled;
enum rgbstate greenled;

unsigned long CurrentMeterCount[2]={0,0},PreviousMeterCount[2]={0,0},TotalMeterCount[2] = {0,0};
//unsigned TicksPerLiter[2] = {DEFAULT_TICKS_PER_LITER,DEFAULT_TICKS_PER_LITER};
unsigned long StartCountingPeriod = 0, prevReportTime = 0;
uint32_t prevsampletime = 0,prevanalyzetime = 0;
char *IMEI= "345";

bool ignoreMeter = false;
bool alarmflag,ledstate;
char tempbuf[300];

void MeterCounter(int n)
{
  if (!ignoreMeter)
  {
    TotalMeterCount[n]++;
    CurrentMeterCount[n]++;
  }
}
void MeterCounter(int n,int delta)
{
  if (!ignoreMeter)
  {
    TotalMeterCount[n] += delta;
    CurrentMeterCount[n] += delta;
  }
}

void Meter0Count ()
{
  MeterCounter(0);
}
void Meter0Count (int count)
{
    MeterCounter(0,count);
}
void Meter1Count ()
{
  MeterCounter(1);
}
void Meter1Count (int count)
{
    MeterCounter(1,count);
}

/*
 *   Check both meters
 */
enum eWaterFlow FlowAnalyze(int meter)
{
  enum eWaterFlow wf = FLOW_UNKNOWN;
  sprintf(tempbuf,"M %d C %lu P %lu",meter,CurrentMeterCount[meter],PreviousMeterCount[meter]);
  Serial.println(tempbuf);
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
      wf = FLOWING;
      if ((millis()/ONE_SECOND - StartCountingPeriod) >= MAX_FLOW_TIME)  // running for a minute or more)
      {
        wf = WATER_LEAK;
      }
    }
    PreviousMeterCount[meter] = CurrentMeterCount[meter];
    CurrentMeterCount[meter] = 0;
  }
  return wf;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(GREEN_LED_PIN,OUTPUT);
  pinMode(BLUE_LED_PIN,OUTPUT);
  pinMode(RED_LED_PIN,OUTPUT);
  pinMode(ONBOARD_LED_PIN,OUTPUT);
  digitalWrite(GREEN_LED_PIN,LOW);
  digitalWrite(BLUE_LED_PIN,LOW);
  digitalWrite(RED_LED_PIN,LOW);
  digitalWrite(ONBOARD_LED_PIN,LOW);
  pinMode(METER_0_DRDY,INPUT);
  attachInterrupt(digitalPinToInterrupt(METER_0_DRDY), Meter0Count, RISING);
  pinMode(METER_1_DRDY,INPUT);
  attachInterrupt(digitalPinToInterrupt(METER_1_DRDY), Meter1Count, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - prevsampletime) >= ONE_SECOND)
  {
    alarmflag = true;
    prevsampletime = millis();
    Meter0Count(15);
    Meter1Count(16);
  }
  if (alarmflag)
  {
    sprintf(tempbuf,"T %lu M %d %lu %lu",prevsampletime,ignoreMeter,TotalMeterCount[0],TotalMeterCount[1]);
    Serial.println(tempbuf);
    alarmflag = false;
// analyze every 10 secs
    if (millis() > (prevanalyzetime + TEN_SECONDS))
    {
      enum eWaterFlow wf[2];
      prevanalyzetime = millis();
      // check both meters and decide how to proceed
      wf[0] = FlowAnalyze(0);
      Serial.print("WF0 ");Serial.println(wf[0]);
      wf[1] = FlowAnalyze(1);
      Serial.print("WF1 ");Serial.println(wf[1]);
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
        TapChangeState(TAP_CLOSE);
       sprintf(tempbuf,"/WC/RawData.php?action=leak&imei=%s&uptime=%lu&m1=%lu&m2=%lu&tap=%s&epoch=%lu",
          IMEI,millis()/1000,TotalMeterCount[0],TotalMeterCount[1],TapToText(),Sim900ToEpoch());
        Serial.println(tempbuf);
        ResetAll();
        ignoreMeter = true;
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
    Serial.println(tempbuf);
  }
}
