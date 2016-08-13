#pragma once
//#define MT9D111
#define OV7670
//#define ov7740
//#define haveSDcard
enum RESOLUTION{VGA,QVGA,QQVGA,
#ifdef MT9D111
                                SVGA,XGA,WXGA,QUVGA,SXGA,UXGA,WUXGA,QXGA
#endif
};
enum COLORSPACE{YUV422,
#ifndef ov7740
//The OV7740 does not support RGB565 output.
                      RGB565,
#endif
                            BAYER_RGB};
#ifdef OV7670
#define CAMERA_SLAVE_ADDRESS 0x21 // NOTE datasheet quotes value << 1
#else 
#error Unknown camera
#endif

