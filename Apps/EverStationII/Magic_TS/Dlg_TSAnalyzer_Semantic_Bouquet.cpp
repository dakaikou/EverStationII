// Dlg_Bouquet.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Semantic_Bouquet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Semantic_Bouquets dialog
#include "../Magic_TS/TSMagicView.h"
#include "Utilities\Directory\Include\TOOL_Directory.h"

CDlg_TSAnalyzer_Semantic_Bouquets::CDlg_TSAnalyzer_Semantic_Bouquets(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Semantic_Bouquets::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Semantic_Bouquets)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_Semantic_Bouquets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Semantic_Bouquets)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Semantic_Bouquets, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Semantic_Bouquets)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_BOUQUET_SEL_CHANGE, OnReportBouquetSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Semantic_Bouquets message handlers
void CDlg_TSAnalyzer_Semantic_Bouquets::Reset(void)
{
	m_pNaviPane->Reset();
	m_pInfoTree->Reset();
}

//void CDlg_TSAnalyzer_Semantic_Bouquets::Set(int offline)
//{
//	m_pPane->Set(offline);
//	m_pTree->Set(offline);
//}

BOOL CDlg_TSAnalyzer_Semantic_Bouquets::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CNaviTree_BouquetGuide), CSize(100,0), NULL);
		m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CTreeView_XMLBrowser), CSize(0,0), NULL);
		
		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pNaviPane = (CNaviTree_BouquetGuide*)m_wndSplitter.GetPane(0, 0);
		m_pNaviPane->Set(this->GetSafeHwnd());

		m_pInfoTree = (CTreeView_XMLBrowser*)m_wndSplitter.GetPane(0, 1);
		m_pInfoTree->Init("Bouquet");
	}

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Semantic_Bouquets::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);

		m_wndSplitter.SetColumnInfo(0, 480, 0);
//		m_wndSplitter.SetColumnInfo(1, 0, 0);

		m_wndSplitter.RecalcLayout();
	}
}


void CDlg_TSAnalyzer_Semantic_Bouquets::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

LRESULT CDlg_TSAnalyzer_Semantic_Bouquets::OnReportBouquetSelChange(WPARAM wParam, LPARAM lParam)
{
	uint32_t code = (uint32_t)lParam;
	uint16_t usKey = ((lParam & 0xffff0000) >> 16);
	uint16_t bouquet_id = (lParam & 0x0000ffff);

	m_pInfoTree->Reset();

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiObjs* pDB_PsiSiOjbs = pTSMagicView->GetPsiSiObjsDBase();

	TALForXMLDoc xmlDoc;
	pDB_PsiSiOjbs->BuildBouquetTree(code, &xmlDoc);
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

	sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\BOUQUET_sematics_0x%04X.xml", pszXmlDir, code & 0xffff);
	xmlDoc.SaveFile(pszFilePath);
#endif

	return 0;
}
