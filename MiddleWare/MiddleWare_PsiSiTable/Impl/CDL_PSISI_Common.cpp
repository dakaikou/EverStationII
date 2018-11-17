#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"

#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "../Include/MiddleWare_PSISI_Common.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

/////////////////////////////////////////////
CPVT::CPVT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension)
{
	m_usKey = Key;
	m_usPID = PID;
	m_ucTableID = table_id;
	m_usTableIDExtension = table_id_extension;

	m_bNormalSectionSyntax = 0;
}

CPVT::~CPVT(void)
{
}

void CPVT::Init(void)
{
	m_aucSectionFlag = NULL;
	m_nSectionCount = 0;

	m_ucVersionNumber = 0xff;

	m_bCollectOver = 0;
}

void CPVT::Reset(void)
{
	m_ucVersionNumber		= 0xff;

	if (m_aucSectionFlag != NULL)
	{
		free(m_aucSectionFlag);
		m_aucSectionFlag = NULL;
	}
	m_nSectionCount = 0;

	m_bCollectOver		= 0;
}

uint16_t CPVT::GetKey(void)
{
	return m_usKey;
}

uint16_t CPVT::GetTableIDExtension(void)
{
	return m_usTableIDExtension;
}

uint8_t CPVT::GetTableID(void)
{
	return m_ucTableID;
}

uint16_t CPVT::GetPID(void)
{
	return m_usPID;
}

uint8_t CPVT::GetVersionNumber(void)
{
	return m_ucVersionNumber;
}

int CPVT::IsCollectOver(void)
{
	return m_bCollectOver;
}

int CPVT::IsNormalSectionSyntax(void)
{
	return m_bNormalSectionSyntax;
}

int CPVT::AddSection(uint16_t usPID, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;					//0 -- fail
	int i;
	int	collect_over;
	
	if (pprivate_section != NULL)
	{
		if ((usPID == m_usPID) && (pprivate_section->table_id == m_ucTableID))
		{
			if (m_bNormalSectionSyntax)
			{
				if (pprivate_section->table_id_extension == m_usTableIDExtension)
				{
					//如何验证section数据的正确性 ？？？？？？
					if (pprivate_section->CRC_32 == pprivate_section->CRC_32_verify)
					{
						if (m_bCollectOver == 0)
						{
							if (m_aucSectionFlag == NULL)
							{
								//说明是第一个DSMCC section
								m_nSectionCount = pprivate_section->last_section_number + 1;

								m_aucSectionFlag = (uint8_t*)calloc(m_nSectionCount, sizeof(uint8_t));
								if (m_aucSectionFlag != NULL)
								{
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
								//m_ucVersionNumber = pprivate_section->version_number;
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
					rtcode = MIDDLEWARE_PSISI_DISAGREE;
				}
			}
			else
			{
				rtcode = MIDDLEWARE_PSISI_NO_ERROR;
			}
		}
		else
		{
			rtcode = MIDDLEWARE_PSISI_DISAGREE;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_PSISI_PARAMETER_ERROR;
	}

	return rtcode;
}

