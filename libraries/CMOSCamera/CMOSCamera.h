/*
 * Generic code for CMOS camera
 * Tested on OV7670 so far
 */
#ifndef _CMOS_CAMERA_H_
#define _CMOS_CAMERA_H_
#include <inttypes.h>
#include "config.h"
#include "CamRegDef.h"
#include "OV7670_regs.h"

// I2C error codes
#define TWI_SUCCESS 0
#define TWI_ADDR_NAK 1
#define TWI_DATA_NAK 2
#define TWI_OTHER 3

class CMOSCamera
{
    private:
      uint8_t slaveAddress;
	  void wrSensorRegs8_8(const struct regval_list reglist[]);
	  uint8_t rdReg(uint8_t);
//	  void wrReg(uint8_t,uint8_t);
    public:
      CMOSCamera();  // constructor
      void begin(void);  // call this in setup
      bool WhoAmI(void);  // sanity check on ID registers (if available);
      bool readRegs(int reg, uint8_t *buffer, int num);
	  uint8_t wrReg(uint8_t,uint8_t);
 //     bool readRegs(int reg, uint8_t *buffer, int num);
	  void initCam(void);	// sets up basic registers
#ifdef OV7670
	  void initCam(enum COLORSPACE);	// sets up basic registers
#else
#error Undefined camera
#endif
	  void setColor(enum COLORSPACE color);
	  void setRes(enum RESOLUTION res);
};
#endif
