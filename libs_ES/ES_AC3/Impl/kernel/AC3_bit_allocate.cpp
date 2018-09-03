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
#include <string.h>

//#include "inttypes.h"
#include "../../Include/kernel/kernel_ac3_audio_decoder.h"

#define maxvalue(a, b) (a > b ? a : b)
#define minvalue(a, b) (a < b ? a : b)

__forceinline static S16 logadd(S16 a, S16 b);
static S16 calc_lowcomp(S16 a,S16 b0,S16 b1,S16 bin);

static void ba_compute_psd(S16 start, S16 end, U16 exps[], S16 psd[], S16 bndpsd[]);
static void ba_compute_excitation(S16 start, S16 end, S16 fgain, S16 sgain, S8 fdecay, S8 sdecay, S16 fastleak, S16 slowleak, S16 bndpsd[], S16 excite[]);
static void ba_compute_mask(S16 start, S16 end, U16 fscod, U8 deltbae, U8 deltnseg, S16	dbknee,
							U8 deltoffst[], U8 deltba[], U8 deltlen[], S16 bndpsd[], S16 excite[], S16 mask[]);
static void ba_compute_bap(S16 start, S16 end, S16 snroffset, S16 floor, S16 psd[], S16 mask[], U16 bap[]);

/* Misc LUTs for bit allocation process */
static U8  slowdec[]  = { 0x0f,  0x11,  0x13,  0x15  };
static U8  fastdec[]  = { 0x3f,  0x53,  0x67,  0x7b  };
static U16 slowgain[] = { 0x540, 0x4d8, 0x478, 0x410 };
static U16 dbpbtab[]  = { 0x000, 0x700, 0x900, 0xb00 };
static U16 floortab[] = { 0x2f0, 0x2b0, 0x270, 0x230, 0x1f0, 0x170, 0x0f0, 0xf800 };
static U16 fastgain[] = { 0x080, 0x100, 0x180, 0x200, 0x280, 0x300, 0x380, 0x400  };

static S16 bndtab[] = {   0,  1,  2,   3,   4,   5,   6,   7,   8,   9, 
						 10, 11, 12,  13,  14,  15,  16,  17,  18,  19,
						 20, 21, 22,  23,  24,  25,  26,  27,  28,  31,
						 34, 37, 40,  43,  46,  49,  55,  61,  67,  73,
						 79, 85, 97, 109, 121, 133, 157, 181, 205, 229};

static S16 bndsz[]  = {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
						 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
						 1,  1,  1,  1,  1,  1,  1,  1,  3,  3,
						 3,  3,  3,  3,  3,  6,  6,  6,  6,  6,
						 6, 12, 12, 12, 12, 24, 24, 24, 24, 24 };

static S16 masktab[] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
						 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 28, 28, 29,
						 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34,
						 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37,
						 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40,
						 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
						 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 43, 43,
						 43, 43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 44,
						 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
						 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 46,
						 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
						 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
						 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 48, 48, 48,
						 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
						 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
						 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  0,  0,  0 };


static S16 latab[] = {  0x0040, 0x003f, 0x003e, 0x003d, 0x003c, 0x003b, 0x003a, 0x0039,
						0x0038, 0x0037, 0x0036, 0x0035, 0x0034, 0x0034, 0x0033, 0x0032,
						0x0031, 0x0030, 0x002f, 0x002f, 0x002e, 0x002d, 0x002c, 0x002c,
						0x002b, 0x002a, 0x0029, 0x0029, 0x0028, 0x0027, 0x0026, 0x0026,
						0x0025, 0x0024, 0x0024, 0x0023, 0x0023, 0x0022, 0x0021, 0x0021,
						0x0020, 0x0020, 0x001f, 0x001e, 0x001e, 0x001d, 0x001d, 0x001c,
						0x001c, 0x001b, 0x001b, 0x001a, 0x001a, 0x0019, 0x0019, 0x0018,
						0x0018, 0x0017, 0x0017, 0x0016, 0x0016, 0x0015, 0x0015, 0x0015,
						0x0014, 0x0014, 0x0013, 0x0013, 0x0013, 0x0012, 0x0012, 0x0012,
						0x0011, 0x0011, 0x0011, 0x0010, 0x0010, 0x0010, 0x000f, 0x000f,
						0x000f, 0x000e, 0x000e, 0x000e, 0x000d, 0x000d, 0x000d, 0x000d,
						0x000c, 0x000c, 0x000c, 0x000c, 0x000b, 0x000b, 0x000b, 0x000b,
						0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x0009, 0x0009, 0x0009,
						0x0009, 0x0009, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
						0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0007, 0x0006, 0x0006,
						0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x0005, 0x0005,
						0x0005, 0x0005, 0x0005, 0x0005, 0x0005, 0x0005, 0x0004, 0x0004,
						0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004,
						0x0004, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003,
						0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0002,
						0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
						0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
						0x0002, 0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
						0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
						0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
						0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
						0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
						0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
						0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
						0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
						0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
						0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
						0x0000, 0x0000, 0x0000, 0x0000
					};

static S16 hth[][50] = {{ 0x04d0, 0x04d0, 0x0440, 0x0400, 0x03e0, 0x03c0, 0x03b0, 0x03b0,  
						  0x03a0, 0x03a0, 0x03a0, 0x03a0, 0x03a0, 0x0390, 0x0390, 0x0390,  
						  0x0380, 0x0380, 0x0370, 0x0370, 0x0360, 0x0360, 0x0350, 0x0350,  
						  0x0340, 0x0340, 0x0330, 0x0320, 0x0310, 0x0300, 0x02f0, 0x02f0,
						  0x02f0, 0x02f0, 0x0300, 0x0310, 0x0340, 0x0390, 0x03e0, 0x0420,
						  0x0460, 0x0490, 0x04a0, 0x0460, 0x0440, 0x0440, 0x0520, 0x0800,
						  0x0840, 0x0840 
						},
						{ 0x04f0, 0x04f0, 0x0460, 0x0410, 0x03e0, 0x03d0, 0x03c0, 0x03b0, 
						  0x03b0, 0x03a0, 0x03a0, 0x03a0, 0x03a0, 0x03a0, 0x0390, 0x0390, 
						  0x0390, 0x0380, 0x0380, 0x0380, 0x0370, 0x0370, 0x0360, 0x0360, 
						  0x0350, 0x0350, 0x0340, 0x0340, 0x0320, 0x0310, 0x0300, 0x02f0, 
						  0x02f0, 0x02f0, 0x02f0, 0x0300, 0x0320, 0x0350, 0x0390, 0x03e0, 
						  0x0420, 0x0450, 0x04a0, 0x0490, 0x0460, 0x0440, 0x0480, 0x0630, 
						  0x0840, 0x0840 
						},
						{ 0x0580, 0x0580, 0x04b0, 0x0450, 0x0420, 0x03f0, 0x03e0, 0x03d0, 
						  0x03c0, 0x03b0, 0x03b0, 0x03b0, 0x03a0, 0x03a0, 0x03a0, 0x03a0, 
						  0x03a0, 0x03a0, 0x03a0, 0x03a0, 0x0390, 0x0390, 0x0390, 0x0390, 
						  0x0380, 0x0380, 0x0380, 0x0370, 0x0360, 0x0350, 0x0340, 0x0330, 
						  0x0320, 0x0310, 0x0300, 0x02f0, 0x02f0, 0x02f0, 0x0300, 0x0310, 
						  0x0330, 0x0350, 0x03c0, 0x0410, 0x0470, 0x04a0, 0x0460, 0x0440, 
						  0x0450, 0x04e0 
						}};

static S16 baptab[] = {  0,  1,  1,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,
						 6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 
						10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14,
						14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15 };

static S16 logadd(S16 a, S16 b) 
{ 
	S16 c;
	S16 address;

	c = a - b; 
	address = minvalue((abs(c) >> 1), 255); 
	
	if (c >= 0)
	{
		return(a + latab[address]); 
	}
	else 
	{
		return(b + latab[address]); 
	}
}

/*------------------------------------------------------------------------------------------------
	函数：AC3_bit_allocate()

	描述：解析比特分配信息

	输入：fscod			-- sampling frequency coding
		  bsi			-- bit stream information
		  audblk		-- syntax part of audio block
	输出：audblk		-- sematic part of audio block
	返回：void

	维护：

			2006.03.22	chendelin		从参考程序移植
------------------------------------------------------------------------------------------------*/
S32 AC3_bit_allocate(U8 fscod, AC3_bsi_t *pbsi, AC3_audblk_t *paudblk)
{
	S32 error = 0;
	S32 ch;

	S8	sdecay;
	S8	fdecay;
	S16	sgain;
	S16	dbknee;
	S16	floor;

	S16 fgain;
	S16 snroffset;
	S16 start;
	S16 end;
	S16 lowcomp;
	S16 fastleak;
	S16 slowleak;

	S16	psd[256];
	S16	bndpsd[256];
//	S16	excite[256];
//	S16	mask[256];

	if ((paudblk->chexpstr[0]  == 0) && (paudblk->chexpstr[1] == 0) &&
		(paudblk->chexpstr[2]  == 0) && (paudblk->chexpstr[3] == 0) &&
		(paudblk->chexpstr[4]  == 0) && (paudblk->cplexpstr   == 0) &&
		(paudblk->lfeexpstr    == 0) && (paudblk->baie        == 0) &&
		(paudblk->snroffste    == 0) && (paudblk->deltbaie    == 0))
	{
		/* Only perform bit_allocation if the exponents have changed or we
		 * have new sideband information */
		error = 1;
	}
	else if ((paudblk->csnroffst    == 0) && (paudblk->fsnroffst[0] == 0) && 
			 (paudblk->fsnroffst[1] == 0) && (paudblk->fsnroffst[2] == 0) && 
			 (paudblk->fsnroffst[3] == 0) && (paudblk->fsnroffst[4] == 0) &&
			 (paudblk->cplfsnroffst == 0) && (paudblk->lfefsnroffst == 0))
	{
		/* if all the SNR offset constants are zero then the whole block is zero */
		memset(paudblk->fbw_bap, 0, sizeof(U16) * 256 * 5);
		memset(paudblk->cpl_bap, 0, sizeof(U16) * 256);
		memset(paudblk->lfe_bap, 0, sizeof(U16) * 7);

		error = 1;
	}
	else
	{
		/* Do some setup before we do the bit alloc */
		sdecay = slowdec[paudblk->sdcycod]; 
		fdecay = fastdec[paudblk->fdcycod];
		sgain = slowgain[paudblk->sgaincod]; 
		dbknee = dbpbtab[paudblk->dbpbcod]; 
		floor = floortab[paudblk->floorcod]; 

		/* Initialize as follows for uncoupled portion of fbw channel*/
		for (ch = 0; ch < pbsi->nfchans; ch++)
		{
			start = paudblk->strtmant[ch];
			end = paudblk->endmant[ch];
			lowcomp = 0;
			fgain = fastgain[paudblk->fgaincod[ch]]; 
			snroffset = (((paudblk->csnroffst - 15) << 4) + paudblk->fsnroffst[ch]) << 2;

			ba_compute_psd(start, end, paudblk->fbw_exp[ch], psd, bndpsd);

			fastleak = 0;
			slowleak = 0;

	//		ba_compute_excitation(start, end, fgain, sgain, fdecay, sdecay, fastleak, slowleak, bndpsd, excite);

	//		ba_compute_mask(start, end, fscod, paudblk->deltbae[ch], paudblk->deltnseg[ch], dbknee, 
	//				paudblk->deltoffst[ch], paudblk->deltba[ch], paudblk->deltlen[ch], bndpsd, excite, mask);

	//		ba_compute_bap(start, end, snroffset, floor, psd, mask, paudblk->fbw_bap[ch]);
		}
	
		/* Initialize as follows for coupling channel*/
		if (paudblk->cplinu)
		{
			start = paudblk->cplstrtmant; 
			end = paudblk->cplendmant; 
			fgain = fastgain[paudblk->cplfgaincod];
			snroffset = (((paudblk->csnroffst - 15) << 4) + paudblk->cplfsnroffst) << 2 ;

			if (paudblk->cplleake)
			{
				fastleak = (paudblk->cplfleak << 8) + 768; 
				slowleak = (paudblk->cplsleak << 8) + 768;
			}
			else
			{
				fastleak = 0;
				slowleak = 0;
			}

//			ba_compute_psd(start, end, paudblk->cpl_exp, psd, bndpsd);
//			ba_compute_excitation(start, end, fgain, sgain, fdecay, sdecay, fastleak, slowleak, bndpsd, excite);

//			ba_compute_mask(start, end, fscod, paudblk->cpldeltbae, paudblk->cpldeltnseg, dbknee,
//					paudblk->cpldeltoffst, paudblk->cpldeltba, paudblk->cpldeltlen, bndpsd, excite, mask);

//			ba_compute_bap(start, end, snroffset, floor, psd, mask, paudblk->cpl_bap);
		}

		/* Initialize as follows for lfe channel*/
		if (pbsi->lfeon)
		{
			start = paudblk->lfestartmant;
			end = paudblk->lfeendmant;
			lowcomp = 0;
			fgain = fastgain[paudblk->lfefgaincod];
			snroffset = (((paudblk->csnroffst - 15) << 4) + paudblk->lfefsnroffst) << 2 ;

//			ba_compute_psd(start, end, paudblk->lfe_exp, psd, bndpsd);

			fastleak = 0;
			slowleak = 0;

	//		ba_compute_excitation(start, end, fgain, sgain, fdecay, sdecay, fastleak, slowleak, bndpsd, excite);

			/* Perform no delta bit allocation for lfe */
	//		ba_compute_mask(start, end, fscod, 2, 0, dbknee, 0, 0, 0, bndpsd, excite, mask);

	//		ba_compute_bap(start, end, snroffset, floor, psd, mask, paudblk->lfe_bap);
		}
	}

	return error;
}

static void ba_compute_psd(S16 start, S16 end, U16 exps[], S16 psd[], S16 bndpsd[])
{
	S32 bin, j, k;
//	S32	i;
	
	/* Map the exponents into dBs */
	for (bin = start; bin < end; bin++) 
	{ 
		psd[bin] = (3072 - (exps[bin] << 7)); 
	}

	/* Integrate the psd function over each bit allocation band */

	j = start; 
	k = masktab[start]; 
/*	
	do 
	{ 
		bndpsd[k] = psd[j]; 
		j++; 

		for (i = j; i < minvalue(bndtab[k + 1], end); i++) 
		{ 
			bndpsd[k] = logadd(bndpsd[k], psd[j]);
			j++; 
		} 
		
		k++;
		
	} while (end > bndtab[k++]);
*/
}

static void ba_compute_excitation(S16 start, S16 end, S16 fgain, S16 sgain,
								    S8 fdecay, S8 sdecay,
									S16 fastleak, S16 slowleak,
									S16 bndpsd[], S16 excite[])
{
	S32 bin;
	S16 bndstrt;
	S16 bndend;
	S16 lowcomp = 0;
	S16 begin = 0;

	/* Compute excitation function */
	bndstrt = masktab[start]; 
	bndend = masktab[end - 1] + 1; 
	
	if (bndstrt == 0) /* For fbw and lfe channels */ 
	{ 
		lowcomp = calc_lowcomp(lowcomp, bndpsd[0], bndpsd[1], 0); 
		excite[0] = bndpsd[0] - fgain - lowcomp; 
		lowcomp = calc_lowcomp(lowcomp, bndpsd[1], bndpsd[2], 1);
		excite[1] = bndpsd[1] - fgain - lowcomp; 
		begin = 7 ; 
		
		/* Note: Do not call calc_lowcomp() for the last band of the lfe channel, (bin = 6) */ 
		for (bin = 2; bin < 7; bin++) 
		{
			if (bndend!= 7 || bin!=6)
				lowcomp = calc_lowcomp(lowcomp, bndpsd[bin], bndpsd[bin+1], bin); 

			fastleak = bndpsd[bin] - fgain; 
			slowleak = bndpsd[bin] - sgain; 
			excite[bin] = fastleak - lowcomp; 

			if (bndend!= 7 || bin!=6)
			{
				if (bndpsd[bin] <= bndpsd[bin+1]) 
				{
					begin = bin + 1 ; 
					break; 
				} 
			}
		} 
		
		for (bin = begin; bin < minvalue(bndend, 22); bin++) 
		{
			if (bndend!= 7 || bin!=6)
			{
				lowcomp = calc_lowcomp(lowcomp, bndpsd[bin], bndpsd[bin+1], bin); 
			}
			fastleak -= fdecay ; 
			fastleak = maxvalue(fastleak, bndpsd[bin] - fgain); 
			slowleak -= sdecay ; 
			slowleak = maxvalue(slowleak, bndpsd[bin] - sgain); 
			excite[bin] = maxvalue(fastleak - lowcomp, slowleak); 
		} 
		begin = 22; 
	} 
	else /* For coupling channel */ 
	{ 
		begin = bndstrt; 
	} 

	for (bin = begin; bin < bndend; bin++) 
	{ 
		fastleak -= fdecay; 
		fastleak = maxvalue(fastleak, bndpsd[bin] - fgain); 
		slowleak -= sdecay; 
		slowleak = maxvalue(slowleak, bndpsd[bin] - sgain); 
		excite[bin] = maxvalue(fastleak, slowleak) ; 
	}
}

static void ba_compute_mask(S16 start, S16 end, U16 fscod, U8 deltbae, U8 deltnseg, S16	dbknee,
							U8 deltoffst[], U8 deltba[], U8 deltlen[], S16 bndpsd[], S16 excite[], S16 mask[])
{
	S32 bin,k;
	S16 bndstrt;
	S16 bndend;
	S16 delta;

	bndstrt = masktab[start]; 
	bndend = masktab[end - 1] + 1; 

	/* Compute the masking curve */

	for (bin = bndstrt; bin < bndend; bin++) 
	{ 
		if (bndpsd[bin] < dbknee) 
		{ 
			excite[bin] += ((dbknee - bndpsd[bin]) >> 2); 
		} 
		mask[bin] = maxvalue(excite[bin], hth[fscod][bin]);
	}
	
	/* Perform delta bit modulation if necessary */

	if ((deltbae == DELTA_BIT_REUSE) || (deltbae == DELTA_BIT_NEW)) 
	{ 
		S16 band = 0; 
		S16 seg = 0; 
		
		for (seg = 0; seg < deltnseg+1; seg++) 
		{ 
			band += deltoffst[seg]; 
			if (deltba[seg] >= 4) 
			{ 
				delta = (deltba[seg] - 3) << 7;
			} 
			else 
			{ 
				delta = (deltba[seg] - 4) << 7;
			} 
			
			for (k = 0; k < deltlen[seg]; k++) 
			{ 
				mask[band] += delta; 
				band++; 
			} 
		} 
	}
}

static void ba_compute_bap(S16 start, S16 end, S16 snroffset, S16 floor, S16 psd[], S16 mask[], U16 bap[])
{
	S32 i,j,k;
	S16 lastbin = 0;
	S16 address = 0;

	/* Compute the bit allocation pointer for each bin */
	i = start; 
	j = masktab[start]; 

	do 
	{ 
		lastbin = minvalue(bndtab[j] + bndsz[j], end); 
		mask[j] -= snroffset; 
		mask[j] -= floor; 
		
		if (mask[j] < 0) 
			mask[j] = 0; 

		mask[j] &= 0x1fe0;
		mask[j] += floor; 
		for (k = i; k < lastbin; k++) 
		{ 
			address = (psd[i] - mask[j]) >> 5; 
			address = minvalue(63, maxvalue(0, address)); 
			bap[i] = baptab[address]; 
			i++; 
		} 
		j++; 
	} while (end > lastbin);
}

static S16 calc_lowcomp(S16 a, S16 b0, S16 b1, S16 bin) 
{ 

	if (bin < 7) 
	{ 
		if ((b0 + 256) == b1)
			a = 384; 
	 	else if (b0 > b1) 
			a = maxvalue(0, a - 64); 
	} 
	else if (bin < 20) 
	{ 
		if ((b0 + 256) == b1) 
			a = 320; 
		else if (b0 > b1) 
			a = maxvalue(0, a - 64) ; 
	}
	else  
		a = maxvalue(0, a - 128); 
	
	return(a);
}
