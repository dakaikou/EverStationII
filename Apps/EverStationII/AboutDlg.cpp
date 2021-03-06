
// EverStationII.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
//#include "afxwinappex.h"
//#include "afxdialogex.h"
#include "AboutDlg.h"
#include "Resource.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	char pszModuleFileName[256];
	char* pszExeName = NULL;

	GetModuleFileNameA(NULL, pszModuleFileName, sizeof(pszModuleFileName));

	char*		 pdest;
	int			 idx = 0;

	pdest = strchr(pszModuleFileName, '\\');
	while (pdest != NULL)
	{
		idx = (int)(pdest - pszModuleFileName);
		pdest = strchr(pszModuleFileName + idx + 1, '\\');
	}
	pszExeName = pszModuleFileName + idx + 1;

	//CString strAppName = AfxGetAppName();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_APPNAME_EVERSTATIONII);

	pWnd->SetWindowText(pszExeName);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

