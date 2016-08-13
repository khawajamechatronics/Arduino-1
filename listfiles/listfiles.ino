/*
  Listfiles
 
 This example shows how print out the files in a 
 directory on a SD card 
 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 2 Feb 2014
 by Scott Fitzgerald
 
 This example code is in the public domain.
 	 
 */
#include <SD.h>
#include "UTFT.h"
// Declare which fonts we will be using
extern uint8_t SmallFont[];

File root;
UTFT myGLCD(ITDB32S,38,39,40,41);
int linenumber = 0;
#define CHARHEIGHT 10

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);  
  myGLCD.clrScr();
  myGLCD.setBackColor(255, 0, 0);
  Serial.print("Initializing SD card...");
  myGLCD.print("Initializing SD card...",LEFT,CHARHEIGHT*linenumber++);
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(10, OUTPUT);

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    myGLCD.print("initialization failed!",LEFT,CHARHEIGHT*linenumber++);
    return;
  }
  Serial.println("initialization done.");
  myGLCD.print("initialization done.",LEFT,CHARHEIGHT*linenumber++);

  root = SD.open("/");
  
  printDirectory(root, 0);
  
  Serial.println("done!");
}

void loop()
{
  // nothing happens after setup finishes.
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     myGLCD.print(entry.name(),LEFT,CHARHEIGHT*linenumber++);
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}




