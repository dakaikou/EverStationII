#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/MPEG_DVB_Common_XML.h"


#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_PresentIOR_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IOP::IOR_t* pIOR)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	char	pszField[96];
	char	pszComment[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pIOR != NULL))
	{
		//解析IOP::IOR()
		XMLElement* pxmlIORNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "IOP::IOR()", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlIORNode, "type_id_length", pIOR->type_id_length, 32, "uimsbf", "N1");

		int N1 = pIOR->type_id_length;
		if (N1 > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlIORNode, "type_id_byte[ ]", (uint8_t*)pIOR->type_id_byte, N1, pIOR->type_id_byte);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlIORNode, "taggedProfiles_count", pIOR->taggedProfiles_count, 32, "uimsbf", "N2");

		if (pIOR->taggedProfiles_count > 0)
		{
			XMLElement* pxmlProfilesNode = XMLDOC_NewElementForString(pxmlDoc, pxmlIORNode, "taggedProfiles()", NULL);

			for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
			{
				uint32_t tag = pIOR->taggedProfile[profile_index].profileId_tag;

				sprintf_s(pszField, sizeof(pszField), "IOP::taggedProfile[%d](tag:0x%08X)", profile_index, tag);
				XMLElement* pxmlProfileNode = XMLDOC_NewElementForString(pxmlDoc, pxmlProfilesNode, pszField, NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlProfileNode, "profileId_tag", pIOR->taggedProfile[profile_index].profileId_tag, 32, "uimsbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlProfileNode, "profile_data_length", pIOR->taggedProfile[profile_index].profile_data_length, 32, "uimsbf", "N3");

				if (pIOR->taggedProfile[profile_index].profileId_tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
				{
					XMLNODE_SetAttribute(pxmlProfileNode, "comment", "BIOP Profile Body");

					BIOPProfileBody_t* pBIOPProfileBody = &(pIOR->taggedProfile[profile_index].u.BIOPProfileBody);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlProfileNode, "profile_data_byte_order", pBIOPProfileBody->profile_data_byte_order, 8, "uimsbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlProfileNode, "liteComponents_count", pBIOPProfileBody->liteComponents_count, 8, "uimsbf", NULL);

					if (pBIOPProfileBody->liteComponents_count > 0)		//大于等于2
					{
						//BIOP::ObjectLocation
						BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);
						assert(pObjectLocation->componentId_tag == 0x49534F50);

						sprintf_s(pszField, sizeof(pszField), "BIOP::ObjectLocation(tag:0x%08X)", pObjectLocation->componentId_tag);
						XMLElement* pxmlObjectLocationNode = XMLDOC_NewElementForString(pxmlDoc, pxmlProfileNode, pszField, NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "componentId_tag", pObjectLocation->componentId_tag, 32, "uimsbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "component_data_length", pObjectLocation->component_data_length, 8, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "carouselId", pObjectLocation->carouselId, 32, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "moduleId", pObjectLocation->moduleId, 16, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "version.major", pObjectLocation->version.major, 8, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "version.minor", pObjectLocation->version.minor, 8, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "objectKey_length", pObjectLocation->objectKey_length, 8, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlObjectLocationNode, "objectKey_data", pObjectLocation->objectKey_data, 32, "uimsbf", NULL);

						//DSM::ConnBinder
						DSM::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);
						sprintf_s(pszField, sizeof(pszField), "DSM::ConnBinder(tag:0x%08X)", pConnBinder->componentId_tag);
						assert(pConnBinder->componentId_tag == 0x49534F40);
						XMLElement* pxmlConnBinderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlProfileNode, pszField, NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlConnBinderNode, "componentId_tag", pConnBinder->componentId_tag, 32, "uimsbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlConnBinderNode, "component_data_length", pConnBinder->component_data_length, 8, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlConnBinderNode, "taps_count", pConnBinder->taps_count, 8, "uimsbf", NULL);

						for (int tap_index = 0; tap_index < pConnBinder->taps_count; tap_index++)
						{
							BIOP::TAP_t* pTap = pConnBinder->Tap + tap_index;

							sprintf_s(pszField, sizeof(pszField), "BIOP::Tap[%d]()", tap_index);
							XMLElement* pxmlTapNode = XMLDOC_NewElementForString(pxmlDoc, pxmlConnBinderNode, pszField, NULL);

							XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "id", pTap->id, 16, "uimsbf", NULL);

							MPEG2_DSMCC_NumericCoding2Text_tapUse(pTap->use, pszComment, sizeof(pszComment));
							XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "use", pTap->use, 16, "uimsbf", pszComment);

							XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "association_tag", pTap->association_tag, 16, "uimsbf", NULL);

							XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "selector_length", pTap->selector_length, 8, "uimsbf", NULL);

							if (tap_index == 0)
							{
								XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "selector_type", pTap->selector_type, 16, "uimsbf", NULL);

								uint16_t dii_table_id_extension = pTap->transactionId & 0x0000ffff;
								sprintf_s(pszComment, sizeof(pszComment), "* dii_table_id_extension=0x%04x", dii_table_id_extension);
								XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "transactionId", pTap->transactionId, 32, "uimsbf", pszComment);

								XMLDOC_NewElementForBits(pxmlDoc, pxmlTapNode, "timeout", pTap->timeout, 32, "uimsbf", NULL);
							}
							else
							{
								assert(0);
								XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlTapNode, "selector_data_byte[]", NULL, pTap->selector_length, NULL);
							}
						}

						for (int component_index = 0; component_index < pBIOPProfileBody->liteComponents_count - 2; component_index++)
						{
							BIOP::LiteComponent_t* pliteComponent = pBIOPProfileBody->liteComponents + component_index;

							sprintf_s(pszField, sizeof(pszField), "liteComponent[%d](tag:0x%08X)", component_index, pliteComponent->componentId_tag);
							XMLElement* pxmlComponentNode = XMLDOC_NewElementForString(pxmlDoc, pxmlProfileNode, pszField, NULL);

							XMLDOC_NewElementForBits(pxmlDoc, pxmlComponentNode, "componentId_tag", pliteComponent->componentId_tag, 32, "uimsbf", NULL);
							XMLDOC_NewElementForBits(pxmlDoc, pxmlComponentNode, "component_data_length", pliteComponent->component_data_length, 8, "uimsbf", NULL);
							if (pliteComponent->component_data_length > 0)
							{
								XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlComponentNode, "component_data_byte[ ]", pliteComponent->component_data_byte, pliteComponent->component_data_length, NULL);
							}
						}
					}
				}
				else if (pIOR->taggedProfile[profile_index].profileId_tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
				{
					XMLNODE_SetAttribute(pxmlProfileNode, "comment", "LiteOptions Profile Body");

					LiteOptionsProfileBody_t* pLiteOptionsProfileBody = &(pIOR->taggedProfile[profile_index].u.LiteOptionsProfileBody);

					if (pLiteOptionsProfileBody->liteComponents_count > 0)		//大于等于2
					{
						for (int component_index = 0; component_index < pLiteOptionsProfileBody->liteComponents_count - 2; component_index++)
						{
							BIOP::LiteComponent_t* pliteComponent = pLiteOptionsProfileBody->liteComponents + component_index;

							sprintf_s(pszField, sizeof(pszField), "liteComponent[%d](tag:0x%08X)", component_index, pliteComponent->componentId_tag);
							XMLElement* pxmlComponentNode = XMLDOC_NewElementForString(pxmlDoc, pxmlProfileNode, pszField, NULL);

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
					XMLNODE_SetAttribute(pxmlProfileNode, "comment", "Unknown Profile Body");

					//pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "profile_data_byte[ ]", ptemp, pIOR->taggedProfile[profile_index].profile_data_length, NULL, pbs);
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



