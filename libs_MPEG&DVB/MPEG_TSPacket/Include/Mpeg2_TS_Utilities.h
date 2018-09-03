#ifndef __MPEG2_TS_PAYLOAD_CLASS_H__
#define __MPEG2_TS_PAYLOAD_CLASS_H__

#include <stdint.h>

#include "../compile.h"

typedef enum _TSPAYLOAD_CLASS_e
{
	TSPAYLOAD_CLASS_UNKNOWN = 0b000,
	TSPAYLOAD_CLASS_SECTION = 0b001,
	TSPAYLOAD_CLASS_PES_AUDIO = 0b010,
	TSPAYLOAD_CLASS_PES_AUDIO_RSV = 0b011,
	TSPAYLOAD_CLASS_PES_VIDEO = 0b100,
	TSPAYLOAD_CLASS_PES_VIDEO_RSV = 0b101,
	TSPAYLOAD_CLASS_PES_DATA = 0b110,
	TSPAYLOAD_CLASS_PES_DATA_SECTION = 0b111				//是PMT中指示的ES流，并且还是section

} TSPAYLOAD_CLASS_e;

_CDL_EXPORT int MPEG2_TS_GetPayloadClassByPID(uint16_t PID);

_CDL_EXPORT int MPEG2_TS_NumericCoding2Text_PID(uint16_t PID, char* pszText, int strSize);
_CDL_EXPORT int MPEG2_TS_NumericCoding2Text_transport_scrambling_control(uint8_t transport_scrambling_control_code, char* pszText, int strSize);
_CDL_EXPORT int MPEG2_TS_NumericCoding2Text_adaptation_field_control(uint8_t adaptation_field_control_code, char* pszText, int strSize);

#endif

