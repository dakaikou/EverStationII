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
	char pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pbouquet_name_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "bouquet_name_descriptor(tag: 0x%02X)", pbouquet_name_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pbouquet_name_descriptor->trimmed_bouquet_name, pbouquet_name_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pbouquet_name_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pbouquet_name_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pbouquet_name_descriptor->descriptor_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "bouquet_name[ ]", (uint8_t*)pbouquet_name_descriptor->bouquet_name,
				pbouquet_name_descriptor->descriptor_length, pbouquet_name_descriptor->trimmed_bouquet_name);
		}
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
	char  pszField[48];
	char  pszSrvType[48];
	char  pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pservice_descriptor != NULL))
	{
		DVB_SI_NumericCoding2Text_ServiceType(pservice_descriptor->service_type, pszSrvType, sizeof(pszSrvType));
		sprintf_s(pszField, sizeof(pszField), "service_descriptor(tag: 0x%02X)", pservice_descriptor->descriptor_tag);
		sprintf_s(pszComment, sizeof(pszComment), "Type:%s, Name:%s, Provider:%s", pszSrvType, pservice_descriptor->trimmed_service_name, pservice_descriptor->trimmed_service_provider_name);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pservice_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pservice_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pservice_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "service_type", pservice_descriptor->service_type, 8, "uimsbf", pszSrvType);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "service_provider_name_length", pservice_descriptor->service_provider_name_length, 8, "uimsbf", NULL);

		if (pservice_descriptor->service_provider_name_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "service_provider_name", (uint8_t*)pservice_descriptor->service_provider_name, pservice_descriptor->service_provider_name_length,
				pservice_descriptor->trimmed_service_provider_name);
		}

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "service_name_length", pservice_descriptor->service_name_length, 8, "uimsbf", NULL);

		if (pservice_descriptor->service_name_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "service_name", (uint8_t*)pservice_descriptor->service_name, pservice_descriptor->service_name_length,
				pservice_descriptor->trimmed_service_name);
		}
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
	char pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pshort_event_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "short_event_descriptor(tag: 0x%02X)", pshort_event_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pshort_event_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pshort_event_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pshort_event_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "ISO_639_language_code[ ]", (uint8_t*)pshort_event_descriptor->ISO_639_language_code_char, 3, pshort_event_descriptor->ISO_639_language_code_char);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "event_name_length", pshort_event_descriptor->event_name_length, 8, "uimsbf", NULL);

		if (pshort_event_descriptor->event_name_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "event_name_char[ ]", (uint8_t*)pshort_event_descriptor->event_name_char, pshort_event_descriptor->event_name_length,
				pshort_event_descriptor->trimmed_event_name_char);
		}

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "text_length", pshort_event_descriptor->text_length, 8, "uimsbf", NULL);

		if (pshort_event_descriptor->text_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "text_char[ ]", (uint8_t*)pshort_event_descriptor->text_char, pshort_event_descriptor->text_length,
				pshort_event_descriptor->trimmed_text_char);
		}
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
int DVB_SI_decode_stream_identifier_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, stream_identifier_descriptor_t* pStreamIdentifierDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		stream_identifier_descriptor_t* pstream_identifier_descriptor = (pStreamIdentifierDescriptor == NULL) ? new stream_identifier_descriptor_t : pStreamIdentifierDescriptor;
		rtcode = DVB_SI_decode_stream_identifier_descriptor(buf, length, pstream_identifier_descriptor);
		DVB_SI_present_stream_identifier_descriptor_to_xml(pxmlDoc, pxmlParentNode, pstream_identifier_descriptor);

		if (pStreamIdentifierDescriptor == NULL)
		{
			delete pstream_identifier_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_stream_identifier_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, stream_identifier_descriptor_t* pstream_identifier_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pstream_identifier_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "stream_identifier_descriptor(tag: 0x%02X)", pstream_identifier_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pstream_identifier_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pstream_identifier_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pstream_identifier_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "component_tag", pstream_identifier_descriptor->component_tag, 8, "uimsbf", NULL);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解CA标识描述子				0x53
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_CA_identifier_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, CA_identifier_descriptor_t* pCAIdentifierDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		CA_identifier_descriptor_t* pCA_identifier_descriptor = (pCAIdentifierDescriptor == NULL) ? new CA_identifier_descriptor_t : pCAIdentifierDescriptor;

		rtcode = DVB_SI_decode_CA_identifier_descriptor(buf, length, pCA_identifier_descriptor);
		DVB_SI_present_CA_identifier_descriptor_to_xml(pxmlDoc, pxmlParentNode, pCA_identifier_descriptor);

		if (pCAIdentifierDescriptor == NULL)
		{
			delete pCA_identifier_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_CA_identifier_descriptor_to_xml(HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, CA_identifier_descriptor_t* pCA_identifier_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];
	char	pszCAVendor[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pCA_identifier_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "CA_identifier_descriptor(tag: 0x%02X)", pCA_identifier_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pCA_identifier_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pCA_identifier_descriptor->descriptor_tag, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pCA_identifier_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pCA_identifier_descriptor->descriptor_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "CA_system_loop(共 %d个)", pCA_identifier_descriptor->N);
			XMLElement* pxmlLoopNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, NULL, pCA_identifier_descriptor->descriptor_length);

			for (int i = 0; i < pCA_identifier_descriptor->N; i++)
			{
				//sprintf_s(pszField, sizeof(pszField), "CA_system[%d](ID: 0x%04X)", i, pCA_identifier_descriptor->CA_system_ID[i]);
				MPEG_DVB_NumericCoding2Text_CASystemID(pCA_identifier_descriptor->CA_system_ID[i], pszCAVendor, sizeof(pszCAVendor));

				//XMLElement* pxmlCANode = pxmlDoc->NewBranchElement(pxmlLoopNode, pszField, pszCAVendor, 2);

				pxmlDoc->NewElementForBits(pxmlLoopNode, "CA_system_ID", pCA_identifier_descriptor->CA_system_ID[i], 16, "uimsbf", pszCAVendor);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解内容描述子				0x54
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_content_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, content_descriptor_t* pContentDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		content_descriptor_t* pcontent_descriptor = (pContentDescriptor == NULL) ? new content_descriptor_t : pContentDescriptor;
		rtcode = DVB_SI_decode_content_descriptor(buf, length, pcontent_descriptor);
		DVB_SI_present_content_descriptor_to_xml(pxmlDoc, pxmlParentNode, pcontent_descriptor);

		if (pContentDescriptor == NULL)
		{
			delete pcontent_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_content_descriptor_to_xml(HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, content_descriptor_t* pcontent_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[32];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pcontent_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "content_descriptor(tag: 0x%02X)", pcontent_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pcontent_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pcontent_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pcontent_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pcontent_descriptor->descriptor_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "content_nibble_loop(共 %d个)", pcontent_descriptor->N);
			XMLElement* pxmlLoopNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, NULL, pcontent_descriptor->descriptor_length);

			for (int content_index = 0; content_index < pcontent_descriptor->N; content_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "content nibble[%d]()", content_index);
				XMLElement* pxmlNibbleNode = pxmlDoc->NewBranchElement(pxmlLoopNode, pszField, NULL, 2);

				pxmlDoc->NewElementForBits(pxmlNibbleNode, "content_nibble_level_1", pcontent_descriptor->content_nibble_level_1[content_index], 4, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlNibbleNode, "content_nibble_level_2", pcontent_descriptor->content_nibble_level_2[content_index], 4, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlNibbleNode, "user_nibble_1", pcontent_descriptor->user_nibble_1[content_index], 4, "uimsbf", NULL);
				pxmlDoc->NewElementForBits(pxmlNibbleNode, "user_nibble_2", pcontent_descriptor->user_nibble_2[content_index], 4, "uimsbf", NULL);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解家长级别描述子				0x55
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_parental_rating_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, parental_rating_descriptor_t* pParentalRatingDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		parental_rating_descriptor_t* pparental_rating_descriptor = (pParentalRatingDescriptor == NULL) ? new parental_rating_descriptor_t : pParentalRatingDescriptor;

		rtcode = DVB_SI_decode_parental_rating_descriptor(buf, length, pparental_rating_descriptor);
		DVB_SI_present_parental_rating_descriptor_to_xml(pxmlDoc, pxmlParentNode, pparental_rating_descriptor);

		if (pParentalRatingDescriptor == NULL)
		{
			delete pparental_rating_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_parental_rating_descriptor_to_xml(HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, parental_rating_descriptor_t* pparental_rating_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pparental_rating_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "parental_rating_descriptor(tag: 0x%02X)", pparental_rating_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pparental_rating_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pparental_rating_descriptor->descriptor_tag, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pparental_rating_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pparental_rating_descriptor->descriptor_length > 0)
		{
			//uint8_t* old_loop_ptr = bs.p_cur;
			sprintf_s(pszField, sizeof(pszField), "parental_rating_loop(共 %d个)", pparental_rating_descriptor->N);
			XMLElement* pxmlLoopNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, NULL, pparental_rating_descriptor->descriptor_length);

			for (int country_index = 0; country_index < pparental_rating_descriptor->N; country_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "parental rating[%d]()", country_index);
				XMLElement* pxmlRatingNode = pxmlDoc->NewBranchElement(pxmlLoopNode, pszField, NULL, 4);

				pxmlDoc->NewElementForByteBuf(pxmlRatingNode, "country_code[ ]", (uint8_t*)pparental_rating_descriptor->country_code_char[country_index], 3, 
					pparental_rating_descriptor->country_code_char[country_index]);

				pxmlDoc->NewElementForBits(pxmlRatingNode, "rating", pparental_rating_descriptor->rating[country_index], 8, "uimsbf", NULL);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

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

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		local_time_offset_descriptor_t* plocal_time_offset_descriptor = (pLocalTimeOffsetDescriptor == NULL) ? new local_time_offset_descriptor_t : pLocalTimeOffsetDescriptor;

		rtcode = DVB_SI_decode_local_time_offset_descriptor(buf, length, plocal_time_offset_descriptor);
		DVB_SI_present_local_time_offset_descriptor_to_xml(pxmlDoc, pxmlParentNode, plocal_time_offset_descriptor);

		if (pLocalTimeOffsetDescriptor == NULL)
		{
			delete plocal_time_offset_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_local_time_offset_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, local_time_offset_descriptor_t* plocal_time_offset_descriptor)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[48];
	char   pszComment[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (plocal_time_offset_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "local_time_offset_descriptor(tag: 0x%02X)", plocal_time_offset_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, plocal_time_offset_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", plocal_time_offset_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", plocal_time_offset_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (plocal_time_offset_descriptor->descriptor_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "本地时间偏移循环(共 %d个)", plocal_time_offset_descriptor->N);
			XMLElement* pxmlLoopsNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, NULL, plocal_time_offset_descriptor->descriptor_length);

			for (int local_index = 0; local_index < plocal_time_offset_descriptor->N; local_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "本地时间偏移[%d]()", local_index);
				XMLElement* pxmlLocalNode = pxmlDoc->NewBranchElement(pxmlLoopsNode, pszField, plocal_time_offset_descriptor->st[local_index].country_code_char, 13);

				pxmlDoc->NewElementForByteBuf(pxmlLocalNode, "country_code[ ]", (uint8_t*)plocal_time_offset_descriptor->st[local_index].country_code_char, 3,
					plocal_time_offset_descriptor->st[local_index].country_code_char);

				pxmlDoc->NewElementForBits(pxmlLocalNode, "country_region_id", plocal_time_offset_descriptor->st[local_index].country_region_id, 6, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlLocalNode, "reserved", plocal_time_offset_descriptor->st[local_index].reserved, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlLocalNode, "local_time_offset_polarity", plocal_time_offset_descriptor->st[local_index].local_time_offset_polarity, 1, "bslbf", NULL);

				pxmlDoc->NewElementForBits(pxmlLocalNode, "local_time_offset", plocal_time_offset_descriptor->st[local_index].local_time_offset, 16, "bslbf", NULL);

				DVB_SI_NumericCoding2Text_UTCTime(plocal_time_offset_descriptor->st[local_index].time_of_change, pszComment, sizeof(pszComment));
				pxmlDoc->NewElementForX64Bits(pxmlLocalNode, "time_of_change", plocal_time_offset_descriptor->st[local_index].time_of_change, 40, "bslbf", pszComment);

				DVB_SI_NumericCoding2Text_BCDTime(plocal_time_offset_descriptor->st[local_index].next_time_offset, pszComment, sizeof(pszComment));
				pxmlDoc->NewElementForBits(pxmlLocalNode, "next_time_offset", plocal_time_offset_descriptor->st[local_index].next_time_offset, 16, "bslbf", pszComment);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

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
int DVB_SI_decode_multilingual_network_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_network_name_descriptor_t* pMultilingualNetworkNameDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		multilingual_network_name_descriptor_t* pmultilingual_network_name_descriptor = (pMultilingualNetworkNameDescriptor == NULL) ? new multilingual_network_name_descriptor_t : pMultilingualNetworkNameDescriptor;

		rtcode = DVB_SI_decode_multilingual_network_name_descriptor(buf, length, pmultilingual_network_name_descriptor);
		DVB_SI_present_multilingual_network_name_descriptor_to_xml(pxmlDoc, pxmlParentNode, pmultilingual_network_name_descriptor);

		if (pMultilingualNetworkNameDescriptor == NULL)
		{
			delete pmultilingual_network_name_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_multilingual_network_name_descriptor_to_xml(HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_network_name_descriptor_t* pmultilingual_network_name_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];
	char   pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmultilingual_network_name_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "multilingual_network_name_descriptor(tag: 0x%02X)", pmultilingual_network_name_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pmultilingual_network_name_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pmultilingual_network_name_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pmultilingual_network_name_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pmultilingual_network_name_descriptor->descriptor_length > 0)
		{
			for (int lang_index = 0; lang_index < pmultilingual_network_name_descriptor->N; lang_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "language[%d]", lang_index);
				sprintf_s(pszComment, sizeof(pszComment), "%s - %s", pmultilingual_network_name_descriptor->st[lang_index].ISO_639_language_code_char, pmultilingual_network_name_descriptor->st[lang_index].trimmed_network_name_char);
				XMLElement* pxmlLangNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, pszComment, 3 + 1 + pmultilingual_network_name_descriptor->st[lang_index].network_name_length);

				pxmlDoc->NewElementForByteBuf(pxmlLangNode, "ISO_639_language_code[ ]", (uint8_t*)pmultilingual_network_name_descriptor->st[lang_index].ISO_639_language_code_char,
					3, pmultilingual_network_name_descriptor->st[lang_index].ISO_639_language_code_char);

				pxmlDoc->NewElementForBits(pxmlLangNode, "network_name_length", pmultilingual_network_name_descriptor->st[lang_index].network_name_length, 8, "uimsbf", NULL);

				if (pmultilingual_network_name_descriptor->st[lang_index].network_name_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlLangNode, "network_name[ ]", (uint8_t*)pmultilingual_network_name_descriptor->st[lang_index].network_name_char,
						pmultilingual_network_name_descriptor->st[lang_index].network_name_length, pmultilingual_network_name_descriptor->st[lang_index].trimmed_network_name_char);
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解多语言业务群名称描述子			0x5C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_bouquet_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_bouquet_name_descriptor_t* pMultilingualBouquetNameDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		multilingual_bouquet_name_descriptor_t* pmultilingual_bouquet_name_descriptor = (pMultilingualBouquetNameDescriptor == NULL) ? new multilingual_bouquet_name_descriptor_t : pMultilingualBouquetNameDescriptor;

		rtcode = DVB_SI_decode_multilingual_bouquet_name_descriptor(buf, length, pmultilingual_bouquet_name_descriptor);
		DVB_SI_present_multilingual_bouquet_name_descriptor_to_xml(pxmlDoc, pxmlParentNode, pmultilingual_bouquet_name_descriptor);

		if (pMultilingualBouquetNameDescriptor == NULL)
		{
			delete pmultilingual_bouquet_name_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_multilingual_bouquet_name_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_bouquet_name_descriptor_t* pmultilingual_bouquet_name_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];
	char   pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmultilingual_bouquet_name_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "multilingual_bouquet_name_descriptor(tag: 0x%02X)", pmultilingual_bouquet_name_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pmultilingual_bouquet_name_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pmultilingual_bouquet_name_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pmultilingual_bouquet_name_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pmultilingual_bouquet_name_descriptor->descriptor_length > 0)
		{
			for (int lang_index = 0; lang_index < pmultilingual_bouquet_name_descriptor->N; lang_index ++)
			{
				sprintf_s(pszField, sizeof(pszField), "language[%d]", lang_index);
				sprintf_s(pszComment, sizeof(pszComment), "%s - %s", pmultilingual_bouquet_name_descriptor->st[lang_index].ISO_639_language_code_char, pmultilingual_bouquet_name_descriptor->st[lang_index].trimmed_bouquet_name_char);
				XMLElement* pxmlLangNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, pszComment, 3 + 1 + pmultilingual_bouquet_name_descriptor->st[lang_index].bouquet_name_length);

				pxmlDoc->NewElementForByteBuf(pxmlLangNode, "ISO_639_language_code[ ]", (uint8_t*)pmultilingual_bouquet_name_descriptor->st[lang_index].ISO_639_language_code_char,
					3, pmultilingual_bouquet_name_descriptor->st[lang_index].ISO_639_language_code_char);

				pxmlDoc->NewElementForBits(pxmlLangNode, "bouquet_name_length", pmultilingual_bouquet_name_descriptor->st[lang_index].bouquet_name_length, 8, "uimsbf", NULL);

				if (pmultilingual_bouquet_name_descriptor->st[lang_index].bouquet_name_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlLangNode, "bouquet_name[ ]", (uint8_t*)pmultilingual_bouquet_name_descriptor->st[lang_index].bouquet_name_char,
						pmultilingual_bouquet_name_descriptor->st[lang_index].bouquet_name_length, pmultilingual_bouquet_name_descriptor->st[lang_index].trimmed_bouquet_name_char);
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解多语言业务名称描述子				0x5D
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_service_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_service_name_descriptor_t* pMultilingualServiceNameDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		multilingual_service_name_descriptor_t* pmultilingual_service_name_descriptor = (pMultilingualServiceNameDescriptor == NULL) ? new multilingual_service_name_descriptor_t : pMultilingualServiceNameDescriptor;

		rtcode = DVB_SI_decode_multilingual_service_name_descriptor(buf, length, pmultilingual_service_name_descriptor);
		DVB_SI_present_multilingual_service_name_descriptor_to_xml(pxmlDoc, pxmlParentNode, pmultilingual_service_name_descriptor);

		if (pMultilingualServiceNameDescriptor == NULL)
		{
			delete pmultilingual_service_name_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_multilingual_service_name_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_service_name_descriptor_t* pmultilingual_service_name_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];
	char   pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmultilingual_service_name_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "multilingual_service_name_descriptor(tag: 0x%02X)", pmultilingual_service_name_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pmultilingual_service_name_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pmultilingual_service_name_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pmultilingual_service_name_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pmultilingual_service_name_descriptor->descriptor_length > 0)
		{
			for (int lang_index = 0; lang_index < pmultilingual_service_name_descriptor->N; lang_index ++)
			{
				sprintf_s(pszField, sizeof(pszField), "language[%d]", lang_index);
				sprintf_s(pszComment, sizeof(pszComment), "%s - Name:%s, Provider:%s", pmultilingual_service_name_descriptor->st[lang_index].ISO_639_language_code_char, 
					pmultilingual_service_name_descriptor->st[lang_index].trimmed_service_name_char,
					pmultilingual_service_name_descriptor->st[lang_index].trimmed_service_provider_name_char);

				XMLElement* pxmlLangNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, pszComment, 
					3 + 1 + pmultilingual_service_name_descriptor->st[lang_index].service_provider_name_length + 1 + pmultilingual_service_name_descriptor->st[lang_index].service_name_length);

				pxmlDoc->NewElementForByteBuf(pxmlLangNode, "ISO_639_language_code[ ]", (uint8_t*)pmultilingual_service_name_descriptor->st[lang_index].ISO_639_language_code_char,
					3, pmultilingual_service_name_descriptor->st[lang_index].ISO_639_language_code_char);

				pxmlDoc->NewElementForBits(pxmlLangNode, "service_provider_name_length", pmultilingual_service_name_descriptor->st[lang_index].service_provider_name_length, 8, "uimsbf", NULL);

				if (pmultilingual_service_name_descriptor->st[lang_index].service_provider_name_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlLangNode, "service_provider_name[ ]", (uint8_t*)pmultilingual_service_name_descriptor->st[lang_index].service_provider_name_char,
						pmultilingual_service_name_descriptor->st[lang_index].service_provider_name_length, pmultilingual_service_name_descriptor->st[lang_index].trimmed_service_provider_name_char);
				}

				pxmlDoc->NewElementForBits(pxmlLangNode, "service_name_length", pmultilingual_service_name_descriptor->st[lang_index].service_name_length, 8, "uimsbf", NULL);

				if (pmultilingual_service_name_descriptor->st[lang_index].service_name_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlLangNode, "service_name[ ]", (uint8_t*)pmultilingual_service_name_descriptor->st[lang_index].service_name_char,
						pmultilingual_service_name_descriptor->st[lang_index].service_name_length, pmultilingual_service_name_descriptor->st[lang_index].trimmed_service_name_char);
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解多语言组成描述子					0x5E
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_multilingual_component_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_component_descriptor_t* pMultilingualComponentDescriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		multilingual_component_descriptor_t* pmultilingual_component_descriptor = (pMultilingualComponentDescriptor == NULL) ? new multilingual_component_descriptor_t : pMultilingualComponentDescriptor;

		rtcode = DVB_SI_decode_multilingual_component_descriptor(buf, length, pmultilingual_component_descriptor);
		DVB_SI_present_multilingual_component_descriptor_to_xml(pxmlDoc, pxmlParentNode, pmultilingual_component_descriptor);

		if (pMultilingualComponentDescriptor == NULL)
		{
			delete pmultilingual_component_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int DVB_SI_present_multilingual_component_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multilingual_component_descriptor_t* pmultilingual_component_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[64];
	char   pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmultilingual_component_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "multilingual_component_descriptor(tag: 0x%02X)", pmultilingual_component_descriptor->descriptor_tag);
		sprintf_s(pszComment, sizeof(pszComment), "component_tag: 0x%02X", pmultilingual_component_descriptor->component_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pmultilingual_component_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pmultilingual_component_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pmultilingual_component_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "component_tag", pmultilingual_component_descriptor->component_tag, 8, "uimsbf", NULL);

		if (pmultilingual_component_descriptor->descriptor_length > 1)
		{
			for (int lang_index = 0; lang_index < pmultilingual_component_descriptor->N; lang_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "language[%d]", lang_index);
				sprintf_s(pszComment, sizeof(pszComment), "%s - %s", pmultilingual_component_descriptor->st[lang_index].ISO_639_language_code_char, pmultilingual_component_descriptor->st[lang_index].text_char);
				XMLElement* pxmlLangNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, pszComment, 3 + 1 + pmultilingual_component_descriptor->st[lang_index].text_description_length);

				pxmlDoc->NewElementForByteBuf(pxmlLangNode, "ISO_639_language_code[ ]", (uint8_t*)pmultilingual_component_descriptor->st[lang_index].ISO_639_language_code_char,
					3, pmultilingual_component_descriptor->st[lang_index].ISO_639_language_code_char);

				pxmlDoc->NewElementForBits(pxmlLangNode, "text_description_length", pmultilingual_component_descriptor->st[lang_index].text_description_length, 8, "uimsbf", NULL);

				if (pmultilingual_component_descriptor->st[lang_index].text_description_length > 0)
				{
					pxmlDoc->NewElementForByteBuf(pxmlLangNode, "text_char[ ]", (uint8_t*)pmultilingual_component_descriptor->st[lang_index].text_char,
						pmultilingual_component_descriptor->st[lang_index].text_description_length, pmultilingual_component_descriptor->st[lang_index].text_char);
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

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



