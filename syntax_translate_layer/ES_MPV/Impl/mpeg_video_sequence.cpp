#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "../../common/BitStream.h"

#include "../Include/mpeg_video_sequence.h"
#include "../Include/mpeg_video_errorcode.h"


int mpgv_decode_sequence_header(uint8_t* nal_buf, int nal_length, MPGV_sequence_header_t* psequence_header)
{
	int		rtcode = MPV_NO_ERROR;
	int		i;
	BITS_t	bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (psequence_header != NULL))
	{
		memset(psequence_header, 0x00, sizeof(MPGV_sequence_header_t));
		psequence_header->nal_snapshot.buf = nal_buf;
		psequence_header->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		/*syntax part parse*/
		psequence_header->sequence_header_code = BITS_get(&bs, 32);
		psequence_header->horizontal_size_value = BITS_get(&bs, 12);
		psequence_header->vertical_size_value = BITS_get(&bs, 12);
		psequence_header->aspect_ratio_information = BITS_get(&bs, 4);
		psequence_header->frame_rate_code = BITS_get(&bs, 4);
		psequence_header->bit_rate_value = BITS_get(&bs, 18);
		psequence_header->marker_bit = BITS_get(&bs, 1);
		psequence_header->vbv_buffer_size_value = BITS_get(&bs, 10);
		psequence_header->constrained_parameters_flag = BITS_get(&bs, 1);
		psequence_header->load_intra_quantiser_matrix = BITS_get(&bs, 1);

		if (psequence_header->load_intra_quantiser_matrix)
		{
			for (i = 0; i < 64; i++)
			{
				psequence_header->intra_quantiser_matrix[i] = BITS_get(&bs, 8);
			}

			//for (v = 0; v < 8; v++)
			//{
			//	for (u = 0; u < 8; u++)
			//	{
			//		//W[0][v][u] = psequence_header->intra_quantiser_matrix[scan[0][v][u]];
			//	}
			//}
		}
		else		//use default quant matrix 
		{
			//memcpy(W[0], DW[0], 64 * sizeof(S16));
		}

		psequence_header->load_non_intra_quantiser_matrix = BITS_get(&bs, 1);

		if (psequence_header->load_non_intra_quantiser_matrix)
		{
			for (i = 0; i < 64; i++)
			{
				psequence_header->non_intra_quantiser_matrix[i] = BITS_get(&bs, 8);
			}

			//for (v = 0; v < 8; v++)
			//{
			//	for (u = 0; u < 8; u++)
			//	{
			//		W[1][v][u] = psequence_header->non_intra_quantiser_matrix[scan[0][v][u]];
			//	}
			//}
		}
		else
		{
			//memcpy(W[1], DW[1], 64 * sizeof(S16));
		}

		//	mpeg_video_next_start_code(pbs);

		//added by chendelin		20060730
		//memcpy(W[2], W[0], 64 * sizeof(S16));
		//memcpy(W[3], W[1], 64 * sizeof(S16));

		if (!BITS_beAligned(&bs))
		{
			psequence_header->padding_bits_length = bs.i_left;
			psequence_header->padding_bits_value = BITS_get(&bs, psequence_header->padding_bits_length);
		}

		if (!BITS_beEOF(&bs))
		{
			assert(BITS_beAligned(&bs));
			psequence_header->padding_length = (int)(bs.p_end - bs.p_cur);
			psequence_header->padding_bytes = bs.p_cur;
			BITS_byteSkip(&bs, psequence_header->padding_length);
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_decode_group_of_pictures_header(uint8_t* nal_buf, int nal_length, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header)
{
	int			rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pgroup_of_pictures_header != NULL))
	{
		memset(pgroup_of_pictures_header, 0x00, sizeof(MPGV_group_of_pictures_header_t));
		pgroup_of_pictures_header->nal_snapshot.buf = nal_buf;
		pgroup_of_pictures_header->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		pgroup_of_pictures_header->group_start_code = BITS_get(&bs, 32);			//bytes 0~3

		pgroup_of_pictures_header->drop_frame_flag = BITS_get(&bs, 1);
		pgroup_of_pictures_header->time_code_hours = BITS_get(&bs, 5);
		pgroup_of_pictures_header->time_code_minutes = BITS_get(&bs, 6);
		pgroup_of_pictures_header->marker_bit = BITS_get(&bs, 1);
		pgroup_of_pictures_header->time_code_seconds = BITS_get(&bs, 6);
		pgroup_of_pictures_header->time_code_pictures = BITS_get(&bs, 6);
		pgroup_of_pictures_header->closed_gop = BITS_get(&bs, 1);
		pgroup_of_pictures_header->broken_link = BITS_get(&bs, 1);

		if (!BITS_beAligned(&bs))
		{
			pgroup_of_pictures_header->padding_bits_length = bs.i_left;
			pgroup_of_pictures_header->padding_bits_value = BITS_get(&bs, pgroup_of_pictures_header->padding_bits_length);
		}

		if (!BITS_beEOF(&bs))
		{
			assert(BITS_beAligned(&bs));
			pgroup_of_pictures_header->padding_length = (int)(bs.p_end - bs.p_cur);
			pgroup_of_pictures_header->padding_bytes = bs.p_cur;
			BITS_byteSkip(&bs, pgroup_of_pictures_header->padding_length);
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_picture_header(uint8_t* nal_buf, int nal_length, MPGV_picture_header_t* ppicture_header)
{
	int		rtcode = MPV_NO_ERROR;
	BITS_t		bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (ppicture_header != NULL))
	{
		memset(ppicture_header, 0x00, sizeof(MPGV_picture_header_t));
		ppicture_header->nal_snapshot.buf = nal_buf;
		ppicture_header->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		ppicture_header->picture_start_code = BITS_get(&bs, 32);			//bytes 0~3

		ppicture_header->temporal_reference = BITS_get(&bs, 10);
		ppicture_header->picture_coding_type = BITS_get(&bs, 3);			//000	-- forbidden
																			//001	-- I frame
																			//010	-- P frame
																			//011	-- B frame
																			//100	-- DC intra-coded(D)
																			//101	-- reserved
																			//110	-- reserved
																			//111	-- reserved

		ppicture_header->vbv_delay = BITS_get(&bs, 16);

		if (ppicture_header->picture_coding_type == MPEG_P_FRAME || ppicture_header->picture_coding_type == MPEG_B_FRAME)
		{
			ppicture_header->full_pel_forward_vector = BITS_get(&bs, 1);
			ppicture_header->forward_f_code = BITS_get(&bs, 3);
		}

		if (ppicture_header->picture_coding_type == MPEG_B_FRAME)
		{
			ppicture_header->full_pel_backward_vector = BITS_get(&bs, 1);
			ppicture_header->backward_f_code = BITS_get(&bs, 3);
		}

		if (!BITS_beAligned(&bs))
		{
			ppicture_header->padding_bits_length = bs.i_left;
			ppicture_header->padding_bits_value = BITS_get(&bs, ppicture_header->padding_bits_length);
		}

		if (!BITS_beEOF(&bs))
		{
			assert(BITS_beAligned(&bs));
			ppicture_header->padding_length = (int)(bs.p_end - bs.p_cur);
			ppicture_header->padding_bytes = bs.p_cur;
			BITS_byteSkip(&bs, ppicture_header->padding_length);
		}
		//ppicture_header->extra_bit_picture = BITS_show(&bs, 1);

		//如果是mpeg-1视频，则在图像头后就进行解码帧重排序，否则要等到解picture_coding_extension之后
		//if (m_mpeg2_video_stream == 0)
		//{
		//	mpgv_decode_reorder_decode_frame(m_picture_header.picture_coding_type, m_picture_coding_extension.picture_structure);
		//}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_extension(uint8_t* nal_buf, int nal_length, MPGV_sequence_extension_t* psequence_extension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (psequence_extension != NULL))
	{
		memset(psequence_extension, 0x00, sizeof(MPGV_sequence_extension_t));
		psequence_extension->nal_snapshot.buf = nal_buf;
		psequence_extension->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		psequence_extension->extension_start_code = BITS_get(&bs, 32);			//bytes 0~3
		psequence_extension->extension_start_code_identifier = BITS_get(&bs, 4);		//byte 4

		psequence_extension->profile_and_level_indication = BITS_get(&bs, 8);
		psequence_extension->progressive_sequence = BITS_get(&bs, 1);
		psequence_extension->chroma_format = BITS_get(&bs, 2);
		psequence_extension->horizontal_size_extension = BITS_get(&bs, 2);
		psequence_extension->vertical_size_extension = BITS_get(&bs, 2);
		psequence_extension->bit_rate_extension = BITS_get(&bs, 12);					//byte8
		psequence_extension->marker_bit = BITS_get(&bs, 1);

		psequence_extension->vbv_buffer_size_extension = BITS_get(&bs, 8);	//byte 10

		psequence_extension->low_delay = BITS_get(&bs, 1);
		psequence_extension->frame_rate_extension_n = BITS_get(&bs, 2);
		psequence_extension->frame_rate_extension_d = BITS_get(&bs, 5);

		if (!BITS_beAligned(&bs))
		{
			psequence_extension->padding_bits_length = bs.i_left;
			psequence_extension->padding_bits_value = BITS_get(&bs, psequence_extension->padding_bits_length);
		}

		if (!BITS_beEOF(&bs))
		{
			assert(BITS_beAligned(&bs));
			psequence_extension->padding_length = (int)(bs.p_end - bs.p_cur);
			psequence_extension->padding_bytes = bs.p_cur;
			BITS_byteSkip(&bs, psequence_extension->padding_length);
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_picture_coding_extension(uint8_t* nal_buf, int nal_length, MPGV_picture_coding_extension_t* ppicture_coding_extension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (ppicture_coding_extension != NULL))
	{
		memset(ppicture_coding_extension, 0x00, sizeof(MPGV_picture_coding_extension_t));
		ppicture_coding_extension->nal_snapshot.buf = nal_buf;
		ppicture_coding_extension->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		ppicture_coding_extension->extension_start_code = BITS_get(&bs, 32);

		ppicture_coding_extension->extension_start_code_identifier = BITS_get(&bs, 4);

		ppicture_coding_extension->f_code[0][0] = BITS_get(&bs, 4);
		ppicture_coding_extension->f_code[0][1] = BITS_get(&bs, 4);
		ppicture_coding_extension->f_code[1][0] = BITS_get(&bs, 4);
		ppicture_coding_extension->f_code[1][1] = BITS_get(&bs, 4);

		ppicture_coding_extension->intra_dc_precision = BITS_get(&bs, 2);
		ppicture_coding_extension->picture_structure = BITS_get(&bs, 2);

		ppicture_coding_extension->top_field_first = BITS_get(&bs, 1);
		ppicture_coding_extension->frame_pred_frame_dct = BITS_get(&bs, 1);
		ppicture_coding_extension->concealment_motion_vectors = BITS_get(&bs, 1);
		ppicture_coding_extension->q_scale_type = BITS_get(&bs, 1);
		ppicture_coding_extension->intra_vlc_format = BITS_get(&bs, 1);
		ppicture_coding_extension->alternate_scan = BITS_get(&bs, 1);
		ppicture_coding_extension->repeat_first_field = BITS_get(&bs, 1);
		ppicture_coding_extension->chroma_420_type = BITS_get(&bs, 1);

		ppicture_coding_extension->progressive_frame = BITS_get(&bs, 1);
		ppicture_coding_extension->composite_display_flag = BITS_get(&bs, 1);

		if (ppicture_coding_extension->composite_display_flag)
		{
			ppicture_coding_extension->v_axis = BITS_get(&bs, 1);
			ppicture_coding_extension->field_sequence = BITS_get(&bs, 3);
			ppicture_coding_extension->sub_carrier = BITS_get(&bs, 1);

			ppicture_coding_extension->burst_amplitude = BITS_get(&bs, 7);
			ppicture_coding_extension->sub_carrier_phase = BITS_get(&bs, 8);
		}

		if (!BITS_beAligned(&bs))
		{
			ppicture_coding_extension->padding_bits_length = bs.i_left;
			ppicture_coding_extension->padding_bits_value = BITS_get(&bs, ppicture_coding_extension->padding_bits_length);
		}

		if (!BITS_beEOF(&bs))
		{
			assert(BITS_beAligned(&bs));
			ppicture_coding_extension->padding_length = (int)(bs.p_end - bs.p_cur);
			ppicture_coding_extension->padding_bytes = bs.p_cur;
			BITS_byteSkip(&bs, ppicture_coding_extension->padding_length);
		}

		//如果是mpeg-2视频，在此重排序解码帧缓存，否则需要在解picture_header时就要重排序
		//if (m_mpeg2_video_stream == 1)
		//{
		//	mpgv_decode_reorder_decode_frame(m_picture_header.picture_coding_type, m_picture_coding_extension.picture_structure);
		//}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_display_extension(uint8_t* nal_buf, int nal_length, MPGV_sequence_display_extension_t* psequence_display_extension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (psequence_display_extension != NULL))
	{
		memset(psequence_display_extension, 0x00, sizeof(MPGV_sequence_display_extension_t));
		psequence_display_extension->nal_snapshot.buf = nal_buf;
		psequence_display_extension->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		psequence_display_extension->extension_start_code = BITS_get(&bs, 32);			//bytes 0~3

		psequence_display_extension->extension_start_code_identifier = BITS_get(&bs, 4);		//byte 4
		psequence_display_extension->video_format = BITS_get(&bs, 3);
		psequence_display_extension->colour_description = BITS_get(&bs, 1);

		if (psequence_display_extension->colour_description)
		{
			psequence_display_extension->colour_primaries = BITS_get(&bs, 8);

			psequence_display_extension->transfer_characteristics = BITS_get(&bs, 8);

			psequence_display_extension->matrix_coefficients = BITS_get(&bs, 8);
		}

		psequence_display_extension->display_horizontal_size = BITS_get(&bs, 14);
		psequence_display_extension->marker_bit = BITS_get(&bs, 1);
		psequence_display_extension->display_vertical_size = BITS_get(&bs, 14);

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_decode_user_data(uint8_t* nal_buf, int nal_length, MPGV_user_data_t* puser_data)
{
	int rtcode = MPV_NO_ERROR;
	BITS_t			bs;

	if ((nal_buf != NULL) && (nal_length >= 4) && (puser_data != NULL))
	{
		memset(puser_data, 0x00, sizeof(MPGV_user_data_t));

		puser_data->nal_snapshot.buf = nal_buf;
		puser_data->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		puser_data->user_data_start_code = BITS_get(&bs, 32);

		puser_data->user_data_length = nal_length - 4;
		puser_data->user_data_buf = bs.p_cur;
		BITS_byteSkip(&bs, puser_data->user_data_length);
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_slice(uint8_t* nal_buf, int nal_length, MPGV_slice_t* pslice)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t	 bs;
	//int		 quantiser_scale_code;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pslice != NULL))
	{
		memset(pslice, 0x00, sizeof(MPGV_slice_t));

		pslice->nal_snapshot.buf = nal_buf;
		pslice->nal_snapshot.length = nal_length;

		BITS_map(&bs, nal_buf, nal_length);

		pslice->slice_start_code = BITS_get(&bs, 32);
		pslice->slice_vertical_position = (pslice->slice_start_code & 0x000000ff);

		if (!BITS_beAligned(&bs))
		{
			pslice->padding_bits_length = bs.i_left;
			pslice->padding_bits_value = BITS_get(&bs, pslice->padding_bits_length);
		}

		if (!BITS_beEOF(&bs))
		{
			assert(BITS_beAligned(&bs));
			pslice->padding_length = (int)(bs.p_end - bs.p_cur);
			pslice->padding_bytes = bs.p_cur;
			BITS_byteSkip(&bs, pslice->padding_length);
		}

//		if (gbInitDecoder
//#if MPEG_VID_DECODER_I
//			&& (m_picture_header.picture_coding_type == MPEG_I_FRAME))
//#elif MPEG_VID_DECODER_P
//			&& (m_picture_header.picture_coding_type == MPEG_P_FRAME))
//#elif MPEG_VID_DECODER_B 
//			&& (m_picture_header.picture_coding_type == MPEG_B_FRAME))
//#elif MPEG_VID_DECODER_IP 
//			&& ((m_picture_header.picture_coding_type == MPEG_I_FRAME) ||
//				(m_picture_header.picture_coding_type == MPEG_P_FRAME)))
//#else
//			)
//#endif
//		{
//			//		if (m_slice.slice_vertical_position < 2)
//			{
//				if (m_sequence_header.vertical_size_value > 2800)			//vertical_size is a member variable, derived from m_sequence_header and m_sequence_extension
//				{
//					m_slice.slice_vertical_position = (BITS_get(&bs, 3) << 7) + m_slice.slice_vertical_position;
//				}
//
//				if (m_find_sequence_scalable_extension)
//				{
//					if (m_sequence_scalable_extension.scalable_mode == MPEG_DATA_PARTITIONING)		//data partitioning
//					{
//						//priority breakpoint
//						BITS_skip(&bs, 7);
//					}
//				}
//
//				quantiser_scale_code = BITS_get(&bs, 5);
//				m_quantiser_scale = LUT_quantiser_scale[m_picture_coding_extension.q_scale_type][quantiser_scale_code];
//
//#if MPEG_VID_DECODER_DEBUG
//				if (m_hDebugFile)
//				{
//					fprintf(m_hDebugFile, " [SLICE]quantiser_scale_code = %d\n", quantiser_scale_code);
//				}
//#endif
//
//				if (BITS_show(&bs, 1))
//				{
//					//intra_slice_flag
//					BITS_skip(&bs, 1);
//					//intra_slice
//					BITS_skip(&bs, 1);
//
//					// slice_id introduced in March 1995 as part of the video corridendum
//					// (after the IS was drafted in November 1994) 
//					BITS_skip(&bs, 1);
//					BITS_skip(&bs, 6);
//
//					while (BITS_show(&bs, 1))
//					{
//						//extra_bit_slice
//						BITS_skip(&bs, 1);
//						//extra_information_slice
//						BITS_skip(&bs, 8);
//					}
//				}
//
//				//extra_bit_slice
//				BITS_skip(&bs, 1);
//
//				m_macroblock.previous_macroblock_address = (m_slice.slice_vertical_position - 1) * m_VidDecodeInfo.mb_width - 1;		//at the start of a slice
//				m_macroblock.max_mb_address_in_slice = (m_slice.slice_vertical_position * m_VidDecodeInfo.mb_width);
//
//				//片的开始处, 初始化预测器
//
//				/* ISO/IEC 13818-2 section 7.2.1 */
//				m_macroblock.dc_dct_pred[CC_Y] = m_macroblock.dc_dct_pred[CC_Cb] = m_macroblock.dc_dct_pred[CC_Cr] = LUT_dc_dct_pred[m_picture_coding_extension.intra_dc_precision];
//
//				/* ISO/IEC 13818-2 section 7.6.3.4 */
//				//forward
//				m_macroblock.PMV[0][0][0] = m_macroblock.PMV[0][0][1] = m_macroblock.PMV[1][0][0] = m_macroblock.PMV[1][0][1] = 0;
//				//backward
//				m_macroblock.PMV[0][1][0] = m_macroblock.PMV[0][1][1] = m_macroblock.PMV[1][1][0] = m_macroblock.PMV[1][1][1] = 0;
//
//				//		uint32_t		nextbits;
//				do
//				{
//					rtcode = mpgv_decode_macroblock(&bs);
//
//					//			nextbits = BITS_show(pbs, 23);
//					//			if (nextbits == 0)
//					//			{
//					//				break;
//					//			}
//				} while ((rtcode >= 0) && (BITS_show(&bs, 23) != 0));
//				//			} while ((rtcode >= 0) && (bs.p < bs.p_end));
//				//		} while (rtcode >= 0);
//			}
//
//			//		mpeg_video_next_start_code(pbs);
//
//			if (m_macroblock.macroblock_address >= (m_VidDecodeInfo.mb_count - 1))		//reach the last macroblock
//			{
//				mpgv_decode_reorder_display_frame(m_picture_header.picture_coding_type);
//
//#if MPEG_VID_FRAMERATE_DEBUG
//				if (m_hFrameRateFile != NULL)
//				{
//					newtime = timeGetTime();
//					fprintf(m_hFrameRateFile, "frame = %d, picture_coding_type = %d, picture_structure = %d, decoding_time = %d(ms)\n",
//						m_nDebugFrameCount,
//						m_picture_header.picture_coding_type,
//						m_picture_coding_extension.picture_structure,
//						newtime - oldtime);
//				}
//#endif
//			}
//		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

