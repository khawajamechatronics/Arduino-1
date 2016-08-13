/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
int sensorPin = A0; 
#define RED 41
#define GREEN 42
#define BLUE 43
void green(bool on)
{
  if (on)
    digitalWrite(GREEN,HIGH);
   else
    digitalWrite(GREEN,LOW);
}
void yellow(bool on)
{
  if (on)
    digitalWrite(BLUE,HIGH);
   else
    digitalWrite(BLUE,LOW);
}
void red(bool on)
{
  if (on)
    digitalWrite(RED,HIGH);
   else
    digitalWrite(RED,LOW);
}

bool abba()
{
  int s = analogRead(sensorPin);
  if (s>600)
  {
    while (s>600)
    {
      red(false);
      yellow(false);
      green(true);
      s = analogRead(sensorPin);
    }
    return true;
  }
  else
    return false;
}
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(10, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  red(false);
  yellow(false);
  green(false);
  Serial.begin(9600);
}
void ramzor()
{
  red(true);
  green(false);
  yellow(false);
  delay(2000);
  if (abba())
    return;
  yellow(true);
  if (abba())
    return;
  delay(2000);
  red(false);
  yellow(false);
  green(true);
  delay(3000);
  if (abba())
    return;
  green(false);
  yellow(true);
  if (abba())
    return;
  delay(1000);
  yellow(false);
}
  
// the loop function runs over and over again forever
void loop() {
  ramzor();
  delay(500);
}
