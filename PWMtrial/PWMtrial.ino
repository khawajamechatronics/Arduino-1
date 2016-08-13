/*
 * PWM games
 * Change pin 9 from default frequency 490Hz to 30.64Hz 
 * Output square wave at 50% duty cycle
 */
void setup() {
  // put your setup code here, to run once:
  // pins 9, 10
  //set timer 1 divisor to  1024 for PWM frequency of 30.64 Hz
 // TCCR1B = TCCR1B & B11111000 | B00000101;
//  analogWrite(9,128);
// pins 5,6 
// set timer 0 divisor to 8 for PWM frequency of 7812.50 Hz
  TCCR0B = TCCR0B & B11111000 | B00000010; 
   analogWrite(5,128); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
