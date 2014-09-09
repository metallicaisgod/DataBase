// CRC32 - Cyclic Redundancy Checksum
//
#include "stdafx.h"
#include "crc32.h"

namespace CDICommon
{

// Polinomal:
// x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
// 1110 1101 1011 1000 1000 0011 0010 0000  
#define CRCPOLY1		0x10008110
#define CRCPOLY2		0x10A80100
#define CRCPOLY3		0xCD100110
//#define CRCPOLY		0xEDB88320
unsigned long	g_crc_table[256];
//
void key17_create_table(void)
{
	int				i, j;
	unsigned long	t;
	int CRCPOLY=0;
	CRCPOLY+=CRCPOLY1;
	CRCPOLY+=CRCPOLY2+CRCPOLY3;

	for (i = 0; i < 256; i++)
	{
		t = i;

		for (j = 8; j > 0; j--)
		{
			if (t & 1)
				t = (t >> 1) ^ CRCPOLY;
			else
				t >>= 1;
		}
		g_crc_table[i] = t;
	}
}


#define DO1(buf)	crc = g_crc_table[(crc ^ (*buf++)) & 0xFF] ^ (crc >> 8);
#define DO2(buf)	DO1(buf); DO1(buf);
#define DO4(buf)	DO2(buf); DO2(buf);
#define DO8(buf)	DO4(buf); DO4(buf);

bool g_Create=true;
// ------------------------------------------------------------
// calculate CRC32
//
void key17_calc(unsigned char *sz, unsigned long len, unsigned long *crc32)
{
    unsigned long	crc = *crc32;
	unsigned char	*t = sz;

	if(g_Create)
	{
		key17_create_table();
		g_Create=false;
	}

	while (len >= 8)
	{
		DO8(t);
		len -= 8;
	}
	while (len--)
		DO1(t);

	*crc32 = crc;
}



// ------------------------------------------------------------
// calculate CRC32 of the string
//
unsigned long key17_str(char *sz, bool bAddKey)
{
	unsigned long crc = 0xFFFFFFFF;

	key17_calc((unsigned char *) sz, (unsigned long)strlen(sz), &crc);
	if(bAddKey)
		return (~crc)^0x12421380;
		//return (~crc)^0x18121380;

	else
		return ~crc;
}

} //end of namespace CDICommon