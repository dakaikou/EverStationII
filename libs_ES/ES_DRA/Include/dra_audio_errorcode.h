#ifndef _DRA_AUDIO_ERROR_H_
#define _DRA_AUDIO_ERROR_H_

#ifdef DRA_NO_ERROR
#undef DRA_NO_ERROR
#endif

#define DRA_NO_ERROR					0x00000000

//ÏµÍ³´íÎó´úÂë
#define DRA_PARAMETER_ERROR				0xE0050001

#define DRA_UNKNOWN_ERROR				0xFFFFFFFF

#endif