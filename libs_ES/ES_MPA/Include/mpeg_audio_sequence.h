#ifndef __KERNEL_MPEG_AUDIO_DECODER_H__
#define __KERNEL_MPEG_AUDIO_DECODER_H__

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../compile.h"

/*------------------------------------------------------------------------
 *
 * Copyright (c) 1997-1998 by Cornell University.
 * 
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * mp123dec.c
 *
 * Ooi Wei Tsang  weitsang@cs.cornell.edu
 *
 * Usage : mp123dec inputMPG outputPCM
 *
 * Decodes an input MPEG-1 Audio (layer 1, 2 or 3) into a PCM file.
 * It assumes that all frames in the audio are encoded using the same
 * layer encoding method.  
 *
 * NOTE : Currently we output each frame to file after decoding, this 
 * is not the most efficient way of doing things.
 *
 *------------------------------------------------------------------------
 */
 
//mpeg audio constants
#define MPEG_AUDIO_LAYER_1			 3
#define MPEG_AUDIO_LAYER_2			 2
#define MPEG_AUDIO_LAYER_3			 1

#define MPEG_AUDIO_STEREO			 0
#define MPEG_AUDIO_JOINT_STEREO		 1
#define MPEG_AUDIO_DUAL_CHANNEL		 2
#define MPEG_AUDIO_SINGLE_CHANNEL	 3

#define MPEG_AUDIO_BITS_PER_SAMPLE	 16

typedef struct MPA_header_s
{
	/*syntax part*/
	U16		syncword;					//12
	U8		ID;							//1
	U8		layer;						//2
	U8		protection_bit;				//1

	U8		bitrate_index;				//4
	U8		sampling_frequency;			//2
	U8		padding_bit;				//1
	U8		private_bit;				//1

	U8		mode;						//2
	U8		mode_extension;				//2
	U8		copyright;					//1
	U8		original_or_copy;			//1
	U8		emphasis;					//2

	U16		crc_check;					//16

	/*sematic part*/

	S8		layer_index;
	double	bit_rate;
	S32		sampling_rate;
	S32		nch;
	S32		bitspersample;

	S32		num_of_slots;
	S32		crc_length;
	S32		data_length;

} MPA_header_t, *pMPA_header_t;

typedef struct MPEG1_layer1_audio_data_s
{
	U8		 allocation[32];
	U8		 scalefactor[32];
	U16		 sample[12][32];
	double	 fraction[12][32];

} MPEG1_layer1_audio_data_t, *pMEPG1_layer1_audio_data_t;

typedef struct MPEG1_layer2_audio_data_s
{
	S32		 sblimit;
	S8		 allocation[32];
	S8		 scfsi[32];
	S8		 scalefactor[3][32];
//	U32		 sample[12][3][32];
	double	 fraction[12][3][32];

} MPEG1_layer2_audio_data_t, *pMEPG1_layer2_audio_data_t;

typedef struct MPEG1_layer3_audio_data_s
{
	U16		main_data_begin;
	U8		private_bits;
	U8		scfsi[2][4];
	U16		part2_3_length[2][2];
	U16		big_values[2][2];
	U8		global_gain[2][2];
	U8		scalefac_compress[2][2];
	U8		window_switching_flag[2][2];
	U8		block_type[2][2];
	U8		mixed_block_flag[2][2];
	U8		table_select[2][2][3];
	U8		subblock_gain[2][2][3];
	U8		region0_count[2][2];
	U8		region1_count[2][2];

	U8		preflag[2][2];
	U8		scalefac_scale[2][2];
	U8		count1table_select[2][2];

	U8		scalefac_l[2][2][21];
	U8		scalefac_s[2][2][13][4];

} MPEG1_layer3_audio_data_t, *pMEPG1_layer3_audio_data_t;

typedef struct MPA_frame_s
{
	MPA_header_t header;
} MPA_frame_t;

typedef struct MpegAudioSynData_s
{
    S32		 offset;
    double	 syn[1024];

} MpegAudioSynData_t, *pMpegAudioSynData_t;

/*
 * 8-bit and 16-bit audio buffer
 * to use as 16-bit audio buffer, just type-cast the 
 * (unsigned char *) to (short *)
 */
//typedef struct AudioStream_s
//{
//    S32	 start;              /* number of samples skipped from the physical buffer */
//    S32	 length;             /* number of samples in the current buffer */
//    U8	 *firstSample;     /* ptr to the first audio data sample */
//    U8	 *buffer;			  /* ptr to the beginning of the physical buffer */
//    S8	 isVirtual;         /* 1 iff it's a virtual buffer */
//} AudioStream_t, *pAudioStream_t;

//S32		mpeg_audio_init(void);
//S32		mpeg_audio_term(void);
//
//S32		mpeg_audio_open(Audio_decode_info_t* pdecode_info);
//S32		mpeg_audio_close(void);
//
////S32		mpeg_audio_resync(FIFO_t* pbs);
//
_CDL_EXPORT int	mpeg_audio_decode_frame_header(unsigned char* es_header_buf, int es_header_length, MPA_header_t* pmpa_header);
//
//S32		mpeg_audio_decode_layer1_audio_data(FIFO_t* pbs);
//S32		mpeg_audio_decode_layer2_audio_data(FIFO_t* pbs);
//S32		mpeg_audio_decode_layer3_audio_data(FIFO_t* pbs);
//
//S32		mpeg_audio_ancillary_data(FIFO_t* pbs);

//U8*		mpeg_audio_output(S32* pnSampleCount);
//
//void	Synth_Mpeg1_layer1_audio_data(M2A_header_t* hdr, MPEG1_layer1_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm);
//void	Synth_Mpeg1_layer2_audio_data(M2A_header_t* hdr, MPEG1_layer2_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm);
//void	Synth_Mpeg1_layer3_audio_data(M2A_header_t* hdr, MPEG1_layer3_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm);
//
//void	SubBandSynthesis(MpegAudioSynData_t* syndata, double* fraction, short** pcm);
//
//void	AudioNew(AudioStream_t* paudio, S32 length);
//void	AudioFree(AudioStream_t* paudio);
//void	AudioMerge(AudioStream_t* pleft, AudioStream_t* pright, AudioStream_t* pdest);	

#endif

