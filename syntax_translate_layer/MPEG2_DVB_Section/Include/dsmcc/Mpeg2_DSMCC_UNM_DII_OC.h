#ifndef _MPEG_DSMCC_UNM_DII_OC_H_
#define _MPEG_DSMCC_UNM_DII_OC_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DSMCC_UNM_DII_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DSMCC_UNM_DII_LIB __declspec(dllimport)
#   else
#       define DSMCC_UNM_DII_LIB
#   endif
#elif __GNUC__ >= 4
#   define DSMCC_UNM_DII_LIB __attribute__((visibility("default")))
#else
#   define DSMCC_UNM_DII_LIB
#endif

#include <stdint.h>

#include "Mpeg2_DSMCC_BIOP.h"

DSMCC_UNM_DII_LIB int	MPEG2_DSMCC_DII_OC_DecodeModuleInfo(uint8_t *buf, int length, BIOP::ModuleInfo_t* pOC_ModuleInfo);


#endif
