#!/usr/bin/python
# t5403 barometer
from smbus import SMBus
from time import sleep

slaveAddress = 0x77
#registers
T5403_COMMAND_REG = 0xf1
T5403_DATA_REG_LSB = 0xf5
T5403_DATA_REG_MSB = 0xf6
#commands
COMMAND_GET_TEMP = 0x03
#definitions for pressure reading commands with accuracy modes
MODE_LOW = 0x00
MODE_STANDARD = 0x01
MODE_HIGH = 0x10
MODE_ULTRA = 0x11
b = SMBus(1)
global c1,c2,c3,c4,c5,c6,c7,c8
def uint16Toint16(data):
        if data > 32767:
                return data - 0x10000
        else:
                return data
def sendCommand(cmd):
        b.write_byte_data(slaveAddress,T5403_COMMAND_REG,cmd)
def getData():
        return (b.read_byte_data(slaveAddress,T5403_DATA_REG_MSB)<<8) + b.read_byte_data(slaveAddress,T5403_DATA_REG_LSB)
def begin():
        global c1,c2,c3,c4,c5,c6,c7,c8
        c1 = (b.read_byte_data(slaveAddress,0x8f)<<8) + b.read_byte_data(slaveAddress,0x8e)
        c2 = (b.read_byte_data(slaveAddress,0x91)<<8) + b.read_byte_data(slaveAddress,0x90)
        c3 = (b.read_byte_data(slaveAddress,0x93)<<8) + b.read_byte_data(slaveAddress,0x92)
        c4 = (b.read_byte_data(slaveAddress,0x95)<<8) + b.read_byte_data(slaveAddress,0x94)
        c5 = (b.read_byte_data(slaveAddress,0x97)<<8) + b.read_byte_data(slaveAddress,0x96)
        c6 = (b.read_byte_data(slaveAddress,0x99)<<8) + b.read_byte_data(slaveAddress,0x98)
        c7 = (b.read_byte_data(slaveAddress,0x9b)<<8) + b.read_byte_data(slaveAddress,0x9a)
        c8 = (b.read_byte_data(slaveAddress,0x9d)<<8) + b.read_byte_data(slaveAddress,0x9c)
        c5 = uint16Toint16(c5)
        c6 = uint16Toint16(c6)
        c7 = uint16Toint16(c7)
        c8 = uint16Toint16(c8)
def getTemperature():
        sendCommand(COMMAND_GET_TEMP)
        sleep(0.006)
        temp_raw = uint16Toint16(getData())
        temp_actual = (((( c1 * temp_raw)/ 0x100) + ( c2 * 0x40)) * 100) / 0x10000;
        return float(temp_actual)/100.0
def getPressure(precision):
        sendCommand(COMMAND_GET_TEMP)
        sleep(0.006)
        temp_raw = uint16Toint16(getData())
        #Load measurement noise level into command along with start command bit.
        precision = (precision << 3)|(0x01)
        # Start pressure measurement
        sendCommand(precision)
        if precision == MODE_LOW:
                sleep(0.005)
        elif precision == MODE_STANDARD:
                sleep(0.011)
        elif precision == MODE_HIGH:
                sleep(0.019)
        elif precision == MODE_ULTRA:
                sleep(0.067)
        else:
                sleep(0.1)
        pressure_raw = getData()
        # calculate pressure
        s = (((( c5 * temp_raw) >> 15) * temp_raw) >> 19) + c3 + (( c4 * temp_raw) >> 17);
        o = (((( c8 * temp_raw) >> 15) * temp_raw) >> 4) + (( c7 * temp_raw) >> 3) + (c6 * 0x4000);
        return (s * pressure_raw + o) >> 14
# off we go
begin()
print 'temperature=',getTemperature()
print 'pressure=',getPressure(MODE_ULTRA)


pi@raspberrypi ~/t5403 $ sudo ./t5302.py
temperature= 20.99
pressure= 100983
pi@raspberrypi ~/t5403 $
