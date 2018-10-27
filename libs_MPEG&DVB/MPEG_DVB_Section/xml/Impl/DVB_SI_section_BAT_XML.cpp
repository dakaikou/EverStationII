#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/MPEG_DVB_Common_XML.h"
#include "../Include/DVB_SI_section_XML.h"
#include "../Include/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_BAT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, bouquet_association_section_t* pbat_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	char	 pszField[64];
	char	 pszComment[64];

	if ((pxmlDoc != NULL) && (pbat_section != NULL))
	{
		//根节点
		sprintf_s(pszField, sizeof(pszField), "bouquet_association_section(table_id=0x%02X)", pbat_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, pbat_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", pbat_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", pbat_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use0, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pbat_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", pbat_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "bouquet_id", pbat_section->bouquet_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pbat_section->reserved1, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", pbat_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", pbat_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", pbat_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", pbat_section->last_section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use1, 4, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "bouquet_descriptors_length", pbat_section->bouquet_descriptors_length, 12, "uimsbf", NULL);

		if (pbat_section->bouquet_descriptors_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "bouquet_descriptors()");
			XMLElement* pxmlBouquetDescriptorsNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, pbat_section->bouquet_descriptors_length);

			for (int descriptor_index = 0; descriptor_index < pbat_section->bouquet_descriptor_count; descriptor_index++)
			{
				descriptor_buf = pbat_section->bouquet_descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = pbat_section->bouquet_descriptors[descriptor_index].descriptor_tag;
				descriptor_length = pbat_section->bouquet_descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
				case DVB_SI_BOUQUET_NAME_DESCRIPTOR:
					DVB_SI_decode_bouquet_name_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlBouquetDescriptorsNode);
					break;
				case DVB_SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR:
					DVB_SI_decode_multilingual_bouquet_name_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlBouquetDescriptorsNode);
					break;
				case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
					DVB_SI_decode_private_data_specifier_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlBouquetDescriptorsNode);
					break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlBouquetDescriptorsNode, pbat_section->bouquet_descriptors + descriptor_index);
					break;
				}
			}
		}

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use2, 4, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_stream_loop_length", pbat_section->transport_stream_loop_length, 12, "uimsbf", NULL);

		if (pbat_section->transport_stream_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "transport_stream_loop(<共 %d 个流, %d字节>)", pbat_section->stream_count, pbat_section->transport_stream_loop_length);
			XMLElement* pxmlStreamsLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, pbat_section->transport_stream_loop_length);

			for (int stream_index = 0; stream_index < pbat_section->stream_count; stream_index++)
			{
				STREAM_DESCRIPTION_t*	pstStream = pbat_section->astStreams + stream_index;

				int stream_description_length = 6 + pstStream->transport_descriptors_length;
				sprintf_s(pszField, sizeof(pszField), "transport_stream(<TSID=0x%04X, ONetID=0x%04X, %d字节>)", pstStream->transport_stream_id, pstStream->original_network_id, stream_description_length);
				XMLElement* pxmlStreamNode = pxmlDoc->NewBranchElement(pxmlStreamsLoopNode, pszField, NULL, stream_description_length);

				pxmlDoc->NewElementForBits(pxmlStreamNode, "transport_stream_id", pstStream->transport_stream_id, 16, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlStreamNode, "original_network_id", pstStream->original_network_id, 16, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlStreamNode, "reserved", pstStream->reserved, 4, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlStreamNode, "transport_descriptors_length", pstStream->transport_descriptors_length, 12, "uimsbf", NULL);

				if (pstStream->transport_descriptors_length > 0)
				{
					sprintf_s(pszField, sizeof(pszField), "transport_descriptors(%d字节)", pstStream->transport_descriptors_length);
					XMLElement* pxmlTransportDescriptorNode = pxmlDoc->NewBranchElement(pxmlStreamNode, pszField, NULL, pstStream->transport_descriptors_length);

					for (int descriptor_index = 0; descriptor_index < pstStream->transport_descriptor_count; descriptor_index++)
					{
						descriptor_buf = pstStream->transport_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = pstStream->transport_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = pstStream->transport_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
						case DVB_SI_SERVICE_LIST_DESCRIPTOR:
							DVB_SI_decode_service_list_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlTransportDescriptorNode);
							break;
							//case DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:
							//	DVB_SI_decode_satellite_delivery_system_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
						case DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR:
							DVB_SI_decode_cable_delivery_system_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlTransportDescriptorNode);
							break;
						case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
							DVB_SI_decode_private_data_specifier_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlTransportDescriptorNode);
							break;
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlTransportDescriptorNode, pstStream->transport_descriptors + descriptor_index);
							break;
						}
					}
				}
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", pbat_section->CRC_32, 32, "rpchof", NULL);

		if (pbat_section->CRC_32_recalculated != pbat_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pbat_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}

int DVB_SI_BAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, bouquet_association_section_t* pBATSection)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	bouquet_association_section_t* pbat_section = (pBATSection != NULL) ? pBATSection : new bouquet_association_section_t;
	rtcode = DVB_SI_BAT_DecodeSection(section_buf, section_size, pbat_section);
	rtcode = DVB_SI_BAT_PresentSection_to_XML(pxmlDoc, pbat_section);

	if (pBATSection == NULL)
	{
		//说明pbat_section指针临时分配，函数返回前需要释放
		delete pbat_section;
	}

	return rtcode;
}

