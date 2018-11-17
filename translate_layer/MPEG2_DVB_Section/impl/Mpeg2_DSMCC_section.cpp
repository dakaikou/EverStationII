#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Stream_AL/BitStream.h"
#include "../../Stream_AL/ByteStream.h"

#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

#include "CRC_32.h"

/////////////////////////////////////////////
int MPEG2_DSMCC_DecodeSection(uint8_t *section_buf, int section_size, dsmcc_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	uint8_t*				ptemp;
	int						stream_error;

	uint8_t					section_syntax_indicator;
	uint32_t				encodedCheckValue;

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	if ((section_buf != NULL) &&
		((section_size >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (section_size <= MPEG2_DSMCC_SECTION_MAX_SIZE)) &&
		(pdsmcc_section != NULL))
	{
		memset(pdsmcc_section, 0x00, sizeof(dsmcc_section_t));

		section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

		stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			pdsmcc_section->recalculatedCheckValue = Encode_CRC_32(section_buf, section_size - 4);
			encodedCheckValue = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculatedCheckValue != encodedCheckValue)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加
			pdsmcc_section->recalculatedCheckValue = Encode_CRC_32(section_buf, section_size - 4);
			encodedCheckValue = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculatedCheckValue != encodedCheckValue)
			{
				stream_error = 1;
			}
		}

		//if (stream_error == 0)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pdsmcc_section->table_id = BITS_get(&bs, 8);
			//assert(pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM);

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

					if (pdsmccMessageHeader->dsmccType == 0x03)
					{
						pdsmccMessageHeader->messageId = BYTES_get(&bytes, 2);
						pdsmccMessageHeader->TxOrDnloadID = BYTES_get(&bytes, 4);
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

							pdsmccAdaptationHeader->adaptationDataLength = pdsmccMessageHeader->adaptationLength - 1;

							//BYTES_copy(pdsmccAdaptationHeader->adaptationDataByte, sizeof(pdsmccAdaptationHeader->adaptationDataByte), &bytes, pdsmccAdaptationHeader->adaptationDataLength);
							pdsmccAdaptationHeader->adaptationDataByte = bytes.p_cur;
							BYTES_skip(&bytes, pdsmccAdaptationHeader->adaptationDataLength);
						}

						pdsmcc_section->dsmccMessagePayloadLength = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
						if (pdsmcc_section->dsmccMessagePayloadLength > 0)
						{
							assert(pdsmcc_section->dsmccMessagePayloadLength <= 4072);
							pdsmcc_section->dsmccMessagePayloadBuf = bytes.p_cur;
							BYTES_skip(&bytes, pdsmcc_section->dsmccMessagePayloadLength);
						}

						assert(bytes.p_cur == bs.p_cur);
					}
					else
					{
						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}

				pdsmcc_section->encodedCheckValue = BITS_get(&bs, 32);
				assert(pdsmcc_section->encodedCheckValue == encodedCheckValue);
			}
			else
			{
				rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
			}
		}

		if (pdsmcc_section->recalculatedCheckValue != pdsmcc_section->encodedCheckValue)
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
