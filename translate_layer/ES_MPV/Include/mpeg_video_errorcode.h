#ifndef _MPEG_VIDEO_ERROR_H_
#define _MPEG_VIDEO_ERROR_H_

#ifdef MPV_NO_ERROR
#undef MPV_NO_ERROR
#endif

#define MPV_NO_ERROR					0x00000000

//ÏµÍ³´íÎó´úÂë
#define MPV_PARAMETER_ERROR				0xE0080001

#define MPV_UNKNOWN_ERROR				0xFFFFFFFF

#endif