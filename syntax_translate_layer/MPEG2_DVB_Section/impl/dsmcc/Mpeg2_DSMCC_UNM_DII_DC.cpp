#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_DC.h"
#include "../../Include/MPEG2_DVB_ErrorCode.h"

//#include "../../Stream_AL/BitStream.h"
//#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_DII_DC_DecodeModuleInfo(uint8_t *buf, int length, DC_ModuleInfo_t* pDC_ModuleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	uint16_t descriptor_tag;
	uint8_t  descriptor_length;
	int		 move_length;

	if ((buf != NULL) && (length >= 2) && (pDC_ModuleInfo != NULL))
	{
		int reserved_count = 0;
		int loop_length = length;
		uint8_t* moduleInfoByte = buf;
		while ((loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
		{
			descriptor_tag = (moduleInfoByte[0] | 0x3000);
			descriptor_length = moduleInfoByte[1];
			move_length = descriptor_length + 2;

			pDC_ModuleInfo->module_descriptors[reserved_count].descriptor_tag = descriptor_tag;
			pDC_ModuleInfo->module_descriptors[reserved_count].descriptor_length = descriptor_length;
			pDC_ModuleInfo->module_descriptors[reserved_count].descriptor_buf = moduleInfoByte;
			pDC_ModuleInfo->module_descriptors[reserved_count].descriptor_size = move_length;

			switch (descriptor_tag)
			{
			case MPEG2_DSMCC_NAME_DESCRIPTOR:
				rtcode = MPEG2_DSMCC_decode_name_descriptor(moduleInfoByte, move_length, &(pDC_ModuleInfo->name_descriptor));
				break;
			case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
				rtcode = MPEG2_DSMCC_decode_location_descriptor(moduleInfoByte, move_length, &(pDC_ModuleInfo->location_descriptor));
				break;
			default:

				if (descriptor_tag == 0x3081)
				{
					if (pDC_ModuleInfo->name_descriptor.descriptor_tag == 0x0000)
					{
						rtcode = MPEG2_DSMCC_decode_name_descriptor(moduleInfoByte, move_length, &(pDC_ModuleInfo->name_descriptor));
					}
				}

				break;
			}

			reserved_count++;
			moduleInfoByte += move_length;
			loop_length -= move_length;
		}

		pDC_ModuleInfo->module_descriptor_count = reserved_count;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
/////////////////////////////////////////////

