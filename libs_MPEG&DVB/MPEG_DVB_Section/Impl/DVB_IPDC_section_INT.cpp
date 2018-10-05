#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../../Include/DVB_IPDC_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_IPDC_INT_DecodeSection(uint8_t *buf, int length, IP_MAC_notification_section_t* pint_section)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U32		 N = 0;
//	U8*		 ptemp;
	U8*		 pnext;
	U8*		 pend;

	U16		 descriptor_tag;
	U8		 descriptor_length;
	U16		 move_length;
	S32		 reserved_count;

	S32		 target_count = 0;

	if ((buf != NULL) && (length >= DVB_IPDC_INT_SECTION_MIN_SIZE) && (pint_section != NULL))
	{
		memset(pint_section, 0x00, sizeof(IP_MAC_notification_section_t));

		pint_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pint_section->CRC_32 = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		if (pint_section->CRC_32 == pint_section->CRC_32_verify)
		{
			pend = buf + length - 4;			//exclude CRC32

			pint_section->table_id = *buf++;
			pint_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			pint_section->reserved_for_future_use = (*buf & 0x40) >> 6;
			pint_section->reserved0 = (*buf & 0x30) >> 4;

			pint_section->section_length = (*buf++ & 0x0f) << 8;
			pint_section->section_length |= *buf++;

			pint_section->action_type = *buf++;
			pint_section->platform_id_hash = *buf++;

			pint_section->reserved1 = (*buf & 0xc0) >> 6;
			pint_section->version_number = (*buf & 0x3E) >> 1;
			pint_section->current_next_indicator = (*buf++ & 0x01);

			pint_section->section_number = *buf++;
			pint_section->last_section_number = *buf++;

			pint_section->platform_id = *buf++;
			pint_section->platform_id <<= 8;
			pint_section->platform_id |= *buf++;
			pint_section->platform_id <<= 8;
			pint_section->platform_id |= *buf++;

			pint_section->processing_order = *buf++;

			//decoding platform_descriptor_loop
			pint_section->platform_descriptor_loop.reserved = (*buf & 0xf0) >> 4;
			pint_section->platform_descriptor_loop.platform_descriptor_loop_length = (*buf++ & 0x0f);
			pint_section->platform_descriptor_loop.platform_descriptor_loop_length <<= 4;
			pint_section->platform_descriptor_loop.platform_descriptor_loop_length |= *buf++;

			pnext = buf + pint_section->platform_descriptor_loop.platform_descriptor_loop_length;

			reserved_count = 0;
			while (buf < pnext)
			{
				descriptor_tag = buf[0];
				descriptor_length = buf[1];
				move_length = descriptor_length + 2;

				pint_section->platform_descriptor_loop.reserved_descriptor[reserved_count].descriptor_tag = (0x1000 | descriptor_tag);
				pint_section->platform_descriptor_loop.reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
				pint_section->platform_descriptor_loop.reserved_descriptor[reserved_count].descriptor_buf = buf;
				pint_section->platform_descriptor_loop.reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

				reserved_count ++;
				buf += move_length;
			}

			pint_section->platform_descriptor_loop.reserved_count = reserved_count;

			//decoding target loop
			target_count = 0;

			while (buf < pend)
			{
				//decoding target_descriptor_loop
				pint_section->target_descriptor_loop[target_count].reserved = (*buf & 0xf0) >> 4;
				pint_section->target_descriptor_loop[target_count].target_descriptor_loop_length = (*buf++ & 0x0f);
				pint_section->target_descriptor_loop[target_count].target_descriptor_loop_length <<= 4;
				pint_section->target_descriptor_loop[target_count].target_descriptor_loop_length |= *buf++;

				pnext = buf + pint_section->target_descriptor_loop[target_count].target_descriptor_loop_length;

				reserved_count = 0;
				while (buf < pnext)
				{
					descriptor_tag = buf[0];
					descriptor_length = buf[1];
					move_length = descriptor_length + 2;

					pint_section->target_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_tag = (0x1000 | descriptor_tag);
					pint_section->target_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
					pint_section->target_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_buf = buf;
					pint_section->target_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

					reserved_count ++;
					buf += move_length;
				}
				pint_section->target_descriptor_loop[target_count].reserved_count = reserved_count;

				//decoding operational_descriptor_loop
				pint_section->operational_descriptor_loop[target_count].reserved = (*buf & 0xf0) >> 4;
				pint_section->operational_descriptor_loop[target_count].operational_descriptor_loop_length = (*buf++ & 0x0f);
				pint_section->operational_descriptor_loop[target_count].operational_descriptor_loop_length <<= 4;
				pint_section->operational_descriptor_loop[target_count].operational_descriptor_loop_length |= *buf++;

				pnext = buf + pint_section->operational_descriptor_loop[target_count].operational_descriptor_loop_length;

				reserved_count = 0;
				while (buf < pnext)
				{
					descriptor_tag = buf[0];
					descriptor_length = buf[1];
					move_length = descriptor_length + 2;

					if (descriptor_tag >= 0x40)
					{
						pint_section->operational_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
					}
					else
					{
						pint_section->operational_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_tag = (0x1000 | descriptor_tag);
					}
					pint_section->operational_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
					pint_section->operational_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_buf = buf;
					pint_section->operational_descriptor_loop[target_count].reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

					reserved_count ++;
					buf += move_length;
				}

				pint_section->operational_descriptor_loop[target_count].reserved_count = reserved_count;
				target_count ++;
			}

			pint_section->N = target_count;
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


