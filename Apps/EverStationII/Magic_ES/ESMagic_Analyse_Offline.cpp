#include "stdafx.h"

//#include "..\Common\define.h"

//#include "TSMagic_Callbacks_From_Lib.h"
//#include "TSMagic_Callbacks_To_Gui.h"

#include "ESMagic_Analyze_Offline.h"
//#include "TSMagic_GuiApi_MSG.h"
//#include "TSMagic_ErrorCode.h"
//
//#include "TSMagic_Trigger.h"
//#include "TSMagic_Trigger_Section.h"
//
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_ErrorCode.h"
//#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
//#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_PESSplicer.h"
//#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/MiddleWare_SectionSplicer_ErrorCode.h"
//#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
//#include "MiddleWare/MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
//#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
//
//#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
//#include "syntax_translate_layer/MPEG2_TSPacket\Include\Mpeg2_TS_Utilities.h"

//#include "MiddleWare_ESDecoder/Include/VideoDecoder_Mpeg2.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_H264.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_AVS.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_Mpeg2.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AC3.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AAC.h"

#include "thirdparty_libs\glog\glog\logging.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

void offline_es_analyze_loop(es_thread_params_t* pThreadParams)
{
	char	  pszDebug[MAX_PATH];
	char	  pszTemp[128];

	double file_size_div_100;
	int64_t	  read_byte_pos = 0;			//只能读小于2G的文件

	int			  old_ratio = 0;
	int			  analyse_ratio = 0;
	uint32_t	  thread_start_tickcount;
	uint32_t	  thread_finish_tickcount;
	time_t		  syncread_start_time;
	int			  diff_tickcount;

	int	  stream_synced = 0;

	int					rtcode;
	int					filter_index;

	if (pThreadParams != NULL)
	{
		assert(pThreadParams->main_thread_stopped == 0);
		pThreadParams->main_thread_running = 1;

		thread_start_tickcount = ::GetTickCount();

		::SendMessage(pThreadParams->hCallerWnd, WM_ESMAGIC_ANALYZE_THREAD, 1, 0);

		sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 开始(文件名：%s)", pThreadParams->pszFileName);
		//::SendMessage(pThreadParams->hMainWnd, WM_ESMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		LOG(INFO) << pszDebug;

		for (int step = 0; step < 100; step++)
		{
			::SendMessage(pThreadParams->hCallerWnd, WM_ESMAGIC_ANALYZE_THREAD, 1, step);
			Sleep(500);
		}

		::SendMessage(pThreadParams->hCallerWnd, WM_ESMAGIC_ANALYZE_THREAD, 0, 0);

		pThreadParams->main_thread_stopped = 1;			//通知应用程序，主线程顺利退出
	}
	else
	{
		sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 输入参数错误！");
		//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
		LOG(INFO) << pszDebug;
	}
}

uint32_t ESMagic_analyze_thread(LPVOID lpParam)
{
	es_thread_params_t*	pThreadParams = (es_thread_params_t*)lpParam;

	offline_es_analyze_loop(pThreadParams);

	return 0;
}


