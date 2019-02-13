// Dlg_Bouquet.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Bouquet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TS_Analyzer_Bouquets dialog


CDlg_TSAnalyzer_Bouquets::CDlg_TSAnalyzer_Bouquets(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Bouquets::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TS_Analyzer_Bouquets)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_Bouquets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TS_Analyzer_Bouquets)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Bouquets, CDialog)
	//{{AFX_MSG_MAP(CDlg_TS_Analyzer_Bouquets)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TS_Analyzer_Bouquets message handlers
void CDlg_TSAnalyzer_Bouquets::Reset(void)
{
	m_pPane->Reset();
	m_pInfoTree->Reset();
}

//void CDlg_TS_Analyzer_Bouquets::Set(int offline)
//{
//	m_pPane->Set(offline);
//	m_pTree->Set(offline);
//}

BOOL CDlg_TSAnalyzer_Bouquets::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CNaviTree_Bouquets), CSize(100,0), NULL);
		m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CTreeView_PacketSyntax), CSize(0,0), NULL);
		
		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pPane = (CNaviTree_Bouquets*)m_wndSplitter.GetPane(0, 0);
		m_pInfoTree = (CTreeView_PacketSyntax*)m_wndSplitter.GetPane(0, 1);
		m_pInfoTree->Init("Bouquet分析");

		m_pPane->m_pInfoTree = m_pInfoTree;
	}

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Bouquets::OnSize(UINT nType, int cx, int cy) 
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


void CDlg_TSAnalyzer_Bouquets::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
