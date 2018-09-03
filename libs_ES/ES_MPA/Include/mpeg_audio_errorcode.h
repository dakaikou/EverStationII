#ifndef _MPEG_AUDIO_ERROR_H_
#define _MPEG_AUDIO_ERROR_H_

#ifdef MPA_NO_ERROR
#undef MPA_NO_ERROR
#endif

#define MPA_NO_ERROR					0x00000000

//ÏµÍ³´íÎó´úÂë
#define MPA_PARAMETER_ERROR				0xE0070001

#define MPA_UNKNOWN_ERROR				0xFFFFFFFF

#endif