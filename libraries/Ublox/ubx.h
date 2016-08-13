#ifndef UBX_H
#define UBX_H
/*
    General purpose library for servicing UBX data stram
*/
#include "Arduino.h"
#include <inttypes.h>

#define SYNC1 0xb5
#define SYNC2 0x62
//#define UBX_DEBUG
// Parser state machine
enum eState {WAIT_S1,WAIT_S2,WAIT_CLASS,WAIT_ID,WAIT_L1,WAIT_L2,PAYLOAD,WAIT_CS1,WAIT_CS2};

// Message class definitions
#define NAV 1
#define RXM 2
#define INF 4
#define ACK 5
#define CFG 6
#define MON 10
#define AID 11
#define TIM 13
#define MGA 0x13
#define LOG 0x21

class UBX {
	public:
		bool parse(byte);   // true for complete message, will be contained in the work buffer
		void begin(byte *,int);  // initialize with pointer to work buffer
		void Checksum(byte *buffer,int length); // see ublox manual
		// CS is calculated checksum after calling Checksum
		byte CK_A, CK_B;
	private:
		enum eState state;
		byte *workarea;
		int max_length;
		int offset;
		uint16_t payloadlength;
};
// lots of structures. Pack them to be the same as ublox standard
#pragma pack(1)
struct UbxHeader {
	byte sync1;
	byte sync2;
	byte classx;
	byte id;
	uint16_t plength;
};
struct Chskm {
	uint8_t ck_a;
	uint8_t ck_b;
};
#include "ubx_ack.h"
#include "ubx_nav.h"
#include "ubx_aid.h"
#include "ubx_mga.h"
#pragma pack(0)
#endif
