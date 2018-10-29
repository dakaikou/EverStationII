#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/DVB_SI_Utilities.h"
#include "../Include/MPEG_DVB_Common_XML.h"
#include "../Include/DVB_SI_section_XML.h"
#include "../Include/DVB_SI_Descriptor_XML.h"

/////////////////////////////////////////////
int DVB_SI_SDT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, service_description_section_t* psdt_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	char	 pszField[128];
	char	 pszComment[128];
	//service_descriptor_t service_descriptor;

	SERVICE_DESCRIPTION_t*	pstServiceDescription;

	if ((pxmlDoc != NULL) && (psdt_section != NULL))
	{
		//根节点
		sprintf_s(pszField, sizeof(pszField), "service_description_section(table_id=0x%02X)", psdt_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, psdt_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", psdt_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", psdt_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", psdt_section->reserved_future_use0, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", psdt_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", psdt_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_stream_id", psdt_section->transport_stream_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", psdt_section->reserved1, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", psdt_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", psdt_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", psdt_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", psdt_section->last_section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "original_network_id", psdt_section->original_network_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", psdt_section->reserved_future_use1, 8, "bslbf", NULL);

		int service_loop_length = psdt_section->section_length - 12;
		if (service_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "业务循环(共 %d 个业务)", psdt_section->service_count);
			XMLElement* pxmlServiceLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, service_loop_length);

			for (int service_index = 0; service_index < psdt_section->service_count; service_index++)
			{
				pstServiceDescription = psdt_section->astServiceDescriptions + service_index;

				int service_description_length = 5 + pstServiceDescription->descriptors_loop_length;
				sprintf_s(pszField, sizeof(pszField), "业务(ID=0x%04X)", pstServiceDescription->service_id);
				XMLElement* pxmlServiceNode = pxmlDoc->NewBranchElement(pxmlServiceLoopNode, pszField, NULL, service_description_length);

				pxmlDoc->NewElementForBits(pxmlServiceNode, "service_id", pstServiceDescription->service_id, 16, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlServiceNode, "reserved_future_use", pstServiceDescription->reserved_future_use, 6, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlServiceNode, "EIT_schedule_flag", pstServiceDescription->EIT_schedule_flag, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlServiceNode, "EIT_present_following_flag", pstServiceDescription->EIT_present_following_flag, 1, "bslbf", NULL);

				DVB_SI_NumericCoding2Text_RunningStatus(pstServiceDescription->running_status, pszComment, sizeof(pszComment));
				pxmlDoc->NewElementForBits(pxmlServiceNode, "running_status", pstServiceDescription->running_status, 3, "uimsbf", pszComment);
				pxmlDoc->NewElementForBits(pxmlServiceNode, "free_CA_mode", pstServiceDescription->free_CA_mode, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlServiceNode, "descriptors_loop_length", pstServiceDescription->descriptors_loop_length, 12, "uimsbf", NULL);

				if (pstServiceDescription->descriptors_loop_length > 0)
				{
					sprintf_s(pszField, sizeof(pszField), "service_descriptors()");
					XMLElement* pxmlServiceDescriptorsLoopNode = pxmlDoc->NewBranchElement(pxmlServiceNode, pszField, NULL, pstServiceDescription->descriptors_loop_length);

					for (int descriptor_index = 0; descriptor_index < pstServiceDescription->service_descriptor_count; descriptor_index++)
					{
						descriptor_buf = pstServiceDescription->service_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = pstServiceDescription->service_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = pstServiceDescription->service_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
						case DVB_SI_BOUQUET_NAME_DESCRIPTOR:
							DVB_SI_decode_bouquet_name_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlServiceDescriptorsLoopNode);
							break;
						case DVB_SI_SERVICE_DESCRIPTOR:
							DVB_SI_decode_service_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlServiceDescriptorsLoopNode);
							break;
						case DVB_SI_CA_IDENTIFIER_DESCRIPTOR:
							DVB_SI_decode_CA_identifier_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlServiceDescriptorsLoopNode);
							break;
						case DVB_SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR:
							DVB_SI_decode_multilingual_service_name_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlServiceDescriptorsLoopNode);
							break;
						case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
							DVB_SI_decode_private_data_specifier_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlServiceDescriptorsLoopNode);
							break;
						case DVB_SI_DATA_BROADCAST_DESCRIPTOR:
							DVB_SI_decode_data_broadcast_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlServiceDescriptorsLoopNode);
							break;
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlServiceDescriptorsLoopNode, pstServiceDescription->service_descriptors + descriptor_index);
							break;
						}
					}
				}
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", psdt_section->CRC_32, 32, "rpchof", NULL);
		if (psdt_section->CRC_32_recalculated != psdt_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", psdt_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}


int DVB_SI_SDT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, service_description_section_t* pSDTSection)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	service_description_section_t* psdt_section = (pSDTSection != NULL) ? pSDTSection : new service_description_section_t;
	rtcode = DVB_SI_SDT_DecodeSection(section_buf, section_size, psdt_section);
	rtcode = DVB_SI_SDT_PresentSection_to_XML(pxmlDoc, psdt_section);

	if (pSDTSection == NULL)
	{
		//说明psdt_section指针临时分配，函数返回前需要释放
		delete psdt_section;
	}

	return rtcode;
}

