#include <assert.h>
#include <string.h>
//#include <malloc.h>

//#include "../Include/MiddleWare_DB_PsiSiTables.h"
#include "../Include/MiddleWare_DB_OCDCs.h"
#include "../Include/MiddleWare_DB_ErrorCode.h"

CDB_OCDCs::CDB_OCDCs()
{
	Init();
}

CDB_OCDCs::~CDB_OCDCs()
{
	Reset();
}

void CDB_OCDCs::Init(void)
{
	m_nDownloadSessionCount = 0;
}

void CDB_OCDCs::Reset(void)
{
	int		download_index;

	for (download_index = 0; download_index < m_nDownloadSessionCount; download_index++)
	{
		m_astDownloadInfos[download_index].usCandidatePID = 0xffff;
	}
	m_nDownloadSessionCount = 0;
}


int CDB_OCDCs::AppendDownloadInfo(DOWNLOAD_INFO_t* pstDownloadInfo)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pstDownloadInfo != NULL)
	{
		if (m_nDownloadSessionCount < MAX_DOWNLOAD_SESSION)
		{
			m_nDownloadSessionCount++;

			memcpy(m_astDownloadInfos + m_nDownloadSessionCount - 1, pstDownloadInfo, sizeof(DOWNLOAD_INFO_t));
		}
		else
		{
			rtcode = MIDDLEWARE_DB_NO_ENOUGH_MEMORY;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

int CDB_OCDCs::GetDownloadCount(void)
{
	return m_nDownloadSessionCount;
}

int CDB_OCDCs::GetDownloadInfo(int index, DOWNLOAD_INFO_t* pstDownloadInfo)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if ((index >= 0 && index < m_nDownloadSessionCount) && (pstDownloadInfo != NULL))
	{
		memcpy(pstDownloadInfo, m_astDownloadInfos + index, sizeof(DOWNLOAD_INFO_t));
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}



