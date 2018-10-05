#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/nativeInclude/DVB_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\nativeInclude\MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
CEIT::CEIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CEIT::~CEIT(void)
{
	Reset();
}

void CEIT::Init(void)
{
	CPVT::Init();

	m_astEvent = NULL;
	//m_astSegment = NULL;

	Reset();
}

void CEIT::Reset(void)
{
	//S32				i;
	//eit_segment_t*	psegment;

	m_usServiceID = 0xffff;
	m_usTransportStreamID = 0xffff;
	m_usOriginalNetworkID = 0xffff;

	if (m_astEvent != NULL)
	{
		free(m_astEvent);
		m_astEvent = NULL;
	}
	m_nMemoryForEvents = 0;
	m_nEventCount = 0;

	//if (m_astSegment != NULL)
	//{
	//	for (i = 0; i < m_nSegmentCount; i++)
	//	{
	//		psegment = m_astSegment + i;

	//		if (psegment->aucSectionFlag != NULL)
	//		{
	//			free(psegment->aucSectionFlag);
	//		}
	//		psegment->aucSectionFlag = NULL;
	//		psegment->bCollectOver = 0;
	//		psegment->nSectionCount = 0;
	//	}

	//	free(m_astSegment);
	//}
	//m_astSegment = NULL;
	//m_nMemoryForSegments = 0;
	//m_nSegmentCount = 0;

	CPVT::Reset();
}

int CEIT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int			 rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	event_information_section_t		eit_section;
	int								descriptor_index;
	short_event_descriptor_t		short_event_descriptor;

	rtcode = CPVT::AddSection(usPID, pprivate_section);
	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_SI_EIT_DecodeSection(buf, length, &eit_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_usServiceID = eit_section.service_id;
			m_usTransportStreamID = eit_section.transport_stream_id;
			m_usOriginalNetworkID = eit_section.original_network_id;

			if (m_usServiceID == 0x012D)
			{
				rtcode = SECTION_PARSE_NO_ERROR;
			}

			m_astEvent = (EVENT_INFO_t*)realloc(m_astEvent, (m_nEventCount + eit_section.event_count) * sizeof(EVENT_INFO_t));
			if (m_astEvent != NULL)
			{
				m_nMemoryForEvents = (m_nEventCount + eit_section.event_count) * sizeof(EVENT_INFO_t);

				for (int i = 0; i < eit_section.event_count; i++)
				{
					m_astEvent[m_nEventCount].event_id = eit_section.astEvents[i].event_id;
					m_astEvent[m_nEventCount].start_time = eit_section.astEvents[i].start_time;
					m_astEvent[m_nEventCount].duration = eit_section.astEvents[i].duration;

					for (descriptor_index = 0; descriptor_index < eit_section.astEvents[i].event_descriptor_count; descriptor_index++)
					{
						uint8_t* descriptor_buf = eit_section.astEvents[i].event_descriptors[descriptor_index].descriptor_buf;
						int descriptor_size = eit_section.astEvents[i].event_descriptors[descriptor_index].descriptor_size;

						if (eit_section.astEvents[i].event_descriptors[descriptor_index].descriptor_tag == DVB_SI_SHORT_EVENT_DESCRIPTOR)
						{
							rtcode = DVB_SI_decode_short_event_descriptor(descriptor_buf, descriptor_size, &short_event_descriptor);
							if (rtcode == SECTION_PARSE_NO_ERROR)
							{
								strcpy_s(m_astEvent[m_nEventCount].pszEventName, sizeof(m_astEvent[m_nEventCount].pszEventName), short_event_descriptor.trimmed_event_name_char);
								strcpy_s(m_astEvent[m_nEventCount].pszEventText, sizeof(m_astEvent[m_nEventCount].pszEventText), short_event_descriptor.trimmed_text_char);
							}
							else
							{
								//assert(0);
								strcpy_s(m_astEvent[m_nEventCount].pszEventName, sizeof(m_astEvent[m_nEventCount].pszEventName), "*** syntax error!");
								strcpy_s(m_astEvent[m_nEventCount].pszEventText, sizeof(m_astEvent[m_nEventCount].pszEventText), "*** syntax error!");
							}
							break;
						}
					}

					m_nEventCount++;
				}
			}
		}
	}

	return rtcode;
}

uint16_t CEIT::GetServiceID(void)
{
	return m_usServiceID;
}

uint16_t CEIT::GetTransportStreamID(void)
{
	return m_usTransportStreamID;
}

uint16_t CEIT::GetOriginalNetworkID(void)
{
	return m_usOriginalNetworkID;
}

int CEIT::GetEventCount(void)
{
	return m_nEventCount;
}

int CEIT::GetEventInfoByIndex(int event_index, EVENT_INFO_t* pstDstEventInfo)
{
	int rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	if (pstDstEventInfo != NULL)
	{
		if ((event_index >= 0) && (event_index < m_nEventCount))
		{
			EVENT_INFO_t* pstSrcEventInfo = m_astEvent + event_index;

			//pstDstEventInfo->event_id = pstSrcEventInfo->event_id;
			//pstDstEventInfo->start_time = pstSrcEventInfo->start_time;
			//pstDstEventInfo->duration = pstSrcEventInfo->duration;

			memcpy(pstDstEventInfo, pstSrcEventInfo, sizeof(EVENT_INFO_t));
			rtcode = MIDDLEWARE_PSISI_NO_ERROR;
		}
		else
		{
			rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
	}

	return rtcode;
}
