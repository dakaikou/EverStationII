#include "StdAfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "TSMagic_ErrorCode.h"
#include "TSMagic_Trigger.h"

CTrigger::CTrigger(void)
{
	int	i;

	m_bOpened = 0;
	m_bFull = 0;

	m_nGotCount = 0;
	m_nDemandCount = 0;
//	m_nBrowseIndex = 0;

	for (i = 0; i < MAX_TRIGGER_COUNT; i++)
	{
		m_nDataLength[i] = 0;
		m_ucDataBuf[i] = NULL;
	}

	m_nMatchLength = 0;
	memset(m_ucMatchMasks, 0x00, sizeof(m_ucMatchMasks));
	memset(m_ucMatchDatas, 0x00, sizeof(m_ucMatchDatas));
}

CTrigger::~CTrigger(void)
{
	Reset();
}

void CTrigger::Reset(void)
{
	int		i;

	for (i = 0; i < m_nGotCount; i++)
	{
		if (m_ucDataBuf[i] != NULL)
		{
			free(m_ucDataBuf[i]);
			m_ucDataBuf[i] = NULL;
		}

		m_nDataLength[i] = 0;
	}

	m_bOpened = 0;
	m_bFull = 0;

	m_nGotCount = 0;
	m_nDemandCount = 0;
//	m_nBrowseIndex = 0;

	//	TRIGGER_PSISI_SECTION.PID = 0xffff;
	m_nMatchLength = 0;
	memset(m_ucMatchMasks, 0x00, sizeof(m_ucMatchMasks));
	memset(m_ucMatchDatas, 0x00, sizeof(m_ucMatchDatas));
}

int CTrigger::IsOpened(void)
{
	return m_bOpened;
}

int CTrigger::IsFull(void)
{
	return m_bFull;
}

//int CTrigger::GetFilterID(void)
//{
//	return m_nFilterID;
//}
//
int CTrigger::GetCatchedCount(void)
{
	return m_nGotCount;
}

uint8_t*	CTrigger::GetCatchedDatas(int nIndex, int* plength)
{
	uint8_t* buf = NULL;
	if ((nIndex >= 0) && (nIndex < m_nGotCount))
	{
		*plength = m_nDataLength[nIndex];
		buf = m_ucDataBuf[nIndex];
	}

	return buf;
}

uint8_t*	CTrigger::GetMatchMasks(int* plength)
{
	uint8_t* buf = m_ucMatchMasks;;
	*plength = m_nMatchLength;

	return buf;
}

uint8_t*	CTrigger::GetMatchDatas(int* plength)
{
	uint8_t* buf = m_ucMatchDatas;;
	*plength = m_nMatchLength;

	return buf;
}

int CTrigger::SaveTheWholePacket(uint8_t* buf, int length)
{
	int				rtcode = TSMAGIC_PARAMETER_ERROR;

	if ((buf != NULL) && (length > 0))
	{
		if (!m_bFull)
		{
			assert(m_ucDataBuf[m_nGotCount] == NULL);

			m_ucDataBuf[m_nGotCount] = (uint8_t*)malloc(length);
			m_nDataLength[m_nGotCount] = length;

			if (m_ucDataBuf[m_nGotCount] != NULL)
			{
				memcpy(m_ucDataBuf[m_nGotCount], buf, length);
			}

			m_nGotCount++;
			if (m_nGotCount >= m_nDemandCount)
			{
				m_bFull = 1;
			}

			rtcode = TSMAGIC_NO_ERROR;
		}
	}

	return rtcode;
}

int CTrigger::SaveAsNewStart(uint8_t* buf, int length)
{
	int				handle = -1;

	if ((buf != NULL) && (length > 0))
	{
		if (!m_bFull)
		{
			handle = m_nGotCount;

			assert(m_ucDataBuf[m_nGotCount] == NULL);
			m_ucDataBuf[m_nGotCount] = (uint8_t*)malloc(length);

			m_nDataLength[m_nGotCount] = length;

			if (m_ucDataBuf[m_nGotCount] != NULL)
			{
				memcpy(m_ucDataBuf[m_nGotCount], buf, length);
			}
		}
	}

	return handle;
}

int CTrigger::AppendToLast(int handle, uint8_t* buf, int length)
{
	int				rtcode = TSMAGIC_PARAMETER_ERROR;

	assert(handle == m_nGotCount);

	uint8_t* new_buf;
	int	new_size = 0;
	uint8_t*	last_buf = m_ucDataBuf[handle];
	int	last_length = m_nDataLength[handle];

	if ((buf != NULL) && (length > 0))
	{
		assert(last_buf != NULL);

		new_size = last_length + length;
		new_buf = (uint8_t*)realloc(last_buf, new_size);
		if (new_buf != NULL)
		{
			memcpy(new_buf + last_length, buf, length);
		}
		m_ucDataBuf[handle] = new_buf;
		m_nDataLength[handle] = new_size;

		rtcode = TSMAGIC_NO_ERROR;
	}

	return rtcode;
}

int CTrigger::SaveAndClose(int handle, uint8_t* buf, int length)
{
	assert(handle == m_nGotCount);

	uint8_t* new_buf;
	int	new_size = 0;
	uint8_t*	last_buf = m_ucDataBuf[handle];
	int	last_length = m_nDataLength[handle];

	if ((buf != NULL) && (length > 0))
	{
		assert(last_buf != NULL);

		new_size = last_length + length;
		new_buf = (uint8_t*)realloc(last_buf, new_size);
		if (new_buf != NULL)
		{
			memcpy(new_buf + last_length, buf, length);
		}
		m_ucDataBuf[handle] = new_buf;
		m_nDataLength[handle] = new_size;
	}

	m_nGotCount++;
	if (m_nGotCount >= m_nDemandCount)
	{
		m_bFull = 1;
	}

	return 0;
}

void CTrigger::SetMatchParams(uint8_t* match_mask, uint8_t* match_data, int match_len, int demand_count)
{
	int match_length;
	//	int i;

	Reset();

	match_length = (match_len < sizeof(m_ucMatchMasks)) ? match_len : sizeof(m_ucMatchMasks);

	if (match_mask != NULL)
	{
		memcpy(m_ucMatchMasks, match_mask, match_length);
	}
	if (match_data != NULL)
	{
		memcpy(m_ucMatchDatas, match_data, match_length);
	}
	m_nMatchLength = match_length;

	m_nDemandCount = demand_count;

	m_bOpened = 1;
	//	m_bFull = 0;
}

int CTrigger::IsMatched(uint8_t* buf, int length)
{
	int		 i;
	int		 equal = -1;
	uint8_t		 mask;
	uint8_t		 data;
	uint8_t		 src;
	uint8_t		 dst;

	if ((buf != NULL) && (length >= m_nMatchLength))
	{
		for (i = 0; i < m_nMatchLength; i++)
		{
			mask = m_ucMatchMasks[i];
			data = m_ucMatchDatas[i];

			src = buf[i] & mask;
			dst = data & mask;
			if (src != dst)
			{
				equal = 0;
				break;
			}
		}
	}

	return equal;
}




