#ifndef __KERNEL_AAC_AUDIO_DECODER_H__
#define __KERNEL_AAC_AUDIO_DECODER_H__

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../compile.h"

typedef struct AAC_adts_fixed_header_s
{
	/*syntax part*/
	U16		Syncword;					//12
	U8		ID;							//1
	U8		Layer;						//2
	U8		protection_absent;			//1

	U8		Profile;					//2
	U8		sampling_frequency_index;	//4
	U8		private_bit;				//1
	U8		channel_configuration;		//3
	U8		original_or_copy;			//1
	U8		Home;						//1
	U8		Emphasis;					//2

	/*sematic part*/

} AAC_adts_fixed_header_t, *pAAC_adts_fixed_header_t;

typedef struct AAC_adts_variable_header_s
{
	U8		copyright_identification_bit;		//1
	U8		copyright_identification_start;		//1
	U8		aac_frame_length;					//13
	U8		adts_buffer_fullness;				//11
	U8		no_raw_data_blocks_in_frame;		//2

} AAC_adts_variable_header_t, *pAAC_adts_variable_header_t;

typedef struct AAC_adts_error_check_s
{
	U16		crc_check;							//16

} AAC_adts_error_check_t, *pAAC_adts_error_check_t;

//S32		aac_audio_init(void);
//S32		aac_audio_term(void);
//
////S32		aac_audio_open(AC3_audio_syncinfo_t* psyncinfo);
//S32		aac_audio_close(void);
//
////S32		aac_audio_resync(FIFO_t* pbs);
//
//AAC_adts_fixed_header_t* aac_audio_get_adts_fixed_header(void);
//AAC_adts_variable_header_t* aac_audio_get_adts_variable_header(void);
//AAC_adts_error_check_t* aac_audio_get_adts_error_check(void);

_CDL_EXPORT int	aac_audio_decode_adts_fixed_header(unsigned char* adts_fixed_header_buf, int adts_fixed_header_length, AAC_adts_fixed_header_t* padts_fixed_header);
_CDL_EXPORT int	aac_audio_decode_adts_variable_header(unsigned char* adts_variable_header_buf, int adts_variable_header_length, AAC_adts_variable_header_t* padts_variable_header);
_CDL_EXPORT int	aac_audio_decode_adts_error_check(unsigned char* adts_error_check_buf, int adts_error_check_length, AAC_adts_error_check_t* padts_error_check);

//U8*		aac_audio_output(S32* pnSampleCount);


#endif

