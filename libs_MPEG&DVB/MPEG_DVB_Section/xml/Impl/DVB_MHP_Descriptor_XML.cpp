#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "HAL/HAL_BitStream/Include/HALForBitStream.h"

#include "../../Include/DVB_MHP_Descriptor.h"
#include "../../Include/DVB_MHP_Utilities.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/DVB_MHP_Descriptor_XML.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//标签：tag = 0x00，defined in TS 101 812
//功能：解应用描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2018.7 chendelin
_CDL_EXPORT int DVB_MHP_decode_application_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, application_descriptor_t* pApplicationDescriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	char     pszTemp[128];
	uint8_t* ptemp;
	BITS_t   bs;
	BITS_t   bs_profiles;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "application_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		application_descriptor_t* papplication_descriptor = (pApplicationDescriptor == NULL) ? new application_descriptor_t : pApplicationDescriptor;
		memset(papplication_descriptor, 0x00, sizeof(application_descriptor_t));

		BITS_map(&bs, buf, length);

		papplication_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", papplication_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		papplication_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", papplication_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		papplication_descriptor->application_profiles_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "application_profiles_length", papplication_descriptor->application_profiles_length, 8, "uimsbf", NULL, &bs);

		if (papplication_descriptor->application_profiles_length > 0)
		{
			ptemp = bs.p_cur;
			BITS_byteSkip(&bs, papplication_descriptor->application_profiles_length);
			tinyxml2::XMLElement* pxmlProfilesNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "application_profiles()", -1, -1, NULL, NULL, &bs);

			BITS_map(&bs_profiles, ptemp, papplication_descriptor->application_profiles_length);

			int loop_length = papplication_descriptor->application_profiles_length;
			int N = 0;
			while ((loop_length >= 5) & (N < MAX_APPLICATION_PROFILES))
			{
				uint8_t* old_ptr = bs_profiles.p_cur;

				sprintf_s(pszTemp, sizeof(pszTemp), "application_profile: %d", N);
				tinyxml2::XMLElement* pxmlProfileNode = pxmlDoc->NewKeyValuePairElement(pxmlProfilesNode, pszTemp);

				papplication_descriptor->application_profile[N] = BITS_get(&bs_profiles, 16);
				pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "application_profile", papplication_descriptor->application_profile[N], 16, "uimsbf", NULL, &bs_profiles);

				papplication_descriptor->version_major[N] = BITS_get(&bs_profiles, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "version_major", papplication_descriptor->version_major[N], 8, "uimsbf", NULL, &bs_profiles);

				papplication_descriptor->version_minor[N] = BITS_get(&bs_profiles, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "version_minor", papplication_descriptor->version_minor[N], 8, "uimsbf", NULL, &bs_profiles);

				papplication_descriptor->version_micro[N] = BITS_get(&bs_profiles, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "version_micro", papplication_descriptor->version_micro[N], 8, "uimsbf", NULL, &bs_profiles);

				sprintf_s(pszTemp, "application_profile=0x%04x, version=%d.%d.%d", papplication_descriptor->application_profile[N], papplication_descriptor->version_major[N], papplication_descriptor->version_minor[N], papplication_descriptor->version_micro[N]);
				pxmlDoc->UpdateBufMark(pxmlProfileNode, old_ptr, bs_profiles.p_cur);
				pxmlProfileNode->SetAttribute("comment", pszTemp);

				loop_length -= (int)(bs_profiles.p_cur - old_ptr);
				N++;
			}
			papplication_descriptor->application_profiles_N = N;
		}

		papplication_descriptor->service_bound_flag = BITS_get(&bs, 1);
		DVB_MHP_NumericCoding2Text_ApplicationServiceBoundFlag(papplication_descriptor->service_bound_flag, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_bound_flag", papplication_descriptor->service_bound_flag, 1, "bslbf", pszTemp, &bs);

		papplication_descriptor->visibility = BITS_get(&bs, 2);
		DVB_MHP_NumericCoding2Text_ApplicationVisibility(papplication_descriptor->visibility, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "visibility", papplication_descriptor->visibility, 2, "bslbf", pszTemp, &bs);

		papplication_descriptor->reserved_future_use = BITS_get(&bs, 5);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved_future_use", papplication_descriptor->reserved_future_use, 5, "bslbf", NULL, &bs);

		papplication_descriptor->application_priority = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "application_priority", papplication_descriptor->application_priority, 8, "uimsbf", NULL, &bs);

		papplication_descriptor->transport_protocols_N = papplication_descriptor->descriptor_length - papplication_descriptor->application_profiles_length - 3;

		if (papplication_descriptor->transport_protocols_N > 0)
		{
			int copy_length = min(papplication_descriptor->transport_protocols_N, MAX_APPLICATION_TRANSPORT_PROTOCOLS);
			memcpy(papplication_descriptor->transport_protocol_lable, bs.p_cur, copy_length);
			BITS_byteSkip(&bs, papplication_descriptor->transport_protocols_N);

			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "transport_protocol_lable", papplication_descriptor->transport_protocol_lable, copy_length, NULL, &bs);
		}

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", papplication_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

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


//标签：tag = 0x01，defined in TS 101 812
//功能：解析应用名称描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_application_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, application_name_descriptor_t* pApplicationNameDescriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	int			N;
	char     pszTemp[64];
	uint8_t* ptemp;
	BITS_t   bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "application_name_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		application_name_descriptor_t* papplication_name_descriptor = (pApplicationNameDescriptor == NULL) ? new application_name_descriptor_t : pApplicationNameDescriptor;
		memset(papplication_name_descriptor, 0x00, sizeof(application_name_descriptor_t));

		BITS_map(&bs, buf, length);

		papplication_name_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", papplication_name_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		papplication_name_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", papplication_name_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		if (papplication_name_descriptor->descriptor_length > 0)
		{
			int loop_length = papplication_name_descriptor->descriptor_length;
			N = 0;
			while ((loop_length >= 4) && (N < MAX_LANGUAGES))
			{
				uint8_t* old_ptr = bs.p_cur;

				sprintf_s(pszTemp, sizeof(pszTemp), "application_name(%d)", N);
				tinyxml2::XMLElement* pxmlLanguageNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

				memcpy(papplication_name_descriptor->ISO_639_language_char[N], bs.p_cur, 3);
				papplication_name_descriptor->ISO_639_language_char[N][3] = '\0';
				papplication_name_descriptor->ISO_639_language_code[N] = BITS_get(&bs, 24);
				pxmlDoc->NewKeyValuePairElement(pxmlLanguageNode, "ISO_639_language_code", papplication_name_descriptor->ISO_639_language_code[N], 24, "bslbf",
					papplication_name_descriptor->ISO_639_language_char[N], &bs);

				papplication_name_descriptor->application_name_length[N] = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlLanguageNode, "application_name_length", papplication_name_descriptor->application_name_length[N], 8, "uimsbf", NULL, &bs);

				if (papplication_name_descriptor->application_name_length[N] > 0)
				{
					ptemp = bs.p_cur;
					int copy_length = min(papplication_name_descriptor->application_name_length[N], 63);
					memcpy(papplication_name_descriptor->application_name_char[N], ptemp, copy_length);
					papplication_name_descriptor->application_name_char[N][copy_length] = '\0';
					BITS_byteSkip(&bs, papplication_name_descriptor->application_name_length[N]);
					pxmlDoc->NewKeyValuePairElement(pxmlLanguageNode, "application_name_char", (uint8_t*)papplication_name_descriptor->application_name_char[N],
						papplication_name_descriptor->application_name_length[N], 
						papplication_name_descriptor->application_name_char[N], &bs);

					pxmlLanguageNode->SetAttribute("comment", papplication_name_descriptor->application_name_char[N]);
				}

				pxmlDoc->UpdateBufMark(pxmlLanguageNode, old_ptr, bs.p_cur);

				loop_length -= (int)(bs.p_cur - old_ptr);
				N++;
			}

			papplication_name_descriptor->N = N;
		}

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", papplication_name_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

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


//标签：tag = 0x02，defined in TS 101 812
//功能：解析传输协议描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_transport_protocol_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, transport_protocol_descriptor_t* pTransportProtocolDescriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	char     pszTemp[64];
	BITS_t   bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 5))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "transport_protocol_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		transport_protocol_descriptor_t* ptransport_protocol_descriptor = (pTransportProtocolDescriptor == NULL) ? new transport_protocol_descriptor_t : pTransportProtocolDescriptor;
		memset(ptransport_protocol_descriptor, 0x00, sizeof(transport_protocol_descriptor_t));

		BITS_map(&bs, buf, length);

		ptransport_protocol_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", ptransport_protocol_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		ptransport_protocol_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", ptransport_protocol_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		ptransport_protocol_descriptor->protocol_id = BITS_get(&bs, 16);
		DVB_MHP_NumericCoding2Text_ApplicationTransportProtocolID(ptransport_protocol_descriptor->protocol_id, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "protocol_id", ptransport_protocol_descriptor->protocol_id, 16, "uimsbf", pszTemp, &bs);

		ptransport_protocol_descriptor->transport_protocol_label = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "transport_protocol_label", ptransport_protocol_descriptor->transport_protocol_label, 8, "uimsbf", NULL, &bs);

		ptransport_protocol_descriptor->N = ptransport_protocol_descriptor->descriptor_length - 3;

		if (ptransport_protocol_descriptor->N > 0)
		{
			int copy_length = min(ptransport_protocol_descriptor->N, sizeof(ptransport_protocol_descriptor->selector_byte));
			memcpy(ptransport_protocol_descriptor->selector_byte, bs.p_cur, copy_length);
			BITS_byteSkip(&bs, ptransport_protocol_descriptor->N);

			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_byte[ ]", ptransport_protocol_descriptor->selector_byte, copy_length, NULL, &bs);
		}

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", ptransport_protocol_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

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

//标签：tag = 0x04，defined in TS 101 812
//功能：解析DVB-J应用位置描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_dvb_j_application_location_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, dvb_j_application_location_descriptor_t* pApplicationLocationDescriptor)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;
	char     pszTemp[64];
	BITS_t   bs;
	tinyxml2::XMLElement* pxmlTempNode;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 4))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "dvb_j_application_location_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor = (pApplicationLocationDescriptor == NULL) ? new dvb_j_application_location_descriptor_t : pApplicationLocationDescriptor;
		memset(pdvb_j_application_location_descriptor, 0x00, sizeof(dvb_j_application_location_descriptor_t));

		BITS_map(&bs, buf, length);

		pdvb_j_application_location_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pdvb_j_application_location_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		pdvb_j_application_location_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pdvb_j_application_location_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		pdvb_j_application_location_descriptor->base_directory_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "base_directory_length", pdvb_j_application_location_descriptor->base_directory_length, 8, "uimsbf", NULL, &bs);

		int copy_length = min(127, pdvb_j_application_location_descriptor->base_directory_length);
		if (copy_length > 0)
		{
			memcpy(pdvb_j_application_location_descriptor->base_directory_byte, bs.p_cur, copy_length);
			pdvb_j_application_location_descriptor->base_directory_byte[copy_length] = '\0';
			BITS_byteSkip(&bs, pdvb_j_application_location_descriptor->base_directory_length);

			pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "base_directory_byte", (uint8_t*)pdvb_j_application_location_descriptor->base_directory_byte,
				pdvb_j_application_location_descriptor->base_directory_length, pdvb_j_application_location_descriptor->base_directory_byte, &bs);
			pxmlDoc->UpdateBufMark(pxmlTempNode, bs.p_old, bs.p_cur);
		}

		pdvb_j_application_location_descriptor->classpath_extension_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "classpath_extension_length", pdvb_j_application_location_descriptor->classpath_extension_length, 8, "uimsbf", NULL, &bs);

		copy_length = min(127, pdvb_j_application_location_descriptor->classpath_extension_length);
		if (copy_length > 0)
		{
			memcpy(pdvb_j_application_location_descriptor->classpath_extension_byte, bs.p_cur, copy_length);
			pdvb_j_application_location_descriptor->classpath_extension_byte[copy_length] = '\0';
			BITS_byteSkip(&bs, pdvb_j_application_location_descriptor->base_directory_length);
			pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "classpath_extension_byte", (uint8_t*)pdvb_j_application_location_descriptor->classpath_extension_byte,
				pdvb_j_application_location_descriptor->classpath_extension_length, pdvb_j_application_location_descriptor->classpath_extension_byte, &bs);
			pxmlDoc->UpdateBufMark(pxmlTempNode, bs.p_old, bs.p_cur);
		}

		int N = pdvb_j_application_location_descriptor->descriptor_length - pdvb_j_application_location_descriptor->base_directory_length - pdvb_j_application_location_descriptor->classpath_extension_length - 2;
		pdvb_j_application_location_descriptor->N = N;

		copy_length = min(127, N);
		if (copy_length > 0)
		{
			memcpy(pdvb_j_application_location_descriptor->initial_class_byte, bs.p_cur, copy_length);
			pdvb_j_application_location_descriptor->initial_class_byte[copy_length] = '\0';
			BITS_byteSkip(&bs, pdvb_j_application_location_descriptor->N);
			pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "initial_class_byte", (uint8_t*)pdvb_j_application_location_descriptor->initial_class_byte,
				pdvb_j_application_location_descriptor->N, pdvb_j_application_location_descriptor->initial_class_byte, &bs);
			pxmlDoc->UpdateBufMark(pxmlTempNode, bs.p_old, bs.p_cur);
		}

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pdvb_j_application_location_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

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
