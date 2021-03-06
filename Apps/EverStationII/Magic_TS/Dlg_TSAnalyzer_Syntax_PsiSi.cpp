// Dlg_PsiSiSyntax.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation.h"
#include "Dlg_TSAnalyzer_Syntax_PsiSi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Syntax_PsiSi dialog
#include "../Common/Define.h"
#include "../Magic_TS/TSMagicView.h"
#include "../Magic_TS/TSMagic_AnalyseThread_MSG.h"

#include "syntax_translate_layer\MPEG2_DVB_Section\Include\Mpeg2_table_id.h"
#include "syntax_translate_layer\MPEG2_DVB_Section\Include\DVB_table_id.h"

#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\Mpeg2_DSMCC_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\Mpeg2_PSI_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\DVB_SI_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\DVB_IPDC_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\DVB_MHP_section_XML.h"

#include "Utilities\Directory\Include\TOOL_Directory.h"


CDlg_TSAnalyzer_Syntax_PsiSi::CDlg_TSAnalyzer_Syntax_PsiSi(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Syntax_PsiSi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Syntax_PsiSi)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pNaviTree = NULL;
	m_pSyntaxTree = NULL;
	m_pHexList = NULL;
}


void CDlg_TSAnalyzer_Syntax_PsiSi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Syntax_PsiSi)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Syntax_PsiSi, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Syntax_PsiSi)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_PSISI_SEL_CHANGE, OnReportPsiSiSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Syntax_PsiSi message handlers

void CDlg_TSAnalyzer_Syntax_PsiSi::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);

		m_wndSplitter.SetColumnInfo(0, 500, 0);

		int n2ColWidth = cx - 500 - 10;

		int nHexViewWidth = 740;
		if (n2ColWidth < 1480)
		{
			nHexViewWidth = n2ColWidth / 2;
		}

		m_wndSplitter.SetColumnInfo(1, n2ColWidth - nHexViewWidth, 0);
		m_wndSplitter.SetColumnInfo(2, nHexViewWidth, 0);

		m_wndSplitter.RecalcLayout();
	}
}

BOOL CDlg_TSAnalyzer_Syntax_PsiSi::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 3);
		m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CNaviTree_PsiSiSectionGuide), CSize(100,0), NULL);
		m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CTreeView_XMLBrowser), CSize(100,0), NULL);
		m_wndSplitter.CreateView(0,2,RUNTIME_CLASS(CHexEditView_ByteBuffer), CSize(0, 0), NULL);

		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pNaviTree = (CNaviTree_PsiSiSectionGuide*)m_wndSplitter.GetPane(0, 0);
		m_pNaviTree->Set(this->GetSafeHwnd());

		m_pSyntaxTree = (CTreeView_XMLBrowser*)m_wndSplitter.GetPane(0, 1);
		m_pSyntaxTree->Init("PSI/SI section 语法分析");
		m_pSyntaxTree->m_hNotifyParent = GetSafeHwnd();
		m_pHexList = (CHexEditView_ByteBuffer*)m_wndSplitter.GetPane(0, 2);
		m_pHexList->SetBPR(16);
	}

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Syntax_PsiSi::Reset(void)
{
	m_pNaviTree->Reset();
	m_pSyntaxTree->Reset();
	m_pHexList->Reset();
}

//void CDlg_TSAnalyzer_PsiSi::Set(int offline)
//{
//	m_pNaviTree->Set(offline);
//	m_pSyntaxTree->Set(offline);
//	m_pHexList->Set(offline);
//}

void CDlg_TSAnalyzer_Syntax_PsiSi::DisplaySection(uint8_t* section_buf, int section_length)
{
	if (m_pHexList != NULL)
	{
		m_pHexList->Reset();
		m_pHexList->DisplayByteBuffer(section_buf, section_length);
	}

	if (m_pSyntaxTree != NULL)
	{
#ifdef _DEBUG
		char	pszExeFile[MAX_PATH];
		char	exeDrive[3];
		char	pszXmlDir[MAX_PATH];
		char	pszFilePath[MAX_PATH];
		GetModuleFileName(NULL, pszExeFile, MAX_PATH);
		exeDrive[0] = pszExeFile[0];
		exeDrive[1] = pszExeFile[1];
		exeDrive[2] = '\0';

		sprintf_s(pszXmlDir, sizeof(pszXmlDir), "%s\\~EverStationII\\xml", exeDrive);
		DIR_BuildDirectory(pszXmlDir);
#endif
		uint8_t	table_id;
		if ((section_buf != NULL) && (section_length >= 3))
		{
			TALForXMLDoc xml2Doc;

			table_id = section_buf[0];

			if (table_id == TABLE_ID_PAT)
			{
				MPEG2_PSI_PAT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\PAT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_PMT)
			{
				MPEG2_PSI_PMT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\PMT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_CAT)
			{
				MPEG2_PSI_CAT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\CAT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_NIT_ACTUAL || table_id == TABLE_ID_NIT_OTHER)
			{
				DVB_SI_NIT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\NIT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_SDT_ACTUAL || table_id == TABLE_ID_SDT_OTHER)
			{
				DVB_SI_SDT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\SDT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_BAT)
			{
				DVB_SI_BAT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\BAT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if ((table_id >= TABLE_ID_EIT_PF_ACTUAL) && (table_id <= TABLE_ID_EIT_SCH_OTHER_F))
			{
				DVB_SI_EIT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\EIT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_TSDT)
			{
				MPEG2_PSI_TSDT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TSDT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_TDT)
			{
				DVB_SI_TDT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TDT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_TOT)
			{
				DVB_SI_TOT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TOT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_RST)
			{
				DVB_SI_RST_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\RST-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if ((table_id >= TABLE_ID_ECM_MIN) && (table_id <= TABLE_ID_EMM_MAX))
			{
				DVB_SI_CMT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\CMT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_INT)
			{
				DVB_IPDC_INT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\INT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_AIT)
			{
				DVB_MHP_AIT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\AIT-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_DSMCC_MPE)
			{
				DVB_IPDC_MPE_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\MPE-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_DSMCC_UNM)
			{
				MPEG2_DSMCC_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_UNM-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if (table_id == TABLE_ID_DSMCC_DDM)
			{
				MPEG2_DSMCC_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_DDM-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
			else if ((table_id >= TABLE_ID_DSMCC_SD) && (table_id <= TABLE_ID_DSMCC_RSV))
			{
				//MPEG2_DSMCC_DecodeSection_to_XML(section_buf, section_length, &xmlDoc);
				assert(0);

#ifdef _DEBUG
				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_RSV-0x%02X.xml", pszXmlDir, table_id);
#endif
			}
#ifdef _DEBUG
			xml2Doc.SaveFile(pszFilePath);
#endif
			m_pSyntaxTree->Reset();
			m_pSyntaxTree->ShowXMLDoc(&xml2Doc);
		}
	}
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdatePAT(int section_number, CPAT* pPAT)
{
	m_pNaviTree->UpdatePAT(section_number, pPAT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdatePMT(int section_number, CPMT* pPMT)
{
	m_pNaviTree->UpdatePMT(section_number, pPMT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateBAT(int section_number, CBAT* pBAT)
{
	m_pNaviTree->UpdateBAT(section_number, pBAT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateSDT(int section_number, CSDT* pSDT)
{
	m_pNaviTree->UpdateSDT(section_number, pSDT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateNIT(int section_number, CNIT* pNIT)
{
	m_pNaviTree->UpdateNIT(section_number, pNIT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateAIT(int section_number, CAIT* pAIT)
{
	m_pNaviTree->UpdateAIT(section_number, pAIT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateINT(int section_number, CINT* pINT)
{
	m_pNaviTree->UpdateINT(section_number, pINT);

}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateEIT(int section_number, CEIT* pEIT)
{
	m_pNaviTree->UpdateEIT(section_number, pEIT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateDSMCC(int section_number, CPVT* pPVT)
{
	m_pNaviTree->UpdateDSMCC(section_number, pPVT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateMPE(int section_number, CMPE* pMPE)
{
	m_pNaviTree->UpdateMPE(section_number, pMPE);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateCAT(int section_number, CCAT* pCAT)
{
	m_pNaviTree->UpdateCAT(section_number, pCAT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateTSDT(int section_number, CTSDT* pTSDT)
{
	m_pNaviTree->UpdateTSDT(section_number, pTSDT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateTDT(CTDT* pTDT)
{
	m_pNaviTree->UpdateTDT(pTDT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateTOT(CTOT* pTOT)
{
	m_pNaviTree->UpdateTOT(pTOT);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateRST(CRST* pRST)
{
	m_pNaviTree->UpdateRST(pRST);
}

void CDlg_TSAnalyzer_Syntax_PsiSi::UpdateCMT(CCMT* pCMT)
{
	m_pNaviTree->UpdateCMT(pCMT);
}

BOOL CDlg_TSAnalyzer_Syntax_PsiSi::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	NMHDR* nmhdr = (LPNMHDR)lParam;

	if (m_pSyntaxTree != NULL)
	{
		//语法树和16进制字节表显示联动
		if (nmhdr->hwndFrom == m_pSyntaxTree->GetSafeHwnd())
		{
			if (nmhdr->code == TVN_SELCHANGED)
			{
				LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(nmhdr);

				TVITEM item = pNMTreeView->itemNew;
				HTREEITEM hSelItem = item.hItem;

				if (hSelItem != NULL)
				{
					if (item.mask & TVIF_PARAM)
					{
						DWORD uiValue = (DWORD)(item.lParam);
						//char pszTemp[64];
						//sprintf_s(pszTemp, 64, "Parent: OnTvnSelchanged(), uiValue=0x%08X\0", uiValue);
						//AfxMessageBox(pszTemp);

						short offset = (short)((uiValue & 0xffff0000) >> 16);
						short length = (short)(uiValue & 0x0000ffff);

						if ((offset >= 0) && (length > 0))
						{
							m_pHexList->SetSel(offset, offset + length);
						}
						else
						{
							m_pHexList->SetSel(-1, -1);
						}
					}
				}

				return false;	////如果不返回false，会将TVN_SELECTED消息反射回m_pSyntaxTree控件，然后又弹射回来，如此反复，无限循环
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CDlg_TSAnalyzer_Syntax_PsiSi::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

LRESULT CDlg_TSAnalyzer_Syntax_PsiSi::OnReportPsiSiSelChange(WPARAM wParam, LPARAM lParam)
{
	m_pSyntaxTree->Reset();
	m_pHexList->Reset();

	uint16_t	  usKeyID;
	uint32_t	  dwSelectItemData = (uint32_t)lParam;
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	char		  pszText[MAX_PATH];

	if (pTSMagicView->m_kThreadParams.main_thread_running)
	{
		if ((pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1) ||
			(pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1) ||
			//(pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1) ||
			(pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1) ||
			(pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1) ||
			(pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1))
		{
			if (pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现TS捕捉线程尚未结束！");
				::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现PES捕捉线程尚未结束！");
				::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			//if (pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1)
			//{
			//	sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现ES捕捉线程尚未结束！");
			//	::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//}
			if (pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现section捕捉线程尚未结束！");
				::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现DSMCC下载线程尚未结束！");
				::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现TS录制线程尚未结束！");
				::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
		}
		else
		{
			CTrigger_PsiSiSection* pSectionTrigger = pTSMagicView->GetSectionTrigger();
			CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();
			CPVT* pPVT = NULL;

			usKeyID = (dwSelectItemData & 0xffff0000) >> 16;
			pPVT = pDB_PsiSiObjs->QueryByKey(usKeyID);
			if (pPVT != NULL)
			{
				if (pPVT->IsNormalSectionSyntax() == 1)
				{
					int section_number = (dwSelectItemData & 0x000000ff);
					pSectionTrigger->SetMatchParamsForNormalSection(pPVT->GetPID(), pPVT->GetTableID(), pPVT->GetTableIDExtension(), section_number);
				}
				else
				{
					pSectionTrigger->SetMatchParamsForOtherSection(pPVT->GetPID(), pPVT->GetTableID());
				}
			}

			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_SECTION_TRIGGER_STATE, 1, 0);

			if (pTSMagicView->m_kThreadParams.offline == 1)
			{
				if (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)
				{
					::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_section_trigger_thread, (LPVOID) & (pTSMagicView->m_kThreadParams), 0, 0);
				}
			}
		}
	}

	return 0;
}
