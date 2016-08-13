long j;
void setup()
{

  /* add setup code here */
	Serial.begin(9600);
}

void loop()
{

  /* add main program code here */
  Serial.println(j++);
}
