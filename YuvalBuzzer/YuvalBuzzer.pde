/*
  Read potentiometer on A2 (values 0 - 1023) to set delay to
  same number of seconds.
  When timer expires start buzzer and leave it running for 10 secs.
  When buzzer stops start the whole thing again
  Buzeer is run by PWM on pin D9/PB1
 */
#define P_DIVIDER 20  // so we get delay 0-50 secs
#define LED_ON 10000  // millsecs that led will be on
/*
 Some code copied from GadgetShield Melody example
*/
#include <avr/pgmspace.h>
#include <GadgetShield.h>

// Convert note number to physical frequencies. This mapping uses 110Hz as
// note 0.
uint16_t note2freq[64] = {
  110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208,
  220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415,
  440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831,
  880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661,
  1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322,
  3520, 3729, 3951, 0/*63==silence*/ };

// Which note is currently playing
unsigned note_index;

int sensorPin = A2;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
unsigned long exitloop1,exitloop2;
unsigned long currenttime;

void setup() {
  Serial.begin(115200);
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  // turn the ledPin off:        
  digitalWrite(ledPin, LOW);   
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin); 
  Serial.print("Sensor ");
  Serial.println(sensorValue); 
  exitloop1 = millis() + ((sensorValue / P_DIVIDER) * 1000);
  currenttime = 0;
  GS.Setup();
  GS.Speaker(0);
}

void loop()
{
  int note = 40;
  if (millis()/1000 > currenttime)
  {
    Serial.print("tick ");
    Serial.println(currenttime++);
  }
  if (millis() > exitloop1)
  {
    // turn on led and calculate when to turn off
    digitalWrite(ledPin, HIGH); 
    Serial.println("LON");
    GS.Speaker(note2freq[note]);
    exitloop2 =  exitloop1 + LED_ON;
    while (millis() < exitloop2)
    {
      if (millis()/1000 > currenttime)
      {
        Serial.print("tick ");
        Serial.println(currenttime++);
      }
    }
    // turn on led off
    digitalWrite(ledPin, LOW); 
    GS.Speaker(0);
    Serial.println("LOFF");
    // recalc exitloop1
    sensorValue = analogRead(sensorPin);
    exitloop1 = millis() + ((sensorValue / P_DIVIDER) * 1000);
    Serial.print("el1 ");
    Serial.println(exitloop1/1000);
  }
}
