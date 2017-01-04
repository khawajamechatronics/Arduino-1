const int GREEN_LED_PIN = 10;
const int BLUE_LED_PIN = 11;
const int RED_LED_PIN = 12;
const int ONBOARD_LED_PIN = 13;
const int METER_0_DRDY = 21; // alarm must be an interrupt pin
const int METER_1_DRDY = 20; // alarm must be an interrupt pin

enum eWaterFlow {FLOW_UNKNOWN,FLOWING,NOT_FLOWING,WATER_LEAK};
enum tapState {TAP_UNKNOWN,TAP_CLOSE,TAP_OPEN} ;
void TapChangeState(enum tapState t);

enum rgbled { RED_LED, BLUE_LED ,GREEN_LED};
enum rgbstate { LED_ON, LED_OFF };
void RGBActivate(enum rgbled led ,enum rgbstate onoff);

#define ONE_SECOND 1000
#define TEN_SECONDS (10L*ONE_SECOND)
#define MAX_FLOW_TIME 30
#define REPORT_RATE  13

char *TapToText();
unsigned long Sim900ToEpoch();
void ResetAll();


