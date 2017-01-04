/*
 *    UNO pin current 40mA max
 *    According to datasheets on IR LEDS sold by Adafruit and Spartkfun, typical
 *    led takes up to 50mA, Use 330ohm resistor
 *    If driving both pins off 1 pin need a MOSFET
 */
#include <TimerOne.h>

#define LEFT_IR_RECEIVER 2    // must be an interrupt pin
#define RIGHT_IR_RECEIVER 3    // must be an interrupt pin
#define LH_IR_SENDER 5
#define RH_IR_SENDER 6
#define LEFT_LED 7
#define RIGHT_LED 8

#define IR_RATE 37900L   // rate at which IR sensor works

bool IRLedOn = false;
void IRLedToggle()
{
  digitalWrite(LH_IR_SENDER,IRLedOn ? HIGH : LOW );
  digitalWrite(RH_IR_SENDER,IRLedOn ? HIGH : LOW );
  IRLedOn = !IRLedOn;
}
void leftir()
{
  Serial.println('L');
  digitalWrite(LEFT_LED,!digitalRead(LEFT_IR_RECEIVER));
}
void rightir()
{
  Serial.println('R');
  digitalWrite(RIGHT_LED,!digitalRead(RIGHT_IR_RECEIVER));
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LH_IR_SENDER,OUTPUT);
  pinMode(RH_IR_SENDER,OUTPUT);
  Timer1.initialize((1000000L/IR_RATE)/2);   // toggle IR led at 38KHz
  Timer1.attachInterrupt( IRLedToggle );
  pinMode(LEFT_LED,OUTPUT);
  digitalWrite(LEFT_LED,HIGH);
  pinMode(RIGHT_LED,OUTPUT);
  digitalWrite(RIGHT_LED,HIGH);
  pinMode(LEFT_IR_RECEIVER,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_IR_RECEIVER),leftir,CHANGE);
  pinMode(RIGHT_IR_RECEIVER,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RIGHT_IR_RECEIVER),rightir,CHANGE);
}

void loop() {
}
