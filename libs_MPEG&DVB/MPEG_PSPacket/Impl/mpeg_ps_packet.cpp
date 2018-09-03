#include <stdio.h>
#include <string.h>
#include "../Include/mpeg_ps_packet.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

S32 mpeg_ps_decode_pack_header(U8 *buf, S32* plength, pMPEG_pack_header_t ppack_header)
{
	BITS_t		bs;
	S32			i;
	U8			nextbits;

	if ((buf != NULL) && (ppack_header != NULL))
	{
		BITS_map(&bs, buf, *plength);

		ppack_header->pack_start_code = BITS_get(&bs, 32);

		nextbits = BITS_show(&bs, 4);
		if ((nextbits & 0xf) == 0x2)					//"0010"		11172
		{
			ppack_header->u.mpeg1_ps.reserved0 = BITS_get(&bs, 4);
			ppack_header->u.mpeg1_ps.system_clock_reference_base_32_30 = BITS_get(&bs, 3);
			ppack_header->u.mpeg1_ps.marker_bit0 = BITS_get(&bs, 1);

			ppack_header->u.mpeg1_ps.system_clock_reference_base_29_15 = BITS_get(&bs, 15);
			ppack_header->u.mpeg1_ps.marker_bit1 = BITS_get(&bs, 1);

			ppack_header->u.mpeg1_ps.system_clock_reference_base_14_0 = BITS_get(&bs, 15);
			ppack_header->u.mpeg1_ps.marker_bit2 = BITS_get(&bs, 1);

			ppack_header->u.mpeg1_ps.marker_bit3 = BITS_get(&bs, 1);
			ppack_header->u.mpeg1_ps.program_mux_rate = BITS_get(&bs, 22);
			ppack_header->u.mpeg1_ps.marker_bit4 = BITS_get(&bs, 1);
		}
		else if ((nextbits & 0xc) == 0x4)				//"0100"		13818
		{
			ppack_header->u.mpeg2_ps.reserved0 = BITS_get(&bs, 2);
			ppack_header->u.mpeg2_ps.system_clock_reference_base_32_30 = BITS_get(&bs, 3);
			ppack_header->u.mpeg2_ps.marker_bit0 = BITS_get(&bs, 1);
			ppack_header->u.mpeg2_ps.system_clock_reference_base_29_15 = BITS_get(&bs, 15);
			ppack_header->u.mpeg2_ps.marker_bit1 = BITS_get(&bs, 1);
			ppack_header->u.mpeg2_ps.system_clock_reference_base_14_0 = BITS_get(&bs, 15);
			ppack_header->u.mpeg2_ps.marker_bit2 = BITS_get(&bs, 1);
			ppack_header->u.mpeg2_ps.system_clock_reference_extension = BITS_get(&bs, 9);
			ppack_header->u.mpeg2_ps.marker_bit3 = BITS_get(&bs, 1);

			ppack_header->u.mpeg2_ps.program_mux_rate = BITS_get(&bs, 22);
			ppack_header->u.mpeg2_ps.marker_bit4 = BITS_get(&bs, 1);
			ppack_header->u.mpeg2_ps.marker_bit5 = BITS_get(&bs, 1);

			ppack_header->u.mpeg2_ps.reserved1 = BITS_get(&bs, 5);
			ppack_header->u.mpeg2_ps.pack_stuffing_length = BITS_get(&bs, 3);

			for (i = 0; i < ppack_header->u.mpeg2_ps.pack_stuffing_length; i++)
			{
				ppack_header->u.mpeg2_ps.stuffing_byte[i] = BITS_get(&bs, 8);
			}
		}

		ppack_header->nextbits = nextbits;
	}

	*plength = (int)(bs.p_cur - bs.p_start);

	return 0;
}

S32 mpeg_ps_decode_system_header(U8* buf, S32* plength, pMPEG_system_header_t psystem_header)
{
	BITS_t		bs;
//	S32			i;
	S32			N;
	S8			nextbits;

	if ((buf != NULL) && (psystem_header != NULL))
	{
		BITS_map(&bs, buf, *plength);

		psystem_header->system_header_start_code = BITS_get(&bs, 32);

		psystem_header->header_length = BITS_get(&bs, 16);

		psystem_header->marker_bit0 = BITS_get(&bs, 1);
		psystem_header->rate_bound = BITS_get(&bs, 22);
		psystem_header->marker_bit1 = BITS_get(&bs, 1);

		psystem_header->audio_bound = BITS_get(&bs, 6);
		psystem_header->fixed_flag = BITS_get(&bs, 1);
		psystem_header->CSPS_flag = BITS_get(&bs, 1);

		psystem_header->system_audio_lock_flag = BITS_get(&bs, 1);
		psystem_header->system_video_lock_flag = BITS_get(&bs, 1);
		psystem_header->marker_bit2 = BITS_get(&bs, 1);
		psystem_header->video_bound = BITS_get(&bs, 5);

		psystem_header->packet_rate_restriction_flag = BITS_get(&bs, 1);
		psystem_header->reserved_bits = BITS_get(&bs, 7);

		N = 0;
		while ((nextbits = BITS_show(&bs, 1)) == 1)
		{
			psystem_header->stream_id[N] = BITS_get(&bs, 8);

			psystem_header->reserved[N] = BITS_get(&bs, 2);
			psystem_header->P_STD_buffer_bound_scale[N] = BITS_get(&bs, 1);
			psystem_header->P_STD_buffer_size_bound[N] = BITS_get(&bs, 13);

			N ++;
		}

		psystem_header->stream_count = N;
	}

	*plength = (int)(bs.p_cur - bs.p_start);

	return 0;
}

