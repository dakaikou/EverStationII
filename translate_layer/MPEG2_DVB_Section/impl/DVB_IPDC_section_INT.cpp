#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../../Stream_AL/BitStream.h"

#include "../Include/DVB_IPDC_section.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

#include "CRC_32.h"

/////////////////////////////////////////////
int DVB_IPDC_INT_DecodeSection(uint8_t *section_buf, int section_size, IP_MAC_notification_section_t* pint_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t	 descriptor_tag;
	uint8_t		 descriptor_length;
	int			 move_length;
	int			 reserved_count;
	int			 loop_length;

	//int		 target_count = 0;

	if ((section_buf != NULL) && 
		((section_size >= DVB_IPDC_INT_SECTION_MIN_SIZE) && (section_size <= DVB_IPDC_INT_SECTION_MAX_SIZE)) &&
		(pint_section != NULL))
	{
		memset(pint_section, 0x00, sizeof(IP_MAC_notification_section_t));

		pint_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (pint_section->CRC_32 == CRC_32_encoded)
		{
			BITS_t  bs;			//exclude CRC32
			BITS_map(&bs, section_buf, section_size);

			pint_section->table_id = BITS_get(&bs, 8);

			pint_section->section_syntax_indicator = BITS_get(&bs, 1);
			pint_section->reserved_for_future_use = BITS_get(&bs, 1);
			pint_section->reserved0 = BITS_get(&bs, 2);
			pint_section->section_length = BITS_get(&bs, 12);

			pint_section->action_type = BITS_get(&bs, 8);

			pint_section->platform_id_hash = BITS_get(&bs, 8);

			pint_section->reserved1 = BITS_get(&bs, 2);
			pint_section->version_number = BITS_get(&bs, 5);
			pint_section->current_next_indicator = BITS_get(&bs, 1);

			pint_section->section_number = BITS_get(&bs, 8);

			pint_section->last_section_number = BITS_get(&bs, 8);

			pint_section->platform_id = BITS_get(&bs, 24);

			pint_section->processing_order = BITS_get(&bs, 8);

			//decoding platform_descriptor_loop
			platform_descriptor_loop_t* pplatform_descriptor_loop = &(pint_section->platform_descriptor_loop);
			pplatform_descriptor_loop->reserved = BITS_get(&bs, 4);
			pplatform_descriptor_loop->platform_descriptor_loop_length = BITS_get(&bs, 12);

			if (pplatform_descriptor_loop->platform_descriptor_loop_length > 0)
			{
				uint8_t* ptemp = bs.p_cur;
				BITS_byteSkip(&bs, pplatform_descriptor_loop->platform_descriptor_loop_length);

				reserved_count = 0;
				loop_length = pplatform_descriptor_loop->platform_descriptor_loop_length;
				while ((loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
				{
					descriptor_tag = ptemp[0];
					descriptor_length = ptemp[1];
					move_length = descriptor_length + 2;

					pplatform_descriptor_loop->reserved_descriptor[reserved_count].descriptor_tag = (0x1000 | descriptor_tag);
					pplatform_descriptor_loop->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
					pplatform_descriptor_loop->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
					pplatform_descriptor_loop->reserved_descriptor[reserved_count].descriptor_size = move_length;

					reserved_count++;
					ptemp += move_length;
					loop_length -= move_length;
				}

				pplatform_descriptor_loop->reserved_count = reserved_count;
			}

			loop_length = section_size - 14 - pplatform_descriptor_loop->platform_descriptor_loop_length - 4;
			if (loop_length > 0)
			{
				uint8_t* ptemp = bs.p_cur;
				BITS_byteSkip(&bs, loop_length);

				int	notification_count = 0;

				while ((loop_length >= 4) && (notification_count < MAX_INT_TARGETS))
				{
					target_descriptor_loop_t* ptarget_descriptor_loop = &(pint_section->notifications[notification_count].target_descriptor_loop);

					//decoding target_descriptor_loop
					ptarget_descriptor_loop->reserved = (*ptemp & 0xf0) >> 4;
					ptarget_descriptor_loop->target_descriptor_loop_length = (*ptemp++ & 0x0f);
					ptarget_descriptor_loop->target_descriptor_loop_length <<= 4;
					ptarget_descriptor_loop->target_descriptor_loop_length |= *ptemp++;

					uint8_t* pnext = ptemp + ptarget_descriptor_loop->target_descriptor_loop_length;

					reserved_count = 0;
					int target_descriptor_loop_length = ptarget_descriptor_loop->target_descriptor_loop_length;
					while ((target_descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
					{
						descriptor_tag = ptemp[0];
						descriptor_length = ptemp[1];
						move_length = descriptor_length + 2;

						ptarget_descriptor_loop->target_descriptors[reserved_count].descriptor_tag = (0x1000 | descriptor_tag);
						ptarget_descriptor_loop->target_descriptors[reserved_count].descriptor_length = descriptor_length;
						ptarget_descriptor_loop->target_descriptors[reserved_count].descriptor_buf = ptemp;
						ptarget_descriptor_loop->target_descriptors[reserved_count].descriptor_size = move_length;

						reserved_count++;
						ptemp += move_length;
						target_descriptor_loop_length -= move_length;
					}
					ptarget_descriptor_loop->target_descriptor_count = reserved_count;

					//decoding operational_descriptor_loop
					ptemp = pnext;									
					operational_descriptor_loop_t* poperational_descriptor_loop = &(pint_section->notifications[notification_count].operational_descriptor_loop);

					poperational_descriptor_loop->reserved = (*ptemp & 0xf0) >> 4;
					poperational_descriptor_loop->operational_descriptor_loop_length = (*ptemp++ & 0x0f);
					poperational_descriptor_loop->operational_descriptor_loop_length <<= 4;
					poperational_descriptor_loop->operational_descriptor_loop_length |= *ptemp++;

					pnext = ptemp + poperational_descriptor_loop->operational_descriptor_loop_length;

					reserved_count = 0;
					int operational_descriptor_loop_length = poperational_descriptor_loop->operational_descriptor_loop_length;
					while ((operational_descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
					{
						descriptor_tag = ptemp[0];
						descriptor_length = ptemp[1];
						move_length = descriptor_length + 2;

						if (descriptor_tag >= 0x40)
						{
							poperational_descriptor_loop->operational_descriptors[reserved_count].descriptor_tag = descriptor_tag;
						}
						else
						{
							poperational_descriptor_loop->operational_descriptors[reserved_count].descriptor_tag = (0x1000 | descriptor_tag);
						}
						poperational_descriptor_loop->operational_descriptors[reserved_count].descriptor_length = descriptor_length;
						poperational_descriptor_loop->operational_descriptors[reserved_count].descriptor_buf = ptemp;
						poperational_descriptor_loop->operational_descriptors[reserved_count].descriptor_size = move_length;

						reserved_count++;
						ptemp += move_length;
						operational_descriptor_loop_length -= move_length;
					}

					poperational_descriptor_loop->operational_descriptor_count = reserved_count;

					notification_count++;
					loop_length -= ((2 + ptarget_descriptor_loop->target_descriptor_loop_length) + (2 + poperational_descriptor_loop->operational_descriptor_loop_length));
				}

				pint_section->notification_count = notification_count;
			}

			pint_section->CRC_32 = BITS_get(&bs, 32);
			assert(pint_section->CRC_32 == CRC_32_encoded);		//检验比特流读取指针是否发生错误
		}

		if (pint_section->CRC_32_recalculated != pint_section->CRC_32)
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


