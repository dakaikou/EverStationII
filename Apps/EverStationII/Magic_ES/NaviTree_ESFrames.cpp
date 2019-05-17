// NaviTree_ESFrames.cpp : implementation file
//

#include "stdafx.h"
#include "NaviTree_ESFrames.h"
//#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
//#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Utilities.h"
//#include "syntax_translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_Utilities.h"

//#include "../resource.h"
//#include "..\Common\define.h"
#include "..\Common\Dlg_VideoShowScreen.h"

//#include "..\Magic_TS\TSMagic_GuiApi_MSG.h"
//#include "..\Magic_TS\TSMagic_Trigger_PESES.h"
/////////////////////////////////////////////////////////////////////////////
// CNaviTree_ESFrames

IMPLEMENT_DYNCREATE(CNaviTree_ESFrames, CTreeView)

CNaviTree_ESFrames::CNaviTree_ESFrames()
{
}

CNaviTree_ESFrames::~CNaviTree_ESFrames()
{
//	DeleteChildItems(m_hPsisiRootItem);
//	DeleteChildItems(m_hDataCastRootItem);
}


BEGIN_MESSAGE_MAP(CNaviTree_ESFrames, CTreeView)
	//{{AFX_MSG_MAP(CNaviTree_ESFrames)
	ON_WM_CREATE()
//	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_RCLICK, &CNaviTree_ESFrames::OnNMRClick)
	ON_COMMAND(ID_ES_VIDEO_PREVIEW, &CNaviTree_ESFrames::OnEsVideoPreview)
	ON_COMMAND(ID_ES_AUDIO_PREVIEW, &CNaviTree_ESFrames::OnEsAudioPreview)
	ON_COMMAND(ID_ES_SYNTAX_ANALYSE, &CNaviTree_ESFrames::OnEsSyntaxAnalyse)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CNaviTree_ESFrames::OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_ESFrames drawing

void CNaviTree_ESFrames::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_ESFrames diagnostics

#ifdef _DEBUG
void CNaviTree_ESFrames::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNaviTree_ESFrames::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPane_PesEsPIDTreeView message handlers

BOOL CNaviTree_ESFrames::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= (TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT);
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CNaviTree_ESFrames::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[128];

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

//	m_ImageList.Create(IDB_TREE_INFO, 20, 1, RGB(255, 255, 255));
	m_ImageList.Create(IDB_ES, 20, 1, RGB(0xab, 0xcd, 0xef));
	treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	TreeCtrlItem.hParent = TVI_ROOT;
#if CHINESE_VERSION
	strcpy_s(pszText, sizeof(pszText), "现行TS流中的节目");
#else
	strcpy_s(pszText, sizeof(pszText), "programs in actural TS");
#endif
	m_hPesEsRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hPesEsRootItem, 0x00000000);
	treeCtrl.SetItemImage(m_hPesEsRootItem, 0, 0);
	treeCtrl.SetItemState(m_hPesEsRootItem, ~TVIS_BOLD, TVIS_BOLD);

//	treeCtrl.Expand(m_hPsisiRootItem, TVE_EXPAND);
//	treeCtrl.Expand(m_hDataCastRootItem, TVE_EXPAND);

	return 0;
}

//void CNaviTree_ESFrames::UpdatePAT(CPAT* pPAT)
//{
//	char			pszText[64];
//	int				valid_program_count;
//
//	PROGRAM_MAP_DESCRIPTION_t		ProgramMap;
//	HTREEITEM		hItem;
//
//	CTreeCtrl& treeCtrl = GetTreeCtrl();
//
//	if (pPAT != NULL)
//	{
//		valid_program_count = 0;
//
//		int nProgramCount = pPAT->GetProgramCount();
//		for (int program_index = 0; program_index < nProgramCount; program_index++)
//		{
//			int rtcode = pPAT->GetProgramByIndex(program_index, &ProgramMap);
//			if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
//			{
//				if (ProgramMap.program_number != 0x0000)
//				{
//#if CHINESE_VERSION
//					sprintf_s(pszText, sizeof(pszText), "节目 0x%04X(%d)", ProgramMap.program_number, ProgramMap.program_number);
//#else
//					sprintf_s(pszText, sizeof(pszText), "PROGRAM 0x%04X(%d)", ProgramMap.program_number, ProgramMap.program_number);
//#endif
//					hItem = treeCtrl.InsertItem(pszText, m_hPesEsRootItem);
//
//					treeCtrl.SetItemData(hItem, ProgramMap.program_number & 0x0000FFFF);
//					treeCtrl.SetItemImage(hItem, 1, 1);
//
//					valid_program_count++;
//				}
//			}
//		}
//		treeCtrl.SortChildren(m_hPesEsRootItem);
//
//		uint16_t usTransportStreamID = pPAT->GetTransportStreamID();
//#if CHINESE_VERSION
//		sprintf_s(pszText, sizeof(pszText), "现行流<TSID=0x%04X(%d)>中的节目(共%d个)", usTransportStreamID, usTransportStreamID, valid_program_count);
//#else
//		sprintf_s(pszText, sizeof(pszText), "programs in actural stream<TSID=0x%04X(%d)>-(total:%d)", usTransportStreamID, usTransportStreamID, valid_program_count);
//#endif
//		treeCtrl.SetItemText(m_hPesEsRootItem, pszText);
//
//		treeCtrl.SetItemState(m_hPesEsRootItem, TVIS_BOLD, TVIS_BOLD);
//		treeCtrl.Expand(m_hPesEsRootItem, TVE_EXPAND);
//	}
//}

//void CNaviTree_ESFrames::UpdatePMT(CPMT* pPMT)
//{
//	char			pszText[MAX_TXT_CHARS];
//	char			pszTemp[128];
//	int				program_number;
//	uint8_t				class_type;
//	HTREEITEM		hESItem;
//	HTREEITEM		hPmtItem = NULL;
//	DWORD			dwItemData;
//	BOOL			bFound = false;
//	int				valid_program_count = 0;
//
//	CTreeCtrl& treeCtrl = GetTreeCtrl();
//
//	if (pPMT != NULL)
//	{
//		uint16_t usProgramNumber = pPMT->GetProgramNumber();
//
//		if (treeCtrl.ItemHasChildren(m_hPesEsRootItem))
//		{
//			HTREEITEM hNextItem;
//			HTREEITEM hChildItem = treeCtrl.GetChildItem(m_hPesEsRootItem);
//			while (hChildItem != NULL)
//			{
//				valid_program_count++;
//				program_number = treeCtrl.GetItemData(hChildItem) & 0x0000ffff;
//				if (program_number == usProgramNumber)
//				{
//					treeCtrl.SetItemState(hChildItem, TVIS_BOLD, TVIS_BOLD);
//
//					hPmtItem = hChildItem;
//					bFound = true;
//
//					break;
//				}
//				else
//				{
//					hNextItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
//					hChildItem = hNextItem;
//				}
//			}
//
//			if (!bFound)
//			{
//#if CHINESE_VERSION
//				sprintf_s(pszText, sizeof(pszText), "节目 0x%04X(%d)", usProgramNumber, usProgramNumber);
//#else
//				sprintf_s(pszText, sizeof(pszText), "PROGRAM 0x%04X(%d)", pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
//#endif
//				hPmtItem = treeCtrl.InsertItem(pszText, m_hPesEsRootItem);
//
//				treeCtrl.SetItemData(hPmtItem, usProgramNumber & 0x0000FFFF);
//				treeCtrl.SetItemImage(hPmtItem, 1, 1);
//				treeCtrl.SetItemState(hPmtItem, TVIS_BOLD, TVIS_BOLD);
//
//				valid_program_count++;
//				treeCtrl.SortChildren(m_hPesEsRootItem);
//
////#if CHINESE_VERSION
////				sprintf_s(pszText, sizeof(pszText), "现行流<TSID=0x%04X(%d)>中的节目(共%d个)", pPAT->m_usTransportStreamID, pPAT->m_usTransportStreamID, valid_program_count);
////#else
////				sprintf_s(pszText, sizeof(pszText), "programs in actural stream<TSID=0x%04X(%d)>-(total:%d)", pPAT->m_usTransportStreamID, pPAT->m_usTransportStreamID, valid_program_count);
////#endif
////				treeCtrl.SetItemText(m_hPesEsRootItem, pszText);
//			}
//
//			if (hPmtItem != NULL)
//			{
//				int nEsCount = pPMT->GetESCount();
//				ES_INFO_t es_info;
//
//				for (int es_index = 0; es_index < nEsCount; es_index++)
//				{
//					int rtcode = pPMT->GetESInfo(es_index, &es_info);
//					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
//					{
//						if (es_info.elementary_PID == 0x0600)
//						{
//							int debug = 1;   //仅调试用
//						}
//
//						MPEG2_PSI_NumericCoding2Text_StreamType(es_info.stream_type, es_info.stream_subtype, pszTemp, sizeof(pszTemp));
//						class_type = MPEG2_TS_GetPayloadClassByStreamType(es_info.stream_type, es_info.stream_subtype);
//						//pid_style = StreamTypeLookup(pPMT->m_astEsInfo[es_index].stream_type, pPMT->m_astEsInfo[es_index].stream_subtype, pszTemp, sizeof(pszTemp));
//						//class_type = (pid_style & 0x00070000) >> 16;
//						//main_type = (pid_style & 0x0000ff00) >> 8;
//						//sub_type = (pid_style & 0x000000ff);
//
//						if (class_type == TSPAYLOAD_CLASS_SECTION)
//						{
//#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "PID=0x%04X(%d)[%s]", es_info.elementary_PID, es_info.elementary_PID, pszTemp);
//#else
//							sprintf_s(pszText, sizeof(pszText), "Section:PID=0x%04X(%d)", es_info.elementary_PID, es_info.elementary_PID);
//#endif
//							hESItem = treeCtrl.InsertItem(pszText, hPmtItem);
//							treeCtrl.SetItemImage(hESItem, 1, 1);
//						}
//						else if (class_type == TSPAYLOAD_CLASS_PES_VIDEO)
//						{
//#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "PID=0x%04X(%d)[%s]", es_info.elementary_PID, es_info.elementary_PID, pszTemp);
//#else
//							sprintf_s(pszText, sizeof(pszText), "Video:PID=0x%04X(%d)", es_info.elementary_PID, es_info.elementary_PID);
//#endif
//							hESItem = treeCtrl.InsertItem(pszText, hPmtItem);
//							treeCtrl.SetItemImage(hESItem, 2, 2);
//						}
//						else if (class_type == TSPAYLOAD_CLASS_PES_AUDIO)
//						{
//#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "PID=0x%04X(%d)[%s]", es_info.elementary_PID, es_info.elementary_PID, pszTemp);
//#else
//							sprintf_s(pszText, sizeof(pszText), "Audio:PID=0x%04X(%d)", es_info.elementary_PID, es_info.elementary_PID);
//#endif
//							hESItem = treeCtrl.InsertItem(pszText, hPmtItem);
//							treeCtrl.SetItemImage(hESItem, 3, 3);
//						}
//						else //默认都为数据
//						{
//#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "PID=0x%04X(%d)[%s]", es_info.elementary_PID, es_info.elementary_PID, pszTemp);
//#else
//							sprintf_s(pszText, sizeof(pszText), "Data:PID=0x%04X(%d)", es_info.elementary_PID, es_info.elementary_PID);
//#endif
//							hESItem = treeCtrl.InsertItem(pszText, hPmtItem);
//							treeCtrl.SetItemImage(hESItem, 4, 4);
//						}
//
//						dwItemData = class_type;
//						dwItemData <<= 13;
//						dwItemData |= es_info.elementary_PID;
//						dwItemData <<= 8;
//						dwItemData |= es_info.stream_type;
//						dwItemData <<= 8;
//						dwItemData |= es_info.stream_subtype;
//
//						treeCtrl.SetItemData(hESItem, dwItemData);
//					}
//				}
//			}
//		}
//
//		treeCtrl.Expand(m_hPesEsRootItem, TVE_EXPAND);
//	}
//}

void CNaviTree_ESFrames::Set(HWND hwndReceiver, int offline)
{
	m_hwndReceiver = hwndReceiver;
}

void CNaviTree_ESFrames::Reset(void)
{
	char			pszText[128];

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	DeleteChildItems(m_hPesEsRootItem);
#if CHINESE_VERSION
	strcpy_s(pszText, sizeof(pszText), "现行TS流中的节目");
#else
	strcpy_s(pszText, sizeof(pszText), "programs in actural TS");
#endif
	treeCtrl.SetItemText(m_hPesEsRootItem, pszText);
	treeCtrl.SetItemState(m_hPesEsRootItem, ~TVIS_BOLD, TVIS_BOLD);
}

void CNaviTree_ESFrames::DeleteChildItems(HTREEITEM hParentItem)
{
	HTREEITEM hChildItem;
	HTREEITEM hOldItem;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (treeCtrl.ItemHasChildren(hParentItem))
	{
		hChildItem = treeCtrl.GetChildItem(hParentItem);

		while (1)
		{
			hOldItem = hChildItem;
			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);

			treeCtrl.DeleteItem(hOldItem);

			if (hChildItem == NULL)
			{
				break;
			}
		}
	}
}

void CNaviTree_ESFrames::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
	DWORD dwItemData = (uint32_t)treeCtrl.GetItemData(hSelItem);

	if ((dwItemData & 0xffff0000) != 0x00000000)
	{
		::SendMessage(m_hwndReceiver, WM_USER_ES_FRAME_SEL_CHANGE, NULL, dwItemData);
	}

	*pResult = 0;
}

void CNaviTree_ESFrames::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//int			  nClassType;
	//int			  nPID;
	//int			  nStreamType;
	//int			  nSubType;

	//CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	//CTreeCtrl& treeCtrl = GetTreeCtrl();

	//HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
	//DWORD dwItemData = (uint32_t)treeCtrl.GetItemData(hSelItem);

	//if ((dwItemData & 0xffff0000) != 0x00000000)
	//{
	//	nClassType = (dwItemData & 0xE0000000) >> 29;
	//	nPID = (dwItemData & 0x1FFF0000) >> 16;
	//	nStreamType = (dwItemData & 0x0000FF00) >> 8;
	//	nSubType = (dwItemData & 0x000000FF);

	//	if (((pTSMagicView->m_kThreadParams.offline == 1) && (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)) ||
	//		(pTSMagicView->m_kThreadParams.offline == 0))
	//	{
	//		CMenu	menu, *pSubMenu;
	//		menu.LoadMenu(IDR_RIGHT_KEY_MENU);//装载自定义的右键菜单

	//		pSubMenu = menu.GetSubMenu(1);//获取第一个弹出菜单，所以第一个菜单必须有子菜单

	//		CPoint oPoint;//定义一个用于确定光标位置的位置
	//		GetCursorPos(&oPoint);//获取当前光标的位置，以便使得菜单可以跟随光标

	//		if (nClassType == TSPAYLOAD_CLASS_PES_VIDEO)
	//		{
	//			pSubMenu->EnableMenuItem(ID_ES_VIDEO_PREVIEW, MF_ENABLED);
	//		}
	//		else if (nClassType == TSPAYLOAD_CLASS_PES_AUDIO)
	//		{
	//			pSubMenu->EnableMenuItem(ID_ES_AUDIO_PREVIEW, MF_ENABLED);
	//		}

	//		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this); //在指定位置显示弹出菜单
	//	}
	//}

	*pResult = 0;
}


void CNaviTree_ESFrames::OnEsVideoPreview()
{
	// TODO: 在此添加命令处理程序代码
	CDlg_VideoShowScreen	m_dlgTV;

	m_dlgTV.DoModal();
}


void CNaviTree_ESFrames::OnEsAudioPreview()
{
	// TODO: 在此添加命令处理程序代码
}


void CNaviTree_ESFrames::OnEsSyntaxAnalyse()
{
	// TODO: 在此添加命令处理程序代码
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
	DWORD dwItemData = (uint32_t)treeCtrl.GetItemData(hSelItem);

	if ((dwItemData & 0xffff0000) != 0x00000000)
	{
		::SendMessage(m_hwndReceiver, WM_USER_ES_FRAME_SEL_CHANGE, NULL, dwItemData);
	}
}

//void CNaviTree_ESs::FiringCatchThread(void)
//{
//	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
//	char		  pszText[MAX_TXT_CHARS];
//	int			  nClassType;
//	int			  nPID;
//	int			  nStreamType;
//	int			  nSubType;
//
//	CTreeCtrl& treeCtrl = GetTreeCtrl();
//
//	HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
//	DWORD dwItemData = (uint32_t)treeCtrl.GetItemData(hSelItem);
//
//	if ((dwItemData & 0xffff0000) != 0x00000000)
//	{
//		nClassType = (dwItemData & 0xE0000000) >> 29;
//		nPID = (dwItemData & 0x1FFF0000) >> 16;
//		nStreamType = (dwItemData & 0x0000FF00) >> 8;
//		nSubType = (dwItemData & 0x000000FF);
//
//		if ((nClassType == TSPAYLOAD_CLASS_PES_AUDIO) ||
//			(nClassType == TSPAYLOAD_CLASS_PES_VIDEO) ||
//			(nClassType == TSPAYLOAD_CLASS_PES_DATA))
//		{
//			if (pTSMagicView->m_kThreadParams.main_thread_running)
//			{
//				if ((pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1) ||
//					(pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1) ||
//					(pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1) ||
//					(pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1) ||
//					(pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1) ||
//					(pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1))
//				{
//					if (pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1)
//					{
//						sprintf_s(pszText, sizeof(pszText), "PES捕捉：未能启动，因为发现TS捕捉线程尚未结束！");
//						::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
//					}
//					if (pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1)
//					{
//						sprintf_s(pszText, sizeof(pszText), "PES捕捉：未能启动，因为发现PES捕捉线程尚未结束！");
//						::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
//					}
//					if (pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1)
//					{
//						sprintf_s(pszText, sizeof(pszText), "PES捕捉：未能启动，因为发现ES捕捉线程尚未结束！");
//						::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
//					}
//					if (pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1)
//					{
//						sprintf_s(pszText, sizeof(pszText), "PES捕捉：未能启动，因为发现section捕捉线程尚未结束！");
//						::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
//					}
//					if (pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1)
//					{
//						sprintf_s(pszText, sizeof(pszText), "PES捕捉：未能启动，因为发现DSMCC下载线程尚未结束！");
//						::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
//					}
//					if (pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1)
//					{
//						sprintf_s(pszText, sizeof(pszText), "PES捕捉：未能启动，因为发现TS录制线程尚未结束！");
//						::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
//					}
//				}
//				else
//				{
//					CTrigger_PESPacket* pPESPacketTrigger = pTSMagicView->GetPESPacketTrigger();
//					pPESPacketTrigger->SetMatchParams(-1, nPID, dwItemData, 1);
//					pTSMagicView->m_kThreadParams.hPesEsMsgWnd = pTSMagicView->m_dlgTSAnalyzerPesEs.GetSafeHwnd();
//
//					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_PES_TRIGGER_STATE, 1, 0);
//
//					if (pTSMagicView->m_kThreadParams.offline == 1)
//					{
//						if (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)
//						{
//							pTSMagicView->m_kThreadParams.pes_trigger_thread_running = 0;
//							::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_pes_trigger_thread, (LPVOID)&(pTSMagicView->m_kThreadParams), 0, 0);
//						}
//					}
//				}
//			}
//		}
//	}
//}
