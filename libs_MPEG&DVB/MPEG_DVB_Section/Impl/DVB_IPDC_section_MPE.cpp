#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../Include/DVB_IPDC_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_IPDC_MPE_DecodeSection(uint8_t *section_buf, int section_size, datagram_section_t* pdatagram_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
////	S32  N;
//	S32  loop_length;
//	U8*  ptemp;

	uint32_t encodedCheckValue;

	if ((section_buf != NULL) &&
		((section_size >= DVB_IPDC_MPE_SECTION_MIN_SIZE) && (section_size <= DVB_IPDC_MPE_SECTION_MAX_SIZE)) &&
		(pdatagram_section != NULL))
	{
		memset(pdatagram_section, 0x00, sizeof(datagram_section_t));

		int section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

		int stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			pdatagram_section->recalculatedCheckValue = Encode_CRC_32(section_buf, section_size - 4);
			encodedCheckValue = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdatagram_section->recalculatedCheckValue != encodedCheckValue)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加
			pdatagram_section->recalculatedCheckValue = Encode_CRC_32(section_buf, section_size - 4);
			encodedCheckValue = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdatagram_section->recalculatedCheckValue != encodedCheckValue)
			{
				stream_error = 1;
			}
		}

		//if (stream_error == 0)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pdatagram_section->table_id = BITS_get(&bs, 8);

			pdatagram_section->section_syntax_indicator = BITS_get(&bs, 1);
			pdatagram_section->private_indicator = BITS_get(&bs, 1);
			pdatagram_section->reserved0 = BITS_get(&bs, 2);
			pdatagram_section->section_length = BITS_get(&bs, 12);

			pdatagram_section->MAC_address_6 = BITS_get(&bs, 8);
			pdatagram_section->MAC_address_5 = BITS_get(&bs, 8);

			pdatagram_section->reserved1 = BITS_get(&bs, 2);
			pdatagram_section->payload_scrambling_control = BITS_get(&bs, 2);
			pdatagram_section->address_scrambling_control = BITS_get(&bs, 2);
			pdatagram_section->LLC_SNAP_flag = BITS_get(&bs, 1);
			pdatagram_section->current_next_indicator = BITS_get(&bs, 1);

			pdatagram_section->section_number = BITS_get(&bs, 8);
			pdatagram_section->last_section_number = BITS_get(&bs, 8);

			pdatagram_section->MAC_address_4 = BITS_get(&bs, 8);
			pdatagram_section->MAC_address_3 = BITS_get(&bs, 8);
			pdatagram_section->MAC_address_2 = BITS_get(&bs, 8);
			pdatagram_section->MAC_address_1 = BITS_get(&bs, 8);

			int payload_length = pdatagram_section->section_length - 9 - 4;			//exclude CRC32
			if (payload_length > 0)
			{
				uint8_t* ptemp = bs.p_cur;
				BITS_byteSkip(&bs, payload_length);

				if (pdatagram_section->LLC_SNAP_flag == 1)
				{
					pdatagram_section->LLC_SNAP_data_length = payload_length;
					pdatagram_section->LLC_SNAP_data_byte = ptemp;

				}
				else
				{
					//get the IP data
					//FILE* fp = NULL;
					//fopen_s(&fp, "e:\\ip_data.dat", "wb");
					//if (fp != NULL)
					//{
					//	fwrite(ptemp, 1, loop_length, fp);
					//	fclose(fp);
					//}

					pdatagram_section->IP_datagram_data_length = payload_length;
					pdatagram_section->IP_datagram_data_byte = ptemp;
				}
			}

			pdatagram_section->encodedCheckValue = BITS_get(&bs, 32);
			assert(pdatagram_section->encodedCheckValue == encodedCheckValue);

			if (pdatagram_section->recalculatedCheckValue != pdatagram_section->encodedCheckValue)
			{
				rtcode = SECTION_PARSE_CRC_ERROR;						//校验错误
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

