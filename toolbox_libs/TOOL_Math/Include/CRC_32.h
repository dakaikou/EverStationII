#ifndef __CRC32_H__
#define __CRC32_H__

#include "../compile.h"
//void init_crc32_table(void);
_CDL_EXPORT int Decode_CRC_32(unsigned char* buf, int length);
_CDL_EXPORT int Encode_CRC_32(unsigned char* buf, int length);

#endif

