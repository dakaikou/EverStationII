#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

/////////////////////////////////////////////
int DVB_SI_NIT_DecodeSection(uint8_t *section_buf, int section_size, network_information_section_t* pnit_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	int		 stream_count = 0;
	int		 stream_loop_length;
	int		 descriptor_loop_length;
	uint8_t* pl1temp;
	uint8_t* pl2temp;

	uint16_t descriptor_tag;
	int		 descriptor_length;

	int		 move_length;
	int		 reserved_count;

	STREAM_DESCRIPTION_t*	pstStream;

	if ((section_buf != NULL) && 
		((section_size >= DVB_SI_NIT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_NIT_SECTION_MAX_SIZE)) && 
		(pnit_section != NULL))
	{
		memset(pnit_section, 0x00, sizeof(network_information_section_t));

		pnit_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (pnit_section->CRC_32_recalculated == pnit_section->CRC_32)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pnit_section->table_id = BITS_get(&bs, 8);

			pnit_section->section_syntax_indicator = BITS_get(&bs, 1);
			pnit_section->reserved_future_use0 = BITS_get(&bs, 1);
			pnit_section->reserved0 = BITS_get(&bs, 2);
			pnit_section->section_length = BITS_get(&bs, 12);

			pnit_section->network_id = BITS_get(&bs, 16);

			pnit_section->reserved1 = BITS_get(&bs, 2);
			pnit_section->version_number = BITS_get(&bs, 5);
			pnit_section->current_next_indicator = BITS_get(&bs, 1);

			pnit_section->section_number = BITS_get(&bs, 8);
			pnit_section->last_section_number = BITS_get(&bs, 8);

			pnit_section->reserved_future_use1 = BITS_get(&bs, 4);
			pnit_section->network_descriptors_length = BITS_get(&bs, 12);

			if (pnit_section->network_descriptors_length > 0)
			{
				pl1temp = bs.p_cur;							//save the network descriptor buf pointer
				BITS_byteSkip(&bs, pnit_section->network_descriptors_length);

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
					pnit_section->network_descriptors[reserved_count].descriptor_size = move_length;

					reserved_count++;

					pl1temp += move_length;
					descriptor_loop_length -= move_length;
				}

				pnit_section->network_descriptor_count = reserved_count;
			}

			pnit_section->reserved_future_use2 = BITS_get(&bs, 4);

			pnit_section->transport_stream_loop_length = BITS_get(&bs, 12);

			if (pnit_section->transport_stream_loop_length > 0)
			{
				pl1temp = bs.p_cur;
				BITS_byteSkip(&bs, pnit_section->transport_stream_loop_length);

				stream_loop_length = pnit_section->transport_stream_loop_length;
				stream_count = 0;

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
							pstStream->transport_descriptors[reserved_count].descriptor_size = move_length;

							reserved_count ++;

							pl2temp += move_length;
							descriptor_loop_length -= move_length;
						}
					}
					pstStream->transport_descriptor_count = reserved_count;

					stream_loop_length -= (6 + pstStream->transport_descriptors_length);

					stream_count++;
				}
				pnit_section->stream_count = stream_count;
			}

			pnit_section->CRC_32 = BITS_get(&bs, 32);
			assert(pnit_section->CRC_32 == CRC_32_encoded);
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

