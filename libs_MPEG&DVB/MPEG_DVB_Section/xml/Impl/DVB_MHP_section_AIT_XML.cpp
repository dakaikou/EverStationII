#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_MHP_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../Include/MPEG_DVB_Common_XML.h"
#include "../Include/DVB_MHP_section_XML.h"
#include "../Include/DVB_MHP_Descriptor_XML.h"

/////////////////////////////////////////////
int DVB_MHP_AIT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, application_information_section_t* pAITSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	application_information_section_t* pait_section = (pAITSection != NULL) ? pAITSection : new application_information_section_t;
	rtcode = DVB_MHP_AIT_DecodeSection(section_buf, section_size, pait_section);
	rtcode = DVB_MHP_AIT_PresentSection_to_XML(pxmlDoc, pait_section);

	if (pAITSection == NULL)
	{
		//说明pdsmcc_section指针临时分配，函数返回前需要释放
		delete pait_section;
	}

	return rtcode;
}

int DVB_MHP_AIT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, application_information_section_t* pait_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	char		pszComment[128];
	char		pszField[64];

	if ((pxmlDoc != NULL) && (pait_section != NULL))
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "application_information_section()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, pait_section->section_length + 3);

		//AIT section最小长度16字节
		if (pait_section->table_id == TABLE_ID_AIT)
		{
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pait_section->table_id, 8, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pait_section->section_syntax_indicator, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pait_section->reserved_future_use0, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pait_section->reserved0, 2, "bslbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", pait_section->section_length, 12, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "test_application_flag", pait_section->test_application_flag, 1, "bslbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "application_type", pait_section->application_type, 15, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pait_section->reserved1, 2, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pait_section->version_number, 5, "uimsbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pait_section->current_next_indicator, 1, "bslbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pait_section->section_number, 8, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pait_section->last_section_number, 8, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use1", pait_section->reserved_future_use1, 4, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "common_descriptors_length", pait_section->common_descriptors_length, 12, "uimsbf", NULL);

			if (pait_section->common_descriptors_length > 0)
			{
				XMLElement* pxmlDescriptorsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "AIT描述符循环()", NULL);
				XMLNODE_SetFieldLength(pxmlDescriptorsNode, pait_section->common_descriptors_length);

				for (int descriptor_index = 0; descriptor_index < pait_section->common_descriptor_count; descriptor_index++)
				{
					descriptor_buf = pait_section->common_descriptors[descriptor_index].descriptor_buf;
					descriptor_tag = pait_section->common_descriptors[descriptor_index].descriptor_tag;
					descriptor_length = pait_section->common_descriptors[descriptor_index].descriptor_length;
					descriptor_size = descriptor_length + 2;

					switch (descriptor_tag)
					{
					//case DVB_MHP_TRANSPORT_PROTOCOL_DESCRIPTOR:
					//	DVB_MHP_decode_transport_protocol_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlDescriptorsNode);
					//	break;
					default:
						MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlDescriptorsNode, pait_section->common_descriptors + descriptor_index);

						break;
					}
				}
			}

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", pait_section->reserved_future_use2, 4, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "application_loop_length", pait_section->application_loop_length, 12, "uimsbf", NULL);

			if (pait_section->application_loop_length > 0)
			{
				XMLElement* pxmlApplicationsLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "应用循环()", NULL);
				XMLNODE_SetFieldLength(pxmlApplicationsLoopNode, pait_section->application_loop_length);

				for (int application_index = 0; application_index < pait_section->application_count; application_index ++)
				{
					application_t* papplication = pait_section->applications + application_index;

					sprintf_s(pszField, sizeof(pszField), "应用[%d](ID:0x%04X)", application_index, papplication->application_identifier.application_id);
					XMLElement* pxmlApplicationNode = XMLDOC_NewElementForString(pxmlDoc, pxmlApplicationsLoopNode, pszField, NULL);
					XMLNODE_SetFieldLength(pxmlApplicationNode, 9 + papplication->application_descriptors_loop_length);

					//解析applications
					XMLElement* pxmlIdentifierNode = XMLDOC_NewElementForString(pxmlDoc, pxmlApplicationNode, "application_identifier()", NULL);
					XMLNODE_SetFieldLength(pxmlIdentifierNode, 6);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlIdentifierNode, "organisation_id", papplication->application_identifier.organisation_id, 32, "uimsbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlIdentifierNode, "application_id", papplication->application_identifier.application_id, 16, "uimsbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlApplicationNode, "application_control_code", papplication->application_control_code, 8, "uimsbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlApplicationNode, "reserved_future_use", papplication->reserved_future_use, 4, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlApplicationNode, "application_descriptors_loop_length", papplication->application_descriptors_loop_length, 12, "uimsbf", NULL);

					if (papplication->application_descriptors_loop_length > 0)
					{
						XMLElement* pxmlDescriptorsLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlApplicationNode, "应用描述符循环()", NULL);
						XMLNODE_SetFieldLength(pxmlDescriptorsLoopNode, papplication->application_descriptors_loop_length);

						for (int descriptor_index = 0; descriptor_index < papplication->descriptor_count; descriptor_index ++)
						{
							descriptor_buf = papplication->descriptors[descriptor_index].descriptor_buf;
							descriptor_tag = papplication->descriptors[descriptor_index].descriptor_tag;
							descriptor_length = papplication->descriptors[descriptor_index].descriptor_length;
							descriptor_size = descriptor_length + 2;

							switch (descriptor_tag)
							{
							//case DVB_MHP_APPLICATION_DESCRIPTOR:
							//	DVB_MHP_decode_application_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_MHP_APPLICATION_NAME_DESCRIPTOR:
							//	DVB_MHP_decode_application_name_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_MHP_TRANSPORT_PROTOCOL_DESCRIPTOR:
							//	DVB_MHP_decode_transport_protocol_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_MHP_DVB_J_APPLICATION_LOCATION_DESCRIPTOR:
							//	DVB_MHP_decode_dvb_j_application_location_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							default:
								MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlDescriptorsLoopNode, papplication->descriptors + descriptor_index);

								break;
							}
						}
					}
				}
			}

			XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", pait_section->CRC_32, 32, "rpchof", NULL);

			if (pait_section->CRC_32_recalculated != pait_section->CRC_32)
			{
				sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pait_section->CRC_32_recalculated);
				XMLNODE_SetAttribute(pxmlCrcNode, "error", pszComment);
			}
		}
		else
		{
			sprintf_s(pszComment, sizeof(pszComment), "section syntax error! incorrect table_id = 0x%02X", pait_section->table_id);
			pxmlRootNode->SetAttribute("error", pszComment);
			rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

