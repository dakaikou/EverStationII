#ifndef _DVB_SI_UTILITIES_H_
#define _DVB_SI_UTILITIES_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DVB_SI_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DVB_SI_UTILITY_LIB __declspec(dllimport)
#   else
#       define DVB_SI_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define DVB_SI_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define DVB_SI_UTILITY_LIB
#endif

#include <stdint.h>

//defined in EN 300 468

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_UTCTime(uint64_t time_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_BCDTime(uint32_t bcd_code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_ServiceType(uint8_t service_type_code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_FrequencyCode(uint16_t code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_Bitrate(uint32_t code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_FECOuter(uint8_t FEC_outer_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_FECInner(uint8_t FEC_inner_code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_CableModulation(uint8_t modulation_code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_SatelliteWestEastFlag(uint8_t west_east_flag_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_SatellitePolarization(uint8_t polarization_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_SatelliteModulationSystem(uint8_t modulation_system_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_SatelliteModulationType(uint8_t modulation_type_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_SatelliteModulationRollOff(uint8_t roll_off_code, char* pszText, int strSize);

//�����г����
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_TerrestrialBandWidth(uint8_t terrestrial_bandwidth_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_TerrestrialPriority(uint8_t terrestrial_priority_code, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_TerrestrialConstellation(uint8_t terrestrial_constellation_code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_RunningStatus(uint8_t running_status_code, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_DataBroadcastID(uint16_t data_broadcast_ID, char* pszText, int strSize);
DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_CarouselTypeID(uint8_t carousel_type_id, char* pszText, int strSize);

DVB_SI_UTILITY_LIB int DVB_SI_NumericCoding2Text_TeletextType(uint8_t teletext_type, char* pszText, int size);

#endif
