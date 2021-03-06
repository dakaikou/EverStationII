
// EverStationII.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "EverStationII.h"
#include "MainFrm.h"
#include "AboutDlg.h"

#include "ChildFrm.h"

#include "./Magic_TS/TSMagicView.h"
#include "./Magic_TS/TSMagicDoc.h"
#include "./Magic_PS/PSMagicView.h"
#include "./Magic_PS/PSMagicDoc.h"
#include "./Magic_ES/ESMagicView.h"
#include "./Magic_ES/ESMagicDoc.h"
#include "./Magic_YUV/YUVMagicView.h"
#include "./Magic_YUV/YUVMagicDoc.h"

#include "./DlgConfigCoderDebug.h"
#include "./DlgConfigWinpcap.h"
#include "./DlgConfigTSInput.h"

#include "Utilities\Directory\Include\TOOL_Directory.h"

#include "thirdparty_libs\glog\glog\logging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEverStationIIApp

BEGIN_MESSAGE_MAP(CEverStationIIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CEverStationIIApp::OnAppAbout)
	// 基于文件的标准文档命令
	//ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	//ON_COMMAND(ID_FILE_SAVE, &CEverStationIIApp::OnFileSave)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(IDC_BTN_TSMAGIC, &CEverStationIIApp::OnBtnTSMagic)
	ON_COMMAND(IDC_BTN_PSMAGIC, &CEverStationIIApp::OnBtnPSMagic)
	ON_COMMAND(IDC_BTN_YUVMAGIC, &CEverStationIIApp::OnBtnYUVMagic)
	ON_COMMAND(IDC_BTN_ESMAGIC, &CEverStationIIApp::OnBtnESMagic)
	ON_COMMAND(ID_MENU_SETUP_SYS_DEBUG, &CEverStationIIApp::OnMenuSetupCoderDebug)
	ON_COMMAND(ID_MENU_SETUP_WINPCAP_DEBUG, &CEverStationIIApp::OnMenuSetupWinpcapDebug)
	ON_UPDATE_COMMAND_UI(ID_MENU_SETUP_SYS_DEBUG, &CEverStationIIApp::OnUpdateMenuSetupCoderDebug)
	ON_UPDATE_COMMAND_UI(ID_MENU_SETUP_WINPCAP_DEBUG, &CEverStationIIApp::OnUpdateMenuSetupWinpcapDebug)
	ON_COMMAND(ID_MENU_SETUP_TS_INPUT, &CEverStationIIApp::OnMenuSetupTsInput)
	ON_UPDATE_COMMAND_UI(ID_MENU_SETUP_TS_INPUT, &CEverStationIIApp::OnUpdateMenuSetupTsInput)
//	ON_COMMAND(ID_FILE_OPEN, &CEverStationIIApp::OnFileOpen)
END_MESSAGE_MAP()


// CEverStationIIApp 构造

CEverStationIIApp::CEverStationIIApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("EverStationII.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_bRuning = false;
	m_hMDIAccel = NULL;
	m_hMDIMenu = NULL;
	m_nAppLook = 0;

	m_pDocTemplateForTS = NULL;
	m_pDocTemplateForPS = NULL;
	m_pDocTemplateForES = NULL;
	m_pDocTemplateForYUV = NULL;
}

// 唯一的 CEverStationIIApp 对象

CEverStationIIApp theApp;

// CEverStationIIApp 初始化

BOOL CEverStationIIApp::InitInstance()
{
	char	pszExeFile[MAX_PATH];
	char	pszLogDir[MAX_PATH];
	//int		len;

	//GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	//char* ptemp = strrchr(pszExeFile, '\\');
	//len = (int)(ptemp - pszExeFile);

	//len = DIR_GetModulePathLength(pszExeFile);
	//assert(len > 0);

	//memcpy(pszLogDir, pszExeFile, len);
	//pszLogDir[len] = '\0';

	char	exeDrive[3];
	char	pszAppTempPath[MAX_PATH];
	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);

	//获取可执行文件所在的盘符
	exeDrive[0] = pszExeFile[0];		//C、D、E etc
	exeDrive[1] = pszExeFile[1];		//:
	exeDrive[2] = '\0';

	//构建应用程序运行数据主目录   disk://~EverStationII
	sprintf_s(pszAppTempPath, sizeof(pszAppTempPath), "%s\\~EverStationII", exeDrive);

	//构建日志文件存放目录
	sprintf_s(pszLogDir, sizeof(pszLogDir), "%s\\log\\", pszAppTempPath);
	DIR_BuildDirectory(pszLogDir);

	//初始化开源glog库
	//FLAGS_log_dir = pszLogDir;
	google::InitGoogleLogging("EverStationII");
	FLAGS_colorlogtostderr = true;
	google::SetLogDestination(google::GLOG_INFO, pszLogDir);

	LOG(INFO) << "CEverStationIIApp::InitInstance()";
	//LOG(WARNING) << "CEverStationIIApp::InitInstance(WARNING)";
	//LOG(ERROR) << "CEverStationIIApp::InitInstance(ERROR)";
	//LOG(FATAL) << "CEverStationIIApp::InitInstance(FATAL)";

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("码流分析工作站"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	//CMultiDocTemplate* pDocTemplate;
	//m_pDocTemplate = new CMultiDocTemplate(IDR_EverStationIITYPE,
	//	RUNTIME_CLASS(CEverStationIIDoc),
	//	RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
	//	RUNTIME_CLASS(CEverStationIIView));
	//if (!m_pDocTemplate)
	//	return FALSE;
	//AddDocTemplate(pDocTemplate);

	m_pDocTemplateForTS = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CTSMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CTSMagicView));

	m_pDocTemplateForPS = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CPSMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CPSMagicView));

	m_pDocTemplateForES = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CESMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CESMagicView));

	m_pDocTemplateForYUV = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CYUVMagicDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CYUVMagicView));

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 主窗口已初始化，因此显示它并对其进行更新
	//pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	//pMainFrame->UpdateWindow();

	//OnBtnYUVMagic();
	//OnBtnESMagic();
	//OnBtnTSMagic();
	//OnBtnPSMagic();

	//m_bRuning = 0;

	//sqlite3_test();

	return TRUE;
}

int CEverStationIIApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	if (m_pDocTemplateForTS != nullptr)
	{
		delete m_pDocTemplateForTS;
		m_pDocTemplateForTS = NULL;
	}
	if (m_pDocTemplateForPS != nullptr)
	{
		delete m_pDocTemplateForPS;
		m_pDocTemplateForPS = NULL;
	}
	if (m_pDocTemplateForES != nullptr)
	{
		delete m_pDocTemplateForES;
		m_pDocTemplateForES = NULL;
	}
	if (m_pDocTemplateForYUV != nullptr)
	{
		delete m_pDocTemplateForYUV;
		m_pDocTemplateForYUV = NULL;
	}

	//关于第三方开源glog库
	LOG(INFO) << "CEverStationIIApp::ExitInstance()";
	google::ShutdownGoogleLogging();

	return CWinAppEx::ExitInstance();
}

// CEverStationIIApp 消息处理程序

// 用于运行对话框的应用程序命令
void CEverStationIIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CEverStationIIApp 自定义加载/保存方法

void CEverStationIIApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CEverStationIIApp::LoadCustomState()
{
}

void CEverStationIIApp::SaveCustomState()
{
}

// CEverStationIIApp 消息处理程序



void CEverStationIIApp::OnBtnTSMagic()
{
	// TODO: 在此添加命令处理程序代码

	 //注册应用程序的文档模板。  
	 //文档模板将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForTS != nullptr)
	{
		POSITION docPos = m_pDocTemplateForTS->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForTS->OpenDocumentFile(NULL);
			pDoc->SetTitle("MPEG2/DVB TS流分析");
		}
		else
		{
			//不重复创建，只创建一个子框架/视图
		}
	}
}

void CEverStationIIApp::OnBtnYUVMagic()
{
	// TODO: Add your control notification handler code here

	//注册应用程序的文档模板。  文档模板
	//将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForYUV != nullptr)
	{
		POSITION docPos = m_pDocTemplateForYUV->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForYUV->OpenDocumentFile(NULL);
			pDoc->SetTitle("YUV序列分析");
		}
		else
		{
			//不重复创建，只创建一个子框架/视图
		}
	}
}

void CEverStationIIApp::OnBtnESMagic()
{
	// TODO: 在此添加命令处理程序代码

	//注册应用程序的文档模板。  文档模板
	//将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForES != nullptr)
	{
		POSITION docPos = m_pDocTemplateForES->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForES->OpenDocumentFile(NULL);
			pDoc->SetTitle("ES流分析");
		}
		else
		{
			//不重复创建，只创建一个子框架/视图
		}
	}
}

void CEverStationIIApp::OnBtnPSMagic()
{
	// TODO: 在此添加命令处理程序代码

	//注册应用程序的文档模板。  文档模板
	//将用作文档、框架窗口和视图之间的连接
	if (m_pDocTemplateForPS != nullptr)
	{
		POSITION docPos = m_pDocTemplateForPS->GetFirstDocPosition();
		if (docPos == NULL)
		{
			CDocument* pDoc = m_pDocTemplateForPS->OpenDocumentFile(NULL);
			pDoc->SetTitle("PS流分析");
		}
		else
		{
			//不重复创建，只创建一个子框架/视图
		}
	}
}

int	CEverStationIIApp::GetRunningState()
{
	return m_bRuning;
}

int CEverStationIIApp::SetRunningState(int state)
{
	m_bRuning = state;
	return m_bRuning;
}

void CEverStationIIApp::OnMenuSetupCoderDebug()
{
	// TODO: 在此添加命令处理程序代码
#ifdef _DEBUG
	CDlgConfigCoderDebug dlgSysDebug;
	if (dlgSysDebug.DoModal() == IDOK)
	{

	}
#endif // 
}


void CEverStationIIApp::OnMenuSetupWinpcapDebug()
{
	// TODO: 在此添加命令处理程序代码
	CDlgConfigWinpcap dlgWinpcap;
	if (dlgWinpcap.DoModal() == IDOK)
	{

	}
}


void CEverStationIIApp::OnMenuSetupTsInput()
{
	// TODO: 在此添加命令处理程序代码
	CDlgConfigTSInput dlgTSInput;
	if (dlgTSInput.DoModal() == IDOK)
	{

	}
}

void CEverStationIIApp::OnUpdateMenuSetupCoderDebug(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
#ifdef _DEBUG
	if (m_bRuning)
	{
		pCmdUI->Enable(false);
	}
#else
	pCmdUI->Enable(false);
#endif
}


void CEverStationIIApp::OnUpdateMenuSetupWinpcapDebug(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_bRuning)
	{
		pCmdUI->Enable(false);
	}
}

void CEverStationIIApp::OnUpdateMenuSetupTsInput(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_bRuning)
	{
		pCmdUI->Enable(false);
	}
}

//void CEverStationIIApp::OnFileOpen()
//{
//	// TODO: 在此添加命令处理程序代码
//	//可以实现成根据文件类型打开响应的框架
//}
