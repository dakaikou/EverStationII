#ifndef __EVERSTATION_MIDDLEWARE_UTILITIES_MEDIAFILE_H__
#define __EVERSTATION_MIDDLEWARE_UTILITIES_MEDIAFILE_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_UTILITY_EXPORT
#       define MW_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MW_UTILITY_IMPORT)
#       define MW_UTILITY_LIB __declspec(dllimport)
#   else
#       define MW_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MW_UTILITY_LIB
#endif

#include <stdint.h>

typedef enum
{
	STREAM_UNKNOWN			= 0x00000000,
	STREAM_TS				= 0x00000001,
	STREAM_PS				= 0x00000002,
//	STREAM_PES				= 0x00000004,
	STREAM_MPEGAES			= 0x00000008,
	STREAM_MPEGVES			= 0x00000010,
	STREAM_AC3AES			= 0x00000020,
	STREAM_H264VES			= 0x00000040,
	STREAM_AACAES			= 0x00000080,
	STREAM_AVSVES			= 0x00000100,
	STREAM_YUVVES			= 0x00000200,
	STREAM_WAVEAES			= 0x00000400,

} STREAM_TYPE_e;

#define STREAM_FILE			0x80000000

typedef enum
{
	DECIMATE_NONE			=	0,
	
	DECIMATE_TS2TS			=	1,
	DECIMATE_TS2PES			=	2,
	DECIMATE_TS2ES			=	3,
	DECIMATE_TS2SECTION		=	4,


	DECIMATE_PS2MPEGVES		=	13,
	DECIMATE_PS2MPEGAES		=	14,
	DECIMATE_PS2AC3AES		=	15,

	DECIMATE_MPEG_VES2VES	=	20,
	DECIMATE_H264_VES2VES	=	21,
	DECIMATE_AVS_VES2VES	=	22,

} DECIMATE_TYPE_e;

MW_UTILITY_LIB int FILE_GetStreamType(char* pszFileName);												//��ȡ��ý���ļ�������
MW_UTILITY_LIB int FILE_CheckStreamType(int stream_type, char* pszFileName);								//��ȡ��ý���ļ�������

#endif

