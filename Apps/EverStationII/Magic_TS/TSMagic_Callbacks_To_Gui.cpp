#include <stdafx.h>
#include <assert.h>

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_ErrorCode.H"

#include "TSMagic_Callbacks_To_Gui.h"
#include "TSMagic_Trigger_Section.h"

#include "..\Magic_TS\TSMagicView.h"

#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_ErrorCode.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Utilities.h"
#include "syntax_translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_Utilities.h"

void REPORT_TS_packet_statistic(WPARAM wParam, LPARAM lParam)
{
#if GUI_REPORT_TS_STATISTIC

	CTSMagicView* pWindow = CTSMagicView::GetView();

 	if (pWindow != NULL)
	{
#if GUI_TS_ANALYZER_PACKETS
		pWindow->m_dlgTSAnalyzerPackets.UpdatePIDList();
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
		pWindow->m_dlgPacketTrigger.UpdatePIDList();
#endif

#if GUI_TS_ANALYZER_OVERVIEW
		pWindow->m_dlgTSAnalyzerOverview.UpdatePacketInfo();
#endif
	}

#endif
}

void CALLBACK_REPORT_PCR_Diagnosis(RECORD_PCR_t* pPcrRecord)
{
#if GUI_REPORT_PCR_DIAGNOSIS

	CTSMagicView* pWindow = CTSMagicView::GetView();

	if (pWindow != NULL)
	{
#if GUI_TS_ANALYZER_PCR
		pWindow->m_dlgTSAnalyzerPcr.UpdatePCRDiagnosis(pPcrRecord);
#endif
	}

#endif
}

//curInterval -- ns
//curJitter -- ns
int CALLBACK_REPORT_PCR_Observation(int ID, int curInterval, int curJitter)
{
#if GUI_REPORT_PCR_OBSERVATION

	CTSMagicView* pWindow = CTSMagicView::GetView();

	if (pWindow != NULL)
	{
#if GUI_TS_ANALYZER_PCR
		pWindow->m_dlgTSAnalyzerPcr.UpdatePCRObservation(ID, curInterval, curJitter);
#endif
	}

#endif

	return 0;
}

//void CALLBACK_REPORT_bitrates(BITRATE_ATTRIBUTE_t* pattr)
//{
//#if GUI_REPORT_TS_BITRATE
//
//	CTSMagicView* pWindow = CTSMagicView::GetView();
//
//	if (pWindow != NULL)
//	{
//#if GUI_TS_ANALYZER_OVERVIEW
//		pWindow->m_dlgTSAnalyzerOverview.UpdateBitrateSample(pattr);
//#endif
//	}
//
//#endif
//}

int CALLBACK_REPORT_bitrate(int ID, int bitrate)
{
#if GUI_REPORT_TS_BITRATE

	CTSMagicView* pWindow = CTSMagicView::GetView();

	if (pWindow != NULL)
	{
#if GUI_TS_ANALYZER_OVERVIEW
		pWindow->m_dlgTSAnalyzerOverview.UpdateBitrateSample(bitrate);
#endif
	}

#endif

	return 0;
}

void CALLBACK_REPORT_dsmcc(int section_number, CPVT* pPVT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if ((pTSMagicView != NULL) && (pPVT != NULL))
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateDSMCC(section_number, pPVT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateDSMCC(section_number, pPVT);
#endif
#if GUI_TS_ANALYZER_OCDC
		pTSMagicView->m_dlgTSAnalyzerDsmcc.UpdateDSMCC(pPVT);
#endif
	}
}

void CALLBACK_REPORT_mpe(int section_number, CMPE* pMPE)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateMPE(section_number, pMPE);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateMPE(section_number, pMPE);
#endif
	}
}

void CALLBACK_REPORT_pat(int section_number, CPAT* pPAT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	uint16_t usPID;
	char pszMsg[64];
	RECORD_TSPacket_t TSPacketInfo;
	int				  rtcode;

 	if (pTSMagicView != NULL)
	{
		CDB_TSPackets* pDB_TSPackets = pTSMagicView->GetTSPacketsDBase();
		
		PROGRAM_MAP_DESCRIPTION_t stProgramMap;
		int nProgramCount = pPAT->GetProgramCount();
		for (int i = 0; i < nProgramCount; i++)
		{
			rtcode = pPAT->GetProgramByIndex(i, &stProgramMap);
			if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
			{
				if (stProgramMap.program_number == 0x0000)
				{
					strcpy_s(pszMsg, sizeof(pszMsg), "NIT, refered in PAT\n");
				}
				else
				{
					strcpy_s(pszMsg, sizeof(pszMsg), "PMT, refered in PAT\n");
				}
				usPID = stProgramMap.program_map_PID;

				rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);

				if (rtcode != MIDDLEWARE_DB_NO_ERROR)		//PAT指示的TS PID在数据库中未找到
				{
					pDB_TSPackets->AddDummyRecord(usPID, TSPAYLOAD_CLASS_SECTION, 0x00, 0x00, pszMsg, sizeof(pszMsg));	//建立一个空记录，这个属于ETR 290的检错项
				}
			}
		}

#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdatePAT(section_number, pPAT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdatePAT(section_number, pPAT);
#endif
#if GUI_TS_ANALYZER_OVERVIEW
		pTSMagicView->m_dlgTSAnalyzerOverview.UpdatePAT(pPAT);
#endif
#if GUI_TS_ANALYZER_PESES
		pTSMagicView->m_dlgTSAnalyzerPesEs.UpdatePAT(pPAT);
#endif
#if GUI_TS_ANALYZER_ES
		pTSMagicView->m_dlgTSAnalyzerEs.SendMessage(WM_UPDATE_PAT, (WPARAM)pPAT, NULL);
#endif
	}
}

void CALLBACK_REPORT_pmt(int section_number, CPMT* pPMT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	uint16_t usPID;
	uint8_t stream_type;
	uint8_t stream_subtype;
	char pszMsg[128];
	char pszTemp[64];
	RECORD_TSPacket_t TSPacketInfo;
	int   			rtcode;

 	if (pTSMagicView != NULL)
	{
		CDB_TSPackets* pDB_TSPackets = pTSMagicView->GetTSPacketsDBase();

		uint16_t usProgramNumber = pPMT->GetProgramNumber();
		uint16_t usPcrPID = pPMT->GetPcrPID();

		sprintf_s(pszMsg, sizeof(pszMsg), "PCR, refered in PMT(program_number=0x%04X)\n", usProgramNumber);
		rtcode = pDB_TSPackets->GetRecord(usPcrPID, &TSPacketInfo);
		if (rtcode == MIDDLEWARE_DB_NO_ERROR)
		{
			pDB_TSPackets->UpdateDescription(usPcrPID, pszMsg, sizeof(pszMsg));	//更新描述
		}
		else
		{
			pDB_TSPackets->AddDummyRecord(usPcrPID, TSPAYLOAD_CLASS_SECTION, 0x00, 0x00, pszMsg, sizeof(pszMsg));	//建立一个空记录，这个属于ETR 290的检错项
		}

		ES_INFO_t stESInfo;
		int nEsCount = pPMT->GetESCount();
		for (int i = 0; i < nEsCount; i++)
		{
			rtcode = pPMT->GetESInfo(i, &stESInfo);
			if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
			{
				stream_type = stESInfo.stream_type;
				stream_subtype = stESInfo.stream_subtype;
				usPID = stESInfo.elementary_PID;

				rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);
				if (rtcode != MIDDLEWARE_DB_NO_ERROR)		//PMT指示的TS PID在数据库中未找到
				{
					MPEG2_PSI_NumericCoding2Text_StreamType(stream_type, stream_subtype, pszTemp, sizeof(pszTemp));
					sprintf_s(pszMsg, sizeof(pszMsg), "%s, refered in PMT(program_number=0x%04X)\n", pszTemp, usProgramNumber);
					pDB_TSPackets->AddDummyRecord(usPID, TSPAYLOAD_CLASS_PES_DATA, stream_type, stream_subtype, pszMsg, sizeof(pszMsg));	//建立一个空记录，这个属于ETR 290的检错项
				}
				else
				{
					pDB_TSPackets->UpdateStreamType(usPID, stream_type);
				}
			}
		}

#if GUI_TS_ANALYZER_OVERVIEW
		pTSMagicView->m_dlgTSAnalyzerOverview.UpdatePMT(pPMT);
#endif

#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdatePMT(section_number, pPMT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdatePMT(section_number, pPMT);
#endif

#if GUI_TS_ANALYZER_PESES
		pTSMagicView->m_dlgTSAnalyzerPesEs.UpdatePMT(pPMT);
#endif

#if GUI_TS_ANALYZER_ES
		pTSMagicView->m_dlgTSAnalyzerEs.SendMessage(WM_UPDATE_PMT, (WPARAM)pPMT, (LPARAM)program_index);
#endif

#if GUI_TS_ANALYZER_SUBTITLE
		pTSMagicView->m_dlgTSAnalyzerSubtitle.SendMessage(WM_UPDATE_PMT, (WPARAM)pPMT, (LPARAM)program_index);
#endif

#if GUI_TS_ANALYZER_TELETEXT
		pTSMagicView->m_dlgTSAnalyzerTeletext.SendMessage(WM_UPDATE_PMT, (WPARAM)pPMT, (LPARAM)program_index);
#endif

#if GUI_TS_ANALYZER_OCDC
		pTSMagicView->m_dlgTSAnalyzerDsmcc.UpdatePMT(pPMT);
#endif

	}
}

void CALLBACK_REPORT_cat(int section_number, CCAT* pCAT)
{
	char pszMsg[128];
	RECORD_TSPacket_t TSPacketInfo;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	if (pTSMagicView != NULL)
	{
		CDB_TSPackets* pDB_TSPackets = pTSMagicView->GetTSPacketsDBase();

		if (pCAT != NULL)
		{
			CA_SYSTEM_INFO_t stCASystemInfo;
			int nCASystemCount = pCAT->GetCASystemCount();
			for (int i = 0; i < nCASystemCount; i++)
			{
				int rtcode = pCAT->GetCASystemInfoByIndex(i, &stCASystemInfo);
				if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
				{
					strcpy_s(pszMsg, sizeof(pszMsg), "EMM, refered in CAT\n");
					uint16_t usPID = stCASystemInfo.EMM_PID;

					rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);

					if (rtcode != MIDDLEWARE_DB_NO_ERROR)		//PAT指示的TS PID在数据库中未找到
					{
						pDB_TSPackets->AddDummyRecord(usPID, TSPAYLOAD_CLASS_SECTION, 0x00, 0x00, pszMsg, sizeof(pszMsg));	//建立一个空记录，这个属于ETR 290的检错项
					}
				}
			}

#if GUI_TS_ANALYZER_PSISI
			pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateCAT(section_number, pCAT);
			//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateCAT(section_number, pCAT);
#endif
		}
	}
}

void CALLBACK_REPORT_tsdt(int section_number, CTSDT* pTSDT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateTSDT(section_number, pTSDT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateTSDT(section_number, pTSDT);
#endif
	}
}

void CALLBACK_REPORT_cmt(CCMT* pCMT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
		if (pCMT != NULL)
		{
			uint8_t table_id = pCMT->GetTableID();

#if GUI_TS_ANALYZER_PSISI
			pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateCMT(pCMT);
			//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateCMT(pCMT);

			//if ((table_id >= TABLE_ID_ECM_MIN) && (table_id <= TABLE_ID_ECM_MAX))
			//{
			//	pWindow->m_dlgTSAnalyzerPsiSi.UpdateECM(pCMT);
			//}
			//else if ((table_id >= TABLE_ID_EMM_MIN) && (table_id <= TABLE_ID_EMM_MAX))
			//{
			//	pWindow->m_dlgTSAnalyzerPsiSi.UpdateEMM(pCMT);
			//}
#endif
		}
	}
}

void CALLBACK_REPORT_nit(int section_number, CNIT* pNIT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_NETWORK
		pWindow->m_dlgTSAnalyzerServices.m_pPane->UpdateNIT(network_index, pNIT);
#endif

#if GUI_TS_ANALYZER_EPG
		pTSMagicView->m_dlgTSAnalyzerEpg.UpdateNIT(pNIT);
#endif

#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateNIT(section_number, pNIT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateNIT(section_number, pNIT);
#endif

#if GUI_TS_ANALYZER_OVERVIEW
		pTSMagicView->m_dlgTSAnalyzerOverview.UpdateNIT(pNIT);
#endif
	}
}

void CALLBACK_REPORT_int(int section_number, CINT* pINT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateINT(section_number, pINT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateINT(section_number, pINT);
#endif
	}
}

void CALLBACK_REPORT_ait(int section_number, CAIT* pAIT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateAIT(section_number, pAIT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateAIT(section_number, pAIT);
#endif
	}
}

void CALLBACK_REPORT_bat(int section_number, CBAT* pBAT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateBAT(section_number, pBAT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateBAT(section_number, pBAT);
#endif
#if GUI_TS_ANALYZER_BOUQUETS
		pTSMagicView->m_dlgTSAnalyzerBouquets.m_pNaviPane->UpdateBAT(pBAT);
#endif
	}
}

void CALLBACK_REPORT_sdt(int section_number, CSDT* pSDT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateSDT(section_number, pSDT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateSDT(section_number, pSDT);
#endif

#if GUI_TS_ANALYZER_NETWORK
		pTSMagicView->m_dlgTSAnalyzerServices.m_pPane->UpdateSDT(sdt_index, pSDT);
#endif

#if GUI_TS_ANALYZER_OVERVIEW
		pTSMagicView->m_dlgTSAnalyzerOverview.UpdateSDT(pSDT);
#endif

#if GUI_TS_ANALYZER_EPG
		pTSMagicView->m_dlgTSAnalyzerEpg.UpdateSDT(pSDT);
#endif

#if GUI_TS_ANALYZER_OCDC
		pTSMagicView->m_dlgTSAnalyzerDsmcc.UpdateSDT(pSDT);
#endif
	}
}

void CALLBACK_REPORT_tot(CTOT* pTOT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateTOT(pTOT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateTOT(pTOT);
#endif
	}
}

void CALLBACK_REPORT_tdt(CTDT* pTDT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateTDT(pTDT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateTDT(pTDT);
#endif
	}
}

void CALLBACK_REPORT_rst(CRST* pRST)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateRST(pRST);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateRST(pRST);
#endif
	}
}

void CALLBACK_REPORT_eit(int section_number, CEIT* pEIT)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
#if GUI_TS_ANALYZER_PSISI
		pTSMagicView->m_dlgTSAnalyzerPsiSi.UpdateEIT(section_number, pEIT);
		//pTSMagicView->m_tabItem_PsiSiAnalyzer.UpdateEIT(section_number, pEIT);
#endif
	}
}

void CALLBACK_GUI_RESET_nit(void)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

 	if (pTSMagicView != NULL)
	{
	}
}

