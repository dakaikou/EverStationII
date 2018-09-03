// Dlg_ESOverview.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_ESOverview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_ESOverview dialog
#include "..\resource.h"

CDlg_ESOverview::CDlg_ESOverview(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_ESOverview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_ESOverview)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_ESOverview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_ESOverview)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_ESOverview, CDialog)
	//{{AFX_MSG_MAP(CDlg_ESOverview)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_ESOverview message handlers
void CDlg_ESOverview::Reset(void)
{
}

