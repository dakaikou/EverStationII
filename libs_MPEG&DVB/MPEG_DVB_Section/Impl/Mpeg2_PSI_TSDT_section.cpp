#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int MPEG2_PSI_TSDT_DecodeSection(unsigned char *buf, int length, TS_description_section_t* pTS_description_section)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		move_length;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		loop_length;
	S32		reserved_count;

	if ((buf != NULL) && (pTS_description_section != NULL) && (length >= MPEG2_PSI_TSDT_SECTION_MIN_SIZE))
	{
		memset(pTS_description_section, 0x00, sizeof(TS_description_section_t));

		pTS_description_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pTS_description_section->CRC_32 = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		if (pTS_description_section->CRC_32 == pTS_description_section->CRC_32_verify)
		{
			pTS_description_section->table_id = *buf++;

			pTS_description_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			pTS_description_section->reserved_future_use = (*buf & 0x40) >> 6;
			pTS_description_section->reserved0 = (*buf & 0x30) >> 4;
		
			pTS_description_section->section_length = (*buf++ & 0x0f) << 8;
			pTS_description_section->section_length |= *buf++;
		
			pTS_description_section->reserved1 = *buf++;
			pTS_description_section->reserved1 <<= 8;
			pTS_description_section->reserved1 |= *buf++;
			pTS_description_section->reserved1 <<= 2;
			pTS_description_section->reserved1 |= (*buf & 0xC0) >> 6;
			pTS_description_section->version_number = (*buf & 0x3E) >> 1;
			pTS_description_section->current_next_indicator = (*buf++ & 0x01);
		
			pTS_description_section->section_number = *buf++;
			pTS_description_section->last_section_number = *buf++;

			loop_length = pTS_description_section->section_length - 9;
			ptemp = buf;
			reserved_count = 0;
			while (loop_length > 0)
			{
				descriptor_tag = ptemp[0];
				descriptor_length = ptemp[1];
				move_length = descriptor_length + 2;

				pTS_description_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
				pTS_description_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
				pTS_description_section->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
				pTS_description_section->reserved_descriptor[reserved_count].descriptor_size = move_length;

				reserved_count ++;

				ptemp += move_length;
				loop_length -= move_length;
			}

			pTS_description_section->reserved_count = reserved_count;
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




