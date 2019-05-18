#include <stdafx.h>
#include <assert.h>
#include <stdint.h>

#include "../Common/define.h"
//#include "TSMagic_GuiApi.h"
#include "TSMagic_AnalyseThread_MSG.h"
#include "TSMagic_ErrorCode.h"

#include "TSMagic_Callbacks_From_Lib.h"
#include "TSMagic_Callbacks_To_Gui.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_Common.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_IPDC_Section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_MHP_Section.h"

#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_ErrorCode.h"

//#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare/MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"

int REPORT_PSISI_section(CDB_PsiSiObjs* pDB_PsiSiObjs, HWND hWnd, uint16_t PID, uint8_t* buf, int length)
{
	int			rtcode = TSMAGIC_UNKNOWN_ERROR;
	CPVT*		pPVT = NULL;

	char		pszText[MAX_PATH];
	char		pszTemp[MAX_PATH];

	if ((pDB_PsiSiObjs != NULL) && (buf != NULL) && (length >= 3))
	{
		private_section_t private_section; 
		rtcode = MPEG_DVB_PVT_DecodeSection(buf, length, &private_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			pPVT = pDB_PsiSiObjs->QueryBy3ID(PID, private_section.table_id, private_section.table_id_extension);
			if (pPVT == NULL)
			{
				pPVT = pDB_PsiSiObjs->Append(PID, private_section.table_id, private_section.table_id_extension);
			}

			if (pPVT != NULL)
			{
				if (private_section.table_id == TABLE_ID_PAT)
				{
#if DEBUG_PAT
					CPAT* pPAT = (CPAT*)pPVT;
					rtcode = pPAT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_pat(private_section.section_number, pPAT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_CAT)
				{
#if DEBUG_CAT
					CCAT* pCAT = (CCAT*)pPVT;
					rtcode = pCAT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_cat(private_section.section_number, pCAT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_PMT)
				{
#if DEBUG_PMT
					CPMT* pPMT = (CPMT*)pPVT;
					rtcode = pPMT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_pmt(private_section.section_number, pPMT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_TSDT)
				{
#if DEBUG_TSDT
					CTSDT* pTSDT = (CTSDT*)pPVT;
					rtcode = pTSDT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_tsdt(private_section.section_number, pTSDT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_DSMCC_MPE)
				{
#if DEBUG_MPE
					CMPE* pMPE = (CMPE*)pPVT;
					rtcode = pMPE->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_mpe(private_section.section_number, pMPE);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_DSMCC_UNM)
				{
#if DEBUG_DSMCC
					CDSMCC_UNM* pDSMCC = (CDSMCC_UNM*)pPVT;
					rtcode = pDSMCC->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_dsmcc(private_section.section_number, pDSMCC);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_DSMCC_DDM)
				{
#if DEBUG_DSMCC
					CDSMCC_DDM* pDSMCC = (CDSMCC_DDM*)pPVT;
					rtcode = pDSMCC->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_dsmcc(private_section.section_number, pDSMCC);
					}
#endif
				}
//				else if ((private_section.table_id == TABLE_ID_DSMCC_SD) || (private_section.table_id == TABLE_ID_DSMCC_RSV))
//				{
//#if DEBUG_DSMCC
//					CDSMCC_PVT* pDSMCC = (CDSMCC_PVT*)pPVT;
//					rtcode = pDSMCC->AddSection(PID, buf, length, &private_section);
//					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
//					{
//						CALLBACK_REPORT_dsmcc(private_section.section_number, pDSMCC);
//					}
//#endif
//				}
				else if ((private_section.table_id == TABLE_ID_NIT_ACTUAL) || (private_section.table_id == TABLE_ID_NIT_OTHER))
				{
#if DEBUG_NIT
					CNIT* pNIT = (CNIT*)pPVT;
					rtcode = pNIT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_nit(private_section.section_number, pNIT);
					}
#endif
				}
				else if ((private_section.table_id == TABLE_ID_SDT_ACTUAL) || (private_section.table_id == TABLE_ID_SDT_OTHER))
				{
#if DEBUG_SDT
					CSDT* pSDT = (CSDT*)pPVT;
					rtcode = pSDT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_sdt(private_section.section_number, pSDT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_BAT)
				{
#if DEBUG_BAT
					CBAT* pBAT = (CBAT*)pPVT;
					rtcode = pBAT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_bat(private_section.section_number, pBAT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_INT)
				{
#if DEBUG_INT
					CINT* pINT = (CINT*)pPVT;
					rtcode = pINT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_int(private_section.section_number, pINT);
					}
#endif
				}
				else if ((private_section.table_id >= TABLE_ID_EIT_PF_ACTUAL) && (private_section.table_id <= TABLE_ID_EIT_SCH_OTHER_F))
				{
#if DEBUG_EIT
					CEIT* pEIT = (CEIT*)pPVT;
					rtcode = pEIT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_eit(private_section.section_number, pEIT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_TDT)
				{
#if DEBUG_TDT
					CTDT* pTDT = (CTDT*)pPVT;
					rtcode = pTDT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_tdt(pTDT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_RST)
				{
#if DEBUG_RST
					CRST* pRST = (CRST*)pPVT;
					rtcode = pRST->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_rst(pRST);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_TOT)
				{
#if DEBUG_TOT
					CTOT* pTOT = (CTOT*)pPVT;
					rtcode = pTOT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_tot(pTOT);
					}
#endif
				}
				else if (private_section.table_id == TABLE_ID_AIT)
				{
#if DEBUG_AIT
					CAIT* pAIT = (CAIT*)pPVT;
					rtcode = pAIT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_ait(private_section.section_number, pAIT);
					}
#endif
				}
				else if ((private_section.table_id >= TABLE_ID_ECM_MIN) && (private_section.table_id <= TABLE_ID_ECM_MAX))
				{
#if DEBUG_ECM
					CCMT* pCMT = (CCMT*)pPVT;
					rtcode = pCMT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_cmt(pCMT);
					}
#endif
				}
				else if ((private_section.table_id >= TABLE_ID_EMM_MIN) && (private_section.table_id <= TABLE_ID_EMM_MAX))
				{
#if DEBUG_EMM
					CCMT* pCMT = (CCMT*)pPVT;
					rtcode = pCMT->AddSection(PID, buf, length, &private_section);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						CALLBACK_REPORT_cmt(pCMT);
					}
#endif
				}
				else
				{
					rtcode = pPVT->AddSection(PID, &private_section);

					//assert(0);
				}

				uint16_t usPID = pPVT->GetPID();
				uint8_t ucTableID = pPVT->GetTableID();
				uint16_t usTableIDExtension = pPVT->GetTableIDExtension();
				uint8_t ucVersionNumber = pPVT->GetVersionNumber();

				switch (rtcode)		//if it is a new dsmcc_private section
				{
				case MIDDLEWARE_PSISI_VERSION_CHANGE:

					sprintf_s(pszText, sizeof(pszText), "PSISI解析：PID=0x%04X table_id=0x%02x table_id_extension=0x%04X的版本变更(%d->%d)", usPID, ucTableID, usTableIDExtension, ucVersionNumber, private_section.version_number);
					::SendMessage(hWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_WARNING);

					//assert(0);
					//pDB_PsiSiTables->ResetDsmccDDMs();

					//CALLBACK_REPORT_dsmcc(-1, -1, NULL);

					break;

				case MIDDLEWARE_PSISI_NO_ERROR:

					//CALLBACK_REPORT_dsmcc(pvt_index, pprivate_section->section_number, pPVT);

					if (pPVT->IsCollectOver())
					{
						sprintf_s(pszText, sizeof(pszText), "PSISI解析：PID=0x%04X table_id=0x%02x table_id_extension=0x%04X version=%d 的section收集完整", usPID, ucTableID, usTableIDExtension, ucVersionNumber);
						::SendMessage(hWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_INFO);
					}
					break;

				case MIDDLEWARE_PSISI_DUPLICATED_SECTION:

					//正常现象
					rtcode = TSMAGIC_NO_ERROR;
					break;

				default:

					TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));

					sprintf_s(pszText, sizeof(pszText), "PSISI解析：PID=0x%04X table_id=0x%02x table_id_extension=0x%04X %s", usPID, ucTableID, usTableIDExtension, pszTemp);
					::SendMessage(hWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);

					break;
				}
			}
		}
	}
	else
	{
		sprintf_s(pszText, sizeof(pszText), "PSISI解析：输入参数错误");
		::SendMessage(hWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);

		rtcode = TSMAGIC_PARAMETER_ERROR;
	}

	return rtcode;
}

