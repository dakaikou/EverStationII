#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Impl/DVB_SI_Utilities_Inner.h"

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/DVB_SI_Descriptor.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../Include/DVB_Databroadcast_XML.h"
#include "../Include/DVB_SI_Descriptor_XML.h"

//功能：解网络名称描述子			0x40
//输入：buffer, 起始位置nIndex
//返回：解析状态码
int DVB_SI_decode_network_name_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, network_name_descriptor_t* pNetworkNameDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		network_name_descriptor_t* pnetwork_name_descriptor = (pNetworkNameDescriptor == NULL) ? new network_name_descriptor_t : pNetworkNameDescriptor;
		rtcode = DVB_SI_decode_network_name_descriptor(buf, length, pnetwork_name_descriptor);
		DVB_SI_present_network_name_descriptor_to_xml(pxmlDoc, pxmlParentNode, pnetwork_name_descriptor);

		if (pNetworkNameDescriptor == NULL)
		{
			delete pnetwork_name_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_present_network_name_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, network_name_descriptor_t* pnetwork_name_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pnetwork_name_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "network_name_descriptor(tag: 0x%02X)", pnetwork_name_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pnetwork_name_descriptor->trimmed_network_name, pnetwork_name_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pnetwork_name_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pnetwork_name_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pnetwork_name_descriptor->descriptor_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "network_name", (uint8_t*)pnetwork_name_descriptor->network_name, pnetwork_name_descriptor->descriptor_length, pnetwork_name_descriptor->trimmed_network_name);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解服务列表描述子				0x41
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_service_list_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, service_list_descriptor_t* pServiceListDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		service_list_descriptor_t* pservice_list_descriptor = (pServiceListDescriptor == NULL) ? new service_list_descriptor_t : pServiceListDescriptor;
		rtcode = DVB_SI_decode_service_list_descriptor(buf, length, pservice_list_descriptor);
		DVB_SI_present_service_list_descriptor_to_xml(pxmlDoc, pxmlParentNode, pservice_list_descriptor);

		if (pServiceListDescriptor == NULL)
		{
			delete pservice_list_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_service_list_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, service_list_descriptor_t* pservice_list_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char  pszField[64];
	char  pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pservice_list_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "service_list_descriptor(tag: 0x%02X)", pservice_list_descriptor->descriptor_tag);
		sprintf_s(pszComment, sizeof(pszComment), "共 %d个业务", pservice_list_descriptor->N);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pservice_list_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pservice_list_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pservice_list_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pservice_list_descriptor->descriptor_length > 0)
		{
			XMLElement* pxmlListNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, "业务列表()", pszComment, pservice_list_descriptor->descriptor_length);

			for  (int service_index = 0; service_index < pservice_list_descriptor->N; service_index ++)
			{
				DVB_SI_NumericCoding2Text_ServiceType(pservice_list_descriptor->service_type[service_index], pszComment, sizeof(pszComment));
				sprintf_s(pszField, sizeof(pszField), "业务[%d](ID: 0x%04X，类型: 0x%02X)", service_index, pservice_list_descriptor->service_id[service_index], pservice_list_descriptor->service_type[service_index]);
				XMLElement* pxmlServiceNode = pxmlDoc->NewBranchElement(pxmlListNode, pszField, pszComment, 3);

				pxmlDoc->NewElementForBits(pxmlServiceNode, "service_id", pservice_list_descriptor->service_id[service_index], 16, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlServiceNode, "service_type", pservice_list_descriptor->service_type[service_index], 8, "uimsbf", pszComment);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解填充描述子					0x42
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解卫星传送系统描述子				0x43
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_satellite_delivery_system_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, satellite_delivery_system_descriptor_t* pSatelliteDeliverySystemDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		satellite_delivery_system_descriptor_t* psatellite_delivery_system_descriptor = (pSatelliteDeliverySystemDescriptor == NULL) ? new satellite_delivery_system_descriptor_t : pSatelliteDeliverySystemDescriptor;
		rtcode = DVB_SI_decode_satellite_delivery_system_descriptor(buf, length, psatellite_delivery_system_descriptor);
		DVB_SI_present_satellite_delivery_system_descriptor_to_xml(pxmlDoc, pxmlParentNode, psatellite_delivery_system_descriptor);

		if (pSatelliteDeliverySystemDescriptor == NULL)
		{
			delete psatellite_delivery_system_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_satellite_delivery_system_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, satellite_delivery_system_descriptor_t* psatellite_delivery_system_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];
	char   pszComment[256];
	char   pszFreq[32];
	char   pszModulation[32];
	char   pszPolar[32];
	char   pszSymboal[32];
	char   pszOrbital[32];
	char   pszWestEast[32];
	char   pszTemp[32];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (psatellite_delivery_system_descriptor != NULL))
	{
		sprintf_s(pszFreq, sizeof(pszFreq), "%X.%05X GHz", (psatellite_delivery_system_descriptor->frequency & 0xFFF00000) >> 20, psatellite_delivery_system_descriptor->frequency & 0x000FFFFF);
		sprintf_s(pszSymboal, sizeof(pszSymboal), "%X.%04X Msymbol/s", (psatellite_delivery_system_descriptor->symbol_rate & 0xFFFF0000) >> 16, (psatellite_delivery_system_descriptor->symbol_rate & 0x0000FFFF));
		sprintf_s(pszOrbital, sizeof(pszOrbital), "%X.%X degrees", (psatellite_delivery_system_descriptor->orbital_position & 0xFFF0) >> 4, psatellite_delivery_system_descriptor->orbital_position & 0x000F);

		DVB_SI_NumericCoding2Text_SatellitePolarization(psatellite_delivery_system_descriptor->polarization, pszPolar, sizeof(pszPolar));
		DVB_SI_NumericCoding2Text_SatelliteModulationSystem(psatellite_delivery_system_descriptor->modulation_system, pszModulation, sizeof(pszModulation));
		DVB_SI_NumericCoding2Text_SatelliteWestEastFlag(psatellite_delivery_system_descriptor->west_east_flag, pszWestEast, sizeof(pszWestEast));

		sprintf_s(pszField, sizeof(pszField), "satellite_delivery_system_descriptor(tag: 0x%02X)", psatellite_delivery_system_descriptor->descriptor_tag);
		sprintf_s(pszComment, sizeof(pszComment), "%s, %s, %s, %s, %s, %s", pszFreq, pszModulation, pszSymboal, pszPolar, pszOrbital, pszWestEast);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, psatellite_delivery_system_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", psatellite_delivery_system_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", psatellite_delivery_system_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "frequency", psatellite_delivery_system_descriptor->frequency, 32, "bslbf", pszFreq);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "orbital_position", psatellite_delivery_system_descriptor->orbital_position, 16, "bslbf", pszOrbital);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "west_east_flag", psatellite_delivery_system_descriptor->west_east_flag, 1, "bslbf", pszWestEast);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "polarization", psatellite_delivery_system_descriptor->polarization, 2, "bslbf", pszPolar);

		DVB_SI_NumericCoding2Text_SatelliteModulationRollOff(psatellite_delivery_system_descriptor->roll_off, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "roll_off", psatellite_delivery_system_descriptor->roll_off, 2, "bslbf", pszTemp);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "modulation_system", psatellite_delivery_system_descriptor->modulation_system, 1, "bslbf", pszModulation);

		DVB_SI_NumericCoding2Text_SatelliteModulationType(psatellite_delivery_system_descriptor->modulation_type, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "modulation_type", psatellite_delivery_system_descriptor->modulation_type, 2, "bslbf", pszTemp);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "symbol_rate", psatellite_delivery_system_descriptor->symbol_rate, 28, "bslbf", pszSymboal);

		DVB_SI_NumericCoding2Text_FECInner(psatellite_delivery_system_descriptor->FEC_inner, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "FEC_inner", psatellite_delivery_system_descriptor->FEC_inner, 4, "bslbf", pszTemp);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解有线传送系统描述子			0x44
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_cable_delivery_system_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, cable_delivery_system_descriptor_t* pCableDeliverySystemDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		cable_delivery_system_descriptor_t* pcable_delivery_system_descriptor = (pCableDeliverySystemDescriptor == NULL) ? new cable_delivery_system_descriptor_t : pCableDeliverySystemDescriptor;
		rtcode = DVB_SI_decode_cable_delivery_system_descriptor(buf, length, pcable_delivery_system_descriptor);
		DVB_SI_present_cable_delivery_system_descriptor_to_xml(pxmlDoc, pxmlParentNode, pcable_delivery_system_descriptor);

		if (pCableDeliverySystemDescriptor == NULL)
		{
			delete pcable_delivery_system_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_cable_delivery_system_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, cable_delivery_system_descriptor_t* pcable_delivery_system_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	char  pszField[64];
	char  pszComment[256];
	char  pszFreq[64];
	char  pszSymbol[64];
	char  pszModulation[64];
	char  pszFECOuter[64];
	char  pszFECInner[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pcable_delivery_system_descriptor != NULL))
	{
		sprintf_s(pszFreq, sizeof(pszFreq), "%X.%04X MHz", (pcable_delivery_system_descriptor->frequency & 0xFFFF0000) >> 16, pcable_delivery_system_descriptor->frequency & 0x0000FFFF);
		sprintf_s(pszSymbol, sizeof(pszSymbol), "%X.%04X Msymbol/s", (pcable_delivery_system_descriptor->symbol_rate & 0xFFFF0000) >> 16, pcable_delivery_system_descriptor->symbol_rate & 0x0000FFFF);

		DVB_SI_NumericCoding2Text_FECOuter(pcable_delivery_system_descriptor->FEC_outer, pszFECOuter, sizeof(pszFECOuter));
		DVB_SI_NumericCoding2Text_FECInner(pcable_delivery_system_descriptor->FEC_inner, pszFECInner, sizeof(pszFECInner));
		DVB_SI_NumericCoding2Text_CableModulation(pcable_delivery_system_descriptor->modulation, pszModulation, sizeof(pszModulation));

		sprintf_s(pszField, sizeof(pszField), "cable_delivery_system_descriptor(tag: 0x%02X)", pcable_delivery_system_descriptor->descriptor_tag);
		sprintf_s(pszComment, sizeof(pszComment), "%s, %s, %s", pszFreq, pszModulation, pszSymbol);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pcable_delivery_system_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pcable_delivery_system_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pcable_delivery_system_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "frequency", pcable_delivery_system_descriptor->frequency, 32, "bslbf", pszFreq);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved_future_use", pcable_delivery_system_descriptor->reserved_future_use, 12, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "FEC_outer", pcable_delivery_system_descriptor->FEC_outer, 4, "bslbf", pszFECOuter);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "modulation", pcable_delivery_system_descriptor->modulation, 8, "bslbf", pszModulation);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "symbol_rate", pcable_delivery_system_descriptor->symbol_rate, 28, "bslbf", pszSymbol);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "FEC_inner", pcable_delivery_system_descriptor->FEC_inner, 4, "bslbf", pszFECInner);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//0x45

//功能：解业务群名称描述子				0x47
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_bouquet_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, bouquet_name_descriptor_t* pBouquetNameDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		bouquet_name_descriptor_t* pbouquet_name_descriptor = (pBouquetNameDescriptor == NULL) ? new bouquet_name_descriptor_t : pBouquetNameDescriptor;
		rtcode = DVB_SI_decode_bouquet_name_descriptor(buf, length, pbouquet_name_descriptor);
		DVB_SI_present_bouquet_name_descriptor_to_xml(pxmlDoc, pxmlParentNode, pbouquet_name_descriptor);

		if (pBouquetNameDescriptor == NULL)
		{
			delete pbouquet_name_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_bouquet_name_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, bouquet_name_descriptor_t* pbouquet_name_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pbouquet_name_descriptor != NULL))
	{
		//tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "bouquet_name_descriptor()");

		//pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		//bouquet_name_descriptor_t* pbouquet_name_descriptor = (pBouquetNameDescriptor == NULL) ? new bouquet_name_descriptor_t : pBouquetNameDescriptor;
		//memset(pbouquet_name_descriptor, 0x00, sizeof(bouquet_name_descriptor_t));

		//BITS_map(&bs, buf, length);

		//pbouquet_name_descriptor->descriptor_tag = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pbouquet_name_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		//pbouquet_name_descriptor->descriptor_length = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pbouquet_name_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		//if ((pbouquet_name_descriptor->descriptor_length > 0) && (pbouquet_name_descriptor->descriptor_length <= (length - 2)))
		//{
		//	copy_length = min(pbouquet_name_descriptor->descriptor_length, MAX_BOUQUET_NAME_LENGTH);
		//	memcpy(pbouquet_name_descriptor->bouquet_name, bs.p_cur, copy_length);
		//	pbouquet_name_descriptor->bouquet_name[copy_length] = '\0';

		//	pbouquet_name_descriptor->trimmed_bouquet_name = DVB_SI_StringPrefixTrim(pbouquet_name_descriptor->bouquet_name);
		//	BITS_byteSkip(&bs, pbouquet_name_descriptor->descriptor_length);

		//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "bouquet_name", (uint8_t*)pbouquet_name_descriptor->bouquet_name,
		//		copy_length, pbouquet_name_descriptor->trimmed_bouquet_name, &bs);

		//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节, %s", pbouquet_name_descriptor->descriptor_tag, length, pbouquet_name_descriptor->trimmed_bouquet_name);
		//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);
		//}
		//else
		//{
		//	pxmlDescriptorNode->SetAttribute("error", "descriptor_length parameter error!");
		//}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解业务描述子				0x48
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_service_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, service_descriptor_t* pServiceDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		service_descriptor_t* pservice_descriptor = (pServiceDescriptor == NULL) ? new service_descriptor_t : pServiceDescriptor;
		rtcode = DVB_SI_decode_service_descriptor(buf, length, pservice_descriptor);
		DVB_SI_present_service_descriptor_to_xml(pxmlDoc, pxmlParentNode, pservice_descriptor);

		if (pServiceDescriptor == NULL)
		{
			delete pservice_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_service_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, service_descriptor_t* pservice_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pservice_descriptor != NULL))
	{
		//tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "service_descriptor()");

		//pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pservice_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		//pservice_descriptor->descriptor_length = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pservice_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		//pservice_descriptor->service_type = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_type", pservice_descriptor->service_type, 8, "uimsbf", NULL, &bs);

		//pservice_descriptor->service_provider_name_length = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_provider_name_length", pservice_descriptor->service_provider_name_length, 8, "uimsbf", NULL, &bs);

		//if (pservice_descriptor->service_provider_name_length > 0)
		//{
		//	copy_length = min(pservice_descriptor->service_provider_name_length, MAX_SERVICE_PROVIDER_NAME_LENGTH);
		//	memcpy(pservice_descriptor->service_provider_name, bs.p_cur, copy_length);
		//	pservice_descriptor->service_provider_name[copy_length] = '\0';

		//	pservice_descriptor->trimmed_service_provider_name = DVB_SI_StringPrefixTrim(pservice_descriptor->service_provider_name);
		//	BITS_byteSkip(&bs, pservice_descriptor->service_provider_name_length);

		//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_provider_name", (uint8_t*)pservice_descriptor->service_provider_name, copy_length,
		//		pservice_descriptor->trimmed_service_provider_name, &bs);
		//}

		//pservice_descriptor->service_name_length = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_name_length", pservice_descriptor->service_name_length, 8, "uimsbf", NULL, &bs);

		//if (pservice_descriptor->service_name_length > 0)
		//{
		//	copy_length = min(pservice_descriptor->service_name_length, MAX_SERVICE_NAME_LENGTH);

		//	memcpy(pservice_descriptor->service_name, bs.p_cur, copy_length);
		//	pservice_descriptor->service_name[copy_length] = '\0';

		//	pservice_descriptor->trimmed_service_name = DVB_SI_StringPrefixTrim(pservice_descriptor->service_name);
		//	BITS_byteSkip(&bs, pservice_descriptor->service_name_length);

		//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "service_name", (uint8_t*)pservice_descriptor->service_name, copy_length,
		//		pservice_descriptor->trimmed_service_name, &bs);

		//	sprintf_s(pszTemp, sizeof(pszTemp), "业务名称：%s", pservice_descriptor->trimmed_service_name);
		//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);
		//}

		//sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节, %s", pservice_descriptor->descriptor_tag, length, DVB_SI_StringPrefixTrim(pservice_descriptor->trimmed_service_name));
		//pxmlDescriptorNode->SetAttribute("comment", pszTemp);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解有效性国家描述子					0x49
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解链接描述子					0x4A
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解NVOD参考描述子					0x4B
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解时间偏移业务描述子					0x4C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解短节目段描述子					0x4D
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_short_event_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, short_event_descriptor_t* pShortEventDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		short_event_descriptor_t* pshort_event_descriptor = (pShortEventDescriptor == NULL) ? new short_event_descriptor_t : pShortEventDescriptor;
		rtcode = DVB_SI_decode_short_event_descriptor(buf, length, pshort_event_descriptor);
		DVB_SI_present_short_event_descriptor_to_xml(pxmlDoc, pxmlParentNode, pshort_event_descriptor);

		if (pShortEventDescriptor == NULL)
		{
			delete pshort_event_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_short_event_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, short_event_descriptor_t* pshort_event_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pshort_event_descriptor != NULL))
	{
		//XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "short_event_descriptor()");

		//pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		//short_event_descriptor_t* pshort_event_descriptor = (pShortEventDescriptor == NULL) ? new short_event_descriptor_t : pShortEventDescriptor;
		//memset(pshort_event_descriptor, 0x00, sizeof(short_event_descriptor_t));

		//BITS_map(&bs, buf, length);

		//pshort_event_descriptor->descriptor_tag = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pshort_event_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		//pshort_event_descriptor->descriptor_length = BITS_get(&bs, 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pshort_event_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		//if (pshort_event_descriptor->descriptor_length <= (length - 2))
		//{
		//	memcpy(pshort_event_descriptor->ISO_639_language_code_char, bs.p_cur, 3);
		//	pshort_event_descriptor->ISO_639_language_code_char[3] = '\0';

		//	pshort_event_descriptor->ISO_639_language_code = BITS_get(&bs, 24);
		//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "ISO_639_language_code", pshort_event_descriptor->ISO_639_language_code, 24, "bslbf", pshort_event_descriptor->ISO_639_language_code_char, &bs);

		//	pshort_event_descriptor->event_name_length = BITS_get(&bs, 8);
		//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "event_name_length", pshort_event_descriptor->event_name_length, 8, "uimsbf", NULL, &bs);

		//	if (pshort_event_descriptor->event_name_length <= (pshort_event_descriptor->descriptor_length - 5))
		//	{
		//		if (pshort_event_descriptor->event_name_length > 0)
		//		{
		//			uint8_t* ptemp = bs.p_cur;
		//			BITS_byteSkip(&bs, pshort_event_descriptor->event_name_length);

		//			copy_length = min(pshort_event_descriptor->event_name_length, MAX_EVENT_NAME_LENGTH);

		//			memcpy(pshort_event_descriptor->event_name_char, ptemp, copy_length);
		//			pshort_event_descriptor->event_name_char[copy_length] = '\0';

		//			pshort_event_descriptor->trimmed_event_name_char = DVB_SI_StringPrefixTrim(pshort_event_descriptor->event_name_char);

		//			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "event_name_char[ ]", (uint8_t*)pshort_event_descriptor->event_name_char, copy_length,
		//				pshort_event_descriptor->trimmed_event_name_char, &bs);
		//		}

		//		pshort_event_descriptor->text_length = BITS_get(&bs, 8);
		//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "text_length", pshort_event_descriptor->text_length, 8, "uimsbf", NULL, &bs);

		//		if (pshort_event_descriptor->text_length <= (pshort_event_descriptor->descriptor_length - 5 - pshort_event_descriptor->event_name_length))
		//		{
		//			uint8_t* ptemp = bs.p_cur;
		//			BITS_byteSkip(&bs, pshort_event_descriptor->text_length);

		//			if (pshort_event_descriptor->text_length > 0)
		//			{
		//				copy_length = min(pshort_event_descriptor->text_length, MAX_EVENT_TEXT_LENGTH);

		//				memcpy(pshort_event_descriptor->text_char, ptemp, copy_length);
		//				pshort_event_descriptor->text_char[copy_length] = '\0';

		//				pshort_event_descriptor->trimmed_text_char = DVB_SI_StringPrefixTrim(pshort_event_descriptor->text_char);

		//				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "text_char[ ]", (uint8_t*)pshort_event_descriptor->text_char, copy_length,
		//					pshort_event_descriptor->trimmed_text_char, &bs);
		//			}
		//		}
		//		else
		//		{
		//			sprintf_s(pszTemp, sizeof(pszTemp), "text_length error!");
		//			pxmlDescriptorNode->SetAttribute("error", pszTemp);

		//			rtcode = SECTION_PARSE_SYNTAX_ERROR;
		//		}
		//	}
		//	else
		//	{
		//		sprintf_s(pszTemp, sizeof(pszTemp), "event_name_length error!");
		//		pxmlDescriptorNode->SetAttribute("error", pszTemp);

		//		rtcode = SECTION_PARSE_SYNTAX_ERROR;
		//	}
		//}
		//else
		//{
		//	sprintf_s(pszTemp, sizeof(pszTemp), "descriptor length error!");
		//	pxmlDescriptorNode->SetAttribute("error", pszTemp);

		//	rtcode = SECTION_PARSE_SYNTAX_ERROR;
		//}

		//sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pshort_event_descriptor->descriptor_tag, length);
		//pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		//if (pShortEventDescriptor == NULL)
		//{
		//	delete pshort_event_descriptor;
		//}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解扩展节目段描述子					0x4E	
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解时间偏移节目段描述子				0x4F
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解组件描述子							0x50
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解马赛克描述子					SI_MOSAIC_DESCRIPTOR  = 0x51
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解流识别描述子					0x52
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin；
int DVB_SI_decode_stream_identifier_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, stream_identifier_descriptor_t* pStreamIdentifierDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	//char   pszTemp[64];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "stream_identifier_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	stream_identifier_descriptor_t* pstream_identifier_descriptor = (pStreamIdentifierDescriptor == NULL) ? new stream_identifier_descriptor_t : pStreamIdentifierDescriptor;
	//	memset(pstream_identifier_descriptor, 0x00, sizeof(stream_identifier_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pstream_identifier_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pstream_identifier_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pstream_identifier_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pstream_identifier_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	pstream_identifier_descriptor->component_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "component_tag", pstream_identifier_descriptor->component_tag, 8, "uimsbf", NULL, &bs);

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pstream_identifier_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pStreamIdentifierDescriptor == NULL)
	//	{
	//		delete pstream_identifier_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解CA标识描述子				0x53
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_CA_identifier_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, CA_identifier_descriptor_t* pCAIdentifierDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	//char   pszTemp[64];
	//BITS_t bs;
	////int    N;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "CA_identifier_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	CA_identifier_descriptor_t* pCA_identifier_descriptor = (pCAIdentifierDescriptor == NULL) ? new CA_identifier_descriptor_t : pCAIdentifierDescriptor;
	//	memset(pCA_identifier_descriptor, 0x00, sizeof(CA_identifier_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pCA_identifier_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pCA_identifier_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);
	//	pCA_identifier_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pCA_identifier_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pCA_identifier_descriptor->descriptor_length > 0)
	//	{
	//		tinyxml2::XMLElement* pxmlLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "CA_system_loop()");
	//		pxmlDoc->UpdateBufMark(pxmlLoopNode, bs.p_cur, bs.p_cur + pCA_identifier_descriptor->descriptor_length);

	//		pCA_identifier_descriptor->N = pCA_identifier_descriptor->descriptor_length / 2;

	//		for (int i = 0; i < pCA_identifier_descriptor->N; i++)
	//		{
	//			sprintf_s(pszTemp, sizeof(pszTemp), "CA_system %d", i);
	//			tinyxml2::XMLElement* pxmlCANode = pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, pszTemp);
	//			pxmlDoc->UpdateBufMark(pxmlCANode, bs.p_cur, bs.p_cur + 2);

	//			pCA_identifier_descriptor->CA_system_id[i] = BITS_get(&bs, 16);
	//			MPEG_DVB_NumericCoding2Text_CASystemID(pCA_identifier_descriptor->CA_system_id[i], pszTemp, sizeof(pszTemp));
	//			pxmlDoc->NewKeyValuePairElement(pxmlCANode, "CA_system_id", pCA_identifier_descriptor->CA_system_id[i], 16, "uimsbf", pszTemp, &bs);

	//			pxmlCANode->SetAttribute("comment", pszTemp);
	//		}
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pCA_identifier_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pCAIdentifierDescriptor == NULL)
	//	{
	//		delete pCA_identifier_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解内容描述子				0x54
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_content_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, content_descriptor_t* pContentDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	//char   pszTemp[64];
	//BITS_t bs;
	//int    N;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "content_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	content_descriptor_t* pcontent_descriptor = (pContentDescriptor == NULL) ? new content_descriptor_t : pContentDescriptor;
	//	memset(pcontent_descriptor, 0x00, sizeof(content_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pcontent_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pcontent_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pcontent_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pcontent_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pcontent_descriptor->descriptor_length <= (length - 2))
	//	{
	//		if (pcontent_descriptor->descriptor_length > 0)
	//		{
	//			sprintf_s(pszTemp, sizeof(pszTemp), "content_nibble_loop()");
	//			tinyxml2::XMLElement* pxmlLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);
	//			pxmlDoc->UpdateBufMark(pxmlLoopNode, bs.p_cur, bs.p_cur + pcontent_descriptor->descriptor_length);

	//			int loop_length = pcontent_descriptor->descriptor_length;

	//			N = 0;
	//			while ((loop_length >= 2) && (N < MAX_NIBBLES))
	//			{
	//				sprintf_s(pszTemp, sizeof(pszTemp), "content nibble %d", N);
	//				tinyxml2::XMLElement* pxmlNibbleNode = pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, pszTemp);
	//				pxmlDoc->UpdateBufMark(pxmlNibbleNode, bs.p_cur, bs.p_cur + 2);

	//				pcontent_descriptor->content_nibble_level_1[N] = BITS_get(&bs, 4);
	//				pxmlDoc->NewKeyValuePairElement(pxmlNibbleNode, "content_nibble_level_1", pcontent_descriptor->content_nibble_level_1[N], 4, "uimsbf", NULL, &bs);
	//				pcontent_descriptor->content_nibble_level_2[N] = BITS_get(&bs, 4);
	//				pxmlDoc->NewKeyValuePairElement(pxmlNibbleNode, "content_nibble_level_2", pcontent_descriptor->content_nibble_level_2[N], 4, "uimsbf", NULL, &bs);
	//				pcontent_descriptor->user_nibble_1[N] = BITS_get(&bs, 4);
	//				pxmlDoc->NewKeyValuePairElement(pxmlNibbleNode, "user_nibble_1", pcontent_descriptor->user_nibble_1[N], 4, "uimsbf", NULL, &bs);
	//				pcontent_descriptor->user_nibble_2[N] = BITS_get(&bs, 4);
	//				pxmlDoc->NewKeyValuePairElement(pxmlNibbleNode, "user_nibble_2", pcontent_descriptor->user_nibble_2[N], 4, "uimsbf", NULL, &bs);

	//				N++;
	//				loop_length -= 2;
	//			}
	//			pcontent_descriptor->N = N;
	//		}
	//	}
	//	else
	//	{
	//		sprintf_s(pszTemp, sizeof(pszTemp), "descriptor length error!");
	//		pxmlDescriptorNode->SetAttribute("error", pszTemp);

	//		rtcode = SECTION_PARSE_SYNTAX_ERROR;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pcontent_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pContentDescriptor == NULL)
	//	{
	//		delete pcontent_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解家长级别描述子				0x55
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_parental_rating_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, parental_rating_descriptor_t* pParentalRatingDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	//char   pszTemp[64];
	//BITS_t bs;
	//int    N;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "parental_rating_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	parental_rating_descriptor_t* pparental_rating_descriptor = (pParentalRatingDescriptor == NULL) ? new parental_rating_descriptor_t : pParentalRatingDescriptor;
	//	memset(pparental_rating_descriptor, 0x00, sizeof(parental_rating_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pparental_rating_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pparental_rating_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);
	//	pparental_rating_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pparental_rating_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pparental_rating_descriptor->descriptor_length <= (length - 2))
	//	{
	//		if (pparental_rating_descriptor->descriptor_length > 0)
	//		{
	//			//uint8_t* old_loop_ptr = bs.p_cur;
	//			sprintf_s(pszTemp, sizeof(pszTemp), "parental_rating_loop()");
	//			tinyxml2::XMLElement* pxmlLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);
	//			pxmlDoc->UpdateBufMark(pxmlLoopNode, bs.p_cur, bs.p_cur + pparental_rating_descriptor->descriptor_length);

	//			int loop_length = pparental_rating_descriptor->descriptor_length;

	//			N = 0;
	//			while ((loop_length >= 4) && (N < MAX_COUNTRIES))
	//			{
	//				sprintf_s(pszTemp, sizeof(pszTemp), "parental rating %d", N);
	//				tinyxml2::XMLElement* pxmlRatingNode = pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, pszTemp);
	//				pxmlDoc->UpdateBufMark(pxmlRatingNode, bs.p_cur, bs.p_cur + 4);

	//				memcpy(pparental_rating_descriptor->country_code_char[N], bs.p_cur, 3);
	//				pparental_rating_descriptor->country_code_char[N][3] = '\0';

	//				pparental_rating_descriptor->country_code[N] = BITS_get(&bs, 24);
	//				pxmlDoc->NewKeyValuePairElement(pxmlRatingNode, "country_code", pparental_rating_descriptor->country_code[N], 24, "bslbf", pparental_rating_descriptor->country_code_char[N], &bs);

	//				pparental_rating_descriptor->rating[N] = BITS_get(&bs, 8);
	//				pxmlDoc->NewKeyValuePairElement(pxmlRatingNode, "rating", pparental_rating_descriptor->rating[N], 8, "uimsbf", NULL, &bs);

	//				N++;
	//				loop_length -= 4;
	//			}

	//			pparental_rating_descriptor->N = N;
	//		}
	//	}
	//	else
	//	{
	//		sprintf_s(pszTemp, sizeof(pszTemp), "descriptor length error!");
	//		pxmlDescriptorNode->SetAttribute("error", pszTemp);

	//		rtcode = SECTION_PARSE_SYNTAX_ERROR;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pparental_rating_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pParentalRatingDescriptor == NULL)
	//	{
	//		delete pparental_rating_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解图文描述子						0x56
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
/*TAG = DVB_SI_TELETEXT_DESCRIPTOR				0x56		*/
int DVB_SI_decode_teletext_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, teletext_descriptor_t* pTeletextDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int N = 0;
	//int loop_length;
	//char pszTemp[32];
	//char pszLoopTitle[64];

	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "teletext_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	teletext_descriptor_t* pteletext_descriptor = (pTeletextDescriptor == NULL) ? new teletext_descriptor_t : pTeletextDescriptor;
	//	memset(pteletext_descriptor, 0x00, sizeof(teletext_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pteletext_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pteletext_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pteletext_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pteletext_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pteletext_descriptor->descriptor_length > 0)
	//	{
	//		uint8_t* ptemp = bs.p_cur;
	//		loop_length = pteletext_descriptor->descriptor_length;
	//		BITS_byteSkip(&bs, pteletext_descriptor->descriptor_length);

	//		BITS_t loop_bs;
	//		BITS_map(&loop_bs, ptemp, loop_length);

	//		while ((loop_length >= 5) && (N < MAX_LANGUAGES))
	//		{
	//			sprintf_s(pszTemp, sizeof(pszTemp), "%d", N);
	//			tinyxml2::XMLElement* pxmlLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

	//			uint8_t* old_ptr = loop_bs.p_cur;

	//			memcpy(pteletext_descriptor->description_loop[N].ISO_639_language_code_char, loop_bs.p_cur, 3);
	//			pteletext_descriptor->description_loop[N].ISO_639_language_code_char[3] = '\0';

	//			pteletext_descriptor->description_loop[N].ISO_639_language_code = BITS_get(&loop_bs, 24);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "ISO_639_language_code", pteletext_descriptor->description_loop[N].ISO_639_language_code, 24, "bslbf", pteletext_descriptor->description_loop[N].ISO_639_language_code_char, &loop_bs);

	//			pteletext_descriptor->description_loop[N].teletext_type = BITS_get(&loop_bs, 5);
	//			DVB_SI_NumericCoding2Text_TeletextType(pteletext_descriptor->description_loop[N].teletext_type, pszTemp, sizeof(pszTemp));
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "teletext_type", pteletext_descriptor->description_loop[N].teletext_type, 5, "uimsbf", pszTemp, &loop_bs);

	//			sprintf_s(pszLoopTitle, sizeof(pszLoopTitle), "%s-<%s>", pszTemp, pteletext_descriptor->description_loop[N].ISO_639_language_code_char);

	//			pteletext_descriptor->description_loop[N].teletext_magazine_number = BITS_get(&loop_bs, 3);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "teletext_magazine_number", pteletext_descriptor->description_loop[N].teletext_magazine_number, 3, "uimsbf", NULL, &loop_bs);

	//			pteletext_descriptor->description_loop[N].teletext_page_number = BITS_get(&loop_bs, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "teletext_page_number", pteletext_descriptor->description_loop[N].teletext_page_number, 8, "uimsbf", NULL, &loop_bs);

	//			pxmlDoc->UpdateBufMark(pxmlLoopNode, old_ptr, loop_bs.p_cur);
	//			pxmlLoopNode->SetAttribute("comment", pszLoopTitle);

	//			N++;
	//			loop_length -= 5;
	//		}

	//		pteletext_descriptor->N = N;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pteletext_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pTeletextDescriptor == NULL)
	//	{
	//		delete pteletext_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;
	//}

	return rtcode;
}

//功能：解电话描述子					0x57
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解当地时间偏移描述子			0x58
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_local_time_offset_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, local_time_offset_descriptor_t* pLocalTimeOffsetDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	//int  N = 0;
	//int  loop_length;
	//char   pszTemp[64];
	//char   pszCountry[4];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "local_time_offset_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	local_time_offset_descriptor_t* plocal_time_offset_descriptor = (pLocalTimeOffsetDescriptor == NULL) ? new local_time_offset_descriptor_t : pLocalTimeOffsetDescriptor;
	//	memset(plocal_time_offset_descriptor, 0x00, sizeof(local_time_offset_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	plocal_time_offset_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", plocal_time_offset_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	plocal_time_offset_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", plocal_time_offset_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	N = 0;
	//	if (plocal_time_offset_descriptor->descriptor_length > 0)
	//	{
	//		tinyxml2::XMLElement* pxmlLoopsNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "本地时间偏移循环()");
	//		pxmlDoc->UpdateBufMark(pxmlLoopsNode, bs.p_cur, bs.p_cur + plocal_time_offset_descriptor->descriptor_length);

	//		loop_length = plocal_time_offset_descriptor->descriptor_length;
	//		while ((loop_length >= 13) && (N < MAX_LOCAL_TIME_AREA))
	//		{
	//			unsigned char* old_local_ptr = bs.p_cur;
	//			tinyxml2::XMLElement* pxmlLocalNode = pxmlDoc->NewKeyValuePairElement(pxmlLoopsNode, "本地时间偏移()");

	//			plocal_time_offset_descriptor->local_time_offset[N].country_code = BITS_get(&bs, 24);

	//			unsigned int country_code = plocal_time_offset_descriptor->local_time_offset[N].country_code;
	//			pszCountry[0] = (char)((country_code & 0xff0000) >> 16);
	//			pszCountry[1] = (char)((country_code & 0x00ff00) >> 8);
	//			pszCountry[2] = (char)(country_code & 0x0000ff);
	//			pszCountry[3] = '\0';

	//			pxmlDoc->NewKeyValuePairElement(pxmlLocalNode, "country_code", plocal_time_offset_descriptor->local_time_offset[N].country_code, 24, "bslbf", pszCountry, &bs);

	//			plocal_time_offset_descriptor->local_time_offset[N].country_region_id = BITS_get(&bs, 6);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLocalNode, "country_region_id", plocal_time_offset_descriptor->local_time_offset[N].country_region_id, 6, "bslbf", NULL, &bs);

	//			plocal_time_offset_descriptor->local_time_offset[N].reserved = BITS_get(&bs, 1);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLocalNode, "reserved", plocal_time_offset_descriptor->local_time_offset[N].reserved, 1, "bslbf", NULL, &bs);

	//			plocal_time_offset_descriptor->local_time_offset[N].local_time_offset_polarity = BITS_get(&bs, 1);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLocalNode, "local_time_offset_polarity", plocal_time_offset_descriptor->local_time_offset[N].local_time_offset_polarity, 1, "bslbf", NULL, &bs);

	//			plocal_time_offset_descriptor->local_time_offset[N].local_time_offset = BITS_get(&bs, 16);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLocalNode, "local_time_offset", plocal_time_offset_descriptor->local_time_offset[N].local_time_offset, 16, "bslbf", NULL, &bs);

	//			int64_t time_of_change = BITS_x64_get(&bs, 40);
	//			plocal_time_offset_descriptor->local_time_offset[N].time_of_change = time_of_change;
	//			DVB_SI_NumericCoding2Text_UTCTime(time_of_change, pszTemp, sizeof(pszTemp));
	//			pxmlDoc->NewKeyValuePairElementX64Mode(pxmlLocalNode, "time_of_change", plocal_time_offset_descriptor->local_time_offset[N].time_of_change, 40, "bslbf", pszTemp, &bs);

	//			plocal_time_offset_descriptor->local_time_offset[N].next_time_offset = BITS_get(&bs, 16);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLocalNode, "next_time_offset", plocal_time_offset_descriptor->local_time_offset[N].next_time_offset, 16, "bslbf", NULL, &bs);

	//			pxmlDoc->UpdateBufMark(pxmlLocalNode, old_local_ptr, bs.p_cur);
	//			pxmlLocalNode->SetAttribute("comment", pszCountry);

	//			loop_length -= 13;
	//			N++;
	//		}
	//	}
	//	plocal_time_offset_descriptor->N = N;

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", plocal_time_offset_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pLocalTimeOffsetDescriptor == NULL)
	//	{
	//		delete plocal_time_offset_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解字幕描述子						0x59
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_subtitling_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, subtitling_descriptor_t* pSubtitlingDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int N = 0;
	//int loop_length;
	//char pszTemp[32];
	//char pszLoopTitle[64];

	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "subtitling_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	subtitling_descriptor_t* psubtitling_descriptor = (pSubtitlingDescriptor == NULL) ? new subtitling_descriptor_t : pSubtitlingDescriptor;
	//	memset(psubtitling_descriptor, 0x00, sizeof(subtitling_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	psubtitling_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", psubtitling_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	psubtitling_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", psubtitling_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (psubtitling_descriptor->descriptor_length > 0)
	//	{
	//		uint8_t* ptemp = bs.p_cur;
	//		loop_length = psubtitling_descriptor->descriptor_length;
	//		BITS_byteSkip(&bs, psubtitling_descriptor->descriptor_length);

	//		BITS_t loop_bs;
	//		BITS_map(&loop_bs, ptemp, loop_length);

	//		while ((loop_length >= 6) && (N < MAX_LANGUAGES))
	//		{
	//			sprintf_s(pszTemp, sizeof(pszTemp), "%d", N);
	//			tinyxml2::XMLElement* pxmlLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

	//			uint8_t* old_ptr = loop_bs.p_cur;

	//			memcpy(psubtitling_descriptor->description_loop[N].ISO_639_language_code_char, loop_bs.p_cur, 3);
	//			psubtitling_descriptor->description_loop[N].ISO_639_language_code_char[3] = '\0';

	//			psubtitling_descriptor->description_loop[N].ISO_639_language_code = BITS_get(&loop_bs, 24);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "ISO_639_language_code", psubtitling_descriptor->description_loop[N].ISO_639_language_code, 24, "bslbf", psubtitling_descriptor->description_loop[N].ISO_639_language_code_char, &loop_bs);

	//			psubtitling_descriptor->description_loop[N].subtitling_type = BITS_get(&loop_bs, 8);
	//			//DVB_SI_NumericCoding2Text_TeletextType(psubtitling_descriptor->description_loop[N].subtitling_type, pszTemp, sizeof(pszTemp));
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "subtitling_type", psubtitling_descriptor->description_loop[N].subtitling_type, 8, "uimsbf", NULL, &loop_bs);

	//			sprintf_s(pszLoopTitle, sizeof(pszLoopTitle), "%s", psubtitling_descriptor->description_loop[N].ISO_639_language_code_char);

	//			psubtitling_descriptor->description_loop[N].composition_page_id = BITS_get(&loop_bs, 16);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "teletext_magazine_number", psubtitling_descriptor->description_loop[N].composition_page_id, 16, "uimsbf", NULL, &loop_bs);

	//			psubtitling_descriptor->description_loop[N].ancillary_page_id = BITS_get(&loop_bs, 16);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLoopNode, "ancillary_page_id", psubtitling_descriptor->description_loop[N].ancillary_page_id, 16, "uimsbf", NULL, &loop_bs);

	//			pxmlDoc->UpdateBufMark(pxmlLoopNode, old_ptr, loop_bs.p_cur);
	//			pxmlLoopNode->SetAttribute("comment", pszLoopTitle);

	//			N++;
	//			loop_length -= 6;
	//		}

	//		psubtitling_descriptor->N = N;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", psubtitling_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pSubtitlingDescriptor == NULL)
	//	{
	//		delete psubtitling_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;
	//}

	return rtcode;
}

//功能：解地面传送系统描述子				0x5A
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解多语言网络名称描述子				0x5B
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_network_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_network_name_descriptor_t* pMultilingualNetworkNameDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int N = 0;
	//int loop_length;
	//char   pszTemp[128];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "multilingual_network_name_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	multilingual_network_name_descriptor_t* pmultilingual_network_name_descriptor = (pMultilingualNetworkNameDescriptor == NULL) ? new multilingual_network_name_descriptor_t : pMultilingualNetworkNameDescriptor;
	//	memset(pmultilingual_network_name_descriptor, 0x00, sizeof(multilingual_network_name_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pmultilingual_network_name_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pmultilingual_network_name_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pmultilingual_network_name_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pmultilingual_network_name_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pmultilingual_network_name_descriptor->descriptor_length > 0)
	//	{
	//		loop_length = pmultilingual_network_name_descriptor->descriptor_length;
	//		while ((loop_length >= 4) && (N < MAX_LANGUAGES))
	//		{
	//			uint8_t* old_loop_ptr = bs.p_cur;

	//			sprintf_s(pszTemp, sizeof(pszTemp), "language %d", N);
	//			tinyxml2::XMLElement* pxmlLangNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

	//			memcpy(pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, bs.p_cur, 3);
	//			pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code_char[3] = '\0';

	//			pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code = BITS_get(&bs, 24);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "ISO_639_language_code", pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code,
	//				24, "bslbf", pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, &bs);

	//			pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length = BITS_get(&bs, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "network_name_length", pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length, 8, "uimsbf", NULL, &bs);

	//			int copy_length = (pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length < MAX_NETWORK_NAME_LENGTH) ?
	//				pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length : MAX_NETWORK_NAME_LENGTH;

	//			memcpy(pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char, bs.p_cur, copy_length);
	//			pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char[copy_length] = '\0';
	//			BITS_byteSkip(&bs, pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length);

	//			pmultilingual_network_name_descriptor->LANGUAGE[N].trimmed_network_name_char = DVB_SI_StringPrefixTrim(pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char);

	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "network_name",
	//				(uint8_t*)pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char, pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length, 
	//				pmultilingual_network_name_descriptor->LANGUAGE[N].trimmed_network_name_char, &bs);

	//			pxmlDoc->UpdateBufMark(pxmlLangNode, old_loop_ptr, bs.p_cur);

	//			sprintf_s(pszTemp, sizeof(pszTemp), "%s:%s", pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, 
	//				pmultilingual_network_name_descriptor->LANGUAGE[N].trimmed_network_name_char);
	//			pxmlLangNode->SetAttribute("comment", pszTemp);

	//			loop_length -= (4 + pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length);
	//			N++;
	//		}

	//		pmultilingual_network_name_descriptor->N = N;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pmultilingual_network_name_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pMultilingualNetworkNameDescriptor == NULL)
	//	{
	//		delete pmultilingual_network_name_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解多语言业务群名称描述子			0x5C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_bouquet_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_bouquet_name_descriptor_t* pMultilingualBouquetNameDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int N = 0;
	//int loop_length;
	//char   pszTemp[128];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "multilingual_bouquet_name_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	multilingual_bouquet_name_descriptor_t* pmultilingual_bouquet_name_descriptor = (pMultilingualBouquetNameDescriptor == NULL) ? new multilingual_bouquet_name_descriptor_t : pMultilingualBouquetNameDescriptor;
	//	memset(pmultilingual_bouquet_name_descriptor, 0x00, sizeof(multilingual_bouquet_name_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pmultilingual_bouquet_name_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pmultilingual_bouquet_name_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pmultilingual_bouquet_name_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pmultilingual_bouquet_name_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pmultilingual_bouquet_name_descriptor->descriptor_length > 0)
	//	{
	//		loop_length = pmultilingual_bouquet_name_descriptor->descriptor_length;
	//		while ((loop_length >= 4) && (N < MAX_LANGUAGES))
	//		{
	//			uint8_t* old_loop_ptr = bs.p_cur;

	//			sprintf_s(pszTemp, sizeof(pszTemp), "language %d", N);
	//			tinyxml2::XMLElement* pxmlLangNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

	//			memcpy(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, bs.p_cur, 3);
	//			pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code_char[3] = '\0';
	//			pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code = BITS_get(&bs, 24);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "ISO_639_language_code", pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code,
	//				24, "bslbf", pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, &bs);

	//			pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length = BITS_get(&bs, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "bouquet_name_length", pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length, 8, "uimsbf", NULL, &bs);

	//			int copy_length = (pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length < MAX_BOUQUET_NAME_LENGTH) ?
	//				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length : MAX_BOUQUET_NAME_LENGTH;

	//			memcpy(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char, bs.p_cur, copy_length);
	//			pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char[copy_length] = '\0';
	//			pmultilingual_bouquet_name_descriptor->LANGUAGE[N].trimmed_bouquet_name_char = DVB_SI_StringPrefixTrim(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char);
	//			BITS_byteSkip(&bs, pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length);

	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "bouquet_name[ ]", (uint8_t*)pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char,
	//				copy_length, pmultilingual_bouquet_name_descriptor->LANGUAGE[N].trimmed_bouquet_name_char, &bs);

	//			pxmlDoc->UpdateBufMark(pxmlLangNode, old_loop_ptr, bs.p_cur);

	//			sprintf_s(pszTemp, sizeof(pszTemp), "%s:%s", pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, 
	//				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].trimmed_bouquet_name_char);

	//			pxmlLangNode->SetAttribute("comment", pszTemp);

	//			loop_length -= (4 + pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length);
	//			N++;
	//		}

	//		pmultilingual_bouquet_name_descriptor->N = N;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pmultilingual_bouquet_name_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pMultilingualBouquetNameDescriptor == NULL)
	//	{
	//		delete pmultilingual_bouquet_name_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解多语言业务名称描述子				0x5D
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_service_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_service_name_descriptor_t* pMultilingualServiceNameDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int N = 0;
	//int loop_length;
	//char   pszTemp[128];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "multilingual_service_name_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	multilingual_service_name_descriptor_t* pmultilingual_service_name_descriptor = (pMultilingualServiceNameDescriptor == NULL) ? new multilingual_service_name_descriptor_t : pMultilingualServiceNameDescriptor;
	//	memset(pmultilingual_service_name_descriptor, 0x00, sizeof(multilingual_service_name_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pmultilingual_service_name_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pmultilingual_service_name_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pmultilingual_service_name_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pmultilingual_service_name_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	if (pmultilingual_service_name_descriptor->descriptor_length > 0)
	//	{
	//		loop_length = pmultilingual_service_name_descriptor->descriptor_length;
	//		while ((loop_length >= 5) && (N < MAX_LANGUAGES))
	//		{
	//			uint8_t* old_loop_ptr = bs.p_cur;

	//			sprintf_s(pszTemp, sizeof(pszTemp), "language %d", N);
	//			tinyxml2::XMLElement* pxmlLangNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

	//			memcpy(pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, bs.p_cur, 3);
	//			pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code_char[3] = '\0';
	//			pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code = BITS_get(&bs, 24);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "ISO_639_language_code", pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code,
	//				24, "bslbf", pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, &bs);

	//			pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length = BITS_get(&bs, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "service_provider_name_length", pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length,
	//				8, "uimsbf", NULL, &bs);

	//			int copy_length = (pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length < MAX_SERVICE_PROVIDER_NAME_LENGTH) ?
	//				pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length : MAX_SERVICE_PROVIDER_NAME_LENGTH;

	//			memcpy(pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char, bs.p_cur, copy_length);
	//			pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char[copy_length] = '\0';
	//			pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_provider_name_char = DVB_SI_StringPrefixTrim(pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char);

	//			BITS_byteSkip(&bs, pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length);

	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "service_provider_name",
	//				(uint8_t*)pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char, 
	//				pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length, 
	//				pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_provider_name_char, &bs);

	//			pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length = BITS_get(&bs, 8);
	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "service_name_length", pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length, 8, "uimsbf", NULL, &bs);

	//			copy_length = (pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length < MAX_SERVICE_NAME_LENGTH) ?
	//				pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length : MAX_SERVICE_NAME_LENGTH;

	//			memcpy(pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char, bs.p_cur, copy_length);
	//			pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char[copy_length] = '\0';
	//			pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_name_char = DVB_SI_StringPrefixTrim(pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char);

	//			BITS_byteSkip(&bs, pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length);

	//			pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "service_name[ ]",
	//				(uint8_t*)pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char, 
	//				pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length, 
	//				pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_name_char, &bs);

	//			pxmlDoc->UpdateBufMark(pxmlLangNode, old_loop_ptr, bs.p_cur);
	//			sprintf_s(pszTemp, sizeof(pszTemp), "%s:%s", pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_name_char);
	//			pxmlLangNode->SetAttribute("comment", pszTemp);

	//			loop_length -= (5 + pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length + pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length);
	//			N++;
	//		}

	//		pmultilingual_service_name_descriptor->N = N;
	//	}

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pmultilingual_service_name_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pMultilingualServiceNameDescriptor == NULL)
	//	{
	//		delete pmultilingual_service_name_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解多语言组成描述子					0x5E
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_component_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_component_descriptor_t* pMultilingualComponentDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int N = 0;
	//int loop_length;
	//char   pszTemp[128];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "multilingual_component_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	multilingual_component_descriptor_t* pmultilingual_component_descriptor = (pMultilingualComponentDescriptor == NULL) ? new multilingual_component_descriptor_t : pMultilingualComponentDescriptor;
	//	memset(pmultilingual_component_descriptor, 0x00, sizeof(multilingual_component_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pmultilingual_component_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pmultilingual_component_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pmultilingual_component_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pmultilingual_component_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	pmultilingual_component_descriptor->component_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "component_tag", pmultilingual_component_descriptor->component_tag, 8, "uimsbf", NULL, &bs);

	//	loop_length = pmultilingual_component_descriptor->descriptor_length - 1;

	//	while ((loop_length >= 4) && (N < MAX_LANGUAGES))
	//	{
	//		uint8_t* old_loop_ptr = bs.p_cur;

	//		sprintf_s(pszTemp, sizeof(pszTemp), "language %d", N);
	//		tinyxml2::XMLElement* pxmlLangNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, pszTemp);

	//		memcpy(pmultilingual_component_descriptor->ISO_639_language_code_char[N], bs.p_cur, 3);
	//		pmultilingual_component_descriptor->ISO_639_language_code_char[N][3] = '\0';
	//		pmultilingual_component_descriptor->ISO_639_language_code[N] = BITS_get(&bs, 24);
	//		pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "ISO_639_language_code", pmultilingual_component_descriptor->ISO_639_language_code[N], 24, "bslbf", pmultilingual_component_descriptor->ISO_639_language_code_char[N], &bs);

	//		pmultilingual_component_descriptor->text_description_length[N] = BITS_get(&bs, 8);

	//		int copy_length = (pmultilingual_component_descriptor->text_description_length[N] < MAX_COMPONENT_TEXT_LENGTH) ?
	//			pmultilingual_component_descriptor->text_description_length[N] : MAX_COMPONENT_TEXT_LENGTH;

	//		memcpy(pmultilingual_component_descriptor->text_char[N], bs.p_cur, copy_length);
	//		pmultilingual_component_descriptor->text_char[N][copy_length] = '\0';
	//		BITS_byteSkip(&bs, pmultilingual_component_descriptor->text_description_length[N]);
	//		pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "text_description", -1, -1, NULL, pmultilingual_component_descriptor->text_char[N], &bs);

	//		pxmlDoc->UpdateBufMark(pxmlLangNode, old_loop_ptr, bs.p_cur);
	//		sprintf_s(pszTemp, sizeof(pszTemp), "%s:%s", pmultilingual_component_descriptor->ISO_639_language_code_char[N], pmultilingual_component_descriptor->text_char[N]);
	//		pxmlLangNode->SetAttribute("comment", pszTemp);

	//		loop_length -= (4 + pmultilingual_component_descriptor->text_description_length[N]);
	//		N++;
	//	}

	//	pmultilingual_component_descriptor->N = N;

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pmultilingual_component_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pMultilingualComponentDescriptor == NULL)
	//	{
	//		delete pmultilingual_component_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR			0x5F
//功能：解私有数据说明描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_private_data_specifier_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, private_data_specifier_descriptor_t* pPrivateDataSpecifierDescriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		private_data_specifier_descriptor_t* pprivate_data_specifier_descriptor = (pPrivateDataSpecifierDescriptor == NULL) ? new private_data_specifier_descriptor_t : pPrivateDataSpecifierDescriptor;
		rtcode = DVB_SI_decode_private_data_specifier_descriptor(buf, length, pprivate_data_specifier_descriptor);
		DVB_SI_present_private_data_specifier_descriptor_to_xml(pxmlDoc, pxmlParentNode, pprivate_data_specifier_descriptor);

		if (pPrivateDataSpecifierDescriptor == NULL)
		{
			delete pprivate_data_specifier_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_private_data_specifier_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, private_data_specifier_descriptor_t* pprivate_data_specifier_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];
	char   pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pprivate_data_specifier_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "private_data_specifier_descriptor(tag:0x%02X)", pprivate_data_specifier_descriptor->descriptor_tag);
		sprintf_s(pszComment, sizeof(pszComment), "private_data_specifier = 0x%08X", pprivate_data_specifier_descriptor->private_data_specifier);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pprivate_data_specifier_descriptor->descriptor_tag);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pprivate_data_specifier_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pprivate_data_specifier_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "private_data_specifier", pprivate_data_specifier_descriptor->private_data_specifier, 32, "uimsbf", NULL);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解业务迁移描述子				0x60
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解短平滑缓冲器描述子				0x61
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解频率列表描述子						0x62
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解部分传送流描述子						0x63
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解传送流描述符					0x67
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_transport_stream_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, transport_stream_descriptor_t* pTSDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[128];
	char	pszComment[128];

	transport_stream_descriptor_t* ptransport_stream_descriptor = (pTSDescriptor == NULL) ? new transport_stream_descriptor_t : pTSDescriptor;
	rtcode = DVB_SI_decode_transport_stream_descriptor(buf, length, ptransport_stream_descriptor);

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "transport_stream_descriptor(tag: 0x%02X)", ptransport_stream_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, ptransport_stream_descriptor->descriptor_length + 2);

		if (rtcode != SECTION_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			pxmlDescriptorNode->SetAttribute("error", pszComment);
		}

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", ptransport_stream_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", ptransport_stream_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "byte_char", (uint8_t*)(ptransport_stream_descriptor->byte_char), ptransport_stream_descriptor->descriptor_length, ptransport_stream_descriptor->byte_char);
	}

	if (pTSDescriptor == NULL)
	{
		delete ptransport_stream_descriptor;
	}

	return rtcode;
}

//
//int EN301192_decode_data_carousel_info_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_carousel_info_t* pDataCarouselInfo = NULL)
//{
//	int rtcode = SECTION_PARSE_NO_ERROR;
//	char pszTemp[64];
//	BITS_t  bs;
//
//	if ((pxmlDoc != NULL) && (buf != NULL) && (length >= 16))
//	{
//		//tinyxml2::XMLElement* pxmlParentNode = pxmlDoc->GetActiveNode();
//
//		data_carousel_info_t* pdata_carousel_info = (pDataCarouselInfo == NULL) ? new data_carousel_info_t : pDataCarouselInfo;
//		memset(pdata_carousel_info, 0x00, sizeof(data_carousel_info_t));
//
//		BITS_map(&bs, buf, length);
//
//		pdata_carousel_info->carousel_type_id = BITS_get(&bs, 2);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "carousel_type_id", pdata_carousel_info->carousel_type_id, 2, "bslbf", NULL, &bs);
//
//		pdata_carousel_info->reserved0 = BITS_get(&bs, 6);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pdata_carousel_info->reserved0, 6, "bslbf", NULL, &bs);
//
//		pdata_carousel_info->transaction_id = BITS_get(&bs, 32);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "transaction_id", pdata_carousel_info->transaction_id, 32, "uimsbf", NULL, &bs);
//
//		pdata_carousel_info->time_out_value_DSI = BITS_get(&bs, 32);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DSI", pdata_carousel_info->time_out_value_DSI, 32, "uimsbf", NULL, &bs);
//
//		pdata_carousel_info->time_out_value_DII = BITS_get(&bs, 32);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DII", pdata_carousel_info->time_out_value_DII, 32, "uimsbf", NULL, &bs);
//
//		pdata_carousel_info->reserved1 = BITS_get(&bs, 2);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pdata_carousel_info->reserved1, 2, "bslbf", NULL, &bs);
//
//		pdata_carousel_info->leak_rate = BITS_get(&bs, 22);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "leak_rate", pdata_carousel_info->leak_rate, 22, "uimsbf", NULL, &bs);
//
//		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", length);
//		pxmlParentNode->SetAttribute("comment", pszTemp);
//
//		if (pDataCarouselInfo == NULL)
//		{
//			delete pdata_carousel_info;
//		}
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
//	}
//
//	return rtcode;
//}

//int EN301192_decode_object_carousel_info_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, object_carousel_info_t* pObjectCarouselInfo)
//{
//	int rtcode = SECTION_PARSE_NO_ERROR;
//	int	N;
//	int	copy_length;
//	char pszTemp[64];
//	BITS_t  bs;
//
//
//	if ((pxmlDoc != NULL) && (buf != NULL) && (length >= 16))
//	{
//		//tinyxml2::XMLElement* pxmlParentNode = pxmlDoc->GetActiveNode();
//
//		object_carousel_info_t* pobject_carousel_info = (pObjectCarouselInfo == NULL) ? new object_carousel_info_t : pObjectCarouselInfo;
//		memset(pobject_carousel_info, 0x00, sizeof(object_carousel_info_t));
//
//		BITS_map(&bs, buf, length);
//
//		pobject_carousel_info->carousel_type_id = BITS_get(&bs, 2);
//		DVB_SI_NumericCoding2Text_CarouselTypeID(pobject_carousel_info->carousel_type_id, pszTemp, sizeof(pszTemp));
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "carousel_type_id", pobject_carousel_info->carousel_type_id, 2, "bslbf", pszTemp, &bs);
//
//		pobject_carousel_info->reserved0 = BITS_get(&bs, 6);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pobject_carousel_info->reserved0, 6, "bslbf", NULL, &bs);
//
//		pobject_carousel_info->transaction_id = BITS_get(&bs, 32);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "transaction_id", pobject_carousel_info->transaction_id, 32, "uimsbf", NULL, &bs);
//
//		pobject_carousel_info->time_out_value_DSI = BITS_get(&bs, 32);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DSI", pobject_carousel_info->time_out_value_DSI, 32, "uimsbf", NULL, &bs);
//
//		pobject_carousel_info->time_out_value_DII = BITS_get(&bs, 32);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "time_out_value_DII", pobject_carousel_info->time_out_value_DII, 32, "uimsbf", NULL, &bs);
//
//		pobject_carousel_info->reserved1 = BITS_get(&bs, 2);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "reserved", pobject_carousel_info->reserved1, 2, "bslbf", NULL, &bs);
//
//		pobject_carousel_info->leak_rate = BITS_get(&bs, 22);
//		pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "leak_rate", pobject_carousel_info->leak_rate, 22, "uimsbf", NULL, &bs);
//
//		N = 0;
//		int object_name_loop_length = (int)(bs.p_end - bs.p_cur);
//		if (object_name_loop_length > 0)
//		{
//			uint8_t* pl1temp = bs.p_cur;
//			BITS_byteSkip(&bs, object_name_loop_length);
//			tinyxml2::XMLElement* pxmlServiceLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "object_name 循环()", -1, -1, NULL, NULL, &bs);
//
//			BITS_t object_name_loop_bs;
//			BITS_map(&object_name_loop_bs, pl1temp, object_name_loop_length);
//			while ((object_name_loop_length >= 4) && (N < 16))
//			{
//				uint8_t* old_ptr = object_name_loop_bs.p_cur;
//
//				sprintf_s(pszTemp, sizeof(pszTemp), "object_name [%d]", N);
//				tinyxml2::XMLElement* pxmlObjectNameNode = pxmlDoc->NewKeyValuePairElement(pxmlServiceLoopNode, pszTemp);
//
//				memcpy(pobject_carousel_info->ISO_639_language_code_char[N], object_name_loop_bs.p_cur, 3);
//				pobject_carousel_info->ISO_639_language_code_char[N][3] = '\0';
//				pobject_carousel_info->ISO_639_language_code[N] = BITS_get(&object_name_loop_bs, 24);
//				pxmlDoc->NewKeyValuePairElement(pxmlObjectNameNode, "ISO_639_language_code", pobject_carousel_info->ISO_639_language_code[N], 24, "bslbf", pobject_carousel_info->ISO_639_language_code_char[N], &object_name_loop_bs);
//
//				pobject_carousel_info->object_name_length[N] = BITS_get(&object_name_loop_bs, 8);
//				pxmlDoc->NewKeyValuePairElement(pxmlObjectNameNode, "object_name_length", pobject_carousel_info->object_name_length[N], 8, "uimsbf", NULL, &object_name_loop_bs);
//
//				if (pobject_carousel_info->object_name_length[N] > 0)
//				{
//					copy_length = pobject_carousel_info->object_name_length[N];
//					if (copy_length > 31)
//					{
//						copy_length = 31;
//					}
//					memcpy(pobject_carousel_info->object_name_char[N], object_name_loop_bs.p_cur, copy_length);
//					pobject_carousel_info->object_name_char[N][copy_length] = '\0';
//					BITS_byteSkip(&object_name_loop_bs, pobject_carousel_info->object_name_length[N]);
//					pxmlDoc->NewKeyValuePairElement(pxmlObjectNameNode, "object_name_char", -1, -1, NULL, pobject_carousel_info->object_name_char[N], &object_name_loop_bs);
//				}
//				pxmlDoc->UpdateBufMark(pxmlObjectNameNode, old_ptr, object_name_loop_bs.p_cur);
//				object_name_loop_length -= (int)(object_name_loop_bs.p_cur - old_ptr);
//
//				N++;
//			}
//		}
//		pobject_carousel_info->N = N;
//
//		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", length);
//		pxmlParentNode->SetAttribute("comment", pszTemp);
//
//		if (pObjectCarouselInfo == NULL)
//		{
//			delete pobject_carousel_info;
//		}
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
//	}
//
//	return rtcode;
//}

//功能：解数据广播描述子				0x64
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_data_broadcast_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_broadcast_descriptor_t* pDataBroadcastDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int	copy_length;
	//char pszTemp[64];
	//char pszCarouselType[64];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "data_broadcast_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	if ((buf != NULL) && (length >= 10))
	//	{
	//		data_broadcast_descriptor_t* pdata_broadcast_descriptor = (pDataBroadcastDescriptor == NULL) ? new data_broadcast_descriptor_t : pDataBroadcastDescriptor;
	//		memset(pdata_broadcast_descriptor, 0x00, sizeof(data_broadcast_descriptor_t));

	//		BITS_map(&bs, buf, length);

	//		pdata_broadcast_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pdata_broadcast_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//		pdata_broadcast_descriptor->descriptor_length = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pdata_broadcast_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//		pdata_broadcast_descriptor->data_broadcast_id = BITS_get(&bs, 16);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "data_broadcast_id", pdata_broadcast_descriptor->data_broadcast_id, 16, "uimsbf", NULL, &bs);

	//		pdata_broadcast_descriptor->component_tag = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "component_tag", pdata_broadcast_descriptor->component_tag, 8, "uimsbf", NULL, &bs);

	//		pdata_broadcast_descriptor->selector_length = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_length", pdata_broadcast_descriptor->selector_length, 8, "uimsbf", NULL, &bs);

	//		if (pdata_broadcast_descriptor->selector_length > 0)
	//		{
	//			uint8_t* old_ptr = bs.p_cur;
	//			tinyxml2::XMLElement* pxmlSelectorNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_byte()");

	//			if (pdata_broadcast_descriptor->data_broadcast_id == 0x0005)
	//			{
	//				pxmlSelectorNode->SetAttribute("comment", "multiprotocol_encapsulation_info");
	//				DVB_Databroadcast_decode_multiprotocol_encapsulation_info(bs.p_cur, pdata_broadcast_descriptor->selector_length, &(pdata_broadcast_descriptor->selector_byte.multiprotocol_encapsulation_info));
	//			}
	//			else if (pdata_broadcast_descriptor->data_broadcast_id == 0x0006)
	//			{
	//				pxmlSelectorNode->SetAttribute("comment", "data_carousel_info");
	//				DVB_Databroadcast_decode_data_carousel_info_to_xml(bs.p_cur, pdata_broadcast_descriptor->selector_length, pxmlDoc, pxmlSelectorNode, &(pdata_broadcast_descriptor->selector_byte.data_carousel_info));
	//			}
	//			else if (pdata_broadcast_descriptor->data_broadcast_id == 0x0007)
	//			{
	//				pxmlSelectorNode->SetAttribute("comment", "object_carousel_info");
	//				DVB_Databroadcast_decode_object_carousel_info_to_xml(bs.p_cur, pdata_broadcast_descriptor->selector_length, pxmlDoc, pxmlSelectorNode, &(pdata_broadcast_descriptor->selector_byte.object_carousel_info));
	//			}
	//			else if (pdata_broadcast_descriptor->data_broadcast_id == 0x000B)
	//			{
	//				pxmlSelectorNode->SetAttribute("comment", "IP/MAC_notification_info");
	//				INT_decode_IP_MAC_notification_info(bs.p_cur, pdata_broadcast_descriptor->selector_length, &(pdata_broadcast_descriptor->selector_byte.IP_MAC_notification_info));
	//			}
	//			else
	//			{
	//				copy_length = (pdata_broadcast_descriptor->selector_length < MAX_DATABROADCAST_SELECT_LENGTH) ?
	//					pdata_broadcast_descriptor->selector_length : MAX_DATABROADCAST_SELECT_LENGTH;
	//				memcpy(pdata_broadcast_descriptor->selector_byte.buf, bs.p_cur, copy_length);
	//			}
	//			BITS_byteSkip(&bs, pdata_broadcast_descriptor->selector_length);
	//			pxmlDoc->UpdateBufMark(pxmlSelectorNode, old_ptr, bs.p_cur);
	//		}

	//		memcpy(pdata_broadcast_descriptor->ISO_639_language_code_char, bs.p_cur, 3);
	//		pdata_broadcast_descriptor->ISO_639_language_code_char[3] = '\0';
	//		pdata_broadcast_descriptor->ISO_639_language_code = BITS_get(&bs, 24);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "ISO_639_language_code", pdata_broadcast_descriptor->ISO_639_language_code, 24, "uimsbf", pdata_broadcast_descriptor->ISO_639_language_code_char, &bs);

	//		pdata_broadcast_descriptor->text_length = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "text_length", pdata_broadcast_descriptor->text_length, 8, "uimsbf", NULL, &bs);

	//		if (pdata_broadcast_descriptor->text_length > 0)
	//		{
	//			copy_length = (pdata_broadcast_descriptor->text_length < MAX_DATABROADCAST_TEXT_LENGTH) ? pdata_broadcast_descriptor->text_length : MAX_DATABROADCAST_TEXT_LENGTH;
	//			memcpy(pdata_broadcast_descriptor->text_char, bs.p_cur, copy_length);
	//			pdata_broadcast_descriptor->text_char[copy_length] = '\0';

	//			BITS_byteSkip(&bs, pdata_broadcast_descriptor->text_length);
	//		}

	//		DVB_SI_NumericCoding2Text_DataBroadcastID(pdata_broadcast_descriptor->data_broadcast_id, pszCarouselType, sizeof(pszCarouselType));
	//		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节, %s", pdata_broadcast_descriptor->descriptor_tag, length, pszCarouselType);
	//		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//		if (pDataBroadcastDescriptor == NULL)
	//		{
	//			delete pdata_broadcast_descriptor;
	//		}
	//	}
	//	else
	//	{
	//		pxmlDescriptorNode->SetAttribute("error", "Parameter Error!");
	//		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//功能：解加扰描述子					0x65
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解数据广播标识描述子				0x66
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_data_broadcast_id_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_broadcast_id_descriptor_t* pDataBroadcastIDDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	//int	copy_length;
	//char pszTemp[64];
	//char pszCarouselType[32];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "data_broadcast_id_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	data_broadcast_id_descriptor_t* pdata_broadcast_id_descriptor = (pDataBroadcastIDDescriptor == NULL) ? new data_broadcast_id_descriptor_t : pDataBroadcastIDDescriptor;
	//	memset(pdata_broadcast_id_descriptor, 0x00, sizeof(data_broadcast_id_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pdata_broadcast_id_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pdata_broadcast_id_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pdata_broadcast_id_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pdata_broadcast_id_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	pdata_broadcast_id_descriptor->data_broadcast_id = BITS_get(&bs, 16);
	//	DVB_SI_NumericCoding2Text_DataBroadcastID(pdata_broadcast_id_descriptor->data_broadcast_id, pszCarouselType, sizeof(pszCarouselType));
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "data_broadcast_id", pdata_broadcast_id_descriptor->data_broadcast_id, 16, "uimsbf", pszCarouselType, &bs);

	//	copy_length = pdata_broadcast_id_descriptor->descriptor_length - 2;

	//	pdata_broadcast_id_descriptor->id_selector_length = copy_length;

	//	if (copy_length > 0)
	//	{
	//		uint8_t* old_ptr = bs.p_cur;
	//		tinyxml2::XMLElement* pxmlSelectorNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "id_selector_byte()");
	//		pxmlSelectorNode->SetAttribute("comment", pszCarouselType);

	//		if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x0005)
	//		{
	//			DVB_Databroadcast_decode_multiprotocol_encapsulation_info(bs.p_cur, copy_length, &(pdata_broadcast_id_descriptor->id_selector_byte.multiprotocol_encapsulation_info));
	//		}
	//		else if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x0006)
	//		{
	//			DVB_Databroadcast_decode_data_carousel_info_to_xml(bs.p_cur, copy_length, pxmlDoc, pxmlSelectorNode, &(pdata_broadcast_id_descriptor->id_selector_byte.data_carousel_info));
	//		}
	//		else if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x0007)
	//		{
	//			DVB_Databroadcast_decode_object_carousel_info_to_xml(bs.p_cur, copy_length, pxmlDoc, pxmlSelectorNode, &(pdata_broadcast_id_descriptor->id_selector_byte.object_carousel_info));
	//		}
	//		else if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x000B)
	//		{
	//			INT_decode_IP_MAC_notification_info(bs.p_cur, copy_length, &(pdata_broadcast_id_descriptor->id_selector_byte.IP_MAC_notification_info));
	//		}
	//		else
	//		{
	//			copy_length = (copy_length < MAX_DATABROADCAST_SELECT_LENGTH) ? copy_length : MAX_DATABROADCAST_SELECT_LENGTH;
	//			memcpy(pdata_broadcast_id_descriptor->id_selector_byte.buf, bs.p_cur, copy_length);
	//			pdata_broadcast_id_descriptor->id_selector_length = copy_length;
	//		}

	//		BITS_byteSkip(&bs, copy_length);
	//		pxmlDoc->UpdateBufMark(pxmlSelectorNode, old_ptr, bs.p_cur);
	//	}

	//	DVB_SI_NumericCoding2Text_DataBroadcastID(pdata_broadcast_id_descriptor->data_broadcast_id, pszCarouselType, sizeof(pszCarouselType));
	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节, %s", pdata_broadcast_id_descriptor->descriptor_tag, length, pszCarouselType);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pDataBroadcastIDDescriptor == NULL)
	//	{
	//		delete pdata_broadcast_id_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}


//功能：解传送流描述符					0x67
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针


//功能：解AC3描述子					0x6A
//输入：
//返回：
//在TS 102 366中定义
int DVB_SI_decode_ac3_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ac3_descriptor_t* pAC3Descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	//int  N = 0;
	//char   pszTemp[64];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "ac3_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	ac3_descriptor_t* pac3_descriptor = (pAC3Descriptor == NULL) ? new ac3_descriptor_t : pAC3Descriptor;
	//	memset(pac3_descriptor, 0x00, sizeof(ac3_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pac3_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pac3_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pac3_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pac3_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	pac3_descriptor->component_type_flag = BITS_get(&bs, 1);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "component_type_flag", pac3_descriptor->component_type_flag, 1, "bslbf", NULL, &bs);
	//	pac3_descriptor->bsid_flag = BITS_get(&bs, 1);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "bsid_flag", pac3_descriptor->bsid_flag, 1, "bslbf", NULL, &bs);
	//	pac3_descriptor->mainid_flag = BITS_get(&bs, 1);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "mainid_flag", pac3_descriptor->mainid_flag, 1, "bslbf", NULL, &bs);
	//	pac3_descriptor->asvc_flag = BITS_get(&bs, 1);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "asvc_flag", pac3_descriptor->asvc_flag, 1, "bslbf", NULL, &bs);
	//	pac3_descriptor->reserved_flags = BITS_get(&bs, 4);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved_flags", pac3_descriptor->reserved_flags, 1, "bslbf", NULL, &bs);

	//	if (pac3_descriptor->component_type_flag)
	//	{
	//		pac3_descriptor->component_type = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "component_type", pac3_descriptor->component_type, 8, "uimsbf", NULL, &bs);
	//	}
	//	if (pac3_descriptor->bsid_flag)
	//	{
	//		pac3_descriptor->bsid = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "bsid", pac3_descriptor->bsid, 8, "uimsbf", NULL, &bs);
	//	}
	//	if (pac3_descriptor->mainid_flag)
	//	{
	//		pac3_descriptor->mainid = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "mainid", pac3_descriptor->mainid, 8, "uimsbf", NULL, &bs);
	//	}
	//	if (pac3_descriptor->asvc_flag)
	//	{
	//		pac3_descriptor->asvc = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "asvc", pac3_descriptor->asvc, 8, "uimsbf", NULL, &bs);
	//	}

	//	N = length - (int)(bs.p_cur - bs.p_start);
	//	BITS_byteCopy(pac3_descriptor->additional_info, sizeof(pac3_descriptor->additional_info), &bs, N);		//maybe overflow
	//	pac3_descriptor->N = N;
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "additional_info", -1, -1, NULL, NULL, &bs);

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pac3_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pAC3Descriptor == NULL)
	//	{
	//		delete pac3_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}


//功能：解APPLICATION_SIGNALLING描述子			0x6F
//输入：
//返回：

//0x6C

//0x6D

//0x77

//0x7F

//0x81
//功能：解AC3_AUDIO描述子
//输入：
//返回：

//功能：解CHANNEL_ORDER描述子
//输入：
//返回：

//功能：解CHANNEL_VOLUME_COMPENSATING描述子
//输入：
//返回：

//功能：解CHANNEL_CLASSIFY描述子
//输入：
//返回：


//0x0C


//0x0D


//0x11

//0x13


//0x00

//0x01

//0x02

//0x03

//0x04

//0x05

//0x08

//0x09


//0x0A

//0x0B

//0x10


//0x14

//0x16

//DSM-CC  0x02

//DSM-CC  0x06



