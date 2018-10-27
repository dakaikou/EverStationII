#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_OC.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_DII_OC_DecodeModuleInfo(uint8_t *buf, int length, BIOP::ModuleInfo_t* pOC_ModuleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length >= 14) && (pOC_ModuleInfo != NULL))
	{
		BYTES_t bytes;
		BYTES_map(&bytes, buf, length);

		pOC_ModuleInfo->moduleTimeOut = BYTES_get(&bytes, 4);

		pOC_ModuleInfo->blockTimeOut = BYTES_get(&bytes, 4);

		pOC_ModuleInfo->minBlockTime = BYTES_get(&bytes, 4);

		pOC_ModuleInfo->taps_count = BYTES_get(&bytes, 1);
		assert(pOC_ModuleInfo->taps_count <= 4);

		for (int tap_index = 0; tap_index < pOC_ModuleInfo->taps_count; tap_index++)
		{
			pOC_ModuleInfo->TAP[tap_index].id = BYTES_get(&bytes, 2);

			pOC_ModuleInfo->TAP[tap_index].use = BYTES_get(&bytes, 2);

			pOC_ModuleInfo->TAP[tap_index].association_tag = BYTES_get(&bytes, 2);

			pOC_ModuleInfo->TAP[tap_index].selector_length = BYTES_get(&bytes, 1);
			if (pOC_ModuleInfo->TAP[tap_index].selector_length > 0)
			{
				assert(0);
				BYTES_skip(&bytes, pOC_ModuleInfo->TAP[tap_index].selector_length);
			}
		}

		pOC_ModuleInfo->userInfoLength = BYTES_get(&bytes, 1);
		if (pOC_ModuleInfo->userInfoLength > 0)
		{
			pOC_ModuleInfo->userInfo_data_byte = bytes.p_cur;
			BYTES_skip(&bytes, pOC_ModuleInfo->userInfoLength);
			//BYTES_copy(pBIOP_moduleInfo->userInfo_data_byte, sizeof(pOC_ModuleInfo->userInfo_data_byte), &bytes, pOC_ModuleInfo->userInfoLength);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}



