#pragma pack(1)
struct sUBX_MGA_INI_TIME_UTC_payload {
  uint8_t type = 0x10;
  uint8_t r1 = 0;
  uint8_t ref = 0;
  uint8_t leapsec= -128;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t r2;
  uint32_t ns = 0;
  uint16_t taccs = 0;
  uint16_t r3;
  uint32_t taccns = 0;
};
struct sUBX_MGA_INI_TIME_UTC
{
  uint8_t header0 = 0xb5;
  uint8_t header1 = 0x62;
  uint8_t classx = 0x13;
  uint8_t id = 0x40;
  uint16_t length = sizeof(struct sUBX_MGA_INI_TIME_UTC_payload);
  struct sUBX_MGA_INI_TIME_UTC_payload payload;
  uint8_t cka;
  uint8_t ckb;
} ubxmessage;
#pragma pack(0)

// adapted from ubloxM8 receiver description UBX checksum
#define CK_A Buffer[length-2]
#define CK_B Buffer[length-1]
void UBXchecksum(uint8_t *Buffer,int length)
{
  Serial.println(length);
  CK_A = 0;
  CK_B = 0;
  
  for (int I = 2; I< length-2;I++)
  {
    CK_A = CK_A + Buffer[I];
    CK_B = CK_B + CK_A;
  }
}

void setup() {
  // put your setup code here, to run once:
  uint8_t *cP = (uint8_t *)&ubxmessage;
  Serial.begin(9600);
  ubxmessage.payload.year = 2016;
  ubxmessage.payload.month = 5;
  ubxmessage.payload.day = 2;
  ubxmessage.payload.hour = 18;
  ubxmessage.payload.minute = 20;
  ubxmessage.payload.second = 15;
  UBXchecksum((uint8_t *)&ubxmessage, sizeof(ubxmessage));
  for (int i=0; i<sizeof(struct sUBX_MGA_INI_TIME_UTC);i++)
  {
      Serial.print(*cP++,HEX); Serial.print(' ');
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
