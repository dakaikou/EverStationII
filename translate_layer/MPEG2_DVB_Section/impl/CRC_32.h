#ifndef __CRC32_H__
#define __CRC32_H__

//void init_crc32_table(void);
int Decode_CRC_32(unsigned char* buf, int length);
int Encode_CRC_32(unsigned char* buf, int length);

#endif

