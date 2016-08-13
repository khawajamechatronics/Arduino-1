/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 11;
int LED = 13;
//#define ON_OFF 0x926d58a7 //LG streamer
//#define ON_OFF 0x1818D02F // YES TV & Haier
#define ON_OFF 0x213C7C83  // yes RED
bool ledon;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  ledon = false;
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
    if (results.value == ON_OFF)
    {
      if (ledon)
        digitalWrite(LED, LOW);
      else
        digitalWrite(LED, HIGH);
      ledon = !ledon;
    }
  }
  delay(100);
}
