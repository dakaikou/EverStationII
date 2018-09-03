// Dlg_YUVCombine.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_YUVCombine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVCombine dialog
#include "Dlg_xxxyuvFileName.h"

CDlg_YUVCombine::CDlg_YUVCombine(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_YUVCombine::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_YUVCombine)
	m_nCombineType = 1;
	//}}AFX_DATA_INIT
}


void CDlg_YUVCombine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_YUVCombine)
	DDX_Radio(pDX, IDC_RDO_DYDUDV2YUV, m_nCombineType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_YUVCombine, CDialog)
	//{{AFX_MSG_MAP(CDlg_YUVCombine)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_COMBINE, OnBtnCombine)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_RESET, OnBtnReset)
	ON_BN_CLICKED(IDC_BTN_SAVEAS_YUV, OnBtnSaveasYuv)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVCombine message handlers

void CDlg_YUVCombine::Reset(void)
{
	CWnd* pWnd = NULL;
	CListBox* pListBox = NULL;

	pListBox = (CListBox*)GetDlgItem(IDC_LIST_SRC);
	pListBox->ResetContent();

	pWnd = GetDlgItem(IDC_EDIT_DST_FILE);
	pWnd->SetWindowText("");
}

BOOL CDlg_YUVCombine::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	m_dlgProgress.ShowWindow(SW_HIDE);

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_YUVCombine::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	CString					strFilePath;
	CString					strTrim;
	CDlg_xxxyuvFileName		dlg_filename;
	char					pszFileName[128];
	char					pszIndex[128];
//	int						nPosExt;
	int						nPathLength;
	FILE*					fp;

	if (dlg_filename.DoModal() == IDOK)
	{
		strFilePath = dlg_filename.m_strFileName;
		nPathLength = strFilePath.GetLength();

		m_dlgProgress.SetWindowText("增加文件");
		m_dlgProgress.ShowWindow(SW_SHOW);
		for (int i = dlg_filename.m_nStartIndex; i <= dlg_filename.m_nEndIndex; i++)
		{
			sprintf_s(pszIndex, sizeof(pszIndex), "%d", i);
			strTrim = strFilePath.Left(nPathLength - strlen(pszIndex));

			sprintf_s(pszFileName, sizeof(pszFileName), "%s%s.yuv", strTrim.GetBuffer(128), pszIndex);

//			fp = fopen(pszFileName, "rb");
			fopen_s(&fp, pszFileName, "rb");
			if (fp != NULL)
			{
				CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_SRC);
				pListBox->AddString(pszFileName);
//				m_listyuv.AddString(pszFileName);
				fclose(fp);
			}
		}
		m_dlgProgress.ShowWindow(SW_HIDE);
	}
}

void CDlg_YUVCombine::OnBtnCombine() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_SRC);
	int nCount = pListBox->GetCount();
	FILE*	fp_src = NULL;
	FILE*	fp_dst = NULL;
	unsigned char	buf[2048];
	int				rdcount;
	char			strSrc[128];
	CString			strDstFile;

	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_EDIT_DST_FILE);
	pWnd->GetWindowText(strDstFile);

	if (strDstFile != "")
	{
		fopen_s(&fp_dst, strDstFile.GetBuffer(128), "wb");
		if (fp_dst != NULL)
		{
			m_dlgProgress.SetWindowText("合并文件");
			m_dlgProgress.ShowWindow(SW_SHOW);
			for (int i = 0; i < nCount; i++)
			{
				pListBox->GetText(i, strSrc);
				fopen_s(&fp_src, strSrc, "rb");
				if (fp_src != NULL)
				{
					do
					{
						rdcount = fread(buf, 1, 2048, fp_src);
						fwrite(buf, 1, rdcount, fp_dst);

					} while (rdcount == 2048);

					fclose(fp_src);
				}
			}
			m_dlgProgress.ShowWindow(SW_HIDE);

			fclose(fp_dst);
		}
	}
}

void CDlg_YUVCombine::OnBtnDel() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlg_YUVCombine::OnBtnDown() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlg_YUVCombine::OnBtnReset() 
{
	// TODO: Add your control notification handler code here
	Reset();
}

void CDlg_YUVCombine::OnBtnSaveasYuv() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgDst(FALSE, NULL);	

	if (dlgDst.DoModal() == IDOK)
	{
		CWnd* pWnd = (CWnd*)GetDlgItem(IDC_EDIT_DST_FILE);
		pWnd->SetWindowText(dlgDst.GetPathName());
	}
}

void CDlg_YUVCombine::OnBtnUp() 
{
	// TODO: Add your control notification handler code here
	
}
