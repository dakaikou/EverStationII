#ifndef _DVB_MHP_UTILITIES_H_
#define _DVB_MHP_UTILITIES_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DVB_MHP_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DVB_MHP_UTILITY_LIB __declspec(dllimport)
#   else
#       define DVB_MHP_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define DVB_MHP_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define DVB_MHP_UTILITY_LIB
#endif

#include <stdint.h>


//application_information_sectionœ‡πÿ
DVB_MHP_UTILITY_LIB int DVB_MHP_Num2Text_ApplicationServiceBoundFlag(int service_bound_flag, char* pszText, int strSize);
DVB_MHP_UTILITY_LIB int DVB_MHP_Num2Text_ApplicationVisibility(int visibility_code, char* pszText, int strSize);
DVB_MHP_UTILITY_LIB int DVB_MHP_Num2Text_ApplicationTransportProtocolID(uint16_t protocol_id, char* pszText, int strSize);

DVB_MHP_UTILITY_LIB int AIT_ApplicationTypeLookup(uint16_t application_type, char* pszText, int strSize);
DVB_MHP_UTILITY_LIB int AIT_ApplicationControlCodeLookup(uint16_t application_type, uint8_t application_control_code, char* pszText, int strSize);

#endif
