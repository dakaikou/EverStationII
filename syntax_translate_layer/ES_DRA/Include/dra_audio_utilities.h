#ifndef __API_DRA_AUDIO_TOOLS_H__
#define __API_DRA_AUDIO_TOOLS_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _DRA_EXPORT
#       define DRA_SYNTAX_LIB __declspec(dllexport)
#   elif defined(_DRA_IMPORT)
#       define DRA_SYNTAX_LIB __declspec(dllimport)
#   else
#       define DRA_SYNTAX_LIB
#   endif
#elif __GNUC__ >= 4
#   define DRA_SYNTAX_LIB __attribute__((visibility("default")))
#else
#   define DRA_SYNTAX_LIB
#endif

#include <stdint.h>

DRA_SYNTAX_LIB int DRA_DecodeFrmHeaderType(uint8_t nFrmHeaderType, char* pszText, int strSize);
DRA_SYNTAX_LIB int DRA_DecodeSamplingFrequency(uint8_t sample_rate_index, char* pszText, int strSize);

#endif

