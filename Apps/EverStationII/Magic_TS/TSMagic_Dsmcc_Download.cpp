#include "stdafx.h"

#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_TSPacket\Include\Mpeg2_TS_Utilities.h"
#include "syntax_translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"

#include "Utilities\Directory\Include\TOOL_Directory.h"

#include "..\Common\define.h"

#include "../Magic_TS/TSMagicView.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_Dsmcc_Download.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
//#include <direct.h>
#include <math.h>
#include <assert.h>
#include <Shlwapi.h>

//DSMCC OC/DC�����߳�
void ts_dsmcc_download_loop(pthread_params_t pThreadParams)
{
#if OPEN_DSMCC_DOWNLOAD

	char  pszDebug[MAX_TXT_CHARS];
	int	  old_ratio = 0;

	CDB_PsiSiObjs*				pDB_PsiSiObjs = NULL;
	CDB_OCDCs*					pDB_OCDCs = NULL;

	DOWNLOAD_INFO_t				stDownloadInfo;
	int							download_index;
	int64_t							total_file_size;							//������Ҫ�����ļ��Ĵ�С��K�ֽ�
	int64_t							download_file_size;
	//int							download_ratio;

	uint32_t							old_tickcount;
	uint32_t							new_tickcount;
	int							diff_tickcount;

	if (pThreadParams != NULL)
	{
		pThreadParams->dsmcc_download_thread_running = 1;

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_THREAD, 1, NULL);

		old_tickcount = ::GetTickCount();

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC����: ��ʼ");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		pDB_PsiSiObjs = pThreadParams->pDB_PsiSiObjs;
		pDB_OCDCs = pThreadParams->pDB_OCDCs;

		//���������ܵ�������
		total_file_size = 0;
		download_file_size = 0;
		//int download_file_count = pDB_OCDCs->GetDownloadCount();
		//for (download_index = 0; download_index < dc_download_count; download_index ++)
		//{
		//	pDCDownloadInfo = pDB_OCDCs->GetDCDownloadInfo(download_index);
		//	total_file_size += pDCDownloadInfo->moduleSize;
		//}

		//sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC����: ��Ҫ����%d K�ֽ����ݣ���%d���ļ�", (int)(total_file_size / 1024), dc_download_count);
		//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
		//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

		char	pszExeFile[MAX_PATH];
		char	exeDrive[3];
		char	pszDownloadRootPath[MAX_PATH];
		GetModuleFileName(NULL, pszExeFile, MAX_PATH);
		exeDrive[0] = pszExeFile[0];
		exeDrive[1] = pszExeFile[1];
		exeDrive[2] = '\0';

		sprintf_s(pszDownloadRootPath, sizeof(pszDownloadRootPath), "%s\\~EverStationII\\download_OCDC", exeDrive);

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC����: �������ظ�Ŀ¼ %s", pszDownloadRootPath);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

		DIR_BuildDirectory(pszDownloadRootPath);

		int download_count = pDB_OCDCs->GetDownloadCount();
		for (download_index = 0; download_index < download_count; download_index++)
		{
			pDB_OCDCs->GetDownloadInfo(download_index, &stDownloadInfo);

			sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC����: ����TS_PID=0x%04XЯ����OC/DC����", stDownloadInfo.usCandidatePID);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			pDB_PsiSiObjs->DownloadDsmccTree(stDownloadInfo.usCandidatePID, pszDownloadRootPath);

			//�ж�Ӧ�ó����Ƿ��ж�������
			if (pThreadParams->dsmcc_download_thread_running == 0)
			{
				break;
			}
		}

		pDB_OCDCs->Reset();

		pThreadParams->dsmcc_download_thread_running = 0;
		pThreadParams->dsmcc_download_thread_stopped = 1;

		new_tickcount = ::GetTickCount();
		diff_tickcount = new_tickcount - old_tickcount;

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC����: ����(��ʱ%dms)", diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_THREAD, 0, NULL);
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC����: �����������");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
#endif
}

uint32_t TSMagic_dsmcc_download_thread(LPVOID lpParam)
{
	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;
	ts_dsmcc_download_loop(pThreadParams);

	return 0;
}


