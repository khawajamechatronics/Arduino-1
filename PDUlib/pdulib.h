/*
  A library for encoding/decoding PDU strings
  Currently only ASCII 7-bit supported

  encodePDU returns the length of the PDU byte buffer, if any error the length reported as -1
*/
#define BITMASK_7BITS 0x7F

// DCS bit masks
#define DCS_COMPRESSED (5<<1)
#define DCS_CLASS_MEANING (4<<1)
#define DCS_ALPHABET_MASK (3<<2)
#define DCS_ALPHABET_OFFSET 2
#define DCS_7BIT_ALPHABET_MASK (0<<2)
#define DCS_8BIT_ALPHABET_MASK (1<<2)
#define DCS_16BIT_ALPHABET_MASK (2<<2)
#define DCS_CLASS_MASK 3
#define DCS_IMMEDIATE_DISPLAY 3
#define DCS_ME_SPECIFIC_MASK 1
#define DCS_SIM_SPECIFIC_MASK 2
#define DCS_TE_SPECIFIC_MASK 3

 // PDU bits
#define PDU_VALIDITY_MASK_OFFSET 3
#define PDU_VALIDITY_NOT_PRESENT 0
#define PDU_VALIDITY_PRESENT_RELATIVE 2
#define PDU_VALIDITY_PRESENT_ENHANCED 1
#define PDU_VALIDITY_PRESENT_ABSOLUTE 3
#define PSU_SMS_DELIVER 0
#define PSU_SMS_SUBMIT  1


#define INTERNATIONAL_NUMBER 0x91
#define NATIONAL_NUMBER 0x81

#define MAX_SMS_LENGTH_7BIT 160
#define MAX_NUMBER_LENGTH 30

 /* Define Non-Printable Characters as a question mark */
#define NPC7    63
#define NPC8    '?'

enum eDCS { ALPHABET_7BIT, ALPHABET_8BIT, ALPHABET_16BIT };

class PDU
{
public:
  PDU();
  ~PDU();
  // encodePDU creates a BINARY record which should be converted to ASCII for sending to modem
  int encodePDU(uint8_t *pdubuffer, char *recipient, char *message, enum eDCS dcs);
  int encodePDU(uint8_t *pdubuffer, char *recipient, char *message);
  int encodePDU(uint8_t *pdubuffer);  // creates a binary PDU buffer
  // decodePDU works on the ASCII string as  received from the modem
  bool decodePDU(char *pdu, char *SCA, char *ts, char *sender, char *text);
  //  bool setSCA(char *sca);
  bool setRecepient(char *number);
  bool setMessage(char *message);
  bool setCharSet(enum eDCS dcs);
private:
  bool scavalid, recvalid, csvalid, mesvalid;
  int scalength;
  char scabuff[MAX_NUMBER_LENGTH];  // ample for any phone number
  int reclength;
  char recbuff[MAX_NUMBER_LENGTH];  // ample for any phone number
  int meslength;
  char mesbuff[MAX_SMS_LENGTH_7BIT];  // ample for any message
  enum eDCS dcs;
  int pduoutindex;
//  uint8_t charToNibble(char digit, bool upper);
  void PDU::stringToBDC(char *number, uint8_t *pdu);
  int ascii_to_pdu(char *ascii, uint8_t *pdu);
  int convert_ascii_to_7bit(char *ascii, char *a7bit);
  uint8_t gethex(char *pc);
  int pdu_to_ascii(char *pdu, int pdulength, char *ascii);
  int convert_7bit_to_ascii(uint8_t *a7bit, int length, char *ascii);
};

