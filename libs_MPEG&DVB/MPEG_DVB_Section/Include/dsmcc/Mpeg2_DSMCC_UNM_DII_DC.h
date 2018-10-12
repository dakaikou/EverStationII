#ifndef _MPEG_DSMCC_UNM_DII_DC_H_
#define _MPEG_DSMCC_UNM_DII_DC_H_

#include "../../compile.h"

#include "../MPEG_DVB_SysCapability.h"
#include "../MPEG_DVB_Common.h"
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

_CDL_EXPORT int	MPEG2_DSMCC_DII_DC_DecodeModuleInfo(uint8_t *buf, int length, DC_ModuleInfo_t* pDC_ModuleInfo);


#endif
