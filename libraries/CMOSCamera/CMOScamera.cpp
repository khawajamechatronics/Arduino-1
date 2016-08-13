/*
 *  Generic code for CMOS camera
 *  Tested so far on OV7670
 */

extern "C" {
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "CCC_twi.h"
}
#include "CMOScamera.h"

// Constructors ////////////////////////////////////////////////////////////////

CMOSCamera::CMOSCamera()
{
  slaveAddress = CAMERA_SLAVE_ADDRESS;
}
// public functions

void CMOSCamera::begin()
{
    // could setup registers here 
    twi_init(); 
}
/*
 *  Read preset ID values and check against defined defaults
 *  You need to read each datasheet for these values
 */
bool CMOSCamera::WhoAmI()
{
	bool rc = false;
#ifdef OV7670
  uint8_t data[2];
  rc = readRegs(REG_PID,data,2);
  if (rc)
  {
    rc = (data[0] == PID_VALUE && data[1] == VER_VALUE);
  }
  return rc;
#else
#error Camera not supported
#endif
}

/*
 *  In library cpp code we cannot use Arduino libraries such as Wire
 *  Instead use the underlying Atmel TWI library
 */
bool CMOSCamera::readRegs(int reg, uint8_t *buffer, int num)
{
	uint8_t actualRX;
	twi_writeToReadFrom(slaveAddress, (uint8_t*)&reg,1, buffer, num, &actualRX);
	return (actualRX == num);
}

#ifdef OV7670
void CMOSCamera::initCam(enum COLORSPACE bayerUse)	// sets up basic registers
{
	wrReg(REG_COM7, COM7_RESET);
	_delay_ms(100);  // iplementation guide says wait 1ms
	if(bayerUse==BAYER_RGB){
		wrSensorRegs8_8(OV7670_QVGA);
	}
	else if(bayerUse==RGB565){
		uint16_t n;
		for(n = 0; n < sizeof(reg_init_data);n+=2)
			wrReg(pgm_read_byte_near(reg_init_data+n), pgm_read_byte_near(reg_init_data+n+1));
	}
	else
		wrSensorRegs8_8(ov7670_default_regs);
	if(bayerUse!=BAYER_RGB)
		wrReg(REG_MVFP,rdReg(REG_MVFP)|MVFP_MIRROR);//hflip
	if(bayerUse==RGB565)
		wrReg(REG_COM10,COM10_PCLK_HB |COM10_PCLK_REV);  // 0x30
	else
		wrReg(REG_COM10,COM10_PCLK_HB|COM10_PCLK_REV);// 0x20 pclk does not toggle on HBLANK
	wrReg(REG_COM11,COM11_NMFR|COM11_EXP); // 0x62	
}
#else
void CMOSCamera::initCam(void)	// sets up basic registers
{
}
#endif
void CMOSCamera::wrSensorRegs8_8(const struct regval_list reglist[]){
	uint8_t reg_addr,reg_val;
	const struct regval_list *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff)){
		reg_addr = pgm_read_byte(&next->reg_num);
		reg_val = pgm_read_byte(&next->value);
		wrReg(reg_addr, reg_val);
	   	next++;
	}
}
uint8_t CMOSCamera::rdReg(uint8_t reg)
{
	return twi_readSingle(slaveAddress,reg);
}
uint8_t CMOSCamera::wrReg(uint8_t reg,uint8_t value)
{
	return twi_writeSingle(slaveAddress,reg, value);
}

void CMOSCamera::setColor(enum COLORSPACE color){
	#ifdef MT9D111
		wrReg16(0xF0,1);
	#endif
	switch(color){
		case YUV422:
			#ifdef MT9D111
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|125);
				wrReg16(0xC8,0);
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|126);
				wrReg16(0xC8,0);
			#else
				wrSensorRegs8_8(yuv422_ov7670);
			#endif
		break;
		case RGB565:
			#ifdef MT9D111
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|125);
				wrReg16(0xC8,(1<<5));
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|126);
				wrReg16(0xC8,(1<<5));
			#else
				wrSensorRegs8_8(rgb565_ov7670);
				{uint8_t temp=rdReg(REG_CLKRC);
				_delay_ms(1);
				wrReg(REG_CLKRC,temp);}//According to the Linux kernel driver RGB565 PCLK needs rewriting.
			#endif
		break;
		#ifndef MT9D111
		case BAYER_RGB:
			wrSensorRegs8_8(bayerRGB_ov7670);
		break;
		#endif
	}
}
void CMOSCamera::setRes(enum RESOLUTION res)
{
	switch(res){
		#ifdef MT9D111
		case SVGA:
			setMT9D111res(800,600);
		break;
		#endif
		case VGA:
			//wrReg(REG_CLKRC,2);//divider
			#ifdef ov7740
				scalingToggle(0);
			#elif defined MT9D111
				//wrSensorRegs8_16(MT9D111_VGA);
				setMT9D111res(640,480);
			#else
				wrReg(REG_COM3,0);	// REG_COM3
				wrSensorRegs8_8(vga_ov7670);
			#endif
		break;
		case QVGA:
			#ifdef ov7740
				scalingToggle(1);
			#elif defined MT9D111
				//wrSensorRegs8_16(MT9D111_QVGA);
				setMT9D111res(320,240);
			#else
				wrReg(REG_CLKRC,1);//divider
				wrReg(REG_COM3,COM3_DCWEN);	// REG_COM3 enable scaling
				wrSensorRegs8_8(qvga_ov7670);
			#endif
		break;
		case QQVGA:
			#ifdef ov7740
				scalingToggle(1);
			#elif defined MT9D111
				setMT9D111res(160,120);
			#else
				wrReg(REG_CLKRC,0);
				wrReg(REG_COM3,COM3_DCWEN);	// REG_COM3 enable scaling
				wrSensorRegs8_8(qqvga_ov7670);
			#endif
	}	
}