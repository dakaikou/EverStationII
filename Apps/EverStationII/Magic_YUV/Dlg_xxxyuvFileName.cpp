// Dlg_xxxyuvFileName.cpp : implementation file
//

#include "stdafx.h"
//#include "yuvviewer.h"
#include "Dlg_xxxyuvFileName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_xxxyuvFileName dialog


CDlg_xxxyuvFileName::CDlg_xxxyuvFileName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_xxxyuvFileName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_xxxyuvFileName)
	m_nNumWidth = 0;
	m_strFilePrefix = _T("");
	m_nStartIndex = 0;
	m_nEndIndex = 0;
	//}}AFX_DATA_INIT
}


void CDlg_xxxyuvFileName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_xxxyuvFileName)
	DDX_Text(pDX, IDC_EDIT_NUM_WIDTH, m_nNumWidth);
	DDX_Text(pDX, IDC_EDIT_PRE_FILENAME, m_strFilePrefix);
	DDX_Text(pDX, IDC_EDIT_NUM_START, m_nStartIndex);
	DDX_Text(pDX, IDC_EDIT_NUM_END, m_nEndIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_xxxyuvFileName, CDialog)
	//{{AFX_MSG_MAP(CDlg_xxxyuvFileName)
	ON_BN_CLICKED(IDC_BTN_LOOKUP, OnBtnLookup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_xxxyuvFileName message handlers

BOOL CDlg_xxxyuvFileName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_strFileName = _T("");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_xxxyuvFileName::OnBtnLookup() 
{
	// TODO: Add your control notification handler code here
	char BASED_CODE szFilter_yuv[] = "All Files (*.yuv)|*.yuv||";
	CFileDialog dlg_yuv(TRUE, "yuv", NULL, OFN_HIDEREADONLY, szFilter_yuv);

	CString strExt;
	CString strFileName;
	int		nExtLength;
	int		nFileNameLength;

	if (dlg_yuv.DoModal() == IDOK)
	{
		strFileName = dlg_yuv.GetPathName();
		strExt = dlg_yuv.GetFileExt();
		strExt.Insert(0, '.');
		nExtLength = strExt.GetLength();
		nFileNameLength = strFileName.GetLength();
		nFileNameLength -= nExtLength;
		m_strFilePrefix = strFileName.Left(nFileNameLength);
	}

	UpdateData(FALSE);
}

void CDlg_xxxyuvFileName::OnOK() 
{
	// TODO: Add extra validation here
//	int		nFileNameLength;

	UpdateData(TRUE);

//	nFileNameLength = m_strFilePrefix.GetLength() - m_nNumWidth;
//	m_strFileName = m_strFilePrefix.Left(nFileNameLength);

	m_strFileName = m_strFilePrefix;
	
	CDialog::OnOK();
}
