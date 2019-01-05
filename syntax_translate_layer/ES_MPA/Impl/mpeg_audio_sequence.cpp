#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../common/BitStream.h"

#include "../Include/mpeg_audio_sequence.h"
#include "../Include/mpeg_audio_errorcode.h"

#include "./kernel/Mpeg_alloctables.h"
#include "./kernel/Mpeg_audiotables.h"
#include "./kernel/Mpeg_decodetable.h"
#include "./kernel/Mpeg_filtertable.h"


static AllocTablePtr alloc_table_table[4] =
{
    (AllocTablePtr) & alloc_0,
    (AllocTablePtr) & alloc_1,
    (AllocTablePtr) & alloc_2,
    (AllocTablePtr) & alloc_3
};

static int sblims[4] =
{
    27, 30, 8, 12
};

static int which_table[3][2][16] =			//[sampling_frequency][stero mode][bitrate index]
{
    {
        {0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0},			/*stero mode*/
        {0, 2, 2, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}},			/*signal channel mode*/
    {
        {0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {
        {0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 1, 1, 1, 1, 1, 0},
        {0, 3, 3, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}}
};

static int slen[2][16] =
{
    {0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
    {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}
};

//static Audio_decode_info_t				decode_info;

//static	MpegAudioSynData_t				lv, rv;
//static 	AudioStream_t				    left, right, merge;

//static	MPEG1_layer1_audio_data_t		l1_data[2];
//static 	MPEG1_layer2_audio_data_t		l2_data[2];
//static 	MPEG1_layer3_audio_data_t		l3_data[2];

//	uint8_t*								m_pDataBuf;
//	int								m_nDataSize;
	/*}}} ported from GUN project		*/

//int mpeg_audio_open(Audio_decode_info_t* pdecode_info)
//{
//	int		nSampleBytesPerFrame;
//
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	if (gbInitDecoder == FALSE)
//	{
//		switch (header.layer)
//		{
//		case MPEG_AUDIO_LAYER_1:
//			nSampleBytesPerFrame = 384;
//			break;
//		case MPEG_AUDIO_LAYER_2:
//			nSampleBytesPerFrame = 1152;
//			break;
//		default:
//			nSampleBytesPerFrame = 1152;
//			break;
//		}
//
//		AudioNew(&left, nSampleBytesPerFrame);
//		AudioNew(&right, nSampleBytesPerFrame);
//		AudioNew(&merge, nSampleBytesPerFrame * 2);
//
//		lv.offset = 64;
//		memset(lv.syn, 0x00, sizeof(lv.syn));
//
//		rv.offset = 64;
//		memset(lv.syn, 0x00, sizeof(lv.syn));
//
//		gbInitDecoder = TRUE;
//	}
//
//	pdecode_info->nch = header.nch;
//	pdecode_info->sampling_rate = header.sampling_rate;
//	pdecode_info->bitspersample = header.bitspersample;
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//int mpeg_audio_close(void)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	if (gbInitDecoder == TRUE)
//	{
//		AudioFree(&left);
//		AudioFree(&right);
//		AudioFree(&merge);
//
//		gbInitDecoder = FALSE;
//	}
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}
//
//M2A_header_t* mpeg_audio_get_header(void)
//{
//	return &header;
//}

/*********************************************************************************
	函数：Decode_header()
	描述：解析MPEG音频数据流，要求数据必须帧对齐
	输入：pbs				-- 音频数据流
		  pheader			-- 参考的信息头数据，并将解析后的数据返回
	返回：0					-- 信息头未发生变化
		  1					-- 信息头发生变化
*********************************************************************************/
int	mpeg_audio_decode_frame_header(unsigned char* es_header_buf, int es_header_length, MPA_header_t* pmpa_header)
{
	BITS_t	bs;
	int		rtcode = MPA_UNKNOWN_ERROR;
	int     header_size = 4;

	if ((es_header_buf != NULL) && (es_header_length >= 4) && (pmpa_header != NULL))
	{
		memset(pmpa_header, 0x00, sizeof(MPA_header_t));
		BITS_map(&bs, es_header_buf, es_header_length);

		pmpa_header->syncword = BITS_get(&bs, 12);
		pmpa_header->ID = BITS_get(&bs, 1);
		pmpa_header->layer = BITS_get(&bs, 2);
		pmpa_header->protection_bit = BITS_get(&bs, 1);

		pmpa_header->bitrate_index = BITS_get(&bs, 4);
		pmpa_header->sampling_frequency = BITS_get(&bs, 2);
		pmpa_header->padding_bit = BITS_get(&bs, 1);
		pmpa_header->private_bit = BITS_get(&bs, 1);

		pmpa_header->mode = BITS_get(&bs, 2);
		pmpa_header->mode_extension = BITS_get(&bs, 2);
		pmpa_header->copyright = BITS_get(&bs, 1);
		pmpa_header->original_or_copy = BITS_get(&bs, 1);
		pmpa_header->emphasis = BITS_get(&bs, 2);

		/*sematic part*/
		pmpa_header->layer_index = 3 - pmpa_header->layer;
		pmpa_header->bit_rate = MPA_bit_rate_table[pmpa_header->ID][pmpa_header->layer_index][pmpa_header->bitrate_index];
		pmpa_header->sampling_rate = MPA_sampling_rate_table[pmpa_header->ID][pmpa_header->sampling_frequency];

		if (pmpa_header->protection_bit == 0)
		{
			pmpa_header->crc_check = BITS_get(&bs, 16);

			pmpa_header->crc_length = 2;
			header_size += 2;
		}
		else
		{
			pmpa_header->crc_length = 0;
		}

		pmpa_header->num_of_slots = (int)(144 * pmpa_header->bit_rate / pmpa_header->sampling_rate) + pmpa_header->padding_bit;
		pmpa_header->data_length = pmpa_header->num_of_slots - header_size;

		pmpa_header->bitspersample = MPEG_AUDIO_BITS_PER_SAMPLE;

		if (pmpa_header->mode == MPEG_AUDIO_SINGLE_CHANNEL)				//singla channel
		{
			pmpa_header->nch = 1;
		}
		else
		{
			pmpa_header->nch = 2;
		}

		rtcode = MPA_NO_ERROR;
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


/*********************************************************************************
函数：Decode_header()
描述：解析MPEG音频数据流，要求数据必须帧对齐
输入：pbs				-- 音频数据流
pheader			-- 参考的信息头数据，并将解析后的数据返回
返回：0					-- 信息头未发生变化
1					-- 信息头发生变化
*********************************************************************************/
int	mpga_decode_frame(uint8_t* frame_buf, int frame_size, MPA_frame_t* pmpa_frame)
{
	BITS_t	bs;
	int		rtcode = MPA_UNKNOWN_ERROR;
	int     header_size = 4;

	if ((frame_buf != NULL) && (frame_size >= 4) && (pmpa_frame != NULL))
	{
		memset(pmpa_frame, 0x00, sizeof(MPA_frame_t));

		pmpa_frame->snapshot.buf = frame_buf;
		pmpa_frame->snapshot.length = frame_size;

		BITS_map(&bs, frame_buf, frame_size);

		MPA_header_t* pmpa_header = &(pmpa_frame->header);

		pmpa_header->syncword = BITS_get(&bs, 12);
		pmpa_header->ID = BITS_get(&bs, 1);
		pmpa_header->layer = BITS_get(&bs, 2);
		pmpa_header->protection_bit = BITS_get(&bs, 1);

		pmpa_header->bitrate_index = BITS_get(&bs, 4);
		pmpa_header->sampling_frequency = BITS_get(&bs, 2);
		pmpa_header->padding_bit = BITS_get(&bs, 1);
		pmpa_header->private_bit = BITS_get(&bs, 1);

		pmpa_header->mode = BITS_get(&bs, 2);
		pmpa_header->mode_extension = BITS_get(&bs, 2);
		pmpa_header->copyright = BITS_get(&bs, 1);
		pmpa_header->original_or_copy = BITS_get(&bs, 1);
		pmpa_header->emphasis = BITS_get(&bs, 2);

		if (pmpa_header->protection_bit == 0)
		{
			pmpa_header->crc_check = BITS_get(&bs, 16);

			pmpa_header->crc_length = 2;
			header_size += 2;
		}
		else
		{
			pmpa_header->crc_length = 0;
		}

		assert(bs.i_left == 8);
		pmpa_frame->audio_data_buf = bs.p_cur;
		pmpa_frame->audio_data_length = (int)(bs.p_end - bs.p_cur);

		if ((pmpa_header->syncword == 0xFFF) &&
			(pmpa_header->layer != 0b00) && 
			(pmpa_header->bitrate_index != 0b1111) &&
			(pmpa_header->sampling_frequency != 0b11))
		{
			/*sematic part*/
			pmpa_header->layer_index = 3 - pmpa_header->layer;
			pmpa_header->bit_rate = MPA_bit_rate_table[pmpa_header->ID][pmpa_header->layer_index][pmpa_header->bitrate_index];
			pmpa_header->sampling_rate = MPA_sampling_rate_table[pmpa_header->ID][pmpa_header->sampling_frequency];

			pmpa_header->num_of_slots = (int)(144 * pmpa_header->bit_rate / pmpa_header->sampling_rate) + pmpa_header->padding_bit;
			pmpa_header->data_length = pmpa_header->num_of_slots - header_size;

			pmpa_header->bitspersample = MPEG_AUDIO_BITS_PER_SAMPLE;

			if (pmpa_header->mode == MPEG_AUDIO_SINGLE_CHANNEL)				//singla channel
			{
				pmpa_header->nch = 1;
			}
			else
			{
				pmpa_header->nch = 2;
			}

			//assert(pmpa_frame->audio_data_length == pmpa_header->data_length);
			if (pmpa_frame->audio_data_length < pmpa_header->data_length)
			{
				//indicats the frame data is not complete
				rtcode = MPA_PARSE_LENGTH_ERROR;
			}
			else
			{
				rtcode = MPA_NO_ERROR;
			}
		}
		else
		{
			rtcode = MPA_PARSE_SYNTAX_ERROR;
		}
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


/*
 * Two tables for dequantization of layer 2 audio data
 */
static double c_coeff[17] =
{
    1.33333333333, 1.60000000000, 1.14285714286, 
    1.77777777777, 1.06666666666, 1.03225806452,
    1.01587301587, 1.00787401575, 1.00392156863,
    1.00195694716, 1.00097751711, 1.00048851979,
    1.00024420024, 1.00012208522, 1.00006103888,
    1.00003051851, 1.00001525902
};

static double d_coeff[17] =
{
    0.50000000000, 0.50000000000, 0.25000000000, 0.50000000000,
    0.12500000000, 0.06250000000, 0.03125000000, 0.01562500000,
    0.00781250000, 0.00390625000, 0.00195312500, 0.00097656250,
    0.00048828125, 0.00024414063, 0.00012207031, 0.00006103516,
	0.00003051758
};

//int mpeg_audio_decode_layer1_audio_data(FIFO_t* pbs)
//{
//	int				bound;
//
//	int				sb, ch, s;
//	int				nb;
//
//    register int	l, k;
//	uint32_t				sample;
//	double			dsample;
//	double			dscalefactor;
//	int				nch = header.nch;
//
//	if (gbInitDecoder)
//	{
//		::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//		bound = (header.mode == MPEG_AUDIO_JOINT_STEREO ? (header.mode_extension << 2 + 4) : 32);
//
//		/*step 1: get bit allocation information*/
//		for (sb = 0; sb < bound; sb++)
//		{
//			for (ch = 0; ch < nch; ch++)
//			{
//				l1_data[ch].allocation[sb] = FIFO_Get(pbs, 4);
//			}
//		}
//
//		for (sb = bound; sb < 32; sb++)
//		{
//			l1_data[0].allocation[sb] = FIFO_Get(pbs, 4);
//			l1_data[1].allocation[sb] = l1_data[0].allocation[sb];
//		}
//
//		/*step 2: get scale factor information*/
//		for (sb = 0; sb < 32; sb ++)
//		{
//			for (ch = 0; ch < nch; ch ++)
//			{
//				if (l1_data[ch].allocation[sb] != 0)
//				{
//					l1_data[ch].scalefactor[sb] = FIFO_Get(pbs, 6);
//				}
//			}
//		}
//
//		/*step 3: get sample information*/
//		for (s = 0; s < 12; s++)
//		{
//			for (sb = 0; sb < bound; sb ++)
//			{
//				for (ch = 0; ch < nch; ch ++)
//				{
//					if (l1_data[ch].allocation[sb] != 0)
//					{
//						nb = layer1_bit_alloc_table[l1_data[ch].allocation[sb]];
//						l1_data[ch].sample[s][sb] = FIFO_Get(pbs, nb);
//					}
//				}
//			}
//
//			for (sb = bound; sb < 32; sb ++)
//			{
//				if (l1_data[0].allocation[sb] != 0)
//				{
//					nb = layer1_bit_alloc_table[l1_data[0].allocation[sb]];
//					l1_data[0].sample[s][sb] = FIFO_Get(pbs, nb);
//					l1_data[1].sample[s][sb] = l1_data[0].sample[s][sb];
//				}
//			}
//		}
//
//		/*step 4: de-quantize	*/
//		for (s = 0; s < 12; s ++)
//		{
//			for (sb = 0; sb < 32; sb ++)
//			{
//				for (ch = 0; ch < nch; ch ++)
//				{
//					if (l1_data[ch].allocation[sb])
//					{
//						nb = MPA_layer1_bit_alloc_table[l1_data[ch].allocation[sb]];
//						sample = l1_data[ch].sample[s][sb];
//		
//						if (((sample >> (nb - 1)) & 1) == 1)
//						{
//							dsample = 0.0;
//						}
//						else
//						{
//							dsample = -1.0;
//						}
//		
//						k = 1L << (nb - 1);
//						l = 1L << nb;
//		
//						dscalefactor = scale_factor_table[l1_data[ch].scalefactor[sb]];
//
//						dsample += (double)(sample & (k - 1)) / (double)k;
//						dsample = (double)l / (l - 1) * (dsample + 1.0 / (double)k);
//						dsample *= dscalefactor;
//
//						l1_data[ch].fraction[s][sb] = dsample;
//					}
//					else
//					{
//						l1_data[ch].fraction[s][sb] = 0.0;
//					}
//				}
//			}
//		}
//
//		Synth_Mpeg1_layer1_audio_data(&header, &(l1_data[0]), &lv, &left);
//		Synth_Mpeg1_layer1_audio_data(&header, &(l1_data[1]), &rv, &right);
//
//		::SetEvent(ghAccessEvent);
//	}
//
//	return 0;
//}

//int mpeg_audio_decode_layer2_audio_data(FIFO_t* pbs)
//{
//	int				table_index = 0;
//	int				sblimit;
//	int				bound;
//	AllocTablePtr	table;
//
//	int				sb, ch, gr, s;
//	int				allocation = 0;
//
//    uint32_t				num_of_bits, c;
//	uint32_t				num_of_levels;
//	uint32_t				quant;
//    register int	x, k;
//	uint32_t				sample;
//	double			dsample;
//	int				nch = header.nch;
//
//	if (gbInitDecoder)
//	{
//		::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//		table_index = which_table[header.sampling_frequency][2 - nch][header.bitrate_index];
//		sblimit = sblims[table_index];
//		l2_data[0].sblimit = l2_data[1].sblimit = sblimit;
//		bound = (header.mode == MPEG_AUDIO_JOINT_STEREO ? (header.mode_extension << 2 + 4) : sblimit);
//		table = alloc_table_table[table_index];
//
//		/*step 1: get bit allocation information*/
//		for (sb = 0; sb < bound; sb++)
//		{
//			num_of_bits = (*table)[sb][0].bits;
//
//			for (ch = 0; ch < nch; ch++)
//			{
//				l2_data[ch].allocation[sb] = FIFO_Get(pbs, num_of_bits);
//			}
//		}
//
//		for (sb = bound; sb < sblimit; sb++)
//		{
//			num_of_bits = (*table)[sb][0].bits;
//			l2_data[0].allocation[sb] = FIFO_Get(pbs, num_of_bits);
//			l2_data[1].allocation[sb] = l2_data[0].allocation[sb];
//		}
//
//		/*step 2: get scale factor select information*/
//		for (sb = 0; sb < sblimit; sb++)
//		{
//			for (ch = 0; ch < nch; ch ++)
//			{
//				if (l2_data[ch].allocation[sb] != 0)
//				{
//					l2_data[ch].scfsi[sb] = FIFO_Get(pbs, 2);
//				}
//			}
//		}
//
//		/*step 3: get scale factor information*/
//		for (sb = 0; sb < sblimit; sb++)
//		{
//			for (ch = 0; ch < nch; ch ++)
//			{
//				if (l2_data[ch].allocation[sb] != 0)
//				{
//					switch (l2_data[ch].scfsi[sb])
//					{
//					case 0:
//						l2_data[ch].scalefactor[0][sb] = FIFO_Get(pbs, 6);
//						l2_data[ch].scalefactor[1][sb] = FIFO_Get(pbs, 6);
//						l2_data[ch].scalefactor[2][sb] = FIFO_Get(pbs, 6);
//						break;
//
//					case 1:
//						l2_data[ch].scalefactor[0][sb] = FIFO_Get(pbs, 6);
//						l2_data[ch].scalefactor[1][sb] = l2_data[ch].scalefactor[0][sb];
//						l2_data[ch].scalefactor[2][sb] = FIFO_Get(pbs, 6);
//						break;
//
//					case 2:
//						l2_data[ch].scalefactor[0][sb] = FIFO_Get(pbs, 6);
//						l2_data[ch].scalefactor[1][sb] = l2_data[ch].scalefactor[0][sb];
//						l2_data[ch].scalefactor[2][sb] = l2_data[ch].scalefactor[0][sb];
//						break;
//
//					case 3:
//						l2_data[ch].scalefactor[0][sb] = FIFO_Get(pbs, 6);
//						l2_data[ch].scalefactor[1][sb] = FIFO_Get(pbs, 6);
//						l2_data[ch].scalefactor[2][sb] = l2_data[ch].scalefactor[1][sb];
//						break;
//					}
//				}
//			}
//		}
//
//		/*step 4: get sample information*/
//		for (gr = 0; gr < 12; gr ++)
//		{
//			for (sb = 0; sb < bound; sb++)
//			{
//				for (ch = 0; ch < nch; ch ++)
//				{
//					allocation = l2_data[ch].allocation[sb];
//					if (allocation != 0)
//					{
//						num_of_levels = (*table)[sb][allocation].steps;
//						num_of_bits = (*table)[sb][allocation].bits;
//						quant = (*table)[sb][allocation].quant;
//
//						// find MSB
//						x = 0;
//						while ((1L << x) < (int)num_of_levels)
//						{
//							x++;
//						}
//
//						if ((num_of_levels == 3) ||					//grouping
//							(num_of_levels == 5) ||
//							(num_of_levels == 9))
//						{
//							c = FIFO_Get(pbs, num_of_bits);
//							for (s = 0; s < 3; s++)
//							{
//								sample = c % num_of_levels;
//								c /= num_of_levels;
//
//								if (((sample >> (x - 1)) & 1) == 1)					//negative
//								{
//									dsample = 0.0;
//								}
//								else												//positive
//								{
//									dsample = -1.0;
//								}
//
//								// calculate 2's complement sample
//								k = (1L << (x - 1));
//
//								dsample += (double) (sample & (k - 1)) / (double) k;
//
//								dsample += d_coeff[quant];
//								dsample *= c_coeff[quant];
//								dsample *= scale_factor_table[l2_data[ch].scalefactor[s][sb]];
//
//								l2_data[ch].fraction[gr][s][sb] = dsample;
//							}
//						}
//						else
//						{
//							for (s = 0; s < 3; s++)							/*not group*/
//							{
//								sample = FIFO_Get(pbs, num_of_bits);
//								if (((sample >> (x - 1)) & 1) == 1)					//negative
//								{
//									dsample = 0.0;
//								}
//								else												//positive
//								{
//									dsample = -1.0;
//								}
//
//								// calculate 2's complement sample
//								k = (1L << (x - 1));
//
//								dsample += (double) (sample & (k - 1)) / (double) k;
//
//								dsample += d_coeff[quant];
//								dsample *= c_coeff[quant];
//								dsample *= scale_factor_table[l2_data[ch].scalefactor[s][sb]];
//
//								l2_data[ch].fraction[gr][s][sb] = dsample;
//							}
//						}
//					}
//					else
//					{
//						l2_data[ch].fraction[gr][0][sb] = 0.0;
//						l2_data[ch].fraction[gr][1][sb] = 0.0;
//						l2_data[ch].fraction[gr][2][sb] = 0.0;
//					}
//				}
//			}
//
//			for (sb = bound; sb < sblimit; sb ++)
//			{
//				allocation = l2_data[0].allocation[sb];
//				if (allocation != 0)
//				{
//					num_of_levels = (*table)[sb][allocation].steps;
//					num_of_bits = (*table)[sb][allocation].bits;
//					quant = (*table)[sb][allocation].quant;
//
//					if ((num_of_levels == 3) ||					//grouping
//						(num_of_levels == 5) ||
//						(num_of_levels == 9))
//					{
//						c = FIFO_Get(pbs, num_of_bits);
//						for (s = 0; s < 3; s++)
//						{
//							sample = c % num_of_levels;
//							c /= num_of_levels;
//
//							if (((sample >> (x - 1)) & 1) == 1)					//negative
//							{
//								dsample = 0.0;
//							}
//							else												//positive
//							{
//								dsample = -1.0;
//							}
//
//							// calculate 2's complement sample
//							k = (1L << (x - 1));
//
//							dsample += (double) (sample & (k - 1)) / (double) k;
//
//							dsample += d_coeff[quant];
//							dsample *= c_coeff[quant];
//							dsample *= scale_factor_table[l2_data[0].scalefactor[s][sb]];
//
//							l2_data[0].fraction[gr][s][sb] = l2_data[1].fraction[gr][s][sb] = dsample;
//						}
//					}
//					else
//					{
//						for (s = 0; s < 3; s++)
//						{
//							sample = FIFO_Get(pbs, num_of_bits);
//							if (((sample >> (x - 1)) & 1) == 1)					//negative
//							{
//								dsample = 0.0;
//							}
//							else												//positive
//							{
//								dsample = -1.0;
//							}
//
//							// calculate 2's complement sample
//							k = (1L << (x - 1));
//
//							dsample += (double) (sample & (k - 1)) / (double) k;
//
//							dsample += d_coeff[quant];
//							dsample *= c_coeff[quant];
//							dsample *= scale_factor_table[l2_data[0].scalefactor[s][sb]];
//
//							l2_data[0].fraction[gr][s][sb] = l2_data[1].fraction[gr][s][sb] = dsample;
//						}
//					}
//				}
//				else
//				{
//					l2_data[0].fraction[gr][0][sb] = l2_data[1].fraction[gr][0][sb] = 0.0;
//					l2_data[0].fraction[gr][1][sb] = l2_data[1].fraction[gr][1][sb] = 0.0;
//					l2_data[0].fraction[gr][2][sb] = l2_data[1].fraction[gr][2][sb] = 0.0;
//				}
//			}
//
//			for (sb = sblimit; sb < 32; sb ++)
//			{
//				for (ch = 0; ch < nch; ch ++)
//				{
//					l2_data[ch].fraction[gr][0][sb] = 0.0;
//					l2_data[ch].fraction[gr][1][sb] = 0.0;
//					l2_data[ch].fraction[gr][2][sb] = 0.0;
//				}
//			}
//		}
//
//		/*step 5: dequantize*/
//
//		/*step 6: subband synthesis*/
//	/*
//		S16*	   curr_pcm;
//		double*	   fraction;
//
//		curr_pcm = (S16 *)left.firstSample;
//
//		for (gr = 0; gr < 12; gr++)
//		{
//			for (s = 0; s < 3; s++)
//			{
//				fraction = &(l2_data[0].fraction[gr][s][0]);
//				SubBandSynthesis(&lv, fraction, &curr_pcm);
//			}
//		}
//
//		curr_pcm = (S16 *)right.firstSample;
//		for (gr = 0; gr < 12; gr++)
//		{
//			for (s = 0; s < 3; s++)
//			{
//				fraction = &(l2_data[1].fraction[gr][s][0]);
//				SubBandSynthesis(&rv, fraction, &curr_pcm);
//			}
//		}
//	*/
//
//		Synth_Mpeg1_layer2_audio_data(&header, &(l2_data[0]), &lv, &left);
//		Synth_Mpeg1_layer2_audio_data(&header, &(l2_data[1]), &rv, &right);
//
//		::SetEvent(ghAccessEvent);
//	}
//
//	return 0;
//}

//int mpeg_audio_decode_layer3_audio_data(FIFO_t* pbs)
//{
//	int				scfsi_band;
//	int				ch;
//	int				gr;
//	int				region;
//	int				window;
//	int				sfb;
//	int				num_of_slots;
//	int				header_size;
//	int				slen1, slen2;
//	int				nch = header.nch;
//
//	if (gbInitDecoder)
//	{
//		::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//		/*main_data_begin*/
//		l3_data->main_data_begin = FIFO_Get(pbs, 9);
//
//		if (header.mode == MPEG_AUDIO_SINGLE_CHANNEL)
//		{
//			l3_data->private_bits = FIFO_Get(pbs, 5);
//		}
//		else
//		{
//			l3_data->private_bits = FIFO_Get(pbs, 3);
//		}
//
//		for (ch = 0; ch < nch; ch ++)
//		{
//			for (scfsi_band = 0; scfsi_band < 4; scfsi_band ++)
//			{
//				l3_data->scfsi[ch][scfsi_band] = FIFO_Get(pbs, 1);
//			}
//		}
//
//		for (gr = 0; gr < 2; gr ++)
//		{
//			for (ch = 0; ch < nch; ch ++)
//			{
//				l3_data->part2_3_length[gr][ch] = FIFO_Get(pbs, 12);
//				l3_data->big_values[gr][ch] = FIFO_Get(pbs, 9);
//				l3_data->global_gain[gr][ch] = FIFO_Get(pbs, 8);
//				l3_data->scalefac_compress[gr][ch] = FIFO_Get(pbs, 4);
//				l3_data->window_switching_flag[gr][ch] = FIFO_Get(pbs, 1);
//
//				if (l3_data->window_switching_flag)
//				{
//					l3_data->block_type[gr][ch] = FIFO_Get(pbs, 2);
//					l3_data->mixed_block_flag[gr][ch] = FIFO_Get(pbs, 1);
//
//					for (region = 0; region < 2; region ++)
//					{
//						l3_data->table_select[gr][ch][region] = FIFO_Get(pbs, 5);
//					}
//
//					for (window = 0; window < 3; window ++)
//					{
//						l3_data->subblock_gain[gr][ch][window] =FIFO_Get(pbs, 3);
//					}
//
//					if ((l3_data->block_type[gr][ch] == 2) && (!l3_data->mixed_block_flag[gr][ch]))
//					{
//						l3_data->region0_count[gr][ch] = 8;
//					}
//					else
//					{
//						l3_data->region0_count[gr][ch] = 7;
//					}
//
//					l3_data->region1_count[gr][ch] = 20 - l3_data->region0_count[gr][ch];	//11172-3 defines: 63
//				}
//				else
//				{
//					l3_data->block_type[gr][ch] = 0;
//
//					for (region = 0; region < 3; region ++)
//					{
//						l3_data->table_select[gr][ch][region] = FIFO_Get(pbs, 5);
//					}
//
//					l3_data->region0_count[gr][ch] = FIFO_Get(pbs, 4);
//					l3_data->region1_count[gr][ch] = FIFO_Get(pbs, 3);
//				}
//
//				l3_data->preflag[gr][ch] = FIFO_Get(pbs, 1);
//				l3_data->scalefac_scale[gr][ch] = FIFO_Get(pbs, 1);
//				l3_data->count1table_select[gr][ch] = FIFO_Get(pbs, 1);
//			}
//		}
//
//		/* calculate the size of this header in bytes */
//		header_size = 4 + (header.protection_bit == 0 ? 2 : 0) + (nch == 1 ? 17 : 32);
//
//		/* calculate the size of the main_data block of this frame */
//		/* the distance between two consequitive syncwords is determined from
//		   the formula: N = 144 * bitrate / sampling_frequency + padding */
//		/* then we find the size of the main data by taking that number and
//		subtracting the size of the header and padding */
//		num_of_slots = (int)(144 * header.bit_rate / header.sampling_rate) + header.padding_bit;
//		num_of_slots -= header_size;
//
//		if (l3_data->main_data_begin == 0)
//		{
//		/*main data*/   
//		for (gr = 0; gr < 2; gr ++)
//		{
//			for (ch = 0; ch < nch; ch ++)
//			{
//				slen1 = slen[0][l3_data->scalefac_compress[gr][ch]];
//				slen2 = slen[1][l3_data->scalefac_compress[gr][ch]];
//
//				if ((l3_data->window_switching_flag[gr][ch] == 1) && (l3_data->block_type[gr][ch] == 2))
//				{
//					if (l3_data->mixed_block_flag[gr][ch])
//					{
//						/* mixed blocks - a mixed block uses
//							both long scalefactors (0-7) and short scalefactors (3-11)
//							which together makes up the whole spectrum 
//						*/
//
//						/* block_type 2 and mixed_block_flag 1, 
//							slen1: length of scalefactors for bands 0 to 7 of the
//							long sf and length of sf's 3 to 5
//							of the short scalefactor band 
//							slen2: length of scalefactors for bands 6 to 11 of the
//							short scalefactor band
//						*/
//
//						for (sfb = 0; sfb < 8; sfb++)
//						{
//							l3_data->scalefac_l[gr][ch][sfb] = FIFO_Get(pbs, slen1);
//						}
//
//						for (sfb = 3; sfb < 6; sfb ++)
//						{
//							for (window = 0; window < 3; window ++)
//							{
//								l3_data->scalefac_s[gr][ch][sfb][window] = FIFO_Get(pbs, slen1);
//							}
//						}
//
//						for (sfb = 7; sfb < 12; sfb ++)
//						{
//							for (window = 0; window < 3; window ++)
//							{
//								l3_data->scalefac_s[gr][ch][sfb][window] = FIFO_Get(pbs, slen2);
//							}
//						}
//
//						for (window = 0; window < 3; window ++)
//						{
//							l3_data->scalefac_s[gr][ch][12][window] = 0;
//						}
//					}
//					else
//					{
//						/* short block using only short scalefactors
//							slen1: length of scalefactors for short sf bands 0 to 5
//							slen2: length of scalefactors for short sf bands 6 to 11
//						*/
//						for (sfb = 0; sfb < 6; sfb ++)
//						{
//							for (window = 0; window < 3; window ++)
//							{
//								l3_data->scalefac_s[gr][ch][sfb][window] = FIFO_Get(pbs, slen1);
//							}
//						}
//
//						for (sfb = 6; sfb < 12; sfb ++)
//						{
//							for (window = 0; window < 3; window ++)
//							{
//								l3_data->scalefac_s[gr][ch][sfb][window] = FIFO_Get(pbs, slen2);
//							}
//						}
//
//						for (window = 0; window < 3; window ++)
//						{
//							l3_data->scalefac_s[gr][ch][12][window] = 0;
//						}
//					}
//				}
//				else
//				{
//					/* long block
//					slen1: length of scalefactors for long sf bands 0 to 10
//					slen2: length of scalefactors for long sf bands 11 to 20
//					*/
//
//					/* the sf bands are divided in 4 parts, 0-5, 6-10, 11-15, 16-20
//					the standard seems to disagree with itself here, but apparently
//					it works this way:
//					*/
//
//					static int sfb_bound[5] = {0, 6, 11, 16, 21};
//
//					for (scfsi_band = 0; scfsi_band < 4; scfsi_band++)
//					{
//						if ((l3_data->scfsi[ch][scfsi_band] == 0) || (gr == 0))
//						{
//							for (sfb = sfb_bound[scfsi_band]; sfb < sfb_bound[scfsi_band + 1]; sfb++) 
//							{
//								l3_data->scalefac_l[gr][ch][sfb] = FIFO_Get(pbs, (sfb < 11 ? slen1 : slen2));
//							}
//						}
//					}
//				}
//			}
//		}
//		
//		} //main_data_begin == 0
//
//		::SetEvent(ghAccessEvent);
//	}
//
//	return 0;
//}

//int	mpeg_audio_ancillary_data(FIFO_t* pbs)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//void Synth_Mpeg1_layer1_audio_data(M2A_header_t* hdr, MPEG1_layer1_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm)
//{
//
//    int		   s;
//	double*	   fraction;
//    S16		   *curr_pcm;
//
//    curr_pcm = (S16 *) pcm->firstSample;
//
//    for (s = 0; s < 12; s++)
//	{
//		fraction = data->fraction[s];					//32 sample:		32 subband
//
//        SubBandSynthesis(v, fraction, &curr_pcm);
//    }
//}

//void Synth_Mpeg1_layer2_audio_data(M2A_header_t* hdr, MPEG1_layer2_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm)
//{
//    int		   s, gr;
//	double*	   fraction;
//    S16		   *curr_pcm;
//
//    curr_pcm = (S16 *) pcm->firstSample;
//
//    for (gr = 0; gr < 12; gr++)
//	{
//        for (s = 0; s < 3; s++)
//		{
//			fraction = data->fraction[gr][s];					//32 sample:		32 subband
//
//            SubBandSynthesis(v, fraction, &curr_pcm);
//        }
//    }
//}

//void Synth_Mpeg1_layer3_audio_data(M2A_header_t* hdr, MPEG1_layer3_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm)
//{
///*
//    int		   s, gr;
//	double*	   fraction;
//    S16		   *curr_pcm;
//
//    curr_pcm = (S16 *) pcm->firstSample;
//
//    for (gr = 0; gr < 12; gr++)
//	{
//        for (s = 0; s < 3; s++)
//		{
//			fraction = data->fraction[gr][s];					//32 sample:		32 subband
//
//            SubBandSynthesis(v, fraction, &curr_pcm);
//        }
//    }
//*/
//}

extern double filter_table[64][32];

// extern double decode_window[512];
extern double decode_window[512];

void SubBandSynthesis(MpegAudioSynData_t* syndata, double* fraction, short** pcm)
{
    register int i, j;

    /*register long *curr_f; */
    register double *curr_f;
    register double *curr_fraction;
    register double *curr_filter;
    register double *curr_v;
    register double sum;
    register int offset;
    register double *v;
    int longpcm;

    syndata->offset = (syndata->offset - 64) & 0x3ff;
    offset = syndata->offset;
    v = syndata->syn;

    // end_v =  v + 1023;
    // curr_v = v + 959;
    /*memmove(end_v, curr_v, sizeof(double)*960); */

    // DO_N_TIMES(960,
    //  *end_v-- = *curr_v--;
    // );

    // sub band synthesis

    curr_filter = &(filter_table[0][0]);
    curr_v = &v[offset];

    for (i = 0; i < 64; i++) {
        double sumf = 0;

        curr_fraction = fraction;
        for (j = 0; j < 32; j ++)
		{
            sumf += (*curr_fraction++) * (*curr_filter++);
		}

        *curr_v++ = sumf;
    }

    // calculate samples.
    // here is the mapping :
    // if  x % 64 <  32 : w[x] = v[(x/64)*128 + (x%64)] * decode_window[x]
    // if  x % 64 >= 32 : w[x] = v[(x/64)*128 + (x%64) + 96] * decode_window[x]

    for (j = 0; j < 32; j++) {

        // for (i = 0; i < 16; i++) {
        //   sum += decode_window[k]* v[k + (((i+1)>>1) << 6)];
        //   k += 32;
        // }
        // decode_window is the window given in the standard *4096*32768
        //
        curr_f = &(decode_window[j]);
        sum = *(curr_f) * v[(j + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 96 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 128 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 224 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 256 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 352 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 384 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 480 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 512 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 608 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 640 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 736 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 768 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 864 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 896 + offset) & 0x3ff];
        curr_f += 32;
        sum += *(curr_f) * v[(j + 992 + offset) & 0x3ff];
        /*
           if (sum > 134217728) // 4096*32768
           **pcm = 32767;
           else if (sum < -134217728)
           **pcm = -32768;
           else 
           **pcm = ((long)sum)/4096 ; // div by 4096
         */
#if MPEG_AUDIO_BITS_PER_SAMPLE == 16
		longpcm = (int) (sum * 32768);
        
		if (longpcm >= 32768)
		{
            **pcm = 32767;
		}
        else if (longpcm < -32768)
		{
            **pcm = -32768;
		}
        else
		{
            **pcm = (short) longpcm;
		}
#else
	#error bits of audio samples not defined
#endif

        /*fprintf(stderr, "%d\n", **pcm); */
        (*pcm)++;
    }
}

//void AudioNew(AudioStream_t* paudio, int length)
//{
//	if (paudio != NULL)
//	{
//		paudio->length = length;			//sample count
//		paudio->start = 0;
////		paudio->buffer = (uint8_t *)malloc(sizeof(S16) * length);
//
//#if MPEG_AUDIO_BITS_PER_SAMPLE == 16
//		paudio->buffer = (uint8_t *)malloc(sizeof(S16) * length);
//#else
//	#error sample bits not defined.....
//#endif
//		
//		paudio->firstSample = paudio->buffer;
//		paudio->isVirtual = 0;
//	}
//}

//void AudioFree(AudioStream_t* paudio)
//{
//	if (paudio != NULL)
//	{
//		if (paudio->buffer != NULL)
//		{
//			free(paudio->buffer);
//			paudio->buffer = NULL;
//			paudio->firstSample = NULL;
//			paudio->length = 0;
//			paudio->start = 0;
//		}
//	}
//}

//void AudioMerge(AudioStream_t* pleft, AudioStream_t* pright, AudioStream_t* pdest)
//{
//	int		 i;
//    int		 size;
//
//#if MPEG_AUDIO_BITS_PER_SAMPLE == 16
//    S16		 *currDest, *currLeft, *currRight;
//#else
//	#error bits of audio sample not defined
//#endif
//
//	if ((pleft != NULL) && (pright != NULL) && (pdest != NULL))
//	{
//		size = min(pleft->length, pright->length);
//
//		currDest = (short *)pdest->firstSample;
//		currLeft = (short *)pleft->firstSample;
//		currRight = (short *)pright->firstSample;
//
//		for (i = 0; i < size; i ++)
//		{
//			*(currDest++) = *(currLeft++);
//			*(currDest++) = *(currRight++);
//		}
//	}
//}

//int mpeg_audio_resync(FIFO_t* pbs)
//{
//	uint16_t		syncword = 0;
//
//	if (pbs->bitleft > 0)
//	{
//		if (pbs->rdbit_offset > 0)
//		{
//			FIFO_Flush(pbs, (8 - pbs->rdbit_offset));
//		}
//	}
//
//	do
//	{
//		syncword = FIFO_Peek(pbs, 16);
//		if ((syncword & 0xfff0) == 0xfff0)
//		{
//			break;
//		}
//		else
//		{
//			FIFO_Flush(pbs, 8);
//
//			if (pbs->bitleft < 16)
//			{
//				syncword = -1;
//				break;
//			}
//		}
//
//	} while (1);
//
//	return syncword;
//}

//uint8_t*	mpeg_audio_output(int* pnSampleCount)
//{
//	uint8_t*		pucSample = NULL;
//	int		nSize = 0;
//
//	if (gbInitDecoder)
//	{
//		::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//		AudioMerge(&left, &right, &merge);
//
//		pucSample = (uint8_t*)merge.firstSample;
//
//	#if MPEG_AUDIO_BITS_PER_SAMPLE == 16
//		nSize = merge.length * sizeof(S16);
//	#else
//	#error bits of audio sample not defined
//	#endif
//
//		if (pnSampleCount != NULL)
//		{
//			*pnSampleCount = nSize;
//		}
//
//		::SetEvent(ghAccessEvent);
//	}
//
//	return pucSample;
//}
