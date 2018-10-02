#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/DVB_SI_section_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_BAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, bouquet_association_section_t* pBATSection)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	char	 pszField[64];
	char	 pszComment[64];

	STREAM_DESCRIPTION_t*	pstStream;

	bouquet_association_section_t* pbat_section = (pBATSection != NULL) ? pBATSection : new bouquet_association_section_t;
	rtcode = DVB_SI_BAT_DecodeSection(section_buf, section_size, pbat_section);

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "bouquet_association_section()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, section_size);

		sprintf_s(pszComment, sizeof(pszComment), "%d字节", section_size);
		XMLNODE_SetAttribute(pxmlRootNode, "comment", pszComment);

		if (rtcode != SECTION_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pbat_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pbat_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use0, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pbat_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", pbat_section->section_length, 12, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "bouquet_id", pbat_section->bouquet_id, 16, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pbat_section->reserved1, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pbat_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pbat_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pbat_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pbat_section->last_section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use1, 4, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "bouquet_descriptors_length", pbat_section->bouquet_descriptors_length, 12, "uimsbf", NULL);

		if (pbat_section->bouquet_descriptors_length > 0)
		{
			XMLElement* pxmlBouquetDescriptorsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "bouquet_descriptors()", NULL);
			XMLNODE_SetFieldLength(pxmlBouquetDescriptorsNode, pbat_section->bouquet_descriptors_length);

			for (int descriptor_index = 0; descriptor_index < pbat_section->bouquet_descriptor_count; descriptor_index++)
			{
				descriptor_buf = pbat_section->bouquet_descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = pbat_section->bouquet_descriptors[descriptor_index].descriptor_tag;
				descriptor_length = pbat_section->bouquet_descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
					//case DVB_SI_BOUQUET_NAME_DESCRIPTOR:
					//	DVB_SI_decode_bouquet_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case DVB_SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR:
					//	DVB_SI_decode_multilingual_bouquet_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlBouquetDescriptorsNode, pbat_section->bouquet_descriptors + descriptor_index);
					break;
				}
			}
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use2, 4, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_stream_loop_length", pbat_section->transport_stream_loop_length, 12, "uimsbf", NULL);

		if (pbat_section->transport_stream_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "transport_stream_loop(共 %d 个流)", pbat_section->stream_count);
			XMLElement* pxmlStreamsLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlStreamsLoopNode, pbat_section->transport_stream_loop_length);

			for (int stream_index = 0; stream_index < pbat_section->stream_count; stream_index++)
			{
				STREAM_DESCRIPTION_t*	pstStream = pbat_section->astStreams + stream_index;

				sprintf_s(pszField, sizeof(pszField), "transport_stream(TSID=0x%04X, ONetID=0x%04X)", pstStream->transport_stream_id, pstStream->original_network_id);
				XMLElement* pxmlStreamNode = XMLDOC_NewElementForString(pxmlDoc, pxmlStreamsLoopNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlStreamNode, 6 + pstStream->transport_descriptors_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "transport_stream_id", pstStream->transport_stream_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "original_network_id", pstStream->original_network_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "reserved", pstStream->reserved, 4, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlStreamNode, "transport_descriptors_length", pstStream->transport_descriptors_length, 12, "uimsbf", NULL);

				if (pstStream->transport_descriptors_length > 0)
				{
					XMLElement* pxmlTransportDescriptorNode = XMLDOC_NewElementForString(pxmlDoc, pxmlStreamNode, "transport_descriptors()", NULL);
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

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", pbat_section->CRC_32, 32, "rpchof", NULL);

		if (pbat_section->CRC_32_recalculated != pbat_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pbat_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	if (pBATSection == NULL)
	{
		//说明pbat_section指针临时分配，函数返回前需要释放
		delete pbat_section;
	}

	return rtcode;
}

