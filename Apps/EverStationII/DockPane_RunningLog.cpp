
#include "stdafx.h"

#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar
#include "DockPane_RunningLog.h"

CDockPane_RunningLog::CDockPane_RunningLog()
{
}

CDockPane_RunningLog::~CDockPane_RunningLog()
{
}

BEGIN_MESSAGE_MAP(CDockPane_RunningLog, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int CDockPane_RunningLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建选项卡窗口: 
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("未能创建输出选项卡窗口\n");
		return -1;      // 未能创建
	}

	// 创建输出窗格: 
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndUserActionDebug.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("未能创建用户操作日志窗口\n");
		return -1;      // 未能创建
	}
	if (!m_wndLogTSDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3))
	{
		TRACE0("未能创建TS Debug窗口\n");
		return -1;      // 未能创建
	}
	if (!m_wndLogPcrDebug.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("未能创建Pcr Debug窗口\n");
		return -1;      // 未能创建
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// 将列表窗口附加到选项卡: 
	bNameValid = strTabName.LoadString(IDS_USER_ACTION_TAB);
	ASSERT(bNameValid);
	if (m_wndUserActionDebug.GetSafeHwnd() != NULL)
	{
		m_wndTabs.AddTab(&m_wndUserActionDebug, strTabName, (UINT)0);
	}

	bNameValid = strTabName.LoadString(IDS_TS_WARNING_TAB);
	ASSERT(bNameValid);
	if (m_wndLogTSDebug.GetSafeHwnd() != NULL)
	{
		m_wndTabs.AddTab(&m_wndLogTSDebug, strTabName, (UINT)1);
	}

	bNameValid = strTabName.LoadString(IDS_PCR_RECORD_TAB);
	ASSERT(bNameValid);
	if (m_wndLogPcrDebug.GetSafeHwnd() != NULL)
	{
		m_wndTabs.AddTab(&m_wndLogPcrDebug, strTabName, (UINT)2);
	}

	return 0;
}

void CDockPane_RunningLog::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区: 
	m_wndTabs.SetWindowPos (nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

//void CDockPane_RunningLog::AdjustHorzScroll(CListBox& wndListBox)
//{
//	CClientDC dc(this);
//	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);
//
//	int cxExtentMax = 0;
//
//	for (int i = 0; i < wndListBox.GetCount(); i ++)
//	{
//		CString strItem;
//		wndListBox.GetText(i, strItem);
//
//		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
//	}
//
//	wndListBox.SetHorizontalExtent(cxExtentMax);
//	dc.SelectObject(pOldFont);
//}

void CDockPane_RunningLog::UpdateFonts()
{
	if (m_wndUserActionDebug.GetSafeHwnd() != NULL)
	{
		m_wndUserActionDebug.SetFont(&afxGlobalData.fontRegular);
	}
	if (m_wndLogTSDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogTSDebug.SetFont(&afxGlobalData.fontRegular);
	}
	if (m_wndLogPcrDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogPcrDebug.SetFont(&afxGlobalData.fontRegular);
	}
}

void CDockPane_RunningLog::AppendLogForUserAction(int type, char* pszItem)
{
	if (m_wndUserActionDebug.GetSafeHwnd() != NULL)
	{
		m_wndUserActionDebug.AppendLog(type, pszItem);
	}
}

//void CDockPane_RunningLog::AppendLogForUserActionW(int type, wchar_t* pszItem)
//{
//	if (m_wndUserActionDebug.GetSafeHwnd() != NULL)
//	{
//		m_wndUserActionDebug.AppendLogW(type, pszItem);
//	}
//}

void CDockPane_RunningLog::AppendLogForTSWarning(int type, char* pszItem)
{
	if (m_wndLogTSDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogTSDebug.AppendLog(type, pszItem);
	}
}

void CDockPane_RunningLog::AppendLogForPcrRecord(int type, char* pszItem)
{
	if (m_wndLogPcrDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogPcrDebug.AppendLog(type, pszItem);
	}
}

void CDockPane_RunningLog::Open(void)
{
	if (m_wndUserActionDebug.GetSafeHwnd() != NULL)
	{
		m_wndUserActionDebug.Open();
	}

	if (m_wndLogTSDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogTSDebug.Open();
	}

	if (m_wndLogPcrDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogPcrDebug.Open();
	}

	SetTimer(TIMER_SHOW_LOG, 500, NULL);
}

void CDockPane_RunningLog::Close(void)
{
	KillTimer(TIMER_SHOW_LOG);

	if (m_wndUserActionDebug.GetSafeHwnd() != NULL)
	{
		m_wndUserActionDebug.Close();
	}

	if (m_wndLogTSDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogTSDebug.Close();
	}

	if (m_wndLogPcrDebug.GetSafeHwnd() != NULL)
	{
		m_wndLogPcrDebug.Close();
	}
}


void CDockPane_RunningLog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TRACE("CDockPane_RunningLog\n");

	m_wndUserActionDebug.ShowLog();
	m_wndLogTSDebug.ShowLog();
	m_wndLogPcrDebug.ShowLog();

	CDockablePane::OnTimer(nIDEvent);
}


void CDockPane_RunningLog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	if (bShow)
	{
		Open();
	}
	else
	{
		Close();
	}
}
