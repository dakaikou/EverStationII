#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"

/////////////////////////////////////////////
CBAT::CBAT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CBAT::~CBAT(void)
{
	Reset();
}

void CBAT::Init(void)
{
	CPVT::Init();

	m_astStreamInfo = NULL;
	m_aucSectionFlag = NULL;

	m_pszBouquetName = NULL;

	Reset();
}

void CBAT::Reset(void)
{
	m_usBouquetID = 0xFFFF;
	//memset(m_pszBouquetName, 0x00, sizeof(m_pszBouquetName));
	if (m_pszBouquetName != NULL)
	{
		free(m_pszBouquetName);
		m_pszBouquetName = NULL;
	}

	if (m_astStreamInfo != NULL)
	{
		free(m_astStreamInfo);
		m_astStreamInfo = NULL;
	}
	m_nStreamCount = 0;
	m_nMemoryForStreamInfos = 0;

	CPVT::Reset();
}

int CBAT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int			 rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;					//0 -- fail

	int			 descriptor_index;
	int			 stream_index;

	int			 nStreamCount;

	bouquet_association_section_t			bat_section;

	STREAM_INFO_t*							pStreamInfo;
	STREAM_DESCRIPTION_t*					pStreamDescription;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_SI_BAT_DecodeSection(buf, length, &bat_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_usBouquetID = bat_section.bouquet_id;

			for (descriptor_index = 0; descriptor_index < bat_section.bouquet_descriptor_count; descriptor_index ++)
			{
				uint8_t* descriptor_buf = bat_section.bouquet_descriptors[descriptor_index].descriptor_buf;
				int descriptor_size = bat_section.bouquet_descriptors[descriptor_index].descriptor_size;

				if (bat_section.bouquet_descriptors[descriptor_index].descriptor_tag == DVB_SI_BOUQUET_NAME_DESCRIPTOR)
				{
					if (m_pszBouquetName == NULL)
					{
						bouquet_name_descriptor_t				bouquet_name_descriptor;
						DVB_SI_decode_bouquet_name_descriptor(descriptor_buf, descriptor_size, &bouquet_name_descriptor);
						//strcpy_s(m_pszBouquetName, sizeof(m_pszBouquetName), bouquet_name_descriptor.trimmed_bouquet_name);
						//memcpy_s(m_pszBouquetName, sizeof(m_pszBouquetName), bouquet_name_descriptor.trimmed_bouquet_name, bouquet_name_descriptor.trimmed_bouquet_name_length);
						//m_pszBouquetName[bouquet_name_descriptor.trimmed_bouquet_name_length] = '\0';

						m_pszBouquetName = (char*)malloc(bouquet_name_descriptor.trimmed_bouquet_name_length + 1);
						memcpy(m_pszBouquetName, bouquet_name_descriptor.trimmed_bouquet_name, bouquet_name_descriptor.trimmed_bouquet_name_length);
						m_pszBouquetName[bouquet_name_descriptor.trimmed_bouquet_name_length] = '\0';

						break;
					}
				}
				else if (bat_section.bouquet_descriptors[descriptor_index].descriptor_tag == DVB_SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR)
				{
					if (m_pszBouquetName == NULL)
					{
						multilingual_bouquet_name_descriptor_t	multilingual_bouquet_name_descriptor;
						DVB_SI_decode_multilingual_bouquet_name_descriptor(descriptor_buf, descriptor_size, &multilingual_bouquet_name_descriptor);
//						strcpy_s(m_pszBouquetName, sizeof(m_pszBouquetName), multilingual_bouquet_name_descriptor.bouquet_name_char[0]);
						//memcpy_s(m_pszBouquetName, sizeof(m_pszBouquetName), multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_char, multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_length);
						//m_pszBouquetName[multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_length] = '\0';

						m_pszBouquetName = (char*)malloc(multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_length + 1);
						memcpy(m_pszBouquetName, multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_char, multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_length);
						m_pszBouquetName[multilingual_bouquet_name_descriptor.st[0].trimmed_bouquet_name_length] = '\0';

						break;
					}
				}
			}
				
			nStreamCount = m_nStreamCount + bat_section.stream_count;
			m_astStreamInfo = (STREAM_INFO_t*)realloc(m_astStreamInfo, nStreamCount * sizeof(STREAM_INFO_t));
			if (m_astStreamInfo != NULL)
			{
				m_nMemoryForStreamInfos = nStreamCount * sizeof(STREAM_INFO_t);

				for (stream_index = 0; stream_index < bat_section.stream_count; stream_index ++)
				{
					pStreamDescription = bat_section.astStreams + stream_index;
					pStreamInfo = m_astStreamInfo + m_nStreamCount;

					pStreamInfo->original_network_id = pStreamDescription->original_network_id;
					pStreamInfo->transport_stream_id = pStreamDescription->transport_stream_id;

					memset(&(pStreamInfo->service_list_descriptor), 0x00, sizeof(service_list_descriptor_t));

					for (descriptor_index = 0; descriptor_index < pStreamDescription->transport_descriptor_count; descriptor_index ++)
					{
						if (pStreamDescription->transport_descriptors[descriptor_index].descriptor_tag == DVB_SI_SERVICE_LIST_DESCRIPTOR)
						{
							service_list_descriptor_t				service_list_descriptor;
							DVB_SI_decode_service_list_descriptor(pStreamDescription->transport_descriptors[descriptor_index].descriptor_buf,
																pStreamDescription->transport_descriptors[descriptor_index].descriptor_size,
																&service_list_descriptor);
							memcpy(&(pStreamInfo->service_list_descriptor), &service_list_descriptor, sizeof(service_list_descriptor_t));

							break;
						}
					}

					m_nStreamCount ++;
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

uint16_t CBAT::GetBouquetID(void)
{
	return m_usBouquetID;
}

int	CBAT::GetBouquetName(char* pszName, int size)
{
	int rtcode = MIDDLEWARE_PSISI_NO_ERROR;
	if ((pszName != NULL) && (m_pszBouquetName != NULL))
	{
		strcpy_s(pszName, size, m_pszBouquetName);
		//memcpy_s(pszName, size, m_pszBouquetName, sizeof(m_pszBouquetName));
	}
	else
	{
		rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
	}

	return rtcode;
}

int	CBAT::GetStreamCount(void)
{
	return m_nStreamCount;
}

int CBAT::GetStreamByIndex(int stream_index, STREAM_INFO_t* pstStreamInfo)
{
	int rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	if (pstStreamInfo != NULL)
	{
		if ((stream_index >= 0) && (stream_index < m_nStreamCount))
		{
			memcpy(pstStreamInfo, m_astStreamInfo + stream_index, sizeof(STREAM_INFO_t));
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

int CBAT::GetStreamByID(uint16_t transport_stream_id, STREAM_INFO_t* pstStreamInfo)
{
	int rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	if (pstStreamInfo != NULL)
	{
		for (int i = 0; i < m_nStreamCount; i++)
		{
			if (m_astStreamInfo[i].transport_stream_id == transport_stream_id)
			{
				memcpy(pstStreamInfo, m_astStreamInfo + i, sizeof(STREAM_INFO_t));
				rtcode = MIDDLEWARE_PSISI_NO_ERROR;

				break;
			}
		}
	}
	else
	{
		rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
	}

	return rtcode;
}
