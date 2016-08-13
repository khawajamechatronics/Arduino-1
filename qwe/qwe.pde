int loopc = 0;
void setup()
{

	pinMode(13,OUTPUT);
  /* add setup code here */
	Serial.begin(115200);
}

void loop()
{
	digitalWrite(13,HIGH);
	delay(100);
	digitalWrite(13,LOW);
	delay(100);
  /* add main program code here */
	loopc++;
	Serial.println(loopc);
}
