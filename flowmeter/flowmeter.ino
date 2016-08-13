#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
// Hardware SPI 11 MOSI 13 CLK
Adafruit_PCD8544 display = Adafruit_PCD8544(25, 24, 23);

#define PPS_PIN 2
#define LED_PIN 13
#define TICKS_PER_LITER (2400/8)

int state = 0;
unsigned long count = 0, prevcount = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(PPS_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), blink, RISING);
  Serial.begin(115200);
  display.begin();
  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);

  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(BLACK);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, state);
  if (prevcount != count)
  {
    prevcount = count;
    display.setCursor(0,0);
    display.clearDisplay();
    display.print(prevcount/TICKS_PER_LITER,DEC);
    display.display();
  }
}

void blink()
{
  state = !state;
  ++count;
}



