#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Common/BitStream.h"

#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

#include "CRC_32.h"

int MPEG2_PSI_CAT_DecodeSection(uint8_t *section_buf, int section_size, CA_section_t* pcat_section)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;
	int				move_length;
	uint16_t		descriptor_tag;
	uint8_t			descriptor_length;
	int				loop_length;
	int				reserved_count;

	if ((section_buf != NULL) &&
		((section_size >= MPEG2_PSI_CAT_SECTION_MIN_SIZE) && (section_size <= MPEG2_PSI_CAT_SECTION_MAX_SIZE)) &&
		(pcat_section != NULL))
	{
		memset(pcat_section, 0x00, sizeof(CA_section_t));

		pcat_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (pcat_section->CRC_32 == pcat_section->CRC_32_verify)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pcat_section->table_id = BITS_get(&bs, 8);

			pcat_section->section_syntax_indicator = BITS_get(&bs, 1);
			pcat_section->reserved_future_use = BITS_get(&bs, 1);
			pcat_section->reserved0 = BITS_get(&bs, 2);
			pcat_section->section_length = BITS_get(&bs, 12);
		
			pcat_section->reserved1 = BITS_get(&bs, 18);
			pcat_section->version_number = BITS_get(&bs, 5);
			pcat_section->current_next_indicator = BITS_get(&bs, 1);
		
			pcat_section->section_number = BITS_get(&bs, 8);
			pcat_section->last_section_number = BITS_get(&bs, 8);

			loop_length = pcat_section->section_length - 9;
			if (loop_length > 0)
			{
				ptemp = bs.p_cur;
				BITS_byteSkip(&bs, loop_length);

				reserved_count = 0;
				while ((loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
				{
					descriptor_tag = ptemp[0];
					descriptor_length = ptemp[1];
					move_length = descriptor_length + 2;

					pcat_section->CA_descriptors[reserved_count].descriptor_tag = descriptor_tag;
					pcat_section->CA_descriptors[reserved_count].descriptor_length = descriptor_length;
					pcat_section->CA_descriptors[reserved_count].descriptor_buf = ptemp;
					pcat_section->CA_descriptors[reserved_count].descriptor_size = move_length;

					reserved_count++;

					ptemp += move_length;
					loop_length -= move_length;
				}

				pcat_section->CA_descriptor_count = reserved_count;
			}

			pcat_section->CRC_32 = BITS_get(&bs, 32);
			assert(pcat_section->CRC_32 == CRC_32_encoded);		//检验比特流读取指针是否发生错误
		}

		if (pcat_section->CRC_32_recalculated != pcat_section->CRC_32)
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




