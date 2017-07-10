#include <SoftwareSerial.h>
#include "mp3player.h"

SoftwareSerial mp3uart(10,11);  // mp3 player only receives commands
MP3Player mp3(mp3uart);

// DTMF decoder connections
const int STQ = 3;        // must be interrupt pin
const int Q1 = 8;    
const int Q2 = 9;      
const int Q3 = 10;      
const int Q4 = 11;        

int dtmfvalue = -1;   // -1 means no meaningful value
bool newdtmf = false;
void dtmfrcv()
{
  dtmfvalue = digitalRead(Q1) + (digitalRead(Q2)<<1) + (digitalRead(Q3)<<2) + (digitalRead(Q4)<<3);
  newdtmf = true;
}

void setup() {
  Serial.begin(115200);
  mp3uart.begin(9600);
 // Make all DTMF pins input, STQ goes high on detecting tone so make it an interrupt
//  pinMode(STQ, INPUT);
 // pinMode(Q4, INPUT);
//  pinMode(Q3, INPUT);
//  pinMode(Q2, INPUT);
//  pinMode(Q1, INPUT);
//  attachInterrupt(digitalPinToInterrupt(STQ), dtmfrcv, RISING);
  Serial.println("Start..");
  mp3.begin(true);
  delay(1000);
 // mp3.PlayFolderName(1,1);
}

void loop() {
  // put your main code here, to run repeatedly:
  mp3.FeedRead();
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 'v':
        mp3.VolumeDown();
        break;
      case 'V':
        mp3.VolumeUp();
        break;
      case 'n':
        mp3.Next();
        break;
      case 'p':
        mp3.Previous();
        break;
      case 'P':
        mp3.Pause();
        break;
      case 'r':
        mp3.Resume();
        break;
      case 'x':
        mp3.PlayFolderName(4,4);
        break;
      case 'X':
        mp3.PlayFolderName(1,1);
        break;
      case 'f':
        mp3.FoldersPerDiskQuery();
        break;
      case 't':
        mp3.TracksPerFolderQuery(1);
        break;
    }
  }
}

void MP3Player::FeedbackCallback(enum eCommands c,byte b)
{
  Serial.print(c,HEX);
  Serial.print(' ');
  Serial.println(b);
}

