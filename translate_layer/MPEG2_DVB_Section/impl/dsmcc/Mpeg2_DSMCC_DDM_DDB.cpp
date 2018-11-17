#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_DDM_DDB.h"
#include "../../Include/MPEG2_DVB_ErrorCode.h"
//
/////////////////////////////////////////////
int	MPEG2_DSMCC_DDM_DecodeDownloadDataBlock(uint8_t *buf, int length, DownloadDataBlock_t* pDownloadDataBlock)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;

	if ((buf != NULL) && (length > 0) && (pDownloadDataBlock != NULL))
	{
		memset(pDownloadDataBlock, 0x00, sizeof(DownloadDataBlock_t));

		ptemp = buf;

		pDownloadDataBlock->moduleId = *ptemp++;
		pDownloadDataBlock->moduleId <<= 8;
		pDownloadDataBlock->moduleId |= *ptemp++;

		pDownloadDataBlock->moduleVersion = *ptemp++;

		pDownloadDataBlock->reserved = *ptemp++;

		pDownloadDataBlock->blockNumber = *ptemp++;
		pDownloadDataBlock->blockNumber <<= 8;
		pDownloadDataBlock->blockNumber |= *ptemp++;

		pDownloadDataBlock->blockDataLength = length - (unsigned short)(ptemp - buf);
		if (pDownloadDataBlock->blockDataLength > 0)
		{
			pDownloadDataBlock->blockDataByte = ptemp;
			ptemp += pDownloadDataBlock->blockDataLength;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

