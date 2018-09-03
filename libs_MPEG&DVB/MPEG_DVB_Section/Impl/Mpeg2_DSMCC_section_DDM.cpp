#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

//#include "../Include/Mpeg2_DSMCC_Descriptor.h"
//#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_DSMCC_section_DDM.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DecodeDownloadDataBlock(uint8_t *buf, int length, DownloadDataBlock_t* pDownloadDataBlock)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;

	if ((buf != NULL) && (length > 0) && (pDownloadDataBlock != NULL))
	{
		memset(pDownloadDataBlock, 0x00, sizeof(DownloadDataBlock_t));

		ptemp = buf;

		pDownloadDataBlock->moduleId = *ptemp ++;
		pDownloadDataBlock->moduleId <<= 8;
		pDownloadDataBlock->moduleId |= *ptemp ++;

		pDownloadDataBlock->moduleVersion = *ptemp ++;

		pDownloadDataBlock->reserved = *ptemp ++;

		pDownloadDataBlock->blockNumber = *ptemp ++;
		pDownloadDataBlock->blockNumber <<= 8;
		pDownloadDataBlock->blockNumber |= *ptemp ++;

		pDownloadDataBlock->N = length - (unsigned short)(ptemp - buf);
		if (pDownloadDataBlock->N > 0)
		{
//			memcpy(pDownloadDataBlock->blockDataByte, ptemp, pDownloadDataBlock->N);
			pDownloadDataBlock->blockDataByte = ptemp;
			ptemp += pDownloadDataBlock->N;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_DSMCC_DDM_DecodeSection(uint8_t *buf, int length, dsmcc_ddm_section_t* pDSMCC_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	int						payload_length;
	int						copy_length;
	uint8_t*				ptemp;
	uint32_t				tempId;
	int						stream_error;

	uint8_t					section_syntax_indicator;

	dsmccDownloadDataHeader_t*			pdsmccDownloadDataHeader;
	DownloadDataBlock_t*				pDownloadDataBlock;

	if ((buf != NULL) && (length >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (pDSMCC_section != NULL))
	{
		memset(pDSMCC_section, 0x00, sizeof(dsmcc_ddm_section_t));

		pDSMCC_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pDSMCC_section->CRC_32 = ((buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1]);

		section_syntax_indicator = (buf[1] & 0x80) >> 7;

		stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			if (pDSMCC_section->CRC_32_verify != pDSMCC_section->CRC_32)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加

//			assert((length % 4) == 0);
		}

		if (stream_error == 0)
		{
			pDSMCC_section->table_id = *buf++;
			assert(pDSMCC_section->table_id == TABLE_ID_DSMCC_DDM);

			pDSMCC_section->section_syntax_indicator = (*buf & 0x80) >> 7;

			pDSMCC_section->dsmcc_section_length = (*buf++ & 0x0f) << 8;
			pDSMCC_section->dsmcc_section_length |= *buf++;

			if ((pDSMCC_section->dsmcc_section_length + 3) == length)
			{
				pDSMCC_section->table_id_extension = (*buf++) << 8;
				pDSMCC_section->table_id_extension |= *buf++;

				pDSMCC_section->version_number = (*buf & 0x3E) >> 1;
				pDSMCC_section->current_next_indicator = (*buf++ & 0x01);

				pDSMCC_section->section_number = *buf++;
				pDSMCC_section->last_section_number = *buf++;

				ptemp = buf;
				buf += (pDSMCC_section->dsmcc_section_length - 9);

				pdsmccDownloadDataHeader = &(pDSMCC_section->dsmccDownloadDataHeader);

				pdsmccDownloadDataHeader->protocolDiscriminator = *ptemp++;
				pdsmccDownloadDataHeader->dsmccType = *ptemp ++;

				pdsmccDownloadDataHeader->messageId = *ptemp ++;
				pdsmccDownloadDataHeader->messageId <<= 8;
				pdsmccDownloadDataHeader->messageId |= *ptemp ++;

				tempId = *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;

				pdsmccDownloadDataHeader->downloadId = tempId;

				pdsmccDownloadDataHeader->reserved = *ptemp ++;

				pdsmccDownloadDataHeader->adaptationLength = *ptemp ++;

				pdsmccDownloadDataHeader->messageLength = *ptemp ++;
				pdsmccDownloadDataHeader->messageLength <<= 8;
				pdsmccDownloadDataHeader->messageLength |= *ptemp ++;

				assert(pdsmccDownloadDataHeader->adaptationLength <= pdsmccDownloadDataHeader->messageLength);
				//messageLength解析错误，将是灾难性的

				if (pdsmccDownloadDataHeader->adaptationLength > 0)
				{
					//解析adaptation
					pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationType = *ptemp ++;

					copy_length = min(64, pdsmccDownloadDataHeader->adaptationLength - 1);
					pdsmccDownloadDataHeader->dsmccAdaptationHeader.N = copy_length;
					if (copy_length > 0)
					{
						memcpy(pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationDataByte, ptemp, copy_length);
						ptemp += (pdsmccDownloadDataHeader->adaptationLength - 1);
					}
				}

				payload_length = (int)(buf - ptemp);

				if (payload_length >= pdsmccDownloadDataHeader->messageLength)
				{
					if (pdsmccDownloadDataHeader->messageId == 0x1002)			//DII
					{
						assert(0);
					}
					else if (pdsmccDownloadDataHeader->messageId == 0x1003)					//DDB
					{
						pDownloadDataBlock = &(pDSMCC_section->DownloadDataBlock);
						rtcode = MPEG2_DSMCC_DecodeDownloadDataBlock(ptemp, payload_length, pDownloadDataBlock);

					}
					else if (pdsmccDownloadDataHeader->messageId == 0x1006)							//DSI
					{
						//assert(0);
					}
					else
					{
						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}

				buf += 4;
			}
			else
			{
				rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
			}
		}
		else
		{
			rtcode = SECTION_PARSE_CRC_ERROR;						//校验错误
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

