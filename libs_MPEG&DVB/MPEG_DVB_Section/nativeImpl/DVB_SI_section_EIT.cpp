#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/Mpeg2_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"
#include "../nativeInclude/DVB_SI_Utilities.h"

#include "libs_Math/Include/CRC_32.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

/////////////////////////////////////////////
int DVB_SI_EIT_DecodeSection(uint8_t *section_buf, int section_size, event_information_section_t* peit_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	int		 N = 0;
	uint8_t* pl1temp;
	uint8_t* pl2temp;

	uint16_t descriptor_tag;
	uint8_t	 descriptor_length;
	int		 move_length;
	int		 reserved_count;

	EVENT_DESCRIPTION_t*	pstEvent;

	if ((section_buf != NULL) && 
		((section_size >= DVB_SI_EIT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_EIT_SECTION_MAX_SIZE)) &&
		(peit_section != NULL))
	{
		memset(peit_section, 0x00, sizeof(event_information_section_t));

		peit_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (peit_section->CRC_32 == peit_section->CRC_32_verify)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			peit_section->table_id = BITS_get(&bs, 8);

			peit_section->section_syntax_indicator = BITS_get(&bs, 1);
			peit_section->reserved_future_use = BITS_get(&bs, 1);
			peit_section->reserved0 = BITS_get(&bs, 2);

			peit_section->section_length = BITS_get(&bs, 12);

			peit_section->service_id = BITS_get(&bs, 16);

			peit_section->reserved1 = BITS_get(&bs, 2);
			peit_section->version_number = BITS_get(&bs, 5);
			peit_section->current_next_indicator = BITS_get(&bs, 1);

			peit_section->section_number = BITS_get(&bs, 8);
			peit_section->last_section_number = BITS_get(&bs, 8);

			peit_section->transport_stream_id = BITS_get(&bs, 16);

			peit_section->original_network_id = BITS_get(&bs, 16);

			peit_section->segment_last_section_number = BITS_get(&bs, 8);
			peit_section->last_table_id = BITS_get(&bs, 8);

			N = 0;
			int event_loop_length = peit_section->section_length - 15;
			if (event_loop_length > 0)
			{
				pl1temp = bs.p_cur;
				BITS_byteSkip(&bs, event_loop_length);
				while ((event_loop_length >= 12) && (N < MAX_EVENTS_IN_EIT_SECTION))
				{
					pstEvent = &(peit_section->astEvents[N]);

					pstEvent->event_id = *pl1temp++;
					pstEvent->event_id <<= 8;
					pstEvent->event_id |= *pl1temp++;

					pstEvent->start_time = *pl1temp++;
					pstEvent->start_time <<= 8;
					pstEvent->start_time |= *pl1temp++;
					pstEvent->start_time <<= 8;
					pstEvent->start_time |= *pl1temp++;
					pstEvent->start_time <<= 8;
					pstEvent->start_time |= *pl1temp++;
					pstEvent->start_time <<= 8;
					pstEvent->start_time |= *pl1temp++;

					pstEvent->duration = *pl1temp++;
					pstEvent->duration <<= 8;
					pstEvent->duration |= *pl1temp++;
					pstEvent->duration <<= 8;
					pstEvent->duration |= *pl1temp++;

					pstEvent->running_status = (*pl1temp & 0xE0) >> 5;
					pstEvent->free_CA_mode = (*pl1temp & 0x10) >> 4;

					pstEvent->descriptors_loop_length = (*pl1temp++ & 0x0f) << 8;
					pstEvent->descriptors_loop_length |= *pl1temp++;

					reserved_count = 0;
					if (pstEvent->descriptors_loop_length)
					{
						pl2temp = pl1temp;
						pl1temp += pstEvent->descriptors_loop_length;
						int descriptor_loop_length = pstEvent->descriptors_loop_length;
						while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
						{
							descriptor_tag = pl2temp[0];
							descriptor_length = pl2temp[1];
							move_length = descriptor_length + 2;

							pstEvent->event_descriptors[reserved_count].descriptor_tag = descriptor_tag;
							pstEvent->event_descriptors[reserved_count].descriptor_length = descriptor_length;
							pstEvent->event_descriptors[reserved_count].descriptor_buf = pl2temp;
							pstEvent->event_descriptors[reserved_count].descriptor_size = move_length;

							reserved_count++;

							pl2temp += move_length;
							descriptor_loop_length -= move_length;
						}
					}
					pstEvent->event_descriptor_count = reserved_count;

					event_loop_length -= (12 + pstEvent->descriptors_loop_length);
					N++;
				}
			}

			peit_section->event_count = N;

			peit_section->CRC_32 = BITS_get(&bs, 32);
			assert(peit_section->CRC_32 == CRC_32_encoded);
		}

		if (peit_section->CRC_32_recalculated != peit_section->CRC_32)
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

