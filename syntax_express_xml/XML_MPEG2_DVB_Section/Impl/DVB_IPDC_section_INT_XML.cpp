#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "translate_layer/MPEG2_DVB_Section/Include/DVB_IPDC_section.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

#include "../Include/DVB_IPDC_section_XML.h"
#include "../Include/MPEG_DVB_Common_XML.h"

/////////////////////////////////////////////

int DVB_IPDC_INT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, IP_MAC_notification_section_t* pint_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pint_section != NULL))
	{
		uint16_t descriptor_tag;
		int		 descriptor_length;
		uint8_t* descriptor_buf;
		int		 descriptor_size;
		char pszField[128];
		char pszComment[128];

		//根节点
		sprintf_s(pszField, sizeof(pszField), "IP/MAC_notification_section(table_id=0x%02X)", pint_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, pint_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", pint_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", pint_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_for_future_use", pint_section->reserved_for_future_use, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pint_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", pint_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "action_type", pint_section->action_type, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "platform_id_hash", pint_section->platform_id_hash, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pint_section->reserved1, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", pint_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", pint_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", pint_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", pint_section->last_section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "platform_id", pint_section->platform_id, 24, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "processing_order", pint_section->processing_order, 8, "uimsbf", NULL);

		platform_descriptor_loop_t* pplatform_descriptor_loop = &(pint_section->platform_descriptor_loop);

		XMLElement* pxmlPlatformDescriptorLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, "platform_descriptor_loop()", NULL, 2 + pplatform_descriptor_loop->platform_descriptor_loop_length);

		pxmlDoc->NewElementForBits(pxmlPlatformDescriptorLoopNode, "reserved", pplatform_descriptor_loop->reserved, 4, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlPlatformDescriptorLoopNode, "platform_descriptor_loop_length", pplatform_descriptor_loop->platform_descriptor_loop_length, 12, "uimsbf", NULL);

		if (pplatform_descriptor_loop->platform_descriptor_loop_length > 0)
		{
			for (int descriptor_index = 0; descriptor_index < pplatform_descriptor_loop->reserved_count; descriptor_index++)
			{
				descriptor_buf = pplatform_descriptor_loop->reserved_descriptor[descriptor_index].descriptor_buf;
				descriptor_tag = pplatform_descriptor_loop->reserved_descriptor[descriptor_index].descriptor_tag;
				descriptor_length = pplatform_descriptor_loop->reserved_descriptor[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlPlatformDescriptorLoopNode, pplatform_descriptor_loop->reserved_descriptor + descriptor_index);
					break;
				}
			}
		}

		int loop_length = pint_section->section_length - 11 - pplatform_descriptor_loop->platform_descriptor_loop_length - 4;
		if (loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "target & operational descriptor loop(共 %d 个)", pint_section->notification_count);
			XMLElement* pxmlTargetAndOperationalLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, loop_length);

			for (int notification_index = 0; notification_index < pint_section->notification_count; notification_index++)
			{
				target_descriptor_loop_t* ptarget_descriptor_loop = &(pint_section->notifications[notification_index].target_descriptor_loop);
				operational_descriptor_loop_t* poperational_descriptor_loop = &(pint_section->notifications[notification_index].operational_descriptor_loop);

				sprintf_s(pszField, sizeof(pszField), "target & operational[%d]()", notification_index);
				XMLElement* pxmlTargetAndOperationalNode = pxmlDoc->NewBranchElement(pxmlTargetAndOperationalLoopNode, pszField, NULL, 2 + ptarget_descriptor_loop->target_descriptor_loop_length + 2 + poperational_descriptor_loop->operational_descriptor_loop_length);

				sprintf_s(pszField, sizeof(pszField), "target_descriptor_loop()");
				XMLElement* pxmlTargetNode = pxmlDoc->NewBranchElement(pxmlTargetAndOperationalNode, pszField, NULL, 2 + ptarget_descriptor_loop->target_descriptor_loop_length);

				pxmlDoc->NewElementForBits(pxmlTargetNode, "reserved", ptarget_descriptor_loop->reserved, 4, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlTargetNode, "target_descriptor_loop_length", ptarget_descriptor_loop->target_descriptor_loop_length, 12, "uimsbf", NULL);

				if (ptarget_descriptor_loop->target_descriptor_loop_length > 0)
				{
					//sprintf_s(pszField, sizeof(pszField), "target_descriptor_loop()");
					//XMLElement* pxmlDescriptorsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlTargetNode, pszField, NULL);
					//XMLNODE_SetFieldLength(pxmlDescriptorsNode, ptarget_descriptor_loop->target_descriptor_loop_length);

					for (int descriptor_index = 0; descriptor_index < ptarget_descriptor_loop->target_descriptor_count; descriptor_index++)
					{
						descriptor_buf = ptarget_descriptor_loop->target_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = ptarget_descriptor_loop->target_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = ptarget_descriptor_loop->target_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlTargetNode, ptarget_descriptor_loop->target_descriptors + descriptor_index);
							break;
						}
					}
				}

				sprintf_s(pszField, sizeof(pszField), "operational_descriptor_loop()");
				XMLElement* pxmlOperationalNode = pxmlDoc->NewBranchElement(pxmlTargetAndOperationalNode, pszField, NULL, 2 + poperational_descriptor_loop->operational_descriptor_loop_length);

				pxmlDoc->NewElementForBits(pxmlOperationalNode, "reserved", poperational_descriptor_loop->reserved, 4, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlOperationalNode, "operational_descriptor_loop_length", poperational_descriptor_loop->operational_descriptor_loop_length, 12, "uimsbf", NULL);

				if (poperational_descriptor_loop->operational_descriptor_loop_length > 0)
				{
					//sprintf_s(pszField, sizeof(pszField), "operational_descriptor_loop()");
					//XMLElement* pxmlDescriptorsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlTargetNode, pszField, NULL);
					//XMLNODE_SetFieldLength(pxmlDescriptorsNode, poperational_descriptor_loop->operational_descriptor_loop_length);

					for (int descriptor_index = 0; descriptor_index < poperational_descriptor_loop->operational_descriptor_count; descriptor_index++)
					{
						descriptor_buf = poperational_descriptor_loop->operational_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = poperational_descriptor_loop->operational_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = poperational_descriptor_loop->operational_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlOperationalNode, poperational_descriptor_loop->operational_descriptors + descriptor_index);
							break;
						}
					}
				}
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", pint_section->CRC_32, 32, "rpchof", NULL);

		if (pint_section->CRC_32_recalculated != pint_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pint_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_IPDC_INT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, TALForXMLDoc* pxmlDoc, IP_MAC_notification_section_t* pINTSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	IP_MAC_notification_section_t* pint_section = (pINTSection != NULL) ? pINTSection : new IP_MAC_notification_section_t;
	rtcode = DVB_IPDC_INT_DecodeSection(section_buf, section_size, pint_section);
	rtcode = DVB_IPDC_INT_PresentSection_to_XML(pxmlDoc, pint_section);

	if (pINTSection == NULL)
	{
		//说明ppmt_section指针临时分配，函数返回前需要释放
		delete pint_section;
	}

	return rtcode;
}
