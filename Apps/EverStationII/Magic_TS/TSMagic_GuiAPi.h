#ifndef _MPEG2_GUIAPI_H_
#define _MPEG2_GUIAPI_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "libs_Mpeg&DVB/Mpeg_TSPacket/Include/Mpeg2_TS_packet.h"

#include "TSMagic_Trigger_Section.h"
#include "TSMagic_Trigger_TSPacket.h"
#include "TSMagic_Trigger_PESES.h"

#include "..\Common\Define.h"

#define GUI_TS_ANALYZER_OVERVIEW			1
#define GUI_TS_ANALYZER_NETWORK				0
#define GUI_TS_ANALYZER_EPG					1
#define GUI_TS_ANALYZER_PCR					1
#define GUI_TS_ANALYZER_PACKETS				1
#define GUI_TS_ANALYZER_PSISI				1
#define GUI_TS_ANALYZER_BOUQUETS			1
#define GUI_TS_ANALYZER_PESES				1
#define GUI_TS_ANALYZER_ES					0
#define GUI_TS_ANALYZER_TELETEXT			0
#define GUI_TS_ANALYZER_SUBTITLE			0
#define GUI_TS_ANALYZER_OCDC				1
#define GUI_TS_ANALYZER_PK_TRIGGER			1
#define GUI_TS_PTSDTS						1
#define GUI_TS_PK_MODIFY					0
#define GUI_TS_SEC_TRIGGER					0

#define DTSPTS_FIFO_LENGTH					256

#define	MAX_PICTURE_COUNT					40

/*--------------------------------------------------------------------
	Thread parameters' table
--------------------------------------------------------------------*/
typedef struct thread_params_s
{
	HWND		hMainWnd;
	HWND		hPesEsMsgWnd;

	S32			offline;

	//主线程的状态
	S32			main_thread_running;			//用户控制
	S32			main_thread_stopped;			//反馈状态

	//TS抽选线程
	S32			packet_decimate_thread_running;		//用户控制
	S32			packet_decimate_thread_stopped;		//反馈状态

	//实时流监测线程
	S32			monitor_thread_running;
	S32			monitor_thread_stopped;

	//DSMCC OC/DC下载线程
	S32			dsmcc_download_thread_running;
	S32			dsmcc_download_thread_stopped;

	S32			ts_trigger_thread_running;				
//	S32			ts_trigger_thread_stopped;

	S32			pes_trigger_thread_running;
//	S32			pes_trigger_thread_stopped;

	S32			es_trigger_thread_running;
	S32			es_trigger_thread_stopped;

	S32			section_trigger_thread_running;
//	S32			section_trigger_thread_stopped;

//	S32			triggering;

	S32			find_signal;

	S32			stream_option;					//0 -- ts
												//1 -- audio pes
												//2 -- video pes
												//3 -- audio es
												//4 -- video es

//	S32			standard_option;				//0 -- ATSC
												//1 -- DVB
												//2 -- NGB

	S8			pszFileName[MAX_PATH];

	//CESDecoder* pVidDecoder;
	//CESDecoder* pAudDecoder;

	S32			nDecimateStyle;							
	S8			pszDecimatePath[MAX_PATH];

	S8			pszVesFileName[MAX_PATH];
	S8			pszAesFileName[MAX_PATH];

	CTransportStream*	   pTStream;

	CDB_TSPackets*		   pDB_TSPackets;
	CDB_Pcrs*			   pDB_Pcrs;
	CDB_PsiSiTables*	   pDB_PsiSiTables;
	CDB_OCDCs*			   pDB_OCDCs;

	CTrigger_PsiSiSection*  pTrigger_Section;
	CTrigger_TSPacket*		pTrigger_TSPacket;
	CTrigger_PESPacket*		pTrigger_PESPacket;

} thread_params_t, *pthread_params_t;

U32 TSMagic_offline_thread(LPVOID lpParam);
U32 TSMagic_realtime_analyzer(LPVOID lpParam);
U32 TSMagic_realtime_monitor(LPVOID lpParam);

void TSMagic_threadparams_init(thread_params_t* pthread_params);
void TSMagic_threadparams_reset(thread_params_t* pthread_params);
//void TSMagic_threadparams_regist_esdecoder(thread_params_t* pthread_params, int type, CESDecoder* pDecoder);
//void TSMagic_threadparams_unregist_esdecoder(thread_params_t* pthread_params, int type);

#endif
