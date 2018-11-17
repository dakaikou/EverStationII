#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "../../Stream_AL/BitStream.h"

#include "../Include/ac3_audio_sequence.h"
#include "../Include/ac3_audio_errorcode.h"
#include "./kernel/AC3_audiotables.h"

extern int  AC3_bit_allocate(uint8_t fscod, AC3_bsi_t *bsi, AC3_audblk_t *audblk);
extern void AC3_drc_init(void);
extern void AC3_imdct_init(void);
//extern void AC3_coeff_unpack(FIFO_t* pbs, AC3_bsi_t *bsi, AC3_audblk_t *audblk, stream_samples_t samples);
extern void AC3_rematrix(AC3_audblk_t *audblk, stream_samples_t samples);
extern void AC3_imdct(AC3_bsi_t *bsi, AC3_audblk_t *audblk, stream_samples_t samples);
extern void AC3_downmix(AC3_audblk_t *audblk, AC3_bsi_t* bsi, stream_samples_t samples, short *s16_samples);
extern int  AC3_exp_unpack(uint8_t expstr, int absexp, char* exps, uint8_t ngrps, uint16_t *dest);

extern int AC3_sanity_check(AC3_syncinfo_t *syncinfo, AC3_bsi_t *bsi, AC3_audblk_t *audblk);

//static AC3_syncinfo_t			syncinfo;
//static AC3_bsi_t				bsi;
//static AC3_audblk_t				audblk;

//AC3_syncinfo_t* ac3_audio_get_syncinfo(void)
//{
//	return &syncinfo;
//}

int	ac3_audio_decode_syncinfo(unsigned char* syncinfo_buf, int syncinfo_length, AC3_syncinfo_t* pac3_syncinfo)
{
	BITS_t	bs;
	int		rtcode = AC3_UNKNOWN_ERROR;
	int     header_size = 4;

	if ((syncinfo_buf != NULL) && (syncinfo_length >= 5) && (pac3_syncinfo != NULL))
	{
		memset(pac3_syncinfo, 0x00, sizeof(AC3_syncinfo_t));
		BITS_map(&bs, syncinfo_buf, syncinfo_length);

		pac3_syncinfo->syncword = BITS_get(&bs, 16);
		pac3_syncinfo->crc1 = BITS_get(&bs, 16);
		pac3_syncinfo->fscod = BITS_get(&bs, 2);
		pac3_syncinfo->frmsizecod = BITS_get(&bs, 6);

		rtcode = AC3_NO_ERROR;
	}
	else
	{
		rtcode = AC3_PARAMETER_ERROR;
	}

	return rtcode;
}

//int	ac3_audio_decode_bsi(FIFO_t* pbs)
//{
//	int		i;
//
//	bsi.bsid = FIFO_Get(pbs, 5);
//	bsi.bsmod = FIFO_Get(pbs, 3);
//	bsi.acmod = FIFO_Get(pbs, 3);
//
//	if ((bsi.acmod & 0x01) && (bsi.acmod != 0x01))
//	{
//		bsi.cmixlev = FIFO_Get(pbs, 2);
//	}
//
//	if (bsi.acmod & 0x04)
//	{
//		bsi.surmixlev = FIFO_Get(pbs, 2);
//	}
//
//	if (bsi.acmod == 0x02)
//	{
//		bsi.dsurmod = FIFO_Get(pbs, 2);
//	}
//
//	bsi.lfeon = FIFO_Get(pbs, 1);
//
//	bsi.dialnorm = FIFO_Get(pbs, 5);
//
//	bsi.compre = FIFO_Get(pbs, 1);
//	if (bsi.compre)
//	{
//		bsi.compr = FIFO_Get(pbs, 8);
//	}
//
//	bsi.langcode = FIFO_Get(pbs, 1);
//	if (bsi.langcode)
//	{
//		bsi.langcod = FIFO_Get(pbs, 8);
//	}
//
//	bsi.audprodie = FIFO_Get(pbs, 1);
//	if (bsi.audprodie)
//	{
//		bsi.mixlevel = FIFO_Get(pbs, 5);
//		bsi.roomtyp = FIFO_Get(pbs, 2);
//	}
//
//	if (bsi.acmod == 0)
//	{
//		bsi.dialnorm2 = FIFO_Get(pbs, 5);
//
//		bsi.compr2e = FIFO_Get(pbs, 1);
//		if (bsi.compr2e)
//		{
//			bsi.compr2 = FIFO_Get(pbs, 8);
//		}
//
//		bsi.langcod2e = FIFO_Get(pbs, 1);
//		if (bsi.langcod2e)
//		{
//			bsi.langcod2 = FIFO_Get(pbs, 8);
//		}
//
//		bsi.audprodi2e = FIFO_Get(pbs, 1);
//		if (bsi.audprodi2e)
//		{
//			bsi.mixlevel2 = FIFO_Get(pbs, 5);
//			bsi.roomtyp2 = FIFO_Get(pbs, 2);
//		}
//	}
//
//	bsi.copyrightb = FIFO_Get(pbs, 1);
//	bsi.origbs = FIFO_Get(pbs, 1);
//	bsi.timecod1e = FIFO_Get(pbs, 1);
//	if (bsi.timecod1e)
//	{
//		bsi.timecod1 = FIFO_Get(pbs, 14);
//	}
//
//	bsi.timecod2e = FIFO_Get(pbs, 1);
//	if (bsi.timecod2e)
//	{
//		bsi.timecod2 = FIFO_Get(pbs, 14);
//	}
//
//	bsi.addbsie = FIFO_Get(pbs, 1);
//	if (bsi.addbsie)
//	{
//		bsi.addbsil = FIFO_Get(pbs, 6);
//
//		for (i = 0; i < (bsi.addbsil + 1); i++)
//		{
//			bsi.addbsi[i] = FIFO_Get(pbs, 8);
//		}
//	}
//
//	bsi.nfchans = nfchans_table[bsi.acmod];
//
//	return 0;
//}

//int	ac3_audio_decode_audblk(FIFO_t* pbs)
//{
//	int		error = 0;
//
//	int		ch;
//	int		bnd;
//	int		rbnd;
//	int		grp;
//	int		seg;
//	int		i;
//	uint16_t		grp_size;
//
//	/* These fields for block switch and dither flags */
//	for (ch = 0; ch < bsi.nfchans; ch++)
//	{
//		audblk.blksw[ch] = FIFO_Get(pbs, 1); 
//	}
//
//	for (ch = 0; ch < bsi.nfchans; ch++)
//	{
//		audblk.dithflag[ch] = FIFO_Get(pbs, 1); 
//	}
//
//	/* These fields for dynamic range control */
//	audblk.dynrnge = FIFO_Get(pbs, 1);
//	if (audblk.dynrnge)
//	{
//		audblk.dynrng = FIFO_Get(pbs, 8);
//	}
//
//	if (bsi.acmod == 0)/* if 1+1 mode */
//	{
//		audblk.dynrng2e = FIFO_Get(pbs, 1);
//		if (audblk.dynrng2e)
//		{
//			audblk.dynrng2 = FIFO_Get(pbs, 8);
//		}
//	}
//
//	/* These fields for coupling strategy information */
//	audblk.cplstre = FIFO_Get(pbs, 1);
//	if (audblk.cplstre)
//	{
//		audblk.cplinu = FIFO_Get(pbs, 1);
//		if (audblk.cplinu)
//		{
//			for (ch = 0; ch < bsi.nfchans; ch++)
//			{
//				audblk.chincpl[ch] = FIFO_Get(pbs, 1);
//			}
//
//			if (bsi.acmod == 0x2)/* if in 2/0 mode*/
//			{
//				audblk.phsflginu = FIFO_Get(pbs, 1);
//			}
//
//			audblk.cplbegf = FIFO_Get(pbs, 4);			//subband index
//			audblk.cplendf = FIFO_Get(pbs, 4);			//subband index: + 2
//
//			audblk.ncplsubnd = 3 + audblk.cplendf - audblk.cplbegf;
//
////			audblk.cplstrtbnd = bndtab[audblk.cplbegf];
//
//			/* Calculate the start and end bins of the coupling channel */
//			audblk.cplstrtmant = (audblk.cplbegf * 12) + 37 ; 
//			audblk.cplendmant =  ((audblk.cplendf + 3) * 12) + 37;
//			
//			audblk.ncplbnd = audblk.ncplsubnd;
//
//			audblk.cplbndstrc[0] = 0;				//not informed by AC3 standards
//			for (bnd = 1; bnd < audblk.ncplsubnd; bnd++)
//			{
//				audblk.cplbndstrc[bnd] = FIFO_Get(pbs, 1);
//				audblk.ncplbnd -= audblk.cplbndstrc[bnd];
//			}
//		}
//	}
//
//	/* These fields for coupling coordinates, phase flags */
//	if (audblk.cplinu)
//	{
//		for (ch = 0; ch < bsi.nfchans; ch++)
//		{
//			if (audblk.chincpl[ch])
//			{
//				audblk.cplcoe[ch] = FIFO_Get(pbs, 1);
//				if (audblk.cplcoe[ch])
//				{
//					audblk.mstrcplco[ch] = FIFO_Get(pbs, 2);
//					/* ncplbnd derived from ncplsubnd, and cplbndstrc */
//					for (bnd = 0; bnd < audblk.ncplbnd; bnd++)
//					{
//						audblk.cplcoexp[ch][bnd] = FIFO_Get(pbs, 4);
//						audblk.cplcomant[ch][bnd] = FIFO_Get(pbs, 4);
//
//						if (audblk.cplcoexp[ch][bnd] == 15)
//						{
//							audblk.cplcomant[ch][bnd] <<= 14;
////							audblk.cplcomant[ch][bnd] /= 16;
//						}
//						else 
//						{
//							audblk.cplcomant[ch][bnd] = (audblk.cplcomant[ch][bnd] | 0x10) << 13;
//						}
//
////						audblk.cplco[ch][bnd] = audblk.cplcomant * scale_factor[audblk.cplcoexp + 3 * audblk.mstrcplco];
//					} 	
//				}
//			}
//		}
//
//		if ((bsi.acmod == 0x2) && audblk.phsflginu && (audblk.cplcoe[0] || audblk.cplcoe[1]))
//		{
//			for (bnd = 0; bnd < audblk.ncplbnd; bnd++)
//			{
//				audblk.phsflg[bnd] = FIFO_Get(pbs, 1);
//
//				if (audblk.phsflg[bnd])
//				{
//					audblk.cplco[1][bnd] = -audblk.cplco[1][bnd];
//				}
//			}
//		}
//	}
//
//	/* These fields for rematrixing operation in the 2/0 mode */
//	if (bsi.acmod == 0x2)		/*if in 2/0 mode*/
//	{
//		audblk.rematstr = FIFO_Get(pbs, 1);
//		if (audblk.rematstr)
//		{
//			if ((audblk.cplbegf > 2) || (audblk.cplinu == 0))
//			{
//				for (rbnd = 0; rbnd < 4; rbnd ++)
//				{
//					audblk.rematflg[rbnd] = FIFO_Get(pbs, 1);
//				}
//			}
//			if (((audblk.cplbegf > 0) && (audblk.cplbegf <= 2)) && audblk.cplinu)
//			{
//				for (rbnd = 0; rbnd < 3; rbnd ++)
//				{
//					audblk.rematflg[rbnd] = FIFO_Get(pbs, 1);
//				}
//			}
//			if ((audblk.cplbegf == 0) && audblk.cplinu)
//			{
//				for (rbnd = 0; rbnd < 2; rbnd ++)
//				{
//					audblk.rematflg[rbnd] = FIFO_Get(pbs, 1);
//				}
//			}
//		}
//	}
//
//	audblk.cplexpstr = EXP_REUSE;
//	audblk.lfeexpstr = EXP_REUSE;
//
//	/* These fields for exponent strategy */
//	if (audblk.cplinu)
//	{
//		audblk.cplexpstr = FIFO_Get(pbs, 2);
//	}
//	
//	for (ch = 0; ch < bsi.nfchans; ch++)
//	{
//		audblk.chexpstr[ch] = FIFO_Get(pbs, 2);
//	}
//	
//	if (bsi.lfeon)
//	{
//		audblk.lfeexpstr = FIFO_Get(pbs, 1);
//	}
//	
//	for (ch = 0; ch < bsi.nfchans; ch++)
//	{
//		if(audblk.chexpstr[ch] != EXP_REUSE)			//reuse
//		{
//			audblk.strtmant[ch] = 0;
//
//			if (audblk.cplinu && audblk.chincpl[ch]) 
//			{
//				audblk.endmant[ch] = audblk.cplstrtmant;
////				audblk.endmant[ch] = 37 + (12 * audblk.cplbegf);
//			}
//			else
//			{
//				audblk.chbwcod[ch] = FIFO_Get(pbs, 6); 
//				audblk.endmant[ch] = ((audblk.chbwcod[ch] + 12) * 3) + 37;
//			}
//		}
//	}
//
//	/*these fields for exponent*/
//
//	/*exponents for the coupling channel*/
//	if (audblk.cplinu)
//	{
//		if (audblk.cplexpstr != EXP_REUSE)
//		{
//			audblk.ncplgrps = (audblk.cplendmant - audblk.cplstrtmant) / (3 << (audblk.cplexpstr-1));
//			
//			audblk.cplabsexp = FIFO_Get(pbs, 4);
//
//			/*ncplgrps derived from ncplsubnd, cplexpstr*/
//			for (grp = 0; grp < audblk.ncplgrps; grp ++)
//			{
//				audblk.cplexps[grp] = FIFO_Get(pbs, 7);
//			}
//
//			AC3_exp_unpack(audblk.cplexpstr, (audblk.cplabsexp << 1), audblk.cplexps, audblk.ncplgrps, audblk.cpl_exp);
//		}
//	}
//
//	/*exponents for the full bandwidth channels*/
//	for (ch = 0; ch < bsi.nfchans; ch ++)
//	{
//		if (audblk.chexpstr[ch] != EXP_REUSE)
//		{
//			/* Calculate the number of exponent groups to fetch */
//			grp_size =  3 << (audblk.chexpstr[ch] - 1);
//			audblk.nchgrps[ch] = (audblk.endmant[ch] - 1 + (grp_size - 3)) / grp_size;
//
//			audblk.exps[ch][0] = FIFO_Get(pbs, 4);
//			for (grp = 1; grp <= audblk.nchgrps[ch]; grp++)
//			{
//				audblk.exps[ch][grp] = FIFO_Get(pbs, 7);
//			}
//			AC3_exp_unpack(audblk.chexpstr[ch], audblk.exps[ch][0], &(audblk.exps[ch][1]), audblk.nchgrps[ch], audblk.fbw_exp[ch]);
//
//			audblk.gainrng[ch] = FIFO_Get(pbs, 2);
//		}
//	}
//
//	/*exponents for the low frequency effects channel*/
//	if (bsi.lfeon)
//	{
//		if (audblk.lfeexpstr != EXP_REUSE)
//		{
//			audblk.lfestartmant = 0;
//			audblk.lfeendmant = 7;
//
//			audblk.lfeexps[0] = FIFO_Get(pbs, 4);
//
//			audblk.nlfegrps = 2;
//			for (grp = 1; grp <= audblk.nlfegrps; grp ++)
//			{
//				audblk.lfeexps[grp] = FIFO_Get(pbs, 7);
//			}
//			AC3_exp_unpack(audblk.lfeexpstr, audblk.lfeexps[0], &(audblk.lfeexps[1]), audblk.nlfegrps, audblk.lfe_exp);
//		}
//	}
//
//	audblk.baie = FIFO_Get(pbs, 1);
//	if (audblk.baie)
//	{
//		audblk.sdcycod = FIFO_Get(pbs, 2);
//		audblk.fdcycod = FIFO_Get(pbs, 2);
//		audblk.sgaincod = FIFO_Get(pbs, 2);
//		audblk.dbpbcod = FIFO_Get(pbs, 2);
//		audblk.floorcod = FIFO_Get(pbs, 3);
//	}
//
//	/* Get the SNR off set info if it exists */
//	audblk.snroffste = FIFO_Get(pbs, 1);
//
//	/*these fields for bit-allocation parametric information*/
//	if(audblk.snroffste)
//	{
//		audblk.csnroffst = FIFO_Get(pbs, 6);
//
//		if(audblk.cplinu)
//		{
//			audblk.cplfsnroffst = FIFO_Get(pbs, 4);
//			audblk.cplfgaincod = FIFO_Get(pbs, 3);
//		}
//
//		for (ch = 0; ch < bsi.nfchans; ch++)
//		{
//			audblk.fsnroffst[ch] = FIFO_Get(pbs, 4);
//			audblk.fgaincod[ch] = FIFO_Get(pbs, 3);
//		}
//
//		if (bsi.lfeon)
//		{
//			audblk.lfefsnroffst = FIFO_Get(pbs, 4);
//			audblk.lfefgaincod = FIFO_Get(pbs, 3);
//		}
//	}
//
//	/* Get coupling leakage info if it exists */
//	if(audblk.cplinu)
//	{
//		audblk.cplleake = FIFO_Get(pbs, 1);	
//		
//		if(audblk.cplleake)
//		{
//			audblk.cplfleak = FIFO_Get(pbs, 3);
//			audblk.cplsleak = FIFO_Get(pbs, 3);
//		}
//	}
//	
//	/* Get the delta bit alloaction info */
//	audblk.deltbaie = FIFO_Get(pbs, 1);	
//	
//	if(audblk.deltbaie)
//	{
//		if (audblk.cplinu)
//		{
//			audblk.cpldeltbae = FIFO_Get(pbs, 2);
//		}
//
//		for (ch = 0; ch < bsi.nfchans; ch++)
//		{
//			audblk.deltbae[ch] = FIFO_Get(pbs, 2);
//		}
//
//		if (audblk.cplinu)
//		{
//			if (audblk.cpldeltbae == DELTA_BIT_NEW)
//			{
//				audblk.cpldeltnseg = FIFO_Get(pbs, 3);
//				for (seg = 0; seg <= audblk.cpldeltnseg; seg++)
//				{
//					audblk.cpldeltoffst[seg] = FIFO_Get(pbs, 5);
//					audblk.cpldeltlen[seg] = FIFO_Get(pbs, 4);
//					audblk.cpldeltba[seg] = FIFO_Get(pbs, 3);
//				}
//			}
//		}
//
//		for (ch = 0; ch < bsi.nfchans; ch++)
//		{
//			if (audblk.deltbae[ch] == DELTA_BIT_NEW)
//			{
//				audblk.deltnseg[ch] = FIFO_Get(pbs, 3);
//				for(seg = 0; seg <= audblk.deltnseg[ch]; seg++)
//				{
//					audblk.deltoffst[ch][seg] = FIFO_Get(pbs, 5);
//					audblk.deltlen[ch][seg] = FIFO_Get(pbs, 4);
//					audblk.deltba[ch][seg] = FIFO_Get(pbs, 3);
//				}
//			}
//		}
//	}
//
//	/* Check to see if there's any dummy info to get */
//	audblk.skiple = FIFO_Get(pbs, 1);
//	if (audblk.skiple)
//	{
//		uint8_t skip_data;
//
//		audblk.skipl = FIFO_Get(pbs, 9);
//
//		for (i = 0; i < audblk.skipl; i++)
//		{
//			skip_data = FIFO_Get(pbs, 8);
//		}
//	}
//
//	error = AC3_bit_allocate(syncinfo.fscod, &bsi, &audblk);
//	if (error == 0)
//	{
//	//	AC3_coeff_unpack(pbs, pbsi, paudblk, samples);
//
//	//	if (bsi.acmod == 0x2)
//	//	{
//	//		AC3_rematrix(&audblk, samples);
//	//	}
//	}
//
//	return error;
//}

//int	ac3_audio_init(void)
//{
//	memset(&syncinfo, 0, sizeof(AC3_syncinfo_t));
//	memset(&bsi, 0, sizeof(AC3_bsi_t));
//	memset(&audblk, 0, sizeof(AC3_audblk_t));
//
//	return 0;
//}
//
//int	ac3_audio_term(void)
//{
//	return 0;
//}
//
//int	ac3_audio_open(AC3_syncinfo_t* psyncinfo)
//{
//	return 0;
//}
//
//int	ac3_audio_close(void)
//{
//	return 0;
//}

//int	ac3_audio_resync(FIFO_t* pbs)
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
//		if (syncword == 0x0B77)
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
//	return 0;
//}

//uint8_t*	ac3_audio_output(int* pnSampleCount)
//{
//	return 0;
//}

