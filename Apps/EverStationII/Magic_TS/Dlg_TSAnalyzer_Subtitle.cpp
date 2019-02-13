// Dlg_TS_WatchTeletext.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Subtitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "..\Magic_TS\TSMagic_GuiApi_PsiSi.h"
#include "..\Magic_TS\TSMagic_GuiApi_MSG.h"
#include "..\resource.h"

#include "TSMagicView.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"
//#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Subtitle dialog

CDlg_TSAnalyzer_Subtitle::CDlg_TSAnalyzer_Subtitle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Subtitle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Subtitle)
	//}}AFX_DATA_INIT

	fp_debug = NULL;
}

void CDlg_TSAnalyzer_Subtitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Subtitle)
	DDX_Control(pDX, IDC_LIST_SUBTITLE_PID, m_listSubtitlePID);
	//DDX_Control(pDX, IDC_TREE_SUBTITLE_SYNTAX, m_treeSubtitleSyntax);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Subtitle, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Subtitle)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SUBTITLE_PID, OnDblclkListSubtitlePid)
	ON_BN_CLICKED(IDC_BTN_SUBTITLE_TRIGGER, OnBtnSubtitleTrigger)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_UPDATE_PAT, OnUpdatePAT)
	ON_MESSAGE(WM_UPDATE_PMT, OnUpdatePMT)

	//ON_MESSAGE(WM_UPDATE_PES_HEADER, OnUpdatePesHeader)

	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Subtitle message handlers

BOOL CDlg_TSAnalyzer_Subtitle::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	InitList();
	InitTree();

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Subtitle::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	CRect	rect;

	pWnd = GetDlgItem(IDC_TREE_SUBTITLE_SYNTAX);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 10, cy - rect.top - 10, 0);
	}

	if (m_listSubtitlePID.GetSafeHwnd() != NULL)
	{
		CRect rectList;
		m_listSubtitlePID.GetWindowRect(&rectList);
		int listwidth = rectList.Width();

		CHeaderCtrl* pHeaderCtrl = m_listSubtitlePID.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			listwidth -= m_listSubtitlePID.GetColumnWidth(i);
			listwidth -= 3;
		}
		m_listSubtitlePID.SetColumnWidth(columns - 1, listwidth);
	}
}

void CDlg_TSAnalyzer_Subtitle::Reset(void)
{
	CTSMagicView* pView = CTSMagicView::GetView();

	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_BTN_SUBTITLE_TRIGGER);
	pWnd->EnableWindow(FALSE);

	m_listSubtitlePID.DeleteAllItems();

//	TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hVidPesItem);
//	TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hVidEsItem);
//	TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hAudPesItem);
//	TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hAudEsItem);

	if (fp_debug != NULL)
	{
		fclose(fp_debug);
		fp_debug = NULL;
	}
}

//void CDlg_TSAnalyzer_Subtitle::Set(void)
//{
////	pWnd = GetDlgItem(IDC_BTN_SUBTITLE_TRIGGER);
////	pWnd->EnableWindow(TRUE);
//
//	if (fp_debug == NULL)
//	{
////		fp_debug = fopen("e:\\dts_pts.txt", "wt");
//	}
//}

void CDlg_TSAnalyzer_Subtitle::InitList(void)
{
	int	nColWidth[] = {22, 120, 180};
	CString str[] = {"", "PID", "所属业务"};
	UINT	uiAlign[] = {LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT};

	m_ESImageList.Create(IDB_ES, 20, 1, RGB(0xab,0xcd,0xef));
	m_listSubtitlePID.SetImageList(&m_ESImageList, LVSIL_SMALL);

	m_listSubtitlePID.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (int i = 0; i < sizeof(nColWidth) / sizeof(int); i++)
	{
		m_listSubtitlePID.InsertColumn(i, str[i], uiAlign[i], nColWidth[i], -1);
	}
}

void CDlg_TSAnalyzer_Subtitle::OnBtnSubtitleTrigger() 
{
	// TODO: Add your control notification handler code here
//	char		pszText[MAX_TXT_CHARS];

	CTSMagicView* pWindow = CTSMagicView::GetView();

	if (pWindow->m_kThreadParams.main_thread_running)
	{
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hVidPesItem);
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hVidEsItem);
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hAudPesItem);
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hAudEsItem);
/*
		if (pWindow->m_kThreadParams.offline == 1)
		{
			//离线分析
			if (pWindow->m_kThreadParams.finished == 1)
			{
				//分析已经结束
				if (pWindow->m_kThreadParams.triggering == 0)
				{
					if (pWindow->m_kThreadParams.downloading == 0)
					{
						pWindow->m_kThreadParams.triggering = 1;
						::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_es_trigger_thread, (LPVOID)pWindow, 0, 0);
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
*/		
	}
}

void CDlg_TSAnalyzer_Subtitle::SetPIDInfo(CPMT* pPMT)
{
/*
	char			pszText[MAX_TXT_CHARS];
	int				j;
	uint32_t				usPCR_PID;
	int				nItem;
	uint32_t				code;

	CTSMagicView* pView = CTSMagicView::GetView();

	m_listSubtitlePID.DeleteAllItems();

//	if (m_hVidPesItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hVidPesItem);
//	}
//	if (m_hVidEsItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hVidEsItem);
//	}
//	if (m_hAudPesItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hAudPesItem);
//	}
//	if (m_hAudEsItem != NULL)
//	{
//		TreeFun_DeleteChildItems(&m_treeSubtitleSyntax, m_hAudEsItem);
//	}

	m_listSubtitlePID.InsertItem(0, "", 5);

#if CHINESE_VERSION
//	sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_usPCR_PID, pPMT->m_usPCR_PID);
#else
//	sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_usPCR_PID, pPMT->m_usPCR_PID);
#endif
//	m_listSubtitlePID.SetItemData(0, pPMT->m_usPCR_PID);
//	m_listSubtitlePID.SetItemText(0, 1, pszText);
//	m_listSubtitlePID.SetItemText(0, 2, "PCR_PID");

//	SetESInfo();
*/
}

void CDlg_TSAnalyzer_Subtitle::OnDblclkListSubtitlePid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	CEverStation_TSMagicDlg* pFrame = (CEverStation_TSMagicDlg*)AfxGetMainWnd();

	CString strItem;
	CWnd*	pWnd;
	DWORD 	dwPID;

	int nSel = m_listSubtitlePID.GetSelectionMark();
	if (nSel >= 0)
	{
		dwPID = (DWORD)m_listSubtitlePID.GetItemData(nSel);

		strItem = m_listSubtitlePID.GetItemText(nSel, 1);
		pWnd = GetDlgItem(IDC_EDIT_SUBTITLE_CUR_PID);
		pWnd->SetWindowText(strItem);
	}

	*pResult = 0;
}

//
LRESULT CDlg_TSAnalyzer_Subtitle::OnUpdatePMT(WPARAM wParam, LPARAM lParam)
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

	CTSMagicView* pWindow = CTSMagicView::GetView();

//	uint8_t	ucSubType = 0x00;

	if (pPMT != NULL)
	{
		for (es_index = 0; es_index < pPMT->m_nEsCount; es_index++)
		{
			nPIDStyle = StreamTypeLookup(pPMT->m_astEsInfo[es_index].stream_type, pPMT->m_astEsInfo[es_index].stream_subtype, NULL);
			if (nPIDStyle == PIDSTYLE_SUBTITLE)
			{
				pWnd = GetDlgItem(IDC_BTN_SUBTITLE_TRIGGER);
				pWnd->EnableWindow(TRUE);

				bExit = 0;
				nItemCount = m_listSubtitlePID.GetItemCount();
				for (item = 0; item < nItemCount; item ++)
				{
					dwPID = m_listSubtitlePID.GetItemData(item);
					if (dwPID == pPMT->m_astEsInfo[es_index].elementary_PID)
					{
						m_listSubtitlePID.GetItemText(item, 2, pszTemp, MAX_TXT_CHARS);

						bExit = 1;
						break;
					}
				}

				if (bExit == 0)
				{
					m_listSubtitlePID.InsertItem(0, "", 5);

					sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID);

					m_listSubtitlePID.SetItemData(0, pPMT->m_astEsInfo[es_index].elementary_PID);
					m_listSubtitlePID.SetItemText(0, 1, pszText);

					sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
					m_listSubtitlePID.SetItemData(0, pPMT->m_astEsInfo[es_index].elementary_PID);
					m_listSubtitlePID.SetItemText(0, 2, pszText);
				}
				else
				{
					sprintf_s(pszText, sizeof(pszText), "%s, 0x%04X(%d)", pszTemp, pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
					m_listSubtitlePID.SetItemText(0, 2, pszText);
				}
			}
		}
	}
*/	
	return 0;
}

void CDlg_TSAnalyzer_Subtitle::InitTree(void)
{
//	TREEITEM hVidItem;
//	TREEITEM hAudItem;

	//TreeFun_InitTree(&m_treeSubtitleSyntax, this, IDC_TREE_TELETEXT_SYNTAX);
/*
	hVidItem = TreeFun_InsertItem(&m_treeSubtitleSyntax, TREE_ROOT, "video_stream()", -1, 0, NULL);
	m_hVidPesItem = TreeFun_InsertItem(&m_treeSubtitleSyntax, hVidItem, "PES_packet()", -1, 0, NULL);
	m_hVidEsItem = TreeFun_InsertItem(&m_treeSubtitleSyntax, hVidItem, "ES()", -1, 0, NULL);

	hAudItem = TreeFun_InsertItem(&m_treeSubtitleSyntax, TREE_ROOT, "audio_stream()", -1, 0, NULL);
	m_hAudPesItem = TreeFun_InsertItem(&m_treeSubtitleSyntax, hAudItem, "PES_packet()", -1, 0, NULL);
	m_hAudEsItem = TreeFun_InsertItem(&m_treeSubtitleSyntax, hAudItem, "ES()", -1, 0, NULL);

	m_treeSubtitleSyntax.Expand(hVidItem, TREE_EXPAND);
	m_treeSubtitleSyntax.Expand(hAudItem, TREE_EXPAND);
*/	
}

LRESULT CDlg_TSAnalyzer_Subtitle::OnUpdatePesHeader(WPARAM wParam, LPARAM lParam)
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
		if (!TreeFun_HasChild(&m_treeSubtitleSyntax, hParentItem))
		{
			PES_HEADER(&m_treeSubtitleSyntax, hParentItem, pPES_packet);
//			m_treeSubtitleSyntax.Expand(hParentItem, TVE_EXPAND);
		}

		UpdateDtsPts(pPES_packet, 0);
	}
*/
	
	return 0;
}

void CDlg_TSAnalyzer_Subtitle::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
