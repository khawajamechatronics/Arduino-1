#include <SPI.h>
const int  cs=10; //chip select 
const int drdy = 3; // alarm
int flag = 0;
int nextsecond = 0;
void setup() {
  Serial.begin(115200);
  RTC_init();
  //day(1-31), month(1-12), year(0-99), hour(0-23), minute(0-59), second(0-59)
  SetTimeDate(31,12,13,23,59,50); 
}

char printbuf[20];

byte BCD(byte val)
{
  int b= val/10;
  int a= val-b*10;
  return a+(b<<4);
}

void loop() {
  if (flag == 1)
  {
    byte stat;
    flag = 0;
    Serial.println("interrupt");
    // clear interrupt
    stat = ReadRegister(15);
    stat &= 0xFE;
    WriteRegister(15,stat);
    nextsecond += 5;
    nextsecond %= 60;
    SetSecondsAlarm(BCD(nextsecond));
 //   EnableInterrupt();
  }
  sprintf(printbuf,"%02X,",ReadRegister(14));
  Serial.print(printbuf);
  sprintf(printbuf,"%02X,",ReadRegister(15));
  Serial.print(printbuf);
  sprintf(printbuf,"%02X,",ReadRegister(7));
  Serial.print(printbuf);
  sprintf(printbuf,"%02X,",ReadRegister(8));
  Serial.print(printbuf);
  sprintf(printbuf,"%02X,",ReadRegister(9));
  Serial.print(printbuf);
  sprintf(printbuf,"%02X,",ReadRegister(10));
  Serial.print(printbuf);
  Serial.println(ReadTimeDate());
  delay(1000);
}

void intfunc()
{
  flag = 1;
}

//=====================================
int RTC_init(){ 
	  pinMode(cs,OUTPUT); // chip select
	  // start the SPI library:
	  SPI.begin();
	  SPI.setBitOrder(MSBFIRST); 
	  SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work 
	  //set control register 
	  digitalWrite(cs, LOW);  
	  SPI.transfer(0x8E);
	  SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
	  digitalWrite(cs, HIGH);
	  delay(10);
          SetSecondsAlarm(0);
          pinMode(drdy,INPUT);
          attachInterrupt(digitalPinToInterrupt(drdy),intfunc, FALLING); // SQK is active LOW
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
		  
		digitalWrite(cs, LOW);
		SPI.transfer(i+0x80); 
		SPI.transfer(TimeDate[i]);        
		digitalWrite(cs, HIGH);
  }
}
//=====================================
String ReadTimeDate(){
	String temp;
	int TimeDate [7]; //second,minute,hour,null,day,month,year		
	for(int i=0; i<=6;i++){
		if(i==3)
			i++;
		digitalWrite(cs, LOW);
		SPI.transfer(i+0x00); 
		unsigned int n = SPI.transfer(0x00);        
		digitalWrite(cs, HIGH);
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
	temp.concat(TimeDate[6]);
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
  digitalWrite(cs, LOW);
  SPI.transfer(reg); // set register
  unsigned int n = SPI.transfer(0x00);        
  digitalWrite(cs, HIGH);
  return n;
}

void WriteRegister(byte reg,byte val)
{
  digitalWrite(cs, LOW);
  SPI.transfer(0x80 | reg);
  SPI.transfer(val);
  digitalWrite(cs, HIGH);
}
void EnableInterrupt()
{
  byte control;
  // enable interrupt, read control reg
  digitalWrite(cs, LOW);
  SPI.transfer(0x0e); 
  control = SPI.transfer(0x00);        
  digitalWrite(cs, HIGH);
  control |= 0x05;// A1IE INTCN
  digitalWrite(cs, LOW);  
  SPI.transfer(0x8E);
  SPI.transfer(control); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
  digitalWrite(cs, HIGH);
}

void SetSecondsAlarm(byte value)
{
  // A1M1 = 0 A1M2,A1M3, A1M4 = 1;
  digitalWrite(cs, LOW);
  SPI.transfer(7+0x80); 
  SPI.transfer(value);        
  digitalWrite(cs, HIGH);
  digitalWrite(cs, LOW);
  SPI.transfer(8+0x80); 
  SPI.transfer(0x80);        
  digitalWrite(cs, HIGH);
  digitalWrite(cs, LOW);
  SPI.transfer(9+0x80); 
  SPI.transfer(0x80);        
  digitalWrite(cs, HIGH);
  digitalWrite(cs, LOW);
  SPI.transfer(10+0x80); 
  SPI.transfer(0x80);        
  digitalWrite(cs, HIGH);
  EnableInterrupt();
}


