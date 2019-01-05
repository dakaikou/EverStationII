#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"

////////////////////////////////////////////
CPMT::CPMT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CPMT::~CPMT(void)
{
	Reset();
}

void CPMT::Init(void)
{
	CPVT::Init();

	m_astEsInfo = NULL;

	Reset();
}

void CPMT::Reset(void)
{
	m_usProgramNumber		= 0xffff;
	m_usPCR_PID				= 0xffff;

	if (m_astEsInfo != NULL)
	{
		free(m_astEsInfo);
		m_astEsInfo = NULL;
	}
	m_nMemoryForEsInfos = 0;
	m_nEsCount = 0;

	CPVT::Reset();
}

int CPMT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;
	int descriptor_index;
	int	es_index;
	TS_program_map_section_t		pmt_section;

	data_broadcast_id_descriptor_t		data_broadcast_id_descriptor;
	stream_identifier_descriptor_t		stream_identifier_descriptor;
	association_tag_descriptor_t		association_tag_descriptor;
	carousel_identifier_descriptor_t	carousel_identifier_descriptor;

	rtcode = CPVT::AddSection(usPID, pprivate_section);
	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = MPEG2_PSI_PMT_DecodeSection(buf, length, &pmt_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			if (m_bCollectOver)
			{
				m_usPCR_PID = pmt_section.PCR_PID;
				m_usProgramNumber = pmt_section.program_number;
			}

			//the max stream is just limited by PC's memory
			m_astEsInfo = (ES_INFO_t*)realloc(m_astEsInfo, (m_nEsCount + pmt_section.ES_map_count) * sizeof(ES_INFO_t));
			if (m_astEsInfo != NULL)
			{
				m_nMemoryForEsInfos = (m_nEsCount + pmt_section.ES_map_count) * sizeof(ES_INFO_t);

				for (es_index = 0; es_index < pmt_section.ES_map_count; es_index ++)
				{
					m_astEsInfo[m_nEsCount].stream_type = pmt_section.astESMaps[es_index].stream_type;
					m_astEsInfo[m_nEsCount].stream_subtype = pmt_section.astESMaps[es_index].stream_subtype;
					m_astEsInfo[m_nEsCount].elementary_PID = pmt_section.astESMaps[es_index].elementary_PID;

					//DVB_SI_STREAM_IDENTIFIER_DESCRIPTOR在ES流第二层循环不一定出现
					m_astEsInfo[m_nEsCount].component_tag = 0xFF;				//默认值

					//MPEG2_DSMCC_ASSOCIATION_TAG_DESCRIPTOR在ES流第二层循环不一定出现
					m_astEsInfo[m_nEsCount].association_tag = 0xFFFF;
					m_astEsInfo[m_nEsCount].transaction_id = 0xFFFFFFFF;

					//DVB_SI_DATA_BROADCAST_ID_DESCRIPTOR在ES流第二层循环不一定出现
					m_astEsInfo[m_nEsCount].data_broadcast_id = 0xFFFF;
					m_astEsInfo[m_nEsCount].carousel_type_id = 0xFFFF;
					m_astEsInfo[m_nEsCount].carousel_id = 0xFFFFFFFF;

					for (descriptor_index = 0; descriptor_index < pmt_section.astESMaps[es_index].ES_descriptor_count; descriptor_index ++)
					{
						uint8_t* descriptor_buf = pmt_section.astESMaps[es_index].ES_descriptors[descriptor_index].descriptor_buf;
						int		 descriptor_size = pmt_section.astESMaps[es_index].ES_descriptors[descriptor_index].descriptor_size;

						if (pmt_section.astESMaps[es_index].ES_descriptors[descriptor_index].descriptor_tag == DVB_SI_DATA_BROADCAST_ID_DESCRIPTOR)
						{
							if (DVB_SI_decode_data_broadcast_id_descriptor(descriptor_buf, descriptor_size, &data_broadcast_id_descriptor) == SECTION_PARSE_NO_ERROR)
							{
								m_astEsInfo[m_nEsCount].data_broadcast_id = data_broadcast_id_descriptor.data_broadcast_id;
							}
						}
						else if (pmt_section.astESMaps[es_index].ES_descriptors[descriptor_index].descriptor_tag == DVB_SI_STREAM_IDENTIFIER_DESCRIPTOR)
						{
							if (DVB_SI_decode_stream_identifier_descriptor(descriptor_buf, descriptor_size, &stream_identifier_descriptor) == SECTION_PARSE_NO_ERROR)
							{
								m_astEsInfo[m_nEsCount].component_tag = stream_identifier_descriptor.component_tag;
							}
						}
						else if (pmt_section.astESMaps[es_index].ES_descriptors[descriptor_index].descriptor_tag == MPEG2_DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR)
						{
							if (MPEG2_DSMCC_decode_carousel_identifier_descriptor(descriptor_buf, descriptor_size, &carousel_identifier_descriptor) == SECTION_PARSE_NO_ERROR)
							{
								m_astEsInfo[m_nEsCount].carousel_id = carousel_identifier_descriptor.carousel_id;
							}
						}
						else if (pmt_section.astESMaps[es_index].ES_descriptors[descriptor_index].descriptor_tag == MPEG2_DSMCC_ASSOCIATION_TAG_DESCRIPTOR)
						{
							if (MPEG2_DSMCC_decode_association_tag_descriptor(descriptor_buf, descriptor_size, &association_tag_descriptor) == SECTION_PARSE_NO_ERROR)
							{
								m_astEsInfo[m_nEsCount].association_tag = association_tag_descriptor.association_tag;

								if (association_tag_descriptor.use == 0x0000)
								{
									m_astEsInfo[m_nEsCount].transaction_id = association_tag_descriptor.u.use_0x0000.transaction_id;
								}
							}
						}
					}

					m_nEsCount ++;
				}
			}
			else
			{
				rtcode = MIDDLEWARE_PSISI_MEMORY_ERROR;
			}
		}
	}

	return rtcode;
}

uint16_t CPMT::GetProgramNumber(void)
{
	return m_usProgramNumber;
}

uint16_t CPMT::GetPcrPID(void)
{
	return m_usPCR_PID;
}

int CPMT::GetESCount(void)
{
	return m_nEsCount;
}

int CPMT::GetESInfo(int index, ES_INFO_t* pstESInfo)
{
	int rtcode = MIDDLEWARE_PSISI_NO_ERROR;

	if ((index >= 0) && (index < m_nEsCount))
	{
		if (pstESInfo != NULL)
		{
			memcpy(pstESInfo, m_astEsInfo + index, sizeof(ES_INFO_t));
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

