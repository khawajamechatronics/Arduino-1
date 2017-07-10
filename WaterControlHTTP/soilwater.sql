use WaterMeter;
#select * from SoilWater order by TS desc;
#select ID,TS,ADC from SoilWater where TIMESTAMPDIFF(SECOND,TS,CURRENT_TIMESTAMP) < 7200 and ID=99 order by TS desc;
select * from SoilWater order by TS desc;
#alter table SoilWater add valid bool default true;
#update SoilWater set valid = true where ADC > 80;