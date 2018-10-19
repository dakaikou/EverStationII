#include <string.h>
#include <assert.h>

#include "../Include/MPEG_PES_ErrorCode.h"
#include "../Include/MPEG_PES_packet.h"
#include "../Include/MPEG_stream_id.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int	MPEG_decode_PES_packet(uint8_t *pes_buf, int pes_size, PES_packet_t* pPES_packet)
{
	int rtcode = PES_PACKET_NO_ERROR;

	if ((pes_buf != NULL) && (pes_size > 4) && (pPES_packet != NULL))
	{
		memset(pPES_packet, 0x00, sizeof(PES_packet_t));

		BITS_t bs;
		BITS_map(&bs, pes_buf, pes_size);

		pPES_packet->packet_start_code_prefix = BITS_get(&bs, 24);

		pPES_packet->stream_id = BITS_get(&bs, 8);

		pPES_packet->PES_packet_length = BITS_get(&bs, 16);

		pPES_packet->PES_packet_data_length = pes_size - 6;
		pPES_packet->PES_packet_data_byte = bs.p_cur;

		if ((pPES_packet->stream_id != PROGRAM_STREAM_MAP) &&
			(pPES_packet->stream_id != PADDING_STREAM) &&
			(pPES_packet->stream_id != PRIVATE_STREAM_2) &&
			(pPES_packet->stream_id != ECM_STREAM) &&
			(pPES_packet->stream_id != EMM_STREAM) &&
			(pPES_packet->stream_id != PROGRAM_STREAM_DIRECTORY) &&
			(pPES_packet->stream_id != DSMCC_STREAM) &&
			(pPES_packet->stream_id != TYPE_E_STREAM))
		{
			pPES_packet->mpeg_flag = BITS_get(&bs, 2);
			pPES_packet->PES_scrambling_control = BITS_get(&bs, 2);
			pPES_packet->PES_priority = BITS_get(&bs, 1);
			pPES_packet->data_alignment_indicator = BITS_get(&bs, 1);
			pPES_packet->copyright = BITS_get(&bs, 1);
			pPES_packet->original_or_copy = BITS_get(&bs, 1);

			pPES_packet->PTS_DTS_flags = BITS_get(&bs, 2);
			pPES_packet->ESCR_flag = BITS_get(&bs, 1);
			pPES_packet->ES_rate_flag = BITS_get(&bs, 1);
			pPES_packet->DSM_trick_mode_flag = BITS_get(&bs, 1);
			pPES_packet->additional_copy_info_flag = BITS_get(&bs, 1);
			pPES_packet->PES_CRC_flag = BITS_get(&bs, 1);
			pPES_packet->PES_extension_flag = BITS_get(&bs, 1);

			pPES_packet->PES_header_data_length = BITS_get(&bs, 8);

			if ((pPES_packet->PTS_DTS_flags & 0b10) == 0b10)			//have PTS
			{
				pPES_packet->PTS_marker = BITS_get(&bs, 4);
				assert((pPES_packet->PTS_marker == 0b0010) || (pPES_packet->PTS_marker == 0b0011));
				pPES_packet->PTS_32_30 = BITS_get(&bs, 3);
				pPES_packet->PTS_marker_bit1 = BITS_get(&bs, 1);

				pPES_packet->PTS_29_15 = BITS_get(&bs, 15);
				pPES_packet->PTS_marker_bit2 = BITS_get(&bs, 1);

				pPES_packet->PTS_14_0 = BITS_get(&bs, 15);
				pPES_packet->PTS_marker_bit3 = BITS_get(&bs, 1);

				if ((pPES_packet->PTS_DTS_flags & 0b01) == 0b01)			// DTS
				{
					pPES_packet->DTS_marker = BITS_get(&bs, 4);
					pPES_packet->DTS_32_30 = BITS_get(&bs, 3);
					pPES_packet->DTS_marker_bit1 = BITS_get(&bs, 1);

					pPES_packet->DTS_29_15 = BITS_get(&bs, 15);
					pPES_packet->DTS_marker_bit2 = BITS_get(&bs, 1);

					pPES_packet->DTS_14_0 = BITS_get(&bs, 15);
					pPES_packet->DTS_marker_bit3 = BITS_get(&bs, 1);
				}
			}

			if (pPES_packet->ESCR_flag)
			{
				pPES_packet->ESCR_reserved = BITS_get(&bs, 2);
				pPES_packet->ESCR_base_32_30 = BITS_get(&bs, 3);
				pPES_packet->ESCR_marker_bit0 = BITS_get(&bs, 1);
				pPES_packet->ESCR_base_29_15 = BITS_get(&bs, 15);
				pPES_packet->ESCR_marker_bit1 = BITS_get(&bs, 1);
				pPES_packet->ESCR_base_14_0 = BITS_get(&bs, 15);
				pPES_packet->ESCR_marker_bit2 = BITS_get(&bs, 1);
				pPES_packet->ESCR_extension = BITS_get(&bs, 9);
				pPES_packet->ESCR_marker_bit3 = BITS_get(&bs, 1);
			}

			if (pPES_packet->ES_rate_flag)
			{
				pPES_packet->ES_rate_marker_bit0 = BITS_get(&bs, 1);
				pPES_packet->ES_rate = BITS_get(&bs, 22);
				pPES_packet->ES_rate_marker_bit1 = BITS_get(&bs, 1);
			}

			if (pPES_packet->DSM_trick_mode_flag)
			{
				pPES_packet->trick_mode_control = BITS_get(&bs, 3);

				if (pPES_packet->trick_mode_control == FAST_FORWARD) {

					pPES_packet->trick_mode.fast_forward.field_id = BITS_get(&bs, 2);
					pPES_packet->trick_mode.fast_forward.intra_slice_refresh = BITS_get(&bs, 1);
					pPES_packet->trick_mode.fast_forward.frequency_truncation = BITS_get(&bs, 2);
				}
				else if (pPES_packet->trick_mode_control == SLOW_MOTION) {

					pPES_packet->trick_mode.slow_motion.rep_cntrl = BITS_get(&bs, 5);
				}
				else if (pPES_packet->trick_mode_control == FREEZE_FRAME) {

					pPES_packet->trick_mode.freeze_frame.field_id = BITS_get(&bs, 2);
					pPES_packet->trick_mode.freeze_frame.reserved = BITS_get(&bs, 3);
				}
				else if (pPES_packet->trick_mode_control == FAST_REVERSE) {

					pPES_packet->trick_mode.fast_reverse.field_id = BITS_get(&bs, 2);
					pPES_packet->trick_mode.fast_reverse.intra_slice_refresh = BITS_get(&bs, 1);
					pPES_packet->trick_mode.fast_reverse.frequency_truncation = BITS_get(&bs, 2);
				}
				else if (pPES_packet->trick_mode_control == SLOW_REVERSE) {

					pPES_packet->trick_mode.slow_reverse.rep_cntrl = BITS_get(&bs, 5);
				}
				else 
				{
					pPES_packet->trick_mode.reserved = BITS_get(&bs, 5);
				}
			}

			if (pPES_packet->additional_copy_info_flag) {

				pPES_packet->additional_copy_info_marker_bit = BITS_get(&bs, 1);
				pPES_packet->additional_copy_info = BITS_get(&bs, 7);
			}

			if (pPES_packet->PES_CRC_flag) {

				pPES_packet->previous_PES_packet_CRC = BITS_get(&bs, 16);
			}

			if (pPES_packet->PES_extension_flag) {

				pPES_packet->PES_private_data_flag = BITS_get(&bs, 1);
				pPES_packet->pack_header_field_flag = BITS_get(&bs, 1);
				pPES_packet->program_packet_sequence_counter_flag = BITS_get(&bs, 1);
				pPES_packet->P_STD_buffer_flag = BITS_get(&bs, 1);
				pPES_packet->PES_extension_reserved = BITS_get(&bs, 3);
				pPES_packet->PES_extension_flag_2 = BITS_get(&bs, 1);

				if (pPES_packet->PES_private_data_flag) {

					pPES_packet->PES_private_data_byte = bs.p_cur;
					pPES_packet->PES_private_data_length = 16;
					BITS_byteSkip(&bs, 16);
				}
				if (pPES_packet->pack_header_field_flag) {

					pPES_packet->pack_field_length = BITS_get(&bs, 8);
					if (pPES_packet->pack_field_length > 0)
					{
						pPES_packet->pack_header_data_byte = bs.p_cur;
						BITS_byteSkip(&bs, pPES_packet->pack_field_length);
					}
				}
				if (pPES_packet->program_packet_sequence_counter_flag) {

					pPES_packet->program_packet_marker_bit0 = BITS_get(&bs, 1);
					pPES_packet->program_packet_sequence_counter = BITS_get(&bs, 7);
					pPES_packet->program_packet_marker_bit1 = BITS_get(&bs, 1);
					pPES_packet->MPEG1_MPEG2_identifier = BITS_get(&bs, 1);
					pPES_packet->original_stuff_length = BITS_get(&bs, 6);
				}
				if (pPES_packet->P_STD_buffer_flag) {

					pPES_packet->P_STD_marker = BITS_get(&bs, 2);
					pPES_packet->P_STD_buffer_scale = BITS_get(&bs, 1);
					pPES_packet->P_STD_buffer_size = BITS_get(&bs, 13);
				}
				if (pPES_packet->PES_extension_flag_2) {
					pPES_packet->PES_extension_field_marker_bit = BITS_get(&bs, 1);
					pPES_packet->PES_extension_field_length = BITS_get(&bs, 7);
					pPES_packet->PES_extension_field_data_byte = bs.p_cur;
					BITS_byteSkip(&bs, pPES_packet->PES_extension_field_length);
				}
			}

			int parsed_length = (int)(bs.p_cur - pes_buf);
			pPES_packet->stuffing_length = (6 + 3 + pPES_packet->PES_header_data_length) - parsed_length;
			if (pPES_packet->stuffing_length > 0)
			{
				pPES_packet->stuffing_byte = bs.p_cur;
				BITS_byteSkip(&bs, pPES_packet->stuffing_length);
			}
		}

		pPES_packet->payload_buf = bs.p_cur;
		pPES_packet->payload_length = pes_size - (int)(pPES_packet->payload_buf - pes_buf);
		BITS_byteSkip(&bs, pPES_packet->payload_length);

		//else if (pPES_packet->stream_id == PROGRAM_STREAM_MAP ||
		//	pPES_packet->stream_id == PRIVATE_STREAM_2 ||
		//	pPES_packet->stream_id == ECM_STREAM ||
		//	pPES_packet->stream_id == EMM_STREAM ||
		//	pPES_packet->stream_id == PROGRAM_STREAM_DIRECTORY ||
		//	pPES_packet->stream_id == DSMCC_STREAM ||
		//	pPES_packet->stream_id == TYPE_E_STREAM)
		//{
		//	assert(0);
		//	pPES_packet->payload_buf = bs.p_cur;
		//	pPES_packet->payload_length = pes_size - (int)(pPES_packet->payload_buf = pes_buf);
		//	BITS_byteSkip(&bs, pPES_packet->payload_length);
		//}
		//else if (pPES_packet->stream_id == PADDING_STREAM)
		//{
		//	assert(0);
		//	pPES_packet->PES_packet_data_byte = bs.p_cur;
		//	BITS_byteSkip(&bs, pPES_packet->PES_packet_length);
		//}
		//else
		//{
		//	assert(0);
		//	pPES_packet->PES_packet_data_byte = bs.p_cur;
		//	BITS_byteSkip(&bs, pPES_packet->PES_packet_length);
		//}
	}
	else
	{
		rtcode = PES_PACKET_PARAMETER_ERROR;
	}

	return rtcode;
}

