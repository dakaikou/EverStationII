// SignalStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_TSAssist_SignalStatus.h"

#include "..\Magic_TS\TSMagicView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAssist_SignalStatus dialog

CDlg_TSAssist_SignalStatus::CDlg_TSAssist_SignalStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAssist_SignalStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAssist_SignalStatus)
	//}}AFX_DATA_INIT
}


void CDlg_TSAssist_SignalStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_SignalStatus)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_TSAssist_SignalStatus, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAssist_SignalStatus)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAssist_SignalStatus message handlers

int CDlg_TSAssist_SignalStatus::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CenterWindow();

	return 0;
}

BOOL CDlg_TSAssist_SignalStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
