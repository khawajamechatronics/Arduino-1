
/*
  PCA8586  
  Set initial date time
  Setup interrupt on minute vale
  
*/
// Uno Duemilanove SDA A4 SCL A5

// interrupt to pin 2

int clockinterrupt = 2;
int flag = 0;  // set to 1 on interrupt
void clkint();

uint8_t address = 0x51; // PCA8565
#include <Wire.h>
char pbuff[20];
int8_t first_register = 0;
uint8_t read_length = 16;
uint8_t startdate[8] = { // 31/12/2012 23:59:50  set as BCD
  2,    // Seconds register
  0x50, // secs
  0x59, // minutes
  0x23, // hours
  0x31, // day of month 1-31
  0x01, // day of week 0-6 monday
  0x12, // month 1-12
  0x12  // year
};
uint8_t nextMinuteInterrupt = 0;

void setMinInterrupt(uint8_t reg,uint8_t when)
{
  Wire.beginTransmission(address);
  Wire.write(reg);  
  Wire.write(when);  
  Wire.endTransmission();
  // turn on TIE in CONTROL_2
  Wire.beginTransmission(address);
  Wire.write(1); // register CONTROL_2  
  Wire.write(0x42); // TI_TP | TIE
  Wire.endTransmission();
}
void setup()
{
  int j;
  // setup interrupt stuff
  pinMode(clockinterrupt, INPUT);
  // INT pin is ACTIVE LOW
  attachInterrupt(0,clkint,FALLING);
  
 // TWBR=400000L;  // for 400KHz
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("I2C start");
  // set up start date should see year overflow in 10 secs
  Wire.beginTransmission(address);
  for (j=0;j<8;j++)
    Wire.write(startdate[j]);  
  Wire.endTransmission();
  // setup minute interrupt when minute overflows to 00
  setMinInterrupt(9,nextMinuteInterrupt);
  // enable interrupts
  interrupts();
}

void loop()
{
  // check if there was an interrupt
  if (flag)
  {
    Serial.println("interrupt");
    flag = 0;
    // set next interrupt
    setMinInterrupt(9,++nextMinuteInterrupt);
  }
  Wire.beginTransmission(address);
  Wire.write(first_register);  
  Wire.endTransmission();
  Wire.requestFrom(address, read_length);    
  while(Wire.available())   // slave may send less than requested
  {
    uint8_t c = Wire.read(); // receive a byte as character
 //   Serial.print(c);
 //   Serial.print(",");
    sprintf(pbuff,"%02X,",c);
    Serial.print(pbuff);         // print the character
  }
  Serial.println();
  delay(1000);
}

// Install the interrupt routine.
void clkint() {
  // check the value again - since it takes some time to
  // activate the interrupt routine, we get a clear signal.
  uint8_t value = digitalRead(clockinterrupt);
  flag = 1;
}

