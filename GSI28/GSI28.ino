/*
 GSI28
 Reads digital input on pins 2-5 (paper), 7-10(pen holder)
  Analog pin A0 (pen) 
  Print the results to the serial monitor
 */
#define RATE_HZ 250 // 210 effective
unsigned int counter,A,B,C;  // paper, penholder, pen
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  // make the pins input:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  counter = 0;
}
void loop() {
  // read the input pins into composite words:
  char linebuf[30];
  A = digitalRead(2) + (digitalRead(3)<<1)+ (digitalRead(4)<<2)+ (digitalRead(5)<<3);
  B = digitalRead(7) + (digitalRead(8)<<1)+ (digitalRead(9)<<2)+ (digitalRead(10)<<3);
  C = analogRead(A0);
  sprintf(linebuf,"%u,%u,%u,%u",counter++,A,B,C);
  Serial.println(linebuf);
  delay(1000/RATE_HZ); 
}



