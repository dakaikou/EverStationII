#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_BIOP.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"
#include "translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"

#include "../../Include/MPEG_DVB_Common_XML.h"

#include "syntax_express_xml/XML_MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_BIOP_XML.h"

/////////////////////////////////////////////

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_BIOP_PresentIOR_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::IOR_t* pIOR)
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

		XMLElement* pxmlIORNode = pxmlDoc->NewBranchElement(pxmlParentNode, "IOP::IOR()", NULL, pIOR->total_length);

		pxmlDoc->NewElementForBits(pxmlIORNode, "type_id_length", pIOR->type_id_length, 32, "uimsbf", "N1");

		int N1 = pIOR->type_id_length;
		if (N1 > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlIORNode, "type_id_byte[ ]", (uint8_t*)pIOR->type_id_byte, N1, pIOR->type_id_byte);
		}

		pxmlDoc->NewElementForBits(pxmlIORNode, "taggedProfiles_count", pIOR->taggedProfiles_count, 32, "uimsbf", "N2");

		if (pIOR->taggedProfiles_count > 0)
		{
			//XMLElement* pxmlProfilesNode = XMLDOC_NewElementForString(pxmlDoc, pxmlIORNode, "taggedProfiles()", NULL);
			//XMLNODE_SetFieldLength(pxmlProfilesNode, pIOR->taggedProfilesLength);

			for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
			{
				BIOP::TaggedProfile_t* ptaggedProfile = pIOR->taggedProfiles + profile_index;
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
				XMLElement* pxmlProfileNode = pxmlDoc->NewBranchElement(pxmlIORNode, pszField, pszComment, 8 + ptaggedProfile->profile_data_length);

				if (ptaggedProfile->profileId_tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
				{
					MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody_to_xml(ptaggedProfile->profileId_tag,
						ptaggedProfile->profile_data_length,
						ptaggedProfile->profile_data_byte,
						pxmlDoc,
						pxmlProfileNode);
				}
				else if (ptaggedProfile->profileId_tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
				{
					MPEG2_DSMCC_BIOP_DecodeLiteOptionsProfileBody_to_xml(ptaggedProfile->profileId_tag,
						ptaggedProfile->profile_data_length,
						ptaggedProfile->profile_data_byte,
						pxmlDoc,
						pxmlProfileNode);
				}
				else
				{
					pxmlDoc->NewElementForBits(pxmlProfileNode, "profileId_tag", ptaggedProfile->profileId_tag, 32, "uimsbf", NULL);
					pxmlDoc->NewElementForBits(pxmlProfileNode, "profile_data_length", ptaggedProfile->profile_data_length, 32, "uimsbf", "N3");
					pxmlDoc->NewElementForByteBuf(pxmlProfileNode, "profile_data_byte[ ]", ptaggedProfile->profile_data_byte, ptaggedProfile->profile_data_length, NULL);
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


int	MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::BIOPProfileBody_t* pProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	BIOP::BIOPProfileBody_t* pBIOPProfileBody = (pProfileBody != NULL) ? pProfileBody : new BIOP::BIOPProfileBody_t;
	rtcode = MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody(profileId_tag, profile_data_length, profile_data_byte, pBIOPProfileBody);
	rtcode = MPEG2_DSMCC_BIOP_PresentBIOPProfileBody_to_xml(pxmlDoc, pxmlParentNode, pBIOPProfileBody);

	if (pProfileBody == NULL)
	{
		//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
		delete pBIOPProfileBody;
	}

	return rtcode;
}

int	MPEG2_DSMCC_BIOP_PresentBIOPProfileBody_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::BIOPProfileBody_t* pBIOPProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[64];
	char	pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pBIOPProfileBody != NULL))
	{
		pxmlDoc->NewElementForBits(pxmlParentNode, "profileId_tag", pBIOPProfileBody->profileId_tag, 32, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlParentNode, "profile_data_length", pBIOPProfileBody->profile_data_length, 32, "uimsbf", "N3");

		pxmlDoc->NewElementForBits(pxmlParentNode, "profile_data_byte_order", pBIOPProfileBody->profile_data_byte_order, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "liteComponents_count", pBIOPProfileBody->liteComponents_count, 8, "uimsbf", NULL);

		if (pBIOPProfileBody->liteComponents_count > 0)		//大于等于2
		{
			//liteComponent 0
			{
				//BIOP::ObjectLocation
				BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);
				assert(pObjectLocation->componentId_tag == 0x49534F50);

				sprintf_s(pszField, sizeof(pszField), "BIOP::ObjectLocation(tag:0x%08X)", pObjectLocation->componentId_tag);
				XMLElement* pxmlObjectLocationNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, 5 + pObjectLocation->component_data_length);

				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "componentId_tag", pObjectLocation->componentId_tag, 32, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "component_data_length", pObjectLocation->component_data_length, 8, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "carouselId", pObjectLocation->carouselId, 32, "uimsbf", NULL);

				sprintf_s(pszComment, sizeof(pszComment), "* indicates ddb_table_id_extension=0x%04x", pObjectLocation->moduleId);
				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "moduleId", pObjectLocation->moduleId, 16, "uimsbf", pszComment);

				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "version.major", pObjectLocation->version.major, 8, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "version.minor", pObjectLocation->version.minor, 8, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "objectKey_length", pObjectLocation->objectKey_length, 8, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlObjectLocationNode, "objectKey_data", pObjectLocation->objectKey_data, 32, "uimsbf", NULL);
			}

			//liteComponent 1
			{
				//DSM::ConnBinder
				BIOP::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);
				sprintf_s(pszField, sizeof(pszField), "DSM::ConnBinder(tag:0x%08X)", pConnBinder->componentId_tag);
				assert(pConnBinder->componentId_tag == 0x49534F40);
				XMLElement* pxmlConnBinderNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, 5 + pConnBinder->component_data_length);

				pxmlDoc->NewElementForBits(pxmlConnBinderNode, "componentId_tag", pConnBinder->componentId_tag, 32, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlConnBinderNode, "component_data_length", pConnBinder->component_data_length, 8, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlConnBinderNode, "taps_count", pConnBinder->taps_count, 8, "uimsbf", NULL);

				for (int tap_index = 0; tap_index < pConnBinder->taps_count; tap_index++)
				{
					BIOP::TAP_t* pTap = pConnBinder->Tap + tap_index;

					sprintf_s(pszField, sizeof(pszField), "BIOP::Tap[%d]()", tap_index);
					XMLElement* pxmlTapNode = pxmlDoc->NewBranchElement(pxmlConnBinderNode, pszField, NULL, 7 + pTap->selector_length);

					pxmlDoc->NewElementForBits(pxmlTapNode, "id", pTap->id, 16, "uimsbf", NULL);

					MPEG2_DSMCC_NumericCoding2Text_tapUse(pTap->use, pszComment, sizeof(pszComment));
					pxmlDoc->NewElementForBits(pxmlTapNode, "use", pTap->use, 16, "uimsbf", pszComment);

					pxmlDoc->NewElementForBits(pxmlTapNode, "association_tag", pTap->association_tag, 16, "uimsbf", NULL);

					pxmlDoc->NewElementForBits(pxmlTapNode, "selector_length", pTap->selector_length, 8, "uimsbf", NULL);

					if (tap_index == 0)
					{
						pxmlDoc->NewElementForBits(pxmlTapNode, "selector_type", pTap->selector_type, 16, "uimsbf", NULL);

						uint16_t dii_table_id_extension = pTap->transactionId & 0x0000ffff;
						sprintf_s(pszComment, sizeof(pszComment), "* indicates dii_table_id_extension=0x%04x", dii_table_id_extension);
						pxmlDoc->NewElementForBits(pxmlTapNode, "transactionId", pTap->transactionId, 32, "uimsbf", pszComment);

						pxmlDoc->NewElementForBits(pxmlTapNode, "timeout", pTap->timeout, 32, "uimsbf", NULL);
					}
					else
					{
						assert(0);
						pxmlDoc->NewElementForByteBuf(pxmlTapNode, "selector_data_byte[]", NULL, pTap->selector_length, NULL);
					}
				}
			}

			//liteComponent 2 ~ (liteComponents_count - 1)
			for (int component_index = 0; component_index < pBIOPProfileBody->liteComponents_count - 2; component_index++)
			{
				BIOP::LiteComponent_t* pliteComponent = pBIOPProfileBody->liteComponents + component_index;

				sprintf_s(pszField, sizeof(pszField), "liteComponent[%d](tag:0x%08X)", component_index, pliteComponent->componentId_tag);
				XMLElement* pxmlComponentNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, 5 + pliteComponent->component_data_length);

				pxmlDoc->NewElementForBits(pxmlComponentNode, "componentId_tag", pliteComponent->componentId_tag, 32, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlComponentNode, "component_data_length", pliteComponent->component_data_length, 8, "uimsbf", NULL);
				if (pliteComponent->component_data_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlComponentNode, "component_data_byte[ ]", pliteComponent->component_data_byte, pliteComponent->component_data_length, NULL);
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

int	MPEG2_DSMCC_BIOP_DecodeLiteOptionsProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::LiteOptionsProfileBody_t* pProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	BIOP::LiteOptionsProfileBody_t* pLiteOptionsProfileBody = (pProfileBody != NULL) ? pProfileBody : new BIOP::LiteOptionsProfileBody_t;
	rtcode = MPEG2_DSMCC_BIOP_DecodeLiteOptionsProfileBody(profileId_tag, profile_data_length, profile_data_byte, pLiteOptionsProfileBody);
	rtcode = MPEG2_DSMCC_BIOP_PresentLiteOptionsProfileBody_to_xml(pxmlDoc, pxmlParentNode, pLiteOptionsProfileBody);

	if (pProfileBody == NULL)
	{
		//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
		delete pLiteOptionsProfileBody;
	}

	return rtcode;
}

int	MPEG2_DSMCC_BIOP_PresentLiteOptionsProfileBody_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::LiteOptionsProfileBody_t* pLiteOptionsProfileBody)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	char	pszField[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pLiteOptionsProfileBody != NULL))
	{
		pxmlDoc->NewElementForBits(pxmlParentNode, "profileId_tag", pLiteOptionsProfileBody->profileId_tag, 32, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlParentNode, "profile_data_length", pLiteOptionsProfileBody->profile_data_length, 32, "uimsbf", "N3");
		pxmlDoc->NewElementForBits(pxmlParentNode, "profile_data_byte_order", pLiteOptionsProfileBody->profile_data_byte_order, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlParentNode, "liteComponents_count", pLiteOptionsProfileBody->liteComponents_count, 8, "uimsbf", NULL);

		if (pLiteOptionsProfileBody->liteComponents_count > 0)		//大于等于2
		{
			for (int component_index = 0; component_index < pLiteOptionsProfileBody->liteComponents_count - 2; component_index++)
			{
				BIOP::LiteComponent_t* pliteComponent = pLiteOptionsProfileBody->liteComponents + component_index;

				sprintf_s(pszField, sizeof(pszField), "liteComponent[%d](tag:0x%08X)", component_index, pliteComponent->componentId_tag);
				XMLElement* pxmlComponentNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, 5 + pliteComponent->component_data_length);

				pxmlDoc->NewElementForBits(pxmlComponentNode, "componentId_tag", pliteComponent->componentId_tag, 32, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlComponentNode, "component_data_length", pliteComponent->component_data_length, 8, "uimsbf", NULL);
				if (pliteComponent->component_data_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlComponentNode, "component_data_byte[ ]", pliteComponent->component_data_byte, pliteComponent->component_data_length, NULL);
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

int	MPEG2_DSMCC_BIOP_PresentDirectMessage_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::DirectoryMessage_t* pDirectoryMessage)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	char pszField[64];
	char pszComment[64];

	if (strcmp(pDirectoryMessage->objectKind_data, "srg") == 0)
	{
		sprintf_s(pszField, sizeof(pszField), "BIOP::ServiceGatewayMessage (objectKey_data_byte=0x%08X)", pDirectoryMessage->objectKey_data);
	}
	else if (strcmp(pDirectoryMessage->objectKind_data, "dir") == 0)
	{
		sprintf_s(pszField, sizeof(pszField), "BIOP::DirectoryMessage (objectKey_data_byte=0x%08X)", pDirectoryMessage->objectKey_data);
	}
	else
	{
		//								assert(0);
	}

	XMLElement* hDDBItem = pxmlParentNode;
	XMLElement* hSrgItem = pxmlDoc->NewBranchElement(hDDBItem, pszField, NULL);

	pxmlDoc->NewElementForByteBuf(hSrgItem, "magic[ ]", (uint8_t*)pDirectoryMessage->magic, 4, pDirectoryMessage->magic);
	pxmlDoc->NewElementForBits(hSrgItem, "biop_version.major", pDirectoryMessage->biop_version.major, 8, "uimsbf", NULL);
	pxmlDoc->NewElementForBits(hSrgItem, "biop_version.minor", pDirectoryMessage->biop_version.minor, 8, "uimsbf", NULL);

	MPEG2_DSMCC_NumericCoding2Text_byteOrder(pDirectoryMessage->byte_order, pszComment, sizeof(pszComment));
	pxmlDoc->NewElementForBits(hSrgItem, "byte_order", pDirectoryMessage->byte_order, 8, "uimsbf", pszComment);
	pxmlDoc->NewElementForBits(hSrgItem, "message_type", pDirectoryMessage->message_type, 8, "uimsbf", NULL);
	pxmlDoc->NewElementForBits(hSrgItem, "message_size", pDirectoryMessage->message_size, 32, "uimsbf", NULL);
	pxmlDoc->NewElementForBits(hSrgItem, "objectKey_length", pDirectoryMessage->objectKey_length, 8, "uimsbf", NULL);
	pxmlDoc->NewElementForBits(hSrgItem, "objectKey_data_byte", pDirectoryMessage->objectKey_data, 32, "uimsbf", NULL);
	pxmlDoc->NewElementForBits(hSrgItem, "objectKind_length", pDirectoryMessage->objectKind_length, 8, "uimsbf", NULL);
	pxmlDoc->NewElementForByteBuf(hSrgItem, "objectKind_data[ ]", (uint8_t*)pDirectoryMessage->objectKind_data, 4, pDirectoryMessage->objectKind_data);
	pxmlDoc->NewElementForBits(hSrgItem, "objectInfo_length", pDirectoryMessage->objectInfo_length, 16, "uimsbf", NULL);
	if (pDirectoryMessage->objectInfo_length > 0)
	{
		pxmlDoc->NewElementForByteBuf(hSrgItem, "objectInfo_data_byte[ ]", pDirectoryMessage->objectInfo_data_byte, pDirectoryMessage->objectInfo_length, NULL);
	}

	pxmlDoc->NewElementForBits(hSrgItem, "serviceContextList_count", pDirectoryMessage->serviceContextList_count, 8, "uimsbf", NULL);
	if (pDirectoryMessage->serviceContextList_count > 0)
	{
		assert(0);
	}

	pxmlDoc->NewElementForBits(hSrgItem, "messageBody_length", pDirectoryMessage->messageBody_length, 32, "uimsbf", NULL);
	pxmlDoc->NewElementForBits(hSrgItem, "bindings_count", pDirectoryMessage->bindings_count, 16, "uimsbf", NULL);

	if (pDirectoryMessage->bindings_count > 0)
	{
		for (int binding_index = 0; binding_index < pDirectoryMessage->bindings_count; binding_index++)
		{
			BIOP::Binding_t* pBindings = pDirectoryMessage->bindings + binding_index;

			if (pBindings->Name.nameComponents_count > 0)
			{
				sprintf_s(pszField, sizeof(pszField), "binding (%d)----%s:%s", binding_index, pBindings->Name.nameComponents[0].kind_data_byte, pBindings->Name.nameComponents[0].id_data_byte);
			}
			else
			{
				sprintf_s(pszField, sizeof(pszField), "binding (%d)", binding_index);
			}
			XMLElement* hBindingItem = pxmlDoc->NewBranchElement(hSrgItem, pszField, NULL);

			sprintf_s(pszField, sizeof(pszField), "BIOP::Name()");
			XMLElement* hBIOPNameItem = pxmlDoc->NewBranchElement(hBindingItem, pszField, NULL);

			pxmlDoc->NewElementForBits(hBIOPNameItem, "nameComponents_count", pBindings->Name.nameComponents_count, 8, "uimsbf", NULL);

			for (int component_index = 0; component_index < pBindings->Name.nameComponents_count; component_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "nameComponent[%d]()", component_index);
				XMLElement* hNameComponentItem = pxmlDoc->NewBranchElement(hBIOPNameItem, pszField, NULL);

				pxmlDoc->NewElementForBits(hNameComponentItem, "id_length", pBindings->Name.nameComponents[component_index].id_length, 8, "uimsbf", NULL);
				if (pBindings->Name.nameComponents[component_index].id_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(hNameComponentItem, "id_data_byte[]", (uint8_t*)pBindings->Name.nameComponents[component_index].id_data_byte, pBindings->Name.nameComponents[component_index].id_length, pBindings->Name.nameComponents[component_index].id_data_byte);
				}

				pxmlDoc->NewElementForBits(hNameComponentItem, "kind_length", pBindings->Name.nameComponents[component_index].kind_length, 8, "uimsbf", NULL);
				if (pBindings->Name.nameComponents[component_index].kind_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(hNameComponentItem, "kind_data_byte[ ]", (uint8_t*)(pBindings->Name.nameComponents[component_index].kind_data_byte), pBindings->Name.nameComponents[component_index].kind_length, pBindings->Name.nameComponents[component_index].kind_data_byte);
				}
			}

			MPEG2_DSMCC_NumericCoding2Text_bindingType(pBindings->bindingType, pszComment, sizeof(pszComment));
			pxmlDoc->NewElementForBits(hBindingItem, "bindingType", pBindings->bindingType, 8, "uimsbf", pszComment);

			MPEG2_DSMCC_BIOP_PresentIOR_to_xml(pxmlDoc, hBindingItem, &(pBindings->IOR));

			pxmlDoc->NewElementForBits(hBindingItem, "objectInfo_length", pBindings->objectInfo_length, 16, "uimsbf", NULL);
			if (pBindings->objectInfo_length > 0)
			{
				pxmlDoc->NewElementForByteBuf(hBindingItem, "objectInfo_data_byte[ ]", pBindings->objectInfo_data_byte, pBindings->objectInfo_length, NULL);
			}
		}
	}

	return rtcode;
}

int	MPEG2_DSMCC_BIOP_PresentFileMessage_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::FileMessage_t* pFileMessage)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	char pszField[64];
	char pszComment[64];

	if (pFileMessage != NULL)
	{
		XMLElement* hDDBItem = pxmlParentNode;

		sprintf_s(pszField, sizeof(pszField), "BIOP::FileMessage (objectKey_data_byte=0x%08X)", pFileMessage->objectKey_data);
		XMLElement* hFileItem = pxmlDoc->NewBranchElement(hDDBItem, pszField, NULL);

		pxmlDoc->NewElementForByteBuf(hFileItem, "magic", (uint8_t*)(pFileMessage->magic), 4, pFileMessage->magic);
		pxmlDoc->NewElementForBits(hFileItem, "biop_version.major", pFileMessage->biop_version.major, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "biop_version.minor", pFileMessage->biop_version.minor, 8, "uimsbf", NULL);

		MPEG2_DSMCC_NumericCoding2Text_byteOrder(pFileMessage->byte_order, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(hFileItem, "byte_order", pFileMessage->byte_order, 8, "uimsbf", pszComment);

		pxmlDoc->NewElementForBits(hFileItem, "message_type", pFileMessage->message_type, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "message_size", pFileMessage->message_size, 32, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "objectKey_length", pFileMessage->objectKey_length, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "objectKey_data_byte", pFileMessage->objectKey_data, 32, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "objectKind_length", pFileMessage->objectKind_length, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForByteBuf(hFileItem, "objectKind_data[ ]", (uint8_t*)pFileMessage->objectKind_data, pFileMessage->objectKind_length, pFileMessage->objectKind_data);
		pxmlDoc->NewElementForBits(hFileItem, "objectInfo_length", pFileMessage->objectInfo_length, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(hFileItem, "serviceContextList_count", pFileMessage->serviceContextList_count, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "messageBody_length", pFileMessage->messageBody_length, 32, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(hFileItem, "content_length", pFileMessage->content_length, 32, "uimsbf", NULL);
		pxmlDoc->NewElementForByteBuf(hFileItem, "content_data_byte[ ]", pFileMessage->content_data_byte, pFileMessage->content_length, "实际文件内容");
	}

	return rtcode;
}



