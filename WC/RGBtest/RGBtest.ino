/*
 *   Pin
 *   RED earth GREEN BLUE
 */
const int GREEN_LED_PIN = 10;
const int BLUE_LED_PIN = 11;
const int RED_LED_PIN = 12;

void setup() {
  // put your setup code here, to run once:
  pinMode(GREEN_LED_PIN,OUTPUT);
  pinMode(BLUE_LED_PIN,OUTPUT);
  pinMode(RED_LED_PIN,OUTPUT);
 // digitalWrite(GREEN_LED_PIN,HIGH);
//  digitalWrite(BLUE_LED_PIN,HIGH);
  digitalWrite(RED_LED_PIN,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}
