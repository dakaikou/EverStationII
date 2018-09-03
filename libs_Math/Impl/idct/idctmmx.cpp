//////////////////////////////////////////////////////////////////////
//  not functional
//////////////////////////////////////////////////////////////////////
// MPEG2AVI
// -------- 
//  v0.16B34
//    performance optimization, idct_mmx32_rows() and idct_mmx32_cols() now
//    transpose their respective outputs "in-place" (saves a bit of time)
//
//  v0.16B33 initial release
//
// MMX32 iDCT algorithm  (IEEE-1180 compliant) :: idct_mmx32()
//
//  This IDCT implementation is based on Intel Application Note AP-922.
//
//  This file implements the idct algorithm with no transpose.
//  The other file (idctmm32_transpose.c) is faster, but transposes the 
//  output-matrix.  (Intel's code-listing produces a transposed output.)
//
//  ALGORITHM OVERVIEW
//  ------------------
// This was one of the harder pieces of work to code.
// Intel's app-note focuses on the numerical theory/issues of the IDCT 
// implementation, but assumes the programmer is familiar with the
// requisite mathematics, leaving the exact form of the complete IDCT 
// code-listing up to the programmer's imagination.
//
// I played around with Intel's code-fragments for quite a few hours.  
// This file is *A* working IDCT implementation, but it may not be
// the implementation Intel originally intended.  Rest assured, I've
// done everything in my power to guarantee its correctness. 
// This implementation passes all six IEEE accuracy tests by a fair margin.
//
//   My IDCT algorithm consists of 4 steps:
//
//   1) IDCT-row transformation (using the IDCT-row function) on all 8 rows
//      This yields an intermediate 8x8 matrix.
//
//   2) transpose of intermediate matrix (mandatory)
//
//   3) IDCT-row transformation (2nd time) on all 8 rows of the 
//      intermediate matrix.
//      At this point, we have the final-result, in transposed form.
//
//   4) post-transformation matrix transpose 
//      (not necessary if the input-data is already transposed, this could
//       be done during the MPEG "zig-zag" scan, but since my algorithm
//       requires at least one transpose operation, why not re-use the
//       transpose-code.)
//
//   Although the (1st) and (3rd) steps use the same basic row-transform 
//   operation, the (3rd) step uses different shift&round constants 
//   (explained later.)
//
//   Also note that the intermediate transpose (2) would not be neccessary,
//   if the subsequent operation were a iDCT-column transformation.  Since
//   we only have the iDCT-row transform, we transpose the intermediate
//   matrix and use the iDCT-row transform a 2nd time.  I suppose one
//   a faster (but more complicated) code-implementation is possible, 
//   if these steps were merged.
//
//   I had to change some constants/variables for my method to work :
//
//      As given by Intel, #SHIFT_INV_COL and #RND_INV_COL are wrong.  
//      Not surprising since I'm probably implementing the IDCT in
//      perverse fashion.  
//      round_inv_col[], which is given as "4 short" values, should have the
//      same dimensions as round_inv_row[].  The corrected variables are 
//      shown.
//
//      Intel's code defines 4 tables of constants.  My code only uses only
//      one of these tables, row#0.  
//
//   IMPLEMENTATION DETAILs
//   ----------------------
// 
//   I divided the 4-steps of my algorithm into two subroutines,
//    1) idct_mmx32_rows() - transforms 8 rows, then transpose
//    2) idct_mmx32_cols() - transforms 8 rows, then transpose
//       yields final result ("drop-in" direct replacement for INT32 IDCT)
//
//   idct_mmx32_cols() is a carbon-copy of idct_mmx32_rows(), i.e. both
//   execute a row-by-row transformations.  Only the shift&rounding 
//   coefficients differ.
//
//      In the 1st function (rows), the shift & round instructions use 
//       SHIFT_INV_ROW & round_inv_row[] (renamed to r_inv_row[])
//
//      In the 2nd function (cols)-> r_inv_col[], and
//       SHIFT_INV_COL & round_inv_col[] (renamed to r_inv_col[])
//
//   Each function contains an integrated transpose-operator, which comes
//   AFTER the primary transformation operation.  In the future, I'll optimize
//   the code to do more of the transpose-work "in-place".  Right now, I've
//   left the code as two subroutines and a main calling function, so other
//   people can read the code more easily.


//;=============================================================================
//;
//;  AP-922   http://developer.intel.com/vtune/cbts/strmsimd
//; These examples contain code fragments for first stage iDCT 8x8
//; (for rows) and first stage DCT 8x8 (for columns)
//;
//;=============================================================================

#define BITS_INV_ACC	4	// 4 or 5 for IEEE
// 5 yields higher accuracy, but lessens dynamic range on the input matrix
#define SHIFT_INV_ROW	(16 - BITS_INV_ACC)
#define SHIFT_INV_COL	(1 + BITS_INV_ACC + 14)
#define RND_INV_ROW		(1 << (SHIFT_INV_ROW-1))
#define RND_INV_COL		(1 << (SHIFT_INV_COL-1)) 
#define RND_INV_CORR	(RND_INV_COL - 1)

const static int r_inv_row[2] = { RND_INV_ROW, RND_INV_ROW};
const static int r_inv_col[2] = {RND_INV_COL, RND_INV_COL};
const static int r_inv_corr[2] = {RND_INV_CORR, RND_INV_CORR};

static const short tab_i_01234567[] = {
	16384, 16384, 16384, -16384, 21407, 8867, 8867, -21407,
	16384, -16384, 16384, 16384, -8867, 21407, -21407, -8867,
	22725, 12873, 19266, -22725, 19266, 4520, -4520, -12873,
	12873, 4520, 4520, 19266, -22725, 19266, -12873, -22725,
};

#define INP				eax
#ifdef OUT
#undef OUT
#endif
#define OUT				ecx
#define TABLE			ebx
#define round_inv_row	edx
#define round_inv_col	edx
#define ROW_STRIDE		16

static __int64 qwTemp[32];

__forceinline static void idct_mmx32_rows(short *block);
__forceinline static void idct_mmx32_cols(short *block);

// public interface to MMX32 IDCT 8x8 operation
void MMX_IDCT(short *block)
{
	idct_mmx32_rows(block);
	idct_mmx32_cols(block);
}

static void idct_mmx32_rows(short *block)
{
#ifdef _WIN64
#else
	__asm
	{
		mov INP, [block]
		mov edi, 0x00
		lea TABLE, [tab_i_01234567]
		lea OUT, [qwTemp]
		lea round_inv_row, [r_inv_row]
lpa:
		movq		mm0, [INP]
		movq		mm1, [INP+8]
		movq		mm2, mm0
		movq		mm3, [TABLE]
		punpcklwd	mm0, mm1
		movq		mm5, mm0
		punpckldq	mm0, mm0
		movq		mm4, [TABLE+8]
		punpckhwd	mm2, mm1
		pmaddwd		mm3, mm0
		movq		mm6, mm2
		movq		mm1, [TABLE+32]
		punpckldq	mm2, mm2
		pmaddwd		mm4, mm2
		punpckhdq	mm5, mm5
		pmaddwd		mm0, [TABLE+16]
		punpckhdq	mm6, mm6
		movq		mm7, [TABLE+40]
		pmaddwd		mm1, mm5
		paddd		mm3, [round_inv_row]
		pmaddwd		mm7, mm6
		pmaddwd		mm2, [TABLE+24]
		paddd		mm3, mm4
		pmaddwd		mm5, [TABLE+48]
		movq		mm4, mm3
		pmaddwd		mm6, [TABLE+56]
		paddd		mm1, mm7
		paddd		mm0, [round_inv_row]
		psubd		mm3, mm1
		psrad		mm3, SHIFT_INV_ROW
		paddd		mm1, mm4
		paddd		mm0, mm2
		psrad		mm1, SHIFT_INV_ROW
		paddd		mm5, mm6
		movq		mm4, mm0
		paddd		mm0, mm5
		psubd		mm4, mm5
		add			INP, 16
		psrad		mm4, SHIFT_INV_ROW
		psrad		mm0, SHIFT_INV_ROW
		packssdw	mm4, mm3
		packssdw	mm1, mm0
		movq		mm7, mm4
		psrld		mm4, 16
		movq		[OUT], mm1
		pslld		mm7, 16
		por			mm7, mm4
		movq		mm3, [TABLE]
		movq		[OUT+8], mm7
		add			edi, 0x01
		add			OUT, 16
		cmp			edi, 0x08
		jl			lpa
		lea			OUT, [qwTemp]
		movq		mm0, [OUT+ROW_STRIDE*0]
		movq		mm1, [OUT+ROW_STRIDE*1]
		movq		mm4, mm0
		movq		mm2, [OUT+ROW_STRIDE*2]
		punpcklwd	mm0, mm1
		movq		mm3, [OUT+ROW_STRIDE*3]
		punpckhwd	mm4, mm1
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		punpckhwd	mm6, mm3
		movq		mm1, mm0
		mov			INP, [block]
		punpckldq	mm0, mm2
		movq		mm3, mm4
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*0], mm0
		punpckldq	mm4, mm6
		movq		mm0, [OUT+ROW_STRIDE*4+8]
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*2], mm4
		movq		mm4, mm0
		movq		[INP+ROW_STRIDE*1], mm1
		movq		mm1, [OUT+ROW_STRIDE*5+8]
		movq		[INP+ROW_STRIDE*3], mm3
		punpcklwd	mm0, mm1
		movq		mm2, [OUT+ROW_STRIDE*6+8]
		punpckhwd	mm4, mm1
		movq		mm3, [OUT+ROW_STRIDE*7+8]
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		movq		mm1, mm0
		punpckhwd	mm6, mm3
		movq		mm3, mm4
		punpckldq	mm0, mm2
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*4+8], mm0
		punpckldq	mm4, mm6
		movq		mm0, [OUT+ROW_STRIDE*4]
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*6+8], mm4
		movq		mm4, mm0
		movq		[INP+ROW_STRIDE*5+8], mm1
		movq		mm1, [OUT+ROW_STRIDE*5]
		movq		[INP+ROW_STRIDE*7+8], mm3
		punpcklwd	mm0, mm1
		movq		mm2, [OUT+ROW_STRIDE*6]
		punpckhwd	mm4, mm1
		movq		mm3, [OUT+ROW_STRIDE*7]
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		movq		mm1, mm0
		punpckhwd	mm6, mm3
		movq		mm3, mm4
		punpckldq	mm0, mm2
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*0+8], mm0
		punpckldq	mm4, mm6
		movq		mm0, [OUT+ROW_STRIDE*0+8]
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*2+8], mm4
		movq		mm4, mm0
		movq		[INP+ROW_STRIDE*1+8], mm1
		movq		mm1, [OUT+ROW_STRIDE*1+8]
		movq		[INP+ROW_STRIDE*3+8], mm3
		punpcklwd	mm0, mm1
		movq		mm2, [OUT+ROW_STRIDE*2+8]
		punpckhwd	mm4, mm1
		movq		mm3, [OUT+ROW_STRIDE*3+8]
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		movq		mm1, mm0
		punpckhwd	mm6, mm3
		movq		mm3, mm4
		punpckldq	mm0, mm2
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*4], mm0
		punpckldq	mm4, mm6
		movq		[INP+ROW_STRIDE*5], mm1
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*6], mm4
		movq		[INP+ROW_STRIDE*7], mm3
	}
#endif
}

static void idct_mmx32_cols(short *block)
{
#ifdef _WIN64
#else
	__asm
	{
		mov			INP, [block]
		mov			edi, 0x00
		lea			TABLE, [tab_i_01234567]
		lea			OUT, [qwTemp]
		lea			round_inv_col, [r_inv_col]
acc_idct_colloop1:
		movq		mm0, [INP]
		movq		mm1, [INP+8]
		movq		mm2, mm0
		movq		mm3, [TABLE]
		punpcklwd	mm0, mm1
		movq		mm5, mm0
		punpckldq	mm0, mm0
		movq		mm4, [TABLE+8]
		punpckhwd	mm2, mm1
		pmaddwd		mm3, mm0
		movq		mm6, mm2
		movq		mm1, [TABLE+32]
		punpckldq	mm2, mm2
		pmaddwd		mm4, mm2
		punpckhdq	mm5, mm5
		pmaddwd		mm0, [TABLE+16]
		punpckhdq	mm6, mm6
		movq		mm7, [TABLE+40]
		pmaddwd		mm1, mm5
		paddd		mm3, [round_inv_col]
		pmaddwd		mm7, mm6
		pmaddwd		mm2, [TABLE+24]
		paddd		mm3, mm4
		pmaddwd		mm5, [TABLE+48]
		movq		mm4, mm3
		pmaddwd		mm6, [TABLE+56]
		paddd		mm1, mm7
		paddd		mm0, [round_inv_col]
		psubd		mm3, mm1
		psrad		mm3, SHIFT_INV_COL
		paddd		mm1, mm4
		paddd		mm0, mm2
		psrad		mm1, SHIFT_INV_COL
		paddd		mm5, mm6
		movq		mm4, mm0
		paddd		mm0, mm5
		psubd		mm4, mm5
		add			INP, 16
		psrad		mm4, SHIFT_INV_COL
		add			TABLE, 0
		psrad		mm0, SHIFT_INV_COL
		packssdw	mm4, mm3
		packssdw	mm1, mm0
		movq		mm7, mm4
		psrld		mm4, 16
		movq		[OUT], mm1
		pslld		mm7, 16
		por			mm7, mm4
		movq		[OUT+8], mm7
		add			edi, 0x01
		add			OUT, 16
		cmp			edi, 0x08
		jl			acc_idct_colloop1
		lea			OUT, [qwTemp]
		movq		mm0, [OUT+ROW_STRIDE*0]
		movq		mm1, [OUT+ROW_STRIDE*1]
		movq		mm4, mm0
		movq		mm2, [OUT+ROW_STRIDE*2]
		punpcklwd	mm0, mm1
		movq		mm3, [OUT+ROW_STRIDE*3]
		punpckhwd	mm4, mm1
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		punpckhwd	mm6, mm3
		movq		mm1, mm0
		mov			INP, [block]
		punpckldq	mm0, mm2
		movq		mm3, mm4
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*0], mm0
		punpckldq	mm4, mm6
		movq		mm0, [OUT+ROW_STRIDE*4+8]
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*2], mm4
		movq		mm4, mm0
		movq		[INP+ROW_STRIDE*1], mm1
		movq		mm1, [OUT+ROW_STRIDE*5+8]
		movq		[INP+ROW_STRIDE*3], mm3
		punpcklwd	mm0, mm1
		movq		mm2, [OUT+ROW_STRIDE*6+8]
		punpckhwd	mm4, mm1
		movq		mm3, [OUT+ROW_STRIDE*7+8]
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		movq		mm1, mm0
		punpckhwd	mm6, mm3
		movq		mm3, mm4
		punpckldq	mm0, mm2
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*4+8], mm0
		punpckldq	mm4, mm6
		movq		mm0, [OUT+ROW_STRIDE*4]
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*6+8], mm4
		movq		mm4, mm0
		movq		[INP+ROW_STRIDE*5+8], mm1
		movq		mm1, [OUT+ROW_STRIDE*5]
		movq		[INP+ROW_STRIDE*7+8], mm3
		punpcklwd	mm0, mm1
		movq		mm2, [OUT+ROW_STRIDE*6]
		punpckhwd	mm4, mm1
		movq		mm3, [OUT+ROW_STRIDE*7]
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		movq		mm1, mm0
		punpckhwd	mm6, mm3
		movq		mm3, mm4
		punpckldq	mm0, mm2
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*0+8], mm0
		punpckldq	mm4, mm6
		movq		mm0, [OUT+ROW_STRIDE*0+8]
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*2+8], mm4
		movq		mm4, mm0
		movq		[INP+ROW_STRIDE*1+8], mm1
		movq		mm1, [OUT + ROW_STRIDE*1+8]
		movq		[INP+ROW_STRIDE*3+8], mm3
		punpcklwd	mm0, mm1
		movq		mm2, [OUT+ROW_STRIDE*2+8]
		punpckhwd	mm4, mm1
		movq		mm3, [OUT+ROW_STRIDE*3+8]
		movq		mm6, mm2
		punpcklwd	mm2, mm3
		movq		mm1, mm0
		punpckhwd	mm6, mm3
		movq		mm3, mm4
		punpckldq	mm0, mm2
		punpckhdq	mm1, mm2
		movq		[INP+ROW_STRIDE*4], mm0
		punpckldq	mm4, mm6
		movq		[INP+ROW_STRIDE*5], mm1
		punpckhdq	mm3, mm6
		movq		[INP+ROW_STRIDE*6], mm4
		movq		[INP+ROW_STRIDE*7], mm3
	}
#endif
}
