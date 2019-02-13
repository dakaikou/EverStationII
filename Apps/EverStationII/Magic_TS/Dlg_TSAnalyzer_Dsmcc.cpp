// Dlg_Epg.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Dsmcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Epg dialog

CDlg_TSAnalyzer_Dsmcc::CDlg_TSAnalyzer_Dsmcc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Dsmcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Epg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_Dsmcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Epg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Dsmcc, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Dsmcc)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Epg message handlers

void CDlg_TSAnalyzer_Dsmcc::OnSize(UINT nType, int cx, int cy)
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

BOOL CDlg_TSAnalyzer_Dsmcc::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CNaviList_DSMCCs), CSize(100, 0), NULL);
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CTreeView_PacketSyntax), CSize(0, 0), NULL);

		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pNaviPane = (CNaviList_DSMCCs*)m_wndSplitter.GetPane(0, 0);
		m_pInfoTree = (CTreeView_PacketSyntax*)m_wndSplitter.GetPane(0, 1);
		m_pInfoTree->Init("DSMCC 语义分析");

		m_pNaviPane->m_pInfoTree = m_pInfoTree;
	}

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Dsmcc::Reset(void)
{
	m_pNaviPane->Reset();
	m_pInfoTree->Reset();
}

//void CDlg_TSAnalyzer_Epg::Set(int offline)
//{
//	m_pPane->Set(offline);
//	m_pList->Set(offline);
//}

void CDlg_TSAnalyzer_Dsmcc::UpdatePMT(CPMT* pPMT)
{
	m_pNaviPane->UpdatePMT(pPMT);
}

void CDlg_TSAnalyzer_Dsmcc::UpdateSDT(CSDT* pSDT)
{
	m_pNaviPane->UpdateSDT(pSDT);
}

void CDlg_TSAnalyzer_Dsmcc::UpdateDSMCC(CPVT* pPVT)
{
	m_pNaviPane->UpdateDSMCC(pPVT);
}



void CDlg_TSAnalyzer_Dsmcc::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
