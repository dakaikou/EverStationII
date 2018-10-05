#include "stdafx.h"

#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "libs_MPEG&DVB/MPEG_TSPacket/Include/Mpeg2_TS_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_TSPacket\Include\Mpeg2_TS_Utilities.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\nativeInclude\MPEG_DVB_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_DSMCC_Utilities.h"

#include "libs_Utilities\Include\XStream_Utilities.h"

#include "..\Common\define.h"

#include "../Magic_TS/TSMagicView.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_Decimate_Dsmcc.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <direct.h>
#include <math.h>
#include <assert.h>
#include <Shlwapi.h>

//DSMCC OC/DC下载线程
void ts_dsmcc_download_loop(pthread_params_t pThreadParams)
{
#if OPEN_DSMCC_DOWNLOAD

	S8	  pszDebug[MAX_TXT_CHARS];
	S32	  old_ratio = 0;

	CDB_PsiSiTables*			pDB_PsiSiTables = NULL;
	CDB_OCDCs*					pDB_OCDCs = NULL;

	DOWNLOAD_INFO_t				stDownloadInfo;
	S32							download_index;
	S64							total_file_size;							//所有需要下载文件的大小，K字节
	S64							download_file_size;
	S32							download_ratio;

	U32							old_tickcount;
	U32							new_tickcount;
	S32							diff_tickcount;

	if (pThreadParams != NULL)
	{
		pThreadParams->dsmcc_download_thread_running = 1;

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_THREAD, 1, NULL);

		old_tickcount = ::GetTickCount();

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 开始");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		pDB_PsiSiTables = pThreadParams->pDB_PsiSiTables;
		pDB_OCDCs = pThreadParams->pDB_OCDCs;

		//计算下载总的数据量
		total_file_size = 0;
		download_file_size = 0;
		//int download_file_count = pDB_OCDCs->GetDownloadCount();
		//for (download_index = 0; download_index < dc_download_count; download_index ++)
		//{
		//	pDCDownloadInfo = pDB_OCDCs->GetDCDownloadInfo(download_index);
		//	total_file_size += pDCDownloadInfo->moduleSize;
		//}

		//sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 需要下载%d K字节数据，共%d个文件", (int)(total_file_size / 1024), dc_download_count);
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

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 创建下载根目录 %s", pszDownloadRootPath);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

		BuildDirectory(pszDownloadRootPath);

		int download_count = pDB_OCDCs->GetDownloadCount();
		for (download_index = 0; download_index < download_count; download_index++)
		{
			pDB_OCDCs->GetDownloadInfo(download_index, &stDownloadInfo);

			sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 下载TS_PID=0x%04X携带的OC/DC数据", stDownloadInfo.usCandidatePID);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			pDB_PsiSiTables->DownloadDsmccTree(stDownloadInfo.usCandidatePID, pszDownloadRootPath);

			//判断应用程序是否中断了下载
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

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 结束(耗时%dms)", diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_THREAD, 0, NULL);
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
#endif
}

U32 TSMagic_dsmcc_download_thread(LPVOID lpParam)
{
	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;
	ts_dsmcc_download_loop(pThreadParams);

	return 0;
}


