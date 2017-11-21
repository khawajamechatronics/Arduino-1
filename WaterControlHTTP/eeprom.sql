use WaterMeter;
#select * from EEPROM order by TS desc;
insert into EEPROM (tpl0,tpl1) values(231,249);
#alter table EEPROM add reboot int;