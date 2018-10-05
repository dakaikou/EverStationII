#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/Mpeg2_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

/////////////////////////////////////////////
int DVB_SI_BAT_DecodeSection(uint8_t *section_buf, int section_size, bouquet_association_section_t* pbat_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	int		 N = 0;
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
		((section_size >= DVB_SI_BAT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_BAT_SECTION_MAX_SIZE)) &&
		(pbat_section != NULL))
	{
		memset(pbat_section, 0x00, sizeof(bouquet_association_section_t));

		pbat_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (pbat_section->CRC_32_verify == pbat_section->CRC_32)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pbat_section->table_id = BITS_get(&bs, 8);

			pbat_section->section_syntax_indicator = BITS_get(&bs, 1);
			pbat_section->reserved_future_use0 = BITS_get(&bs, 1);
			pbat_section->reserved0 = BITS_get(&bs, 2);
			pbat_section->section_length = BITS_get(&bs, 12);

			pbat_section->bouquet_id = BITS_get(&bs, 16);

			pbat_section->reserved1 = BITS_get(&bs, 2);
			pbat_section->version_number = BITS_get(&bs, 5);
			pbat_section->current_next_indicator = BITS_get(&bs, 1);

			pbat_section->section_number = BITS_get(&bs, 8);
			pbat_section->last_section_number = BITS_get(&bs, 8);

			pbat_section->reserved_future_use1 = BITS_get(&bs, 4);
			pbat_section->bouquet_descriptors_length = BITS_get(&bs, 12);

			if (pbat_section->bouquet_descriptors_length > 0)
			{
				pl1temp = bs.p_cur;
				BITS_byteSkip(&bs, pbat_section->bouquet_descriptors_length);

				reserved_count = 0;
				descriptor_loop_length = pbat_section->bouquet_descriptors_length;
				while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
				{
					descriptor_tag = pl1temp[0];
					descriptor_length = pl1temp[1];
					move_length = descriptor_length + 2;

					pbat_section->bouquet_descriptors[reserved_count].descriptor_tag = descriptor_tag;
					pbat_section->bouquet_descriptors[reserved_count].descriptor_length = descriptor_length;
					pbat_section->bouquet_descriptors[reserved_count].descriptor_buf = pl1temp;
					pbat_section->bouquet_descriptors[reserved_count].descriptor_size = move_length;

					reserved_count++;
					pl1temp += move_length;
					descriptor_loop_length -= move_length;
				}
				pbat_section->bouquet_descriptor_count = reserved_count;
			}

			pbat_section->reserved_future_use2 = BITS_get(&bs, 4);
			pbat_section->transport_stream_loop_length = BITS_get(&bs, 12);

			if (pbat_section->transport_stream_loop_length > 0)
			{
				pl1temp = bs.p_cur;
				BITS_byteSkip(&bs, pbat_section->transport_stream_loop_length);

				stream_loop_length = pbat_section->transport_stream_loop_length;
				N = 0;

				while ((stream_loop_length >= 6) && (N < MAX_STREAMS_PER_BAT_SECTION))
				{
					pstStream = pbat_section->astStreams + N;

					pstStream->transport_stream_id = (*pl1temp++) << 8;
					pstStream->transport_stream_id |= *pl1temp++;

					pstStream->original_network_id = (*pl1temp++) << 8;
					pstStream->original_network_id |= *pl1temp++;

					pstStream->reserved = (*pl1temp & 0xf0) >> 4;

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

					N ++;
				}
				pbat_section->stream_count = N;
			}

			pbat_section->CRC_32 = BITS_get(&bs, 32);
			assert(pbat_section->CRC_32 == CRC_32_encoded);
		}

		if (pbat_section->CRC_32_recalculated != pbat_section->CRC_32)
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


