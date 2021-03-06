// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

uint8_t address = 0x51; // PCA8565
//uint8_t address = 0x40; // BMA180
#include <Wire.h>
char pbuff[20];
int8_t first_register = 2;
uint8_t read_length = 7;
void setup()
{
 // TWBR=400000L;  // for 400KHz
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("I2C start");
}

void loop()
{
  Wire.beginTransmission(address);
  Wire.write(first_register);  // register 0
  Wire.endTransmission();
  Wire.requestFrom(address, read_length);    // request 6 bytes from slave device #2
  while(Wire.available())   // slave may send less than requested
  {
    uint8_t c = Wire.read(); // receive a byte as character
    Serial.print(c);
    Serial.print(",");
 //   sprintf(pbuff,"%02X,",c);
  //  Serial.print(pbuff);         // print the character
  }
  Serial.println();
  delay(500);
}
