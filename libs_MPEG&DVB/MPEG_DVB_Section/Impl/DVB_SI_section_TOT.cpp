#include <string.h>
#include <assert.h>

#include "../Include/DVB_SI_Descriptor.h"
#include "../Include/DVB_SI_section.h"
#include "../Include/DVB_SI_Utilities.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

int DVB_SI_TOT_DecodeSection(uint8_t *section_buf, int section_length, time_offset_section_t* ptot_section)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	S32		reserved_count;
	S32		left_length;
	U16		descriptor_tag;
	U8		descriptor_length;
	U16		move_length;
	U8*		ptemp;
	U8*		pend;
	uint8_t* buf = section_buf;

	if ((section_buf != NULL) && (section_length >= DVB_SI_TOT_SECTION_MIN_SIZE) && (section_length <= DVB_SI_TOT_SECTION_MAX_SIZE) && (ptot_section != NULL))
	{
		memset(ptot_section, 0x00, sizeof(time_offset_section_t));

		ptot_section->CRC_32_verify = Encode_CRC_32(buf, section_length - 4);

		ptot_section->table_id = *buf++;

		ptot_section->section_syntax_indicator = (*buf & 0x80) >> 7;
		ptot_section->reserved_future_use = (*buf & 0x40) >> 6;
		ptot_section->reserved0 = (*buf & 0x30) >> 4;

		ptot_section->section_length = (*buf++ & 0x0f);
		ptot_section->section_length <<= 8;
		ptot_section->section_length |= *buf++;

		ptot_section->UTC_time = *buf++;
		ptot_section->UTC_time <<= 8;
		ptot_section->UTC_time |= *buf++;
		ptot_section->UTC_time <<= 8;
		ptot_section->UTC_time |= *buf++;
		ptot_section->UTC_time <<= 8;
		ptot_section->UTC_time |= *buf++;
		ptot_section->UTC_time <<= 8;
		ptot_section->UTC_time |= *buf++;
		//memcpy(ptot_section->UTC_time, buf, 5);

		ptot_section->reserved1 = (*buf & 0xf0) >> 4;
		ptot_section->descriptors_loop_length = (*buf++ & 0x0f);
		ptot_section->descriptors_loop_length <<= 8;
		ptot_section->descriptors_loop_length |= *buf++;

		ptemp = buf;
		pend = buf + ptot_section->descriptors_loop_length;

		reserved_count = 0;

		left_length = ptot_section->descriptors_loop_length;
		while ((left_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
		{
			descriptor_tag = ptemp[0];
			descriptor_length = ptemp[1];
			move_length = descriptor_length + 2;

			ptot_section->descriptors[reserved_count].descriptor_tag = descriptor_tag;
			ptot_section->descriptors[reserved_count].descriptor_length = descriptor_length;
			ptot_section->descriptors[reserved_count].descriptor_buf = ptemp;
			ptot_section->descriptors[reserved_count].descriptor_size = (uint8_t)move_length;

			reserved_count ++;

			ptemp += move_length;
			left_length -= move_length;
		}

		ptot_section->N = reserved_count;

		buf = pend;
		
		ptot_section->CRC_32 = (*buf++) << 24;
		ptot_section->CRC_32 |= (*buf++) << 16;
		ptot_section->CRC_32 |= (*buf++) << 8;
		ptot_section->CRC_32 |= *buf++;

		buf += 4;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


