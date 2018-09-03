// Dlg_Decimate.cpp : implementation file
//

#include <stdafx.h>
#include "Dlg_Decimate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Decimate dialog


CDlg_Decimate::CDlg_Decimate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Decimate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Decimate)
	m_strAFile = _T("");
	m_strVFile = _T("");
	//}}AFX_DATA_INIT
}


void CDlg_Decimate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Decimate)
	DDX_Control(pDX, IDC_CMB_DISPATCH_OPTION, m_cmbDispatchOption);
	DDX_Control(pDX, IDC_LIST_ES_INFO, m_listESInfo);
	DDX_Text(pDX, IDC_EDIT_AFILE, m_strAFile);
	DDX_Text(pDX, IDC_EDIT_VFILE, m_strVFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Decimate, CDialog)
	//{{AFX_MSG_MAP(CDlg_Decimate)
	ON_BN_CLICKED(IDC_BTN_BROWSE_AFILE, OnBtnBrowseAfile)
	ON_BN_CLICKED(IDC_BTN_BROWSE_VFILE, OnBtnBrowseVfile)
	ON_CBN_SELCHANGE(IDC_CMB_DISPATCH_OPTION, OnSelchangeCmbDispatchOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Decimate message handlers

BOOL CDlg_Decimate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	char	pszText[64];

	int		nColWidth[] = {120, 300};
	CString str[] = {"项目", "信息"};
	UINT	uiAlign[] = {LVCFMT_LEFT, LVCFMT_LEFT};

	m_listESInfo.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	RECT	rect;

	m_listESInfo.GetWindowRect(&rect);
	nColWidth[1] = (rect.right - rect.left) - nColWidth[0] - 5;

	for (int i = 0; i < sizeof(nColWidth) / sizeof(int); i++)
	{
		m_listESInfo.InsertColumn(i, str[i], uiAlign[i], nColWidth[i], -1);
	}

	int	nItem =0;
	m_listESInfo.InsertItem(nItem, _T("系  统"));

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    格  式"));
	m_listESInfo.SetItemText(nItem, 1, m_packinfo.pszFormat);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    码  率"));
	sprintf_s(pszText, sizeof(pszText), "%d bps", m_packinfo.bitrate);
	m_listESInfo.SetItemText(nItem, 1, pszText);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T(""));

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("视  频"));

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    格  式"));
	m_listESInfo.SetItemText(nItem, 1, m_videoinfo.pszFormat);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    流  ID"));
	sprintf_s(pszText, sizeof(pszText), "0x%02X (%d)", m_videoinfo.stream_id, m_videoinfo.stream_id);
	m_listESInfo.SetItemText(nItem, 1, pszText);
	
	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    分辨率"));
	sprintf_s(pszText, sizeof(pszText), "%d x %d", m_videoinfo.weight, m_videoinfo.height);
	m_listESInfo.SetItemText(nItem, 1, pszText);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    宽高比"));
	m_listESInfo.SetItemText(nItem, 1, m_videoinfo.pszAspect);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    帧  频"));
	m_listESInfo.SetItemText(nItem, 1, m_videoinfo.pszFramerate);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    码  率"));
	sprintf_s(pszText, sizeof(pszText), "%d bps", m_videoinfo.bitrate);
	m_listESInfo.SetItemText(nItem, 1, pszText);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T(""));

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("音  频"));

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    格  式"));
	m_listESInfo.SetItemText(nItem, 1, m_audioinfo.pszFormat);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    流  ID"));
	sprintf_s(pszText, sizeof(pszText), "0x%02X (%d)", m_audioinfo.stream_id, m_audioinfo.stream_id);
	m_listESInfo.SetItemText(nItem, 1, pszText);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    采样率"));
	m_listESInfo.SetItemText(nItem, 1, m_audioinfo.pszSamplerate);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    声  道"));
	m_listESInfo.SetItemText(nItem, 1, m_audioinfo.pszChannel);

	nItem++;
	m_listESInfo.InsertItem(nItem, _T("    码  率"));
	m_listESInfo.SetItemText(nItem, 1, m_audioinfo.pszBitrate);

	m_strVFile = m_videoinfo.pszFile;
	m_strAFile = m_audioinfo.pszFile;

	m_cmbDispatchOption.ResetContent();
	m_cmbDispatchOption.AddString(_T("无"));
	m_cmbDispatchOption.AddString(_T("导出视频"));
	m_cmbDispatchOption.AddString(_T("导出音频"));
	m_cmbDispatchOption.AddString(_T("导出视频和音频"));
	m_cmbDispatchOption.SetCurSel(0);

	CWnd* pWnd = GetDlgItem(IDC_EDIT_VFILE);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_EDIT_AFILE);
	pWnd->EnableWindow(FALSE);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Decimate::OnBtnBrowseAfile() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlg_Decimate::OnBtnBrowseVfile() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlg_Decimate::OnSelchangeCmbDispatchOption() 
{
	// TODO: Add your control notification handler code here
	int	nSel;
	CWnd* pWnd;

	nSel = m_cmbDispatchOption.GetCurSel();

	if (nSel == 0)									//不导出
	{
		pWnd = GetDlgItem(IDC_EDIT_VFILE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_AFILE);
		pWnd->EnableWindow(FALSE);
	}
	else if (nSel == 1)									//导出视频
	{
		pWnd = GetDlgItem(IDC_EDIT_VFILE);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_EDIT_AFILE);
		pWnd->EnableWindow(FALSE);
	}
	else if (nSel == 2)									//导出音频
	{
		pWnd = GetDlgItem(IDC_EDIT_VFILE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_AFILE);
		pWnd->EnableWindow(TRUE);
	}
	else if (nSel == 3)									//导出视频和音频
	{
		pWnd = GetDlgItem(IDC_EDIT_VFILE);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_EDIT_AFILE);
		pWnd->EnableWindow(TRUE);
	}

	m_packinfo.dispatch_option = nSel;
}
