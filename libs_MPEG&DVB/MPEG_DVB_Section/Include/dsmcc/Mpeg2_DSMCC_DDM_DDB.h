#ifndef _MPEG_DSMCC_DDM_DDB_H_
#define _MPEG_DSMCC_DDM_DDB_H_

#include "../../compile.h"
#include "Mpeg2_DSMCC_DDM_DDB_OC.h"
#include "Mpeg2_DSMCC_DDM_DDB_DC.h"

///*------------------------------------------------------------
//		DSM-CC definition 
//-------------------------------------------------------------*/
//DDB的封装格式只有一种，DDB的净荷可能有多种情况
typedef struct DownloadDataBlock_s
{
	uint16_t		moduleId;								//16
	uint8_t			moduleVersion;							//8
	uint8_t			reserved;								//8
	uint16_t		blockNumber;							//16

	uint16_t		blockDataLength;
	uint8_t*		blockDataByte;

} DownloadDataBlock_t, *pDownloadDataBlock_t;

typedef struct DownloadDataRequest_s
{
	uint8_t		reserved;
} DownloadDataRequest_t, *pDownloadDataRequest_t;

_CDL_EXPORT	int	MPEG2_DSMCC_DDM_DecodeDownloadDataBlock(uint8_t *buf, int length, DownloadDataBlock_t* pDownloadDataBlock);

#endif
