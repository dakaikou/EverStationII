#ifndef __API_DRA_AUDIO_TOOLS_H__
#define __API_DRA_AUDIO_TOOLS_H__

#include <stdint.h>
#include "../Compile.h"

_CDL_EXPORT int DRA_DecodeFrmHeaderType(uint8_t nFrmHeaderType, char* pszText, int strSize);
_CDL_EXPORT int DRA_DecodeSamplingFrequency(uint8_t sample_rate_index, char* pszText, int strSize);

#endif

