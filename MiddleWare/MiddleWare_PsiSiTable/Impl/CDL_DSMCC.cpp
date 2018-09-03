#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_section.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section\Include\MPEG_DVB_ErrorCode.h"

#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "../Include/MiddleWare_DSMCC_Table.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
CDSMCC::CDSMCC(void)
{
	Init();
}

CDSMCC::~CDSMCC(void)
{
	Reset();
}

void CDSMCC::Init(void)
{
	//S32		i;

	m_aucSectionFlag = NULL;
	Reset();
}

void CDSMCC::Reset(void)
{
	//S32		i;

	m_ucTableID			= 0xff;
	m_usTableIDExtension  = 0xffff;
	m_ucVersionNumber		= 0xff;
	m_ucNewVersionNumber	= 0xff;
//	m_ucCurSectionNumber  = 0;
//	m_ucLastSectionNumber = 0;

	if (m_aucSectionFlag != NULL)
	{
		free(m_aucSectionFlag);
		m_aucSectionFlag = NULL;
	}
	m_nMemoryForSectionFlags = 0;
	m_nSectionCount = 0;

	m_bCollectOver		= FALSE;

}

int CDSMCC::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;					//0 -- fail
	int i;
	int	collect_over;
	
	if ((buf != NULL) && (length >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (usPID == m_usPID) && (pprivate_section != NULL))
	{
		//如何验证section数据的正确性 ？？？？？？
		if (pprivate_section->CRC_32 == pprivate_section->CRC_32_verify)
		{
			if (!m_bCollectOver)
			{
				if (m_aucSectionFlag == NULL)
				{
					//说明是第一个DSMCC section
					m_nSectionCount = pprivate_section->last_section_number + 1;

					m_aucSectionFlag = (uint8_t*)calloc(m_nSectionCount, sizeof(uint8_t));
					if (m_aucSectionFlag != NULL)
					{
						m_nMemoryForSectionFlags = m_nSectionCount * sizeof(uint8_t);

						for (i = 0; i < m_nSectionCount; i++)
						{
							m_aucSectionFlag[i] = 0;
						}

						m_ucVersionNumber = pprivate_section->version_number;
					}
				}
					
				if (m_aucSectionFlag != NULL)
				{
					if (pprivate_section->section_number < m_nSectionCount)
					{
						if (m_aucSectionFlag[pprivate_section->section_number] == 0)
						{
							m_aucSectionFlag[pprivate_section->section_number] = 1;

							collect_over = 1;
							for (i = 0; i < m_nSectionCount; i++)
							{
								if (m_aucSectionFlag[i] == 0)
								{
									collect_over = 0;
									break;
								}
							}
				
							if (collect_over)
							{
								//如果搜集齐了
								m_bCollectOver = 1;
							}

							rtcode = MIDDLEWARE_PSISI_NO_ERROR;
						}
						else
						{
							//正常现象
							rtcode = MIDDLEWARE_PSISI_DUPLICATED_SECTION;
						}
					}
					else
					{
						rtcode = MIDDLEWARE_PSISI_SECTION_NUMBER_ERROR;
					}
				}
				else
				{
					rtcode = MIDDLEWARE_PSISI_MEMORY_ERROR;
				}
			}
			else
			{
				if (pprivate_section->version_number != m_ucVersionNumber)			//all of the datas need to be reset
				{
					m_ucNewVersionNumber = pprivate_section->version_number;
					rtcode = MIDDLEWARE_PSISI_VERSION_CHANGE;					//版本变更
				}
				else
				{
					//这是一种正常现象
					rtcode = MIDDLEWARE_PSISI_DUPLICATED_SECTION;
				}
			}
		}
		else
		{
			rtcode = MIDDLEWARE_PSISI_CRC_ERROR;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
	}

	return rtcode;
}

