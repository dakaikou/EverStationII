// TSMagicView.cpp : implementation file
//

#include "stdafx.h"

#include <assert.h>

#include "..\EverStationII.h"
#include "..\MainFrm.h"
#include "TSMagicView.h"
#include "TSMagicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTSMagicView
//#include "TSMagic_GuiApi.h"
#include "TSMagic_AnalyseThread_MSG.h"
#include "TSMagic_Callbacks_To_Gui.h"
#include "TSMagic_Trigger_TSPacket.h"
#include "TSMagic_Trigger_Section.h"

#include "..\Common\define.h"

#include "..\resource.h"
#include "TSMagicview.h"

#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "Utilities\Directory\Include\TOOL_Directory.h"

IMPLEMENT_DYNCREATE(CTSMagicView, CFormView)

CTSMagicView::CTSMagicView()
	: CFormView(CTSMagicView::IDD)
{
	//{{AFX_DATA_INIT(CTSMagicView)
	//}}AFX_DATA_INIT

	m_bInitDone = 0;

	m_DB_TSPackets.callback_gui_update = REPORT_TS_packet_statistic;

	m_DB_TSPackets.Reset();
	m_DB_Pcrs.Reset();
	m_DB_PsiSiObjs.Reset();
	m_DB_OCDCs.Reset();

	m_Trigger_Section.Reset();
	m_Trigger_TSPacket.Reset();
	m_Trigger_PESPacket.Reset();

	TAB_OVERVIEW = -1;
	TAB_SERVICES = -1;
	TAB_EPG = -1;
	TAB_PCR = -1;
	TAB_PACKET = -1;
	TAB_PK_TRIGGER = -1;
	TAB_PK_MODIFY = -1;
	TAB_SECTION = -1;
	TAB_SEC_TRIGGER = -1;
	TAB_BOUQUET = -1;
	TAB_PESES = -1;
	TAB_ES = -1;
	TAB_WATCH_TELETEXT = -1;
	TAB_WATCH_SUBTITLE = -1;
	TAB_WATCH_OCDC = -1;
	m_nCurActiveTab = -1;

	//m_lastClientSize.cx = 0;
	//m_lastClientSize.cy = 0;
}

CTSMagicView::~CTSMagicView()
{
}

void CTSMagicView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTSMagicView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTSMagicView, CFormView)
	//{{AFX_MSG_MAP(CTSMagicView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_TSFILE_OPEN_CLOSE, OnBtnFileOpenOrClose)
	ON_BN_CLICKED(IDC_BTN_TSTREAM_OPEN_CLOSE, OnBtnStreamOpenOrClose)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_TSMAGIC, OnSelchangeTabConsole)
	//ON_COMMAND(IDC_TOOL_PCR_DEBUG, OnToolPcrDebug)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TSMAGIC_OFFLINE_THREAD, OnTSMagicOfflineThreadMsg)
	ON_MESSAGE(WM_TSMAGIC_REALTIME_THREAD, OnTSMagicRealtimeThreadMsg)
	ON_MESSAGE(WM_TSMAGIC_DOWNLOAD_THREAD, OnTSMagicDownloadThreadMsg)
	ON_MESSAGE(WM_TSMAGIC_DECIMATE_THREAD, OnTSMagicDecimateThreadMsg)
	ON_MESSAGE(WM_TSMAGIC_RECORD_THREAD, OnTSMagicRecordThreadMsg)

	ON_MESSAGE(WM_TSMAGIC_APPEND_LOG, OnTSAppendLog)
	ON_MESSAGE(WM_TSMAGIC_ETR290_LOG, OnTSEtr290Log)
	ON_MESSAGE(WM_TSMAGIC_REPORT_RATIO, OnTSReportRatio)
	ON_MESSAGE(WM_TSMAGIC_REPORT_FILESIZE, OnTSReportFileSize)
	ON_MESSAGE(WM_TSMAGIC_DOWNLOAD_LOG, OnTSDownloadLog)
	ON_MESSAGE(WM_TSMAGIC_DOWNLOAD_RATIO, OnTSDownloadRatio)
	ON_MESSAGE(WM_TSMAGIC_DECIMATE_LOG, OnTSDecimateLog)
	ON_MESSAGE(WM_TSMAGIC_DECIMATE_RATIO, OnTSDecimateRatio)
	ON_MESSAGE(WM_TSMAGIC_RECORD_LOG, OnTSRecordLog)
	ON_MESSAGE(WM_TSMAGIC_RECORD_RATIO, OnTSRecordRatio)

//	ON_MESSAGE(WM_TSMAGIC_REPORT_BITRATE, OnTSReportBitrate)
//	ON_MESSAGE(WM_TSMAGIC_REPORT_BITRATE_MAP, OnTSReportBitrateMap)
	ON_MESSAGE(WM_TSMAGIC_REPORT_PACKET_LENGTH, OnTSReportPacketLength)

	ON_MESSAGE(WM_TSMAGIC_REPORT_SIGNAL_STATUS, OnTSReportSignalStatus)

	ON_MESSAGE(WM_TSMAGIC_TS_TRIGGER_STATE, OnReportTSTriggerStatus)
	ON_MESSAGE(WM_TSMAGIC_PES_TRIGGER_STATE, OnReportPESTriggerStatus)
	//ON_MESSAGE(WM_TSMAGIC_ES_TRIGGER_STATE, OnReportESTriggerStatus)
	ON_MESSAGE(WM_TSMAGIC_SECTION_TRIGGER_STATE, OnReportSectionTriggerStatus)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTSMagicView diagnostics

#ifdef _DEBUG
void CTSMagicView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTSMagicView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTSMagicView message handlers

BOOL CTSMagicView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CTSMagicView::AdjustLayout(int cx, int cy)
{
	CWnd*	pWnd;
	CRect	rect;
	//int		xoffset, yalign;
	int		btn_width, btn_height;

	pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		btn_width = rect.Width();
		btn_height = rect.Height();

		pWnd->SetWindowPos(NULL, cx - btn_width, cy - 10 - 2 * btn_height, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, cx - btn_width, cy - 5 - btn_height, rect.Width(), rect.Height(), 0);
	}

	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_TSMAGIC);
	if (pTabCtrl->GetSafeHwnd() != NULL)
	{
		CRect		rectClient;

		pTabCtrl->SetWindowPos(NULL, 5, 5, cx - btn_width - 15, cy - 10, 0);
		pTabCtrl->GetClientRect(&rectClient);

		CRect		rectTabWorkArea;
		CRect		rectTabTitle;

		pTabCtrl->GetItemRect(0, &rectTabTitle);

		rectTabWorkArea.top = rectClient.top + rectTabTitle.bottom + 5;
		rectTabWorkArea.bottom = rectClient.bottom - 1;
		rectTabWorkArea.left = rectClient.left + 1;
		rectTabWorkArea.right = rectClient.right - 1;

#if GUI_TS_ANALYZER_PACKETS
		m_dlgTSAnalyzerPackets.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_PK_TRIGGER
		m_dlgPacketTrigger.MoveWindow(&rectTabWorkArea);
#endif
//#if GUI_TS_PK_MODIFY
//		m_dlgPacketModify.MoveWindow(&rectTab);
//#endif
#if GUI_TS_ANALYZER_PSISI
		//CRect rectTemp;
		//rectTemp.top = rectTab.top + 10;
		//rectTemp.left = rectTab.left + 10;
		//rectTemp.right = rectTab.right;
		//rectTemp.bottom = rectTab.bottom;

		m_dlgTSAnalyzerPsiSi.MoveWindow(&rectTabWorkArea);
		//m_tabItem_PsiSiAnalyzer.MoveWindow(rectTemp);
#endif
//#if GUI_TS_SEC_TRIGGER
//		m_dlgSectionTrigger.MoveWindow(&rectTabWorkArea);
//#endif
#if GUI_TS_ANALYZER_BOUQUETS
		m_dlgTSAnalyzerBouquets.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_EPG
		m_dlgTSAnalyzerEpg.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_OVERVIEW
		m_dlgTSAnalyzerOverview.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_NETWORK
		m_dlgTSAnalyzerServices.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_PCR
		m_dlgTSAnalyzerPcr.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_PESES
		m_dlgTSAnalyzerPesEs.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_ES
		m_dlgTSAnalyzerEs.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_TELETEXT
		m_dlgTSAnalyzerTeletext.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_SUBTITLE
		m_dlgTSAnalyzerSubtitle.MoveWindow(&rectTabWorkArea);
#endif
#if GUI_TS_ANALYZER_OCDC
		m_dlgTSAnalyzerDsmcc.MoveWindow(&rectTabWorkArea);
#endif

	}

}

void CTSMagicView::LoadInterfaceCfg()
{
	char	pszExeFile[MAX_PATH];
	char	pszIniFile[MAX_PATH];
	int		len;
	char	url[MAX_PATH];

	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH); 
	char* ptemp = strrchr(pszExeFile, '\\');
	len = (int)(ptemp - pszExeFile);

	//len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len]= '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile); 

	char	pszTSFile[MAX_PATH];
	char	pszLastSelected[64];
	char	pszUDPIP[64];
	char	pszUDPPort[8];

	GetPrivateProfileStringA("TS流输入", "最近选择", "Dektec", pszLastSelected, sizeof(pszLastSelected), pszIniFile);
	GetPrivateProfileStringA("TS流输入", "文件模拟", "e:\\java.ts", pszTSFile, sizeof(pszTSFile), pszIniFile);
	GetPrivateProfileStringA("TS流输入", "UDP IP", "224.0.2.111", pszUDPIP, sizeof(pszUDPIP), pszIniFile);
	GetPrivateProfileStringA("TS流输入", "UDP 端口", "1234", pszUDPPort, sizeof(pszUDPPort), pszIniFile);

	if (strcmp(pszLastSelected, "文件模拟") == 0)
	{
		m_strTSInputOption = "FILE";
		m_strTSInputAttribute = pszTSFile;
	}
	else if (strcmp(pszLastSelected, "Dektec") == 0)
	{
		m_strTSInputOption = "ASI";
		m_strTSInputAttribute = "Dektec";
	}
	else if (strcmp(pszLastSelected, "SmartTS") == 0)
	{
		m_strTSInputOption = "ASI";
		m_strTSInputAttribute = "SmartTS";
	}
	else if (strcmp(pszLastSelected, "UDP") == 0)
	{
		m_strTSInputOption = "UDP";
		sprintf_s(url, sizeof(url), "%s:%s", pszUDPIP, pszUDPPort);
		m_strTSInputAttribute = url;
	}
}

//int DIR_BuildDirectory(CString  strPath);

void CTSMagicView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	CTSMagicDoc* pDoc = (CTSMagicDoc*)GetDocument();

	// TODO: Add your specialized code here and/or call the base class
	pDoc->m_strDebug = "宝塔镇河妖";		//仅仅是为了程序测试用

	CRect	rect;
	CWnd*	pWnd;

	InitConsoleTab();
	
	//pWnd = GetDlgItem(IDC_PROGRESS_FILE);
	//pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
	pWnd->SetWindowText("离线分析");
	pWnd->EnableWindow(TRUE);

	pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
	pWnd->SetWindowText("实时分析");
	pWnd->EnableWindow(TRUE);

	m_dlgTSTriggerWaiting.Create(IDD_TRIGGER_WAITING, this);
	m_dlgTSTriggerWaiting.SetWindowText(_T("TS捕捉"));
	m_dlgTSTriggerWaiting.ShowWindow(SW_HIDE);

	m_dlgPESTriggerWaiting.Create(IDD_TRIGGER_WAITING, this);
	m_dlgPESTriggerWaiting.SetWindowText(_T("PES捕捉"));
	m_dlgPESTriggerWaiting.ShowWindow(SW_HIDE);

	//m_dlgESTriggerWaiting.Create(IDD_TRIGGER_WAITING, this);
	//m_dlgESTriggerWaiting.SetWindowText(_T("ES捕捉"));
	//m_dlgESTriggerWaiting.ShowWindow(SW_HIDE);

	m_dlgSectionTriggerWaiting.Create(IDD_TRIGGER_WAITING, this);
	m_dlgSectionTriggerWaiting.SetWindowText(_T("section捕捉"));
	m_dlgSectionTriggerWaiting.ShowWindow(SW_HIDE);

	m_dlgDsmccDownloadWaiting.Create(IDD_TRIGGER_WAITING, this);
	m_dlgDsmccDownloadWaiting.SetWindowText(_T("DSM-CC下载"));
	m_dlgDsmccDownloadWaiting.ShowWindow(SW_HIDE);

	m_dlgTSDecimateProgress.Create(IDD_ANALYSE_PROGRESS, this);
	m_dlgTSDecimateProgress.SetWindowText(_T("TS录制"));
	m_dlgTSDecimateProgress.ShowWindow(SW_HIDE);

	m_dlgSignalStatus.Create(IDD_SIGNAL_STATUS, this);
	m_dlgSignalStatus.ShowWindow(SW_HIDE);

	char	pszExeFile[MAX_PATH];
	char	exeDrive[3];
	char	pszDownloadRootPath[MAX_PATH];
	GetModuleFileName(NULL, pszExeFile, MAX_PATH);
	exeDrive[0] = pszExeFile[0];
	exeDrive[1] = pszExeFile[1];
	exeDrive[2] = '\0';

	sprintf_s(pszDownloadRootPath, sizeof(pszDownloadRootPath), "%s\\~EverStationII\\download_TS", exeDrive);
	DIR_BuildDirectory(pszDownloadRootPath);

	strcpy_s(m_kThreadParams.pszDecimatePath, sizeof(m_kThreadParams.pszDecimatePath), pszDownloadRootPath);

	GetClientRect(&rect);

	CSize size;
	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;
	SetScrollSizes(MM_HIMETRIC, size); // 将CScrollView的大小设置为当前客户区大小，禁止出现滚动条

	AdjustLayout(rect.Width(), rect.Height());

	InitThreadparams();

	m_bInitDone = 1;
}

void CTSMagicView::InitConsoleTab(void)
{
	int i;
	int	item;
	CTabCtrl*	 pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_TSMAGIC);

	m_imageTab.Create(IDB_CONSOLE_TS_MAGIC, 20, 1, RGB(0xab,0xcd,0xef));

	pTabCtrl->SetImageList(&m_imageTab);
//	pTabCtrl->SetMinTabWidth(80);
	pTabCtrl->SetItemSize(CSize(200, 24));

#if GUI_TS_ANALYZER_OVERVIEW
	item = pTabCtrl->GetItemCount();
	TAB_OVERVIEW = item;
	pTabCtrl->InsertItem(item, "TS流监测", 0);
#endif

#if GUI_TS_ANALYZER_PCR
	item = pTabCtrl->GetItemCount();
	TAB_PCR = item;
	pTabCtrl->InsertItem(item, "PCR监测", 5);
#endif

#if GUI_TS_ANALYZER_PACKETS
	item = pTabCtrl->GetItemCount();
	TAB_PACKET = item;
	pTabCtrl->InsertItem(item, "TS包监测", 4);
#endif

#if GUI_TS_ANALYZER_NETWORK
	item = pTabCtrl->GetItemCount();
	TAB_SERVICES = item;
	pTabCtrl->InsertItem(item, "网络分析", 1);
#endif

#if GUI_TS_ANALYZER_BOUQUETS
	item = pTabCtrl->GetItemCount();
	TAB_BOUQUET = item;
	pTabCtrl->InsertItem(item, "业务群分析", 0);
#endif

#if GUI_TS_ANALYZER_EPG
	item = pTabCtrl->GetItemCount();
	TAB_EPG = item;
	pTabCtrl->InsertItem(item, "EPG分析", 2);
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
	item = pTabCtrl->GetItemCount();
	TAB_PK_TRIGGER = item;
	pTabCtrl->InsertItem(item, "TS包语法分析", 6);
#endif

//#if GUI_TS_PK_MODIFY
//	item = pTabCtrl->GetItemCount();
//	TAB_PK_MODIFY = item;
//	pTabCtrl->InsertItem(item, "包修改", 6);
//#endif

#if GUI_TS_ANALYZER_PSISI
	item = pTabCtrl->GetItemCount();
	TAB_SECTION = item;
	pTabCtrl->InsertItem(item, "PSI/SI语法分析", 3);
#endif

#if GUI_TS_ANALYZER_PESES
	item = pTabCtrl->GetItemCount();
	TAB_PESES = item;
	pTabCtrl->InsertItem(item, "PES/ES语法分析", 2);
#endif

#if GUI_TS_ANALYZER_ES
	item = pTabCtrl->GetItemCount();
	TAB_ES = item;
	pTabCtrl->InsertItem(item, "ES解析", 2);
#endif

#if GUI_TS_ANALYZER_OCDC
	item = pTabCtrl->GetItemCount();
	TAB_WATCH_OCDC = item;
	pTabCtrl->InsertItem(item, "数据广播解析", 2);
#endif

#if GUI_TS_ANALYZER_TELETEXT
	item = pTabCtrl->GetItemCount();
	TAB_WATCH_TELETEXT = item;
	pTabCtrl->InsertItem(item, "图文解析", 2);
#endif

#if GUI_TS_ANALYZER_SUBTITLE
	item = pTabCtrl->GetItemCount();
	TAB_WATCH_SUBTITLE = item;
	pTabCtrl->InsertItem(item, "字幕解析", 2);
#endif

	for (i = 0; i < pTabCtrl->GetItemCount(); i++)
	{
		m_pdlgWnd[i] = NULL;
	}

	//同一时间创建这么多窗口，速度有点问题，留待以后优化。  chendelin  2019.5.12
#if GUI_TS_ANALYZER_PACKETS
	m_dlgTSAnalyzerPackets.Create(IDD_TS_ANALYZER_PACKETS, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerPackets.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_PACKET] = (CWnd*)&m_dlgTSAnalyzerPackets;
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
	m_dlgPacketTrigger.Create(IDD_TS_ANALYZER_PACKET_TRIGGER, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgPacketTrigger.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_PK_TRIGGER] = (CWnd*)&m_dlgPacketTrigger;
#endif

#if GUI_TS_ANALYZER_PSISI
	m_dlgTSAnalyzerPsiSi.Create(IDD_TS_ANALYZER_PSISI, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerPsiSi.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_SECTION] = (CWnd*)& m_dlgTSAnalyzerPsiSi;
#endif

#if GUI_TS_ANALYZER_BOUQUETS
	m_dlgTSAnalyzerBouquets.Create(IDD_TS_ANALYZER_BOUQUETS, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerBouquets.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_BOUQUET] = (CWnd*)&m_dlgTSAnalyzerBouquets;
#endif

#if GUI_TS_ANALYZER_EPG
	m_dlgTSAnalyzerEpg.Create(IDD_TS_ANALYZER_EPG, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerEpg.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_EPG] = (CWnd*)&m_dlgTSAnalyzerEpg;
#endif

#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.Create(IDD_TS_ANALYZER_OVERVIEW, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerOverview.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_OVERVIEW] = (CWnd*)&m_dlgTSAnalyzerOverview;
#endif

#if GUI_TS_ANALYZER_NETWORK
	m_dlgTSAnalyzerServices.Create(IDD_TS_ANALYZER_SERVICES, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerServices.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_SERVICES] = (CWnd*)&m_dlgTSAnalyzerServices;
#endif

#if GUI_TS_ANALYZER_PCR
	m_dlgTSAnalyzerPcr.Create(IDD_TS_ANALYZER_PCR, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerPcr.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_PCR] = (CWnd*)&m_dlgTSAnalyzerPcr;
#endif

#if GUI_TS_ANALYZER_PESES
	m_dlgTSAnalyzerPesEs.Create(IDD_TS_ANALYZER_PESES, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerPesEs.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_PESES] = (CWnd*)&m_dlgTSAnalyzerPesEs;
#endif

#if GUI_TS_ANALYZER_ES
	m_dlgTSAnalyzerEs.Create(IDD_TS_ANALYZER_ES, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerEs.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_ES] = (CWnd*)&m_dlgTSAnalyzerEs;
#endif

#if GUI_TS_ANALYZER_TELETEXT
	m_dlgTSAnalyzerTeletext.Create(IDD_TS_ANALYZER_TELETEXT, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerTeletext.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_WATCH_TELETEXT] = (CWnd*)&m_dlgTSAnalyzerTeletext;
#endif

#if GUI_TS_ANALYZER_SUBTITLE
	m_dlgTSAnalyzerSubtitle.Create(IDD_TS_ANALYZER_SUBTITLE, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerSubtitle.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_WATCH_SUBTITLE] = (CWnd*)&m_dlgTSAnalyzerSubtitle;
#endif

#if GUI_TS_ANALYZER_OCDC
	m_dlgTSAnalyzerDsmcc.Create(IDD_TS_ANALYZER_DSMCC, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgTSAnalyzerDsmcc.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_WATCH_OCDC] = (CWnd*)&m_dlgTSAnalyzerDsmcc;
#endif

#if GUI_TS_PK_MODIFY
	m_dlgPacketModify.Create(IDD_TS_PACKET_MODIFY, GetDlgItem(IDC_TAB_TSMAGIC));
	m_dlgPacketModify.ShowWindow(SW_HIDE);
	m_pdlgWnd[TAB_PK_MODIFY] = (CWnd*)&m_dlgPacketModify;
#endif

	pTabCtrl->SetCurSel(0);
	ActivateTabPage(-1, 0);

	m_nCurActiveTab = pTabCtrl->GetCurSel();
}

void CTSMagicView::ActivateTabPage(int nOldSel, int nNewSel)
{
	//int		  i;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_TSMAGIC);
	CWnd*	  pWnd;

	if (nOldSel >= 0)
	{
		pWnd = m_pdlgWnd[nOldSel];
		pWnd->ShowWindow(SW_HIDE);
	}
	if (nNewSel >= 0)
	{
		pWnd = m_pdlgWnd[nNewSel];
		pWnd->ShowWindow(SW_SHOW);
	}

	//for (i = 0; i < pTabCtrl->GetItemCount(); i++)
	//{
	//	pWnd = m_pdlgWnd[i];

	//	if (pWnd != NULL)
	//	{
	//		if (i == nSel)
	//		{
	//			pWnd->ShowWindow(SW_SHOW);
	//		}
	//		else
	//		{
	//			pWnd->ShowWindow(SW_HIDE);
	//		}
	//	}
	//}
}

void CTSMagicView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
	{
		return;		// arrangement not needed
	}

	if (m_bInitDone)
	{
		if ((cx > 0) && (cy > 0))
		{
			AdjustLayout(cx, cy);
		}
	}
}

void CTSMagicView::OnBtnFileOpenOrClose() 
{
	// TODO: Add your command handler code here
	int		rtcode = 0;
	CWnd*	pWnd = NULL;

	if (m_kThreadParams.main_thread_running == 0)
	{
		assert(m_kThreadParams.main_thread_stopped == 1);

		CFileDialog fileDlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Media files(*.*)|*.*|", NULL);

		CString strFileName = _T("");

		if (fileDlg.DoModal() == IDOK)
		{
			strFileName = fileDlg.GetPathName();

			if (FILE_CheckStreamType(m_kThreadParams.stream_option, strFileName.GetBuffer(256)))
			{
				strcpy_s(m_kThreadParams.pszPathHeader, sizeof(m_kThreadParams.pszPathHeader), "FILE");
				strcpy_s(m_kThreadParams.pszPathName, sizeof(m_kThreadParams.pszPathName), strFileName.GetBuffer(256));

				rtcode = m_transport_stream.Open(m_kThreadParams.pszPathHeader, m_kThreadParams.pszPathName, 0);
				if (rtcode == MIDDLEWARE_TS_NO_ERROR)
				{
					pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
					pWnd->EnableWindow(FALSE);

					pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
					pWnd->EnableWindow(FALSE);

					m_kThreadParams.offline = 1;

					m_kThreadParams.main_thread_running = 1;		//若通知线程运行

					assert(m_kThreadParams.monitor_thread_running == 0);
					assert(m_kThreadParams.monitor_thread_stopped == 1);

					assert(m_kThreadParams.packet_decimate_thread_running == 0);
					assert(m_kThreadParams.packet_decimate_thread_stopped == 1);

					assert(m_kThreadParams.dsmcc_download_thread_running == 0);
					assert(m_kThreadParams.dsmcc_download_thread_stopped == 1);

					assert(m_kThreadParams.ts_trigger_thread_running == 0);
					assert(m_kThreadParams.ts_trigger_thread_stopped == 1);

					assert(m_kThreadParams.pes_trigger_thread_running == 0);
					assert(m_kThreadParams.pes_trigger_thread_stopped == 1);

					assert(m_kThreadParams.section_trigger_thread_running == 0);
					assert(m_kThreadParams.section_trigger_thread_stopped == 1);

					m_kThreadParams.find_signal = 0;

					::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_AnalyseThread_offline_main, (LPVOID)& m_kThreadParams, 0, 0);
				}
				else
				{
					AfxMessageBox("<---打开文件失败,请检查文件是否存在!--->", MB_OK | MB_ICONSTOP);
				}
			}
			else
			{
				AfxMessageBox("<---文件类型不正确,请确认是否为TS文件!--->", MB_OK|MB_ICONSTOP);
			}
		}
	}
	else
	{
		m_kThreadParams.packet_decimate_thread_running = 0;			//通知抽选线程退出
		m_kThreadParams.dsmcc_download_thread_running = 0;			//通知DSMCC线程退出
		m_kThreadParams.ts_trigger_thread_running = 0;
		m_kThreadParams.pes_trigger_thread_running = 0;
		m_kThreadParams.section_trigger_thread_running = 0;
		m_kThreadParams.monitor_thread_running = 0;			//通知监控线程退出。离线分析实际上没有启动该线程

		m_kThreadParams.main_thread_running = 0;			//通知主线程退出

		pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
		pWnd->EnableWindow(FALSE);

		if (m_kThreadParams.main_thread_stopped == 1)
		{
			//若离线分析线程已经退出，此时需要补发一个消息，否则界面控件显示会不正常
			::SendMessage(this->GetSafeHwnd(), WM_TSMAGIC_OFFLINE_THREAD, 0, NULL);
		}
	}
}

void CTSMagicView::OnBtnStreamOpenOrClose() 
{
	int		rtcode = 0;
	CWnd*	pWnd = NULL;

	if (m_kThreadParams.main_thread_running == 0)
	{
		assert(m_kThreadParams.main_thread_stopped == 1);

		LoadInterfaceCfg();

		strcpy_s(m_kThreadParams.pszPathHeader, sizeof(m_kThreadParams.pszPathHeader), m_strTSInputOption.GetBuffer(6));
		strcpy_s(m_kThreadParams.pszPathName, sizeof(m_kThreadParams.pszPathName), m_strTSInputAttribute.GetBuffer(256));

		rtcode = m_transport_stream.Open(m_kThreadParams.pszPathHeader, m_kThreadParams.pszPathName, 1);
		if (rtcode == MIDDLEWARE_TS_NO_ERROR)
		{
			pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
			if (pWnd != NULL)
			{
				pWnd->EnableWindow(FALSE);
			}

			pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
			if (pWnd != NULL)
			{
				pWnd->EnableWindow(FALSE);
			}

			m_kThreadParams.offline = 0;
			m_kThreadParams.main_thread_running = 1;		//若线程启动，会将此值修改为1，可以据此判断主线程是否启动

			m_kThreadParams.monitor_thread_running = 1;
			assert(m_kThreadParams.monitor_thread_stopped == 1);

			assert(m_kThreadParams.packet_decimate_thread_running == 0);
			assert(m_kThreadParams.packet_decimate_thread_stopped == 1);

			assert(m_kThreadParams.dsmcc_download_thread_running == 0);
			assert(m_kThreadParams.dsmcc_download_thread_stopped == 1);

			assert(m_kThreadParams.ts_trigger_thread_running == 0);
			assert(m_kThreadParams.ts_trigger_thread_stopped == 1);

			assert(m_kThreadParams.pes_trigger_thread_running == 0);
			assert(m_kThreadParams.pes_trigger_thread_stopped == 1);

			assert(m_kThreadParams.section_trigger_thread_running == 0);
			assert(m_kThreadParams.section_trigger_thread_stopped == 1);

			::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_AnalyseThread_realtime_main, (LPVOID)& m_kThreadParams, 0, 0);
			::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_AnalyseThread_monitor, (LPVOID)& m_kThreadParams, 0, 0);


#if GUI_TS_ANALYZER_OVERVIEW
			m_dlgTSAnalyzerOverview.Set(0);
#endif

#if GUI_TS_ANALYZER_PACKETS
			m_dlgTSAnalyzerPackets.Set(0);
#endif

			//#if GUI_TS_ANALYZER_PESES
			//			m_dlgTSAnalyzerPesEs.Set(0);
			//#endif

#if GUI_TS_ANALYZER_ES
			m_dlgTSAnalyzerEs.Set(0);
#endif

#if GUI_TS_ANALYZER_TELETEXT
			m_dlgTSAnalyzerTeletext.Set();
#endif

			//#if GUI_TS_ANALYZER_SUBTITLE
			//			m_dlgTSAnalyzerSubtitle.Set();
			//#endif

#if GUI_TS_SEC_TRIGGER
			m_dlgSectionTrigger.Set(0);
#endif
		}
		else
		{
			AfxMessageBox("<---连接硬件失败,请检查外围设备连接!--->", MB_OK | MB_ICONSTOP);
		}
	}
	else
	{
		m_kThreadParams.packet_decimate_thread_running = 0;		//通知抽选线程退出
		m_kThreadParams.dsmcc_download_thread_running = 0;			//通知DSMCC线程退出

		m_kThreadParams.ts_trigger_thread_running = 0;
		m_kThreadParams.pes_trigger_thread_running = 0;
		m_kThreadParams.section_trigger_thread_running = 0;

		m_kThreadParams.monitor_thread_running = 0;			//设置为0，强迫实时监测程退出
		m_kThreadParams.main_thread_running = 0;			//设置为0，强迫主线程退出

		pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
		pWnd->EnableWindow(FALSE);
	}
}

void CTSMagicView::OnSelchangeTabConsole(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_TSMAGIC);
	assert(pNMHDR->hwndFrom == pTabCtrl->GetSafeHwnd());
	int nNewSel = pTabCtrl->GetCurSel();

	ActivateTabPage(m_nCurActiveTab, nNewSel);
	m_nCurActiveTab = nNewSel;
	
	*pResult = 0;
}

void CTSMagicView::GUIReset(void)
{
	GetStatusBar().SetPaneText(1, "");
	GetStatusBar().SetPaneProgress(2, 0);

#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.Reset();
#endif
#if GUI_TS_ANALYZER_PACKETS
	m_dlgTSAnalyzerPackets.Reset();
#endif
#if GUI_TS_ANALYZER_PK_TRIGGER
	m_dlgPacketTrigger.Reset();
#endif
#if GUI_TS_ANALYZER_BOUQUETS
	m_dlgTSAnalyzerBouquets.Reset();
#endif
#if GUI_TS_ANALYZER_PSISI
	m_dlgTSAnalyzerPsiSi.Reset();
	//m_tabItem_PsiSiAnalyzer.Reset();
#endif
#if GUI_TS_SEC_TRIGGER
	m_dlgSectionTrigger.Reset();
#endif
#if GUI_TS_ANALYZER_NETWORK
	m_dlgTSAnalyzerServices.Reset();
#endif
#if GUI_TS_ANALYZER_EPG
	m_dlgTSAnalyzerEpg.Reset();
#endif
#if GUI_TS_ANALYZER_PCR
	m_dlgTSAnalyzerPcr.Reset();
#endif
#if GUI_TS_ANALYZER_PESES
	m_dlgTSAnalyzerPesEs.Reset();
#endif
#if GUI_TS_ANALYZER_ES
	m_dlgTSAnalyzerEs.Reset();
#endif
#if GUI_TS_ANALYZER_TELETEXT
	m_dlgTSAnalyzerTeletext.Reset();
#endif
#if GUI_TS_ANALYZER_SUBTITLE
	m_dlgTSAnalyzerSubtitle.Reset();
#endif
#if GUI_TS_ANALYZER_OCDC
	//m_dlgTSAnalyzerOCDC.Reset();
	m_dlgTSAnalyzerDsmcc.Reset();
#endif

	m_dlgTSDecimateProgress.Reset();
	m_dlgTSDecimateProgress.ShowWindow(SW_HIDE);

	m_dlgSignalStatus.ShowWindow(SW_HIDE);

	m_dlgTSTriggerWaiting.ShowWindow(SW_HIDE);
	m_dlgPESTriggerWaiting.ShowWindow(SW_HIDE);
	//m_dlgESTriggerWaiting.ShowWindow(SW_HIDE);
	m_dlgSectionTriggerWaiting.ShowWindow(SW_HIDE);
	m_dlgDsmccDownloadWaiting.ShowWindow(SW_HIDE);

	m_DB_PsiSiObjs.Reset();
	m_DB_OCDCs.Reset();
	m_DB_TSPackets.Reset();
	m_DB_Pcrs.Reset();

	m_Trigger_Section.Reset();
	m_Trigger_PESPacket.Reset();
	m_Trigger_TSPacket.Reset();
}

//TS离线分析线程发送过来的消息处理函数
//wParam -- 1，线程正常进入
//       -- 2, 线程正常退出
//       -- 0，用户强制退出
LRESULT CTSMagicView::OnTSMagicOfflineThreadMsg(WPARAM wParam, LPARAM lParam)
{
	CWnd*			 pWnd;
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();

	if (wParam == 1)		//线程进入
	{
		pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("停止分析");
			pWnd->EnableWindow(TRUE);
		}

		GetStatusBar().SetPaneText(1, m_kThreadParams.pszPathName);

//#if GUI_TS_ANALYZER_PESES
//		m_dlgTSAnalyzerPesEs.Set(1);
//#endif

#if GUI_TS_ANALYZER_ES
		m_dlgTSAnalyzerEs.Set(1);
#endif

#if GUI_TS_ANALYZER_PCR
		m_dlgTSAnalyzerPcr.Set(1);
#endif

#if GUI_TS_ANALYZER_TELETEXT
		m_dlgTSAnalyzerTeletext.Set();
#endif

//#if GUI_TS_ANALYZER_SUBTITLE
//		m_dlgTSAnalyzerSubtitle.Set();
//#endif

#if GUI_TS_SEC_TRIGGER
		m_dlgSectionTrigger.Set(1);
#endif

#if GUI_TS_ANALYZER_OVERVIEW
		m_dlgTSAnalyzerOverview.Set(1);
#endif

		pApp->SetRunningState(1);
	}
	else if (wParam == 2)		//离线分析结束
	{
		pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("关闭文件");
		}

#if GUI_TS_ANALYZER_PACKETS
		m_dlgTSAnalyzerPackets.Set(1);
#endif
	}
	else					//用户强制线程退出
	{
		assert(m_kThreadParams.main_thread_stopped == 1);
		assert(m_kThreadParams.monitor_thread_stopped == 1);
		assert(m_kThreadParams.dsmcc_download_thread_stopped == 1);
		assert(m_kThreadParams.packet_decimate_thread_stopped == 1);
		assert(m_kThreadParams.ts_trigger_thread_stopped == 1);
		assert(m_kThreadParams.pes_trigger_thread_stopped == 1);
		assert(m_kThreadParams.section_trigger_thread_stopped == 1);

		pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("离线分析");
			pWnd->EnableWindow(TRUE);
		}

		pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->EnableWindow(TRUE);
		}

		m_transport_stream.Close();

		pApp->SetRunningState(0);

		GUIReset();
	}

	return 0;
}

LRESULT CTSMagicView::OnTSMagicDownloadThreadMsg(WPARAM wParam, LPARAM lParam)
{
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CWnd*			 pWnd;

	if (wParam == 1)		//线程进入
	{
		if (!m_dlgDsmccDownloadWaiting.IsWindowVisible())
		{
			pWnd = m_dlgDsmccDownloadWaiting.GetDlgItem(IDC_STATIC_TRIGGER_MASK);
			pWnd->ShowWindow(SW_HIDE);

			pWnd = m_dlgDsmccDownloadWaiting.GetDlgItem(IDC_STATIC_MASK);
			pWnd->ShowWindow(SW_HIDE);
	//		pWnd->SetWindowText(pszText[0]);

			pWnd = m_dlgDsmccDownloadWaiting.GetDlgItem(IDC_STATIC_TRIGGER_DATA);
			pWnd->ShowWindow(SW_HIDE);

			pWnd = m_dlgDsmccDownloadWaiting.GetDlgItem(IDC_STATIC_DATA);
	//		pWnd->SetWindowText(pszText[1]);
			pWnd->ShowWindow(SW_HIDE);

//			strcpy_s(pszDebug, sizeof(pszDebug), "DSM-CC 文件下载");
//			m_dlgDsmccDownloadWaiting.SetWindowText(pszDebug);
			m_dlgDsmccDownloadWaiting.ShowWindow(SW_SHOW);
//			m_dlgDsmccDownloadWaiting.m_progress.SetPos(0);
		}
	}
	else					//线程退出
	{
		if (m_dlgDsmccDownloadWaiting.IsWindowVisible())
		{
			m_dlgDsmccDownloadWaiting.ShowWindow(SW_HIDE);
//			m_dlgDsmccDownloadWaiting.m_progress.SetPos(0);
//			m_dlgDsmccDownloadWaiting.Reset();
		}

		if (m_kThreadParams.main_thread_running == 0)
		{
			::SendMessage(this->GetSafeHwnd(), WM_TSMAGIC_OFFLINE_THREAD, 0, NULL);
		}
	}

	return 0;
}

/**
 *  TS流抽选消息响应函数
 */
LRESULT CTSMagicView::OnTSMagicDecimateThreadMsg(WPARAM wParam, LPARAM lParam)
{
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	char			 pszDebug[256];

#if GUI_TS_ANALYZER_PACKETS
	m_dlgTSAnalyzerPackets.ActionMsg(wParam);
#endif

	if (wParam == 1)		//线程进入
	{
		if (!m_dlgTSDecimateProgress.IsWindowVisible())
		{
			strcpy_s(pszDebug, sizeof(pszDebug), "TS包分解");
			m_dlgTSDecimateProgress.SetWindowText(pszDebug);
			m_dlgTSDecimateProgress.ShowWindow(SW_SHOW);
			m_dlgTSDecimateProgress.m_progress.SetPos(0);
		}
	}
	else					//线程退出
	{
		if (m_dlgTSDecimateProgress.IsWindowVisible())
		{
			m_dlgTSDecimateProgress.ShowWindow(SW_HIDE);
//			m_dlgTSDecimateProgress.m_progress.SetPos(0);
			m_dlgTSDecimateProgress.Reset();
		}

		//如果抽选线程在主线程之后退出，必须增加下面的语句，否则界面显示不正确
		if (m_kThreadParams.main_thread_running == 0)
		{
			::SendMessage(this->GetSafeHwnd(), WM_TSMAGIC_OFFLINE_THREAD, 0, NULL);
		}
	}

	return 0;
}

LRESULT CTSMagicView::OnTSMagicRecordThreadMsg(WPARAM wParam, LPARAM lParam)
{
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	char			 pszDebug[256];

#if GUI_TS_ANALYZER_PACKETS
	m_dlgTSAnalyzerPackets.ActionMsg(wParam);
#endif

	if (wParam == 1)		//线程进入
	{
		if (!m_dlgTSDecimateProgress.IsWindowVisible())
		{
			strcpy_s(pszDebug, sizeof(pszDebug), "TS包录制");
			m_dlgTSDecimateProgress.SetWindowText(pszDebug);
			m_dlgTSDecimateProgress.ShowWindow(SW_SHOW);
			m_dlgTSDecimateProgress.m_progress.SetPos(0);
		}
	}
	else					//线程退出
	{
		if (m_dlgTSDecimateProgress.IsWindowVisible())
		{
			m_dlgTSDecimateProgress.ShowWindow(SW_HIDE);
			m_dlgTSDecimateProgress.Reset();
		}
	}

	return 0;
}


//TS实时分析线程发送过来的消息处理函数
//wParam -- 1，线程正常进入
//       -- 2, 线程正常退出（注：实时分析不可能存在该状态）
//       -- 0，用户强制退出
LRESULT CTSMagicView::OnTSMagicRealtimeThreadMsg(WPARAM wParam, LPARAM lParam)
{
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CWnd*			 pWnd;

	if (wParam == 1)		//线程进入
	{
		pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("停止分析");
			pWnd->EnableWindow(TRUE);
		}

		GetStatusBar().SetPaneText(1, m_kThreadParams.pszPathName);

#if GUI_TS_ANALYZER_PACKETS
		m_dlgTSAnalyzerPackets.Set(0);
#endif

#if GUI_TS_ANALYZER_PCR
		m_dlgTSAnalyzerPcr.Set(0);
#endif

		pApp->SetRunningState(1);
	}
	else					//用户强制线程退出
	{
		assert(m_kThreadParams.main_thread_stopped == 1);
		assert(m_kThreadParams.monitor_thread_stopped == 1);
		assert(m_kThreadParams.dsmcc_download_thread_stopped == 1);
		assert(m_kThreadParams.packet_decimate_thread_stopped == 1);
		assert(m_kThreadParams.ts_trigger_thread_stopped == 1);
		assert(m_kThreadParams.pes_trigger_thread_stopped == 1);
		assert(m_kThreadParams.section_trigger_thread_stopped == 1);

		pWnd = GetDlgItem(IDC_BTN_TSTREAM_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("实时分析");
			pWnd->EnableWindow(TRUE);
		}

		pWnd = GetDlgItem(IDC_BTN_TSFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->EnableWindow(TRUE);
		}

		m_transport_stream.Close();

		GUIReset();

		pApp->SetRunningState(0);
	}
	return 0;
}

CTSMagicView* CTSMagicView::GetView(void)
{
	CTSMagicView* pTSMagicView = NULL;

	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CMultiDocTemplate* pDocTemplate = pApp->m_pDocTemplateForTS;

	POSITION posDoc = pDocTemplate->GetFirstDocPosition();
	while (posDoc != NULL)
	{
		CTSMagicDoc* pDoc = (CTSMagicDoc*)pDocTemplate->GetNextDoc(posDoc);
		POSITION posView = pDoc->GetFirstViewPosition();
		while (posView != NULL)
		{
			pTSMagicView = (CTSMagicView*)pDoc->GetNextView(posView);
			break;
		}
	}

	return pTSMagicView;
}

void CTSMagicView::OnToolPcrDebug() 
{
	// TODO: Add your command handler code here
#if GUI_TS_ANALYZER_PCR
//	m_dlgTSAnalyzerPcr.SetPcrDebug();
#endif
}


LRESULT CTSMagicView::OnTSAppendLog(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_DockPane_RunningLog.AppendLogForUserAction((int)lParam, (char*)wParam); 

	return NULL;
}

LRESULT CTSMagicView::OnTSEtr290Log(WPARAM wParam, LPARAM lParam)
{
#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.AppendEtr290Log((int)lParam, (char*)wParam);
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_DockPane_RunningLog.AppendLogForTSWarning((int)lParam, (char*)wParam);
#endif

//	if (m_dlgProgress.IsWindowVisible())
//	{
//		m_dlgProgress.AddMsg((char*)wParam);
//	}

	return NULL;
}

LRESULT CTSMagicView::OnTSDownloadLog(WPARAM wParam, LPARAM lParam)
{

	return NULL;
}

LRESULT CTSMagicView::OnTSDecimateLog(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgTSDecimateProgress.IsWindowVisible())
	{
		m_dlgTSDecimateProgress.AddMsg((char*)wParam);
	}

	return NULL;
}

LRESULT CTSMagicView::OnTSRecordLog(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgTSDecimateProgress.IsWindowVisible())
	{
		m_dlgTSDecimateProgress.AddMsg((char*)wParam);
	}

	return NULL;
}

LRESULT CTSMagicView::OnTSReportFileSize(WPARAM wParam, LPARAM lParam)
{
	int64_t		total_file_size = *(int64_t*)wParam;

#if GUI_TS_ANALYZER_PACKETS
	m_dlgTSAnalyzerPackets.ReportFileSize(total_file_size);
#endif

	return NULL;
}

LRESULT CTSMagicView::OnTSReportRatio(WPARAM wParam, LPARAM lParam)
{
	GetStatusBar().SetPaneProgress(2, (int)lParam);

	return NULL;
}

LRESULT CTSMagicView::OnTSDownloadRatio(WPARAM wParam, LPARAM lParam)
{
//	if (m_dlgProgress.IsWindowVisible())
//	{
//		m_dlgProgress.SetPos((int)lParam);
//	}

	return NULL;
}

LRESULT CTSMagicView::OnTSDecimateRatio(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgTSDecimateProgress.IsWindowVisible())
	{
		m_dlgTSDecimateProgress.SetPos((int)lParam);
	}

	return NULL;
}

LRESULT CTSMagicView::OnTSRecordRatio(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgTSDecimateProgress.IsWindowVisible())
	{
		m_dlgTSDecimateProgress.SetRecordMsg((int)lParam);
	}

	return NULL;
}

/*
LRESULT CTSMagicView::OnTSReportBitrateMap(WPARAM wParam, LPARAM lParam)
{
#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.UpdateBitrateMap((ULONG*)wParam, (int)lParam);
#endif

	return NULL;
}
*/
/*
LRESULT CTSMagicView::OnTSReportBitrate(WPARAM wParam, LPARAM lParam)
{
#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.UpdateBitrate((int)wParam);
#endif

	return NULL;
}
*/
LRESULT CTSMagicView::OnTSReportPacketLength(WPARAM wParam, LPARAM lParam)
{
#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.UpdatePacketLength((int)wParam);
#endif

	return NULL;
}

LRESULT CTSMagicView::OnTSReportSignalStatus(WPARAM wParam, LPARAM lParam)
{
	int		lost;

	lost = (int)lParam;		//0 -- synced, 1 -- lost
	m_dlgSignalStatus.ShowWindow(lost);

#if GUI_TS_ANALYZER_OVERVIEW
	if (lost == 1)
	{
//		m_dlgTSAnalyzerOverview.m_TsRateJitterGraphOscilloscope.Reset();
		m_dlgTSAnalyzerOverview.m_TsRateJitterGraphHistogram.Reset();
	}
#endif

	return NULL;
}

LRESULT CTSMagicView::OnReportTSTriggerStatus(WPARAM wParam, LPARAM lParam)
{
	CWnd* pWnd = NULL;
	char pszText[256];
	int i;
	uint8_t* ucReqMask;
	uint8_t* ucReqData;
	int length;

	if (wParam == 1)
	{
//		m_dlgTSTriggerWaiting.SetWindowText("TS包捕捉");

//		pWnd = m_dlgTSTriggerWaiting.GetDlgItem(IDC_STATIC_TRIGGER_MASK);
//		pWnd->ShowWindow(SW_HIDE);
		ucReqMask = m_Trigger_TSPacket.GetMatchMasks(&length);
		memset(pszText, 0x00, sizeof(pszText));
		for (i = 0; i < length; i++)
		{
			sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqMask[i]);
		}

		pWnd = m_dlgTSTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
//		pWnd->ShowWindow(SW_HIDE);
		pWnd->SetWindowText(pszText);

//		pWnd = m_dlgTSTriggerWaiting.GetDlgItem(IDC_STATIC_TRIGGER_DATA);
//		pWnd->ShowWindow(SW_HIDE);

		ucReqData = m_Trigger_TSPacket.GetMatchDatas(&length);
		memset(pszText, 0x00, sizeof(pszText));
		for (i = 0; i < length; i++)
		{
			sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqData[i]);
		}
		pWnd = m_dlgTSTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
		pWnd->SetWindowText(pszText);
//		pWnd->ShowWindow(SW_HIDE);

		//开始捕捉，显示等待对话框
		m_dlgTSTriggerWaiting.ShowWindow(SW_SHOW);
	}
	else
	{
		//成功捕捉到至少一个包，关闭等待对话框
		m_dlgTSTriggerWaiting.ShowWindow(SW_HIDE);
#if GUI_TS_ANALYZER_PK_TRIGGER
		m_dlgPacketTrigger.UpdateCatchResult();
#endif 
	}

	return NULL;
}

LRESULT CTSMagicView::OnReportPESTriggerStatus(WPARAM wParam, LPARAM lParam)
{
	CWnd* pWnd = NULL;
	//	char* pszText[2] = (char *[2])lParam;

	if (wParam == 1)
	{
		pWnd = m_dlgPESTriggerWaiting.GetDlgItem(IDC_STATIC_TRIGGER_MASK);
		pWnd->ShowWindow(SW_HIDE);

		pWnd = m_dlgPESTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
		//		pWnd->SetWindowText(pszText[0]);
		pWnd->ShowWindow(SW_HIDE);

		pWnd = m_dlgPESTriggerWaiting.GetDlgItem(IDC_STATIC_TRIGGER_DATA);
		pWnd->ShowWindow(SW_HIDE);

		pWnd = m_dlgPESTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
		//		pWnd->SetWindowText(pszText[1]);
		pWnd->ShowWindow(SW_HIDE);

		//开始捕捉，显示等待对话框
		m_dlgPESTriggerWaiting.ShowWindow(SW_SHOW);
	}
	else if (wParam == 2)
	{
#if GUI_TS_ANALYZER_PESES
		int pes_length = 0;
		uint8_t* pes_buf = m_Trigger_PESPacket.GetCatchedDatas(0, &pes_length);
		uint32_t uiPESStyle = m_Trigger_PESPacket.GetStyle();
		m_dlgTSAnalyzerPesEs.DisplayPESPacket(uiPESStyle, pes_buf, pes_length);
#endif
	}
	else
	{
		//成功捕捉到至少一个包，关闭等待对话框
		m_dlgPESTriggerWaiting.ShowWindow(SW_HIDE);
	}

	return NULL;
}

//LRESULT CTSMagicView::OnReportESTriggerStatus(WPARAM wParam, LPARAM lParam)
//{
//	CWnd* pWnd = NULL;
////	char* pszText[2] = (char *[2])lParam;
//
//	if (wParam == 1)
//	{
////		m_dlgTriggerWaiting.SetWindowText("ES包捕捉");
//
//		pWnd = m_dlgESTriggerWaiting.GetDlgItem(IDC_STATIC_TRIGGER_MASK);
//		pWnd->ShowWindow(SW_HIDE);
//
//		pWnd = m_dlgESTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
////		pWnd->SetWindowText(pszText[0]);
//		pWnd->ShowWindow(SW_HIDE);
//
//		pWnd = m_dlgESTriggerWaiting.GetDlgItem(IDC_STATIC_TRIGGER_DATA);
//		pWnd->ShowWindow(SW_HIDE);
//
//		pWnd = m_dlgESTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
////		pWnd->SetWindowText(pszText[1]);
//		pWnd->ShowWindow(SW_HIDE);
//
//		//开始捕捉，显示等待对话框
//		m_dlgESTriggerWaiting.ShowWindow(SW_SHOW);
//	}
//	else
//	{
//		//成功捕捉到至少一个包，关闭等待对话框
//		m_dlgESTriggerWaiting.ShowWindow(SW_HIDE);
//	}
//
//	return NULL;
//}

LRESULT CTSMagicView::OnReportSectionTriggerStatus(WPARAM wParam, LPARAM lParam)
{
	CWnd* pWnd = NULL;
	char pszText[256];
	int i;
	uint8_t* ucReqMask;
	uint8_t* ucReqData;
	uint8_t* section_buf;
	int length;

	if (wParam == 1)
	{
		ucReqMask = m_Trigger_Section.GetMatchMasks(&length);
		memset(pszText, 0x00, sizeof(pszText));
		for (i = 0; i < length; i++)
		{
			sprintf_s(pszText + i*3, 4, "%02X ", ucReqMask[i]);
		}
		pszText[i * 3] = '\0';

		pWnd = m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
		pWnd->SetWindowText(pszText);

		ucReqData = m_Trigger_Section.GetMatchDatas(&length);
		memset(pszText, 0x00, sizeof(pszText));
		for (i = 0; i < length; i++)
		{
			sprintf_s(pszText +i*3, 4, "%02X ", ucReqData[i]);
		}
		pszText[i * 3] = '\0';

		pWnd = m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
		pWnd->SetWindowText(pszText);

		//开始捕捉，显示等待对话框
		m_dlgSectionTriggerWaiting.ShowWindow(SW_SHOW);
	}
	else
	{
		//成功捕捉到至少一个包，关闭等待对话框
		m_dlgSectionTriggerWaiting.ShowWindow(SW_HIDE);

#if GUI_TS_ANALYZER_PSISI
		section_buf = m_Trigger_Section.GetCatchedDatas(0, &length);
		m_dlgTSAnalyzerPsiSi.DisplaySection(section_buf, length);
		//m_tabItem_PsiSiAnalyzer.DisplaySection(section_buf, length);
#endif
	}

	return NULL;
}


CDB_TSPackets* CTSMagicView::GetTSPacketsDBase(void)
{
	return &m_DB_TSPackets;
}

CDB_Pcrs* CTSMagicView::GetPcrsDBase(void)
{
	return &m_DB_Pcrs;
}

CDB_PsiSiObjs* CTSMagicView::GetPsiSiObjsDBase(void)
{
	return &m_DB_PsiSiObjs;
}

CDB_OCDCs* CTSMagicView::GetOCDCsDBase(void)
{
	return &m_DB_OCDCs;
}

CTransportStream* CTSMagicView::GetTStream(void)
{
	return &m_transport_stream;
}

CTrigger_PsiSiSection* CTSMagicView::GetSectionTrigger(void)
{
	return &m_Trigger_Section;
}

CTrigger_TSPacket* CTSMagicView::GetTSPacketTrigger(void)
{
	return &m_Trigger_TSPacket;
}

CTrigger_PESPacket* CTSMagicView::GetPESPacketTrigger(void)
{
	return &m_Trigger_PESPacket;
}

//void CTSMagicView::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
//
//	if (pApp->GetRunningState() == 1)
//	{
//		CString strAppName = AfxGetAppName();
//		strAppName += "程序仍在运行，强行关闭会破坏系统!";
//
//		AfxMessageBox(strAppName);
//	}
//	else
//	{
//		CFormView::OnClose();
//	}
//}

CMFCStatusBar& CTSMagicView::GetStatusBar() const
{
	return ((CMainFrame*)AfxGetMainWnd())->GetStatusBar();
}



void CTSMagicView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_dlgTSTriggerWaiting.DestroyWindow();
	m_dlgPESTriggerWaiting.DestroyWindow();
	//m_dlgESTriggerWaiting.DestroyWindow();
	m_dlgSectionTriggerWaiting.DestroyWindow();
	m_dlgDsmccDownloadWaiting.DestroyWindow();
	m_dlgTSDecimateProgress.DestroyWindow();
	m_dlgSignalStatus.DestroyWindow();

#if GUI_TS_ANALYZER_OVERVIEW
	m_dlgTSAnalyzerOverview.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_PACKETS
	m_dlgTSAnalyzerPackets.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
	m_dlgPacketTrigger.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_PSISI
	m_dlgTSAnalyzerPsiSi.DestroyWindow();
	//m_tabItem_PsiSiAnalyzer.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_BOUQUETS
	m_dlgTSAnalyzerBouquets.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_EPG
	m_dlgTSAnalyzerEpg.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_NETWORK
	m_dlgTSAnalyzerServices.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_PCR
	m_dlgTSAnalyzerPcr.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_PESES
	m_dlgTSAnalyzerPesEs.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_ES
	m_dlgTSAnalyzerEs.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_TELETEXT
	m_dlgTSAnalyzerTeletext.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_SUBTITLE
	m_dlgTSAnalyzerSubtitle.DestroyWindow();
#endif

#if GUI_TS_ANALYZER_OCDC
	//	m_dlgTSAnalyzerOCDC.DestroyWindow();
	m_dlgTSAnalyzerDsmcc.DestroyWindow();
#endif

#if GUI_TS_PK_MODIFY
	m_dlgPacketModify.DestroyWindow();
#endif

}

void CTSMagicView::InitThreadparams(void)
{
	m_kThreadParams.offline = 1;

	m_kThreadParams.main_thread_running = 0;
	m_kThreadParams.main_thread_stopped = 1;

	m_kThreadParams.packet_decimate_thread_running = 0;
	m_kThreadParams.packet_decimate_thread_stopped = 1;

	m_kThreadParams.dsmcc_download_thread_running = 0;
	m_kThreadParams.dsmcc_download_thread_stopped = 1;

	m_kThreadParams.monitor_thread_running = 0;
	m_kThreadParams.monitor_thread_stopped = 1;

	m_kThreadParams.ts_trigger_thread_running = 0;
	m_kThreadParams.ts_trigger_thread_stopped = 1;

	m_kThreadParams.pes_trigger_thread_running = 0;
	m_kThreadParams.pes_trigger_thread_stopped = 1;

	m_kThreadParams.section_trigger_thread_running = 0;
	m_kThreadParams.section_trigger_thread_stopped = 1;

	m_kThreadParams.stream_option = STREAM_TS;
	//m_kThreadParams.standard_option = 1;						//按照DVB标准分析

	//m_kThreadParams.callback_report_streaminfo = REPORT_StreamInformation;
	//m_kThreadParams.callback_report_progress = NULL;

	//		m_kThreadParams.file_read_offset = 0;
	//		m_kThreadParams.file_read_size = 0;
	//		m_kThreadParams.file_total_size = 0;

	m_kThreadParams.nDecimateStyle = DECIMATE_NONE;

	memset(m_kThreadParams.pszPathHeader, 0x00, sizeof(m_kThreadParams.pszPathHeader));
	memset(m_kThreadParams.pszPathName, 0x00, sizeof(m_kThreadParams.pszPathName));
	memset(m_kThreadParams.pszDecimatePath, 0x00, sizeof(m_kThreadParams.pszDecimatePath));
	memset(m_kThreadParams.pszVesFileName, 0x00, sizeof(m_kThreadParams.pszVesFileName));
	memset(m_kThreadParams.pszAesFileName, 0x00, sizeof(m_kThreadParams.pszAesFileName));

	m_kThreadParams.hMainWnd = this->GetSafeHwnd();

	m_kThreadParams.pDB_Pcrs = &m_DB_Pcrs;
	m_kThreadParams.pDB_TSPackets = &m_DB_TSPackets;
	m_kThreadParams.pDB_PsiSiObjs = &m_DB_PsiSiObjs;
	m_kThreadParams.pDB_OCDCs = &m_DB_OCDCs;
	m_kThreadParams.pTStream = &m_transport_stream;
	m_kThreadParams.pTrigger_Section = &m_Trigger_Section;
	m_kThreadParams.pTrigger_TSPacket = &m_Trigger_TSPacket;
	m_kThreadParams.pTrigger_PESPacket = &m_Trigger_PESPacket;
}
