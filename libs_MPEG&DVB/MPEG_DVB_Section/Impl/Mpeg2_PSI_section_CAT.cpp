#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

int MPEG2_PSI_CAT_DecodeSection(uint8_t *buf, int length, CA_section_t* pcat_section)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		move_length;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		loop_length;
	S32		reserved_count;

	if ((buf != NULL) && (pcat_section != NULL) && (length >= MPEG2_PSI_CAT_SECTION_MIN_SIZE))
	{
		memset(pcat_section, 0x00, sizeof(CA_section_t));

		pcat_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pcat_section->CRC_32 = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		if (pcat_section->CRC_32 == pcat_section->CRC_32_verify)
		{
			pcat_section->table_id = *buf++;

			pcat_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			pcat_section->reserved_future_use = (*buf & 0x40) >> 6;
			pcat_section->reserved0 = (*buf & 0x30) >> 4;

			pcat_section->section_length = (*buf++ & 0x0f) << 8;

			pcat_section->section_length |= *buf++;
		
			pcat_section->reserved1 = *buf++;

			pcat_section->reserved1 <<= 8;
			pcat_section->reserved1 |= *buf++;

			pcat_section->reserved1 <<= 2;
			pcat_section->reserved1 |= (*buf & 0xC0) >> 6;

			pcat_section->version_number = (*buf & 0x3E) >> 1;
			pcat_section->current_next_indicator = (*buf++ & 0x01);
		
			pcat_section->section_number = *buf++;
			pcat_section->last_section_number = *buf++;

			loop_length = pcat_section->section_length - 9;
			ptemp = buf;
			reserved_count = 0;
			while (loop_length > 0)
			{
				descriptor_tag = ptemp[0];
				descriptor_length = ptemp[1];
				move_length = descriptor_length + 2;

				pcat_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
				pcat_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
				pcat_section->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
				pcat_section->reserved_descriptor[reserved_count].descriptor_size = move_length;

				reserved_count ++;

				ptemp += move_length;
				loop_length -= move_length;
			}

			pcat_section->reserved_count = reserved_count;
		}
		else
		{
			rtcode = SECTION_PARSE_CRC_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




