#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/DVB_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"
#include "../nativeInclude/DVB_SI_Utilities.h"
#include "../xmlInclude/MPEG_DVB_Common_XML.h"
#include "../xmlInclude/DVB_SI_section_XML.h"
#include "../xmlInclude/DVB_SI_Descriptor_XML.h"

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
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		sprintf_s(pszField, sizeof(pszField), "service_description_section(table_id=0x%02X)", psdt_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, psdt_section->section_length + 3);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", psdt_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", psdt_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", psdt_section->reserved_future_use0, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", psdt_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", psdt_section->section_length, 12, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_stream_id", psdt_section->transport_stream_id, 16, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", psdt_section->reserved1, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", psdt_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", psdt_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", psdt_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", psdt_section->last_section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "original_network_id", psdt_section->original_network_id, 16, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", psdt_section->reserved_future_use1, 8, "bslbf", NULL);

		int service_loop_length = psdt_section->section_length - 12;
		if (service_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "业务循环(共 %d 个业务)", psdt_section->service_count);
			XMLElement* pxmlServiceLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlServiceLoopNode, service_loop_length);

			for (int service_index = 0; service_index < psdt_section->service_count; service_index++)
			{
				pstServiceDescription = psdt_section->astServiceDescriptions + service_index;

				int service_description_length = 5 + pstServiceDescription->descriptors_loop_length;
				sprintf_s(pszField, sizeof(pszField), "业务(ID=0x%04X)", pstServiceDescription->service_id);
				XMLElement* pxmlServiceNode = XMLDOC_NewElementForString(pxmlDoc, pxmlServiceLoopNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlServiceNode, service_description_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "service_id", pstServiceDescription->service_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "reserved_future_use", pstServiceDescription->reserved_future_use, 6, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "EIT_schedule_flag", pstServiceDescription->EIT_schedule_flag, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "EIT_present_following_flag", pstServiceDescription->EIT_present_following_flag, 1, "bslbf", NULL);

				DVB_SI_NumericCoding2Text_RunningStatus(pstServiceDescription->running_status, pszComment, sizeof(pszComment));
				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "running_status", pstServiceDescription->running_status, 3, "uimsbf", pszComment);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "free_CA_mode", pstServiceDescription->free_CA_mode, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlServiceNode, "descriptors_loop_length", pstServiceDescription->descriptors_loop_length, 12, "uimsbf", NULL);

				if (pstServiceDescription->descriptors_loop_length > 0)
				{
					sprintf_s(pszField, sizeof(pszField), "service_descriptors()");
					XMLElement* pxmlServiceDescriptorsLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlServiceNode, pszField, NULL);
					XMLNODE_SetFieldLength(pxmlServiceDescriptorsLoopNode, pstServiceDescription->descriptors_loop_length);

					for (int descriptor_index = 0; descriptor_index < pstServiceDescription->service_descriptor_count; descriptor_index++)
					{
						descriptor_buf = pstServiceDescription->service_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = pstServiceDescription->service_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = pstServiceDescription->service_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
							//case DVB_SI_BOUQUET_NAME_DESCRIPTOR:
							//	DVB_SI_decode_bouquet_name_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_SERVICE_DESCRIPTOR:
							//	DVB_SI_decode_service_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode, &service_descriptor);
							//	break;
							//case DVB_SI_CA_IDENTIFIER_DESCRIPTOR:
							//	DVB_SI_decode_CA_identifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR:
							//	DVB_SI_decode_multilingual_service_name_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
							//	DVB_SI_decode_private_data_specifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_DATA_BROADCAST_DESCRIPTOR:
							//	DVB_SI_decode_data_broadcast_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlServiceDescriptorsLoopNode, pstServiceDescription->service_descriptors + descriptor_index);
							break;
						}
					}
				}
			}

			//sprintf_s(pszComment, sizeof(pszComment), "ID=0x%04X, %s", pServiceDescription->service_id, service_descriptor.trimmed_service_name);
			//pxmlServiceNode->SetAttribute("comment", pszTemp);
		}

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", psdt_section->CRC_32, 32, "rpchof", NULL);
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

