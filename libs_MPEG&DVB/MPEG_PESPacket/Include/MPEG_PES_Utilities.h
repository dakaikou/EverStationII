#ifndef __MPEG_PES_UTILITIES_ID_H__
#define __MPEG_PES_UTILITIES_ID_H__

#include <stdint.h>

#include "../compile.h"

_CDL_EXPORT int MPEG_PES_NumericCoding2Text_StreamID(uint8_t stream_id, char* pszText, int strSize);
_CDL_EXPORT uint8_t GetTSPayloadClassByStreamID(uint8_t stream_id);


#endif	//__MPEG_PES_UTILITIES_ID_H__

