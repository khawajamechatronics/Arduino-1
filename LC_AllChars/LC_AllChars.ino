/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7,8,9,10,11,12);
byte aleph[8] = {
        B10001,
        B10001,
        B11010,
        B10100,
        B10010,
        B10001,
        B11001,
        B00000
};
byte beth[8] = {
        B11110,
        B00010,
        B00010,
        B00010,
        B00010,
        B00010,
        B11111,
        B00000
};

byte gimmel[8] = {
        B11110,
        B00010,
        B00010,
        B00010,
        B01110,
        B10010,
        B10010,
        B00000
};

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
 // lcd.print("hello, world!");
 lcd.createChar(0, aleph);
 lcd.createChar(1, beth);
 lcd.createChar(2, gimmel);
 lcd.createChar(3, aleph);
 lcd.createChar(4, aleph);
 lcd.createChar(5, aleph);
 lcd.createChar(6, aleph);
 lcd.createChar(7, aleph);
}

void loop() {
  int base = 0;
  int col;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  for (base = 0;base < 16;)
  {
    lcd.setCursor(0,0);
   //  lcd.print("hello, world!");
    for (col=0;col<16;col++)
    {
      lcd.write(base+col);
    }
    base += 16;
    lcd.setCursor(0,1);
 //    lcd.print("hello, world!");
    for (col=0;col<16;col++)
    {
      lcd.write(base+col);
    }
    base += 16;
    delay(1000);
  }
}

