#define PPS_PIN 2
#define LED_PIN 13

int state = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(PPS_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PPS_PIN), blink, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, state);
}

void blink()
{
  state = !state;
}

