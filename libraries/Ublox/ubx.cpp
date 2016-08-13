#include <ubx.h>

void UBX::begin(byte *wa,int l)
{
	workarea = wa;
	max_length = l;
	state = WAIT_S1;
	offset = 0;
}

void UBX::Checksum(byte *buf, int length)
{
#ifdef UBX_DEBUG
	char cc[30];
#endif
	CK_A = 0;
	CK_B = 0;
	for (int i=0;i<length;i++)
	{
		CK_A = CK_A + buf[i];
		CK_B = CK_A + CK_B;
	}
}
/*
   if space, add to buffer & parse,
   if at end and correct checksum, return true;
*/
bool UBX::parse(byte b)
{
	bool rc = false;
#ifdef UBX_DEBUG
	char cc[30];
	sprintf(cc,"o %d b %02x s %d\n",offset,b,state);
	Serial.print(cc);
#endif
	// if space available, handle it, else start over
	if (offset >= max_length)
	{
		offset = 0;
		state = WAIT_S1;
	}
	else
	{
		switch(state)
		{
			case WAIT_S1:
				if (b == 0xb5)
				{
					offset = 0;
					workarea[offset++] = 0xb5;
					state = WAIT_S2;
				}
				break;
			case WAIT_S2:
				if (b == 0x62)
				{
					workarea[offset++] = 0x62;
					state = WAIT_CLASS;
				}
				else
					state = WAIT_S1;
				break;
			case WAIT_CLASS:
				workarea[offset++] = b;
				state = WAIT_ID;
				break;
			case WAIT_ID:
				workarea[offset++] = b;
				state = WAIT_L1;
				break;
			case WAIT_L1:
				workarea[offset++] = b;
				payloadlength = b;
				state = WAIT_L2;
				break;
			case WAIT_L2:
				workarea[offset++] = b;
				payloadlength |= b<<8;
				// sanity check that message can fit into this buffer
				if ((payloadlength+8) <= max_length)
					state = PAYLOAD;
				else
				{
					state = WAIT_S1;
					offset = 0;
				}
#ifdef UBX_DEBUG
				Serial.print("payload ");Serial.println(payloadlength);
#endif
				break;
			case PAYLOAD:
				// add to payload until payload exhausted
				workarea[offset++] = b;
				payloadlength--;
				if (payloadlength == 0)
					state = WAIT_CS1;
				break;
			case WAIT_CS1:
				workarea[offset++] = b;
				state = WAIT_CS2;
				break;
			case WAIT_CS2:
				workarea[offset++] = b;
				state = WAIT_S1;
				// complete message, now do checksum
				Checksum(&workarea[2],offset-4);
				rc = (workarea[offset-2] == CK_A && workarea[offset-1] == CK_B);
				break;		
		}
	}
	return rc;
}