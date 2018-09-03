#ifndef __KERNEL_AC3_AUDIO_SEQUENCE_H__
#define __KERNEL_AC3_AUDIO_SEQUENCE_H__

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../compile.h"

/*
#define LEVEL_PLUS6DB 2.0
#define LEVEL_PLUS3DB 1.4142135623730951
#define LEVEL_3DB	  0.7071067811865476
#define LEVEL_45DB	  0.5946035575013605
#define LEVEL_6DB	  0.5
*/
typedef struct AC3_audio_syncinfo_s
{
	/*syntax part*/
	U16		syncword;							//16
	U16		crc1;								//16
	U8		fscod;								//2
	U8		frmsizecod;							//6

	//semantic part
	S32		bitrate;
	S32		sampling_rate;
	S32		words_per_frame;

} AC3_syncinfo_t, *pAC3_syncinfo_t;

typedef struct AC3_audio_bsi_s
{
	U8		bsid;								//5
	U8		bsmod;								//3

	U8		acmod;								//3
	U8		cmixlev;							//2
	U8		surmixlev;							//2
	U8		dsurmod;							//2
	U8		lfeon;								//1
	U8		dialnorm;							//5
	U8		compre;								//1
	U8		compr;								//8
	U8		langcode;							//1
	U8		langcod;							//8
	U8		audprodie;							//1
	U8		mixlevel;							//5
	U8		roomtyp;							//2

	U8		dialnorm2;							//5
	U8		compr2e;							//1
	U8		compr2;								//8

	U8		langcod2e;							//1
	U8		langcod2;							//8

	U8		audprodi2e;							//1
	U8		mixlevel2;							//5
	U8		roomtyp2;							//2

	U8		copyrightb;							//1
	U8		origbs;								//1

	U8		timecod1e;							//1
	U16		timecod1;							//14

	U8		timecod2e;							//1
	U16		timecod2;							//14

	U8		addbsie;							//1
	U8		addbsil;							//6

	U8		addbsi[64];							//(addbsil + 1) * 8

	S32		nfchans;

} AC3_bsi_t, *pAC3_bsi_t;

typedef struct AC3_audio_audblk_s
{
	/* These fields for block switch and dither flags */
	U8		blksw[5];							//1 * 5
	U8		dithflag[5];						//1 * 5
	/* These fields for dynamic range control */
	U8		dynrnge;							//1
	U8		dynrng;								//8
	U8		dynrng2e;							//1
	U8		dynrng2;							//8
	/* These fields for coupling strategy information */
	U8		cplstre;							//1
	U8		cplinu;								//1
	U8		chincpl[5];							//1 * 5
	U8		phsflginu;							//1
	U8		cplbegf;							//4
	U8		cplendf;							//4
	U8		cplbndstrc[18];						//1 * 18
	U8		cplcoe[5];							//1 * 5
	U8		mstrcplco[5];						//2 * 5
	U8		cplcoexp[5][18];					//4 * 5 * 18
	U8		cplcomant[5][18];					//4 * 5 * 18
	U8		phsflg[18];							//1 * 18
	U8		rematstr;							//1
	U8		rematflg[4];						//1 * 4
	U8		cplexpstr;							//2
	U8		chexpstr[5];						//2 * 5
	U8		lfeexpstr;							//1
	U8		chbwcod[5];							//5 * 6
	U8		cplabsexp;							//4
	S8		cplexps[18 * 12 / 3];				//7 *
	S8		exps[5][252 / 3];					//7
	
	U8		gainrng[5];							//2
	S8		lfeexps[3];							//4
	
	U8		baie;								//1
	U8		sdcycod;							//2
	U8		fdcycod;							//2
	U8		sgaincod;							//2
	U8		dbpbcod;							//2
	U8		floorcod;							//3
	
	U8		snroffste;							//1
	U8		csnroffst;							//6
	U8		cplfsnroffst;						//4
	U8		cplfgaincod;						//3
	U8		fsnroffst[5];						//4
	U8		fgaincod[5];						//3
	U8		lfefsnroffst;						//4
	U8		lfefgaincod;						//3
	
	U8		cplleake;							//1
	U16		cplfleak;							//3
	U16		cplsleak;							//3

	U8		deltbaie;							//1
	U8		cpldeltbae;							//2
	U8		deltbae[5];							//2 * 5
	U8		cpldeltnseg;						//3
	U8		cpldeltoffst[8];					//5
	U8		cpldeltlen[8];						//4
	U8		cpldeltba[8];						//3
	U8		deltnseg[5];						//3
	U8		deltoffst[5][8];					//5
	U8		deltlen[5][8];						//4
	U8		deltba[5][8];						//3
	
	U8		skiple;								//1
	U16		skipl;								//9
	
	U16		chmant[5][256];
	U16		cplmant[256];	
	U16		lfemant[7];

	//sematic part	
	U8		ncplsubnd;
	U8		ncplbnd;
	U8		nchgrps[5];
	U8		ncplgrps;
	U8		nlfegrps;

	S16		strtmant[5];
	S16		endmant[5];
	S16		cplstrtmant;
	S16		cplendmant;
	S16		lfestartmant;
	S16		lfeendmant;
	
	U16		cplco[5][256];
	U16		fbw_exp[5][256];
	U16		fbw_bap[5][256];
	U16		cpl_exp[256];			//中置声道指数
	U16		cpl_bap[256];
	U16		lfe_exp[7];				//超重低音声道指数
	U16		lfe_bap[7];

} AC3_audblk_t, *pAC3_audblk_t;

typedef struct AC3_audio_sync_frame_s
{
	AC3_syncinfo_t syncinfo;
	AC3_bsi_t      bsi;

} AC3_syncframe_t;

typedef double stream_samples_t[6][256];

/*-------------------------------------------------------
	CAC3_Audio Class definition
	Author:			chendelin
	Date:			20050312
	Historty:		
-------------------------------------------------------*/
#define FILE_NAME_LENGTH			256

#define AC3CHUNK_MAX_SIZE			65536					//38352 + 408
#define AC3CHUNK_RESERVED_SIZE		 1024					//3 * 204

#define PACKET_BUFFER_SIZE			38352					//188 * 204

#define AC3_AUDIO_BITS_PER_SAMPLE	 16

#define UNPACK_FBW					 1
#define UNPACK_CPL					 2
#define UNPACK_LFE					 4

/* Exponent strategy constants */
#define EXP_REUSE					 0
#define EXP_D15						 1
#define EXP_D25						 2
#define EXP_D45						 3

#define DRC_NONE					 0
#define DRC_LIGHT					 1
#define DRC_NORMAL					 2
#define DRC_HEAVY					 3

/* Delta bit allocation constants */
#define DELTA_BIT_REUSE				 0
#define DELTA_BIT_NEW				 1
#define DELTA_BIT_NONE				 2
#define DELTA_BIT_RESERVED			 3

//S32		ac3_audio_init(void);
//S32		ac3_audio_term(void);

//S32		ac3_audio_open(AC3_syncinfo_t* psyncinfo);
//S32		ac3_audio_close(void);

//S32		ac3_audio_resync(FIFO_t* pbs);

//AC3_syncinfo_t* ac3_audio_get_syncinfo(void);
_CDL_EXPORT int	ac3_audio_decode_syncinfo(unsigned char* syncinfo_buf, int syncinfo_length, AC3_syncinfo_t* pac3_syncinfo);

//AC3_bsi_t* ac3_audio_get_bsi(void);
//S32	ac3_audio_decode_bsi(FIFO_t* pbs);

//S32	ac3_audio_decode_audblk(FIFO_t* pbs);

//U8*		ac3_audio_output(S32* pnSampleCount);

#endif

