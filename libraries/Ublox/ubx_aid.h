/*
   Structures and defines for UBX_AID class of messages
*/

// message IDs
#define AID_INI 1
#define AID_HUI 2
#define AID_DATA 16
#define AID_ALM 0x30
#define AID_EPH 0x31
#define AID_ALPSRV 0x32
#define AID_AOP 0x33

struct sAID_POLL {
	struct UbxHeader header;
	struct Chskm cs;
};
struct sAID_INI_DATA {
	struct UbxHeader header;
	int32_t ecefXOrLat;
	int32_t ecefYOrLon;
	int32_t ecefZOrAlt;
	uint32_t posAcc;
	uint16_t tmCfg;  // see below
	uint16_t wn;
	uint32_t tow;
	int32_t towns;
	uint32_t tAccMs;
	uint32_t tAccNs;
	int32_t ClockDOrFreq;
	uint32_t clkDAccOrFreqAcc;
	int32_t flags;
	struct Chskm cs;
};

// INI_DATA_tmcfg flags
#define fEdge (1<<2)
#define tml (1<<4)
#define fl (1<<6)

// 3 versions of almanac data, depends on payload length
struct sAID_ALM_DATA_1 {
	struct UbxHeader header;
	uint8_t svid;
	struct Chskm cs;
};
struct sAID_ALM_DATA_8 {
	struct UbxHeader header;
	uint32_t svid;
	uint32_t week;
	struct Chskm cs;
};
struct sAID_ALM_DATA_40 {
	struct UbxHeader header;
	uint32_t svid;
	uint32_t week;
	uint32_t words[8];
	struct Chskm cs;
};
