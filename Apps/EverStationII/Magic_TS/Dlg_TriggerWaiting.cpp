// TriggerWaitingDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation_TSMagic.h"
#include "Dlg_TriggerWaiting.h"

#include "..\Magic_TS\TSMagicView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <assert.h>

/////////////////////////////////////////////////////////////////////////////
// CDlg_TriggerWaiting dialog
//#include "EverStation_TSMagicDlg.h"

CDlg_TriggerWaiting::CDlg_TriggerWaiting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TriggerWaiting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TriggerWaiting)
	//}}AFX_DATA_INIT
}


void CDlg_TriggerWaiting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TriggerWaiting)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_TriggerWaiting, CDialog)
	//{{AFX_MSG_MAP(CDlg_TriggerWaiting)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TriggerWaiting message handlers

int CDlg_TriggerWaiting::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CenterWindow();
//	CRect		rect;

//	GetWindowRect(&rect);
//	MoveWindow(400, 300, rect.Width(), rect.Height());

	return 0;
}

void CDlg_TriggerWaiting::OnTimer(UINT_PTR nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CProgressCtrl*	pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_WAITING);

	m_nWaitingTime ++;
	if (m_nWaitingTime >= 10)
	{
		m_nWaitingTime = 0;
	}

	pCtrl->SetPos(m_nWaitingTime);
	
	CDialog::OnTimer(nIDEvent);
}

void CDlg_TriggerWaiting::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	CProgressCtrl*	pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_WAITING);

	m_nWaitingTime = 0;
	pCtrl->SetPos(0);

	if (bShow)
	{
		SetTimer(0, 1000, NULL);
	}
	else
	{
		KillTimer(0);
	}
}

BOOL CDlg_TriggerWaiting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CProgressCtrl*	pCtrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_WAITING);
	pCtrl->SetRange(0, 10);
	pCtrl->SetPos(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TriggerWaiting::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CString strTitle;

	GetWindowText(strTitle);

//	if (pWindow->m_kThreadParams.ts_trigger_thread_running == 1)
	if (strTitle == "TS²¶×½")
	{
//		assert(pWindow->m_kThreadParams.es_trigger_thread_running == 0);
//		assert(pWindow->m_kThreadParams.section_trigger_thread_running == 0);

		pTSMagicView->m_kThreadParams.ts_trigger_thread_running = 0;

#if GUI_TS_ANALYZER_PK_TRIGGER
		pTSMagicView->m_dlgPacketTrigger.CancelTrigger();
#endif
	}
	else if (strTitle == "PES²¶×½")
	{
		//		assert(pWindow->m_kThreadParams.ts_trigger_thread_running == 0);
		//		assert(pWindow->m_kThreadParams.es_trigger_thread_running == 0);

		pTSMagicView->m_kThreadParams.pes_trigger_thread_running = 0;
	}
	else if (strTitle == "ES²¶×½")
	{
//		assert(pWindow->m_kThreadParams.ts_trigger_thread_running == 0);
//		assert(pWindow->m_kThreadParams.section_trigger_thread_running == 0);

		pTSMagicView->m_kThreadParams.es_trigger_thread_running = 0;

//#if GUI_TS_ANALYZER_PESES
//		pWindow->m_dlgTSAnalyzerPesEs.CancelTrigger();
//#endif
	}
//	if (pWindow->m_kThreadParams.section_trigger_thread_running == 1)
	else if (strTitle == "section²¶×½")
	{
//		assert(pWindow->m_kThreadParams.ts_trigger_thread_running == 0);
//		assert(pWindow->m_kThreadParams.es_trigger_thread_running == 0);

		pTSMagicView->m_kThreadParams.section_trigger_thread_running = 0;
	}
	else if (strTitle == "DSM-CCÏÂÔØ")
	{
//		assert(pWindow->m_kThreadParams.ts_trigger_thread_running == 0);
//		assert(pWindow->m_kThreadParams.es_trigger_thread_running == 0);

		pTSMagicView->m_kThreadParams.dsmcc_download_thread_running = 0;
	}

	ShowWindow(SW_HIDE);
	
//	TSMagic_Debug(DEBUG_WARNING, "ÓÃ»§ÖÐÖ¹²¶×½");
}
