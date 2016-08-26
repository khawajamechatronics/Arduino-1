#include "Configuration.h"
#ifdef USE_DS3234
#include <avr/pgmspace.h>
#include "Arduino.h"
#include <SPI.h>
#include "DS3234.h"
#include "fastio.h"

void RS3234Init()
{
  //set control register 
  digitalWrite(DS3234_CS, LOW);  
  SPI.transfer(0x8E);
  SPI.transfer(0x60);
  digitalWrite(DS3234_CS, HIGH);
}

static byte BCD(byte val)
{
  int b= val/10;
  int a= val-b*10;
  return a+(b<<4);
}

static int ReadRegister(int reg)
{
  digitalWrite(DS3234_CS, LOW);
  SPI.transfer(reg); // set register
  unsigned int n = SPI.transfer(0x00);        
  digitalWrite(DS3234_CS, HIGH);
  return n;
}

static void WriteRegister(byte reg,byte val)
{
  digitalWrite(DS3234_CS, LOW);
  SPI.transfer(WRITE_MASK | reg);
  SPI.transfer(val);
  digitalWrite(DS3234_CS, HIGH);
}

//=====================================
void SetTimeDate(int d, int mo, int y, int h, int mi, int s){ 
  int TimeDate [7]={s,mi,h,0,d,mo,y};
  TimeDate[6] -= 1900;   // correct current year to relative from 1900
  for(int i=0; i<=6;i++){
    if(i==3)
      i++;
    int b= TimeDate[i]/10;
    int a= TimeDate[i]-b*10;
    if(i==2){
      if (b==2)
        b=B00000010;
      else if (b==1)
        b=B00000001;
    } 
    TimeDate[i]= a+(b<<4);
      
    digitalWrite(DS3234_CS, LOW);
    SPI.transfer(i+WRITE_MASK); 
    SPI.transfer(TimeDate[i]);        
    digitalWrite(DS3234_CS, HIGH);
  }
}

void SetEverySecondAlarm()
{
  // A1M1,A1M2,A1M3, A1M4 = 1;
  digitalWrite(DS3234_CS, LOW);
  SPI.transfer(7+WRITE_MASK); 
  SPI.transfer(AIM1);        
  SPI.transfer(8+WRITE_MASK); 
  SPI.transfer(AIM2);        
  SPI.transfer(9+WRITE_MASK); 
  SPI.transfer(AIM3);        
  SPI.transfer(10+WRITE_MASK); 
  SPI.transfer(AIM4);        
  digitalWrite(DS3234_CS, HIGH);  
}

//=====================================
String ReadTimeDate(){
  String temp;
  int TimeDate [7]; //second,minute,hour,null,day,month,year    
  for(int i=0; i<=6;i++)
  {
    if(i==3)
      i++;
    digitalWrite(DS3234_CS, LOW);
    SPI.transfer(i+0x00); 
    unsigned int n = SPI.transfer(0x00);        
    digitalWrite(DS3234_CS, HIGH);
    int a=n & B00001111;    
    if(i==2){ 
      int b=(n & B00110000)>>4; //24 hour mode
      if(b==B00000010)
        b=20;        
      else if(b==B00000001)
        b=10;
      TimeDate[i]=a+b;
    }
    else if(i==4){
      int b=(n & B00110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==5){
      int b=(n & B00010000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==6){
      int b=(n & B11110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else{ 
      int b=(n & B01110000)>>4;
      TimeDate[i]=a+b*10; 
      }
  }
  temp.concat(TimeDate[4]);
  temp.concat("/") ;
  temp.concat(TimeDate[5]);
  temp.concat("/") ;
  temp.concat(TimeDate[6]+2000);
  temp.concat("\r\n") ;
  temp.concat(TimeDate[2]);
  temp.concat(":") ;
  temp.concat(TimeDate[1]);
  temp.concat(":") ;
  temp.concat(TimeDate[0]);
  return(temp);
}

void ClearAlarm()
{
    int stat = ReadRegister(15);
    stat &= ~A1F;
    WriteRegister(15,stat);

}

const uint8_t days_in_month [12] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// returns the number of seconds since 01.01.1970 00:00:00 UTC, valid for 2000..FIXME
uint32_t get_unixtime(struct ts t)
{
    uint8_t i;
    uint16_t d;
    int16_t y;
    uint32_t rv;

    if (t.year >= 2000) {
        y = t.year - 2000;
    } else {
        return 0;
    }

    d = t.mday - 1;
    for (i=1; i<t.mon; i++) {
        d += pgm_read_byte(days_in_month + i - 1);
    }
    if (t.mon > 2 && y % 4 == 0) {
        d++;
    }
    // count leap days
    d += (365 * y + (y + 3) / 4);
    rv = ((d * 24UL + t.hour) * 60 + t.min) * 60 + t.sec + SECONDS_FROM_1970_TO_2000;
    return rv;
}

void DS3234_SMS_get(char *ps, struct ts *t)  // gets time from SMS timestamp (see SIM900 +CMGR)
{
	// "16/07/01,18:50:04+12";
	// atoi function stops on next non-digit
	memset((void *)t,0,sizeof(struct ts));
	t->year = atoi(ps)+2000;   // actual year
	ps = strchr(ps,'/');
	t->mon = atoi(++ps);
	ps = strchr(ps,'/');
	t->mday = atoi(++ps);
	ps = strchr(ps,',');
	t->hour = atoi(++ps);
	ps = strchr(ps,':');
	t->min = atoi(++ps);
	ps = strchr(ps,':');
	t->sec = atoi(++ps);
	t->unixtime = get_unixtime(*t);
}

void DS3234_get(struct ts *t)
{
    uint8_t TimeDate[7];        //second,minute,hour,dow,day,month,year
    uint8_t century = 0;
    uint8_t i, n;
    uint16_t year_full;

  for(int i=0; i<=6;i++)
  {
    if(i==3)
      i++;
    digitalWrite(DS3234_CS, LOW);
    SPI.transfer(i+0x00); 
    unsigned int n = SPI.transfer(0x00);        
    digitalWrite(DS3234_CS, HIGH);
    int a=n & B00001111;    
    if(i==2)
    { 
      int b=(n & B00110000)>>4; //24 hour mode
      if(b==B00000010)
        b=20;        
      else if(b==B00000001)
        b=10;
      TimeDate[i]=a+b;
    }
    else if(i==4)
    {
      int b=(n & B00110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else if(i==5)
    {
      int b=(n & B00010000)>>4;
      TimeDate[i]=a+b*10;
      century = (n & 0x80) >> 7;
    }
    else if(i==6)
    {
      int b=(n & B11110000)>>4;
      TimeDate[i]=a+b*10;
    }
    else
    { 
      int b=(n & B01110000)>>4;
      TimeDate[i]=a+b*10; 
    }
  }

    if (century == 1)
        year_full = 2000 + TimeDate[6];
    else
        year_full = 1900 + TimeDate[6];

    t->sec = TimeDate[0];
    t->min = TimeDate[1];
    t->hour = TimeDate[2];
    t->mday = TimeDate[4];
    t->mon = TimeDate[5];
    t->year = year_full;
    t->wday = TimeDate[3];
    t->year_s = TimeDate[6];
    t->unixtime = get_unixtime(*t);
}
#endif


