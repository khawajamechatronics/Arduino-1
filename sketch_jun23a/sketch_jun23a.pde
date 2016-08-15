#include <avr/io.h>
#include <avr/wdt.h>
int count = 0;

unsigned long start;
boolean doreset;
void setup(){
  Serial.begin(115200);
  Serial.println(count);
  wdt_enable(WDTO_4S);
  start = millis();
  doreset = false;
} 

void loop()
{
  //while (true);
  if (Serial.available())
    doreset = true;
  if ((millis() - start) > 3000)
  {
    if (doreset == true)
      wdt_reset();
    Serial.println(count++);
    start = millis();
  }
}
