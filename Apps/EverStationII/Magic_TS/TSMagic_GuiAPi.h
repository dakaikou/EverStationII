#ifndef _MPEG2_GUIAPI_H_
#define _MPEG2_GUIAPI_H_

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

	int			offline;

	//���̵߳�״̬
	int			main_thread_running;			//�û�����
	int			main_thread_stopped;			//����״̬

	//TS��ѡ�߳�
	int			packet_decimate_thread_running;		//�û�����
	int			packet_decimate_thread_stopped;		//����״̬

	//ʵʱ������߳�
	int			monitor_thread_running;
	int			monitor_thread_stopped;

	//DSMCC OC/DC�����߳�
	int			dsmcc_download_thread_running;
	int			dsmcc_download_thread_stopped;

	int			ts_trigger_thread_running;				
//	int			ts_trigger_thread_stopped;

	int			pes_trigger_thread_running;
//	int			pes_trigger_thread_stopped;

	int			es_trigger_thread_running;
	int			es_trigger_thread_stopped;

	int			section_trigger_thread_running;
//	int			section_trigger_thread_stopped;

//	int			triggering;

	int			find_signal;

	int			stream_option;					//0 -- ts
												//1 -- audio pes
												//2 -- video pes
												//3 -- audio es
												//4 -- video es

//	int			standard_option;				//0 -- ATSC
												//1 -- DVB
												//2 -- NGB

	char			pszFileName[MAX_PATH];

	//CESDecoder* pVidDecoder;
	//CESDecoder* pAudDecoder;

	int			nDecimateStyle;							
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

} thread_params_t, *pthread_params_t;

uint32_t TSMagic_offline_thread(LPVOID lpParam);
uint32_t TSMagic_realtime_analyzer(LPVOID lpParam);
uint32_t TSMagic_realtime_monitor(LPVOID lpParam);

void TSMagic_threadparams_init(thread_params_t* pthread_params);
void TSMagic_threadparams_reset(thread_params_t* pthread_params);
//void TSMagic_threadparams_regist_esdecoder(thread_params_t* pthread_params, int type, CESDecoder* pDecoder);
//void TSMagic_threadparams_unregist_esdecoder(thread_params_t* pthread_params, int type);

#endif
