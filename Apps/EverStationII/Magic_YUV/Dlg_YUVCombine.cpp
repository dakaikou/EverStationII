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
	DDX_Radio(pDX, IDC_YUV_COMBINE_RDO_DYDUDV2YUV, m_nCombineType);
	DDX_Control(pDX, IDC_YUV_COMBINE_LIST_SRC, m_listSourceFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_YUVCombine, CDialog)
	//{{AFX_MSG_MAP(CDlg_YUVCombine)
	ON_BN_CLICKED(IDC_YUV_COMBINE_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_YUV_COMBINE_BTN_DO, OnBtnDo)
	ON_BN_CLICKED(IDC_YUV_COMBINE_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_YUV_COMBINE_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_YUV_COMBINE_BTN_RESET, OnBtnReset)
	ON_BN_CLICKED(IDC_YUV_COMBINE_BTN_UP, OnBtnUp)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVCombine message handlers

void CDlg_YUVCombine::Reset(void)
{
	CWnd* pWnd = NULL;
	//CListBox* pListBox = NULL;

	//pListBox = (CListBox*)GetDlgItem(IDC_YUV_COMBINE_LIST_SRC);
	//pListBox->ResetContent();


	m_listSourceFile.ResetContent();
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
	//CString					strTrim;
	//CDlg_xxxyuvFileName		dlg_filename;
	//char					pszFileName[128];
	//char					pszIndex[128];
//	int						nPosExt;
	//int						nPathLength;
	//FILE*					fp;

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER, _T("YUV文件(*.yuv)|*.yuv|所有文件(*.*)|*.*||"));

	// Create buffer for file names.
	const DWORD numberOfFileNames = 1024;//最多允许32个文件
	const DWORD fileNameMaxLength = MAX_PATH + 1;
	const DWORD bufferSize = (numberOfFileNames * fileNameMaxLength) + 1;
	TCHAR* filenamesBuffer = new TCHAR[bufferSize];
	// Initialize beginning and end of buffer.
	filenamesBuffer[0] = '\0';//必须的
	filenamesBuffer[bufferSize - 1] = '\0';

	// Attach buffer to OPENFILENAME member.
	fileDlg.m_ofn.lpstrFile = filenamesBuffer;
	fileDlg.m_ofn.nMaxFile = bufferSize;

	if (fileDlg.DoModal() == IDOK)
	{
		//CListBox* pListBox = (CListBox*)GetDlgItem(IDC_YUV_COMBINE_LIST_SRC);

		POSITION pos = fileDlg.GetStartPosition();
		while (pos != NULL)
		{
			strFilePath = fileDlg.GetNextPathName(pos);//返回选定文件文件名// Retrieve file name(s).
			m_listSourceFile.AddString(strFilePath);
		}

//		strFilePath = dlg_filename.m_strFileName;
//		nPathLength = strFilePath.GetLength();
//
//		m_dlgProgress.SetWindowText("增加文件");
//		m_dlgProgress.ShowWindow(SW_SHOW);
//		for (int i = dlg_filename.m_nStartIndex; i <= dlg_filename.m_nEndIndex; i++)
//		{
//			sprintf_s(pszIndex, sizeof(pszIndex), "%d", i);
//			strTrim = strFilePath.Left(nPathLength - strlen(pszIndex));
//
//			sprintf_s(pszFileName, sizeof(pszFileName), "%s%s.yuv", strTrim.GetBuffer(128), pszIndex);
//
////			fp = fopen(pszFileName, "rb");
//			fopen_s(&fp, pszFileName, "rb");
//			if (fp != NULL)
//			{
//				CListBox* pListBox = (CListBox*)GetDlgItem(IDC_YUV_COMBINE_LIST_SRC);
//				pListBox->AddString(pszFileName);
////				m_listyuv.AddString(pszFileName);
//				fclose(fp);
//			}
//		}
//		m_dlgProgress.ShowWindow(SW_HIDE);
	}

	delete filenamesBuffer;
}

void CDlg_YUVCombine::OnBtnDo() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CFileDialog dlgSaveAs(FALSE, "yuv", "合并文件", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, _T("YUV文件(*.yuv)|*.yuv||"));
	if (dlgSaveAs.DoModal() == IDOK)
	{
		int nCount = m_listSourceFile.GetCount();
		FILE*	fp_src = NULL;
		FILE*	fp_dst = NULL;
		unsigned char	buf[2048];
		int				rdcount;
		char			strSrc[128];
		CString			strDstFile = dlgSaveAs.GetPathName();

		fopen_s(&fp_dst, strDstFile.GetBuffer(128), "wb");
		if (fp_dst != NULL)
		{
			m_dlgProgress.SetWindowText("合并文件");
			m_dlgProgress.ShowWindow(SW_SHOW);
			m_dlgProgress.SetPos(0);
			for (int i = 0; i < nCount; i++)
			{
				m_listSourceFile.GetText(i, strSrc);
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

				double ratio = 100.0 * i / nCount;
				m_dlgProgress.SetPos((int)round(ratio));
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

void CDlg_YUVCombine::OnBtnUp() 
{
	// TODO: Add your control notification handler code here
	
}


void CDlg_YUVCombine::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_YUV_COMBINE_LIST_SRC);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 5, 0);
	}
}
