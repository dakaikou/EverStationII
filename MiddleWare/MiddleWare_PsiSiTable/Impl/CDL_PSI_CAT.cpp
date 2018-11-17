#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"

/////////////////////////////////////////////
CCAT::CCAT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CCAT::~CCAT(void)
{
	Reset();
}

void CCAT::Init(void)
{
	CPVT::Init();

	m_astCASystemInfo = NULL;

	Reset();
}

void CCAT::Reset(void)
{
	if (m_astCASystemInfo != NULL)
	{
		free(m_astCASystemInfo);
		m_astCASystemInfo = NULL;
	}
	m_nMemoryForCASystemInfos = 0;
	m_nCASystemCount = 0;

	CPVT::Reset();
}

int CCAT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int		rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;							//0 -- fail

	CA_section_t	cat_section;
	
	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = MPEG2_PSI_CAT_DecodeSection(buf, length, &cat_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			CA_descriptor_t CA_descriptor;
			for (int descriptor_index = 0; descriptor_index < cat_section.CA_descriptor_count; descriptor_index++)
			{
				if (cat_section.CA_descriptors[descriptor_index].descriptor_tag == MPEG2_PSI_CA_DESCRIPTOR)
				{
					uint8_t* descriptor_buf = cat_section.CA_descriptors[descriptor_index].descriptor_buf;
					int		 descriptor_size = cat_section.CA_descriptors[descriptor_index].descriptor_size;

					if (MPEG2_PSI_decode_CA_descriptor(descriptor_buf, descriptor_size, &CA_descriptor) == SECTION_PARSE_NO_ERROR)
					{
						m_nCASystemCount++;
						m_astCASystemInfo = (CA_SYSTEM_INFO_t*)realloc(m_astCASystemInfo, m_nCASystemCount * sizeof(CA_SYSTEM_INFO_t));
						if (m_astCASystemInfo != NULL)
						{
							m_astCASystemInfo[m_nCASystemCount - 1].CA_system_PID = CA_descriptor.CA_system_ID;
							m_astCASystemInfo[m_nCASystemCount - 1].EMM_PID = CA_descriptor.CA_PID;
						}
					}
				}
			}
		}
	}

	return rtcode;
}

uint16_t CCAT::GetCASystemCount(void)
{
	return m_nCASystemCount;
}

int	CCAT::GetCASystemInfoByID(uint16_t CA_system_ID, CA_SYSTEM_INFO_t* pstCASystemInfo)
{
	int			i;
	int			rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	for (i = 0; i < m_nCASystemCount; i++)
	{
		if (m_astCASystemInfo[i].CA_system_PID == CA_system_ID)
		{
			if (pstCASystemInfo != NULL)
			{
				memcpy(pstCASystemInfo, m_astCASystemInfo + i, sizeof(CA_SYSTEM_INFO_t));

				rtcode = MIDDLEWARE_PSISI_NO_ERROR;
			}
			else
			{
				rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
			}

			break;
		}
	}

	return rtcode;
}

int	CCAT::GetCASystemInfoByIndex(int index, CA_SYSTEM_INFO_t* pstCASystemInfo)
{
	int rtcode = MIDDLEWARE_PSISI_NO_ERROR;

	if (pstCASystemInfo != NULL)
	{
		if ((index >= 0) && (index < m_nCASystemCount))
		{
			memcpy(pstCASystemInfo, m_astCASystemInfo + index, sizeof(CA_SYSTEM_INFO_t));
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

