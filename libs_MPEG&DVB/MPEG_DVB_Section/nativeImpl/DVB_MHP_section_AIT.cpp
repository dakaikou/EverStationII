#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../nativeInclude/DVB_MHP_section.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_MHP_AIT_DecodeSection(uint8_t *buf, int length, application_information_section_t* pait_section)
{
	S32	 rtcode = SECTION_PARSE_NO_ERROR;
	S32  loop_length;
	U8*  ptemp;
	U8*  pend;
	S32	 inner_loop_length;
	S8	 N;
	S8	 reserved_count;

	U16	 descriptor_tag;
	U8	 descriptor_length;
	U16	 move_length;

	//AIT section最小长度16字节
	if ((buf != NULL) && (length >= DVB_MHP_AIT_SECTION_MIN_SIZE) && (pait_section != NULL))
	{
		memset(pait_section, 0x00, sizeof(application_information_section_t));

		pait_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pait_section->CRC_32 = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		if (pait_section->CRC_32 == pait_section->CRC_32_verify)
		{
			pait_section->table_id = *buf++;

			pait_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			pait_section->reserved_future_use0 = (*buf & 0x40) >> 6;
			pait_section->reserved0 = (*buf & 0x30) >> 4;

			pait_section->section_length = (*buf++ & 0x0f) << 8;
			pait_section->section_length |= *buf++;

			assert((pait_section->section_length <= 1021));

			pait_section->test_application_flag = (*buf & 0x80) >> 7;

			pait_section->application_type = (*buf++ & 0x7f) << 8;
			pait_section->application_type |= *buf++;

			pait_section->reserved1 = (*buf & 0xC0) >> 6;
			pait_section->version_number = (*buf & 0x3E) >> 1;
			pait_section->current_next_indicator = (*buf++ & 0x01);

			pait_section->section_number = *buf++;
			pait_section->last_section_number = *buf++;

			pait_section->reserved_future_use1 = (*buf & 0xf0) >> 4;
			pait_section->common_descriptors_length = (*buf++ & 0x0f) << 8;
			pait_section->common_descriptors_length |= *buf++;

			loop_length = pait_section->common_descriptors_length;
			ptemp = buf;
			pend = ptemp + loop_length;

			reserved_count = 0;
			while (ptemp < pend)
			{
				descriptor_tag = ptemp[0];
				descriptor_length = ptemp[1];

				move_length = descriptor_length + 2;

				switch (descriptor_tag)
				{
				case 0:
				default:
					if (reserved_count < MAX_RESERVED_DESCRIPTORS)
					{
						pait_section->reserved_descriptor[reserved_count].descriptor_tag = (0x2000 | descriptor_tag);
						pait_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
						pait_section->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
						pait_section->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

//						decode_reserved_descriptor(ptemp, move_length, &(pait_section->reserved_descriptor[reserved_count]));
						reserved_count ++;
					}
					break;
				}

				ptemp += move_length;
			}

			pait_section->reserved_count = reserved_count;

			buf = pend;

			pait_section->reserved_future_use2 = (*buf & 0xf0) >> 4;
			pait_section->application_loop_length = (*buf++ & 0x0f) << 8;
			pait_section->application_loop_length |= *buf++;

			loop_length = pait_section->application_loop_length;
			ptemp = buf;
			buf += loop_length;

			N = 0;
			if (loop_length > 0)
			{
				//解析applications
				if (N < MAX_APPLICATIONS)
				{
					pait_section->applications[N].application_identifier.organisation_id = *ptemp++;
					pait_section->applications[N].application_identifier.organisation_id <<= 8;
					pait_section->applications[N].application_identifier.organisation_id |= *ptemp++;
					pait_section->applications[N].application_identifier.organisation_id <<= 8;
					pait_section->applications[N].application_identifier.organisation_id |= *ptemp++;
					pait_section->applications[N].application_identifier.organisation_id <<= 8;
					pait_section->applications[N].application_identifier.organisation_id |= *ptemp++;

					pait_section->applications[N].application_identifier.application_id = *ptemp++;
					pait_section->applications[N].application_identifier.application_id <<= 8;
					pait_section->applications[N].application_identifier.application_id |= *ptemp++;

					pait_section->applications[N].application_control_code = *ptemp++;

					pait_section->applications[N].reserved_future_use = (*ptemp & 0xf0) >> 4;
					pait_section->applications[N].application_descriptors_loop_length = (*ptemp++ & 0x0f) << 8;
					pait_section->applications[N].application_descriptors_loop_length |= *ptemp++;

					inner_loop_length = pait_section->applications[N].application_descriptors_loop_length;
					pend = ptemp + inner_loop_length;

					reserved_count = 0;
					while (ptemp < pend)
					{
						descriptor_tag = ptemp[0];
						descriptor_length = ptemp[1];
						move_length = descriptor_length + 2;

						pait_section->applications[N].reserved_descriptor[reserved_count].descriptor_tag = (0x2000 | descriptor_tag);
						pait_section->applications[N].reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
						pait_section->applications[N].reserved_descriptor[reserved_count].descriptor_buf = ptemp;
						pait_section->applications[N].reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;
		
						reserved_count ++;
						ptemp += move_length;
					}

					ptemp = pend;
					pait_section->applications[N].reserved_count = reserved_count;

					N ++;
				}
			}

			pait_section->N = N;
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

