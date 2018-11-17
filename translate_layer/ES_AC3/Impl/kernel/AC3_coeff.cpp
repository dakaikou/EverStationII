/* 
 *	Copyright (C) Aaron Holtzman - May 1999
 *
 *  This file is part of ac3dec, a free Dolby AC-3 stream decoder.
 *	
 *  ac3dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  ac3dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */
#include <stdlib.h>

#include "../../Include/kernel/kernel_ac3_audio_decoder.h"
#include "XStream_BitStream/Include/BitStream.h"

// Lookup tables of 0.15 two's complement quantization values
static const S16 q_1[3] = 
{
	( -2 << 15)/3, 0,(  2 << 15)/3 
};

static const S16 q_2[5] = 
{
	( -4 << 15)/5,( -2 << 15)/5, 0,
	(  2 << 15)/5,(  4 << 15)/5
};

static const S16 q_3[7] = 
{
	( -6 << 15)/7,( -4 << 15)/7,( -2 << 15)/7, 0,
	(  2 << 15)/7,(  4 << 15)/7,(  6 << 15)/7
};

static const S16 q_4[11] = 
{
	(-10 << 15)/11,(-8 << 15)/11,(-6 << 15)/11, ( -4 << 15)/11,(-2 << 15)/11,  0,
	(  2 << 15)/11,( 4 << 15)/11,( 6 << 15)/11, (  8 << 15)/11,(10 << 15)/11
};

static const S16 q_5[15] = 
{
	(-14 << 15)/15,(-12 << 15)/15,(-10 << 15)/15,
	( -8 << 15)/15,( -6 << 15)/15,( -4 << 15)/15,
	( -2 << 15)/15,   0          ,(  2 << 15)/15,
	(  4 << 15)/15,(  6 << 15)/15,(  8 << 15)/15,
	( 10 << 15)/15,( 12 << 15)/15,( 14 << 15)/15
};

// Scale factors for convert_to_double
static const uint32_t u32_scale_factors[25] = 
{
	0x38000000, //2 ^ -(0 + 15)
	0x37800000, //2 ^ -(1 + 15)
	0x37000000, //2 ^ -(2 + 15)
	0x36800000, //2 ^ -(3 + 15)
	0x36000000, //2 ^ -(4 + 15)
	0x35800000, //2 ^ -(5 + 15)
	0x35000000, //2 ^ -(6 + 15)
	0x34800000, //2 ^ -(7 + 15)
	0x34000000, //2 ^ -(8 + 15)
	0x33800000, //2 ^ -(9 + 15)
	0x33000000, //2 ^ -(10 + 15)
	0x32800000, //2 ^ -(11 + 15)
	0x32000000, //2 ^ -(12 + 15)
	0x31800000, //2 ^ -(13 + 15)
	0x31000000, //2 ^ -(14 + 15)
	0x30800000, //2 ^ -(15 + 15)
	0x30000000, //2 ^ -(16 + 15)
	0x2f800000, //2 ^ -(17 + 15)
	0x2f000000, //2 ^ -(18 + 15)
	0x2e800000, //2 ^ -(19 + 15)
	0x2e000000, //2 ^ -(20 + 15)
	0x2d800000, //2 ^ -(21 + 15)
	0x2d000000, //2 ^ -(22 + 15)
	0x2c800000, //2 ^ -(23 + 15)
	0x2c000000  //2 ^ -(24 + 15)
};

static float *scale_factor = (float*)u32_scale_factors;

// These store the persistent state of the packed mantissas
static S16 m_1[3];
static S16 m_2[3];
static S16 m_4[2];
static S16 m_1_pointer;
static S16 m_2_pointer;
static S16 m_4_pointer;

// Conversion from bap to number of bits in the mantissas
// zeros account for cases 0,1,2,4 which are special cased
static uint16_t qnttztab[16] = { 0, 0, 0, 3, 0 , 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16};

static void coeff_reset(void);
//static S16  coeff_get_mantissa(FIFO_t* pbs, uint16_t bap, uint16_t dithflag);
static void coeff_uncouple_ch(double samples[], AC3_bsi_t *bsi, AC3_audblk_t *audblk, uint32_t ch);

// Convert a 0.15 fixed point number into IEEE single
// precision floating point and scale by 2^-exp
__forceinline static float convert_to_float(uint16_t exp, S16 mantissa)
{
	return mantissa * scale_factor[exp];
}

//void AC3_coeff_unpack(FIFO_t* pbs, AC3_bsi_t *bsi, AC3_audblk_t *audblk, stream_samples_t samples)
//{
//	uint16_t ch, j;
//	uint32_t done_cpl = 0;
//	S16 mantissa;
//
//	coeff_reset();
//
//	for (ch = 0; ch < bsi->nfchans; ch++)
//	{
//		for (j = 0; j < audblk->endmant[ch]; j++)
//		{
//			mantissa = coeff_get_mantissa(pbs, audblk->fbw_bap[ch][j], audblk->dithflag[ch]);
//			samples[ch][j] = convert_to_float(audblk->fbw_exp[ch][j], mantissa);
//		}
//
//		if (audblk->cplinu && audblk->chincpl[ch] && !done_cpl)
//		{
//			// ncplmant is equal to 12 * ncplsubnd
//			// Don't dither coupling channel until channel separation so that
//			// interchannel noise is uncorrelated 
//			for (j = audblk->cplstrtmant; j < audblk->cplendmant; j++)
//			{
//				audblk->cplmant[j] = coeff_get_mantissa(pbs, audblk->cpl_bap[j], 0);
//			}
//			done_cpl = 1;
//		}
//	}
//
//	// uncouple the channel if necessary
//	if (audblk->cplinu)
//	{
//		for (ch = 0; ch < bsi->nfchans; ch++)
//		{
//			if (audblk->chincpl[ch])
//			{
//				coeff_uncouple_ch(samples[ch], bsi, audblk, ch);
//			}
//		}
//	}
//
//	if (bsi->lfeon)
//	{
//		// There are always 7 mantissas for lfe, no dither for lfe 
//		for (j = 0; j < 7; j++)
//		{
//			mantissa = coeff_get_mantissa(pbs, audblk->lfe_bap[j], 0);
//			samples[5][j] = convert_to_float(audblk->lfe_exp[j], mantissa);
//		}
//	}
//}

// Fetch a mantissa from the bitstream
// The mantissa returned is a signed 0.15 fixed point number
//static S16 coeff_get_mantissa(FIFO_t* pbs, uint16_t bap, uint16_t dithflag)
//{
//	S16 mantissa;
//	uint16_t group_code;
//	int error_flag = 0;
//
//	switch (bap)
//	{
//	case 0:
//		if (dithflag)
//		{
//			mantissa = (uint16_t)(46340.0 * rand() / RAND_MAX) - 23170;
//		}
//		else
//		{
//			mantissa = 0;
//		}
//		break;
//
//	case 1:
//		if (m_1_pointer > 2)
//		{
//			group_code = FIFO_Get(pbs, 5);
//
//			if(group_code > 26)
//			{
//				goto error;
//			}
//
//			m_1[0] = group_code / 9; 
//			m_1[1] = (group_code % 9) / 3; 
//			m_1[2] = (group_code % 9) % 3; 
//			m_1_pointer = 0;
//		}
//		mantissa = m_1[m_1_pointer++];
//		mantissa = q_1[mantissa];
//		break;
//
//	case 2:
//		if(m_2_pointer > 2)
//		{
//			group_code = FIFO_Get(pbs, 7);
//
//			if (group_code > 124)
//			{
//				goto error;
//			}
//
//			m_2[0] = group_code / 25;
//			m_2[1] = (group_code % 25) / 5 ;
//			m_2[2] = (group_code % 25) % 5 ; 
//			m_2_pointer = 0;
//		}
//		mantissa = m_2[m_2_pointer++];
//		mantissa = q_2[mantissa];
//		break;
//
//	case 3:
//		mantissa = FIFO_Get(pbs, 3);
//
//		if (mantissa > 6)
//		{
//			goto error;
//		}
//
//		mantissa = q_3[mantissa];
//		break;
//
//	case 4:
//		if(m_4_pointer > 1)
//		{
//			group_code = FIFO_Get(pbs, 7);
//
//			if (group_code > 120)
//			{
//				goto error;
//			}
//
//			m_4[0] = group_code / 11;
//			m_4[1] = group_code % 11;
//			m_4_pointer = 0;
//		}
//		mantissa = m_4[m_4_pointer++];
//		mantissa = q_4[mantissa];
//		break;
//
//	case 5:
//		mantissa = FIFO_Get(pbs, 4);
//
//		if (mantissa > 14)
//		{
//			goto error;
//		}
//
//		mantissa = q_5[mantissa];
//		break;
//
//	default:
//		mantissa = FIFO_Get(pbs, qnttztab[bap]);
//		mantissa <<= 16 - qnttztab[bap];
//		break;
//	}
//
//	return mantissa;
//
//error:
//	error_flag = 1;
//
//	return error_flag;
//}

// Reset the mantissa state
static void coeff_reset(void)
{
	m_1[2] = m_1[1] = m_1[0] = 0;
	m_2[2] = m_2[1] = m_2[0] = 0;
	m_4[1] = m_4[0] = 0;
	m_1_pointer = m_2_pointer = m_4_pointer = 3;
}

// Uncouple the coupling channel into a fbw channel
static void coeff_uncouple_ch(double samples[], AC3_bsi_t *bsi, AC3_audblk_t *audblk, uint32_t ch)
{
	uint32_t bnd = 0;
	uint32_t sub_bnd = 0;
	int i, j;
	double cpl_coord = 1.0;
	uint32_t cpl_exp_tmp;
	uint32_t cpl_mant_tmp;
	S16 mantissa;

	for(i=audblk->cplstrtmant; i<audblk->cplendmant;)
	{
		if(!audblk->cplbndstrc[sub_bnd++])
		{
			cpl_exp_tmp = audblk->cplcoexp[ch][bnd] + 3 * audblk->mstrcplco[ch];
			if(audblk->cplcoexp[ch][bnd] == 15)
				cpl_mant_tmp = (audblk->cplcomant[ch][bnd]) << 11;
			else
				cpl_mant_tmp = (16 + audblk->cplcomant[ch][bnd]) << 10;

			cpl_coord = convert_to_float(cpl_exp_tmp, cpl_mant_tmp) * 8.0;

			// Invert the phase for the right channel if necessary
			if(bsi->acmod == 0x2 && audblk->phsflginu && ch == 1 && audblk->phsflg[bnd])
				cpl_coord = -cpl_coord;

			bnd++;
		}

		for(j=0; j<12; j++)
		{
			// Get new dither values for each channel if necessary, so
			// the channels are uncorrelated
			if(audblk->dithflag[ch] && audblk->cpl_bap[i] == 0)
				mantissa = (uint16_t)(46340.0 * rand() / RAND_MAX) - 23170;
			else
				mantissa = audblk->cplmant[i];

			samples[i]  = cpl_coord * convert_to_float(audblk->cpl_exp[i], mantissa);

			i++;
		}
	}
}
