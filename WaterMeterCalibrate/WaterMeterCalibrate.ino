// Nokia5100 display
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SPI.h>

// meter_drdy and button pins must be interrupt enable pins
const int METER_DRDY = 21; // Pulse lead from the Hall Effect Sensor
const int BUTTON = 20; // Press button
// LCD digital pins should not use hardware SPI pins
// mosi 51 miso 50 sck 52 on Mega 2560
const int LCD_RST = 24; // Nokia 5100 reset
const int LCD_CS = 23; // Nokia 5100 chip select
const int LCD_DC = 25; // Nokia 5100 DC 

const int RED_LED_PIN = 41;

unsigned long metercount = 0;
unsigned long ttime;
enum eState {WAIT_TO_START,COUNTING,STOPPED } state = WAIT_TO_START;
// Display uses hardware SPI
Adafruit_PCD8544 display = Adafruit_PCD8544(LCD_DC, LCD_CS, LCD_RST);
bool ledstate = false;
void TapOnOff()
{
  if (digitalRead(BUTTON) == HIGH)
  {
    switch (state)
    {
      case WAIT_TO_START:
        state = COUNTING;
        metercount = 0;
        ttime = millis();
        display.setCursor(0,0);
        display.clearDisplay();
        display.println("Counting");
        display.display();
        break;
      case COUNTING:
        state = STOPPED;
        display.setCursor(0,0);
        display.clearDisplay();
        display.println("Stopped");
        display.print("ticks ");display.println(metercount);
        display.print("time ");display.println(millis()-ttime);
        display.display();
        break;
	  case STOPPED:
        state = WAIT_TO_START;
        display.setCursor(0,0);
        display.clearDisplay();
        display.println("Waiting");
        display.display();
        break;	
	  }
  }
}
void MeterCount()
{
  metercount++;
  if ((metercount % 10) == 0)
  {
    if (ledstate)
      digitalWrite(RED_LED_PIN,HIGH);
    else
      digitalWrite(RED_LED_PIN,LOW);
    ledstate = !ledstate;
  }
}
void setup() {
    // put your setup code here, to run once:
    pinMode(METER_DRDY,INPUT);
    pinMode(RED_LED_PIN,OUTPUT);
    attachInterrupt(digitalPinToInterrupt(METER_DRDY), MeterCount, RISING);
    pinMode(BUTTON,INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON),TapOnOff, CHANGE);
    digitalWrite(LCD_CS,HIGH);
    SPI.begin();
    SPI.setBitOrder(MSBFIRST); 
    // RTC is MODE3, Nokia MODE0 
    SPI.setDataMode(SPI_MODE0); // both mode 1 & 3 should work but only works properly with interrupt
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
    // display on screen
    display.setCursor(0,0);
    display.clearDisplay();
    display.println("Waiting");
    display.display();
 }

void loop() {
  // put your main code here, to run repeatedly:
}
