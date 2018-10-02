#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_BAT_DecodeSection(uint8_t *buf, int length, bouquet_association_section_t* pbat_section)
{
	S32		 rtcode = SECTION_PARSE_NO_ERROR;
	S32		 N = 0;
	S32		 stream_loop_length;
	S32		 descriptor_loop_length;
	U8*		 pl1temp;
	U8*		 pl2temp;

	uint16_t descriptor_tag;
	int		 descriptor_length;

	S32		 move_length;
	S32		 reserved_count;

	STREAM_DESCRIPTION_t*	pStream;

	if ((buf != NULL) && (length >= DVB_SI_BAT_SECTION_MIN_SIZE) && (length <= DVB_SI_BAT_SECTION_MAX_SIZE) && (pbat_section != NULL))
	{
		memset(pbat_section, 0x00, sizeof(bouquet_association_section_t));

		pbat_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pbat_section->CRC_32 = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		if (pbat_section->CRC_32_verify == pbat_section->CRC_32)
		{
			pbat_section->table_id = *buf++;

			pbat_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			pbat_section->reserved_future_use0 = (*buf & 0x40) >> 6;
			pbat_section->reserved0 = (*buf & 0x30) >> 4;

			pbat_section->section_length = (*buf++ & 0x0f) << 8;
			pbat_section->section_length |= *buf++;

			pbat_section->bouquet_id = (*buf++) << 8;
			pbat_section->bouquet_id |= *buf++;

			pbat_section->reserved1 = (*buf & 0xC0) >> 6;
			pbat_section->version_number = (*buf & 0x3E) >> 1;
			pbat_section->current_next_indicator = (*buf++ & 0x01);

			pbat_section->section_number = *buf++;
			pbat_section->last_section_number = *buf++;

			pbat_section->reserved_future_use1 = (*buf & 0xf0) >> 4;
			pbat_section->bouquet_descriptors_length = (*buf++ & 0x0f) << 8;
			pbat_section->bouquet_descriptors_length |= *buf++;

			pl1temp = buf;
			buf += pbat_section->bouquet_descriptors_length;

			reserved_count = 0;
			descriptor_loop_length = pbat_section->bouquet_descriptors_length;
			while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
			{
				descriptor_tag = pl1temp[0];
				descriptor_length = pl1temp[1];
				move_length = descriptor_length + 2;

				pbat_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
				pbat_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
				pbat_section->reserved_descriptor[reserved_count].descriptor_buf = pl1temp;
				pbat_section->reserved_descriptor[reserved_count].descriptor_size = move_length;

				reserved_count ++;
				pl1temp += move_length;
				descriptor_loop_length -= move_length;
			}
			pbat_section->reserved_count = reserved_count;


			pbat_section->reserved_future_use2 = (*buf & 0xf0) >> 4;
			pbat_section->transport_stream_loop_length = (*buf++ & 0x0f) << 8;
			pbat_section->transport_stream_loop_length |= *buf++;

			N = 0;
			if (pbat_section->transport_stream_loop_length > 0)
			{
				pl1temp = buf;
				stream_loop_length = pbat_section->transport_stream_loop_length;
				buf += stream_loop_length;
			
				while ((stream_loop_length >= 6) && (N < MAX_STREAMS_PER_BAT_SECTION))
				{
					pStream = pbat_section->astStream + N;

					pStream->transport_stream_id = (*pl1temp++) << 8;
					pStream->transport_stream_id |= *pl1temp++;

					pStream->original_network_id = (*pl1temp++) << 8;
					pStream->original_network_id |= *pl1temp++;

					pStream->reserved = (*pl1temp & 0xf0) >> 4;

					pStream->transport_descriptors_length = (*pl1temp++ & 0x0f) << 8;
					pStream->transport_descriptors_length |= *pl1temp++;

					reserved_count = 0;
					if (pStream->transport_descriptors_length > 0)
					{
						pl2temp = pl1temp;
						pl1temp += pStream->transport_descriptors_length;
						descriptor_loop_length = pStream->transport_descriptors_length;

						while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
						{
							descriptor_tag = pl2temp[0];
							descriptor_length = pl2temp[1];
							move_length = descriptor_length + 2;

							pStream->transport_descriptors[reserved_count].descriptor_tag = descriptor_tag;
							pStream->transport_descriptors[reserved_count].descriptor_length = descriptor_length;
							pStream->transport_descriptors[reserved_count].descriptor_buf = pl2temp;
							pStream->transport_descriptors[reserved_count].descriptor_size = (uint8_t)move_length;

							reserved_count ++;

							pl2temp += move_length;
							descriptor_loop_length -= move_length;
						}
					}
					pStream->transport_descriptor_count = reserved_count;

					stream_loop_length -= (6 + pStream->transport_descriptors_length);

					N ++;
				}
			}
			
			pbat_section->N = N;
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


