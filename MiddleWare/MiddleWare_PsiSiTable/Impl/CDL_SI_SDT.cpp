#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/DVB_table_id.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section\nativeInclude\MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
CSDT::CSDT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CSDT::~CSDT(void)
{
	Reset();
}

void CSDT::Init(void)
{
	CPVT::Init();

	m_astServiceInfo = NULL;

	Reset();
}

void CSDT::Reset(void)
{
	m_usTransportStreamID = 0xFFFF;
	m_usOriginalNetworkID = 0xFFFF;

	if (m_astServiceInfo != NULL)
	{
		free(m_astServiceInfo);
		m_astServiceInfo = NULL;
	}
	m_nMemoryForServiceInfos = 0;
	m_nServiceCount = 0;

	CPVT::Reset();
}

int CSDT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int			 rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;
	//U16 		 original_network_id;

	S32			 nServiceCount;
	S32			 service_index;
	S32			 descriptor_index;
	S32			 data_broadcast_count;

	service_description_section_t			sdt_section;
	SERVICE_DESCRIPTION_t*					pServiceDescription;
	reserved_descriptor_t*					preserved_descriptor;

	service_descriptor_t					service_descriptor;
	multilingual_service_name_descriptor_t	multilingual_service_name_descriptor;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_SI_SDT_DecodeSection(buf, length, &sdt_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_usOriginalNetworkID = sdt_section.original_network_id;
			m_usTransportStreamID = sdt_section.transport_stream_id;

			nServiceCount = m_nServiceCount + sdt_section.service_count;
			m_astServiceInfo = (SERVICE_INFO_t*)realloc(m_astServiceInfo, nServiceCount * sizeof(SERVICE_INFO_t));
			if (m_astServiceInfo != NULL)
			{
				m_nMemoryForServiceInfos = nServiceCount * sizeof(SERVICE_INFO_t);

				for (service_index = 0; service_index < sdt_section.service_count; service_index ++)
				{
					pServiceDescription = sdt_section.astServiceDescriptions + service_index;

					m_astServiceInfo[m_nServiceCount].service_id = pServiceDescription->service_id;
					m_astServiceInfo[m_nServiceCount].EIT_schedule_flag = pServiceDescription->EIT_schedule_flag;
					m_astServiceInfo[m_nServiceCount].EIT_present_following_flag = pServiceDescription->EIT_present_following_flag;
					m_astServiceInfo[m_nServiceCount].free_CA_mode = pServiceDescription->free_CA_mode;
					m_astServiceInfo[m_nServiceCount].running_status = pServiceDescription->running_status;

					memset(&(m_astServiceInfo[m_nServiceCount].pszServiceName), 0x00, MAX_SERVICE_NAME_LENGTH + 1);
					memset(&(m_astServiceInfo[m_nServiceCount].pszServiceProviderName), 0x00, MAX_SERVICE_PROVIDER_NAME_LENGTH + 1);

					data_broadcast_count = 0;
					for (descriptor_index = 0; descriptor_index < pServiceDescription->service_descriptor_count; descriptor_index++)
					{
						preserved_descriptor = pServiceDescription->service_descriptors + descriptor_index;

						if (preserved_descriptor->descriptor_tag == DVB_SI_SERVICE_DESCRIPTOR)
						{
							DVB_SI_decode_service_descriptor(preserved_descriptor->descriptor_buf, 
														preserved_descriptor->descriptor_size,
														&service_descriptor);

							m_astServiceInfo[m_nServiceCount].service_type = service_descriptor.service_type;

							if (service_descriptor.service_name_length > 0)
							{
								strcpy_s(m_astServiceInfo[m_nServiceCount].pszServiceName, sizeof(m_astServiceInfo[m_nServiceCount].pszServiceName), service_descriptor.trimmed_service_name);
								//memcpy(&(m_astServiceInfo[m_nServiceCount].service_name), service_descriptor.trimmed_service_name, service_descriptor.service_name_length);
							}

							if (service_descriptor.service_provider_name_length > 0)
							{
								strcpy_s(m_astServiceInfo[m_nServiceCount].pszServiceProviderName, sizeof(m_astServiceInfo[m_nServiceCount].pszServiceProviderName), service_descriptor.trimmed_service_provider_name);
								//memcpy(&(m_astServiceInfo[m_nServiceCount].service_provider_name), service_descriptor.service_provider_name, service_descriptor.service_provider_name_length);
							}
						}
						else if (preserved_descriptor->descriptor_tag == DVB_SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR)
						{
							if (strlen(m_astServiceInfo[m_nServiceCount].pszServiceName) == 0)
							{
								DVB_SI_decode_multilingual_service_name_descriptor(preserved_descriptor->descriptor_buf, 
																				preserved_descriptor->descriptor_size,
																				&multilingual_service_name_descriptor);

								if (multilingual_service_name_descriptor.LANGUAGE[0].service_name_length > 0)
								{
									strcpy_s(m_astServiceInfo[m_nServiceCount].pszServiceName,
										sizeof(m_astServiceInfo[m_nServiceCount].pszServiceName),
										multilingual_service_name_descriptor.LANGUAGE[0].trimmed_service_name_char);
								}

								if (multilingual_service_name_descriptor.LANGUAGE[0].service_provider_name_length > 0)
								{
									strcpy_s(m_astServiceInfo[m_nServiceCount].pszServiceProviderName,
										sizeof(m_astServiceInfo[m_nServiceCount].pszServiceProviderName),
										multilingual_service_name_descriptor.LANGUAGE[0].trimmed_service_provider_name_char);
								}
							}

						}
						else if (preserved_descriptor->descriptor_tag == DVB_SI_DATA_BROADCAST_DESCRIPTOR)
						{
							memset(&(m_astServiceInfo[m_nServiceCount].data_broadcast_descriptor[data_broadcast_count]), 0x00, sizeof(data_broadcast_descriptor_t));

							DVB_SI_decode_data_broadcast_descriptor(preserved_descriptor->descriptor_buf, 
														preserved_descriptor->descriptor_size,
														&(m_astServiceInfo[m_nServiceCount].data_broadcast_descriptor[data_broadcast_count]));

							data_broadcast_count ++;
						}
					}

					m_astServiceInfo[m_nServiceCount].data_broadcast_count = data_broadcast_count;

					m_nServiceCount ++;
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

int CSDT::GetTransportStreamID(void)
{
	return m_usTransportStreamID;
}


int CSDT::GetOriginalNetworkID(void)
{
	return m_usOriginalNetworkID;
}

int CSDT::GetServiceCount(void)
{
	return m_nServiceCount;
}

int	CSDT::GetServiceInfoByID(uint16_t service_id, SERVICE_INFO_t* pstServiceInfo)
{
	int rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	if (pstServiceInfo != NULL)
	{
		for (int i = 0; i < m_nServiceCount; i++)
		{
			if (m_astServiceInfo[i].service_id == service_id)
			{
				memcpy(pstServiceInfo, m_astServiceInfo + i, sizeof(SERVICE_INFO_t));
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

int CSDT::GetServiceInfoByIndex(int service_index, SERVICE_INFO_t* pstServiceInfo)
{
	int rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	if (pstServiceInfo != NULL)
	{
		if ((service_index >= 0) && (service_index < m_nServiceCount))
		{
			memcpy(pstServiceInfo, m_astServiceInfo + service_index, sizeof(SERVICE_INFO_t));
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


