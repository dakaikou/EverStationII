#ifndef _AC3_AUDIO_ERROR_H_
#define _AC3_AUDIO_ERROR_H_

#ifdef AC3_NO_ERROR
#undef AC3_NO_ERROR
#endif

#define AC3_NO_ERROR					0x00000000

//ÏµÍ³´íÎó´úÂë
#define AC3_PARAMETER_ERROR				0xE0020001

#define AC3_UNKNOWN_ERROR				0xFFFFFFFF

#endif