#show databases;
use WaterMeter;
#show tables;
#create table;
#Create table rawdata (imei char(15) not NULL, action ENUM ('ping','powerup','leak','eepromreport','tapchange','report','reset')
#default 'report' ,
#tap ENUM ('unknown','tapopen','tapclose') default 'unknown', uptime integer,meterticks integer,rawts varchar(30),liters integer,
#serverts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,imsi char(15));
#show tables;
#Create table EEPROM (imei char(15) ,tpl integer,lis integer, ph varchar(20), 
#  smtps varchar(30),ea varchar(30),pword varchar(20),
#  rcvr varchar(30), apn varchar(30), smsflag boolean,
#  rr integer, pol boolean, plw integer, 
#  https varchar(30), gpto integer, TS TIMESTAMP DEFAULT CURRENT_TIMESTAMP);
#alter table rawdata add IMSI char(15);
#alter table EEPROM add DB varchar(20);
#select * from rawdata order by serverts desc limit 1000;
#select * from rawdata where action = 'leak';
#select * from EEPROM;
#alter table rawdata change IMIE imei char(15);

#drop table rawdata;
#drop table EEPROM;
#insert into EEPROM (ph,smtps,ea,pword,rcvr,apn,https,tpl,lis,rr,plw,smsflag,gpto) values('+972545919886','smtp.mail.yahoo.com','dhdh654321@yahoo.com','Morris59','mgadriver@gmail.com','uinternet','david-henry.dyndns.tv',250,29,600,100,0,15);
#elect liters,serverts from rawdata where action = 'report' order by serverts desc limit 10;
#CREATE TEMPORARY TABLE IF NOT EXISTS table2 AS select liters,serverts from rawdata where action = 'report' order by serverts desc limit 10;
#select liters,serverts from  table2 order by serverts;
#select * from rawdata where serverts > '2016-08-25 16:00:00' limit 2000;
#select * from rawdata where action = 'report' order by serverts desc;
#alter table rawdata add actionnumber int not null AUTO_INCREMENT primary key;
select * from rawdata where  TIMESTAMPDIFF(SECOND,serverts,CURRENT_TIMESTAMP) < 10800 and action = 'report' order by serverts;
#select TIMESTAMPDIFF(SECOND,'2016-08-26 18:00:00',CURRENT_TIMESTAMP);
#select * from rawdata where serverts > '2016-08-26 15:00:00' and action = 'report' order by serverts desc
