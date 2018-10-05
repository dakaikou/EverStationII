#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/DVB_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "../xmlInclude/MPEG_DVB_Common_XML.h"
#include "../xmlInclude/DVB_SI_section_XML.h"
#include "../xmlInclude/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_NIT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, network_information_section_t* pnit_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	char	 pszField[128];
	char	 pszComment[128];

	if ((pxmlDoc != NULL) && (pnit_section != NULL))
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		sprintf_s(pszField, sizeof(pszField), "network_information_section(table_id=0x%02X)", pnit_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, pnit_section->section_length + 3);

		//if (rtcode != SECTION_PARSE_NO_ERROR)
		//{
		//	sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
		//	XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		//}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pnit_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pnit_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pnit_section->reserved_future_use0, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pnit_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", pnit_section->section_length, 12, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "network_id", pnit_section->network_id, 16, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pnit_section->reserved1, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pnit_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pnit_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pnit_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pnit_section->last_section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pnit_section->reserved_future_use1, 4, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "network_descriptors_length", pnit_section->network_descriptors_length, 12, "uimsbf", NULL);

		if (pnit_section->network_descriptors_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "network_descriptors()");
			XMLElement* pxmlNetworkDescriptorsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlNetworkDescriptorsNode, pnit_section->network_descriptors_length);

			for (int descriptor_index = 0; descriptor_index < pnit_section->network_descriptor_count; descriptor_index++)
			{
				descriptor_buf = pnit_section->network_descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = pnit_section->network_descriptors[descriptor_index].descriptor_tag;
				descriptor_length = pnit_section->network_descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
					//case DVB_SI_NETWORK_NAME_DESCRIPTOR:
					//	DVB_SI_decode_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:
					//	DVB_SI_decode_multilingual_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlNetworkDescriptorsNode, pnit_section->network_descriptors + descriptor_index);
					break;
				}
			}
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pnit_section->reserved_future_use2, 4, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_stream_loop_length", pnit_section->transport_stream_loop_length, 12, "uimsbf", NULL);

		if (pnit_section->transport_stream_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "transport_stream_loop(共 %d 个流)", pnit_section->stream_count);
			XMLElement* pxmlStreamsLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlStreamsLoopNode, pnit_section->transport_stream_loop_length);

			for (int stream_index = 0; stream_index < pnit_section->stream_count; stream_index++)
			{
				STREAM_DESCRIPTION_t*	pstStream = pnit_section->astStreams + stream_index;

				int stream_description_length = 6 + pstStream->transport_descriptors_length;
				sprintf_s(pszField, sizeof(pszField), "transport_stream(<TSID=0x%04X, ONetID=0x%04X>)", pstStream->transport_stream_id, pstStream->original_network_id);
				XMLElement* pxmlStreamNode = XMLDOC_NewElementForString(pxmlDoc, pxmlStreamsLoopNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlStreamNode, stream_description_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "transport_stream_id", pstStream->transport_stream_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "original_network_id", pstStream->original_network_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "reserved", pstStream->reserved, 4, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "transport_descriptors_length", pstStream->transport_descriptors_length, 12, "uimsbf", NULL);

				if (pstStream->transport_descriptors_length > 0)
				{
					sprintf_s(pszField, sizeof(pszField), "transport_descriptors()");
					XMLElement* pxmlTransportDescriptorNode = XMLDOC_NewElementForString(pxmlDoc, pxmlStreamNode, pszField, NULL);
					XMLNODE_SetFieldLength(pxmlTransportDescriptorNode, pstStream->transport_descriptors_length);

					for (int descriptor_index = 0; descriptor_index < pstStream->transport_descriptor_count; descriptor_index++)
					{
						descriptor_buf = pstStream->transport_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = pstStream->transport_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = pstStream->transport_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
							//case DVB_SI_SERVICE_LIST_DESCRIPTOR:
							//	DVB_SI_decode_service_list_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:
							//	DVB_SI_decode_satellite_delivery_system_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR:
							//	DVB_SI_decode_cable_delivery_system_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
							//	DVB_SI_decode_private_data_specifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlTransportDescriptorNode, pstStream->transport_descriptors + descriptor_index);
							break;
						}
					}
				}
			}
		}

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", pnit_section->CRC_32, 32, "rpchof", NULL);
		if (pnit_section->CRC_32_recalculated != pnit_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pnit_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}

int DVB_SI_NIT_DecodeSection_to_XML(uint8_t* section_buf, int section_size, HALForXMLDoc* pxmlDoc, network_information_section_t* pNITSection)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	network_information_section_t* pnit_section = (pNITSection != NULL) ? pNITSection : new network_information_section_t;
	rtcode = DVB_SI_NIT_DecodeSection(section_buf, section_size, pnit_section);
	rtcode = DVB_SI_NIT_PresentSection_to_XML(pxmlDoc, pnit_section);

	if (pNITSection == NULL)
	{
		//说明pnit_section指针临时分配，函数返回前需要释放
		delete pnit_section;
	}

	return rtcode;
}
