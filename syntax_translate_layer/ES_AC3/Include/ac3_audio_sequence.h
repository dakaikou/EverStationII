#ifndef __KERNEL_AC3_AUDIO_SEQUENCE_H__
#define __KERNEL_AC3_AUDIO_SEQUENCE_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _AC3_EXPORT
#       define AC3_LIB __declspec(dllexport)
#   elif defined(_AC3_IMPORT)
#       define AC3_LIB __declspec(dllimport)
#   else
#       define AC3_LIB
#   endif
#elif __GNUC__ >= 4
#   define AC3_LIB __attribute__((visibility("default")))
#else
#   define AC3_LIB
#endif

#include <stdint.h>

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
	uint16_t		syncword;							//16
	uint16_t		crc1;								//16
	uint8_t		fscod;								//2
	uint8_t		frmsizecod;							//6

	//semantic part
	int		bitrate;
	int		sampling_rate;
	int		words_per_frame;

} AC3_syncinfo_t, *pAC3_syncinfo_t;

typedef struct AC3_audio_bsi_s
{
	uint8_t		bsid;								//5
	uint8_t		bsmod;								//3

	uint8_t		acmod;								//3
	uint8_t		cmixlev;							//2
	uint8_t		surmixlev;							//2
	uint8_t		dsurmod;							//2
	uint8_t		lfeon;								//1
	uint8_t		dialnorm;							//5
	uint8_t		compre;								//1
	uint8_t		compr;								//8
	uint8_t		langcode;							//1
	uint8_t		langcod;							//8
	uint8_t		audprodie;							//1
	uint8_t		mixlevel;							//5
	uint8_t		roomtyp;							//2

	uint8_t		dialnorm2;							//5
	uint8_t		compr2e;							//1
	uint8_t		compr2;								//8

	uint8_t		langcod2e;							//1
	uint8_t		langcod2;							//8

	uint8_t		audprodi2e;							//1
	uint8_t		mixlevel2;							//5
	uint8_t		roomtyp2;							//2

	uint8_t		copyrightb;							//1
	uint8_t		origbs;								//1

	uint8_t		timecod1e;							//1
	uint16_t		timecod1;							//14

	uint8_t		timecod2e;							//1
	uint16_t		timecod2;							//14

	uint8_t		addbsie;							//1
	uint8_t		addbsil;							//6

	uint8_t		addbsi[64];							//(addbsil + 1) * 8

	int		nfchans;

} AC3_bsi_t, *pAC3_bsi_t;

typedef struct AC3_audio_audblk_s
{
	/* These fields for block switch and dither flags */
	uint8_t		blksw[5];							//1 * 5
	uint8_t		dithflag[5];						//1 * 5
	/* These fields for dynamic range control */
	uint8_t		dynrnge;							//1
	uint8_t		dynrng;								//8
	uint8_t		dynrng2e;							//1
	uint8_t		dynrng2;							//8
	/* These fields for coupling strategy information */
	uint8_t		cplstre;							//1
	uint8_t		cplinu;								//1
	uint8_t		chincpl[5];							//1 * 5
	uint8_t		phsflginu;							//1
	uint8_t		cplbegf;							//4
	uint8_t		cplendf;							//4
	uint8_t		cplbndstrc[18];						//1 * 18
	uint8_t		cplcoe[5];							//1 * 5
	uint8_t		mstrcplco[5];						//2 * 5
	uint8_t		cplcoexp[5][18];					//4 * 5 * 18
	uint8_t		cplcomant[5][18];					//4 * 5 * 18
	uint8_t		phsflg[18];							//1 * 18
	uint8_t		rematstr;							//1
	uint8_t		rematflg[4];						//1 * 4
	uint8_t		cplexpstr;							//2
	uint8_t		chexpstr[5];						//2 * 5
	uint8_t		lfeexpstr;							//1
	uint8_t		chbwcod[5];							//5 * 6
	uint8_t		cplabsexp;							//4
	uint8_t		cplexps[18 * 12 / 3];				//7 *
	uint8_t		exps[5][252 / 3];					//7
	
	uint8_t		gainrng[5];							//2
	uint8_t		lfeexps[3];							//4
	
	uint8_t		baie;								//1
	uint8_t		sdcycod;							//2
	uint8_t		fdcycod;							//2
	uint8_t		sgaincod;							//2
	uint8_t		dbpbcod;							//2
	uint8_t		floorcod;							//3
	
	uint8_t		snroffste;							//1
	uint8_t		csnroffst;							//6
	uint8_t		cplfsnroffst;						//4
	uint8_t		cplfgaincod;						//3
	uint8_t		fsnroffst[5];						//4
	uint8_t		fgaincod[5];						//3
	uint8_t		lfefsnroffst;						//4
	uint8_t		lfefgaincod;						//3
	
	uint8_t		cplleake;							//1
	uint16_t		cplfleak;							//3
	uint16_t		cplsleak;							//3

	uint8_t		deltbaie;							//1
	uint8_t		cpldeltbae;							//2
	uint8_t		deltbae[5];							//2 * 5
	uint8_t		cpldeltnseg;						//3
	uint8_t		cpldeltoffst[8];					//5
	uint8_t		cpldeltlen[8];						//4
	uint8_t		cpldeltba[8];						//3
	uint8_t		deltnseg[5];						//3
	uint8_t		deltoffst[5][8];					//5
	uint8_t		deltlen[5][8];						//4
	uint8_t		deltba[5][8];						//3
	
	uint8_t		skiple;								//1
	uint16_t		skipl;								//9
	
	uint16_t		chmant[5][256];
	uint16_t		cplmant[256];	
	uint16_t		lfemant[7];

	//sematic part	
	uint8_t		ncplsubnd;
	uint8_t		ncplbnd;
	uint8_t		nchgrps[5];
	uint8_t		ncplgrps;
	uint8_t		nlfegrps;

	short		strtmant[5];
	short		endmant[5];
	short		cplstrtmant;
	short		cplendmant;
	short		lfestartmant;
	short		lfeendmant;
	
	uint16_t		cplco[5][256];
	uint16_t		fbw_exp[5][256];
	uint16_t		fbw_bap[5][256];
	uint16_t		cpl_exp[256];			//中置声道指数
	uint16_t		cpl_bap[256];
	uint16_t		lfe_exp[7];				//超重低音声道指数
	uint16_t		lfe_bap[7];

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

//int		ac3_audio_init(void);
//int		ac3_audio_term(void);

//int		ac3_audio_open(AC3_syncinfo_t* psyncinfo);
//int		ac3_audio_close(void);

//int		ac3_audio_resync(FIFO_t* pbs);

//AC3_syncinfo_t* ac3_audio_get_syncinfo(void);
AC3_LIB int	ac3_audio_decode_syncinfo(unsigned char* syncinfo_buf, int syncinfo_length, AC3_syncinfo_t* pac3_syncinfo);

//AC3_bsi_t* ac3_audio_get_bsi(void);
//int	ac3_audio_decode_bsi(FIFO_t* pbs);

//int	ac3_audio_decode_audblk(FIFO_t* pbs);

//uint8_t*		ac3_audio_output(int* pnSampleCount);

#endif

