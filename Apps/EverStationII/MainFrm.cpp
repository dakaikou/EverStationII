
// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "EverStationII.h"

#include "MainFrm.h"
#include "resource.h"
#include "Common\GuiApi_MSG.h"
#include "Common\Define.h"
#include "thirdparty_libs\glog\glog\logging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	//ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_APPEND_LOG, OnAppendLog)
	//ON_WM_PAINT()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_SEPARATOR,           // 状态行指示器
	ID_SEPARATOR,           // 状态行指示器
	//IDS_ANALYSE_OBJECT,
	//IDS_ANALYSE_PROGRESS,
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	m_rectPrimaryDesktop = { 0,0,0,0 };		//left、top、right、bottom
	m_rectSecondDesktop = { 0,0,0,0 };		//left、top、right、bottom
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMDIFrameWndEx::EnableLoadDockState(FALSE);				//added by chendelin 2018.3.25

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	if (!m_wndToolBar_EverStation.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_EverStation.LoadToolBar(theApp.m_bHiColorIcons ? IDR_TOOLBAR_FRAME_256 : IDR_TOOLBAR_FRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	//m_wndToolBar_EverStation.SetWindowTextA(_T("码流分析工具条"));
	//m_wndToolBar_EverStation.SetWindowTextA(strToolBarName);
	//m_wndToolBar_EverStation.EnableCustomizeButton(FALSE, ID_VIEW_CUSTOMIZE, strCustomize);

#if SHOW_SYSDEFAULT_TOOLBAR
	if (!m_wndToolBar_SysDefault.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_SysDefault.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar_SysDefault.SetWindowText(strToolBarName);
	m_wndToolBar_SysDefault.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
#endif


	//// 允许用户定义的工具栏操作: 
	//InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(1, 0L, SBPS_NORMAL, 500);
	m_wndStatusBar.SetTipText(1, "分析对象");
	//m_wndStatusBar.EnablePaneProgressBar(1, 100, 1);
	m_wndStatusBar.SetPaneInfo(2, 0L, SBPS_NORMAL, 200);
	m_wndStatusBar.SetTipText(2, "文件分析进度");
	m_wndStatusBar.EnablePaneProgressBar(2, 100, 1);

	EnableDocking(CBRS_ALIGN_ANY);

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	//m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar_EverStation.EnableDocking(CBRS_ALIGN_ANY);
#if SHOW_SYSDEFAULT_TOOLBAR
	m_wndToolBar_SysDefault.EnableDocking(CBRS_ALIGN_ANY);
#endif
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar_EverStation);
#if SHOW_SYSDEFAULT_TOOLBAR
	DockPane(&m_wndToolBar_SysDefault);
#endif


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上): 
	//CMFCToolBar::AddToolBarForImageCollection(IDR_TOOLBAR_FRAME, theApp.m_bHiColorIcons ? IDB_TOOLBAR_IMAGES_24 : 0);
	//m_wndToolBar_EverStation.AddToolBarForImageCollection(IDR_TOOLBAR_FRAME, theApp.m_bHiColorIcons ? IDB_TOOLBAR_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

#if SHOW_PROPERTIES_WND
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
#endif

	m_DockPane_RunningLog.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_DockPane_RunningLog);

	//m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndFileView);
	//CDockablePane* pTabbedBar = nullptr;
	//m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用增强的窗口管理对话框
	//EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	//CMFCToolBar::EnableQuickCustomization();

	//if (CMFCToolBar::GetUserImages() == nullptr)
	//{
	//	// 加载用户定义的工具栏图像
	//	if (m_imgToolBar_EverStation.Load(_T("./res/toolbar_everstation.bmp")))
	//	{
	//		//CMFCToolBar::SetUserImages(&m_imgToolBar_EverStation);
	//		m_wndToolBar_EverStation.SetUserImages(&m_imgToolBar_EverStation);
	//	}
	//}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	//lstBasicCommands.AddTail(ID_FILE_NEW);
	//lstBasicCommands.AddTail(ID_FILE_OPEN);
	//lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	//lstBasicCommands.AddTail(ID_EDIT_CUT);
	//lstBasicCommands.AddTail(ID_EDIT_PASTE);
	//lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	CheckMonitorInfo(this->GetSafeHwnd(), &m_rectPrimaryDesktop, &m_rectSecondDesktop);		//added by chendelin 2019.1.31

	return 0;
}

//BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
//{
//	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
//		return FALSE;
//	// TODO: 在此处通过修改
//	//  CREATESTRUCT cs 来修改窗口类或样式
//
//	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
//		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
//
//	return TRUE;
//}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	int  left_pane_width;
	int  right_pane_width;
	int  total_width;
	int	 top_pane_height;
	int  bottom_pane_height;
	int  total_height;

	//获取主窗口的矩形客户区尺寸
	CRect rectFrame;
	GetClientRect(&rectFrame);

	total_width = rectFrame.Width();
	right_pane_width = (int)(total_width * 0.2);
	left_pane_width = (int)(total_width * 0.2);	
	//right_pane_width = 400;
	//left_pane_width = 300;

	total_height = rectFrame.Height();
	bottom_pane_height = (int)(total_height * 0.318);		//黄金分割点
	//bottom_pane_height = 500;
	top_pane_height = total_height - bottom_pane_height;

#if SHOW_PROPERTIES_WND
	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, right_pane_width, total_height), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
#endif

	// 创建输出窗口
	CString strRunningLogWnd;
	bNameValid = strRunningLogWnd.LoadString(IDS_RUNNING_LOG_WND);
	ASSERT(bNameValid);
	if (!m_DockPane_RunningLog.Create(strRunningLogWnd, this, CRect(0, 0, total_width, bottom_pane_height), TRUE, ID_VIEW_RUNNINGLOGWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建类视图
	//CString strClassView;
	//bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	//ASSERT(bNameValid);
	//if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, left_pane_width, top_pane_height), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("未能创建“类视图”窗口\n");
	//	return FALSE; // 未能创建
	//}

	// 创建文件视图
	//CString strFileView;
	//bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	//ASSERT(bNameValid);
	//if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, left_pane_width, top_pane_height), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("未能创建“文件视图”窗口\n");
	//	return FALSE; // 未能创建
	//}

	//SetDockingWindowIcons(theApp.m_bHiColorIcons);

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	//HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	//HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_DockPane_RunningLog.SetIcon(hOutputBarIcon, FALSE);

	//HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

//void CMainFrame::OnViewCustomize()
//{
//	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
//	pDlgCust->EnableUserDefinedToolbars();
//	pDlgCust->Create();
//}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_DockPane_RunningLog.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


//可以在重载LoadFrame()函数里面修改MainFrame的样式   chendelin  2019.5.11
BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作
//	dwDefaultStyle |= WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
//		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
	//dwDefaultStyle &= ~WS_MINIMIZEBOX;

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_DockPane_RunningLog.UpdateFonts();
}


//主框架窗口关闭时会调用该函数
void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();

	if (theApp.GetRunningState() == 1)
	{
		AfxMessageBox("EverStationII程序仍在运行，强行关闭会破坏系统!");
	}
	else
	{
		CMDIFrameWndEx::OnClose();
	}
}

//外部发送给主框架窗口的日志消息
LRESULT CMainFrame::OnAppendLog(WPARAM wParam, LPARAM lParam)
{
	m_DockPane_RunningLog.AppendLogForUserAction((int)lParam, (char*)wParam);

	return NULL;
}


CMFCStatusBar& CMainFrame::GetStatusBar()
{
	return m_wndStatusBar;
}

//BOOL CALLBACK CMainFrame::MyInfoEnumProc(
//	HMONITOR hMonitor,
//	HDC hdcMonitor,
//	LPRECT lprcMonitor,
//	LPARAM dwData
//)
//{
//	BOOL rtcode = TRUE;
//
//	MONITORINFOEX mi;
//	ZeroMemory(&mi, sizeof(mi));
//	mi.cbSize = sizeof(mi);
//	GetMonitorInfo(hMonitor, &mi);
//	//wprintf(L"DisplayDevice: %s\n", mi.szDevice);
//
//	LOG(INFO) << "CALLBACK: MONITOR->" << mi.szDevice;
//
//	char pszDebug[256];
//	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
//	HWND hMainWnd = pMainWnd->GetSafeHwnd();
//
//	//sprintf_s(pszDebug, sizeof(pszDebug), "lprcMonitor: (%d,%d,%d,%d)\n", lprcMonitor->left, lprcMonitor->top, lprcMonitor->right, lprcMonitor->bottom);
//	//::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
//
//	sprintf_s(pszDebug, sizeof(pszDebug), "CALLBACK: MONITOR->:'%s'\n", mi.szDevice);
//	::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	sprintf_s(pszDebug, sizeof(pszDebug), "rcMonitor: (%d,%d,%d,%d)\n", mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
//	::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	if (mi.dwFlags & MONITORINFOF_PRIMARY)
//	{
//		sprintf_s(pszDebug, sizeof(pszDebug), "CALLBACK: MONITOR->:primary desktop\n");
//		::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//		m_rectPrimaryDesktop.left = mi.rcMonitor.left;
//		m_rectPrimaryDesktop.top = mi.rcMonitor.top;
//		m_rectPrimaryDesktop.right = mi.rcMonitor.right;
//		m_rectPrimaryDesktop.bottom = mi.rcMonitor.bottom;
//	}
//	else
//	{
//		sprintf_s(pszDebug, sizeof(pszDebug), "CALLBACK: MONITOR->:extended desktop\n");
//		::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//		m_rectSecondDesktop.left = mi.rcMonitor.left;
//		m_rectSecondDesktop.top = mi.rcMonitor.top;
//		m_rectSecondDesktop.right = mi.rcMonitor.right;
//		m_rectSecondDesktop.bottom = mi.rcMonitor.bottom;
//	}
//
//	return rtcode;
//}

BOOL CMainFrame::CheckMonitorInfo(HWND hNotifiedWnd, RECT* prcPrimaryDesktop, RECT* prcSecondDesktop)
{
	char pszDebug[256];

	//the first method to obtain monitors
	sprintf_s(pszDebug, sizeof(pszDebug), "checking monitors...\n");
	::SendMessage(hNotifiedWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
	LOG(INFO) << pszDebug;

	DISPLAY_DEVICE dd;//声明一个该结构体变量
	ZeroMemory(&dd, sizeof(dd));//msdn文档中要求，在填充该结构体前，要
								//先初始化DisplayDevice.cb,所以，意味着要给该结构体预先分配内存，把结构体中所有成员变量都清0
								//也就是满足了DisplayDevice.cb初始化的工作。
	dd.cb = sizeof(dd);//该结构体需要的空间

	BOOL bSuccess = true;
	DWORD dwAdaptor = 0;
	do
	{
		bSuccess = EnumDisplayDevices(NULL, dwAdaptor, &dd, 0);

		if (bSuccess)
		{
			sprintf_s(pszDebug, sizeof(pszDebug), "1st method: Display Adaptor[%d]->DeviceName:'%s', DeviceString: '%s'\n", dwAdaptor, dd.DeviceName, dd.DeviceString);
			LOG(INFO) << pszDebug;
			//::SendMessage(hNotifiedWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			sprintf_s(pszDebug, sizeof(pszDebug), "1st method: Display Adaptor: StateFlags:   %s%s%s%s%s\n",
				((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ? "desktop " : ""),
				((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "primary " : ""),
				((dd.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE) ? "vga " : ""),
				((dd.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER) ? "multi " : ""),
				((dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) ? "mirror " : ""));
			LOG(INFO) << pszDebug;
			//::SendMessage(hNotifiedWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			DEVMODE devMode;
			ZeroMemory(&devMode, sizeof(devMode));
			devMode.dmSize = sizeof(devMode);
			devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;

			bSuccess = EnumDisplaySettingsEx(dd.DeviceName, ENUM_CURRENT_SETTINGS, &devMode, EDS_RAWMODE);

			//sprintf_s(pszDebug, sizeof(pszDebug), "1st: DEVMODE: dmDisplayFlags:   %s%s%s%s%s\n",
			//	((devMode.dmDisplayFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ? "desktop " : ""),
			//	((devMode.dmDisplayFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "primary " : ""),
			//	((devMode.dmDisplayFlags & DISPLAY_DEVICE_VGA_COMPATIBLE) ? "vga " : ""),
			//	((devMode.dmDisplayFlags & DISPLAY_DEVICE_MULTI_DRIVER) ? "multi " : ""),
			//	((devMode.dmDisplayFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) ? "mirror " : ""));
			//LOG(INFO) << pszDebug;
			//::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			//sprintf_s(pszDebug, sizeof(pszDebug), "1st method: DEVMODE: dmDeviceName='%s', w x h = %d x %d \n", devMode.dmDeviceName, devMode.dmPelsWidth, devMode.dmPelsHeight);
			sprintf_s(pszDebug, sizeof(pszDebug), "1st method: DEVMODE: dmDeviceName='%s', pos(%d, %d, %d, %d) \n", devMode.dmDeviceName, devMode.dmPosition.x, devMode.dmPosition.y, devMode.dmPosition.x + devMode.dmPelsWidth, devMode.dmPosition.y + devMode.dmPelsHeight);
			LOG(INFO) << pszDebug;
			//::SendMessage(hNotifiedWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			{
				if ((devMode.dmPelsWidth > 0) && (devMode.dmPelsHeight > 0))
				{
					if ((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == DISPLAY_DEVICE_PRIMARY_DEVICE)
					{
						sprintf_s(pszDebug, sizeof(pszDebug), "-->primary monitor: DeviceName='%s', pos=(%d, %d, %d, %d) \n", devMode.dmDeviceName, devMode.dmPosition.x, devMode.dmPosition.y, devMode.dmPosition.x + devMode.dmPelsWidth, devMode.dmPosition.y + devMode.dmPelsHeight);
						LOG(INFO) << pszDebug;
						::SendMessage(hNotifiedWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

						if (prcPrimaryDesktop != NULL)
						{
							prcPrimaryDesktop->left = devMode.dmPosition.x;
							prcPrimaryDesktop->top = devMode.dmPosition.y;
							prcPrimaryDesktop->right = devMode.dmPosition.x + devMode.dmPelsWidth;
							prcPrimaryDesktop->bottom = devMode.dmPosition.y + devMode.dmPelsHeight;
						}
					}
					else
					{
						sprintf_s(pszDebug, sizeof(pszDebug), "-->second monitor: DeviceName='%s', pos=(%d, %d, %d, %d) \n", devMode.dmDeviceName, devMode.dmPosition.x, devMode.dmPosition.y, devMode.dmPosition.x + devMode.dmPelsWidth, devMode.dmPosition.y + devMode.dmPelsHeight);
						LOG(INFO) << pszDebug;
						::SendMessage(hNotifiedWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

						//if the monitor's count more than 2, which one do we choice?
						if (prcSecondDesktop != NULL)
						{
							prcSecondDesktop->left = devMode.dmPosition.x;
							prcSecondDesktop->top = devMode.dmPosition.y;
							prcSecondDesktop->right = devMode.dmPosition.x + devMode.dmPelsWidth;
							prcSecondDesktop->bottom = devMode.dmPosition.y + devMode.dmPelsHeight;
						}

					}
				}
			}
			//else
			//{
			//	assert(0);
			//}

			// Get more info about the device
			//DISPLAY_DEVICE dd2;
			//ZeroMemory(&dd2, sizeof(dd2));
			//dd2.cb = sizeof(dd2);

			//DWORD monitor;
			//for (monitor = 0; EnumDisplayDevices(dd.DeviceName, monitor, &dd2, 0); monitor++)
			//{
			//	EnumDisplayDevices(dd.DeviceName, 0, &dd2, 0);
			//	sprintf_s(pszDebug, sizeof(pszDebug), "1st method: Monitor %d: DeviceID: '%s'\n", monitor, dd2.DeviceID);
			//	LOG(INFO) << pszDebug;
			//	::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			//	sprintf_s(pszDebug, sizeof(pszDebug), "1st method: Monitor: Monitor Name: '%s'\n", dd2.DeviceString);
			//	LOG(INFO) << pszDebug;
			//	::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			//	sprintf_s(pszDebug, sizeof(pszDebug), "1st method: Monitor: StateFlags:   %s%s%s%s%s\n",
			//		((dd2.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ? "desktop " : ""),
			//		((dd2.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "primary " : ""),
			//		((dd2.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE) ? "vga " : ""),
			//		((dd2.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER) ? "multi " : ""),
			//		((dd2.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) ? "mirror " : ""));
			//	LOG(INFO) << pszDebug;
			//	::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			//	if ((dd2.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0)
			//	{
			//		HDC hdc = CreateDC(dd.DeviceName, NULL, NULL, &devMode);
			//	}
			//}

			dwAdaptor++;
		}

	} while (bSuccess);

	//the second method to obtain monitors
	//sprintf_s(pszDebug, sizeof(pszDebug), "2nd method: EnumDisplayMonitors():->");
	//LOG(INFO) << pszDebug;
	//::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
	//EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);

	//the third method to obtain monitors
	//int numMonitor = GetSystemMetrics(SM_CMONITORS);
	//sprintf_s(pszDebug, sizeof(pszDebug), "3rd method: GetSystemMetrics(): 显示器数量 %d个\n", numMonitor);
	//LOG(INFO) << pszDebug;
	//::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

	//int nScreenWidth, nScreenHeight;
	//nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	//nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	//sprintf_s(pszDebug, sizeof(pszDebug), "3rd method: GetSystemMetrics(): 当前屏幕分辨率: %d x %d \n", nScreenWidth, nScreenHeight);
	//LOG(INFO) << pszDebug;
	//::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

	//nScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	//nScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	//sprintf_s(pszDebug, sizeof(pszDebug), "3rd method: GetSystemMetrics(): 虚拟屏幕分辨率: %d x %d \n", nScreenWidth, nScreenHeight);
	//LOG(INFO) << pszDebug;
	//::SendMessage(hMainWnd, WM_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

	return bSuccess;
}


//void CMainFrame::OnPaint()
//{
//	//CPaintDC dc(this); // device context for painting
//					   // TODO: 在此处添加消息处理程序代码
//					   // 不为绘图消息调用 CMDIFrameWndEx::OnPaint()
//
//	//CDC dc, memdc;
//	//dc.m_hDC = ::GetDC(this->m_hWndMDIClient);
//	//CRect rect;
//	//CBitmap bitmap;
//	//BITMAP szbitmap;
//	//bitmap.LoadBitmap(IDB_DIGITAL_FRONTEND);
//	//bitmap.GetObject(sizeof(BITMAP), &szbitmap);
//	//CSize size(szbitmap.bmWidth, szbitmap.bmHeight);
//	//memdc.CreateCompatibleDC(&dc);
//	//CBitmap* oldbitmap = memdc.SelectObject(&bitmap);
//	//GetClientRect(&rect);
//	//dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, size.cx, size.cy, SRCCOPY);
//	//memdc.SelectObject(oldbitmap);
//	//memdc.DeleteDC();
//	//dc.DeleteDC();
//}


//BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	if ((pMsg->hwnd == m_hWndMDIClient) && (pMsg->message == WM_PAINT))
//	{
//		PostMessage(WM_PAINT);
//	}
//
//	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
//}
