#ifndef __MPEG2_TS_STREAM_ID_H__
#define __MPEG2_TS_STREAM_ID_H__

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

#endif
