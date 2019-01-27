// ListView_RunningLog.cpp : implementation file
//

#include "stdafx.h"

#include "../resource.h"
#include "ListView_RunningLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrl_RunningLog

CListCtrl_RunningLog::CListCtrl_RunningLog()
{
	m_hLogAccess = NULL;
}

CListCtrl_RunningLog::~CListCtrl_RunningLog()
{
}

BEGIN_MESSAGE_MAP(CListCtrl_RunningLog, CListCtrl)
	ON_WM_CONTEXTMENU()
	//ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_RUNNINGLOGWND, OnViewRunningLog)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void CListCtrl_RunningLog::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

//void CListCtrl_RunningLog::OnEditCopy()
//{
//	MessageBox(_T("复制输出"));
//}
//
//void CListCtrl_RunningLog::OnEditClear()
//{
//	MessageBox(_T("清除输出"));
//}

void CListCtrl_RunningLog::OnViewRunningLog()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != nullptr && pParentBar != nullptr)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}


int CListCtrl_RunningLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	int		width = 1000;
	SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	InsertColumn(0, "", LVCFMT_CENTER, 40, -1);					//ICON
	width -= 40;

	InsertColumn(1, "序号", LVCFMT_LEFT, 100, -1);
	width -= 100;

	InsertColumn(2, "时间", LVCFMT_LEFT, 180, -1);
	width -= 180;

	if (width < 0)
	{
		width = 100;
	}
	InsertColumn(3, "日志描述信息", LVCFMT_LEFT, width, -1);

	m_imageDebug.Create(IDB_LOG, 16, 1, RGB(0xab, 0xcd, 0xef));
	SetImageList(&m_imageDebug, LVSIL_SMALL);

	m_nLogCount = 0;
	m_hLogAccess = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	//SetTimer(TIMER_SHOW_LOG, 200, NULL);

	m_nDebugCount = 0;

	return 0;
}


BOOL CListCtrl_RunningLog::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	return CListCtrl::Create(dwStyle, rect, pParentWnd, nID);
}


void CListCtrl_RunningLog::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	int width = cx;
	
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	int columns = pHeaderCtrl->GetItemCount();

	for (int i = 0; i < columns - 1; i++)
	{
		width -= GetColumnWidth(i);
	}
	SetColumnWidth(columns - 1, width);
}

//void CListCtrl_RunningLog::AppendLogW(int type, wchar_t* pszItem)
//{
//	if (pszItem != NULL)
//	{
//		if (m_hLogAccess != NULL)
//		{
//			::WaitForSingleObject(m_hLogAccess, INFINITE);
//		}
//
//		if (m_nLogCount < LOG_ITEM_BUF_COUNT)
//		{
//			m_logItem[m_nLogCount].type = type;
//			//strcpy_s(m_logItem[m_nLogCount].pszText, sizeof(m_logItem[m_nLogCount].pszText), pszItem);
//
//			m_nLogCount++;
//		}
//
//		if (m_hLogAccess != NULL)
//		{
//			::SetEvent(m_hLogAccess);
//		}
//	}
//}

void CListCtrl_RunningLog::AppendLog(int type, char* pszItem)
{
	if (pszItem != NULL)
	{
		if (m_hLogAccess != NULL)
		{
			::WaitForSingleObject(m_hLogAccess, INFINITE);
		}

		if (m_nLogCount < LOG_ITEM_BUF_COUNT)
		{
			m_logItem[m_nLogCount].type = type;
			strcpy_s(m_logItem[m_nLogCount].pszText, sizeof(m_logItem[m_nLogCount].pszText), pszItem);

			m_nLogCount++;
		}

		if (m_hLogAccess != NULL)
		{
			::SetEvent(m_hLogAccess);
		}
	}
}

void CListCtrl_RunningLog::ShowLog(void)
{
	CString			strTime;
	int				i;
	char*			pszItem;
	int				type;
	int				nCount;
	char			pszText[256];
	COleDateTime	timeCurrent;

	if (m_nLogCount > 0)
	{
		if (m_hLogAccess != NULL)
		{
			::WaitForSingleObject(m_hLogAccess, INFINITE);
		}

		for (i = 0; i < m_nLogCount; i++)
		{
			nCount = GetItemCount();

			if (nCount >= RUNNING_LOG_MAX_COUNT)
			{
				DeleteAllItems();
				nCount = 0;
			}

			pszItem = m_logItem[i].pszText;
			type = m_logItem[i].type;

			if (pszItem != NULL)
			{
				timeCurrent = COleDateTime::GetCurrentTime();

				InsertItem(nCount, "", type);

				sprintf_s(pszText, sizeof(pszText), "%d", m_nDebugCount++);
				SetItemText(nCount, 1, pszText);

				strTime = timeCurrent.Format("%Y-%m-%d-%H:%M:%S");
				SetItemText(nCount, 2, strTime);

				SetItemText(nCount, 3, pszItem);

				EnsureVisible(nCount, 0);
			}
		}

		m_nLogCount = 0;

		if (m_hLogAccess != NULL)
		{
			::SetEvent(m_hLogAccess);
		}
	}
}

void CListCtrl_RunningLog::OnDestroy()
{
	CListCtrl::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//KillTimer(TIMER_SHOW_LOG);

	//::CloseHandle(m_hLogAccess);
	//m_hLogAccess = NULL;
}


//void CListCtrl_RunningLog::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if (nIDEvent == TIMER_SHOW_LOG)
//	{
//		ShowLog();
//	}
//
//	CListCtrl::OnTimer(nIDEvent);
//}

void CListCtrl_RunningLog::Open(void)
{
	m_nLogCount = 0;
	m_nDebugCount = 0;
}

void CListCtrl_RunningLog::Close(void)
{
	DeleteAllItems();
	m_nLogCount = 0;
	m_nDebugCount = 0;
}