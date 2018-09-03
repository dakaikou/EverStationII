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
	S32	i;

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
	S32		i;

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

S32 CTrigger::IsOpened(void)
{
	return m_bOpened;
}

S32 CTrigger::IsFull(void)
{
	return m_bFull;
}

//S32 CTrigger::GetFilterID(void)
//{
//	return m_nFilterID;
//}
//
S32 CTrigger::GetCatchedCount(void)
{
	return m_nGotCount;
}

U8*	CTrigger::GetCatchedDatas(S32 nIndex, S32* plength)
{
	U8* buf = NULL;
	if ((nIndex >= 0) && (nIndex < m_nGotCount))
	{
		*plength = m_nDataLength[nIndex];
		buf = m_ucDataBuf[nIndex];
	}

	return buf;
}

U8*	CTrigger::GetMatchMasks(S32* plength)
{
	U8* buf = m_ucMatchMasks;;
	*plength = m_nMatchLength;

	return buf;
}

U8*	CTrigger::GetMatchDatas(S32* plength)
{
	U8* buf = m_ucMatchDatas;;
	*plength = m_nMatchLength;

	return buf;
}

S32 CTrigger::SaveTheWholePacket(U8* buf, S32 length)
{
	S32				rtcode = TSMAGIC_PARAMETER_ERROR;

	if ((buf != NULL) && (length > 0))
	{
		if (!m_bFull)
		{
			assert(m_ucDataBuf[m_nGotCount] == NULL);

			m_ucDataBuf[m_nGotCount] = (U8*)malloc(length);
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

S32 CTrigger::SaveAsNewStart(U8* buf, S32 length)
{
	S32				handle = -1;

	if ((buf != NULL) && (length > 0))
	{
		if (!m_bFull)
		{
			handle = m_nGotCount;

			assert(m_ucDataBuf[m_nGotCount] == NULL);
			m_ucDataBuf[m_nGotCount] = (U8*)malloc(length);

			m_nDataLength[m_nGotCount] = length;

			if (m_ucDataBuf[m_nGotCount] != NULL)
			{
				memcpy(m_ucDataBuf[m_nGotCount], buf, length);
			}
		}
	}

	return handle;
}

S32 CTrigger::AppendToLast(S32 handle, U8* buf, S32 length)
{
	S32				rtcode = TSMAGIC_PARAMETER_ERROR;

	assert(handle == m_nGotCount);

	U8* new_buf;
	S32	new_size = 0;
	U8*	last_buf = m_ucDataBuf[handle];
	S32	last_length = m_nDataLength[handle];

	if ((buf != NULL) && (length > 0))
	{
		assert(last_buf != NULL);

		new_size = last_length + length;
		new_buf = (U8*)realloc(last_buf, new_size);
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

S32 CTrigger::SaveAndClose(S32 handle, U8* buf, S32 length)
{
	assert(handle == m_nGotCount);

	U8* new_buf;
	S32	new_size = 0;
	U8*	last_buf = m_ucDataBuf[handle];
	S32	last_length = m_nDataLength[handle];

	if ((buf != NULL) && (length > 0))
	{
		assert(last_buf != NULL);

		new_size = last_length + length;
		new_buf = (U8*)realloc(last_buf, new_size);
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

void CTrigger::SetMatchParams(U8* match_mask, U8* match_data, S32 match_len, S32 demand_count)
{
	S32 match_length;
	//	S32 i;

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

S32 CTrigger::IsMatched(U8* buf, S32 length)
{
	S32		 i;
	S32		 equal = -1;
	U8		 mask;
	U8		 data;
	U8		 src;
	U8		 dst;

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




