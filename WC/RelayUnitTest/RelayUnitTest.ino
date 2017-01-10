// 5V relay pins
#define IN1 6
#define IN2 7
#define LED 13
//#define BUTTON 20

int state = 0;
void RelaySet(int in1, int in2)
{
  // sets power on to other side of relay
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
}
void Action(int s)
{
    switch (s)
  {
    case 0:
      RelaySet(HIGH,HIGH);
      break;
    case 1:
      RelaySet(HIGH,LOW);
      break;
    case 2:
      RelaySet(LOW,LOW);
      break;
    case 3:
      RelaySet(LOW,HIGH);
      break;
  }
}
#ifdef BUTTON
void TapOnOff()
{
if (digitalRead(BUTTON) ==LOW)
  state++;
  state %= 4;
  Action(state);
}
#endif
void TapOpen()
{
  RelaySet(HIGH,HIGH);
  delay(100);
  RelaySet(HIGH,LOW);
}
void TapClose()
{
  RelaySet(LOW,LOW);
  delay(100);
  RelaySet(LOW,HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(11500);
// initialize relay pins and turn power off to output 
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(LOW,HIGH);
#ifdef BUTTON
  // well use a button to turn on/off
  pinMode(BUTTON,INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON),TapOnOff, CHANGE);
#endif
}
void loop() {
#if 0
  TapOpen();
  delay(10000);
  TapClose();
  delay(10000);
#else
  Serial.println("LL");
  RelaySet(LOW,LOW);
  delay(3000);
  Serial.println("LH");
  RelaySet(LOW,HIGH);
  delay(3000);
  Serial.println("HH");
  RelaySet(HIGH,HIGH);
  delay(3000);
  Serial.println("HL");
  RelaySet(HIGH,LOW);
  delay(3000);  
#endif
}
