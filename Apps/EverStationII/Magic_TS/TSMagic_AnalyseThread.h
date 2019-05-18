#ifndef _TSMAGIC_ANALYSE_THREAD_H_
#define _TSMAGIC_ANALYSE_THREAD_H_

#include <stdint.h>
#include <windef.h>

#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "syntax_translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

#include "TSMagic_Trigger_Section.h"
#include "TSMagic_Trigger_TSPacket.h"
#include "TSMagic_Trigger_PESES.h"

#define OPEN_TS_PACKET_ANALYZER			1

#define OPEN_PCR_ANALYZER				1 & OPEN_TS_PACKET_ANALYZER 
#define OPEN_PACKET_STATISTIC			1 & OPEN_TS_PACKET_ANALYZER
#define OPEN_SECTION_SPLICER			1 & OPEN_TS_PACKET_ANALYZER & OPEN_PACKET_STATISTIC

#define OPEN_SECTION_TRIGGER			1 & OPEN_TS_PACKET_ANALYZER & OPEN_SECTION_SPLICER
#define OPEN_TS_PACKET_TRIGGER			1 & OPEN_TS_PACKET_ANALYZER
#define OPEN_PES_PACKET_TRIGGER			1 & OPEN_TS_PACKET_ANALYZER

#define TS_FILE_READ_INTEGRITY_DIAGNOSIS	0
#define GUI_REPORT_FILE_ANALYSE_RATIO	1
#define GUI_REPORT_TS_STATISTIC			1 & GUI_REPORT_FILE_ANALYSE_RATIO
#define GUI_REPORT_TS_BITRATE			1
#define GUI_REPORT_PCR_DIAGNOSIS		1
#define GUI_REPORT_PCR_OBSERVATION		1

#define MAX_SECTION_FILTERS				16

/*--------------------------------------------------------------------
	Thread parameters' table
--------------------------------------------------------------------*/
typedef struct ts_thread_params_s
{
	HWND		hMainWnd;
	HWND		hPesEsMsgWnd;

	int			offline;

	//主线程的状态
	int			main_thread_running;			//用户控制，在界面层设置，在线程里只读
	int			main_thread_stopped;			//状态反馈，在线程里设置，在界面层只读

	//实时流监测线程
	int			monitor_thread_running;
	int			monitor_thread_stopped;

	//TS抽选线程
	int			packet_decimate_thread_running;		//用户控制
	int			packet_decimate_thread_stopped;		//反馈状态

	//DSMCC OC/DC下载线程
	int			dsmcc_download_thread_running;
	int			dsmcc_download_thread_stopped;

	//包捕捉状态标识
	int			ts_trigger_thread_running;
	int			ts_trigger_thread_stopped;
	int			pes_trigger_thread_running;
	int			pes_trigger_thread_stopped;
	//int			es_trigger_thread_running;
	//int			es_trigger_thread_stopped;
	int			section_trigger_thread_running;
	int			section_trigger_thread_stopped;

	int			thread_running;				//bit0 -- main thread (analyze)
											//bit1 -- ts catch thread
											//bit2 -- pes catch thread
	                                        //bit3 -- es catch thread
	                                        //bit4 -- section catch thread
											//bit5 -- dsmcc_download thread
											//bit6 -- packet_decimate thread


	//发现TS信号标识
	int			find_signal;

	int			stream_option;					//0 -- ts
												//1 -- audio pes
												//2 -- video pes
												//3 -- audio es
												//4 -- video es

//	int			standard_option;				//0 -- ATSC
												//1 -- DVB
												//2 -- NGB

	char			pszPathHeader[6];
	char			pszPathName[MAX_PATH];

	int				nDecimateStyle;							
	char			pszDecimatePath[MAX_PATH];

	char			pszVesFileName[MAX_PATH];
	char			pszAesFileName[MAX_PATH];

	CTransportStream*	   pTStream;

	CDB_TSPackets*		   pDB_TSPackets;
	CDB_Pcrs*			   pDB_Pcrs;
	CDB_PsiSiObjs*		   pDB_PsiSiObjs;
	CDB_OCDCs*			   pDB_OCDCs;

	CTrigger_PsiSiSection*  pTrigger_Section;
	CTrigger_TSPacket*		pTrigger_TSPacket;
	CTrigger_PESPacket*		pTrigger_PESPacket;

} ts_thread_params_t, *pts_thread_params_t;

uint32_t TSMagic_AnalyseThread_offline_main(LPVOID lpParam);
uint32_t TSMagic_AnalyseThread_realtime_main(LPVOID lpParam);
uint32_t TSMagic_AnalyseThread_monitor(LPVOID lpParam);

#endif  //_TSMAGIC_ANALYSE_THREAD_H_
