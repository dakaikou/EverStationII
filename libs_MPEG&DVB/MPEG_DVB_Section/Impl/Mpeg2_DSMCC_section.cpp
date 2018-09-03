#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

//#include "../Include/Mpeg2_DSMCC_Descriptor.h"
//#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
int MPEG2_DSMCC_DecodeSection(uint8_t *buf, int length, dsmcc_section_t* pDSMCC_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	int						payload_length;
	int						copy_length;
	uint8_t*				ptemp;
	uint32_t				tempId;
	int						stream_error;

	uint8_t					section_syntax_indicator;

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	//DownloadServerInitiate_t*			pDownloadServerInitiate;
	//DownloadInfoIndication_t*			pDownloadInfoIndication;
	//DownloadDataBlock_t*				pDownloadDataBlock;

	if ((buf != NULL) && (length >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (pDSMCC_section != NULL))
	{
		memset(pDSMCC_section, 0x00, sizeof(dsmcc_section_t));

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

				if (pDSMCC_section->table_id == TABLE_ID_DSMCC_MPE)
				{
//					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}
				else if ((pDSMCC_section->table_id == TABLE_ID_DSMCC_UNM) ||
						 (pDSMCC_section->table_id == TABLE_ID_DSMCC_DDM))
				{
					pdsmccMessageHeader = &(pDSMCC_section->dsmccMessageHeader);

					pdsmccMessageHeader->protocolDiscriminator = *ptemp++;
					pdsmccMessageHeader->dsmccType = *ptemp ++;

					pdsmccMessageHeader->messageId = *ptemp ++;
					pdsmccMessageHeader->messageId <<= 8;
					pdsmccMessageHeader->messageId |= *ptemp ++;

					tempId = *ptemp ++;
					tempId <<= 8;
					tempId |= *ptemp ++;
					tempId <<= 8;
					tempId |= *ptemp ++;
					tempId <<= 8;
					tempId |= *ptemp ++;

					if (pDSMCC_section->table_id == TABLE_ID_DSMCC_UNM)
					{
						pdsmccMessageHeader->transactionId = tempId;
					}
					//else if (pDSMCC_section->table_id == TABLE_ID_DSM_CC_DDM)
					//{
					//	pdsmccMessageHeader->u.downloadId = tempId;
					//}

					pdsmccMessageHeader->reserved = *ptemp ++;

					pdsmccMessageHeader->adaptationLength = *ptemp ++;

					pdsmccMessageHeader->messageLength = *ptemp ++;
					pdsmccMessageHeader->messageLength <<= 8;
					pdsmccMessageHeader->messageLength |= *ptemp ++;

					assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
					//messageLength解析错误，将是灾难性的

					if (pdsmccMessageHeader->adaptationLength > 0)
					{
						//解析adaptation
						pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType = *ptemp ++;

						copy_length = min(64, pdsmccMessageHeader->adaptationLength - 1);
						pdsmccMessageHeader->dsmccAdaptationHeader.N = copy_length;
						if (copy_length > 0)
						{
							memcpy(pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, ptemp, copy_length);
							ptemp += (pdsmccMessageHeader->adaptationLength - 1);
						}
					}

//					payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
					payload_length = (int)(buf - ptemp);

					if (payload_length >= pdsmccMessageHeader->messageLength)
					{
						if (pdsmccMessageHeader->messageId == 0x1002)			//DII
						{
							//pDownloadInfoIndication = &(pDSMCC_section->u.DownloadInfoIndication);

							//rtcode = MPEG2_DSMCC_DecodeDownloadInfoIndication(ptemp, payload_length, pDownloadInfoIndication);
						}
						else if (pdsmccMessageHeader->messageId == 0x1003)					//DDB
						{
							//pDownloadDataBlock = &(pDSMCC_section->u.DownloadDataBlock);

							//rtcode = MPEG2_DSMCC_DecodeDownloadDataBlock(ptemp, payload_length, pDownloadDataBlock);

						}
						else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
						{
							//pDownloadServerInitiate = &(pDSMCC_section->u.DownloadServerInitiate);

							//rtcode = MPEG2_DSMCC_DecodeDownloadServerInitiate(ptemp, payload_length, pDownloadServerInitiate);
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
				}
				else if (pDSMCC_section->table_id == TABLE_ID_DSMCC_SD)
				{
//					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}
//				else if (pDSMCC_section->table_id == TABLE_ID_DSMCC_PVT)
//				{
////					rtcode = SECTION_PARSE_SYNTAX_ERROR;
//				}

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

