#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

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

//DDM

int	MPEG2_DSMCC_DecodeDownloadDataBlock(uint8_t *buf, int length, DownloadDataBlock_t* pDownloadDataBlock)
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

int MPEG2_DSMCC_DDM_DecodeSection(uint8_t *section_buf, int section_size, dsmcc_ddm_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*				ptemp;
	int						stream_error;

	uint8_t					section_syntax_indicator;

	dsmccDownloadDataHeader_t*			pdsmccDownloadDataHeader;
	DownloadDataBlock_t*				pDownloadDataBlock;

	if ((section_buf != NULL) &&
		((section_size >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (section_size <= MPEG2_DSMCC_SECTION_MAX_SIZE)) &&
		(pdsmcc_section != NULL))
	{
		memset(pdsmcc_section, 0x00, sizeof(dsmcc_ddm_section_t));

		section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

		stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			pdsmcc_section->recalculated.CRC_32 = Encode_CRC_32(section_buf, section_size - 4);
			uint32_t CRC_32_encoded = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculated.CRC_32 != CRC_32_encoded)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加
			pdsmcc_section->recalculated.checksum = Encode_CRC_32(section_buf, section_size - 4);
			uint32_t checksum_encoded = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculated.checksum != checksum_encoded)
			{
				stream_error = 1;
			}
		}

		if (stream_error == 0)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pdsmcc_section->table_id = BITS_get(&bs, 8);
			assert(pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM);

			pdsmcc_section->section_syntax_indicator = BITS_get(&bs, 1);
			pdsmcc_section->private_indicator = BITS_get(&bs, 1);
			pdsmcc_section->reserved0 = BITS_get(&bs, 2);
			pdsmcc_section->dsmcc_section_length = BITS_get(&bs, 12);

			if ((pdsmcc_section->dsmcc_section_length + 3) == section_size)
			{
				pdsmcc_section->table_id_extension = BITS_get(&bs, 16);

				pdsmcc_section->reserved1 = BITS_get(&bs, 2);
				pdsmcc_section->version_number = BITS_get(&bs, 5);
				pdsmcc_section->current_next_indicator = BITS_get(&bs, 1);

				pdsmcc_section->section_number = BITS_get(&bs, 8);
				pdsmcc_section->last_section_number = BITS_get(&bs, 8);

				int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
				if (section_payload_length > 0)
				{
					ptemp = bs.p_cur;
					BITS_byteSkip(&bs, section_payload_length);

					pdsmccDownloadDataHeader = &(pdsmcc_section->dsmccDownloadDataHeader);

					BYTES_t bytes;
					BYTES_map(&bytes, ptemp, section_payload_length);

					pdsmccDownloadDataHeader->protocolDiscriminator = BYTES_get(&bytes, 1);
					pdsmccDownloadDataHeader->dsmccType = BYTES_get(&bytes, 1);
					pdsmccDownloadDataHeader->messageId = BYTES_get(&bytes, 2);
					pdsmccDownloadDataHeader->downloadId = BYTES_get(&bytes, 4);
					pdsmccDownloadDataHeader->reserved = BYTES_get(&bytes, 1);
					pdsmccDownloadDataHeader->adaptationLength = BYTES_get(&bytes, 1);
					pdsmccDownloadDataHeader->messageLength = BYTES_get(&bytes, 2);

					assert(pdsmccDownloadDataHeader->adaptationLength <= pdsmccDownloadDataHeader->messageLength);
					//messageLength解析错误，将是灾难性的

					if (pdsmccDownloadDataHeader->adaptationLength > 0)
					{
						//解析adaptation
						pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationType = BYTES_get(&bytes, 1);
						pdsmccDownloadDataHeader->dsmccAdaptationHeader.N = pdsmccDownloadDataHeader->adaptationLength - 1;

						if (pdsmccDownloadDataHeader->dsmccAdaptationHeader.N > 0)
						{
							dsmccAdaptationHeader_t* pdsmccAdaptationHeader = &(pdsmccDownloadDataHeader->dsmccAdaptationHeader);
							BYTES_copy(pdsmccAdaptationHeader->adaptationDataByte, sizeof(pdsmccAdaptationHeader), &bytes, pdsmccAdaptationHeader->N);
						}
					}

					int msg_payload_length = pdsmccDownloadDataHeader->messageLength - pdsmccDownloadDataHeader->adaptationLength;
					if (msg_payload_length > 0)
					{
						if (pdsmccDownloadDataHeader->messageId == 0x1003)					//DDB
						{
							pDownloadDataBlock = &(pdsmcc_section->DownloadDataBlock);
							rtcode = MPEG2_DSMCC_DecodeDownloadDataBlock(bytes.p_cur, msg_payload_length, pDownloadDataBlock);
						}
						else
						{
							assert(0);
							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
						BYTES_skip(&bytes, msg_payload_length);
					}

					assert(bytes.p_cur == bs.p_cur);
				}

				uint32_t checksum_or_crc32 = BITS_get(&bs, 32);
				if (pdsmcc_section->section_syntax_indicator == 1)
				{
					pdsmcc_section->encode.CRC_32 = checksum_or_crc32;
				}
				else
				{
					pdsmcc_section->encode.checksum = checksum_or_crc32;
				}
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

int MPEG2_DSMCC_UNM_DecodeSection(uint8_t *section_buf, int section_size, dsmcc_unm_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	uint8_t*				ptemp;
	int						stream_error;

	uint8_t					section_syntax_indicator;

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	DownloadServerInitiate_t*			pDownloadServerInitiate;
	DownloadInfoIndication_t*			pDownloadInfoIndication;
	//DownloadDataBlock_t*				pDownloadDataBlock;

	if ((section_buf != NULL) &&
		((section_size >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (section_size <= MPEG2_DSMCC_SECTION_MAX_SIZE)) &&
		(pdsmcc_section != NULL))
	{
		memset(pdsmcc_section, 0x00, sizeof(dsmcc_unm_section_t));

		section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

		stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			pdsmcc_section->recalculated.CRC_32 = Encode_CRC_32(section_buf, section_size - 4);
			uint32_t CRC_32_encoded = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculated.CRC_32 != CRC_32_encoded)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加
			pdsmcc_section->recalculated.checksum = Encode_CRC_32(section_buf, section_size - 4);
			uint32_t checksum_encoded = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculated.checksum != checksum_encoded)
			{
				stream_error = 1;
			}
		}

		//if (stream_error == 0)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pdsmcc_section->table_id = BITS_get(&bs, 8);
			assert(pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM);

			pdsmcc_section->section_syntax_indicator = BITS_get(&bs, 1);
			pdsmcc_section->private_indicator = BITS_get(&bs, 1);
			pdsmcc_section->reserved0 = BITS_get(&bs, 2);
			pdsmcc_section->dsmcc_section_length = BITS_get(&bs, 12);

			if ((pdsmcc_section->dsmcc_section_length + 3) == section_size)
			{
				pdsmcc_section->table_id_extension = BITS_get(&bs, 16);

				pdsmcc_section->reserved1 = BITS_get(&bs, 2);
				pdsmcc_section->version_number = BITS_get(&bs, 5);
				pdsmcc_section->current_next_indicator = BITS_get(&bs, 1);

				pdsmcc_section->section_number = BITS_get(&bs, 8);
				pdsmcc_section->last_section_number = BITS_get(&bs, 8);

				int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
				if (section_payload_length > 0)
				{
					ptemp = bs.p_cur;
					BITS_byteSkip(&bs, section_payload_length);

					pdsmccMessageHeader = &(pdsmcc_section->dsmccMessageHeader);

					BYTES_t bytes;
					BYTES_map(&bytes, ptemp, section_payload_length);

					pdsmccMessageHeader->protocolDiscriminator = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->dsmccType = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->messageId = BYTES_get(&bytes, 2);
					pdsmccMessageHeader->transactionId = BYTES_get(&bytes, 4);
					pdsmccMessageHeader->reserved = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->adaptationLength = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->messageLength = BYTES_get(&bytes, 2);

					assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
					//messageLength解析错误，将是灾难性的

					if (pdsmccMessageHeader->adaptationLength > 0)
					{
						dsmccAdaptationHeader_t* pdsmccAdaptationHeader = &(pdsmccMessageHeader->dsmccAdaptationHeader);
						//解析adaptation
						pdsmccAdaptationHeader->adaptationType = BYTES_get(&bytes, 1);

						pdsmccAdaptationHeader->N = pdsmccMessageHeader->adaptationLength - 1;
						BYTES_copy(pdsmccAdaptationHeader->adaptationDataByte, sizeof(pdsmccAdaptationHeader->adaptationDataByte), &bytes, pdsmccAdaptationHeader->N);
					}

					int msg_payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
					if (msg_payload_length > 0)
					{
						if (pdsmccMessageHeader->messageId == 0x1002)			//DII
						{
							pDownloadInfoIndication = &(pdsmcc_section->u.DownloadInfoIndication);

							rtcode = MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(bytes.p_cur, msg_payload_length, pDownloadInfoIndication);
						}
						else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
						{
							pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);

							rtcode = MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(bytes.p_cur, msg_payload_length, pDownloadServerInitiate);
						}
						else
						{
							assert(0);
							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}

						BYTES_skip(&bytes, msg_payload_length);
					}

					assert(bytes.p_cur == bs.p_cur);
				}

				uint32_t checksum_or_crc32 = BITS_get(&bs, 32);
				if (pdsmcc_section->section_syntax_indicator == 1)
				{
					pdsmcc_section->encode.CRC_32 = checksum_or_crc32;
				}
				else
				{
					pdsmcc_section->encode.checksum = checksum_or_crc32;
				}
			}
			else
			{
				rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
			}
		}

		//else
		//{
		//	rtcode = SECTION_PARSE_CRC_ERROR;						//校验错误
		//}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
