


// GPRS a6 PWR_KEY 1 LOWER, RST 7 LOWER
#define TRANSISTOR_BASE  7  // RST
#define START_BAUD 115200
#define CHANNEL Serial2
void hwreset()
{
  Serial.println("\r\nReset");
  digitalWrite(TRANSISTOR_BASE,LOW);
  delay(100);
  digitalWrite(TRANSISTOR_BASE,HIGH);  
}
void setup() 
{
  //Serial connection.
  Serial.begin(115200);
  Serial.println("Start");
  CHANNEL.begin(START_BAUD);
  pinMode(TRANSISTOR_BASE,OUTPUT);
  digitalWrite(TRANSISTOR_BASE,HIGH);
  hwreset();
};

int brate = 1;  // GPRS A^ only supports 115200 so no change
void loop() { // run over and over
  char c;
  if (CHANNEL.available()) {
    Serial.write(CHANNEL.read());
  }
  if (Serial.available()) {
    c = Serial.read();
    if (c == ']')
      hwreset();
    else
      CHANNEL.write(c);
  }
}
/*
 * AT+IPR?
 * AT+GSN
 * ATD0545919886
 * AT+CREG?
 * AT+CGATT=1
 * AT+CGATT=0
 */
