// Dlg_Epg.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Epg.h"
#include "View_EpgContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Epg dialog


CDlg_TSAnalyzer_Epg::CDlg_TSAnalyzer_Epg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Epg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Epg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_Epg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Epg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Epg, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Epg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Epg message handlers

void CDlg_TSAnalyzer_Epg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);
		m_wndSplitter.SetColumnInfo(0, 580, 0);			//设置列宽度
		m_wndSplitter.RecalcLayout();
	}
}

BOOL CDlg_TSAnalyzer_Epg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0, 0,RUNTIME_CLASS(CNaviTree_Services), CSize(100, 0), NULL);
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CView_EpgContainer), CSize(0, 0), NULL);

		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pServiceListPane = (CNaviTree_Services*)m_wndSplitter.GetPane(0, 0);
		m_pServiceListPane->m_pViewEpg = (CView_EpgContainer*)m_wndSplitter.GetPane(0, 1);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Epg::Reset(void)
{
	m_pServiceListPane->Reset();

	//m_pServiceDescription->Reset();
	//m_pEpgScheduleList->Reset();
}

//void CDlg_TSAnalyzer_Epg::Set(int offline)
//{
//	m_pPane->Set(offline);
//	m_pList->Set(offline);
//}

void CDlg_TSAnalyzer_Epg::UpdateSDT(CSDT* pSDT)
{
	m_pServiceListPane->UpdateSDT(pSDT);
}

void CDlg_TSAnalyzer_Epg::UpdateNIT(CNIT* pNIT)
{
	m_pServiceListPane->UpdateNIT(pNIT);
}



void CDlg_TSAnalyzer_Epg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
