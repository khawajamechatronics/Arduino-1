#include "Configuration.h"

struct ts {
    uint8_t sec;         /* seconds */
    uint8_t min;         /* minutes */
    uint8_t hour;        /* hours */
    uint8_t mday;        /* day of the month */
    uint8_t mon;         /* month */
    int year;            /* year */
    uint8_t wday;        /* day of the week */
    uint8_t yday;        /* day in the year */
    uint8_t isdst;       /* daylight saving time */
    uint8_t year_s;      /* year in short notation*/
    uint32_t unixtime;   /* seconds since 01.01.1970 00:00:00 UTC*/
};

// bit masks
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

#define SECONDS_FROM_1970_TO_2000 946684800

// prototypes
void RS3234Init();
void SetTimeDate(int,int,int,int,int,int);
void SetEverySecondAlarm();
void ClearAlarm();
String ReadTimeDate();
unsigned long GetUnixTime();
void DS3234_get(struct ts *t);  // gets time from DS3234
void DS3234_SMS_get(char *ps,struct ts *t);  // gets time from SMS timestamp (see SIM900 +CMGR)
