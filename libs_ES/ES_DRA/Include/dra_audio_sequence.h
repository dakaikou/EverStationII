#ifndef __KERNEL_DRA_AUDIO_DECODER_H__
#define __KERNEL_DRA_AUDIO_DECODER_H__

#include <stdint.h>
#include "../compile.h"

typedef struct DRA_header_s
{
	/*syntax part*/
	uint16_t    nSyncWord;					//16

	uint8_t		nFrmHeaderType;				//1
	uint16_t    nNumWord;					//10 或者 13 比特

	uint8_t     num_blocks_per_frm_code;	//2
	uint8_t     nNumBlocksPerFrm;		

	uint8_t     nSampleRateIndex;		//4

	uint8_t     num_normal_ch_code;		//3 或者 6 比特
	uint8_t		nNumNormalCh;

	uint8_t		nNumLfeCh;				//1 或者 2 比特

	uint8_t     bAuxData;				//1

	/*sematic part*/

} DRA_FrameHeader_t;

typedef struct DRA_frame_s
{
	DRA_FrameHeader_t FrameHeader;
} DRA_Frame_t;

//_CDL_EXPORT int	dra_audio_decode_frame(unsigned char* adts_fixed_header_buf, int adts_fixed_header_length, DRA_adts_fixed_header_t* padts_fixed_header);
//_CDL_EXPORT int	dra_audio_decode_adts_variable_header(unsigned char* adts_variable_header_buf, int adts_variable_header_length, DRA_adts_variable_header_t* padts_variable_header);
//_CDL_EXPORT int	dra_audio_decode_adts_error_check(unsigned char* adts_error_check_buf, int adts_error_check_length, DRA_adts_error_check_t* padts_error_check);


#endif

