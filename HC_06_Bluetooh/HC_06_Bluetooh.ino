#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX
void setup()
{
  Serial.begin(9600);
  Serial.println("Serial to Software Serial");
  mySerial.begin(9600);
}
void loop()
{
  // send data only when you receive data:
   if (Serial.available() > 0)
  {
    mySerial.write(Serial.read());
  }
  if (mySerial.available() > 0)
  {
    Serial.write(mySerial.read());
  }
}

