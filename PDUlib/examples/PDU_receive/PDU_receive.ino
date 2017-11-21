/*
 * decodePDU works on the ASSCII string output from you modem.
 * It splits into 4 components parts,
 *     Service control number
 *     Timestamp in YYMMDDHHMMSS format
 *     Sender phone number. Your carrier decides how to format that i.e. leave national numbers
 *         as is or extend them to International format
 *     Text is the actual message.
 *     
 *     Only 7-bit Alphabet can be decoded right now. Id an 8 0r 16 bit messag arrives a dummy message is substituted
 * 
 */
#include "pdulib.h"

// 0545919886 sent Abcfdgh123 SCA +972541200320 timestamp 17111619570608
char received[] = "07917952140230F2040C917952541989680000711161917560800A41F1D84C3EA363B219";
char sca[100], sender[100], text[100], timestamp[20];

PDU myPDU;
void setup()
{
  Serial.begin(115200);
  bool rc = true;
  int length,j;
  if (myPDU.decodePDU(received, sca, timestamp, sender, text))
  {
    Serial.print("SCA ");Serial.println(sca);
    Serial.print("TS ");Serial.println(timestamp);
    Serial.print("Sender ");Serial.println(sender);
    Serial.print("Message ");Serial.println(text);
   }
}

void loop(){}

