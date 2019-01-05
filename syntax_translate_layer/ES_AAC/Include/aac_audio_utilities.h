#ifndef __API_AAC_AUDIO_TOOLS_H__
#define __API_AAC_AUDIO_TOOLS_H__

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

typedef enum
{
	AAC_FORMAT_RAW				= 0,
	AAC_FORMAT_ADIF,
	AAC_FORMAT_ADTS,
	AAC_FORMAT_MP4,
	AAC_FORMAT_UNKNOWN,
} AAC_FORMAT_e;

AAC_LIB int AAC_DecodeID(uint8_t ID, char* pszText, int strSize);
AAC_LIB int AAC_DecodeProfile(uint8_t ID, uint8_t profile, char* pszText, int strSize);
AAC_LIB int AAC_DecodeSamplingFrequency(uint8_t sampling_frequency_index, char* pszText, int strSize);
AAC_LIB int AAC_DecodeChannelConfig(uint8_t channel_configuration, char* pszText, int strSize);

#endif

