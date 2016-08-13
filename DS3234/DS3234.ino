#include <SPI.h>
const int  CS = 10; //chip select 
const int NOKIA_CS = 24;
const int DRDY = 3; // alarm
const int LED = 13;
#define WRITE_MASK 0x80
#define AIM1 0x80
#define AIM2 0x80
#define AIM3 0x80
#define AIM4 0x80
#define A1F 0x01
#define A2F 0x02
#define A1IE 0x01
#define A2IE 0x02
#define INTCN 0x04

int flag = 0;
int nextsecond = 0;
bool ledstate = true;

struct sTime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
} newTime;

char *smstime = "16/07/01,18:50:04+12";

void SMStime(char *ps,struct sTime *pst)
{
  // SMS time in format "02/03/18,09:54:28+40"
  pst->year = atoi(ps);   // atoi will stop in next non-digit
  ps = strchr(ps,'/');
  pst->month = atoi(++ps);
  ps = strchr(ps,'/');
  pst->day = atoi(++ps);
  ps = strchr(ps,',');
  pst->hour = atoi(++ps);
  ps = strchr(ps,':');
  pst->minute = atoi(++ps);
  ps = strchr(ps,':');
  pst->second = atoi(++ps);
}
void setup() {
  Serial.begin(115200);
  pinMode(CS,OUTPUT); // chip select
  digitalWrite(CS,HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST); 
  SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work but inly1 works properly with interrupt
  RTC_init();
  SMStime(smstime,&newTime);
  //day(1-31), month(1-12), year(0-99), hour(0-23), minute(0-59), second(0-59)
  SetTimeDate(newTime.day,newTime.month,newTime.year,newTime.hour,newTime.minute,newTime.second); 
  SetEverySecondAlarm();
  EnableInterrupt();
  pinMode(LED,OUTPUT);
  pinMode(NOKIA_CS,OUTPUT); // CS of Nokia 5100
  digitalWrite(NOKIA_CS,HIGH);  // deselect it
  pinMode(DRDY,INPUT);
  attachInterrupt(digitalPinToInterrupt(DRDY),intfunc, FALLING); // SQK is active LOW
}

char printbuf[20];

byte BCD(byte val)
{
  int b= val/10;
  int a= val-b*10;
  return a+(b<<4);
}

// assume clearing A1F
void ClearInterrupt()
{
    int stat = ReadRegister(15);
    stat &= ~A1F;
    WriteRegister(15,stat);

}
void loop() {
  if (flag == 1)
 // if (1)
  {
    flag = 0;
    if (ledstate)
      digitalWrite(LED,HIGH);
    else
      digitalWrite(LED,LOW);
    ledstate = !ledstate;
    Serial.println(ReadTimeDate());
  // clear interrupt
    ClearInterrupt();
  //  delay(1000);
  }
}

void intfunc()
{
  flag = 1;
}

//=====================================
int RTC_init(){ 
	  // start the SPI library:
	  //set control register 
	  digitalWrite(CS, LOW);  
	  SPI.transfer(0x8E);
	  SPI.transfer(0x60);
	  digitalWrite(CS, HIGH);
//	  delay(10);
}
//=====================================
int SetTimeDate(int d, int mo, int y, int h, int mi, int s){ 
	int TimeDate [7]={s,mi,h,0,d,mo,y};
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
		  
		digitalWrite(CS, LOW);
		SPI.transfer(i+WRITE_MASK); 
		SPI.transfer(TimeDate[i]);        
		digitalWrite(CS, HIGH);
  }
}
//=====================================
String ReadTimeDate(){
	String temp;
	int TimeDate [7]; //second,minute,hour,null,day,month,year		
	for(int i=0; i<=6;i++){
		if(i==3)
			i++;
		digitalWrite(CS, LOW);
		SPI.transfer(i+0x00); 
		unsigned int n = SPI.transfer(0x00);        
		digitalWrite(CS, HIGH);
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
	temp.concat("     ") ;
	temp.concat(TimeDate[2]);
	temp.concat(":") ;
	temp.concat(TimeDate[1]);
	temp.concat(":") ;
	temp.concat(TimeDate[0]);
  return(temp);
}

int ReadRegister(int reg)
{
  digitalWrite(CS, LOW);
  SPI.transfer(reg); // set register
  unsigned int n = SPI.transfer(0x00);        
  digitalWrite(CS, HIGH);
  return n;
}

void WriteRegister(byte reg,byte val)
{
  digitalWrite(CS, LOW);
  SPI.transfer(WRITE_MASK | reg);
  SPI.transfer(val);
  digitalWrite(CS, HIGH);
}
void EnableInterrupt()
{
  byte control;
  // enable interrupt, read control reg
  digitalWrite(CS, LOW);
  SPI.transfer(0x0e); 
  control = SPI.transfer(0x00);        
  control |= (A1IE | INTCN);
  SPI.transfer(0x0E | WRITE_MASK);
  SPI.transfer(control); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
  digitalWrite(CS, HIGH);
}

void SetEverySecondAlarm()
{
  // A1M1,A1M2,A1M3, A1M4 = 1;
  digitalWrite(CS, LOW);
  SPI.transfer(7+WRITE_MASK); 
  SPI.transfer(AIM1);        
  SPI.transfer(8+WRITE_MASK); 
  SPI.transfer(AIM2);        
  SPI.transfer(9+WRITE_MASK); 
  SPI.transfer(AIM3);        
  SPI.transfer(10+WRITE_MASK); 
  SPI.transfer(AIM4);        
  digitalWrite(CS, HIGH);
}
void SetSecondsAlarm(byte value)
{
  // A1M1 = 0 A1M2,A1M3, A1M4 = 1;
  digitalWrite(CS, LOW);
  SPI.transfer(7|WRITE_MASK); 
  SPI.transfer(value);        
  digitalWrite(CS, HIGH);
  digitalWrite(CS, LOW);
  SPI.transfer(8|WRITE_MASK); 
  SPI.transfer(0x80);        
  digitalWrite(CS, HIGH);
  digitalWrite(CS, LOW);
  SPI.transfer(9|WRITE_MASK); 
  SPI.transfer(0x80);        
  digitalWrite(CS, HIGH);
  digitalWrite(CS, LOW);
  SPI.transfer(10|WRITE_MASK); 
  SPI.transfer(0x80);        
  digitalWrite(CS, HIGH);
}


