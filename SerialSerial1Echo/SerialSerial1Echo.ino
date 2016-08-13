//#include <SoftwareSerial.h>
#define OTHER_SERIAL Serial1
//SoftwareSerial OTHER_SERIAL(2, 3); // RX, TX

void setup() {
  Serial.begin(115200);
  OTHER_SERIAL.begin(9600);
}

void loop() {
  if (OTHER_SERIAL.available())
  {
    byte b = OTHER_SERIAL.read();
    if (b == 0xb5)
      Serial.println();
    Serial.print(b,HEX);
  }
  if (Serial.available())
    OTHER_SERIAL.write(Serial.read());
}
