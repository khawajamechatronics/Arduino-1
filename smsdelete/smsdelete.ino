/*
 *   Print all SMS on SIM card and delete them 
 */
#include "SIM900.h"
#include "sms.h"
SMSGSM sms;
bool gsmon = false;
char sender[50];
char smsbuffer[160];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);  // used by GSM
  if (gsm.begin(115200))
  {
    Serial.println("GSM on");
    gsmon = true;
  }
  else
    Serial.println("GSM off");
}

void loop() {
  int i;
  if (gsmon)
  {
    i = sms.IsSMSPresent(SMS_ALL);
    Serial.println(i);
    if (i > 0)
    {
      sms.GetSMS(i,sender,smsbuffer,160);
      Serial.println(sender);
      Serial.println(smsbuffer);
      sms.DeleteSMS(i);
    }
    delay(1000);
  }
}
