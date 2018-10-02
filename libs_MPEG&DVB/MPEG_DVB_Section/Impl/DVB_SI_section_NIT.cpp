#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_NIT_DecodeSection(uint8_t *buf, int length, network_information_section_t* pnit_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	int		 stream_count = 0;
	S32		 stream_loop_length;
	S32		 descriptor_loop_length;
	U8*		 pl1temp;
	U8*		 pl2temp;

	uint16_t descriptor_tag;
	int		 descriptor_length;

	S32		 move_length;
	S32		 reserved_count;

	STREAM_DESCRIPTION_t*	pstStream;

	if ((buf != NULL) && (length >= DVB_SI_NIT_SECTION_MIN_SIZE) && (length <= DVB_SI_NIT_SECTION_MAX_SIZE) && (pnit_section != NULL))
	{
		memset(pnit_section, 0x00, sizeof(network_information_section_t));

		pnit_section->CRC_32_recalculated = Encode_CRC_32(buf, length - 4);
		uint32_t CRC_32_encoded = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		//if (pnit_section->CRC_32_recalculated == pnit_section->CRC_32)
		{
			pnit_section->table_id = *buf++;

			pnit_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			pnit_section->reserved_future_use0 = (*buf & 0x40) >> 6;
			pnit_section->reserved0 = (*buf & 0x30) >> 4;

			pnit_section->section_length = (*buf++ & 0x0f) << 8;
			pnit_section->section_length |= *buf++;

			pnit_section->network_id = (*buf++) << 8;
			pnit_section->network_id |= *buf++;

			pnit_section->reserved1 = (*buf & 0xC0) >> 6;
			pnit_section->version_number = (*buf & 0x3E) >> 1;
			pnit_section->current_next_indicator = (*buf++ & 0x01);

			pnit_section->section_number = *buf++;
			pnit_section->last_section_number = *buf++;

			pnit_section->reserved_future_use1 = (*buf & 0xf0) >> 4;

			pnit_section->network_descriptors_length = (*buf++ & 0x0f) << 8;
			pnit_section->network_descriptors_length |= *buf++;

			pl1temp = buf;							//save the network descriptor buf pointer
			buf += pnit_section->network_descriptors_length;

			reserved_count = 0;

			descriptor_loop_length = pnit_section->network_descriptors_length;
			while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
			{
				descriptor_tag = pl1temp[0];
				descriptor_length = pl1temp[1];
				move_length = descriptor_length + 2;

				pnit_section->network_descriptors[reserved_count].descriptor_tag = descriptor_tag;
				pnit_section->network_descriptors[reserved_count].descriptor_length = descriptor_length;
				pnit_section->network_descriptors[reserved_count].descriptor_buf = pl1temp;
				pnit_section->network_descriptors[reserved_count].descriptor_size = (uint8_t)move_length;

				reserved_count ++;

				pl1temp += move_length;
				descriptor_loop_length -= move_length;
			}

			pnit_section->network_descriptor_count = reserved_count;

			pnit_section->reserved_future_use2 = (*buf & 0xf0) >> 4;

			pnit_section->transport_stream_loop_length = (*buf++ & 0x0f) << 8;
			pnit_section->transport_stream_loop_length |= *buf++;

			stream_count = 0;
			if (pnit_section->transport_stream_loop_length > 0)
			{
				pl1temp = buf;
				stream_loop_length = pnit_section->transport_stream_loop_length;
				buf += stream_loop_length;
				
				while ((stream_loop_length >= 6) && (stream_count < MAX_STREAMS_PER_NIT_SECTION))
				{
					pstStream = pnit_section->astStreams + stream_count;

					pstStream->transport_stream_id = (*pl1temp++) << 8;
					pstStream->transport_stream_id |= *pl1temp++;

					pstStream->original_network_id = (*pl1temp++) << 8;
					pstStream->original_network_id |= *pl1temp++;

					pstStream->reserved = (*pl1temp  & 0xf0) >> 4;

					pstStream->transport_descriptors_length = (*pl1temp++ & 0x0f) << 8;
					pstStream->transport_descriptors_length |= *pl1temp++;

					reserved_count = 0;
					if (pstStream->transport_descriptors_length > 0)
					{
						pl2temp = pl1temp;
						pl1temp += pstStream->transport_descriptors_length;
						descriptor_loop_length = pstStream->transport_descriptors_length;

						while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
						{
							descriptor_tag = pl2temp[0];
							descriptor_length = pl2temp[1];
							move_length = descriptor_length + 2;

							pstStream->transport_descriptors[reserved_count].descriptor_tag = descriptor_tag;
							pstStream->transport_descriptors[reserved_count].descriptor_length = descriptor_length;
							pstStream->transport_descriptors[reserved_count].descriptor_buf = pl2temp;
							pstStream->transport_descriptors[reserved_count].descriptor_size = (uint8_t)move_length;

							reserved_count ++;

							pl2temp += move_length;
							descriptor_loop_length -= move_length;
						}
					}
					pstStream->transport_descriptor_count = reserved_count;

					stream_loop_length -= (6 + pstStream->transport_descriptors_length);

					stream_count++;
				}
			}

			pnit_section->stream_count = stream_count;

			pnit_section->CRC_32 = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
			assert(pnit_section->CRC_32 == CRC_32_encoded);
			buf += 4;
		}

		if (pnit_section->CRC_32 != pnit_section->CRC_32_recalculated)
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

