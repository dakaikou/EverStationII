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
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnFileOpenOrClose)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VIDEO_CONTAINER_REPORT_EXIT, OnReportPlayThreadExit)
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
	CComboBox*  pCmbBox;
	CWnd*	    pWnd;

	m_dlgVideo.Create(IDD_SHOW_VIDEO_SCREEN, this);
	m_dlgVideo.ShowWindow(SW_HIDE);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_WH);
	pCmbBox->ResetContent();
	
	//setup the candidate width and height
	int nIndex = pCmbBox->AddString("176x144 [QCIF]");
	pCmbBox->SetItemData(nIndex, (176 << 16) | 144);

	nIndex = pCmbBox->AddString("352x288 [CIF]");
	pCmbBox->SetItemData(nIndex, (352 << 16) | 288);

	nIndex = pCmbBox->AddString("704x576 [4CIF]");
	pCmbBox->SetItemData(nIndex, (704 << 16) | 576);

	nIndex = pCmbBox->AddString("1408x1152 [16CIF]");
	pCmbBox->SetItemData(nIndex, (1408 << 16) | 1152);

	nIndex = pCmbBox->AddString("352x240 [SIF@NTSC]");
	pCmbBox->SetItemData(nIndex, (352 << 16) | 240);

	nIndex = pCmbBox->AddString("352x288 [SIF@PAL/SECAM]");
	pCmbBox->SetItemData(nIndex, (352 << 16) | 288);

	nIndex = pCmbBox->AddString("704x480 [4SIF@NTSC]");
	pCmbBox->SetItemData(nIndex, (704 << 16) | 480);

	nIndex = pCmbBox->AddString("704x576 [4SIF@PAL/SECAM]");
	pCmbBox->SetItemData(nIndex, (704 << 16) | 576);

	nIndex = pCmbBox->AddString("720x486 [D1@NTSC]");
	pCmbBox->SetItemData(nIndex, (720 << 16) | 486);

	nIndex = pCmbBox->AddString("720x576 [D1@PAL/SECAM]");
	pCmbBox->SetItemData(nIndex, (720 << 16) | 576);

	nIndex = pCmbBox->AddString("1920x1080 [HD]");
	pCmbBox->SetItemData(nIndex, (1920 << 16) | 1080);

	pCmbBox->SetCurSel(2);

	//setup the candidate sample structure
	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
	pCmbBox->ResetContent();
	nIndex = pCmbBox->AddString("IYUV [4:2:0 Y-U-V]");
	pCmbBox->SetItemData(nIndex, 0x56555949);

	nIndex = pCmbBox->AddString("I420 [4:2:0 Y-U-V]");
	pCmbBox->SetItemData(nIndex, 0x30323449);

	nIndex = pCmbBox->AddString("YV12 [4:2:0 Y-V-U]");
	pCmbBox->SetItemData(nIndex, 0x32315659);

	//nIndex = pCmbBox->AddString("YUY2");
	//pCmbBox->SetItemData(nIndex, 0x56555949);

	pCmbBox->SetCurSel(0);

	//setup the color space
	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_COLORSPACE);
	pCmbBox->ResetContent();

	nIndex = pCmbBox->AddString("ITU-R.BT.709");
	pCmbBox->SetItemData(nIndex, 709);

	nIndex = pCmbBox->AddString("ITU-R.BT.2020");
	pCmbBox->SetItemData(nIndex, 2020);

	pCmbBox->SetCurSel(0);

	//设置播放帧率
	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FRAMERATE);
	pCmbBox->ResetContent();

	nIndex = pCmbBox->AddString("7.5P");
	pCmbBox->SetItemData(nIndex, 7500);

	nIndex = pCmbBox->AddString("15P");
	pCmbBox->SetItemData(nIndex, 15000);

	nIndex = pCmbBox->AddString("24P");
	pCmbBox->SetItemData(nIndex, 24000);

	nIndex = pCmbBox->AddString("25P");
	pCmbBox->SetItemData(nIndex, 25000);

	nIndex = pCmbBox->AddString("29.97P");
	pCmbBox->SetItemData(nIndex, 29970);

	nIndex = pCmbBox->AddString("30P");
	pCmbBox->SetItemData(nIndex, 30000);

	nIndex = pCmbBox->AddString("60P");
	pCmbBox->SetItemData(nIndex, 60000);

	pCmbBox->SetCurSel(5);

	//pWnd = GetDlgItem(IDC_STATIC_FORMAT);
	//pWnd->SetWindowText(pszItem);

	//m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	//m_dlgProgress.ShowWindow(SW_HIDE);

	pWnd = GetDlgItem(IDC_STATIC_MONITORS);
	pWnd->SetWindowText("");

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_YUVPreview::OnBtnFileOpenOrClose() 
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

			YUV_SOURCE_PARAM_t		stYUVParams;
			//获得参数
			memset(&stYUVParams, 0x00, sizeof(YUV_SOURCE_PARAM_t));

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
				DWORD dwItemData = pCmbBox->GetItemData(nSel);
				stYUVParams.luma_width = ((dwItemData & 0xffff0000) >> 16);
				stYUVParams.luma_height = (dwItemData & 0x0000ffff);
			}
			else
			{
				stYUVParams.luma_width = 352;
				stYUVParams.luma_height = 288;
			}

			pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				stYUVParams.dwFourCC = pCmbBox->GetItemData(nSel);
			}
			else
			{
				stYUVParams.dwFourCC = 0x30323449;			//I420
			}

			pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_COLORSPACE);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				stYUVParams.nColorSpace = pCmbBox->GetItemData(nSel);
			}
			else
			{
				stYUVParams.nColorSpace = 709;			//ITU-R.BT.709
			}

			//char strFrameRate[16];
			pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FRAMERATE);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				int frame_rate_x_1000 = (int)(pCmbBox->GetItemData(nSel));
				stYUVParams.framerate = frame_rate_x_1000 / 1000.0;
			}
			else
			{
				stYUVParams.framerate = 50;
			}

			stYUVParams.quantizationBits = 8;			//default value chendelin 2019.2.15

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
			m_dlgVideo.StopVideoPlayThread();

			m_dlgVideo.ShowWindow(SW_HIDE);
			m_dlgVideo.DetachVideoDecoder(&m_YUVDecoder);
		}

		pWnd->SetWindowText("打开");

		pWnd = GetDlgItem(IDC_EDIT_YUV);
		pWnd->SetWindowText("");

		m_YUVDecoder.Close();
	}
}

BOOL CDlg_YUVPreview::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dlgVideo.DestroyWindow();

	return CDialog::DestroyWindow();
}

LRESULT CDlg_YUVPreview::OnReportPlayThreadExit(WPARAM wParam, LPARAM lParam)
{
	//OnBtnFileOpenOrClose();
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BTN_OPEN, BN_CLICKED), NULL);

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
