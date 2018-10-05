#include <string.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_Descriptor.h"
#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/DVB_SI_Utilities.h"
#include "../nativeInclude/Mpeg2_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

int DVB_SI_TOT_DecodeSection(uint8_t *section_buf, int section_size, time_offset_section_t* ptot_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	int		reserved_count;
	int		left_length;
	uint16_t	descriptor_tag;
	uint8_t		descriptor_length;
	uint16_t	move_length;
	uint8_t*	ptemp;

	if ((section_buf != NULL) && 
		((section_size >= DVB_SI_TOT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_TOT_SECTION_MAX_SIZE)) &&
		(ptot_section != NULL))
	{
		memset(ptot_section, 0x00, sizeof(time_offset_section_t));

		ptot_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		BITS_t bs;
		BITS_map(&bs, section_buf, section_size);

		ptot_section->table_id = BITS_get(&bs, 8);

		ptot_section->section_syntax_indicator = BITS_get(&bs, 1);
		ptot_section->reserved_future_use = BITS_get(&bs, 1);
		ptot_section->reserved0 = BITS_get(&bs, 2);
		ptot_section->section_length = BITS_get(&bs, 12);

		ptot_section->UTC_time = BITS_x64_get(&bs, 40);

		ptot_section->reserved1 = BITS_get(&bs, 4);
		ptot_section->descriptors_loop_length = BITS_get(&bs, 12);

		if (ptot_section->descriptors_loop_length > 0)
		{
			ptemp = bs.p_cur;
			BITS_byteSkip(&bs, ptot_section->descriptors_loop_length);

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

				reserved_count++;

				ptemp += move_length;
				left_length -= move_length;
			}

			ptot_section->descriptor_count = reserved_count;
		}
		
		ptot_section->CRC_32 = BITS_get(&bs, 32);
		assert(ptot_section->CRC_32 == CRC_32_encoded);

		if (ptot_section->CRC_32_recalculated != ptot_section->CRC_32)
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


