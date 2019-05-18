// Dlg_Epg.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_SemanticEx_Dsmcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_SemanticEx_Dsmcc dialog
#include "../Magic_TS/TSMagicView.h"
#include "Utilities\Directory\Include\TOOL_Directory.h"

#include "TSMagic_AnalyseThread_Dsmcc_Download.h"

CDlg_TSAnalyzer_SemanticEx_Dsmcc::CDlg_TSAnalyzer_SemanticEx_Dsmcc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_SemanticEx_Dsmcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_SemanticEx_Dsmcc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_SemanticEx_Dsmcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_SemanticEx_Dsmcc)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_SemanticEx_Dsmcc, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_SemanticEx_Dsmcc)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_OCDC_SEL_CHANGE, OnReportOcDcSelChange)
	ON_MESSAGE(WM_USER_OCDC_DOWNLOAD_PREPARE, OnReportOcDcDownloadPrepare)
	ON_MESSAGE(WM_USER_OCDC_DOWNLOAD_START, OnReportOcDcDownloadStart)
	ON_MESSAGE(WM_USER_OCDC_APPEND_PID, OnReportOcDcAppendPID)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_SemanticEx_Dsmcc message handlers

void CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);
		m_wndSplitter.SetColumnInfo(0, 1024, 400);			//设置列宽度
		m_wndSplitter.RecalcLayout();
	}
}

BOOL CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CNaviList_DsmccGuide), CSize(100, 0), NULL);
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CTreeView_XMLBrowser), CSize(0, 0), NULL);

		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pNaviPane = (CNaviList_DsmccGuide*)m_wndSplitter.GetPane(0, 0);
		m_pNaviPane->Set(this->GetSafeHwnd());

		m_pInfoTree = (CTreeView_XMLBrowser*)m_wndSplitter.GetPane(0, 1);
		m_pInfoTree->Init("DSMCC 语义分析");
	}

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_SemanticEx_Dsmcc::Reset(void)
{
	m_pNaviPane->Reset();
	m_pInfoTree->Reset();
}

//void CDlg_TSAnalyzer_Epg::Set(int offline)
//{
//	m_pPane->Set(offline);
//	m_pList->Set(offline);
//}

void CDlg_TSAnalyzer_SemanticEx_Dsmcc::UpdatePMT(CPMT* pPMT)
{
	m_pNaviPane->UpdatePMT(pPMT);
}

void CDlg_TSAnalyzer_SemanticEx_Dsmcc::UpdateSDT(CSDT* pSDT)
{
	m_pNaviPane->UpdateSDT(pSDT);
}

void CDlg_TSAnalyzer_SemanticEx_Dsmcc::UpdateDSMCC(CPVT* pPVT)
{
	m_pNaviPane->UpdateDSMCC(pPVT);
}



void CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

LRESULT CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnReportOcDcSelChange(WPARAM wParam, LPARAM lParam)
{
	uint16_t usPID = (uint16_t)(lParam & 0x0000ffff);

	m_pInfoTree->Reset();

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();
	
	TALForXMLDoc xmlDoc;
	pDB_PsiSiObjs->BuildDsmccTree(usPID, &xmlDoc);
	m_pInfoTree->ShowXMLDoc(&xmlDoc);
	
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
	
	sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_sematics_0x%04X.xml", pszXmlDir, usPID);
	xmlDoc.SaveFile(pszFilePath);
#endif

	return 0;
}

LRESULT CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnReportOcDcDownloadPrepare(WPARAM wParam, LPARAM lParam)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();

	pDB_OCDCs->Reset();

	return 0;
}

LRESULT CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnReportOcDcAppendPID(WPARAM wParam, LPARAM lParam)
{
	uint16_t usPID = (uint16_t)(lParam & 0x0000ffff);

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();

	DOWNLOAD_INFO_t stDownloadInfo;
	stDownloadInfo.usCandidatePID = usPID;
	pDB_OCDCs->AppendDownloadInfo(&stDownloadInfo);

	return 0;
}

LRESULT CDlg_TSAnalyzer_SemanticEx_Dsmcc::OnReportOcDcDownloadStart(WPARAM wParam, LPARAM lParam)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	pTSMagicView->m_kThreadParams.dsmcc_download_thread_running = 0;
	pTSMagicView->m_kThreadParams.dsmcc_download_thread_stopped = 0;
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_dsmcc_download_thread, (LPVOID) & (pTSMagicView->m_kThreadParams), 0, 0);

	return 0;
}
