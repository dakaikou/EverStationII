#ifndef __MPEG2_TS_PAYLOAD_CLASS_H__
#define __MPEG2_TS_PAYLOAD_CLASS_H__

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_TSPACKET_EXPORT
#       define MPEG2_TS_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPEG2_TSPACKET_IMPORT)
#       define MPEG2_TS_UTILITY_LIB __declspec(dllimport)
#   else
#       define MPEG2_TS_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_TS_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_TS_UTILITY_LIB
#endif

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

MPEG2_TS_UTILITY_LIB int MPEG2_TS_GetPayloadClassByPID(uint16_t PID);
MPEG2_TS_UTILITY_LIB int MPEG2_TS_GetPayloadClassByStreamType(uint8_t stream_type, uint8_t sub_type);
MPEG2_TS_UTILITY_LIB int MPEG2_TS_GetPayloadClassByStreamID(uint8_t stream_id);

MPEG2_TS_UTILITY_LIB int MPEG2_TS_NumericCoding2Text_PID(uint16_t PID, char* pszText, int strSize);
MPEG2_TS_UTILITY_LIB int MPEG2_TS_NumericCoding2Text_transport_scrambling_control(uint8_t transport_scrambling_control_code, char* pszText, int strSize);
MPEG2_TS_UTILITY_LIB int MPEG2_TS_NumericCoding2Text_adaptation_field_control(uint8_t adaptation_field_control_code, char* pszText, int strSize);

#endif

