#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/DVB_SI_Descriptor.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//
int DVB_Databroadcast_decode_data_carousel_info_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_carousel_info_t* pDataCarouselInfo = NULL)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char pszTemp[64];
	BITS_t  bs;

	if ((pxmlDoc != NULL) && (buf != NULL) && (length >= 16))
	{
		//tinyxml2::XMLElement* pxmlParentNode = pxmlDoc->GetActiveNode();

		data_carousel_info_t* pdata_carousel_info = (pDataCarouselInfo == NULL) ? new data_carousel_info_t : pDataCarouselInfo;
		memset(pdata_carousel_info, 0x00, sizeof(data_carousel_info_t));

		BITS_map(&bs, buf, length);

		pdata_carousel_info->carousel_type_id = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "carousel_type_id", pdata_carousel_info->carousel_type_id, 2, "bslbf", NULL, &bs);

		pdata_carousel_info->reserved0 = BITS_get(&bs, 6);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pdata_carousel_info->reserved0, 6, "bslbf", NULL, &bs);

		pdata_carousel_info->transaction_id = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "transaction_id", pdata_carousel_info->transaction_id, 32, "uimsbf", NULL, &bs);

		pdata_carousel_info->time_out_value_DSI = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DSI", pdata_carousel_info->time_out_value_DSI, 32, "uimsbf", NULL, &bs);

		pdata_carousel_info->time_out_value_DII = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DII", pdata_carousel_info->time_out_value_DII, 32, "uimsbf", NULL, &bs);

		pdata_carousel_info->reserved1 = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pdata_carousel_info->reserved1, 2, "bslbf", NULL, &bs);

		pdata_carousel_info->leak_rate = BITS_get(&bs, 22);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "leak_rate", pdata_carousel_info->leak_rate, 22, "uimsbf", NULL, &bs);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", length);
		pxmlParentNode->SetAttribute("comment", pszTemp);

		if (pDataCarouselInfo == NULL)
		{
			delete pdata_carousel_info;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_Databroadcast_decode_object_carousel_info_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, object_carousel_info_t* pObjectCarouselInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	int	N;
	int	copy_length;
	char pszTemp[64];
	BITS_t  bs;


	if ((pxmlDoc != NULL) && (buf != NULL) && (length >= 16))
	{
		object_carousel_info_t* pobject_carousel_info = (pObjectCarouselInfo == NULL) ? new object_carousel_info_t : pObjectCarouselInfo;
		memset(pobject_carousel_info, 0x00, sizeof(object_carousel_info_t));

		BITS_map(&bs, buf, length);

		pobject_carousel_info->carousel_type_id = BITS_get(&bs, 2);
		DVB_SI_NumericCoding2Text_CarouselTypeID(pobject_carousel_info->carousel_type_id, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "carousel_type_id", pobject_carousel_info->carousel_type_id, 2, "bslbf", pszTemp, &bs);

		pobject_carousel_info->reserved0 = BITS_get(&bs, 6);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pobject_carousel_info->reserved0, 6, "bslbf", NULL, &bs);

		pobject_carousel_info->transaction_id = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "transaction_id", pobject_carousel_info->transaction_id, 32, "uimsbf", NULL, &bs);

		pobject_carousel_info->time_out_value_DSI = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DSI", pobject_carousel_info->time_out_value_DSI, 32, "uimsbf", NULL, &bs);

		pobject_carousel_info->time_out_value_DII = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DII", pobject_carousel_info->time_out_value_DII, 32, "uimsbf", NULL, &bs);

		pobject_carousel_info->reserved1 = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pobject_carousel_info->reserved1, 2, "bslbf", NULL, &bs);

		pobject_carousel_info->leak_rate = BITS_get(&bs, 22);
		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "leak_rate", pobject_carousel_info->leak_rate, 22, "uimsbf", NULL, &bs);

		N = 0;
		int object_name_loop_length = (int)(bs.p_end - bs.p_cur);
		if (object_name_loop_length > 0)
		{
			uint8_t* pl1temp = bs.p_cur;
			BITS_byteSkip(&bs, object_name_loop_length);
			tinyxml2::XMLElement* pxmlServiceLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "object_name 循环()", -1, -1, NULL, NULL, &bs);

			BITS_t object_name_loop_bs;
			BITS_map(&object_name_loop_bs, pl1temp, object_name_loop_length);
			while ((object_name_loop_length >= 4) && (N < 16))
			{
				uint8_t* old_ptr = object_name_loop_bs.p_cur;

				sprintf_s(pszTemp, sizeof(pszTemp), "object_name [%d]", N);
				tinyxml2::XMLElement* pxmlObjectNameNode = pxmlDoc->NewKeyValuePairElement(pxmlServiceLoopNode, pszTemp);

				memcpy(pobject_carousel_info->ISO_639_language_code_char[N], object_name_loop_bs.p_cur, 3);
				pobject_carousel_info->ISO_639_language_code_char[N][3] = '\0';
				pobject_carousel_info->ISO_639_language_code[N] = BITS_get(&object_name_loop_bs, 24);
				pxmlDoc->NewKeyValuePairElement(pxmlObjectNameNode, "ISO_639_language_code", pobject_carousel_info->ISO_639_language_code[N],
					24, "bslbf", pobject_carousel_info->ISO_639_language_code_char[N], &object_name_loop_bs);

				pobject_carousel_info->object_name_length[N] = BITS_get(&object_name_loop_bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlObjectNameNode, "object_name_length", pobject_carousel_info->object_name_length[N], 8, "uimsbf", NULL, &object_name_loop_bs);

				if (pobject_carousel_info->object_name_length[N] > 0)
				{
					copy_length = min(pobject_carousel_info->object_name_length[N], 31);
					memcpy(pobject_carousel_info->object_name_char[N], object_name_loop_bs.p_cur, copy_length);
					pobject_carousel_info->object_name_char[N][copy_length] = '\0';
					BITS_byteSkip(&object_name_loop_bs, pobject_carousel_info->object_name_length[N]);
					pxmlDoc->NewKeyValuePairElement(pxmlObjectNameNode, "object_name_char[ ]", (uint8_t*)pobject_carousel_info->object_name_char[N], copy_length, pobject_carousel_info->object_name_char[N], &object_name_loop_bs);
				}
				pxmlDoc->UpdateBufMark(pxmlObjectNameNode, old_ptr, object_name_loop_bs.p_cur);
				object_name_loop_length -= (int)(object_name_loop_bs.p_cur - old_ptr);

				N++;
			}
		}
		pobject_carousel_info->N = N;

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", length);
		pxmlParentNode->SetAttribute("comment", pszTemp);

		if (pObjectCarouselInfo == NULL)
		{
			delete pobject_carousel_info;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

