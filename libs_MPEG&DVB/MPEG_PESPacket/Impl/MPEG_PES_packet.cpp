#include <string.h>
#include <assert.h>

#include "../Include/MPEG_PES_ErrorCode.h"
#include "../Include/MPEG_PES_packet.h"
#include "../Include/MPEG_stream_id.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int	MPEG_decode_PES_packet(uint8_t *buf, int length, PES_packet_t* pPES_packet)
{
	int rtcode = PES_PACKET_NO_ERROR;

	if ((buf != NULL) && (length > 4) && (pPES_packet != NULL))
	{
		U8 *porigin = buf;
		S64	dts_pos = 0;
		S64	pts_pos = 0;

		pPES_packet->packet_start_code_prefix = *buf++;
		pPES_packet->packet_start_code_prefix <<= 8;
		pPES_packet->packet_start_code_prefix |= *buf++;
		pPES_packet->packet_start_code_prefix <<= 8;
		pPES_packet->packet_start_code_prefix |= *buf++;

		pPES_packet->stream_id = *buf++;

		pPES_packet->PES_packet_length = *buf++;
		pPES_packet->PES_packet_length <<= 8;
		pPES_packet->PES_packet_length |= *buf++;

		if ((pPES_packet->stream_id != PROGRAM_STREAM_MAP) &&
			(pPES_packet->stream_id != PADDING_STREAM) &&
			(pPES_packet->stream_id != PRIVATE_STREAM_2) &&
			(pPES_packet->stream_id != ECM_STREAM) &&
			(pPES_packet->stream_id != EMM_STREAM) &&
			(pPES_packet->stream_id != PROGRAM_STREAM_DIRECTORY) &&
			(pPES_packet->stream_id != DSMCC_STREAM) &&
			(pPES_packet->stream_id != TYPE_E_STREAM))
		{

			pPES_packet->mpeg_flag = (*buf & 0xC0) >> 6;

			pPES_packet->PES_scrambling_control = (*buf & 0x30) >> 4;

			pPES_packet->PES_priority = (*buf & 0x08) >> 3;
			pPES_packet->data_alignment_indicator = (*buf & 0x04) >> 2;
			pPES_packet->copyright = (*buf & 0x02) >> 1;
			pPES_packet->original_or_copy = *buf++ & 0x01;

			pPES_packet->PTS_DTS_flags = (*buf & 0xC0) >> 6;
			pPES_packet->ESCR_flag = (*buf & 0x20) >> 5;
			pPES_packet->ES_rate_flag = (*buf & 0x10) >> 4;
			pPES_packet->DSM_trick_mode_flag = (*buf & 0x08) >> 3;
			pPES_packet->additional_copy_info_flag = (*buf & 0x04) >> 2;
			pPES_packet->PES_CRC_flag = (*buf & 0x02) >> 1;
			pPES_packet->PES_extension_flag = *buf++ & 0x01;

			pPES_packet->PES_header_data_length = *buf++;
			memcpy_s(pPES_packet->PES_header_data, sizeof(pPES_packet->PES_header_data), buf, pPES_packet->PES_header_data_length);

			if ((pPES_packet->PTS_DTS_flags & 0b10) == 0b10)			//have PTS£¬5¸ö×Ö½Ú
			{
				pPES_packet->PTS_marker = ((*buf & 0xF0) >> 4);
				assert((pPES_packet->PTS_marker == 0b0010) || (pPES_packet->PTS_marker == 0b0011));
				pPES_packet->PTS_32_30 = (*buf & 0x0E) >> 1;
				pPES_packet->PTS_marker_bit1 = (*buf++ & 0x01);

				pPES_packet->PTS_29_15 = *buf++;
				pPES_packet->PTS_29_15 <<= 7;
				pPES_packet->PTS_29_15 |= (*buf & 0xFE) >> 1;
				pPES_packet->PTS_marker_bit2 = (*buf++ & 0x01);

				pPES_packet->PTS_14_0 = *buf++;
				pPES_packet->PTS_14_0 <<= 7;
				pPES_packet->PTS_14_0 |= (*buf & 0xFE) >> 1;
				pPES_packet->PTS_marker_bit3 = (*buf++ & 0x01);

				if ((pPES_packet->PTS_DTS_flags & 0b01) == 0b01)			// DTS£¬5¸ö×Ö½Ú
				{
					pPES_packet->DTS_marker = ((*buf & 0xF0) >> 4);
					pPES_packet->DTS_32_30 = (*buf & 0x0E) >> 1;
					pPES_packet->DTS_marker_bit1 = (*buf++ & 0x01);

					pPES_packet->DTS_29_15 = *buf++;
					pPES_packet->DTS_29_15 <<= 7;
					pPES_packet->DTS_29_15 |= (*buf & 0xFE) >> 1;
					pPES_packet->DTS_marker_bit2 = (*buf++ & 0x01);

					pPES_packet->DTS_14_0 = *buf++;
					pPES_packet->DTS_14_0 <<= 7;
					pPES_packet->DTS_14_0 |= (*buf & 0xFE) >> 1;
					pPES_packet->DTS_marker_bit3 = (*buf++ & 0x01);
				}
			}

			if (pPES_packet->ESCR_flag)
			{
				pPES_packet->ESCR_base_32_30 = (*buf & 0x38) >> 3;

				pPES_packet->ESCR_base_29_15 = (*buf++ & 0x03);
				pPES_packet->ESCR_base_29_15 <<= 8;
				pPES_packet->ESCR_base_29_15 |= *buf++;
				pPES_packet->ESCR_base_29_15 <<= 5;
				pPES_packet->ESCR_base_29_15 |= (*buf & 0xf8) >> 3;

				pPES_packet->ESCR_base_14_0 = (*buf++ & 0x03);
				pPES_packet->ESCR_base_14_0 <<= 8;
				pPES_packet->ESCR_base_14_0 |= *buf++;
				pPES_packet->ESCR_base_14_0 <<= 5;
				pPES_packet->ESCR_base_14_0 |= (*buf & 0xf8) >> 3;

				pPES_packet->ESCR_extension = (*buf++ & 0x03);
				pPES_packet->ESCR_extension <<= 7;
				pPES_packet->ESCR_extension |= (*buf++ & 0xFE) >> 1;
			}

			if (pPES_packet->ES_rate_flag)
			{
				pPES_packet->ES_rate = *buf++ & 0x7F;
				pPES_packet->ES_rate <<= 8;
				pPES_packet->ES_rate |= *buf++;
				pPES_packet->ES_rate <<= 7;
				pPES_packet->ES_rate |= (*buf++ & 0xFE) >> 1;
			}

			if (pPES_packet->DSM_trick_mode_flag)
			{
				pPES_packet->trick_mode_control = (*buf & 0xE0) >> 5;

				if (pPES_packet->trick_mode_control == FAST_FORWARD) {

					pPES_packet->field_id = (*buf & 0x18) >> 3;
					pPES_packet->intra_slice_refresh = (*buf & 0x04) >> 2;
					pPES_packet->frequency_truncation = *buf++ & 0x03;
				}
				else if (pPES_packet->trick_mode_control == SLOW_MOTION) {

					pPES_packet->rep_cntrl = *buf++ & 0x1F;
				}
				else if (pPES_packet->trick_mode_control == FREEZE_FRAME) {

					pPES_packet->field_id = (*buf++ & 0x18) >> 3;
				}
				else if (pPES_packet->trick_mode_control == FAST_REVERSE) {

					pPES_packet->field_id = (*buf & 0x18) >> 3;
					pPES_packet->intra_slice_refresh = (*buf & 0x04) >> 2;
					pPES_packet->frequency_truncation = *buf++ & 0x03;
				}
				else if (pPES_packet->trick_mode_control == SLOW_REVERSE) {

					pPES_packet->rep_cntrl = *buf++ & 0x1F;
				}
				else {
					buf++;
				}
			}

			if (pPES_packet->additional_copy_info_flag) {

				pPES_packet->additional_copy_info = *buf++ & 0x7F;
			}

			if (pPES_packet->PES_CRC_flag) {

				pPES_packet->previous_PES_packet_CRC = *buf++;
				pPES_packet->previous_PES_packet_CRC <<= 8;
				pPES_packet->previous_PES_packet_CRC |= *buf++;
			}

			if (pPES_packet->PES_extension_flag) {

				pPES_packet->PES_private_data_flag = (*buf & 0x80) >> 7;
				pPES_packet->pack_header_field_flag = (*buf & 0x40) >> 6;
				pPES_packet->program_packet_sequence_counter_flag = (*buf & 0x20) >> 5;
				pPES_packet->P_STD_buffer_flag = (*buf & 0x10) >> 4;
				pPES_packet->PES_extension_flag_2 = *buf++ & 0x01;

				if (pPES_packet->PES_private_data_flag) {

					memcpy(pPES_packet->PES_private_data, buf, 16);
					buf += 16;
				}
				if (pPES_packet->pack_header_field_flag) {

					pPES_packet->pack_field_length = *buf++;
					pPES_packet->ppack_header = buf;

					buf += pPES_packet->pack_field_length;
				}
				if (pPES_packet->program_packet_sequence_counter_flag) {

					pPES_packet->program_packet_sequence_counter = *buf++ & 0x7F;
					pPES_packet->MPEG1_MPEG2_identifier = (*buf & 0x40) >> 6;
					pPES_packet->original_stuff_length = *buf++ & 0x3F;
				}
				if (pPES_packet->P_STD_buffer_flag) {

					pPES_packet->P_STD_buffer_scale = (*buf & 20) >> 5;
					pPES_packet->P_STD_buffer_size = *buf++ & 0x1F;
					pPES_packet->P_STD_buffer_size <<= 8;
					pPES_packet->P_STD_buffer_size |= *buf++;
				}
				if (pPES_packet->PES_extension_flag_2) {

					pPES_packet->PES_extension_field_length = *buf++ & 0x7F;
					buf += pPES_packet->PES_extension_field_length;
				}

			}

			if (pPES_packet->PES_packet_length == 0x00) {// video

														 //			pPES_packet->pPES_packet_data_byte = NULL;
			}
			else {//audio, video or other stream

				  //			pPES_packet->pPES_packet_data_byte = buf;
				buf += pPES_packet->PES_packet_length - pPES_packet->PES_header_data_length - 3;
			}
		}
		else if (pPES_packet->stream_id == PROGRAM_STREAM_MAP ||
			pPES_packet->stream_id == PRIVATE_STREAM_2 ||
			pPES_packet->stream_id == ECM_STREAM ||
			pPES_packet->stream_id == EMM_STREAM ||
			pPES_packet->stream_id == PROGRAM_STREAM_DIRECTORY ||
			pPES_packet->stream_id == DSMCC_STREAM ||
			pPES_packet->stream_id == TYPE_E_STREAM)
		{

			//		pPES_packet->pPES_packet_data_byte = buf;
			buf += pPES_packet->PES_packet_length;
		}
		else if (pPES_packet->stream_id == PADDING_STREAM)
		{

			//		pPES_packet->pPES_packet_data_byte = NULL;
			buf += pPES_packet->PES_packet_length;
		}
		else
		{
			//		pPES_packet->pPES_packet_data_byte = NULL;
			buf += pPES_packet->PES_packet_length;
		}

		//	pPES_packet->pts_pos = pts_pos;
		//	pPES_packet->dts_pos = dts_pos;	
	}
	else
	{
		rtcode = PES_PACKET_PARAMETER_ERROR;
	}

	return rtcode;
}

