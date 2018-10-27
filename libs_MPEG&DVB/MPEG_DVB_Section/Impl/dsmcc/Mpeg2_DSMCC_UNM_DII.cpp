#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII.h"
#include "..\..\Include\MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"
/////////////////////////////////////////////
int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	//int			reserved_count;
	//uint16_t	descriptor_tag;
	//uint8_t		descriptor_length;
	//int			move_length;
	//int			tap_index;

	//DC_moduleInfo_t*	pDC_moduleInfo;
	//BIOP_moduleInfo_t*	pBIOP_moduleInfo;

	if ((buf != NULL) && (length >= 22) && (pDownloadInfoIndication != NULL))
	{
		memset(pDownloadInfoIndication, 0x00, sizeof(DownloadInfoIndication_t));

		BYTES_t bytes;
		BYTES_map(&bytes, buf, length);

		pDownloadInfoIndication->downloadId = BYTES_get(&bytes, 4);

		pDownloadInfoIndication->blockSize = BYTES_get(&bytes, 2);

		pDownloadInfoIndication->windowSize = BYTES_get(&bytes, 1);

		pDownloadInfoIndication->ackPeriod = BYTES_get(&bytes, 1);

		pDownloadInfoIndication->tCDownloadWindow = BYTES_get(&bytes, 4);

		pDownloadInfoIndication->tCDownloadScenario = BYTES_get(&bytes, 4);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadInfoIndication->compatibilityDescriptor);
		pcompatibilityDescriptor->compatibilityDescriptorLength = BYTES_get(&bytes, 2);
		if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
		{
			pcompatibilityDescriptor->compatibilityDescriptorBuf = bytes.p_cur;
			BYTES_skip(&bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
			//BYTES_copy(pcompatibilityDescriptor->compatibilityDescriptorBuf, sizeof(pcompatibilityDescriptor), &bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
		}

		//defined in EN 301192
		pDownloadInfoIndication->numberOfModules = BYTES_get(&bytes, 2);
		assert(pDownloadInfoIndication->numberOfModules <= MAX_MODULES_PER_GROUP);

		for (int moduleIndex = 0; moduleIndex < pDownloadInfoIndication->numberOfModules; moduleIndex++)
		{
			ModuleInfo_t* pmoduleInfo = pDownloadInfoIndication->ModuleInfo + moduleIndex;

			pmoduleInfo->moduleId = BYTES_get(&bytes, 2);

			pmoduleInfo->moduleSize = BYTES_get(&bytes, 4);

			pmoduleInfo->moduleVersion = BYTES_get(&bytes, 1);
			pmoduleInfo->moduleInfoLength = BYTES_get(&bytes, 1);

			if (pmoduleInfo->moduleInfoLength > 0)
			{
				uint8_t* moduleInfoByte = bytes.p_cur;
				pmoduleInfo->moduleInfoByte = moduleInfoByte;
				BYTES_skip(&bytes, pmoduleInfo->moduleInfoLength);

				//判断moduleInfo payload的类型
				pmoduleInfo->data_broadcast_type = 0x0006;			//默认假设为DC
				if (pmoduleInfo->moduleInfoLength >= 21)
				{
					int taps_count = moduleInfoByte[12];
					if (taps_count >= 1)
					{
						uint16_t ID = (moduleInfoByte[13] << 8) | moduleInfoByte[14];
						uint16_t use = (moduleInfoByte[15] << 8) | moduleInfoByte[16];
						uint8_t selector_length = moduleInfoByte[19];
						if ((ID == 0x0000) && (use == 0x0017) && (selector_length == 0x00))
						{
							pmoduleInfo->data_broadcast_type = 0x0007;	//假设为OC
						}
					}
				}

				if (pmoduleInfo->data_broadcast_type == 0x0006)
				{
					DC_ModuleInfo_t dcModuleInfo;
					MPEG2_DSMCC_DII_DC_DecodeModuleInfo(pmoduleInfo->moduleInfoByte, pmoduleInfo->moduleInfoLength, &dcModuleInfo);
					if (strlen(dcModuleInfo.name_descriptor.text_char) > 0)
					{
						strcpy_s(pmoduleInfo->moduleName, sizeof(pmoduleInfo->moduleName), dcModuleInfo.name_descriptor.text_char);
					}
				}
			}
		}

		pDownloadInfoIndication->privateDataLength = BYTES_get(&bytes, 2);
		if (pDownloadInfoIndication->privateDataLength > 0)
		{
			pDownloadInfoIndication->privateDataByte = bytes.p_cur;
			BYTES_skip(&bytes, pDownloadInfoIndication->privateDataLength);
			//BYTES_copy(pDownloadInfoIndication->privateDataByte, sizeof(pDownloadInfoIndication->privateDataByte), &bytes, pDownloadInfoIndication->privateDataLength);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}



