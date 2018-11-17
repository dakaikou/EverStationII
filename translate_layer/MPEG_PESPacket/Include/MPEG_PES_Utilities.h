#ifndef __MPEG_PES_UTILITIES_ID_H__
#define __MPEG_PES_UTILITIES_ID_H__

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG_PESPACKET_EXPORT
#       define MPEG2_PES_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPEG_PESPACKET_IMPORT)
#       define MPEG2_PES_UTILITY_LIB __declspec(dllimport)
#   else
#       define MPEG2_PES_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_PES_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_PES_UTILITY_LIB
#endif

MPEG2_PES_UTILITY_LIB int MPEG_PES_NumericCoding2Text_StreamID(uint8_t stream_id, char* pszText, int strSize);


#endif	//__MPEG_PES_UTILITIES_ID_H__

