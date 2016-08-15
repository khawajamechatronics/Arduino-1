/*
  Add to this structure as more types needed
  
  NOTE all uint16_t fields are BIG ENDIAN
*/
#define ECHO_REQUEST 8
#define ECHO_REPLY 0
#define INFORMATION_REQUEST 15
#define INFORMATION_REPLY 16

#define DATASIZE 32

#pragma pack(1)
struct sICMP {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  union uRemainder {
    struct sEcho {  // for type 0 & 8
      uint16_t id;
      uint16_t seq;
    } echo;
  }remainder;
};

struct eEchoReply {
  struct sICMP header;
  uint8_t data[DATASIZE];
};
struct sIPHeader {
  byte ip[4];
  uint16_t length;
};
#pragma pack()
