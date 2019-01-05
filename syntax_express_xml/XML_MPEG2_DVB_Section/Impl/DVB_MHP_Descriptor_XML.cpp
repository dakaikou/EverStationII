#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_MHP_Descriptor.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_MHP_Utilities.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "../Include/DVB_MHP_Descriptor_XML.h"

//标签：tag = 0x00，defined in TS 101 812
//功能：解应用描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2018.7 chendelin
XML_MPEG_DVB_LIB int DVB_MHP_decode_application_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_descriptor_t* pApplicationDescriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	{
		application_descriptor_t* papplication_descriptor = (pApplicationDescriptor == NULL) ? new application_descriptor_t : pApplicationDescriptor;

		rtcode = DVB_MHP_decode_application_descriptor(buf, length, papplication_descriptor);
		DVB_MHP_present_application_descriptor_to_xml(pxmlDoc, pxmlParentNode, papplication_descriptor);

		if (pApplicationDescriptor == NULL)
		{
			delete papplication_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

XML_MPEG_DVB_LIB int DVB_MHP_present_application_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_descriptor_t* papplication_descriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	//char     pszTemp[128];
	//uint8_t* ptemp;
	//BITS_t   bs;
	//BITS_t   bs_profiles;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "application_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	application_descriptor_t* papplication_descriptor = (pApplicationDescriptor == NULL) ? new application_descriptor_t : pApplicationDescriptor;
	//	memset(papplication_descriptor, 0x00, sizeof(application_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	papplication_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", papplication_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	papplication_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", papplication_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	papplication_descriptor->application_profiles_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "application_profiles_length", papplication_descriptor->application_profiles_length, 8, "uimsbf", NULL, &bs);

	//	if (papplication_descriptor->application_profiles_length > 0)
	//	{
	//		ptemp = bs.p_cur;
	//		BITS_byteSkip(&bs, papplication_descriptor->application_profiles_length);
	//		tinyxml2::XMLElement* pxmlProfilesNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "application_profiles()", -1, -1, NULL, NULL, &bs);

	//		BITS_map(&bs_profiles, ptemp, papplication_descriptor->application_profiles_length);

	//		int loop_length = papplication_descriptor->application_profiles_length;
	//		int N = 0;
	//		while ((loop_length >= 5) & (N < MAX_APPLICATION_PROFILES))
	//		{
	//			uint8_t* old_ptr = bs_profiles.p_cur;

	//			sprintf_s(pszTemp, sizeof(pszTemp), "application_profile: %d", N);
	//			tinyxml2::XMLElement* pxmlProfileNode = pxmlDoc->NewKeyValuePairElement(pxmlProfilesNode, pszTemp);

	//			papplication_descriptor->application_profile[N] = BITS_get(&bs_profiles, 16);
	//			pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "application_profile", papplication_descriptor->application_profile[N], 16, "uimsbf", NULL, &bs_profiles);

	//			papplication_descriptor->version_major[N] = BITS_get(&bs_profiles, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "version_major", papplication_descriptor->version_major[N], 8, "uimsbf", NULL, &bs_profiles);

	//			papplication_descriptor->version_minor[N] = BITS_get(&bs_profiles, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "version_minor", papplication_descriptor->version_minor[N], 8, "uimsbf", NULL, &bs_profiles);

	//			papplication_descriptor->version_micro[N] = BITS_get(&bs_profiles, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "version_micro", papplication_descriptor->version_micro[N], 8, "uimsbf", NULL, &bs_profiles);

	//			sprintf_s(pszTemp, "application_profile=0x%04x, version=%d.%d.%d", papplication_descriptor->application_profile[N], papplication_descriptor->version_major[N], papplication_descriptor->version_minor[N], papplication_descriptor->version_micro[N]);
	//			pxmlDoc->UpdateBufMark(pxmlProfileNode, old_ptr, bs_profiles.p_cur);
	//			pxmlProfileNode->SetAttribute("comment", pszTemp);

	//			loop_length -= (int)(bs_profiles.p_cur - old_ptr);
	//			N++;
	//		}
	//		papplication_descriptor->application_profiles_N = N;
	//	}

	//	papplication_descriptor->service_bound_flag = BITS_get(&bs, 1);
	//	DVB_MHP_NumericCoding2Text_ApplicationServiceBoundFlag(papplication_descriptor->service_bound_flag, pszTemp, sizeof(pszTemp));
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_bound_flag", papplication_descriptor->service_bound_flag, 1, "bslbf", pszTemp, &bs);

	//	papplication_descriptor->visibility = BITS_get(&bs, 2);
	//	DVB_MHP_NumericCoding2Text_ApplicationVisibility(papplication_descriptor->visibility, pszTemp, sizeof(pszTemp));
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "visibility", papplication_descriptor->visibility, 2, "bslbf", pszTemp, &bs);

	//	papplication_descriptor->reserved_future_use = BITS_get(&bs, 5);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved_future_use", papplication_descriptor->reserved_future_use, 5, "bslbf", NULL, &bs);

	//	papplication_descriptor->application_priority = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "application_priority", papplication_descriptor->application_priority, 8, "uimsbf", NULL, &bs);

	//	papplication_descriptor->transport_protocols_N = papplication_descriptor->descriptor_length - papplication_descriptor->application_profiles_length - 3;

	//	if (papplication_descriptor->transport_protocols_N > 0)
	//	{
	//		int copy_length = min(papplication_descriptor->transport_protocols_N, MAX_APPLICATION_TRANSPORT_PROTOCOLS);
	//		memcpy(papplication_descriptor->transport_protocol_lable, bs.p_cur, copy_length);
	//		BITS_byteSkip(&bs, papplication_descriptor->transport_protocols_N);

	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "transport_protocol_lable", papplication_descriptor->transport_protocol_lable, copy_length, NULL, &bs);
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", papplication_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pApplicationDescriptor == NULL)
	//	{
	//		delete papplication_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//标签：tag = 0x01，defined in TS 101 812
//功能：解析应用名称描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_application_name_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_name_descriptor_t* pApplicationNameDescriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		application_name_descriptor_t* papplication_name_descriptor = (pApplicationNameDescriptor == NULL) ? new application_name_descriptor_t : pApplicationNameDescriptor;

		rtcode = DVB_MHP_decode_application_name_descriptor(buf, length, papplication_name_descriptor);
		DVB_MHP_present_application_name_descriptor_to_xml(pxmlDoc, pxmlParentNode, papplication_name_descriptor);

		if (pApplicationNameDescriptor == NULL)
		{
			delete papplication_name_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_MHP_present_application_name_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_name_descriptor_t* papplication_name_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char    pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (papplication_name_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "application_name_descriptor(tag: 0x%02X)", papplication_name_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, papplication_name_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", papplication_name_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", papplication_name_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (papplication_name_descriptor->descriptor_length > 0)
		{
			for (int lang_index = 0; lang_index < papplication_name_descriptor->N; lang_index ++)
			{
				char* pszAppName = NULL;

				int trimmed_application_name_length = papplication_name_descriptor->st[lang_index].trimmed_application_name_length;
				if (trimmed_application_name_length > 0)
				{
					pszAppName = (char*)malloc(trimmed_application_name_length + 1);
					memcpy(pszAppName, papplication_name_descriptor->st[lang_index].trimmed_application_name_char, trimmed_application_name_length);
					pszAppName[trimmed_application_name_length] = '\0';
				}

				sprintf_s(pszField, sizeof(pszField), "application_name[%d]()", lang_index);
				XMLElement* pxmlLanguageNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, pszAppName, 4 + papplication_name_descriptor->st[lang_index].application_name_length);

				pxmlDoc->NewElementForByteBuf(pxmlLanguageNode, "ISO_639_language_code", 
					(uint8_t*)papplication_name_descriptor->st[lang_index].ISO_639_language_char, 3,
					papplication_name_descriptor->st[lang_index].ISO_639_language_char);

				pxmlDoc->NewElementForBits(pxmlLanguageNode, "application_name_length", papplication_name_descriptor->st[lang_index].application_name_length, 8, "uimsbf", NULL);

				if (papplication_name_descriptor->st[lang_index].application_name_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlLanguageNode, "application_name_char[ ]", 
						papplication_name_descriptor->st[lang_index].application_name_char,
						papplication_name_descriptor->st[lang_index].application_name_length,
						pszAppName);
				}

				if (pszAppName != NULL) free(pszAppName);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//标签：tag = 0x02，defined in TS 101 812
//功能：解析传输协议描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_transport_protocol_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, transport_protocol_descriptor_t* pTransportProtocolDescriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 5))
	{
		transport_protocol_descriptor_t* ptransport_protocol_descriptor = (pTransportProtocolDescriptor == NULL) ? new transport_protocol_descriptor_t : pTransportProtocolDescriptor;

		rtcode = DVB_MHP_decode_transport_protocol_descriptor(buf, length, ptransport_protocol_descriptor);
		DVB_MHP_present_transport_protocol_descriptor_to_xml(pxmlDoc, pxmlParentNode, ptransport_protocol_descriptor);

		if (pTransportProtocolDescriptor == NULL)
		{
			delete ptransport_protocol_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_MHP_present_transport_protocol_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, transport_protocol_descriptor_t* ptransport_protocol_descriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	char     pszField[48];
	char	 pszProtocol[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (ptransport_protocol_descriptor != NULL))
	{
		//assert(0);

		DVB_MHP_Num2Text_ApplicationTransportProtocolID(ptransport_protocol_descriptor->protocol_id, pszProtocol, sizeof(pszProtocol));

		sprintf_s(pszField, sizeof(pszField), "transport_protocol_descriptor(tag: 0x%02X)", ptransport_protocol_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszProtocol, ptransport_protocol_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", ptransport_protocol_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", ptransport_protocol_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "protocol_id", ptransport_protocol_descriptor->protocol_id, 16, "uimsbf", pszProtocol);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "transport_protocol_label", ptransport_protocol_descriptor->transport_protocol_label, 8, "uimsbf", NULL);

		if (ptransport_protocol_descriptor->selector_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "selector_byte[ ]", 
				ptransport_protocol_descriptor->selector_byte, 
				ptransport_protocol_descriptor->selector_length);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//标签：tag = 0x04，defined in TS 101 812
//功能：解析DVB-J应用位置描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_dvb_j_application_location_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, dvb_j_application_location_descriptor_t* pApplicationLocationDescriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 4))
	{
		dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor = (pApplicationLocationDescriptor == NULL) ? new dvb_j_application_location_descriptor_t : pApplicationLocationDescriptor;

		rtcode = DVB_MHP_decode_dvb_j_application_location_descriptor(buf, length, pdvb_j_application_location_descriptor);
		DVB_MHP_present_dvb_j_application_location_descriptor_to_xml(pxmlDoc, pxmlParentNode, pdvb_j_application_location_descriptor);

		if (pApplicationLocationDescriptor == NULL)
		{
			delete pdvb_j_application_location_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_MHP_present_dvb_j_application_location_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	char     pszField[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pdvb_j_application_location_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "dvb_j_application_location_descriptor(tag: 0x%02X)", 
			pdvb_j_application_location_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pdvb_j_application_location_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pdvb_j_application_location_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pdvb_j_application_location_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "base_directory_length", pdvb_j_application_location_descriptor->base_directory_length, 8, "uimsbf", NULL);

		if (pdvb_j_application_location_descriptor->base_directory_length > 0)
		{
			char* pszBaseDir = (char*)malloc(pdvb_j_application_location_descriptor->base_directory_length + 1);
			memcpy(pszBaseDir, pdvb_j_application_location_descriptor->base_directory_byte, pdvb_j_application_location_descriptor->base_directory_length);
			pszBaseDir[pdvb_j_application_location_descriptor->base_directory_length] = '\0';

			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "base_directory_byte[ ]", 
				pdvb_j_application_location_descriptor->base_directory_byte,
				pdvb_j_application_location_descriptor->base_directory_length, 
				pszBaseDir);

			free(pszBaseDir);
		}

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "classpath_extension_length", pdvb_j_application_location_descriptor->classpath_extension_length, 8, "uimsbf", NULL);

		if (pdvb_j_application_location_descriptor->classpath_extension_length > 0)
		{
			char* pszClasssExt = (char*)malloc(pdvb_j_application_location_descriptor->classpath_extension_length + 1);
			memcpy(pszClasssExt, pdvb_j_application_location_descriptor->classpath_extension_byte, pdvb_j_application_location_descriptor->classpath_extension_length);
			pszClasssExt[pdvb_j_application_location_descriptor->classpath_extension_length] = '\0';

			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "classpath_extension_byte[ ]",
				pdvb_j_application_location_descriptor->classpath_extension_byte,
				pdvb_j_application_location_descriptor->classpath_extension_length,
				pszClasssExt);

			free(pszClasssExt);
		}

		if (pdvb_j_application_location_descriptor->initial_class_char_length > 0)
		{
			char* pszIntialClasss = (char*)malloc(pdvb_j_application_location_descriptor->initial_class_char_length + 1);
			memcpy(pszIntialClasss, pdvb_j_application_location_descriptor->initial_class_byte, pdvb_j_application_location_descriptor->initial_class_char_length);
			pszIntialClasss[pdvb_j_application_location_descriptor->initial_class_char_length] = '\0';

			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "classpath_extension_byte[ ]",
				pdvb_j_application_location_descriptor->initial_class_byte,
				pdvb_j_application_location_descriptor->initial_class_char_length,
				pszIntialClasss);

			free(pszIntialClasss);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}
