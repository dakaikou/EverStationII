#ifndef __KERNEL_MPEG_AUDIO_DECODER_H__
#define __KERNEL_MPEG_AUDIO_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPA_EXPORT
#       define MPA_SYNTAX_LIB __declspec(dllexport)
#   elif defined(_MPA_IMPORT)
#       define MPA_SYNTAX_LIB __declspec(dllimport)
#   else
#       define MPA_SYNTAX_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPA_SYNTAX_LIB __attribute__((visibility("default")))
#else
#   define MPA_SYNTAX_LIB
#endif

#include <stdint.h>

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
	uint16_t		syncword;					//12
	uint8_t		ID;							//1
	uint8_t		layer;						//2
	uint8_t		protection_bit;				//1

	uint8_t		bitrate_index;				//4
	uint8_t		sampling_frequency;			//2
	uint8_t		padding_bit;				//1
	uint8_t		private_bit;				//1

	uint8_t		mode;						//2
	uint8_t		mode_extension;				//2
	uint8_t		copyright;					//1
	uint8_t		original_or_copy;			//1
	uint8_t		emphasis;					//2

	uint16_t		crc_check;					//16

	/*sematic part*/

	int		layer_index;
	double	bit_rate;
	int		sampling_rate;
	int		nch;
	int		bitspersample;

	int		num_of_slots;
	int		crc_length;
	int		data_length;

} MPA_header_t, *pMPA_header_t;

typedef struct MPEG1_layer1_audio_data_s
{
	uint8_t		 allocation[32];
	uint8_t		 scalefactor[32];
	uint16_t		 sample[12][32];
	double	 fraction[12][32];

} MPEG1_layer1_audio_data_t, *pMEPG1_layer1_audio_data_t;

typedef struct MPEG1_layer2_audio_data_s
{
	int		 sblimit;
	int		 allocation[32];
	int		 scfsi[32];
	int		 scalefactor[3][32];
//	uint32_t		 sample[12][3][32];
	double	 fraction[12][3][32];

} MPEG1_layer2_audio_data_t, *pMEPG1_layer2_audio_data_t;

typedef struct MPEG1_layer3_audio_data_s
{
	uint16_t		main_data_begin;
	uint8_t		private_bits;
	uint8_t		scfsi[2][4];
	uint16_t		part2_3_length[2][2];
	uint16_t		big_values[2][2];
	uint8_t		global_gain[2][2];
	uint8_t		scalefac_compress[2][2];
	uint8_t		window_switching_flag[2][2];
	uint8_t		block_type[2][2];
	uint8_t		mixed_block_flag[2][2];
	uint8_t		table_select[2][2][3];
	uint8_t		subblock_gain[2][2][3];
	uint8_t		region0_count[2][2];
	uint8_t		region1_count[2][2];

	uint8_t		preflag[2][2];
	uint8_t		scalefac_scale[2][2];
	uint8_t		count1table_select[2][2];

	uint8_t		scalefac_l[2][2][21];
	uint8_t		scalefac_s[2][2][13][4];

} MPEG1_layer3_audio_data_t, *pMEPG1_layer3_audio_data_t;

//define this struct is used to save the original nal segment buffer information
typedef struct
{
	uint8_t* buf;					//volatilable
	int		 length;
} MPA_frame_snapshot_t;

typedef struct MPA_frame_s
{
	MPA_frame_snapshot_t snapshot;

	MPA_header_t header;
	uint8_t*	audio_data_buf;
	int			audio_data_length;

} MPA_frame_t;

typedef struct MpegAudioSynData_s
{
    int		 offset;
    double	 syn[1024];

} MpegAudioSynData_t, *pMpegAudioSynData_t;

/*
 * 8-bit and 16-bit audio buffer
 * to use as 16-bit audio buffer, just type-cast the 
 * (unsigned char *) to (short *)
 */
//typedef struct AudioStream_s
//{
//    int	 start;              /* number of samples skipped from the physical buffer */
//    int	 length;             /* number of samples in the current buffer */
//    uint8_t	 *firstSample;     /* ptr to the first audio data sample */
//    uint8_t	 *buffer;			  /* ptr to the beginning of the physical buffer */
//    S8	 isVirtual;         /* 1 iff it's a virtual buffer */
//} AudioStream_t, *pAudioStream_t;

//int		mpeg_audio_init(void);
//int		mpeg_audio_term(void);
//
//int		mpeg_audio_open(Audio_decode_info_t* pdecode_info);
//int		mpeg_audio_close(void);
//
////int		mpeg_audio_resync(FIFO_t* pbs);
//
MPA_SYNTAX_LIB int	mpeg_audio_decode_frame_header(uint8_t* es_header_buf, int es_header_length, MPA_header_t* pmpa_header);
MPA_SYNTAX_LIB int	mpga_decode_frame(uint8_t* frame_buf, int frame_size, MPA_frame_t* pmpa_frame);
//
//int		mpeg_audio_decode_layer1_audio_data(FIFO_t* pbs);
//int		mpeg_audio_decode_layer2_audio_data(FIFO_t* pbs);
//int		mpeg_audio_decode_layer3_audio_data(FIFO_t* pbs);
//
//int		mpeg_audio_ancillary_data(FIFO_t* pbs);

//uint8_t*		mpeg_audio_output(int* pnSampleCount);
//
//void	Synth_Mpeg1_layer1_audio_data(M2A_header_t* hdr, MPEG1_layer1_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm);
//void	Synth_Mpeg1_layer2_audio_data(M2A_header_t* hdr, MPEG1_layer2_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm);
//void	Synth_Mpeg1_layer3_audio_data(M2A_header_t* hdr, MPEG1_layer3_audio_data_t* data, MpegAudioSynData_t* v, AudioStream_t* pcm);
//
//void	SubBandSynthesis(MpegAudioSynData_t* syndata, double* fraction, short** pcm);
//
//void	AudioNew(AudioStream_t* paudio, int length);
//void	AudioFree(AudioStream_t* paudio);
//void	AudioMerge(AudioStream_t* pleft, AudioStream_t* pright, AudioStream_t* pdest);	

#endif

