/*
* Example RF transmitter Code
*/

float temp = -20.0;
byte msg[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 0, 0, 0, 0, 0};

void setup()
{
  //2400 baud for the 434 model
  Serial.begin(2400);
}
void loop()
{
  byte * b = (byte *) &temp; // Create pointer for float->byte convertion
  //send out to transmitter
  msg[sizeof(msg)-5] = b[0];
  msg[sizeof(msg)-4] = b[1];
  msg[sizeof(msg)-3] = b[2];
  msg[sizeof(msg)-2] = b[3];
  msg[sizeof(msg)-1] = CalcCRC(msg, sizeof(msg)-5); // Checksum is xor of 4 temperature bytes
 for (int j=0; j<10; j++) // Send 10 times to make sure data is received
   for (int i=0; i<sizeof(msg); i++)
      Serial.print(msg[i]);
  // Change temperature
  temp+=12.6;
  if (temp >= 100)
    temp  = -20.0;
  
  delay(2000);
}

byte CalcCRC(byte* buf, byte offset)
{
  byte crc;
  for (int i=offset; i<(offset+4); i++)
    crc ^= buf[i];
  return crc;
}

