#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/DVB_SI_Descriptor.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../Include/DVB_Databroadcast_XML.h"

int DVB_Databroadcast_present_multiprotocol_encapsulation_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiprotocol_encapsulation_info_t* pmultiprotocol_encapsulation_info)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	return rtcode;
}

//
int DVB_Databroadcast_present_data_carousel_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_carousel_info_t* pdata_carousel_info)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pdata_carousel_info != NULL))
	{
		DVB_SI_NumericCoding2Text_CarouselTypeID(pdata_carousel_info->carousel_type_id, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlParentNode, "carousel_type_id", pdata_carousel_info->carousel_type_id, 2, "bslbf", pszComment);

		pxmlDoc->NewElementForBits(pxmlParentNode, "reserved", pdata_carousel_info->reserved0, 6, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "transaction_id", pdata_carousel_info->transaction_id, 32, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "time_out_value_DSI", pdata_carousel_info->time_out_value_DSI, 32, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "time_out_value_DII", pdata_carousel_info->time_out_value_DII, 32, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "reserved", pdata_carousel_info->reserved1, 2, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "leak_rate", pdata_carousel_info->leak_rate, 22, "uimsbf", NULL);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_Databroadcast_present_object_carousel_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, object_carousel_info_t* pobject_carousel_info)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char pszField[48];
	char pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pobject_carousel_info != NULL))
	{
		DVB_SI_NumericCoding2Text_CarouselTypeID(pobject_carousel_info->carousel_type_id, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlParentNode, "carousel_type_id", pobject_carousel_info->carousel_type_id, 2, "bslbf", pszComment);

		pxmlDoc->NewElementForBits(pxmlParentNode, "reserved", pobject_carousel_info->reserved0, 6, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "transaction_id", pobject_carousel_info->transaction_id, 32, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "time_out_value_DSI", pobject_carousel_info->time_out_value_DSI, 32, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "time_out_value_DII", pobject_carousel_info->time_out_value_DII, 32, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "reserved", pobject_carousel_info->reserved1, 2, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlParentNode, "leak_rate", pobject_carousel_info->leak_rate, 22, "uimsbf", NULL);

		for (int obj_index = 0; obj_index < pobject_carousel_info->N; obj_index ++)
		{
			char* pszObjName = NULL;

			if (pobject_carousel_info->st[obj_index].trimmed_object_name_length > 0)
			{
				pszObjName = (char*)malloc(pobject_carousel_info->st[obj_index].trimmed_object_name_length + 1);
				memcpy(pszObjName, pobject_carousel_info->st[obj_index].trimmed_object_name_char, pobject_carousel_info->st[obj_index].trimmed_object_name_length);
				pszObjName[pobject_carousel_info->st[obj_index].trimmed_object_name_length] = '\0';
			}

			sprintf_s(pszField, sizeof(pszField), "object[%d]()", obj_index);
			XMLElement* pxmlObjectNameNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszObjName, 3 + pobject_carousel_info->st[obj_index].object_name_length);

			pxmlDoc->NewElementForByteBuf(pxmlObjectNameNode, "ISO_639_language_code[]", 
				(uint8_t*)pobject_carousel_info->st[obj_index].ISO_639_language_code_char,
				3, pobject_carousel_info->st[obj_index].ISO_639_language_code_char);

			pxmlDoc->NewElementForBits(pxmlObjectNameNode, "object_name_length", pobject_carousel_info->st[obj_index].object_name_length, 8, "uimsbf", NULL);

			if (pobject_carousel_info->st[obj_index].object_name_length > 0)
			{
				pxmlDoc->NewElementForByteBuf(pxmlObjectNameNode, "object_name_char[ ]", 
					pobject_carousel_info->st[obj_index].object_name_char, 
					pobject_carousel_info->st[obj_index].object_name_length,
					pszObjName);
			}

			if (pszObjName != NULL) free(pszObjName);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_Databroadcast_present_IP_MAC_notification_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IP_MAC_notification_info_t* pIP_MAC_notification_info)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	return rtcode;
}
