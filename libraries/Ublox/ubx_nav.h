/*
   Structures and Defines for UBX_NAV class of messages
*/
// Message IDs
#define NAV_POSLLH 2
#define NAV_STATUS 3
#define NAV_TIMEUTC 0x21
#define NAV_AOPSTATUS 0x60

struct sUBX_NAV_SOL {
	struct UbxHeader header;
	uint32_t iTOW;
	int32_t fTOW;
	int16_t week;
	uint8_t gpsFix;
	uint8_t flags;  // see below for masks
	int32_t ecefX;
	int32_t ecefY;
	int32_t ecefZ;
	uint32_t pAcc;
	int32_t ecefVX;
	int32_t ecefVY;
	int32_t ecefVZ;
	uint32_t sAcc;
	uint16_t pDOP;
	uint8_t rsv1;
	uint8_t numSV;
	uint8_t rsv2[4];
	struct Chskm cs;
};
// NAV_SOL flags masks
#define GPSfixOK 0x01
#define DiffSoln 0x02
#define WKNSET 0x04
#define TOWSET 0x08

struct sUBX_NAV_STATUS {
	struct UbxHeader header;
	uint32_t iTOW;
	uint8_t gpsFix;
	uint8_t flags;  // see below for masks
	uint8_t fixStat;  // see below for masks
	uint8_t flags2;
	uint32_t ttff;
	uint32_t mmss;
	struct Chskm cs;
};
// NAV_STATUS flags masks
#define dgpsIStat 0x01
#define mapMatching 0xC0
// NAV_STATUS fixstat masks
#define psmState 0x03

struct sUBX_NAV_TIMEUTC {
	struct UbxHeader header;
	uint32_t iTOW;
	uint32_t tAcc;
	int32_t nano;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t valid;
	struct Chskm cs;
};
// NAV_TIMEUTC_valid masks
#define validTOW 0x01
#define validWKN 0x02
#define validUTC 0x04

struct sUBX_NAV_POSLLH {
	struct UbxHeader header;
	uint32_t iTOW;
	int32_t longitude;
	int32_t latitude;
	int32_t height;
	int32_t hMSL;
	uint32_t hAcc;
	uint32_t vAcc;
	struct Chskm cs;
};
struct sUBX_NAV_AOP_STATUS {
	struct UbxHeader header;
	uint32_t iTOW;
	uint8_t aopCfg;
	uint8_t status;
	uint8_t rsv[10];
	struct Chskm cs;
};
