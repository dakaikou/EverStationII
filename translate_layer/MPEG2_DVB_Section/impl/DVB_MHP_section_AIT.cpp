#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Stream_AL/BitStream.h"

#include "../Include/DVB_MHP_section.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

#include "CRC_32.h"

/////////////////////////////////////////////
int DVB_MHP_AIT_DecodeSection(uint8_t *section_buf, int section_size, application_information_section_t* pait_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*	 pl1temp;
	uint8_t*	 pl2temp;

	uint16_t	 descriptor_tag;
	uint8_t		 descriptor_length;
	int			 move_length;

	//AIT section最小长度16字节
	if ((section_buf != NULL) &&
		((section_size >= DVB_MHP_AIT_SECTION_MIN_SIZE) && (section_size <= DVB_MHP_AIT_SECTION_MAX_SIZE)) &&
		(pait_section != NULL))
	{
		memset(pait_section, 0x00, sizeof(application_information_section_t));

		pait_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (pait_section->CRC_32 == pait_section->CRC_32_verify)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pait_section->table_id = BITS_get(&bs, 8);

			pait_section->section_syntax_indicator = BITS_get(&bs, 1);
			pait_section->reserved_future_use0 = BITS_get(&bs, 1);
			pait_section->reserved0 = BITS_get(&bs, 2);
			pait_section->section_length = BITS_get(&bs, 12);

			assert((pait_section->section_length <= 1021));

			pait_section->test_application_flag = BITS_get(&bs, 1);
			pait_section->application_type = BITS_get(&bs, 15);

			pait_section->reserved1 = BITS_get(&bs, 2);
			pait_section->version_number = BITS_get(&bs, 5);
			pait_section->current_next_indicator = BITS_get(&bs, 1);

			pait_section->section_number = BITS_get(&bs, 8);
			pait_section->last_section_number = BITS_get(&bs, 8);

			pait_section->reserved_future_use1 = BITS_get(&bs, 4);
			pait_section->common_descriptors_length = BITS_get(&bs, 12);

			if (pait_section->common_descriptors_length > 0)
			{
				pl1temp = bs.p_cur;
				BITS_byteSkip(&bs, pait_section->common_descriptors_length);

				int descriptor_loop_length = pait_section->common_descriptors_length;

				int descriptors_count = 0;
				while ((descriptor_loop_length >= 2) && (descriptors_count < MAX_RESERVED_DESCRIPTORS))
				{
					descriptor_tag = pl1temp[0];
					descriptor_length = pl1temp[1];
					move_length = descriptor_length + 2;

					assert(descriptors_count < MAX_RESERVED_DESCRIPTORS);

					pait_section->common_descriptors[descriptors_count].descriptor_buf = pl1temp;
					pait_section->common_descriptors[descriptors_count].descriptor_size = move_length;

					pait_section->common_descriptors[descriptors_count].descriptor_tag = descriptor_tag;
					pait_section->common_descriptors[descriptors_count].descriptor_length = descriptor_length;

					descriptors_count++;

					pl1temp += move_length;
					descriptor_loop_length -= move_length;
				}
				pait_section->common_descriptor_count = descriptors_count;
			}


			pait_section->reserved_future_use2 = BITS_get(&bs, 4);
			pait_section->application_loop_length = BITS_get(&bs, 12);

			if (pait_section->application_loop_length > 0)
			{
				pl1temp = bs.p_cur;
				BITS_byteSkip(&bs, pait_section->application_loop_length);

				int loop_length = pait_section->application_loop_length;
				int N = 0;
				while ((loop_length >= 2) && (N < MAX_APPLICATIONS))
				{
					application_t* papplication = pait_section->applications + N;

					//解析applications
					papplication->application_identifier.organisation_id = *pl1temp++;
					papplication->application_identifier.organisation_id <<= 8;
					papplication->application_identifier.organisation_id |= *pl1temp++;
					papplication->application_identifier.organisation_id <<= 8;
					papplication->application_identifier.organisation_id |= *pl1temp++;
					papplication->application_identifier.organisation_id <<= 8;
					papplication->application_identifier.organisation_id |= *pl1temp++;

					papplication->application_identifier.application_id = *pl1temp++;
					papplication->application_identifier.application_id <<= 8;
					papplication->application_identifier.application_id |= *pl1temp++;

					papplication->application_control_code = *pl1temp++;

					papplication->reserved_future_use = (*pl1temp & 0xf0) >> 4;
					papplication->application_descriptors_loop_length = (*pl1temp++ & 0x0f) << 8;
					papplication->application_descriptors_loop_length |= *pl1temp++;

					pl2temp = pl1temp;
					pl1temp += papplication->application_descriptors_loop_length;
					int descriptors_count = 0;
					int inner_loop_length = papplication->application_descriptors_loop_length;
					while ((inner_loop_length >= 2) && (descriptors_count < MAX_RESERVED_DESCRIPTORS))
					{
						descriptor_tag = pl2temp[0];
						descriptor_length = pl2temp[1];
						move_length = descriptor_length + 2;

						papplication->descriptors[descriptors_count].descriptor_tag = (0x2000 | descriptor_tag);
						papplication->descriptors[descriptors_count].descriptor_length = descriptor_length;
						papplication->descriptors[descriptors_count].descriptor_buf = pl2temp;
						papplication->descriptors[descriptors_count].descriptor_size = move_length;

						descriptors_count++;

						pl2temp += move_length;
						inner_loop_length -= move_length;
					}

					papplication->descriptor_count = descriptors_count;

					loop_length -= (9 + papplication->application_descriptors_loop_length);
					N++;
				}
				pait_section->application_count = N;
			}

			pait_section->CRC_32 = BITS_get(&bs, 32);
			assert(pait_section->CRC_32 == CRC_32_encoded);
		}

		if (pait_section->CRC_32_recalculated != pait_section->CRC_32)
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

