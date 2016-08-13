/*
   Structures and defines for UBX_MGA class of messages
*/

// message IDs
#define MGA_INI 0x40
#define MGA_ACK 0x60

struct sMGA_INI_TIME_UTC {
	struct UbxHeader header;
	uint8_t type;
	uint8_t rsv1;
	uint8_t ref;
	int8_t leapSecs;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t rsv2;
	uint32_t ns;
	uint16_t tAccS;
	uint8_t rev3[2];
	uint32_t tAccns;
	struct Chskm cs;
};
struct sMGA_INI_ACK {
	struct UbxHeader header;
	uint8_t type;
	uint8_t version;
	uint8_t errorCode;
	uint8_t msgId;
	uint8_t msgPayloadStart[4];
	struct Chskm cs;
};
