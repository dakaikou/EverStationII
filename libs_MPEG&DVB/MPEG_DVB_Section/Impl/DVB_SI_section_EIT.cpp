#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/DVB_SI_Utilities.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_EIT_DecodeSection(uint8_t *section_buf, int section_length, event_information_section_t* peit_section)
{
	S32		 rtcode = SECTION_PARSE_NO_ERROR;
	S32		 N = 0;
	uint8_t* pl1temp;
	uint8_t* pl2temp;
	uint8_t* buf;

	U16		 descriptor_tag;
	U8		 descriptor_length;
	S32		 move_length;
	S32		 reserved_count;

	EVENT_DESCRIPTION_t*	pEvent;

	if ((section_buf != NULL) && (section_length >= DVB_SI_EIT_SECTION_MIN_SIZE) && (section_length <= DVB_SI_EIT_SECTION_MAX_SIZE) && (peit_section != NULL))
	{
		memset(peit_section, 0x00, sizeof(event_information_section_t));
		
		buf = section_buf;

		peit_section->CRC_32_verify = Encode_CRC_32(buf, section_length - 4);
		peit_section->CRC_32 = (buf[section_length - 4] << 24) | (buf[section_length - 3] << 16) | (buf[section_length - 2] << 8) | buf[section_length - 1];

		if (peit_section->CRC_32 == peit_section->CRC_32_verify)
		{
			peit_section->table_id = *buf++;

			peit_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			peit_section->reserved_future_use = (*buf & 0x40) >> 6;
			peit_section->reserved0 = (*buf & 0x30) >> 4;

			peit_section->section_length = (*buf++ & 0x0f) << 8;
			peit_section->section_length |= *buf++;

			peit_section->service_id = (*buf++) << 8;
			peit_section->service_id |= *buf++;

			peit_section->reserved1 = (*buf & 0xC0) >> 6;
			peit_section->version_number = (*buf & 0x3E) >> 1;
			peit_section->current_next_indicator = (*buf++ & 0x01);

			peit_section->section_number = *buf++;
			peit_section->last_section_number = *buf++;

			peit_section->transport_stream_id = (*buf++) << 8;
			peit_section->transport_stream_id |= *buf++;

			peit_section->original_network_id = *buf++ << 8;
			peit_section->original_network_id |= *buf++;

			peit_section->segment_last_section_number = *buf++;
			peit_section->last_table_id = *buf++;

			N = 0;
			int event_loop_length = peit_section->section_length - 15;
			if (event_loop_length > 0)
			{
				pl1temp = buf;
				buf += event_loop_length;
				while ((event_loop_length >= 12) && (N < MAX_EVENTS_IN_EIT_SECTION))
				{
					pEvent = &(peit_section->astEvent[N]);

					pEvent->event_id = *pl1temp++;
					pEvent->event_id <<= 8;
					pEvent->event_id |= *pl1temp++;

					pEvent->start_time = *pl1temp++;
					pEvent->start_time <<= 8;
					pEvent->start_time |= *pl1temp++;
					pEvent->start_time <<= 8;
					pEvent->start_time |= *pl1temp++;
					pEvent->start_time <<= 8;
					pEvent->start_time |= *pl1temp++;
					pEvent->start_time <<= 8;
					pEvent->start_time |= *pl1temp++;

					pEvent->duration = *pl1temp++;
					pEvent->duration <<= 8;
					pEvent->duration |= *pl1temp++;
					pEvent->duration <<= 8;
					pEvent->duration |= *pl1temp++;

					pEvent->running_status = (*pl1temp & 0xE0) >> 5;
					pEvent->free_CA_mode = (*pl1temp & 0x10) >> 4;

					pEvent->descriptors_loop_length = (*pl1temp++ & 0x0f) << 8;
					pEvent->descriptors_loop_length |= *pl1temp++;

					reserved_count = 0;
					if (pEvent->descriptors_loop_length)
					{
						pl2temp = pl1temp;
						pl1temp += pEvent->descriptors_loop_length;
						int descriptor_loop_length = pEvent->descriptors_loop_length;
						while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
						{
							descriptor_tag = pl2temp[0];
							descriptor_length = pl2temp[1];
							move_length = descriptor_length + 2;

							pEvent->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
							pEvent->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
							pEvent->reserved_descriptor[reserved_count].descriptor_buf = pl2temp;
							pEvent->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

							reserved_count++;

							pl2temp += move_length;
							descriptor_loop_length -= move_length;
						}
					}
					pEvent->reserved_count = reserved_count;

					event_loop_length -= (12 + pEvent->descriptors_loop_length);
					N++;
				}
			}

			peit_section->N = N;
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

