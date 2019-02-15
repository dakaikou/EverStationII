// Dlg_YUVPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_YUVPreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVPreview dialog
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_Video.h"

#include "..\MainFrm.h"
#include "..\Common\Dlg_VideoShowScreen.h"
#include "..\Common\GuiApi_MSG.h"
#include "..\Magic_YUV\GuiApi_YUV.h"

CDlg_YUVPreview::CDlg_YUVPreview(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_YUVPreview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_YUVPreview)
	//}}AFX_DATA_INIT
}


void CDlg_YUVPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_YUVPreview)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_YUVPreview, CDialog)
	//{{AFX_MSG_MAP(CDlg_YUVPreview)
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpenOrClose)
	ON_CBN_SELCHANGE(IDC_CMB_FOURCC, OnSelchangeCmbFourcc)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PLAY_THREAD_EXIT, OnPlayThreadExit)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVPreview message handlers

void CDlg_YUVPreview::Reset(void)
{
	CButton* pBtn = NULL;
	CWnd* pWnd = NULL;

	//源文件设置
	pWnd = GetDlgItem(IDC_EDIT_YUV);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	pWnd->EnableWindow(TRUE);
	pWnd->SetWindowText("打开");

	UpdateData(FALSE);
}

BOOL CDlg_YUVPreview::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int			i;
	int			nSel;
	char		pszItem[128];
	char		pszFourCC[5];
	CComboBox*  pCmbBox;
	CWnd*	    pWnd;
	CRect		rect(0, 0, 720, 576);

	m_dlgVideo.Create(IDD_SHOW_VIDEO_SCREEN, this);
	m_dlgVideo.ShowWindow(SW_HIDE);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_WH);
	pCmbBox->ResetContent();
	
	for (i = 0; i < VIDEO_FORMAT_MAX; i++)
	{
		VIDEO_get_width_and_height_info(i, NULL, pszItem, sizeof(pszItem));
		pCmbBox->AddString(pszItem);
	}
	pCmbBox->SetCurSel(VIDEO_4CIF);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
	pCmbBox->ResetContent();
	pCmbBox->AddString("IYUV");
	pCmbBox->AddString("I420");
	//	pCmbBox->AddString("YV16");
	pCmbBox->AddString("YV12");
	pCmbBox->AddString("YUY2");
	pCmbBox->SetCurSel(0);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		pCmbBox->GetLBText(nSel, pszFourCC);
		DecodeFourCC2Text(pszFourCC, pszItem, sizeof(pszItem));
	}

	//设置播放帧率
	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FRAMERATE);
	pCmbBox->ResetContent();

	pCmbBox->AddString("7.5P");
	pCmbBox->AddString("15P");
	pCmbBox->AddString("25P");
	pCmbBox->AddString("30P");
	pCmbBox->AddString("60P");
	pCmbBox->SetCurSel(3);

	pWnd = GetDlgItem(IDC_STATIC_FORMAT);
	pWnd->SetWindowText(pszItem);

	//m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	//m_dlgProgress.ShowWindow(SW_HIDE);

	pWnd = GetDlgItem(IDC_STATIC_MONITORS);
	pWnd->SetWindowText("");

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_YUVPreview::OnBtnOpenOrClose() 
{
	// TODO: Add your control notification handler code here
	char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	pWnd->GetWindowText(strBtn);

	if (strBtn == "打开")
	{
//		switch (m_nFileFormat)
//		{
//		case YUV_FILE_DY_DU_DV:
//			strcpy(szFilter, "Y Files (*.y)|*.y|U Files (*.u)|*.u|V Files (*.v)|*.v||");
//			break;
//		case YUV_FILE_DYUV:
//		case YUV_FILE_YUV:
		strcpy_s(szFilter, sizeof(szFilter), "All Files (*.yuv)|*.yuv||");
//			break;
//		}

		CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

		if (dlg.DoModal() == IDOK)
		{
			int			nSel;
			CString		strBtn;
			CWnd*		pWnd = NULL;

			WIDTH_HEIGHT_t			wh;
			YUV_SOURCE_PARAM_t		stYUVParams;
			CComboBox*				pCmbBox = NULL;

			CString strPath = dlg.GetPathName();

			pWnd = GetDlgItem(IDC_EDIT_YUV);
			pWnd->SetWindowText(strPath);

			pWnd = GetDlgItem(IDC_BTN_OPEN);
			pWnd->SetWindowText("关闭");

			pWnd = GetDlgItem(IDC_EDIT_YUV);
			pWnd->GetWindowText(strPath);

			pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_WH);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				VIDEO_get_width_and_height_info(nSel, &wh, NULL, 0);
			}
			else
			{
				wh.width = 352;
				wh.height = 288;
			}

			//获得参数
			memset(&stYUVParams, 0x00, sizeof(YUV_SOURCE_PARAM_t));

			pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				pCmbBox->GetLBText(nSel, stYUVParams.pszFourCC);
			}
			else
			{
				strcpy_s(stYUVParams.pszFourCC, sizeof(stYUVParams.pszFourCC), "YV12");
			}

			char strFrameRate[16];
			pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FRAMERATE);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				pCmbBox->GetLBText(nSel, strFrameRate);
			}

			stYUVParams.luma_width = wh.width;
			stYUVParams.luma_height = wh.height;
			stYUVParams.quantizationBits = 8;			//default value chendelin 2019.2.15

			if (strcmp(strFrameRate, "7.5P") == 0)
			{
				stYUVParams.framerate = 7.5;
			}
			else if (strcmp(strFrameRate, "15P") == 0)
			{
				stYUVParams.framerate = 15.0;
			}
			else if (strcmp(strFrameRate, "25P") == 0)
			{
				stYUVParams.framerate = 25.0;
			}
			else if (strcmp(strFrameRate, "30P") == 0)
			{
				stYUVParams.framerate = 30.0;
			}
			else if (strcmp(strFrameRate, "60P") == 0)
			{
				stYUVParams.framerate = 60.0;
			}
			else
			{
				stYUVParams.framerate = 60.0;
			}

			m_YUVDecoder.Open((STREAM_FILE | YUV_FILE_YUV), strPath.GetBuffer(128), &stYUVParams);

			m_dlgVideo.AttachVideoDecoder(&m_YUVDecoder);

			//在第二屏全屏播放
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if (pMainFrame->m_rectSecondDesktop.right > pMainFrame->m_rectSecondDesktop.left)
			{
				m_dlgVideo.MoveWindow(&(pMainFrame->m_rectSecondDesktop));
			}
			else
			{
				m_dlgVideo.MoveWindow(&(pMainFrame->m_rectPrimaryDesktop));
			}

			m_dlgVideo.EnlargeClientAreaToFullScreen();
			m_dlgVideo.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (m_dlgVideo.IsWindowVisible())
		{
			m_dlgVideo.ShowWindow(SW_HIDE);
			m_dlgVideo.DetachVideoDecoder(&m_YUVDecoder);
		}

		pWnd->SetWindowText("打开");

		pWnd = GetDlgItem(IDC_EDIT_YUV);
		pWnd->SetWindowText("");

		m_YUVDecoder.Close();
	}
}

void CDlg_YUVPreview::OnSelchangeCmbFourcc() 
{
	// TODO: Add your control notification handler code here
	int			nSel;
	CComboBox*	pCmbBox;
	CWnd*		pWnd;
	char		pszFourCC[5];
	char		pszItem[128];

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		pCmbBox->GetLBText(nSel, pszFourCC);
		DecodeFourCC2Text(pszFourCC, pszItem, sizeof(pszItem));
	}

	pWnd = GetDlgItem(IDC_STATIC_FORMAT);
	pWnd->SetWindowText(pszItem);
	
}

BOOL CDlg_YUVPreview::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dlgVideo.DestroyWindow();

	return CDialog::DestroyWindow();
}

LRESULT CDlg_YUVPreview::OnPlayThreadExit(WPARAM wParam, LPARAM lParam)
{
	OnBtnOpenOrClose();

	return 0;
}


void CDlg_YUVPreview::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_MONITORS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 5, 0);
	}
}
