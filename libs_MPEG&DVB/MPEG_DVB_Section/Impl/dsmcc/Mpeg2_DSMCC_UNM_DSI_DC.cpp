#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

//#ifndef min
//#define min(a,b)  (((a)<(b))?(a):(b))
//#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	int			reserved_count;
	uint16_t	descriptor_tag;
	uint8_t		descriptor_length;
	int			move_length;

	if ((buf != NULL) && (length > 0) && (pGroupInfoIndication != NULL))
	{
		memset(pGroupInfoIndication, 0x00, sizeof(GroupInfoIndication_t));

		BYTES_t bytes;
		BYTES_map(&bytes, buf, length);

		pGroupInfoIndication->NumberOfGroups = BYTES_get(&bytes, 2);
		assert(pGroupInfoIndication->NumberOfGroups <= MAX_GROUPS);

		for (int group_index = 0; group_index < pGroupInfoIndication->NumberOfGroups; group_index++)
		{
			GroupInfo_t* pGroupInfo = pGroupInfoIndication->GroupInfo + group_index;

			pGroupInfo->GroupId = BYTES_get(&bytes, 4);

			pGroupInfo->GroupSize = BYTES_get(&bytes, 4);

			compatibilityDescriptor_t* pcompatibilityDescriptor = &(pGroupInfo->GroupCompatibility);
			pcompatibilityDescriptor->compatibilityDescriptorLength = BYTES_get(&bytes, 2);
			if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
			{
				pcompatibilityDescriptor->compatibilityDescriptorBuf = bytes.p_cur;
				BYTES_skip(&bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
				//BYTES_copy(pcompatibilityDescriptor->compatibilityDescriptorBuf, sizeof(pcompatibilityDescriptor), &bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
			}

			pGroupInfo->GroupInfoLength = BYTES_get(&bytes, 2);
			if (pGroupInfo->GroupInfoLength > 0)
			{
				int remain_length = pGroupInfo->GroupInfoLength;
				uint8_t* ptemp = bytes.p_cur;
				BYTES_skip(&bytes, pGroupInfo->GroupInfoLength);

				reserved_count = 0;
				while ((remain_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
				{
					descriptor_tag = (ptemp[0] | 0x3000);
					descriptor_length = ptemp[1];
					move_length = descriptor_length + 2;

					pGroupInfo->group_descriptors[reserved_count].descriptor_tag = descriptor_tag;
					pGroupInfo->group_descriptors[reserved_count].descriptor_length = descriptor_length;
					pGroupInfo->group_descriptors[reserved_count].descriptor_buf = ptemp;
					pGroupInfo->group_descriptors[reserved_count].descriptor_size = move_length;
					reserved_count++;

					switch (descriptor_tag)
					{
					case MPEG2_DSMCC_NAME_DESCRIPTOR:
						MPEG2_DSMCC_decode_name_descriptor(ptemp, move_length, &(pGroupInfo->name_descriptor));
						break;
					case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
						MPEG2_DSMCC_decode_location_descriptor(ptemp, move_length, &(pGroupInfo->location_descriptor));
						break;
					default:
						if (descriptor_tag == 0x3081)
						{
							if (pGroupInfo->name_descriptor.descriptor_tag == 0x00)
							{
								MPEG2_DSMCC_decode_name_descriptor(ptemp, move_length, &(pGroupInfo->name_descriptor));
							}
						}

						break;
					}

					remain_length -= move_length;
					ptemp += move_length;
				}
				pGroupInfo->group_descriptor_count = reserved_count;
			}
		}

		pGroupInfoIndication->PrivateDataLength = BYTES_get(&bytes, 2);
		if (pGroupInfoIndication->PrivateDataLength > 0)
		{
			BYTES_copy(pGroupInfoIndication->privateDataByte, sizeof(pGroupInfoIndication->privateDataByte), &bytes, pGroupInfoIndication->PrivateDataLength);

			assert(bytes.p_cur == bytes.p_end);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




