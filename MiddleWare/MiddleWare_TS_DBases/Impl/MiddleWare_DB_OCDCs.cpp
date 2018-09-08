#include <assert.h>
#include <string.h>
#include <malloc.h>

#include "../Include/MiddleWare_DB_PsiSiTables.h"
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
	//m_memory_used = 0;

	//m_dc_download_count = 0;
	//m_astDCDownloadInfos = NULL;

	//m_oc_download_count = 0;
	//m_astOCDownloadInfos = NULL;
}

void CDB_OCDCs::Reset(void)
{
	int		download_index;

	for (download_index = 0; download_index < m_nDownloadSessionCount; download_index++)
	{
		m_astDownloadInfos[download_index].usCandidatePID = 0xffff;
	}
	m_nDownloadSessionCount = 0;

	//for (download_index = 0; download_index < m_dc_download_count; download_index++)
	//{
	//	if (m_astDCDownloadInfos[download_index].blockFlags != NULL)
	//	{
	//		free(m_astDCDownloadInfos[download_index].blockFlags);
	//	}
	//	m_astDCDownloadInfos[download_index].blockFlags = NULL;
	//	m_astDCDownloadInfos[download_index].blockCount = 0;

	//	if (m_astDCDownloadInfos[download_index].fp != NULL)
	//	{
	//		fclose(m_astDCDownloadInfos[download_index].fp);
	//	}
	//}

	//if (m_astDCDownloadInfos != NULL)
	//{
	//	free(m_astDCDownloadInfos);
	//	m_astDCDownloadInfos = NULL;
	//}

	//m_memory_for_dsmcc_dc_download = 0;

	//m_dc_download_count = 0;
	//m_dc_download_over = 0;

	//m_dsmcc_download_enable = 0;

	//for (download_index = 0; download_index < m_oc_download_count; download_index++)
	//{
	//	if (m_astOCDownloadInfos[download_index].fp != NULL)
	//	{
	//		fclose(m_astOCDownloadInfos[download_index].fp);
	//	}
	//}

	//if (m_astOCDownloadInfos != NULL)
	//{
	//	free(m_astOCDownloadInfos);
	//	m_astOCDownloadInfos = NULL;
	//}

	//m_memory_for_dsmcc_oc_download = 0;

	//m_oc_download_count = 0;
	//m_oc_download_over = 0;

	//m_memory_used = 0;
}

//int CDB_OCDCs::AddOCDownloadInfo(uint16_t usPID, uint32_t downloadID, uint32_t objectKey_data, uint16_t moduleID, char* pszFileName)
//{
//	int rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;
//	OC_DOWNLOAD_INFO_t*			pOCDownloadInfo = NULL;
//
//	m_astOCDownloadInfos = (OC_DOWNLOAD_INFO_t*)realloc(m_astOCDownloadInfos, (m_oc_download_count + 1) * sizeof(OC_DOWNLOAD_INFO_t));
//
//	if (m_astOCDownloadInfos != NULL)
//	{
//		m_oc_download_count++;
//		m_memory_for_dsmcc_oc_download = m_oc_download_count * sizeof(OC_DOWNLOAD_INFO_t);
//
//		pOCDownloadInfo = &(m_astOCDownloadInfos[m_oc_download_count - 1]);
//		memset(pOCDownloadInfo, 0x00, sizeof(OC_DOWNLOAD_INFO_t));
//
//		strcpy_s(pOCDownloadInfo->pszFileName, sizeof(pOCDownloadInfo->pszFileName), pszFileName);
//		pOCDownloadInfo->PID = usPID;
//		pOCDownloadInfo->downloadId = downloadID;
//		pOCDownloadInfo->objectKey_data = objectKey_data;
//		pOCDownloadInfo->moduleId = moduleID;
//
//		rtcode = MIDDLEWARE_DB_NO_ERROR;
//	}
//	else
//	{
//		rtcode = MIDDLEWARE_DB_NO_ENOUGH_MEMORY;
//	}
//
//	return rtcode;
//}


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

//int CDB_OCDCs::AddDCDownloadInfo(int oc_flag, uint16_t usPID, uint32_t downloadID, int blockSize, uint16_t moduleID, int moduleSize, int moduleVersion, char* pszModuleName)
//{
//	int rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;
//	DC_DOWNLOAD_INFO_t* pDCDownloadInfo = NULL;
//
//	m_astDCDownloadInfos = (DC_DOWNLOAD_INFO_t*)realloc(m_astDCDownloadInfos, (m_dc_download_count + 1) * sizeof(DC_DOWNLOAD_INFO_t));
//
//	if (m_astDCDownloadInfos != NULL)
//	{
//		m_dc_download_count++;
//		m_memory_for_dsmcc_dc_download = m_dc_download_count * sizeof(DC_DOWNLOAD_INFO_t);
//
//		pDCDownloadInfo = &(m_astDCDownloadInfos[m_dc_download_count - 1]);
//
//		memset(pDCDownloadInfo, 0x00, sizeof(DC_DOWNLOAD_INFO_t));
//
//		pDCDownloadInfo->oc_flag = oc_flag;
//		pDCDownloadInfo->PID = usPID;
//		pDCDownloadInfo->downloadId = downloadID;
//
//		pDCDownloadInfo->blockSize = blockSize;
//		pDCDownloadInfo->moduleId = moduleID;
//		pDCDownloadInfo->moduleSize = moduleSize;
//		pDCDownloadInfo->moduleVersion = moduleVersion;
//
//		strcpy_s(pDCDownloadInfo->moduleName, sizeof(pDCDownloadInfo->moduleName), pszModuleName);
//
//		rtcode = MIDDLEWARE_DB_NO_ERROR;
//	}
//	else
//	{
//		rtcode = MIDDLEWARE_DB_NO_ENOUGH_MEMORY;
//	}
//
//	return rtcode;
//}


int CDB_OCDCs::GetDownloadCount(void)
{
	return m_nDownloadSessionCount;
}

//int CDB_OCDCs::GetDCDownloadCount(void)
//{
//	return m_dc_download_count;
//}

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

//int CDB_OCDCs::GetOCDownloadCount(void)
//{
//	return m_oc_download_count;
//}
//
//OC_DOWNLOAD_INFO_t* CDB_OCDCs::GetOCDownloadInfo(int index)
//{
//	return m_astOCDownloadInfos + index;
//}


//DC_DOWNLOAD_INFO_t* CDB_OCDCs::GetDCDownloadInfo(int index)
//{
//	return m_astDCDownloadInfos + index;
//}
//
//int CDB_OCDCs::GetOCDownloadCount(void)
//{
//	return m_oc_download_count;
//}
//
//OC_DOWNLOAD_INFO_t* CDB_OCDCs::GetOCDownloadInfo(int index)
//{
//	return m_astOCDownloadInfos + index;
//}

//void RSISI_REPORT_check_memory(HWND	hMsgWnd)
//{
//	S32		memory_used;
//	S32		i;
//	S8		pszDebug[256];
//
//	PSISI_REPORT.memory_used = 0;
//
//	//ͳ��PAT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.pat_count; i++)
//	{
//		memory_used += PSISI_REPORT.astPAT[i].m_nMemoryForSectionFlags;
//		memory_used += PSISI_REPORT.astPAT[i].m_nMemoryForProgramInfos;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��PAT, PAT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.pat_count, memory_used, PSISI_REPORT.memory_for_pat);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��PMT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.pmt_count; i++)
//	{
//		memory_used += PSISI_REPORT.astPMT[i].m_nMemoryForEsInfos;
//		memory_used += PSISI_REPORT.astPMT[i].m_nMemoryForSectionFlags;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��PMT, PMT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.pmt_count, memory_used, PSISI_REPORT.memory_for_pmt);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��NIT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.nit_count; i++)
//	{
//		memory_used += PSISI_REPORT.astNIT[i].m_nMemoryForStreamInfos;
//		memory_used += PSISI_REPORT.astNIT[i].m_nMemoryForSectionFlags;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��NIT, NIT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.nit_count, memory_used, PSISI_REPORT.memory_for_nit);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��SDT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.sdt_count; i++)
//	{
//		memory_used += PSISI_REPORT.astSDT[i].m_nMemoryForServiceInfos;
//		memory_used += PSISI_REPORT.astSDT[i].m_nMemoryForSectionFlags;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��SDT, SDT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.sdt_count, memory_used, PSISI_REPORT.memory_for_sdt);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��EIT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.eit_count; i++)
//	{
//		memory_used += PSISI_REPORT.astEIT[i].m_nMemoryForEvents;
//		memory_used += PSISI_REPORT.astEIT[i].m_nMemoryForSegments;
//		memory_used += PSISI_REPORT.astEIT[i].m_nMemoryForSectionFlags;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��EIT, EIT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.eit_count, memory_used, PSISI_REPORT.memory_for_eit);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��BAT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.bat_count; i++)
//	{
//		memory_used += PSISI_REPORT.astBAT[i].m_nMemoryForStreamInfos;
//		memory_used += PSISI_REPORT.astBAT[i].m_nMemoryForSectionFlags;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��BAT, BAT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.bat_count, memory_used, PSISI_REPORT.memory_for_bat);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��AIT
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.ait_count; i++)
//	{
//		memory_used += PSISI_REPORT.astAIT[i].m_nMemoryForSectionFlags;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��AIT, AIT����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.ait_count, memory_used, PSISI_REPORT.memory_for_ait);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
//	//ͳ��MPE
//	memory_used = 0;
////		memory_used += PSISI_REPORT.memory_for_mpe;
////		sprintf_s(pszDebug, sizeof(pszDebug), "MPE�ڴ�ռ�ã�lib=%d, gui=%d �ֽ�", memory_used, PSISI_REPORT.memory_for_mpe);
////		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += PSISI_REPORT.memory_for_mpe;
//
//	//ͳ��DSM_CC
//	memory_used = 0;
//	for (i = 0; i < PSISI_REPORT.dsmcc_count; i++)
//	{
//		memory_used += PSISI_REPORT.astDSMCC[i].m_nMemoryForDirMessages;
//		memory_used += PSISI_REPORT.astDSMCC[i].m_nMemoryForFileMessages;
//		memory_used += PSISI_REPORT.astDSMCC[i].m_nMemoryForGroupInfos;
//		memory_used += PSISI_REPORT.astDSMCC[i].m_nMemoryForModuleInfos;
//		memory_used += PSISI_REPORT.astDSMCC[i].m_nMemoryForSectionFlags;
////		memory_used += PSISI_REPORT.astDSMCC[i].m_nMemoryForBlockBuf;
//	}
//	sprintf_s(pszDebug, sizeof(pszDebug), "����%d��DSMCC, DSMCC����ռ���ڴ棺lib=%d, gui=%d �ֽ�", PSISI_REPORT.dsmcc_count, memory_used, PSISI_REPORT.memory_for_dsmcc);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	PSISI_REPORT.memory_used += memory_used;
//
////		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC - DC�����ڴ�ռ�� gui=%d �ֽ�", PSISI_REPORT.memory_for_dsmcc_dc_download);
////		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
////		PSISI_REPORT.memory_used += PSISI_REPORT.memory_for_dsmcc_dc_download;
//
////		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC - OC�����ڴ�ռ�� gui=%d �ֽ�", PSISI_REPORT.memory_for_dsmcc_oc_download);
////		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
////		PSISI_REPORT.memory_used += PSISI_REPORT.memory_for_dsmcc_oc_download;
//
//	sprintf_s(pszDebug, sizeof(pszDebug), "��ռ���ڴ棺%d�ֽ�", PSISI_REPORT.memory_used);
//	::SendMessage(hMsgWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//}


