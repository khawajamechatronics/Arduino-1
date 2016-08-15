int dip1Pin = 3; // D3
int dip2Pin = 4; // D4
int dip3Pin = 5; // D5
int dip4Pin = 6; // D6

int readDIP()
{
  char buffer[50];
  int j =  digitalRead(dip4Pin) + (digitalRead(dip3Pin)<<1)+ (digitalRead(dip2Pin)<<2)+ (digitalRead(dip1Pin)<<3);
  sprintf(buffer,"%d %d %d %d",digitalRead(dip1Pin),digitalRead(dip2Pin),digitalRead(dip3Pin),digitalRead(dip4Pin));
  Serial.println(buffer);
  return j;
}

void setup()
{
Serial.begin(115200);
 // define all pins and turn on pullu[
  pinMode(dip1Pin,INPUT);
  digitalWrite(dip1Pin,HIGH);
  pinMode(dip2Pin,INPUT);
  digitalWrite(dip2Pin,HIGH);
  pinMode(dip3Pin,INPUT);
  digitalWrite(dip3Pin,HIGH);
  pinMode(dip4Pin,INPUT);
  digitalWrite(dip4Pin,HIGH);
}

void loop()
{
  int j = readDIP();
   Serial.println(j);
  delay(1000);
}

