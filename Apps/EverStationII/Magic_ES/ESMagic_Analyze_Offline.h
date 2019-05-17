#ifndef _ESMAGIC_ANALYZE_OFFLINE_H_
#define _ESMAGIC_ANALYZE_OFFLINE_H_

//#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
//#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
//#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"
//#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
//#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

//#include "syntax_translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

//#include "TSMagic_Trigger_Section.h"
//#include "TSMagic_Trigger_TSPacket.h"
//#include "TSMagic_Trigger_PESES.h"

//#include "..\Common\Define.h"
#include <stdint.h>
#include <windows.h>

#define WM_ESMAGIC_APPEND_LOG					WM_USER + 0x275B
#define WM_ESMAGIC_REPORT_RATIO					WM_USER + 0x19B3
#define WM_ESMAGIC_ANALYZE_THREAD				WM_USER + 0x9963

/*--------------------------------------------------------------------
	Thread parameters' table
--------------------------------------------------------------------*/
typedef struct es_thread_params_s
{
	HWND		hCallerWnd;

	int			offline;

	//主线程的状态
	int			main_thread_running;			//用户控制
	int			main_thread_stopped;			//反馈状态

	//TS抽选线程
	int			packet_decimate_thread_running;		//用户控制
	int			packet_decimate_thread_stopped;		//反馈状态

	//实时流监测线程
	int			monitor_thread_running;
	int			monitor_thread_stopped;

	//DSMCC OC/DC下载线程
	int			dsmcc_download_thread_running;
	int			dsmcc_download_thread_stopped;

	char			pszFileName[MAX_PATH];



} es_thread_params_t, *pes_thread_params_t;

uint32_t ESMagic_analyze_thread(LPVOID lpParam);

#endif  //_ESMAGIC_ANALYZE_OFFLINE_H_
