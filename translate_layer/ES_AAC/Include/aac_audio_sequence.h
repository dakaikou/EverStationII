#ifndef __KERNEL_AAC_AUDIO_DECODER_H__
#define __KERNEL_AAC_AUDIO_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _AAC_EXPORT
#       define AAC_LIB __declspec(dllexport)
#   elif defined(_AAC_IMPORT)
#       define AAC_LIB __declspec(dllimport)
#   else
#       define AAC_LIB
#   endif
#elif __GNUC__ >= 4
#   define AAC_LIB __attribute__((visibility("default")))
#else
#   define AAC_LIB
#endif

#include <stdint.h>

typedef struct AAC_adts_fixed_header_s
{
	/*syntax part*/
	uint16_t	Syncword;					//12
	uint8_t		ID;							//1
	uint8_t		Layer;						//2
	uint8_t		protection_absent;			//1

	uint8_t		Profile;					//2
	uint8_t		sampling_frequency_index;	//4
	uint8_t		private_bit;				//1
	uint8_t		channel_configuration;		//3
	uint8_t		original_or_copy;			//1
	uint8_t		Home;						//1
	uint8_t		Emphasis;					//2

	/*sematic part*/

} AAC_adts_fixed_header_t, *pAAC_adts_fixed_header_t;

typedef struct AAC_adts_variable_header_s
{
	uint8_t		copyright_identification_bit;		//1
	uint8_t		copyright_identification_start;		//1
	uint8_t		aac_frame_length;					//13
	uint8_t		adts_buffer_fullness;				//11
	uint8_t		no_raw_data_blocks_in_frame;		//2

} AAC_adts_variable_header_t, *pAAC_adts_variable_header_t;

typedef struct AAC_adts_error_check_s
{
	uint16_t		crc_check;							//16

} AAC_adts_error_check_t, *pAAC_adts_error_check_t;

//int		aac_audio_init(void);
//int		aac_audio_term(void);
//
////int		aac_audio_open(AC3_audio_syncinfo_t* psyncinfo);
//int		aac_audio_close(void);
//
////int		aac_audio_resync(FIFO_t* pbs);
//
//AAC_adts_fixed_header_t* aac_audio_get_adts_fixed_header(void);
//AAC_adts_variable_header_t* aac_audio_get_adts_variable_header(void);
//AAC_adts_error_check_t* aac_audio_get_adts_error_check(void);

AAC_LIB int	aac_audio_decode_adts_fixed_header(unsigned char* adts_fixed_header_buf, int adts_fixed_header_length, AAC_adts_fixed_header_t* padts_fixed_header);
AAC_LIB int	aac_audio_decode_adts_variable_header(unsigned char* adts_variable_header_buf, int adts_variable_header_length, AAC_adts_variable_header_t* padts_variable_header);
AAC_LIB int	aac_audio_decode_adts_error_check(unsigned char* adts_error_check_buf, int adts_error_check_length, AAC_adts_error_check_t* padts_error_check);

//uint8_t*		aac_audio_output(int* pnSampleCount);


#endif

