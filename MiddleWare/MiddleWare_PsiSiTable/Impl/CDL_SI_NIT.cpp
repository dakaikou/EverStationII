#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\Include\MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
CNIT::CNIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CNIT::~CNIT(void)
{
	Reset();
}

void CNIT::Init(void)
{
	CPVT::Init();

	m_astStreamInfo = NULL;
	m_aucSectionFlag = NULL;

	Reset();
}

void CNIT::Reset(void)
{
	m_usNetworkID = 0xFFFF;
	memset(m_pszNetworkName, 0x00, sizeof(m_pszNetworkName));

	if (m_astStreamInfo != NULL)
	{
		free(m_astStreamInfo);
		m_astStreamInfo = NULL;
	}
	m_nStreamCount = 0;
	m_nMemoryForStreamInfos = 0;

	CPVT::Reset();
}

int CNIT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int			 rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;					//0 -- fail

	int			 descriptor_index;
	int			 stream_index;

	int			 nStreamCount;

	network_name_descriptor_t					network_name_descriptor;
	multilingual_network_name_descriptor_t		multilingual_network_name_descriptor;
	service_list_descriptor_t					service_list_descriptor;
	satellite_delivery_system_descriptor_t		satellite_delivery_system_descriptor;
	cable_delivery_system_descriptor_t			cable_delivery_system_descriptor;
	terrestrial_delivery_system_descriptor_t	terrestrial_delivery_system_descriptor;


	network_information_section_t				nit_section;
	STREAM_INFO_t*								pStreamInfo;
	STREAM_DESCRIPTION_t*						pStreamDescription;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_SI_NIT_DecodeSection(buf, length, &nit_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_usNetworkID = nit_section.network_id;

			for (descriptor_index = 0; descriptor_index < nit_section.network_descriptor_count; descriptor_index++)
			{
				uint8_t* descriptor_buf = nit_section.network_descriptors[descriptor_index].descriptor_buf;
				int descriptor_size = nit_section.network_descriptors[descriptor_index].descriptor_size;

				if (nit_section.network_descriptors[descriptor_index].descriptor_tag == DVB_SI_NETWORK_NAME_DESCRIPTOR)
				{
					DVB_SI_decode_network_name_descriptor(descriptor_buf, descriptor_size, &network_name_descriptor);
					strcpy_s(m_pszNetworkName, sizeof(m_pszNetworkName), network_name_descriptor.trimmed_network_name);
					//memcpy_s(m_pszNetworkName, sizeof(m_pszNetworkName), network_name_descriptor.trimmed_network_name, sizeof(network_name_descriptor.network_name));

					break;
				}
				else if (nit_section.network_descriptors[descriptor_index].descriptor_tag == DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR)
				{
					if (strlen(m_pszNetworkName) == 0)
					{
						DVB_SI_decode_multilingual_network_name_descriptor(descriptor_buf, descriptor_size, &multilingual_network_name_descriptor);
//						strcpy_s(m_pszNetworkName, sizeof(m_pszNetworkName), multilingual_network_name_descriptor.network_name_char[0]);
						memcpy_s(m_pszNetworkName, sizeof(m_pszNetworkName), multilingual_network_name_descriptor.LANGUAGE[0].trimmed_network_name_char, sizeof(multilingual_network_name_descriptor.LANGUAGE[0].trimmed_network_name_char));

						break;
					}
				}
			}

			nStreamCount = m_nStreamCount + nit_section.stream_count;
			m_astStreamInfo = (STREAM_INFO_t*)realloc(m_astStreamInfo, nStreamCount * sizeof(STREAM_INFO_t));
			if (m_astStreamInfo != NULL)
			{
				m_nMemoryForStreamInfos = nStreamCount * sizeof(STREAM_INFO_t);

				for (stream_index = 0; stream_index < nit_section.stream_count; stream_index++)
				{
					pStreamDescription = nit_section.astStreams + stream_index;
					pStreamInfo = m_astStreamInfo + m_nStreamCount;

					pStreamInfo->original_network_id = pStreamDescription->original_network_id;
					pStreamInfo->transport_stream_id = pStreamDescription->transport_stream_id;

					memset(&(pStreamInfo->service_list_descriptor), 0x00, sizeof(service_list_descriptor));
					memset(&(pStreamInfo->satellite_delivery_system_descriptor), 0x00, sizeof(satellite_delivery_system_descriptor_t));
					memset(&(pStreamInfo->cable_delivery_system_descriptor), 0x00, sizeof(cable_delivery_system_descriptor_t));
					memset(&(pStreamInfo->terrestrial_delivery_system_descriptor), 0x00, sizeof(terrestrial_delivery_system_descriptor_t));

					for (descriptor_index = 0; descriptor_index < pStreamDescription->transport_descriptor_count; descriptor_index++)
					{
						if (pStreamDescription->transport_descriptors[descriptor_index].descriptor_tag == DVB_SI_SERVICE_LIST_DESCRIPTOR)
						{
							DVB_SI_decode_service_list_descriptor(pStreamDescription->transport_descriptors[descriptor_index].descriptor_buf,
								pStreamDescription->transport_descriptors[descriptor_index].descriptor_size,
								&service_list_descriptor);
							memcpy(&(pStreamInfo->service_list_descriptor), &service_list_descriptor, sizeof(service_list_descriptor_t));
						}
						else if (pStreamDescription->transport_descriptors[descriptor_index].descriptor_tag == DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR)
						{
							DVB_SI_decode_satellite_delivery_system_descriptor(pStreamDescription->transport_descriptors[descriptor_index].descriptor_buf,
								pStreamDescription->transport_descriptors[descriptor_index].descriptor_size,
								&satellite_delivery_system_descriptor);
							memcpy(&(pStreamInfo->satellite_delivery_system_descriptor), &satellite_delivery_system_descriptor, sizeof(satellite_delivery_system_descriptor));
						}
						else if (pStreamDescription->transport_descriptors[descriptor_index].descriptor_tag == DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR)
						{
							DVB_SI_decode_cable_delivery_system_descriptor(pStreamDescription->transport_descriptors[descriptor_index].descriptor_buf,
								pStreamDescription->transport_descriptors[descriptor_index].descriptor_size,
								&cable_delivery_system_descriptor);
							memcpy(&(pStreamInfo->cable_delivery_system_descriptor), &cable_delivery_system_descriptor, sizeof(cable_delivery_system_descriptor));
						}
						else if (pStreamDescription->transport_descriptors[descriptor_index].descriptor_tag == DVB_SI_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR)
						{
							DVB_SI_decode_terrestrial_delivery_system_descriptor(pStreamDescription->transport_descriptors[descriptor_index].descriptor_buf,
								pStreamDescription->transport_descriptors[descriptor_index].descriptor_size,
								&terrestrial_delivery_system_descriptor);
							memcpy(&(pStreamInfo->terrestrial_delivery_system_descriptor), &terrestrial_delivery_system_descriptor, sizeof(terrestrial_delivery_system_descriptor));
						}
					}

					m_nStreamCount++;
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

uint16_t CNIT::GetNetworkID(void)
{
	return m_usNetworkID;
}

int	CNIT::GetNetworkName(char* pszName, int size)
{
	int rtcode = MIDDLEWARE_PSISI_NO_ERROR;
	if (pszName != NULL)
	{
		strcpy_s(pszName, size, m_pszNetworkName);
		//memcpy_s(pszName, size, m_pszNetworkName, sizeof(m_pszNetworkName));
	}
	else
	{
		rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
	}

	return rtcode;
}

int	CNIT::GetStreamCount(void)
{
	return m_nStreamCount;
}

int CNIT::GetStreamByIndex(int stream_index, STREAM_INFO_t* pstStreamInfo)
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

int CNIT::GetStreamByID(uint16_t transport_stream_id, STREAM_INFO_t* pstStreamInfo)
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
