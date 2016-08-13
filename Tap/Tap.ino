/*
 * SIM900 shield on Mega, disconnect SW/HW serial jumpers
 * Connect Mega RX1 to shield RX, TX1 to shield TX
 * May need 5V power supply to jack if USB not enough
 * 
 * Flow meter sensor must be on interrupt pin so choice limited
 * 
 * Mokia 5100 display on pins 3-7
 */
#define SOURCE_GSM
#include "SIM900.h"
#include "sms.h"
SMSGSM sms;
// LCD stuff
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

char *DefaultHouse = "0545919886";
char Subscriber[20];
boolean started=false;
char smsbuffer[160];
char n[20]= {0};
#define GSM_SERIAL Serial1
#define GSM_BAUD 115200

char temp[50];
#define LED 12
#define WM 2    // water meter pin
int lasttap = -1;
bool ledon;

void ToggleUp()
{
  lasttap = 1;
  RelaySet(LOW,LOW);
  delay(100);
  RelaySet(LOW,HIGH);
}
void ToggleDown()
{
  lasttap = 0;
  RelaySet(HIGH,HIGH);
  delay(100);
  RelaySet(HIGH,LOW);
}
void RelaySet(int in1, int in2)
{
  // sets power on to other side of relay
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
}
void ledToggle()
{
  if (ledon)
    digitalWrite(LED,LOW);
  else
    digitalWrite(LED,HIGH);
  ledon = !ledon;
}
void setup()
{
  Serial.begin(9600);
  pinMode(GSM_ON,INPUT);
  pinMode(LED,OUTPUT);
  ledon = true;
  digitalWrite(LED,HIGH);
#ifdef SOURCE_GSM
  // initialize serial port used by GSM shield
  GSM_SERIAL.begin(GSM_BAUD);
  if (gsm.begin(GSM_BAUD))
  {
    Serial.println("\nstatus=READY");
    started=true;  
  }
#endif
  display.begin();
  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);

  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(2);
  display.setTextColor(BLACK);
// initialize relay pins and turn power off to output 
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(LOW,HIGH);
}

void Parse(String s)
{
  s.trim();
  Serial.println(s);
  if (s.equals(String("/ton")))
    ToggleUp();
  else if (s.equals(String("/toff")))
    ToggleDown();
  else if (s.equals(String("/p")))
  {
    sprintf(temp,"Tap %d",lasttap);
    Serial.println(temp);
    if (sms.SendSMS(n, temp))
      Serial.println("\nSMS sent OK");
  }
}

void loop()
{
  int i;
  ledToggle();
#ifdef SOURCE_GSMx
  if(started)
  {
    //Read if there are messages on SIM card and print them.
    if(gsm.readSMS(smsbuffer, 160, n, 20))
    {
      Serial.println(n);
      Serial.println(smsbuffer);
      strcpy(Subscriber,n);
      Parse(String(smsbuffer));
    }
    delay(1000);
  }
#elif defined(SOURCE_GSM)
  if (started)
  {
    i = sms.IsSMSPresent(SMS_UNREAD);
    if (i > 0)
    {
      sms.GetSMS(i,n,smsbuffer,160);
      Serial.println(n);
      Serial.println(smsbuffer);
      strcpy(Subscriber,n);
      Parse(String(smsbuffer));
    }
  }
#else
  String command;
// get command and execute
  command = Serial.readString();
  command.trim();
//  Serial.println(command);
  Parse(command);
#endif
}
