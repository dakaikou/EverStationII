#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC.h"

#include "../../Include/MPEG_DVB_Common_XML.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC_XML.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGII)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	GroupInfoIndication_t* pGroupInfoIndication = (pGII != NULL) ? pGII : new GroupInfoIndication_t;
	rtcode = MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(buf, size, pGroupInfoIndication);
	rtcode = MPEG2_DSMCC_DSI_DC_PresentGroupInfoIndication_to_xml(pxmlDoc, pxmlParentNode, pGroupInfoIndication);

	if (pGII == NULL)
	{
		//说明pGroupInfoIndication指针临时分配，函数返回前需要释放
		delete pGroupInfoIndication;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DSI_DC_PresentGroupInfoIndication_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pGroupInfoIndication != NULL))
	{
		//XMLElement* pxmlGroupsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "GroupInfoIndication()", NULL);
		XMLElement* pxmlGroupsNode = pxmlParentNode;

		pxmlDoc->NewElementForByteMode(pxmlGroupsNode, "NumberOfGroups", pGroupInfoIndication->NumberOfGroups, 2, NULL);

		for (int group_index = 0; group_index < pGroupInfoIndication->NumberOfGroups; group_index++)
		{
			GroupInfo_t* pGroupInfo = pGroupInfoIndication->GroupInfo + group_index;
			sprintf_s(pszField, sizeof(pszField), "GROUP[%d](ID:0x%08X)", group_index, pGroupInfo->GroupId);

			XMLElement* pxmlGroupNode = pxmlDoc->NewBranchElement(pxmlGroupsNode, pszField, NULL, 4 + 4 + 2 + pGroupInfo->GroupCompatibility.compatibilityDescriptorLength + 2 + pGroupInfo->GroupInfoLength);

			pxmlDoc->NewElementForByteMode(pxmlGroupNode, "GroupId", pGroupInfo->GroupId, 4, NULL);

			pxmlDoc->NewElementForByteMode(pxmlGroupNode, "GroupSize", pGroupInfo->GroupSize, 4, NULL);

			compatibilityDescriptor_t* pcompatibilityDescriptor = &(pGroupInfo->GroupCompatibility);
			XMLElement* pxmlGroupCompatibilityNode = pxmlDoc->NewBranchElement(pxmlGroupNode, "GroupCompatibility()", NULL, 2 + pcompatibilityDescriptor->compatibilityDescriptorLength);

			pxmlDoc->NewElementForByteMode(pxmlGroupCompatibilityNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);
			if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
			{
				pxmlDoc->NewElementForByteBuf(pxmlGroupCompatibilityNode, "compatibilityDescriptorBuf[ ]", pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
			}

			pxmlDoc->NewElementForByteMode(pxmlGroupNode, "GroupInfoLength", pGroupInfo->GroupInfoLength, 2, NULL);

			if (pGroupInfo->GroupInfoLength > 0)
			{
				XMLElement* pxmlDescriptionNode = pxmlDoc->NewBranchElement(pxmlGroupNode, "GroupInfo()", NULL, pGroupInfo->GroupInfoLength);

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

		pxmlDoc->NewElementForByteMode(pxmlGroupsNode, "PrivateDataLength", pGroupInfoIndication->PrivateDataLength, 2, NULL);
		if (pGroupInfoIndication->PrivateDataLength > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlGroupsNode, "privateDataByte[ ]", pGroupInfoIndication->privateDataByte, pGroupInfoIndication->PrivateDataLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

