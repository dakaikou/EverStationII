#ifndef __API_AAC_AUDIO_TOOLS_H__
#define __API_AAC_AUDIO_TOOLS_H__

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../Compile.h"

typedef enum
{
	AAC_FORMAT_RAW				= 0,
	AAC_FORMAT_ADIF,
	AAC_FORMAT_ADTS,
	AAC_FORMAT_MP4,
	AAC_FORMAT_UNKNOWN,
} AAC_FORMAT_e;

_CDL_EXPORT S32 AAC_DecodeID(U8 ID, S8* pszText, S32 strSize);
_CDL_EXPORT S32 AAC_DecodeProfile(U8 ID, U8 profile, S8* pszText, S32 strSize);
_CDL_EXPORT S32 AAC_DecodeSamplingFrequency(U8 sampling_frequency_index, S8* pszText, S32 strSize);
_CDL_EXPORT S32 AAC_DecodeChannelConfig(U8 channel_configuration, S8* pszText, S32 strSize);

#endif

