#ifndef _TSMAGIC_ERRORCODE_H_
#define _TSMAGIC_ERRORCODE_H_

#include <stdint.h>

#ifdef TSMAGIC_NO_ERROR
#undef TSMAGIC_NO_ERROR
#endif

#define TSMAGIC_NO_ERROR					0x00000000

//ÏµÍ³´íÎó´úÂë
#define TSMAGIC_PARAMETER_ERROR				0xE0000001
//#define TSMAGIC_FIFO_EMPTY_ERROR			0xE0000002
#define TSMAGIC_NO_ENOUGH_MEMORY			0xE0000003
//#define TSMAGIC_FIFO_WRITE_OVERFLOW		0xE0000004
#define TSMAGIC_NOT_SUPPORTED_TABLE			0xE0000005

#define TSMAGIC_UNKNOWN_ERROR				0xffffffff

int	TSMagic_ErrorCodeLookup(uint32_t error_code, char* pszText, int strSize);

#endif