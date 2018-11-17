#include "CRC_32.h"

#define POLYNOMIAL_VALUE			0x04c11db7

static unsigned int crc32_table[256];
static int crc32_uninitialized = 1;

void init_crc32_table()
{
	int i, j;
	unsigned int nData;

	unsigned int flag, flag1, flag2;

	// 256 个值
	for (i = 0; i <= 0xFF; i++)
	{
		nData = (unsigned int)(i << 24);
		crc32_table[i] = 0x00000000;

		for (j = 0; j < 8; j++)
		{
			flag1 = (nData & 0x80000000);
			flag2 = crc32_table[i] & 0x80000000;
			flag = flag1 ^ flag2;

			crc32_table[i] <<= 1;

			if (flag)
			{
				crc32_table[i] ^= POLYNOMIAL_VALUE;
			}

			nData <<= 1;
		}
	}
}


int Decode_CRC_32(unsigned char* buf, int length)
{
	unsigned int oldcrc32;
	unsigned int crc32;
	unsigned int oldcrc;
	int	 t;
	int	 c;
	int  i;

	if (crc32_uninitialized)
	{
		init_crc32_table();
		crc32_uninitialized = 0;
	}

	oldcrc32 = 0xffffffff;				 //初值为0xffffffff
	
	for (i = 0; i < length; i++)
	{
		c = buf[i];						 //新移进来的字节值
		t= (oldcrc32 >> 24) ^ c;				 //要移出的字节的值
		oldcrc = crc32_table[t];		 //根据移出的字节的值查表
		oldcrc32 <<= 8;
		oldcrc32 ^= oldcrc;		 //将寄存器与查出的值进行xor 运算
	}

	crc32 = oldcrc32;

	return crc32;
}

int Encode_CRC_32(unsigned char* buf, int length)
{
	unsigned int oldcrc32;
	unsigned int crc32;
	unsigned int oldcrc;
	int	 t;
	int	 c;
	int  i;

	if (crc32_uninitialized)
	{
		init_crc32_table();
		crc32_uninitialized = 0;
	}

	oldcrc32 = 0xffffffff;				 //初值为0xffffffff
	
	for (i = 0; i < length; i++)
	{
		c = buf[i];						 //新移进来的字节值
		t= (oldcrc32 >> 24) ^ c;				 //要移出的字节的值
		oldcrc = crc32_table[t];		 //根据移出的字节的值查表
		oldcrc32 <<= 8;
		oldcrc32 ^= oldcrc;		 //将寄存器与查出的值进行xor 运算
	}

	crc32 = oldcrc32;

	return crc32;
}

/*
int Decode_CRC_32(uint8_t* buf, int length)
{
	int flag_src;
	int flag_dst;
	int flag;

	uint32_t remain = 0xFFFFFFFF;

	int8_t	ch;
	int i, j;

	for (i = 0; i < length; i++)
	{
		ch = *(buf + i);

		for (j = 0; j < 8; j++)
		{
//			flag_src = (ch & 0x80) >> 7;
//			flag_dst = (remain & 0x80000000) >> 31;
			flag_src = ch & 0x80;
			flag_dst = (remain >> 24) & 0x80;

			flag = flag_src ^ flag_dst;

			remain <<= 1;

			if (flag)
			{
				remain ^= POLYNOMIAL_VALUE;
			}

			ch <<= 1;
		}
	}

	return remain;
}

int Encode_CRC_32(uint8_t* buf, int length)
{
	int flag_src;
	int flag_dst;
	int flag;

	uint32_t remain = 0xFFFFFFFF;

	int8_t	ch;
	int i, j;

	for (i = 0; i < length; i++)
	{
		ch = *(buf + i);

		for (j = 0; j < 8; j++)
		{
			flag_src = (ch & 0x80) >> 7;
			flag_dst = (remain & 0x80000000) >> 31;

			flag = flag_src ^ flag_dst;

			remain <<= 1;

			if (flag)
			{
				remain ^= POLYNOMIAL_VALUE;
			}

			ch <<= 1;
		}
	}

	return remain;
}
*/
