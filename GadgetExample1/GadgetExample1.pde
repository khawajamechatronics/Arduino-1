/* Play a tone on the speaker whenever pushbutton #1 is pressed */
#include <GadgetShield.h>
int thisPinIndex = 0;
int thisPin;
int allpins[] = {LED1_PIN,LED2_PIN,LED3_PIN,LED4_PIN};
void setup(void) {

  GS.Setup();
//  pinMode(LED4_PIN, OUTPUT);  already done in GS.Setup    
}

void loop(void) {

 if (GS.IsSwitch(0))
 {
   thisPin = allpins[thisPinIndex % 4];
   digitalWrite(thisPin, HIGH);   // set the LED on
   GS.Speaker(440);
 }
 else
  {
   digitalWrite(thisPin, LOW);   // set the LED on
  GS.Speaker(0);
  thisPinIndex++;
  }
}
