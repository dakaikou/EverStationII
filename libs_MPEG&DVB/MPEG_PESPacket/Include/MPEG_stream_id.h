#ifndef __MPEG2_STREAM_ID_H__
#define __MPEG2_STREAM_ID_H__

//#include "../compile.h"
//#include "Include/INTTYPES.H"

//#define PICTURE_START_CODE			0x00
//#define SLICE_START_CODE_MIN		0x01
//#define SLICE_START_CODE_MAX		0xAF
////reserved							0xB0
////reserved							0xB1
//#define USER_DATA_START_CODE		0xB2
//#define SEQUENCE_HEADER_CODE		0xB3
//#define SEQUENCE_ERROR_CODE			0xB4
//#define EXTENSION_START_CODE		0xB5
////reserved							0xB6
//#define SEQUENCE_END_CODE			0xB7
//#define GROUP_START_CODE			0xB8

#define MPEG_PROGRAM_END_CODE		0xB9
#define PACK_START_CODE				0xBA
#define SYSTEM_HEADER_START_CODE	0xBB
#define PROGRAM_STREAM_MAP			0xBC
#define PRIVATE_STREAM_1			0xBD					//maybe AC-3 stream	
#define PADDING_STREAM				0xBE
#define PRIVATE_STREAM_2			0xBF
#define AUDIO_STREAM_MIN			0xC0
#define AUDIO_STREAM_MAX			0xDF
#define VIDEO_STREAM_MIN			0xE0
#define VIDEO_STREAM_MAX			0xEF
#define ECM_STREAM					0xF0
#define EMM_STREAM					0xF1
#define DSMCC_STREAM				0xF2
#define ISO_13522_STREAM			0xF3
#define TYPE_A_STREAM				0xF4
#define TYPE_B_STREAM				0xF5
#define TYPE_C_STREAM				0xF6
#define TYPE_D_STREAM				0xF7
#define TYPE_E_STREAM				0xF8
#define ANCILLARY_STREAM			0xF9
#define RESERVED_DATA_STREAM_A		0xFA
#define RESERVED_DATA_STREAM_B		0xFB
#define AVC_VIDEO_STREAM			0xFC
#define RESERVED_DATA_STREAM_D		0xFD
#define RESERVED_DATA_STREAM_E		0xFE
#define PROGRAM_STREAM_DIRECTORY	0xFF


#define FAST_FORWARD				0x0
#define SLOW_MOTION					0x1
#define FREEZE_FRAME				0x2
#define FAST_REVERSE				0x3
#define SLOW_REVERSE				0x4

//											//	0x0		reserved	
//#define SEQUENCE_EXTENSION_ID					0x1
//#define SEQUENCE_DISPLAY_EXTENSION_ID			0x2
//#define QUANT_MATRIX_EXTENSION_ID				0x3
//#define COPYRIGHT_EXTENSION_ID					0x4
//#define SEQUENCE_SCALABLE_EXTENSION_ID			0x5
//											//  0x6	reserved
//#define PICTURE_DISPLAY_EXTENSION_ID			0x7
//#define PICTURE_CODING_EXTENSION_ID				0x8
//#define PICTURE_SPATIAL_SCALABLE_EXTENSION_ID	0x9
//#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID	0xA
//											//  0xB ~ 0xF reserved

#define AUDIO_SUBTYPE_SUB						0x20
#define AUDIO_SUBTYPE_AC3						0x80
#define AUDIO_SUBTYPE_DTS						0x88
#define AUDIO_SUBTYPE_PCM						0xA0


#endif

