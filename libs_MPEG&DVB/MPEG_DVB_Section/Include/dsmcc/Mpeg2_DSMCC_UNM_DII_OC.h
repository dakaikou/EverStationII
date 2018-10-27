#ifndef _MPEG_DSMCC_UNM_DII_OC_H_
#define _MPEG_DSMCC_UNM_DII_OC_H_

#include <stdint.h>

#include "../../compile.h"

#include "Mpeg2_DSMCC_BIOP.h"

_CDL_EXPORT int	MPEG2_DSMCC_DII_OC_DecodeModuleInfo(uint8_t *buf, int length, BIOP::ModuleInfo_t* pOC_ModuleInfo);


#endif
