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
#include <math.h>

#include "../../Include/kernel/kernel_ac3_audio_decoder.h"

static int	  DRC_Flag = DRC_NORMAL;
static int	  DSDown_Flag = FALSE;

static double drc[256];
static double DRCScale[4] = {1.0, 1.5, 2.0, 4.0};

static double ds_x1 = 0;
static double ds_x2 = 0;
static double ds_y1 = 0;
static double ds_y2 = 0;

void AC3_drc_init(void)
{
	int i;

	for (i = 0; i < 128; i++)
	{
		drc[i] = pow(DRCScale[DRC_Flag], i/32.0);
	}

	for (i = 128; i < 256; i++)
	{
		drc[i] = pow(DRCScale[DRC_Flag], (i-256)/32.0);
	}
}

static double cmixlev_lut[4] = { 0.2928, 0.2468, 0.2071, 0.2468 };
static double smixlev_lut[4] = { 0.2928, 0.2071, 0.0   , 0.2071 };

static void downmix_3f_2r_to_2ch(double gain, AC3_bsi_t* bsi, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, clev, slev;
	double *centre = 0, *left = 0, *right = 0, *left_sur = 0, *right_sur = 0;

	left      = samples[0];
	centre    = samples[1];
	right     = samples[2];
	left_sur  = samples[3];
	right_sur = samples[4];

	clev = cmixlev_lut[bsi->cmixlev];
	slev = smixlev_lut[bsi->surmixlev];

	for (j = 0; j < 512; j += 2) 
	{
		tmp = (0.4142 * *left++ + clev * *centre + slev * *left_sur++) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + clev * *centre++ + slev * *right_sur++) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_2f_2r_to_2ch(double gain, AC3_bsi_t* bsi, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, slev;
	double *left = 0, *right = 0, *left_sur = 0, *right_sur = 0;

	left      = samples[0];
	right     = samples[1];
	left_sur  = samples[2];
	right_sur = samples[3];

	slev = smixlev_lut[bsi->surmixlev];

	for (j = 0; j < 512; j += 2) 
	{
		tmp = (0.4142 * *left++ + slev * *left_sur++) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + slev * *right_sur++) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_3f_1r_to_2ch(double gain, AC3_bsi_t* bsi, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, clev, slev;
	double *centre = 0, *left = 0, *right = 0, *sur = 0;

	left      = samples[0];
	centre    = samples[1];
	right     = samples[2];
	sur       = samples[3];

	clev = cmixlev_lut[bsi->cmixlev];
	slev = smixlev_lut[bsi->surmixlev];

	for (j = 0; j < 512; j += 2) 
	{
		tmp = (0.4142 * *left++ + clev * *centre++ + slev * *sur) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + clev * *centre + slev * *sur++) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_2f_1r_to_2ch(double gain, AC3_bsi_t* bsi, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, slev;
	double *left = 0, *right = 0, *sur = 0;

	left      = samples[0];
	right     = samples[1];
	sur       = samples[2];

	slev = smixlev_lut[bsi->surmixlev];

	for (j = 0; j < 512; j += 2) 
	{
		tmp = (0.4142 * *left++ + slev * *sur) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + slev * *sur++) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_3f_0r_to_2ch(double gain, AC3_bsi_t* bsi, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, clev;
	double *centre = 0, *left = 0, *right = 0;

	left      = samples[0];
	centre    = samples[1];
	right     = samples[2];

	clev = cmixlev_lut[bsi->cmixlev];

	for (j = 0; j < 512; j += 2) 
	{
		tmp = (0.4142 * *left++ + clev * *centre) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + clev * *centre++) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}
				
static void downmix_2f_0r_to_2ch(double gain, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp;
	double *left = 0, *right = 0;

	left      = samples[0];
	right     = samples[1];

	for (j = 0; j < 512; j += 2)
	{
		tmp = *left++ * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = *right++ * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_1f_0r_to_2ch(double gain, double *centre, S16 *s16_samples)
{
	U32 j;
	double tmp;

	for (j = 0; j < 512; j += 2) 
	{
		tmp = 0.7071 * *centre++ * gain;

		if (tmp > 32767)
			s16_samples[j] = s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j] = s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = s16_samples[j+1] = -32768;
	}
}

//-------------------------------------------------------------------------

static void downmix_3f_2r_to_2ch_dolby(double gain, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, ds_sur;
	double *centre = 0, *left = 0, *right = 0, *left_sur = 0, *right_sur = 0;

	left      = samples[0];
	centre    = samples[1];
	right     = samples[2];
	left_sur  = samples[3];
	right_sur = samples[4];

	for (j = 0; j < 512; j += 2) 
	{
		ds_sur = (ds_x2 + ds_x1 * 2.0 + *left_sur + *right_sur) * 0.12531781 + 
			ds_y1 * 0.77997062 - ds_y2 * 0.28124186;

		ds_x2 = ds_x1;
		ds_x1 = *left_sur++ + *right_sur++;
		ds_y2 = ds_y1;
		ds_y1 = ds_sur;

		tmp = (0.3204 * *left++ + 0.2265 * (*centre - ds_sur)) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;
		
		tmp = (0.3204 * *right++ + 0.2265 * (*centre++ + ds_sur)) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_2f_2r_to_2ch_dolby(double gain, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, ds_sur;
	double *left = 0, *right = 0, *left_sur = 0, *right_sur = 0;

	left      = samples[0];
	right     = samples[1];
	left_sur  = samples[2];
	right_sur = samples[3];

	for (j = 0; j < 512; j += 2) 
	{
		ds_sur = (ds_x2 + ds_x1 * 2.0 + *left_sur + *right_sur) * 0.12531781 + 
			ds_y1 * 0.77997062 - ds_y2 * 0.28124186;

		ds_x2 = ds_x1;
		ds_x1 = *left_sur++ + *right_sur++;
		ds_y2 = ds_y1;
		ds_y1 = ds_sur;

		tmp = (0.4142 * *left++ - 0.2928 * ds_sur) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + 0.2928 * ds_sur) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_3f_1r_to_2ch_dolby(double gain, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, ds_sur;
	double *centre = 0, *left = 0, *right = 0, *sur = 0;

	left      = samples[0];
	centre    = samples[1];
	right     = samples[2];
	sur       = samples[3];

	for (j = 0; j < 512; j += 2) 
	{
		ds_sur = (ds_x2 + ds_x1 * 2.0 + *sur) * 0.12531781 + 
			ds_y1 * 0.77997062 - ds_y2 * 0.28124186;

		ds_x2 = ds_x1;
		ds_x1 = *sur++;
		ds_y2 = ds_y1;
		ds_y1 = ds_sur;

		tmp = (0.4142 * *left++ + 0.2928 * (*centre - ds_sur)) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.4142 * *right++ + 0.2928 * (*centre++ + ds_sur)) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

static void downmix_2f_1r_to_2ch_dolby(double gain, stream_samples_t samples, S16 *s16_samples)
{
	U32 j;
	double tmp, ds_sur;
	double *left = 0, *right = 0, *sur = 0;

	left      = samples[0];
	right     = samples[1];
	sur	      = samples[2];

	for (j = 0; j < 512; j += 2) 
	{
		ds_sur = (ds_x2 + ds_x1 * 2.0 + *sur) * 0.12531781 + 
			ds_y1 * 0.77997062 - ds_y2 * 0.28124186;

		ds_x2 = ds_x1;
		ds_x1 = *sur++;
		ds_y2 = ds_y1;
		ds_y1 = ds_sur;

		tmp = (0.5858 * *left++ - 0.4142 * ds_sur) * gain;

		if (tmp > 32767)
			s16_samples[j] = 32767;
		else if (tmp > 0)
			s16_samples[j] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j] = (S16)(tmp - 0.5);
		else
			s16_samples[j] = -32768;

		tmp = (0.5858 * *right++ + 0.4142 * ds_sur) * gain;

		if (tmp > 32767)
			s16_samples[j+1] = 32767;
		else if (tmp > 0)
			s16_samples[j+1] = (S16)(tmp + 0.5);
		else if (tmp > -32768)
			s16_samples[j+1] = (S16)(tmp - 0.5);
		else
			s16_samples[j+1] = -32768;
	}
}

//-------------------------------------------------------------------------

void AC3_downmix(AC3_audblk_t *audblk, AC3_bsi_t* bsi, stream_samples_t samples, S16 *s16_samples)
{
	double gain;
	
	if (DRC_Flag > 2)
	{
		gain = 32767.0 * drc[bsi->compr];
	}
	else
	{
		gain = 32767.0 * drc[audblk->dynrng];
	}

	switch (bsi->acmod)
	{
	// 3/2
	case 7:
		if (DSDown_Flag)
		{
			downmix_3f_2r_to_2ch_dolby(gain, samples, s16_samples);
		}
		else
		{
			downmix_3f_2r_to_2ch(gain, bsi, samples, s16_samples);
		}
		break;

	// 2/2
	case 6:
		if (DSDown_Flag)
		{
			downmix_2f_2r_to_2ch_dolby(gain, samples, s16_samples);
		}
		else
		{
			downmix_2f_2r_to_2ch(gain, bsi, samples, s16_samples);
		}
		break;

	// 3/1
	case 5:
		if (DSDown_Flag)
		{
			downmix_3f_1r_to_2ch_dolby(gain, samples, s16_samples);
		}
		else
		{
			downmix_3f_1r_to_2ch(gain, bsi, samples, s16_samples);
		}
		break;

	// 2/1
	case 4:
		if (DSDown_Flag)
		{
			downmix_2f_1r_to_2ch_dolby(gain, samples, s16_samples);
		}
		else
		{
			downmix_2f_1r_to_2ch(gain, bsi, samples, s16_samples);
		}
		break;

	// 3/0
	case 3:
		downmix_3f_0r_to_2ch(gain, bsi, samples, s16_samples);
		break;

	// 2/0
	case 2:
		downmix_2f_0r_to_2ch(gain, samples, s16_samples);
		break;

	// 1/0, 1+1, discard dynrng2 :p
	case 1:
	case 0:
		downmix_1f_0r_to_2ch(gain, samples[0], s16_samples);
		break;
	default:
		break;
	}
}
