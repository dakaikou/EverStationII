// Dlg_Progress.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation.h"
#include "Dlg_Progress.h"
#include "..\common\define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Progress dialog

CDlg_Progress::CDlg_Progress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Progress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Progress)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCount = 0;
}


void CDlg_Progress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Progress)
	DDX_Control(pDX, IDC_LIST_PROGRESS_LOG, m_listLog);
	DDX_Control(pDX, IDC_PROGRESS_FILERATIO, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Progress, CDialog)
	//{{AFX_MSG_MAP(CDlg_Progress)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	//ON_MESSAGE(MSG_PS_DISPATCH, OnDispatchMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Progress message handlers

BOOL CDlg_Progress::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CenterWindow();
	m_progress.SetRange(0, 100);

	int		nColWidth[] = {60, 300};
	CString str[] = {"序号", "日志信息"};
	UINT	uiAlign[] = {LVCFMT_LEFT, LVCFMT_LEFT};

	m_listLog.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	RECT	rect;

	m_listLog.GetWindowRect(&rect);
	nColWidth[1] = (rect.right - rect.left) - nColWidth[0] - 5;

	for (int i = 0; i < sizeof(nColWidth) / sizeof(int); i++)
	{
		m_listLog.InsertColumn(i, str[i], uiAlign[i], nColWidth[i], -1);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Progress::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if (bShow)
	{
		m_nCount = 0;
//		SetTimer(0, 1000, NULL);
		m_progress.SetPos(0);

		m_defSize = 10000000;				//10M
		m_nRecordedSize = 0;
		m_nOldRatio = 0;
	}
	else
	{
//		KillTimer(0);
	}
}

void CDlg_Progress::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_progress.SetPos(m_nCount);
	m_nCount ++;
	m_nCount %= 100;
	
	CDialog::OnTimer(nIDEvent);
}

LRESULT CDlg_Progress::OnDispatchMsg(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
	{
//		ShowWindow(SW_HIDE);
	}
	else if (lParam == 1)
	{
		m_progress.SetPos((int)wParam);
	}
	else if (lParam == 2)
	{
		int count = m_listLog.GetItemCount();

		if (count < MAX_LOG_COUNT)
		{
			CString strTemp;
			strTemp.Format("%d", count);
			m_listLog.InsertItem(count, strTemp);
			m_listLog.SetItemText(count, 1, (char*)wParam);

			m_listLog.EnsureVisible(count, 0);
		}
	}

	return 0;
}

void CDlg_Progress::SetPos(int ratio)					//num%
{
	m_progress.SetPos(ratio);
}

void CDlg_Progress::SetRecordMsg(int filesize)					//num%
{
	char pszText[MAX_PATH];


	int deltSize = filesize - m_nRecordedSize;

	if (deltSize >= m_defSize)
	{
		sprintf_s(pszText, sizeof(pszText), "已经录制长度%dM字节", filesize / 1000000);
		AddMsg(pszText);

		CWnd* pWnd = GetDlgItem(IDC_PROGRESS_FILERATIO);
		pWnd->SetWindowText(pszText);

		m_nRecordedSize = filesize;
	}

	int ratio = 100 * deltSize / m_defSize;
	if (ratio != m_nOldRatio)
	{
		m_progress.SetPos(ratio);
		m_nOldRatio = ratio;
	}
//	m_progress.SetWindowText(pszText);

//	UpdateData(FALSE);
}

void CDlg_Progress::AddMsg(char* pszMsg)
{
	CString strTemp;
	int count = m_listLog.GetItemCount();

	strTemp.Format("%d", count);
	m_listLog.InsertItem(count, strTemp);
	m_listLog.SetItemText(count, 1, pszMsg);

	m_listLog.EnsureVisible(count, 0);
}

void CDlg_Progress::Reset(void)
{
	m_progress.SetPos(0);
	m_listLog.DeleteAllItems();
}
