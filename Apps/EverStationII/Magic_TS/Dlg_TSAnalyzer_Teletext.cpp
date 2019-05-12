// Dlg_TS_WatchTeletext.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Teletext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"

//#include "..\Magic_TS\TSMagic_GuiApi_PsiSi.h"
#include "..\Magic_TS\TSMagic_GuiApi_MSG.h"
#include "..\resource.h"

#include "TSMagicView.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Teletext dialog
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"

CDlg_TSAnalyzer_Teletext::CDlg_TSAnalyzer_Teletext(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Teletext::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Teletext)
	//}}AFX_DATA_INIT

	fp_debug = NULL;
}

void CDlg_TSAnalyzer_Teletext::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Teletext)
	DDX_Control(pDX, IDC_LIST_TELETEXT_PID, m_listTeletextPID);
	//DDX_Control(pDX, IDC_TREE_TELETEXT_SYNTAX, m_treeTeletextSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Teletext, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Teletext)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TELETEXT_PID, OnDblclkListTeletextPid)
	ON_BN_CLICKED(IDC_BTN_TELETEXT_TRIGGER, OnBtnTeletextTrigger)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_UPDATE_PAT, OnUpdatePAT)
	ON_MESSAGE(WM_UPDATE_PMT, OnUpdatePMT)

	//ON_MESSAGE(WM_UPDATE_PES_HEADER, OnUpdatePesHeader)

	ON_NOTIFY(NM_CLICK, IDC_LIST_TELETEXT_PID, &CDlg_TSAnalyzer_Teletext::OnNMClickListTeletextPid)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Teletext message handlers

BOOL CDlg_TSAnalyzer_Teletext::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	InitList();
	InitTree();

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Teletext::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	CRect	rect;

	pWnd = GetDlgItem(IDC_TREE_TELETEXT_SYNTAX);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 10, cy - rect.top - 10, 0);
	}

	if (m_listTeletextPID.GetSafeHwnd() != NULL)
	{
		CRect rectList;
		m_listTeletextPID.GetWindowRect(&rectList);
		int listwidth = rectList.Width();

		CHeaderCtrl* pHeaderCtrl = m_listTeletextPID.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			listwidth -= m_listTeletextPID.GetColumnWidth(i);
			listwidth -= 3;
		}
		m_listTeletextPID.SetColumnWidth(columns - 1, listwidth);
	}
}

void CDlg_TSAnalyzer_Teletext::Reset(void)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_BTN_TELETEXT_TRIGGER);
	pWnd->EnableWindow(FALSE);

	m_listTeletextPID.DeleteAllItems();

	//TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hTeletextPesItem);
	//TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hTeletextEsItem);

	if (fp_debug != NULL)
	{
		fclose(fp_debug);
		fp_debug = NULL;
	}
}

void CDlg_TSAnalyzer_Teletext::Set(void)
{
//	CWnd* pWnd = GetDlgItem(IDC_BTN_WATCH);
//	pWnd->EnableWindow(TRUE);

//	pWnd = GetDlgItem(IDC_BTN_PTS);
//	pWnd->EnableWindow(TRUE);

//	pWnd = GetDlgItem(IDC_BTN_DTS);
//	pWnd->EnableWindow(TRUE);

//	pWnd = GetDlgItem(IDC_BTN_TRIGGER);
//	pWnd->EnableWindow(TRUE);

	if (fp_debug == NULL)
	{
//		fp_debug = fopen("e:\\dts_pts.txt", "wt");
	}
}

void CDlg_TSAnalyzer_Teletext::InitList(void)
{
	int	nColWidth[] = {22, 120, 180};
	CString str[] = {"", "PID", "所属业务"};
	UINT	uiAlign[] = {LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT};

	m_ESImageList.Create(IDB_ES, 20, 1, RGB(0xab,0xcd,0xef));
	m_listTeletextPID.SetImageList(&m_ESImageList, LVSIL_SMALL);

	m_listTeletextPID.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (int i = 0; i < sizeof(nColWidth) / sizeof(int); i++)
	{
		m_listTeletextPID.InsertColumn(i, str[i], uiAlign[i], nColWidth[i], -1);
	}
}

void CDlg_TSAnalyzer_Teletext::OnBtnTeletextTrigger() 
{
	// TODO: Add your control notification handler code here
/*
	char		pszText[MAX_TXT_CHARS];

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	if (pWindow->m_kThreadParams.main_thread_running)
	{
		TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hTeletextPesItem);
		TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hTeletextEsItem);

		if (pTSMagicView->m_kThreadParams.offline == 1)
		{
			//离线分析
			if (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)		//分析已经结束
			{
				if (pTSMagicView->m_kThreadParams.triggering == 0)
				{
					if (pTSMagicView->m_kThreadParams.downloading == 0)
					{
						pTSMagicView->m_kThreadParams.triggering = 1;
						::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_es_trigger_thread, (LPVOID)pTSMagicView, 0, 0);
					}
				}
				else
				{
					sprintf_s(pszText, sizeof(pszText), "上一次捕捉请求还未结束...");
					::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
			}
		}
		else
		{
		}
	}
*/
}

/*
void CDlg_TSAnalyzer_Teletext::SetPIDInfo(CPMT* pPMT)
{
	char			pszText[MAX_TXT_CHARS];
	int				j;
	uint32_t				usPCR_PID;
	int				nItem;
	uint32_t				code;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	m_listTeletextPID.DeleteAllItems();

//	if (m_hVidPesItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hVidPesItem);
//	}
//	if (m_hVidEsItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hVidEsItem);
//	}
//	if (m_hAudPesItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hAudPesItem);
//	}
//	if (m_hAudEsItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeTeletextSyntax, m_hAudEsItem);
//	}

	m_listTeletextPID.InsertItem(0, "", 5);

#if CHINESE_VERSION
//	sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_usPCR_PID, pPMT->m_usPCR_PID);
#else
//	sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_usPCR_PID, pPMT->m_usPCR_PID);
#endif
//	m_listTeletextPID.SetItemData(0, pPMT->m_usPCR_PID);
//	m_listTeletextPID.SetItemText(0, 1, pszText);
//	m_listTeletextPID.SetItemText(0, 2, "PCR_PID");

//	SetESInfo();
}
*/
void CDlg_TSAnalyzer_Teletext::OnDblclkListTeletextPid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	CEverStation_TSMagicDlg* pFrame = (CEverStation_TSMagicDlg*)AfxGetMainWnd();
	CString strItem;
	CWnd*	pWnd;
	DWORD 	dwPID;

	int nSel = m_listTeletextPID.GetSelectionMark();
	if (nSel >= 0)
	{
		dwPID = (DWORD)m_listTeletextPID.GetItemData(nSel);

		strItem = m_listTeletextPID.GetItemText(nSel, 1);
		pWnd = GetDlgItem(IDC_EDIT_TELETEXT_CUR_PID);
		pWnd->SetWindowText(strItem);
	}

	*pResult = 0;
}

//
LRESULT CDlg_TSAnalyzer_Teletext::OnUpdatePMT(WPARAM wParam, LPARAM lParam)
{
/*
	CPMT*			pPMT = (CPMT*)wParam;
	int				program_index = (int)lParam;
	char			pszText[MAX_TXT_CHARS];
	char			pszTemp[MAX_TXT_CHARS];
	int				es_index;
	CWnd*			pWnd = NULL;
	int				nPIDStyle;
	int				nItemCount;
	int				item;
	int				bExit;
	DWORD			dwPID;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

//	uint8_t	ucSubType = 0x00;

	if (pPMT != NULL)
	{
		for (es_index = 0; es_index < pPMT->m_nEsCount; es_index++)
		{
			nPIDStyle = StreamTypeLookup(pPMT->m_astEsInfo[es_index].stream_type, pPMT->m_astEsInfo[es_index].stream_subtype, NULL);
			if (nPIDStyle == PIDSTYLE_TELETEXT)
			{
				pWnd = GetDlgItem(IDC_BTN_TELETEXT_TRIGGER);
				pWnd->EnableWindow(TRUE);

				bExit = 0;
				nItemCount = m_listTeletextPID.GetItemCount();
				for (item = 0; item < nItemCount; item ++)
				{
					dwPID = m_listTeletextPID.GetItemData(item);
					if (dwPID == pPMT->m_astEsInfo[es_index].elementary_PID)
					{
						bExit = 1;
						break;
					}
				}

				if (bExit == 0)
				{
					m_listTeletextPID.InsertItem(nItemCount, "", 5);

					sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID);

					m_listTeletextPID.SetItemData(nItemCount, pPMT->m_astEsInfo[es_index].elementary_PID);
					m_listTeletextPID.SetItemText(nItemCount, 1, pszText);

					pWnd = GetDlgItem(IDC_EDIT_TELETEXT_CUR_PID);
					pWnd->GetWindowTextA(pszTemp, MAX_TXT_CHARS);
					if (strcmp(pszTemp, "") == 0)
					{
						pWnd->SetWindowText(pszText);
						m_listTeletextPID.SetItemState(nItemCount, LVIS_SELECTED, LVIS_SELECTED);
					}

					sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
					m_listTeletextPID.SetItemData(nItemCount, pPMT->m_astEsInfo[es_index].elementary_PID);
					m_listTeletextPID.SetItemText(nItemCount, 2, pszText);
				}
				else
				{
					m_listTeletextPID.GetItemText(item, 2, pszTemp, MAX_TXT_CHARS);

					sprintf_s(pszText, sizeof(pszText), "%s, 0x%04X(%d)", pszTemp, pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
					m_listTeletextPID.SetItemText(item, 2, pszText);
				}
			}
		}
	}
*/

	return 0;
}

void CDlg_TSAnalyzer_Teletext::InitTree(void)
{
	//TREEITEM hItem;

	//TreeFun_InitTree(&m_treeTeletextSyntax, this, IDC_TREE_TELETEXT_SYNTAX);

	//hItem = TreeFun_InsertItem(&m_treeTeletextSyntax, TREE_ROOT, "teletext_stream()", -1, 0, NULL);
	//m_hTeletextPesItem = TreeFun_InsertItem(&m_treeTeletextSyntax, hItem, "PES_packet()", -1, 0, NULL);
	//m_hTeletextEsItem = TreeFun_InsertItem(&m_treeTeletextSyntax, hItem, "ES()", -1, 0, NULL);

	//m_treeTeletextSyntax.Expand(hItem, TREE_EXPAND);
}

LRESULT CDlg_TSAnalyzer_Teletext::OnUpdatePesHeader(WPARAM wParam, LPARAM lParam)
{
/*	
	PES_packet_t*				    pPES_packet = (PES_packet_t*)wParam;
	unsigned char					stream_id = (unsigned char)lParam;
	TREEITEM						hParentItem = NULL;

	if (stream_id == 0xBD)		//AC3 stream
	{
		hParentItem = m_hAudPesItem;
	}
	else if ((stream_id >= 0xC0) && (stream_id <= 0xDF))
	{
		hParentItem = m_hAudPesItem;
	}
	else if ((stream_id >= 0xE0) && (stream_id <= 0xEF))
	{
		hParentItem = m_hVidPesItem;
	}

	if (hParentItem != NULL)
	{
		if (!TreeFun_HasChild(&m_treeTeletextSyntax, hParentItem))
		{
			PES_HEADER(&m_treeTeletextSyntax, hParentItem, pPES_packet);
//			m_treeTeletextSyntax.Expand(hParentItem, TVE_EXPAND);
		}

		UpdateDtsPts(pPES_packet, 0);
	}
*/
	
	return 0;
}

void CDlg_TSAnalyzer_Teletext::OnNMClickListTeletextPid(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CDlg_TSAnalyzer_Teletext::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
