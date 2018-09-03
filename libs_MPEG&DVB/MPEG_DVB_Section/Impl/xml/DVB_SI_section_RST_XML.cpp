#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/DVB_SI_section_XML.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

/////////////////////////////////////////////
int DVB_SI_RST_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, running_status_section_t* pRSTSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	int	 N = 0;
	char pszStatus[64];
	char pszKeyName[64];
	char pszComment[128];
	BITS_t	bs;

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);
		pxmlDoc->SetOrigin(buf);

		//根节点
		tinyxml2::XMLElement* pxmlRoot = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("running_status_section()");
		pxmlDoc->InsertEndChild(pxmlRoot);
		pxmlDoc->UpdateBufMark(pxmlRoot, buf, buf + length);

		if ((buf != NULL) && (length >= DVB_SI_RST_SECTION_MIN_SIZE) && (length <= DVB_SI_RST_SECTION_MAX_SIZE))
		{
			running_status_section_t* prst_section = (pRSTSection != NULL) ? pRSTSection : new running_status_section_t;
			memset(prst_section, 0x00, sizeof(running_status_section_t));

			BITS_map(&bs, buf, length);

			prst_section->table_id = BITS_get(&bs, 8);
			assert(prst_section->table_id == TABLE_ID_RST);
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "table_id", prst_section->table_id, 8, "uimsbf", NULL, &bs);

			prst_section->section_syntax_indicator = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_syntax_indicator", prst_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);
			prst_section->reserved_future_use = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved_future_use", prst_section->reserved_future_use, 1, "bslbf", NULL, &bs);
			prst_section->reserved = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved", prst_section->reserved, 2, "bslbf", NULL, &bs);

			prst_section->section_length = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_length", prst_section->section_length, 12, "uimsbf", NULL, &bs);

			N = 0;
			if (prst_section->section_length > 0)
			{
				if ((prst_section->section_length % 9) == 0)
				{
					N = prst_section->section_length / 9;

					int loop_count = min(N, MAX_STREAMS_PER_NETWORK);
					for (int loop_index = 0; loop_index < loop_count; loop_index++)			
					{
						uint8_t* old_loop_ptr = bs.p_cur;
						sprintf_s(pszKeyName, sizeof(pszKeyName), "status %d", loop_index);
						tinyxml2::XMLElement* pxmlStatusNode = pxmlDoc->NewKeyValuePairElement(pxmlRoot, pszKeyName);

						prst_section->RunStatus[loop_index].transport_stream_id = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlStatusNode, "transport_stream_id", prst_section->RunStatus[loop_index].transport_stream_id, 16, "uimsbf", NULL, &bs);

						prst_section->RunStatus[loop_index].original_network_id = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlStatusNode, "original_network_id", prst_section->RunStatus[loop_index].original_network_id, 16, "uimsbf", NULL, &bs);

						prst_section->RunStatus[loop_index].service_id = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlStatusNode, "service_id", prst_section->RunStatus[loop_index].service_id, 16, "uimsbf", NULL, &bs);

						prst_section->RunStatus[loop_index].event_id = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlStatusNode, "event_id", prst_section->RunStatus[loop_index].event_id, 16, "uimsbf", NULL, &bs);

						prst_section->RunStatus[loop_index].reserved_future_use = BITS_get(&bs, 5);
						pxmlDoc->NewKeyValuePairElement(pxmlStatusNode, "reserved_future_use", prst_section->RunStatus[loop_index].reserved_future_use, 5, "bslbf", NULL, &bs);

						prst_section->RunStatus[loop_index].running_status = BITS_get(&bs, 3);
						DVB_SI_NumericCoding2Text_RunningStatus(prst_section->RunStatus[loop_index].running_status, pszStatus, sizeof(pszStatus));
						pxmlDoc->NewKeyValuePairElement(pxmlStatusNode, "running_status", prst_section->RunStatus[loop_index].running_status, 3, "uimsbf", pszStatus, &bs);

						sprintf_s(pszComment, sizeof(pszComment), "ONetID=%04X, TsID=0x%04X, service_id = 0x%04X, event_id = 0x%04X, status = %s",
							prst_section->RunStatus[loop_index].original_network_id,
							prst_section->RunStatus[loop_index].transport_stream_id,
							prst_section->RunStatus[loop_index].service_id,
							prst_section->RunStatus[loop_index].event_id,
							pszStatus);

						pxmlStatusNode->SetAttribute("comment", pszComment);
						pxmlDoc->UpdateBufMark(pxmlStatusNode, old_loop_ptr, bs.p_cur);
					}
				}
				else
				{
					pxmlRoot->SetAttribute("error", "section_length error, must be divided by 9!");
					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}
			}
			prst_section->N = N;

			if (pRSTSection == NULL)
			{
				//说明上面的prst_section指针为临时分配，函数返回前需要释放
				delete prst_section;
			}
		}
		else 
		{
			pxmlRoot->SetAttribute("error", "input parameter error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

