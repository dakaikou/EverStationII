#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/DVB_SI_Utilities.h"
#include "../Include/MPEG_DVB_Common_XML.h"
#include "../Include/DVB_SI_section_XML.h"
#include "../Include/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_EIT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, event_information_section_t* peit_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[256];
	char	pszStartTime[64];
	char	pszDuration[64];
	char	pszComment[128];

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	EVENT_DESCRIPTION_t*	pstEvent;

	if ((pxmlDoc != NULL) && (peit_section != NULL))
	{
		//根节点
		sprintf_s(pszField, sizeof(pszField), "event_information_section(table_id=0x%02X)", peit_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, peit_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", peit_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", peit_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", peit_section->reserved_future_use, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", peit_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", peit_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "service_id", peit_section->service_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved1", peit_section->reserved1, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", peit_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", peit_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", peit_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", peit_section->last_section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_stream_id", peit_section->transport_stream_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "original_network_id", peit_section->original_network_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "segment_last_section_number", peit_section->segment_last_section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_table_id", peit_section->last_table_id, 8, "uimsbf", NULL);

		int event_loop_length = peit_section->section_length - 15;
		if (event_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "事件循环(共 %d条)", peit_section->event_count);
			XMLElement* pxmlEventsLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, event_loop_length);

			for (int event_index = 0; event_index < peit_section->event_count; event_index++)
			{
				pstEvent = peit_section->astEvents + event_index;

				int event_description_length = 12 + pstEvent->descriptors_loop_length;
				DVB_SI_NumericCoding2Text_UTCTime(pstEvent->start_time, pszStartTime, sizeof(pszStartTime));
				DVB_SI_NumericCoding2Text_BCDTime(pstEvent->duration, pszDuration, sizeof(pszDuration));
				sprintf_s(pszField, sizeof(pszField), "事件[%d](<ID=0x%04X, start_time=%s, duration=%s>)", event_index, pstEvent->event_id, pszStartTime, pszDuration);
				XMLElement* pxmlEventNode = pxmlDoc->NewBranchElement(pxmlEventsLoopNode, pszField, NULL, event_description_length);

				pxmlDoc->NewElementForBits(pxmlEventNode, "event_id", pstEvent->event_id, 16, "uimsbf", NULL);

				pxmlDoc->NewElementForX64Bits(pxmlEventNode, "start_time", pstEvent->start_time, 40, "bslbf", pszStartTime);		//5 bytes

				pxmlDoc->NewElementForBits(pxmlEventNode, "duration", pstEvent->duration, 24, "uimsbf", pszDuration);			//3 bytes

				DVB_SI_NumericCoding2Text_RunningStatus(pstEvent->running_status, pszComment, sizeof(pszComment));
				pxmlDoc->NewElementForBits(pxmlEventNode, "running_status", pstEvent->running_status, 3, "uimsbf", pszComment);
				pxmlDoc->NewElementForBits(pxmlEventNode, "free_CA_mode", pstEvent->free_CA_mode, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlEventNode, "descriptors_loop_length", pstEvent->descriptors_loop_length, 12, "uimsbf", NULL);

				if (pstEvent->descriptors_loop_length > 0)
				{
					sprintf_s(pszField, sizeof(pszField), "事件描述符循环()");
					XMLElement* pxmlEventDescriptorsLoopNode = pxmlDoc->NewBranchElement(pxmlEventNode, pszField, NULL, pstEvent->descriptors_loop_length);

					for (int descriptor_index = 0; descriptor_index < pstEvent->event_descriptor_count; descriptor_index++)
					{
						descriptor_buf = pstEvent->event_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = pstEvent->event_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = pstEvent->event_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
							//case DVB_SI_SHORT_EVENT_DESCRIPTOR:
							//	DVB_SI_decode_short_event_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_CONTENT_DESCRIPTOR:
							//	DVB_SI_decode_content_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
							//case DVB_SI_PARENTAL_RATING_DESCRIPTOR:
							//	DVB_SI_decode_parental_rating_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							//	break;
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlEventDescriptorsLoopNode, pstEvent->event_descriptors + descriptor_index);
							break;
						}
					}
				}
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", peit_section->CRC_32, 32, "rpchof", NULL);
		if (peit_section->CRC_32_recalculated != peit_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", peit_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}

int DVB_SI_EIT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, event_information_section_t* pEITSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	event_information_section_t* peit_section = (pEITSection != NULL) ? pEITSection : new event_information_section_t;
	rtcode = DVB_SI_EIT_DecodeSection(section_buf, section_size, peit_section);
	rtcode = DVB_SI_EIT_PresentSection_to_XML(pxmlDoc, peit_section);

	if (pEITSection == NULL)
	{
		//说明peit_section指针临时分配，函数返回前需要释放
		delete peit_section;
	}

	return rtcode;
}
