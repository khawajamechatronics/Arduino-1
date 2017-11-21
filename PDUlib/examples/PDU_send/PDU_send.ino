/*
 *    encode an ASCCI string to PDU for the default 7-bit alphabet
 *    THere are 3 overloaded versions of the encodePDU function.
 *    
 *    encodePDU must have legal parameters of recipient phone number and message.
 *    Only the 7 bit alphabet is supported for now but more is coming
 *    
 *    THe PDU byte array can be conbverted to an ASCII string and pasted into your
 *    modems AT command protocol
  *
  *   Recipient phone number can be in national or international format
 */
#include "pdulib.h"

char *recipient = "0545919886";
char *message = "hello world";
// expected result  0001000C9179525419896800000BE8329BFD06DDDF723699
// expected result 0001000A81505419896800000BE8329BFD06DDDF723619
uint8_t pdubuff[160];

char printbuf[100];
PDU myPDU;

void setup()
{
  Serial.begin(115200);
  bool rc = true;
  int length,j;
  rc &= myPDU.setRecepient(recipient);
  rc &= myPDU.setMessage(message);
  rc &= myPDU.setCharSet(ALPHABET_7BIT);
  if (rc)
  {
    length = myPDU.encodePDU(pdubuff);
    for (j = 0; j < length; j++)
    {
      sprintf(printbuf,"%02X", pdubuff[j]);
      Serial.print(printbuf);
    }
    Serial.println();
  }
  length = myPDU.encodePDU(pdubuff, recipient, message, ALPHABET_7BIT);
  if (length > 0)
  {
    for (j = 0; j < length; j++)
    {
      sprintf(printbuf,"%02X", pdubuff[j]);
      Serial.print(printbuf);
    }
    Serial.println();
  }
  length = myPDU.encodePDU(pdubuff, recipient, message);
  if (length > 0)
  {
    for (j = 0; j < length; j++)
    {
      sprintf(printbuf,"%02X", pdubuff[j]);
      Serial.print(printbuf);
    }
    Serial.println();
  }
}

void loop(){}

