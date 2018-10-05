#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../nativeInclude/Mpeg2_DSMCC_Utilities.h"
#include "../../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "../../nativeInclude/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC.h"

#include "../../xmlInclude/MPEG_DVB_Common_XML.h"
#include "../../xmlInclude/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC_XML.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DSI_DC_PresentGroupInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pGroupInfoIndication != NULL))
	{
		XMLElement* pxmlGroupsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "GroupInfoIndication()", NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupsNode, "NumberOfGroups", pGroupInfoIndication->NumberOfGroups, 2, NULL);

		for (int group_index = 0; group_index < pGroupInfoIndication->NumberOfGroups; group_index++)
		{
			GroupInfo_t* pGroupInfo = pGroupInfoIndication->GroupInfo + group_index;
			sprintf_s(pszField, sizeof(pszField), "GROUP[%d](ID:0x%08X)", group_index, pGroupInfo->GroupId);

			XMLElement* pxmlGroupNode = XMLDOC_NewElementForString(pxmlDoc, pxmlGroupsNode, pszField, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupNode, "GroupId", pGroupInfo->GroupId, 4, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupNode, "GroupSize", pGroupInfo->GroupSize, 4, NULL);

			compatibilityDescriptor_t* pcompatibilityDescriptor = &(pGroupInfo->GroupCompatibility);
			XMLElement* pxmlGroupCompatibilityNode = XMLDOC_NewElementForString(pxmlDoc, pxmlGroupNode, "GroupCompatibility()", NULL);
			XMLNODE_SetFieldLength(pxmlGroupCompatibilityNode, 2 + pcompatibilityDescriptor->compatibilityDescriptorLength);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupCompatibilityNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);
			if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
			{
				XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlGroupCompatibilityNode, "compatibilityDescriptorBuf[ ]", pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
			}

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupNode, "GroupInfoLength", pGroupInfo->GroupInfoLength, 2, NULL);

			if (pGroupInfo->GroupInfoLength > 0)
			{
				XMLElement* pxmlDescriptionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlGroupNode, "GroupInfo()", NULL);

				for (int descriptor_index = 0; descriptor_index < pGroupInfo->group_descriptor_count; descriptor_index++)
				{
					uint8_t* descriptor_buf = pGroupInfo->group_descriptors[descriptor_index].descriptor_buf;
					uint16_t descriptor_tag = pGroupInfo->group_descriptors[descriptor_index].descriptor_tag;
					uint8_t descriptor_length = pGroupInfo->group_descriptors[descriptor_index].descriptor_length;
					int descriptor_size = descriptor_length + 2;

					switch (descriptor_tag)
					{
						//case DVB_SI_NETWORK_NAME_DESCRIPTOR:
						//	DVB_SI_decode_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
						//	break;
						//case DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:
						//	DVB_SI_decode_multilingual_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
						//	break;
					default:
						MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlDescriptionNode, pGroupInfo->group_descriptors + descriptor_index);
						break;
					}
				}

			}
		}

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupsNode, "PrivateDataLength", pGroupInfoIndication->PrivateDataLength, 2, NULL);
		if (pGroupInfoIndication->PrivateDataLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlGroupsNode, "privateDataByte[ ]", pGroupInfoIndication->privateDataByte, pGroupInfoIndication->PrivateDataLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

