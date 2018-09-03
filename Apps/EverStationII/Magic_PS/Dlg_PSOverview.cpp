// Dlg_PSOverview.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_PSOverview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_PSOverview dialog
#include "..\resource.h"

CDlg_PSOverview::CDlg_PSOverview(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_PSOverview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_PSOverview)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_PSOverview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_PSOverview)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_PSOverview, CDialog)
	//{{AFX_MSG_MAP(CDlg_PSOverview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_PSOverview message handlers
void CDlg_PSOverview::Reset(void)
{
}

BOOL CDlg_PSOverview::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
