#ifndef _MPEG_DSMCC_UNM_DII_DC_H_
#define _MPEG_DSMCC_UNM_DII_DC_H_

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


#include "../MPEG2_DVB_SysCapability.h"
#include "../MPEG2_DVB_Common.h"
#include "../Mpeg2_DSMCC_Descriptor.h"
#include "Mpeg2_DSMCC_BIOP.h"

//for DC
typedef struct DC_moduleInfo_s
{
	name_descriptor_t			name_descriptor;
	location_descriptor_t		location_descriptor;

	int							module_descriptor_count;
	reserved_descriptor_t		module_descriptors[MAX_RESERVED_DESCRIPTORS];

} DC_ModuleInfo_t;

DSMCC_UNM_DII_LIB int	MPEG2_DSMCC_DII_DC_DecodeModuleInfo(uint8_t *buf, int length, DC_ModuleInfo_t* pDC_ModuleInfo);


#endif
