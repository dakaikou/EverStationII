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

#include <string.h>
#include <malloc.h>
#include "../../Include/kernel/kernel_ac3_audio_decoder.h"


/*------------------------------------------------------------------------------------------------
	函数：exp_unpack_ch()
	描述：
	输入：type			-- 声道类型
		  expstr		--
		  ngrps			--
		  initial_exp	-- 初始指数
		  exps			--
	输出：dest			--
	返回：0				-- 成功
		  1				-- 失败
	维护：

			2006.03.20	chendelin		从参考程序移植
------------------------------------------------------------------------------------------------*/
static S32 AC3_exp_unpack_ch(U16 type, U8 expstr, U8 ngrps, S8 initial_exp, S8 exps[], U16 *dest)
{
	U16 i,j;
	S16 exp_acc;
	S16 exp_1,exp_2,exp_3;
	S32 error_flag = 0;

	if (expstr == EXP_REUSE)
	{
		error_flag = 1; 
	}
	else
	{
		/* Handle the initial absolute exponent */
		exp_acc = initial_exp;
		j = 0;

		/* In the case of a fbw channel then the initial absolute values is also an exponent */
		if (type != UNPACK_CPL)
		{
			dest[j++] = exp_acc;
		}

		/* Loop through the groups and fill the dest array appropriately */
		for (i=0; i< ngrps; i++)
		{
			if (exps[i] > 124)
			{
				error_flag = 1;
				return error_flag;
			}

			exp_1 = exps[i] / 25;
			exp_2 = (exps[i] - (exp_1 * 25)) / 5;
			exp_3 = exps[i] - (exp_1 * 25) - (exp_2 * 5) ;

			exp_acc += (exp_1 - 2);

			switch (expstr)
			{
			case EXP_D45:
				dest[j++] = exp_acc;
				dest[j++] = exp_acc;
			case EXP_D25:
				dest[j++] = exp_acc;
			case EXP_D15:
				dest[j++] = exp_acc;
			default:
				break;
			}

			exp_acc += (exp_2 - 2);

			switch (expstr)
			{
			case EXP_D45:
				dest[j++] = exp_acc;
				dest[j++] = exp_acc;
			case EXP_D25:
				dest[j++] = exp_acc;
			case EXP_D15:
				dest[j++] = exp_acc;
			default:
				break;
			}

			exp_acc += (exp_3 - 2);

			switch(expstr)
			{
			case EXP_D45:
				dest[j++] = exp_acc;
				dest[j++] = exp_acc;
			case EXP_D25:
				dest[j++] = exp_acc;
			case EXP_D15:
				dest[j++] = exp_acc;
			default:
				break;
			}
		}
	}

	return error_flag;
}

/*------------------------------------------------------------------------------------------------
	函数：AC3_exp_unpack()
	描述：
	输入：expstr		--
		  ngrps			--
		  initial_exp	-- 初始指数
		  exps			--
	输出：dest			--
	返回：0				-- 成功
		  1				-- 失败
	维护：

			2006.03.20	chendelin		从参考程序移植
------------------------------------------------------------------------------------------------*/
S32 AC3_exp_unpack(U8 expstr, S8 absexp, S8* exps, U8 ngrps, U16 *dest)
{
	U16  i, j;
	S32  grp;
	S32	 grpoffset;
	S32	 grpsize;
	S8   expacc;
	S8*  dexp;
	U16* aexp;
	U16  prevexp;

	dexp = (S8*)malloc((ngrps * 3) * sizeof(S8));
	aexp = (U16*)malloc((ngrps * 3) * sizeof(U16));

	if ((dexp != NULL) && (aexp != NULL))
	{
		/* unpack the mapped values */
		grpoffset = 0;
		for (grp = 0; grp < ngrps; grp++)
		{
			expacc = exps[grp];
			dexp[grpoffset + 0] = expacc / 25;

			expacc -= 25 * dexp[grpoffset + 0];
			dexp[grpoffset + 1] = expacc / 5;

			expacc -= 5 * dexp[grpoffset + 1];
			dexp[grpoffset + 2] = expacc;

			grpoffset += 3;
		}

		/* unbiased mapped values*/
		for (i = 0; i < (ngrps * 3); i ++)
		{
			dexp[i] -= 2;
		}

		/* convert from differentials to absolutes*/
		prevexp = absexp;
		for (i = 0; i < (ngrps * 3); i ++)
		{
			aexp[i] = prevexp + dexp[i];
			prevexp = aexp[i];
		}

		/* expand to full absolute exponent array, using grpsize */
		grpsize = 1 << (expstr - 1);

		dest[0] = absexp;
		for (i = 0; i < (ngrps * 3); i++)
		{
			for (j = 0; j < grpsize; j++)
			{
				dest[(i * grpsize) + j + 1] = aexp[i];
			}
		}
	}

	if (dexp != NULL)
	{
		free(dexp);
	}

	if (aexp != NULL)
	{
		free(aexp);
	}

	return 0;
}

/*------------------------------------------------------------------------------------------------
	函数：AC3_exponent_unpack()

	描述：从压缩码流中恢复采样音频样本的指数

	输入：bsi			-- bit stream information
		  audblk		-- syntax part of audio block
	输出：audblk		-- sematic part of audio block
	返回：0				-- 成功
		  1				-- 失败
	维护：

			2006.03.20	chendelin		从参考程序移植
------------------------------------------------------------------------------------------------*/
S32 AC3_exponent_unpack(AC3_bsi_t *bsi, AC3_audblk_t *audblk)
{
	S32 error_flag = 0;
	S32 ch = 0;

	ch = 0;
	while ((ch < bsi->nfchans) && (error_flag == 0))
	{
		error_flag = AC3_exp_unpack_ch(UNPACK_FBW, audblk->chexpstr[ch], audblk->nchgrps[ch], audblk->exps[ch][0], 
									&audblk->exps[ch][1], audblk->fbw_exp[ch]);

		ch ++;
	}

	//coupling channel
	if ((audblk->cplinu) && (error_flag == 0))
	{
		error_flag = AC3_exp_unpack_ch(UNPACK_CPL, audblk->cplexpstr, audblk->ncplgrps, (audblk->cplabsexp << 1),	
									audblk->cplexps, &audblk->cpl_exp[audblk->cplstrtmant]);
	}

	//lfe channel
	if ((bsi->lfeon) && (error_flag == 0))
	{
		error_flag = AC3_exp_unpack_ch(UNPACK_LFE, audblk->lfeexpstr, 2, audblk->lfeexps[0], 
									&audblk->lfeexps[1], audblk->lfe_exp);
	}

	return error_flag;
}


