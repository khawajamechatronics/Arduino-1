// Init the Pins used for PWM
const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;
 
// Init the Pins used for 10K pots
const int redPotPin = 0;
const int greenPotPin = 1;
const int bluePotPin = 2;
 
// Init our Vars
int currentColorValueRed;
int currentColorValueGreen;
int currentColorValueBlue;
 
void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(9600);
}

 void loop()
{
// Read the voltage on each analog pin then scale down to 0-255 and inverting the value for common anode
  currentColorValueRed = map( analogRead(redPotPin), 0, 1024, 0, 255 );
  currentColorValueBlue = map( analogRead(bluePotPin), 0, 1024, 0, 255 );
  currentColorValueGreen = map( analogRead(greenPotPin), 0, 1024, 0, 255 );
//  Serial.print("R ");Serial.println(analogRead(redPotPin));
//  Serial.print("G ");Serial.println(analogRead(greenPotPin));
//  Serial.print("B ");Serial.println(analogRead(bluePotPin));
//  delay(1000);
 
// Write the color to each pin using PWM and the value gathered above
  analogWrite(redPin, currentColorValueRed);
  analogWrite(bluePin, currentColorValueBlue);
  analogWrite(greenPin, currentColorValueGreen);
 
}
