#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/DVB_SI_section_XML.h"

/////////////////////////////////////////////
int DVB_SI_RST_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, running_status_section_t* pRSTSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	int	 N = 0;
	char pszStatus[64];
	char pszFieldName[64];
	char pszComment[128];

	running_status_section_t* prst_section = (pRSTSection != NULL) ? pRSTSection : new running_status_section_t;
	rtcode = DVB_SI_RST_DecodeSection(section_buf, section_size, prst_section);

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "running_status_section()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, section_size);

		sprintf_s(pszComment, sizeof(pszComment), "%d字节", section_size);
		XMLNODE_SetAttribute(pxmlRootNode, "comment", pszComment);

		if (rtcode != SECTION_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", prst_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", prst_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", prst_section->reserved_future_use, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", prst_section->reserved, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", prst_section->section_length, 12, "uimsbf", NULL);

		if (prst_section->section_length > 0)
		{
			for (int loop_index = 0; loop_index < prst_section->N; loop_index++)			
			{
				RUNSTATUS_DESCRIPTION_t* pstRunStatus = prst_section->astRunStatus + loop_index;

				sprintf_s(pszFieldName, sizeof(pszFieldName), "status[%d]()", loop_index);
				XMLElement* pxmlStatusNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszFieldName, NULL);
				XMLNODE_SetFieldLength(pxmlStatusNode, 9);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStatusNode, "transport_stream_id", pstRunStatus->transport_stream_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStatusNode, "original_network_id", pstRunStatus->original_network_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStatusNode, "service_id", pstRunStatus->service_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStatusNode, "event_id", pstRunStatus->event_id, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlStatusNode, "reserved_future_use", pstRunStatus->reserved_future_use, 5, "bslbf", NULL);
				DVB_SI_NumericCoding2Text_RunningStatus(pstRunStatus->running_status, pszStatus, sizeof(pszStatus));
				XMLDOC_NewElementForBits(pxmlDoc, pxmlStatusNode, "running_status", pstRunStatus->running_status, 3, "uimsbf", pszStatus);

				sprintf_s(pszComment, sizeof(pszComment), "ONetID=0x%04X, TsID=0x%04X, service_id = 0x%04X, event_id = 0x%04X, status = %s",
					pstRunStatus->original_network_id,
					pstRunStatus->transport_stream_id,
					pstRunStatus->service_id,
					pstRunStatus->event_id,
					pszStatus);

				XMLNODE_SetAttribute(pxmlStatusNode, "comment", pszComment);
			}
		}
	}

	if (pRSTSection == NULL)
	{
		//说明上面的prst_section指针为临时分配，函数返回前需要释放
		delete prst_section;
	}

	return rtcode;
}

