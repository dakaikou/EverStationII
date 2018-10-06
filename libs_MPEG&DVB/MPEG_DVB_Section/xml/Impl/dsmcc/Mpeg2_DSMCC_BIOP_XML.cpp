#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/MPEG_DVB_Common_XML.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP_XML.h"


#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_IOP_PresentIOR_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IOP::IOR_t* pIOR)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	char	pszField[96];
	char	pszComment[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pIOR != NULL))
	{
		//解析IOP::IOR()
		//int ior_field_length = 8;
		////check the total length for xml node
		//int profile_total_length = 0;
		//for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
		//{
		//	IOP::TaggedProfile_t* ptaggedProfile = pIOR->taggedProfiles + profile_index;
		//	profile_total_length += (8 + ptaggedProfile->profile_data_length);
		//}
		//ior_field_length += profile_total_length;

		XMLElement* pxmlIORNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "IOP::IOR()", NULL);
		XMLNODE_SetFieldLength(pxmlIORNode, pIOR->total_length);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlIORNode, "type_id_length", pIOR->type_id_length, 32, "uimsbf", "N1");

		int N1 = pIOR->type_id_length;
		if (N1 > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlIORNode, "type_id_byte[ ]", (uint8_t*)pIOR->type_id_byte, N1, pIOR->type_id_byte);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlIORNode, "taggedProfiles_count", pIOR->taggedProfiles_count, 32, "uimsbf", "N2");

		if (pIOR->taggedProfiles_count > 0)
		{
			//XMLElement* pxmlProfilesNode = XMLDOC_NewElementForString(pxmlDoc, pxmlIORNode, "taggedProfiles()", NULL);
			//XMLNODE_SetFieldLength(pxmlProfilesNode, pIOR->taggedProfilesLength);

			for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
			{
				IOP::TaggedProfile_t* ptaggedProfile = pIOR->taggedProfiles + profile_index;
				uint32_t tag = ptaggedProfile->profileId_tag;

				sprintf_s(pszField, sizeof(pszField), "IOP::taggedProfiles[%d](tag:0x%08X)", profile_index, tag);
				if (tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
				{
					sprintf_s(pszComment, sizeof(pszComment), "BIOP Profile Body");
				}
				else if (tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
				{
					sprintf_s(pszComment, sizeof(pszComment), "LiteOptions Profile Body");
				}
				else
				{
					sprintf_s(pszComment, sizeof(pszComment), "Unknown Profile Body");
				}
				XMLElement* pxmlProfileNode = XMLDOC_NewElementForString(pxmlDoc, pxmlIORNode, pszField, pszComment);
				XMLNODE_SetFieldLength(pxmlProfileNode, 8 + ptaggedProfile->profile_data_length);

				if (ptaggedProfile->profileId_tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
				{
					MPEG2_DSMCC_IOP_DecodeBIOPProfileBody_to_xml(ptaggedProfile->profileId_tag,
						ptaggedProfile->profile_data_length,
						ptaggedProfile->profile_data_byte,
						pxmlDoc,
						pxmlProfileNode);
				}
				else if (ptaggedProfile->profileId_tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
				{
					MPEG2_DSMCC_IOP_DecodeLiteOptionsProfileBody_to_xml(ptaggedProfile->profileId_tag,
						ptaggedProfile->profile_data_length,
						ptaggedProfile->profile_data_byte,
						pxmlDoc,
						pxmlProfileNode);
				}
				else
				{
					XMLDOC_NewElementForBits(pxmlDoc, pxmlProfileNode, "profileId_tag", ptaggedProfile->profileId_tag, 32, "uimsbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlProfileNode, "profile_data_length", ptaggedProfile->profile_data_length, 32, "uimsbf", "N3");
					XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlProfileNode, "profile_data_byte[ ]", ptaggedProfile->profile_data_byte, ptaggedProfile->profile_data_length, NULL);
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


int	MPEG2_DSMCC_IOP_DecodeBIOPProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOPProfileBody_t* pProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	BIOPProfileBody_t* pBIOPProfileBody = (pProfileBody != NULL) ? pProfileBody : new BIOPProfileBody_t;
	rtcode = MPEG2_DSMCC_IOP_DecodeBIOPProfileBody(profileId_tag, profile_data_length, profile_data_byte, pBIOPProfileBody);
	rtcode = MPEG2_DSMCC_IOP_PresentBIOPProfileBody_to_xml(pxmlDoc, pxmlParentNode, pBIOPProfileBody);

	if (pProfileBody == NULL)
	{
		//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
		delete pBIOPProfileBody;
	}

	return rtcode;
}

int	MPEG2_DSMCC_IOP_PresentBIOPProfileBody_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOPProfileBody_t* pBIOPProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[64];
	char	pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pBIOPProfileBody != NULL))
	{
		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "profileId_tag", pBIOPProfileBody->profileId_tag, 32, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "profile_data_length", pBIOPProfileBody->profile_data_length, 32, "uimsbf", "N3");

		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "profile_data_byte_order", pBIOPProfileBody->profile_data_byte_order, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "liteComponents_count", pBIOPProfileBody->liteComponents_count, 8, "uimsbf", NULL);

		if (pBIOPProfileBody->liteComponents_count > 0)		//大于等于2
		{
			//liteComponent 0
			{
				//BIOP::ObjectLocation
				BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);
				assert(pObjectLocation->componentId_tag == 0x49534F50);

				sprintf_s(pszField, sizeof(pszField), "BIOP::ObjectLocation(tag:0x%08X)", pObjectLocation->componentId_tag);
				XMLElement* pxmlObjectLocationNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlObjectLocationNode, 5 + pObjectLocation->component_data_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "componentId_tag", pObjectLocation->componentId_tag, 32, "uimsbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "component_data_length", pObjectLocation->component_data_length, 8, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "carouselId", pObjectLocation->carouselId, 32, "uimsbf", NULL);

				sprintf_s(pszComment, sizeof(pszComment), "* indicates ddb_table_id_extension=0x%04x", pObjectLocation->moduleId);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "moduleId", pObjectLocation->moduleId, 16, "uimsbf", pszComment);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "version.major", pObjectLocation->version.major, 8, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "version.minor", pObjectLocation->version.minor, 8, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "objectKey_length", pObjectLocation->objectKey_length, 8, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "objectKey_data", pObjectLocation->objectKey_data, 32, "uimsbf", NULL);
			}

			//liteComponent 1
			{
				//DSM::ConnBinder
				DSM::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);
				sprintf_s(pszField, sizeof(pszField), "DSM::ConnBinder(tag:0x%08X)", pConnBinder->componentId_tag);
				assert(pConnBinder->componentId_tag == 0x49534F40);
				XMLElement* pxmlConnBinderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlConnBinderNode, 5 + pConnBinder->component_data_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlConnBinderNode, "componentId_tag", pConnBinder->componentId_tag, 32, "uimsbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlConnBinderNode, "component_data_length", pConnBinder->component_data_length, 8, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlConnBinderNode, "taps_count", pConnBinder->taps_count, 8, "uimsbf", NULL);

				for (int tap_index = 0; tap_index < pConnBinder->taps_count; tap_index++)
				{
					BIOP::TAP_t* pTap = pConnBinder->Tap + tap_index;

					sprintf_s(pszField, sizeof(pszField), "BIOP::Tap[%d]()", tap_index);
					XMLElement* pxmlTapNode = XMLDOC_NewElementForString(pxmlDoc, pxmlConnBinderNode, pszField, NULL);
					XMLNODE_SetFieldLength(pxmlTapNode, 7 + pTap->selector_length);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "id", pTap->id, 16, "uimsbf", NULL);

					MPEG2_DSMCC_NumericCoding2Text_tapUse(pTap->use, pszComment, sizeof(pszComment));
					XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "use", pTap->use, 16, "uimsbf", pszComment);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "association_tag", pTap->association_tag, 16, "uimsbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "selector_length", pTap->selector_length, 8, "uimsbf", NULL);

					if (tap_index == 0)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "selector_type", pTap->selector_type, 16, "uimsbf", NULL);

						uint16_t dii_table_id_extension = pTap->transactionId & 0x0000ffff;
						sprintf_s(pszComment, sizeof(pszComment), "* indicates dii_table_id_extension=0x%04x", dii_table_id_extension);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "transactionId", pTap->transactionId, 32, "uimsbf", pszComment);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "timeout", pTap->timeout, 32, "uimsbf", NULL);
					}
					else
					{
						assert(0);
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlTapNode, "selector_data_byte[]", NULL, pTap->selector_length, NULL);
					}
				}
			}

			//liteComponent 2 ~ (liteComponents_count - 1)
			for (int component_index = 0; component_index < pBIOPProfileBody->liteComponents_count - 2; component_index++)
			{
				BIOP::LiteComponent_t* pliteComponent = pBIOPProfileBody->liteComponents + component_index;

				sprintf_s(pszField, sizeof(pszField), "liteComponent[%d](tag:0x%08X)", component_index, pliteComponent->componentId_tag);
				XMLElement* pxmlComponentNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlComponentNode, 5 + pliteComponent->component_data_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlComponentNode, "componentId_tag", pliteComponent->componentId_tag, 32, "uimsbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlComponentNode, "component_data_length", pliteComponent->component_data_length, 8, "uimsbf", NULL);
				if (pliteComponent->component_data_length > 0)
				{
					XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlComponentNode, "component_data_byte[ ]", pliteComponent->component_data_byte, pliteComponent->component_data_length, NULL);
				}
			}
		}
		else
		{
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}

	return rtcode;
}

int	MPEG2_DSMCC_IOP_DecodeLiteOptionsProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, LiteOptionsProfileBody_t* pProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	LiteOptionsProfileBody_t* pLiteOptionsProfileBody = (pProfileBody != NULL) ? pProfileBody : new LiteOptionsProfileBody_t;
	rtcode = MPEG2_DSMCC_IOP_DecodeLiteOptionsProfileBody(profileId_tag, profile_data_length, profile_data_byte, pLiteOptionsProfileBody);
	rtcode = MPEG2_DSMCC_IOP_PresentLiteOptionsProfileBody_to_xml(pxmlDoc, pxmlParentNode, pLiteOptionsProfileBody);

	if (pProfileBody == NULL)
	{
		//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
		delete pLiteOptionsProfileBody;
	}

	return rtcode;
}

int	MPEG2_DSMCC_IOP_PresentLiteOptionsProfileBody_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, LiteOptionsProfileBody_t* pLiteOptionsProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	char	pszField[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pLiteOptionsProfileBody != NULL))
	{
		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "profileId_tag", pLiteOptionsProfileBody->profileId_tag, 32, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "profile_data_length", pLiteOptionsProfileBody->profile_data_length, 32, "uimsbf", "N3");
		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "profile_data_byte_order", pLiteOptionsProfileBody->profile_data_byte_order, 8, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlParentNode, "liteComponents_count", pLiteOptionsProfileBody->liteComponents_count, 8, "uimsbf", NULL);

		if (pLiteOptionsProfileBody->liteComponents_count > 0)		//大于等于2
		{
			for (int component_index = 0; component_index < pLiteOptionsProfileBody->liteComponents_count - 2; component_index++)
			{
				BIOP::LiteComponent_t* pliteComponent = pLiteOptionsProfileBody->liteComponents + component_index;

				sprintf_s(pszField, sizeof(pszField), "liteComponent[%d](tag:0x%08X)", component_index, pliteComponent->componentId_tag);
				XMLElement* pxmlComponentNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, pszField, NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlComponentNode, "componentId_tag", pliteComponent->componentId_tag, 32, "uimsbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlComponentNode, "component_data_length", pliteComponent->component_data_length, 8, "uimsbf", NULL);
				if (pliteComponent->component_data_length > 0)
				{
					XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlComponentNode, "component_data_byte[ ]", pliteComponent->component_data_byte, pliteComponent->component_data_length, NULL);
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




