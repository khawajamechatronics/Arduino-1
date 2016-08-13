/*************************************************** 
  CC3000 Low Power Datalogging
  
  Example 1: No Low Power Optimizations
  
  Created by Tony DiCola (tony@tonydicola.com)

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Based on CC3000 examples written by Limor Fried & Kevin Townsend 
  for Adafruit Industries and released under a BSD license.
  All text above must be included in any redistribution.
  
 ****************************************************/
#define DUMMY_DATA
#define HAVE_GPS_SERIAL  // use Serial for debugger, GPS_SERIAL for GPS
#define GPS_SERIAL Serial2

const char *xmlformat1="<C I=\"%u\",D=\"%u\",E=\"%c\",T=\"%s\"";  // only needs to be done once
const char *xmlformat2=",L=\"%u\" />";  // do this for each block
char *pXML;

unsigned ID=237,DD=23;
const char Endian='T';  // text, neither big nor little
const char *T="NMEA";
char xmlheader[80];
unsigned xmlfixedheaderlength;
byte buff[64];  // size of Serial buffer;


void setup(void)
{  
  xmlfixedheaderlength = sprintf(xmlheader,xmlformat1,ID,DD,Endian,T);
  pXML = &xmlheader[xmlfixedheaderlength];
  GPS_SERIAL.begin(38400);  // gps
  Serial.begin(38400); // gps
}

void loop(void)
{
  int count,ll;
  if (GPS_SERIAL.available() > 0)
  {
    count = GPS_SERIAL.readBytes(buff,sizeof(buff));
    ll = sprintf(pXML,xmlformat2,count);
    ll += xmlfixedheaderlength;  // total length of XML
    Serial.write(xmlheader,ll);
    Serial.write(buff,count);
  }
}

