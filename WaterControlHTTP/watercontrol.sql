#show databases;
use WaterMeter;
#drop table properties;
#create table properties (id varchar(20) not NULL,TPL integer,rawts integer,serverts TIMESTAMP DEFAULT CURRENT_TIMESTAMP);
#drop table action;
#create table action (id varchar(20) not NULL,action ENUM ('ping','powerup','leak','eepromreport','tapchange','reset',
# 'calibrate') not NULL ,rawts integer,serverts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,moredata varchar(100));
#drop table report2;
#create table report2 (id varchar(20) not NULL,meter1 integer not NULL,meter2 integer not NULL,rawts integer,serverts TIMESTAMP DEFAULT CURRENT_TIMESTAMP);
# 'reset','calibrate') default 'report' ,
#alter table report2 add serial integer;
#show tables;
#create table;
#Create table rawdata (imei char(15) not NULL, action ENUM ('ping','powerup','leak','eepromreport','tapchange','report',
# 'reset','calibrate') default 'report' ,
#tap ENUM ('unknown','tapopen','tapclose') default 'unknown', uptime integer,meterticks integer,rawts varchar(30),liters integer,
#serverts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,imsi char(15), ExternalMeter float, IP char(15));
#show tables;
#Create table EEPROM (imei char(15) ,tpl integer,lis integer, ph varchar(20), 
#  smtps varchar(30),ea varchar(30),pword varchar(20),
#  rcvr varchar(30), apn varchar(30), smsflag boolean,
#  rr integer, pol boolean, plw integer, 
#  https varchar(30), gpto integer, TS TIMESTAMP DEFAULT CURRENT_TIMESTAMP);
#alter table rawdata add IMSI char(15);
#alter table EEPROM add DB varchar(20);
#select * from rawdata order by serverts desc limit 100;
#select * from rawdata where action = 'leak';
#alter table rawdata change IMIE imei char(15);

#drop table rawdata;
#drop table EEPROM;
#insert into EEPROM (ph,smtps,ea,pword,rcvr,apn,https,tpl,lis,rr,plw,smsflag,gpto) values('+972545919886','smtp.mail.yahoo.com','dhdh654321@yahoo.com','Morris59','mgadriver@gmail.com','uinternet','david-henry.dyndns.tv',250,29,600,100,0,15);
#select liters,serverts from rawdata where action = 'report' order by serverts desc limit 10;
#CREATE TEMPORARY TABLE IF NOT EXISTS table2 AS select liters,serverts from rawdata where action = 'report' order by serverts desc limit 10;
#select liters,serverts from  table2 order by serverts;
#select * from rawdata where serverts > '2016-08-25 16:00:00' limit 2000;
#select * from rawdata where action = 'report' order by serverts desc limit 10;
#select * from rawdata where action = 'ping' order by serverts desc;
#alter table rawdata add actionnumber int not null AUTO_INCREMENT primary key;
#select * from rawdata where  TIMESTAMPDIFF(SECOND,serverts,CURRENT_TIMESTAMP) < 10800 and action = 'report' order by serverts desc;
#select * from rawdata order by serverts desc limit 1000;
#select * from rawdata where action='leak' order by serverts desc;
#select TIMESTAMPDIFF(SECOND,'2016-08-26 18:00:00',CURRENT_TIMESTAMP);
#select * from rawdata where serverts > '2016-08-26 15:00:00' and action = 'report' order by serverts desc
#alter table  EEPROM add TS TIMESTAMP DEFAULT CURRENT_TIMESTAMP;
#alter table rawdata add epoch integer;
#alter table rawdata add meter2 integer;
#select * from EEPROM order by TS desc;
#alter table rawdata add ExternalMeter float;
#insert into rawdata (action,ExternalMeter) values('calibrate',759.5057);
#alter table rawdata change action action ENUM ('ping','powerup','leak','eepromreport','tapchange','report','reset','calibrate') default 'report';
#insert into properties (id,TPL,rawts) values('fred',300,1482595092);
#select * from properties;
#insert into action (id,action,rawts) values('fred','ping',1482595092);
#insert into action (id,action,rawts,moredata) values('fred',4,1482595092,'eeprom string');
#select * from action;
#insert into report2 (id,meter1,meter2,rawts) values('fred',1,2,1482595092);
#select * from report2 order by serverts desc limit 20;
#alter table EEPROM add epoch integer;
#alter table EEPROM change imsi imei char(15);
#select * from EEPROM order by TS desc;
select * from rawdata order by serverts desc limit 100;

