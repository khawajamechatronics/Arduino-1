int j;
void setup()
{

  /* add setup code here */
	Serial.begin(115200);
	j = 0;
}

void loop()
{

  /* add main program code here */
	Serial.print("j ");
	Serial.println(j);
	j++;
}
