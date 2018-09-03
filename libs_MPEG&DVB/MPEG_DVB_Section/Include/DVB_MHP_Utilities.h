#ifndef _DVB_MHP_UTILITIES_H_
#define _DVB_MHP_UTILITIES_H_

#include "../compile.h"

#include <stdint.h>


//application_information_sectionœ‡πÿ
_CDL_EXPORT int DVB_MHP_NumericCoding2Text_ApplicationServiceBoundFlag(int service_bound_flag, char* pszText, int strSize);
_CDL_EXPORT int DVB_MHP_NumericCoding2Text_ApplicationVisibility(int visibility_code, char* pszText, int strSize);
_CDL_EXPORT int DVB_MHP_NumericCoding2Text_ApplicationTransportProtocolID(uint16_t protocol_id, char* pszText, int strSize);

_CDL_EXPORT int AIT_ApplicationTypeLookup(uint16_t application_type, char* pszText, int strSize);
_CDL_EXPORT int AIT_ApplicationControlCodeLookup(uint16_t application_type, uint8_t application_control_code, char* pszText, int strSize);

#endif
