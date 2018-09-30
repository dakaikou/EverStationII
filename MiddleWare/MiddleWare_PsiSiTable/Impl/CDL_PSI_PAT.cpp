#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\Include\MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
CPAT::CPAT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CPAT::~CPAT(void)
{
	Reset();
}

void CPAT::Init(void)
{
	CPVT::Init();

	m_astProgram = NULL;
	m_nMemoryForProgramInfos = 0;
	m_nProgramCount = 0;

	Reset();
}

void CPAT::Reset(void)
{
	m_usTransportStreamID   = 0xffff;
	m_usNitPID			= 0xffff;

	if (m_astProgram != NULL)
	{
		free(m_astProgram);
		m_astProgram = NULL;
	}
	m_nMemoryForProgramInfos = 0;
	m_nProgramCount	= 0;

	CPVT::Reset();
}

int CPAT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	int i;
	int index;

	program_association_section_t		pat_section;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = MPEG2_PSI_PAT_DecodeSection(buf, length, &pat_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			if (m_bCollectOver == 1)
			{
				m_usTransportStreamID = pat_section.transport_stream_id;
			}

			m_astProgram = (PROGRAM_MAP_DESCRIPTION_t*)realloc(m_astProgram, (m_nProgramCount + pat_section.program_map_count) * sizeof(PROGRAM_MAP_DESCRIPTION_t));
			if (m_astProgram != NULL)
			{
				m_nMemoryForProgramInfos = (m_nProgramCount + pat_section.program_map_count) * sizeof(PROGRAM_MAP_DESCRIPTION_t);

				index = m_nProgramCount;
				for (i = 0; i < pat_section.program_map_count; i++)
				{
					if (pat_section.astProgramMaps[i].program_number == 0x0000)
					{
						m_usNitPID = pat_section.astProgramMaps[i].program_map_PID;
					}

					memcpy(m_astProgram + index, &(pat_section.astProgramMaps[i]), sizeof(PROGRAM_MAP_DESCRIPTION_t));
					index ++;
				}
				
				m_nProgramCount = index;
			}
			else
			{
				rtcode = MIDDLEWARE_PSISI_MEMORY_ERROR;
			}
		}
	}

	return rtcode;
}

uint16_t CPAT::GetNetworkPID(void)
{
	return m_usNitPID;
}

uint16_t CPAT::GetTransportStreamID(void)
{
	return m_usTransportStreamID;
}

uint16_t CPAT::GetProgramCount(void)
{
	return m_nProgramCount;
}

int CPAT::GetProgramByID(uint16_t program_number, PROGRAM_MAP_DESCRIPTION_t* pstProgramMap)
{
	int			i;
	int			rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	for (i = 0; i < m_nProgramCount; i++)
	{
		if (m_astProgram[i].program_number == program_number)
		{
			if (pstProgramMap != NULL)
			{
				memcpy(pstProgramMap, m_astProgram + i, sizeof(PROGRAM_MAP_DESCRIPTION_t));
			}

			rtcode = MIDDLEWARE_PSISI_NO_ERROR;

			break;
		}
	}

	return rtcode;
}

int CPAT::GetProgramByIndex(int index, PROGRAM_MAP_DESCRIPTION_t* pstProgramMap)
{
	int			rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	if ((index >= 0) && (index < m_nProgramCount))
	{
		if (pstProgramMap != NULL)
		{
			memcpy(pstProgramMap, m_astProgram + index, sizeof(PROGRAM_MAP_DESCRIPTION_t));

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

