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
	//DDX_Control(pDX, IDC_YUVPREVIEWDLG_SPLIT_BTN_WORKING_PREVIEW, m_splitbtnPreview);
	DDX_Control(pDX, IDC_LIST_YUV_PSNR, m_listPSNRReport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_YUVPreview, CDialog)
	//{{AFX_MSG_MAP(CDlg_YUVPreview)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN, OnBtnYuvReferenceFileOpen)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN, OnBtnYuvWorkingFileOpen)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW, OnBtnYuvFilePreview)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR, OnBtnYuvFilePSNR)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VIDEO_CONTAINER_REPORT_PLAY_EXIT, OnReportPlayThreadExit)
	ON_MESSAGE(WM_VIDEO_CONTAINER_REPORT_PSNR_EXIT, OnReportPSNRThreadExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVPreview message handlers

void CDlg_YUVPreview::Reset(void)
{
	//CButton* pBtn = NULL;
	CWnd* pWnd = NULL;

	//源文件设置
	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
	pWnd->EnableWindow(TRUE);
	//pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
	pWnd->EnableWindow(TRUE);
	//pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	pWnd->EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CDlg_YUVPreview::InitComboxForFrameWH(CComboBox* pCmbBox)
{
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

	nIndex = pCmbBox->AddString("640x480 [VGA]");
	pCmbBox->SetItemData(nIndex, (640 << 16) | 480);

	nIndex = pCmbBox->AddString("640x360 [nHD]");
	pCmbBox->SetItemData(nIndex, (640 << 16) | 360);

	nIndex = pCmbBox->AddString("1920x1080 [HD]");
	pCmbBox->SetItemData(nIndex, (1920 << 16) | 1080);
}

void CDlg_YUVPreview::InitComboxForSampleStructure(CComboBox* pCmbBox)
{
	pCmbBox->ResetContent();
	int nIndex = pCmbBox->AddString("IYUV [4:2:0 Y-U-V]");
	pCmbBox->SetItemData(nIndex, 0x56555949);

	nIndex = pCmbBox->AddString("I420 [4:2:0 Y-U-V]");
	pCmbBox->SetItemData(nIndex, 0x30323449);

	nIndex = pCmbBox->AddString("YV12 [4:2:0 Y-V-U]");
	pCmbBox->SetItemData(nIndex, 0x32315659);

	//nIndex = pCmbBox->AddString("YUY2");
	//pCmbBox->SetItemData(nIndex, 0x56555949);
}

void CDlg_YUVPreview::InitComboxForColorSpace(CComboBox* pCmbBox)
{
	pCmbBox->ResetContent();

	int nIndex = pCmbBox->AddString("ITU-R.BT.709");
	pCmbBox->SetItemData(nIndex, 709);

	nIndex = pCmbBox->AddString("ITU-R.BT.2020");
	pCmbBox->SetItemData(nIndex, 2020);
}

void CDlg_YUVPreview::InitComboxForFrameRate(CComboBox* pCmbBox)
{
	pCmbBox->ResetContent();

	int nIndex = pCmbBox->AddString("7.5P");
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

	nIndex = pCmbBox->AddString("50P");
	pCmbBox->SetItemData(nIndex, 50000);

	nIndex = pCmbBox->AddString("60P");
	pCmbBox->SetItemData(nIndex, 60000);

	nIndex = pCmbBox->AddString("100P");
	pCmbBox->SetItemData(nIndex, 100000);

	nIndex = pCmbBox->AddString("120P");
	pCmbBox->SetItemData(nIndex, 120000);
}

BOOL CDlg_YUVPreview::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CComboBox*  pCmbBox;
	CWnd*	    pWnd;
	

	m_dlgVideo.Create(IDD_SHOW_VIDEO_SCREEN, this);
	m_dlgVideo.ShowWindow(SW_HIDE);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
	InitComboxForFrameWH(pCmbBox);
	pCmbBox->SetCurSel(2);

	//setup the candidate sample structure
	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_SAMPLESTRUCTURE);
	InitComboxForSampleStructure(pCmbBox);
	pCmbBox->SetCurSel(0);

	//setup the color space
	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_COLORSPACE);
	InitComboxForColorSpace(pCmbBox);
	pCmbBox->SetCurSel(0);

	//设置播放帧率
	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);
	InitComboxForFrameRate(pCmbBox);
	pCmbBox->SetCurSel(5);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_DST_WH);
	InitComboxForFrameWH(pCmbBox);
	pCmbBox->SetCurSel(2);

	//setup the candidate sample structure
	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_DST_SAMPLESTRUCTURE);
	InitComboxForSampleStructure(pCmbBox);
	pCmbBox->SetCurSel(0);

	//setup the color space
	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_DST_COLORSPACE);
	InitComboxForColorSpace(pCmbBox);
	pCmbBox->SetCurSel(0);

	//设置播放帧率
	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_DST_FRAMERATE);
	InitComboxForFrameRate(pCmbBox);
	pCmbBox->SetCurSel(5);

	//pWnd = GetDlgItem(IDC_STATIC_FORMAT);
	//pWnd->SetWindowText(pszItem);

	m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	m_dlgProgress.ShowWindow(SW_HIDE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_STATIC_MONITORS);
	pWnd->SetWindowText("");

	m_listPSNRReport.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_listPSNRReport.InsertColumn(0, "缩略图", LVCFMT_CENTER, 100, -1);					//ICON

	m_listPSNRReport.InsertColumn(1, "帧序号", LVCFMT_CENTER, 60, -1);					//ICON
	m_listPSNRReport.InsertColumn(2, "PSNR(dB)", LVCFMT_CENTER, 100, -1);
	m_listPSNRReport.InsertColumn(3, "备注", LVCFMT_LEFT,200, -1);

	m_listPSNRReport.DeleteAllItems();
	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_YUVPreview::OnBtnYuvReferenceFileOpen() 
{
	// TODO: Add your control notification handler code here
	char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
	pWnd->GetWindowTextA(strBtn);

	if (strBtn == "选择文件")
	{
		strcpy_s(szFilter, sizeof(szFilter), "All Files (*.yuv)|*.yuv||");
		CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

		if (dlg.DoModal() == IDOK)
		{
			CString strPath = dlg.GetPathName();

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
			pWnd->SetWindowTextA(strPath);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
			pWnd->SetWindowText("关闭文件");

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(TRUE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
			pWnd->GetWindowText(strPath);

			if (strPath.GetLength() > 0)
			{
				pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
				pWnd->EnableWindow(TRUE);
			}
		}
	}
	else
	{
		pWnd->SetWindowText("选择文件");

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
		pWnd->SetWindowText("");

		CString strPath;

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		pWnd->GetWindowText(strPath);
		if (strPath.GetLength() > 0)
		{
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(FALSE);
		}

		m_listPSNRReport.DeleteAllItems();

		//m_YUVDecoder.Close();
	}
}

void CDlg_YUVPreview::OnBtnYuvWorkingFileOpen()
{
	// TODO: Add your control notification handler code here
	char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
	pWnd->GetWindowText(strBtn);

	if (strBtn == "选择文件")
	{
		strcpy_s(szFilter, sizeof(szFilter), "All Files (*.yuv)|*.yuv||");
		CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

		if (dlg.DoModal() == IDOK)
		{
			CString strPath = dlg.GetPathName();

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
			pWnd->SetWindowText(strPath);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
			pWnd->SetWindowText("关闭文件");

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(TRUE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
			pWnd->EnableWindow(TRUE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
			pWnd->GetWindowText(strPath);
			if (strPath.GetLength() > 0)
			{
				pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
				pWnd->EnableWindow(TRUE);
			}
		}
	}
	else
	{
		pWnd->SetWindowText("选择文件");

		CString strPath;

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		pWnd->SetWindowText("");

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
		pWnd->GetWindowText(strPath);
		if (strPath.GetLength() > 0)
		{
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(FALSE);
		}

		m_listPSNRReport.DeleteAllItems();
	}
}

void CDlg_YUVPreview::OnBtnYuvFilePreview()
{
	// TODO: Add your control notification handler code here
	//char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;
	CString strReferenceFile;
	CString strWorkingFile;
	CString strPath;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	pWnd->GetWindowText(strBtn);

	if (strBtn == "预览")
	{
		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
		pWnd->GetWindowText(strReferenceFile);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		pWnd->GetWindowText(strWorkingFile);

		if ((strReferenceFile.GetLength() > 0) || (strWorkingFile.GetLength() > 0))
		{
			if (strWorkingFile.GetLength() > 0)
			{
				strPath = strWorkingFile;
			}
			else
			{
				strPath = strReferenceFile;
			}

			int			nSel;
			CString		strBtn;
			CWnd*		pWnd = NULL;

			YUV_SOURCE_PARAM_t		stYUVParams;
			//获得参数
			memset(&stYUVParams, 0x00, sizeof(YUV_SOURCE_PARAM_t));

			CComboBox*				pCmbBox = NULL;

			CMFCButton* pMfcBtn = (CMFCButton*)GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pMfcBtn->SetWindowText("关闭");
			pMfcBtn->SetImage(IDB_BITMAP_STOP_48PIXEL);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
			pWnd->EnableWindow(FALSE);

			int luma_width = -1;
			int luma_height = -1;
			ParseWandH(strPath.GetBuffer(128), &luma_width, &luma_height);

			pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
			if ((luma_width == -1) || (luma_height == -1))
			{
				CString strFile = strPath;
				strFile.MakeUpper();

				if (strFile.Find("CIF") >= 0)
				{
					pCmbBox->SetCurSel(1);			//CIF
				}

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
			}
			else
			{
				stYUVParams.luma_width = luma_width;
				stYUVParams.luma_height = luma_height;

				CString strWH;
				CString strItem;
				strWH.Format("%dx%d", luma_width, luma_height);

				int nItemCount = pCmbBox->GetCount();
				for (int i = 0; i < nItemCount; i++)
				{
					pCmbBox->GetLBText(i, strItem);
					if (strItem.Find(strWH) >= 0)
					{
						pCmbBox->SetCurSel(i);
						break;
					}
				}
			}

			pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_SAMPLESTRUCTURE);
			nSel = pCmbBox->GetCurSel();
			if (nSel != CB_ERR)
			{
				stYUVParams.dwFourCC = pCmbBox->GetItemData(nSel);
			}
			else
			{
				stYUVParams.dwFourCC = 0x30323449;			//I420
			}

			pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_COLORSPACE);
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
			int fps = -1;
			ParseFPS(strPath.GetBuffer(128), &fps);

			pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);

			if (fps == -1)
			{
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
			}
			else
			{
				stYUVParams.framerate = fps;

				CString strFPS;
				CString strItem;
				strFPS.Format("%dP", fps);

				int nItemCount = pCmbBox->GetCount();
				for (int i = 0; i < nItemCount; i++)
				{
					pCmbBox->GetLBText(i, strItem);
					if (strItem.Find(strFPS) >= 0)
					{
						pCmbBox->SetCurSel(i);
						break;
					}
				}
			}

			UpdateData(FALSE);

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

		//pWnd->SetWindowText("预览");
		CMFCButton* pMfcBtn = (CMFCButton*)GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
		pMfcBtn->SetWindowText("预览");
		pMfcBtn->SetImage(IDB_BITMAP_PREVIEW_48PIXEL);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
		pWnd->EnableWindow(TRUE);

		//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		//pWnd->GetWindowText(strPath);
		//if (strPath.GetLength() > 0)
		//{
		//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
		//	pWnd->EnableWindow(TRUE);
		//}

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
		pWnd->GetWindowText(strPath);
		if (strPath.GetLength() > 0)
		{
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
			pWnd->EnableWindow(TRUE);
		}

		//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		//pWnd->SetWindowText("");

		m_YUVDecoder.Close();

		UpdateData(FALSE);
	}
}

void CDlg_YUVPreview::OnBtnYuvFilePSNR()
{
	// TODO: Add your control notification handler code here

	//char szFilter[128];
	CWnd*	pWnd = NULL;
	//CString strBtn;
	CString strReferenceFile;
	CString strWorkingFile;
	CString strTitle;

	//UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
	pWnd->GetWindowText(strReferenceFile);

	char* path = strReferenceFile.GetBuffer();
	char* reference_filename = strrchr(path, '\\');
	if (reference_filename == NULL)
	{
		reference_filename = path;
	}
	else
	{
		reference_filename++;
	}

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
	pWnd->GetWindowText(strWorkingFile);

	path = strReferenceFile.GetBuffer();
	char* working_filename = strrchr(path, '\\');

	if (working_filename == NULL)
	{
		working_filename = path;
	}
	else
	{
		working_filename++;
	}

	strTitle.Format("PSNR: %s VS %s", reference_filename, working_filename);

	m_dlgProgress.SetTitle(strTitle);
	m_dlgProgress.ShowWindow(SW_SHOW);

	::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)YUV_PSNR_Thread, this, 0, 0);

	//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	//pWnd->GetWindowText(strBtn);

	//if (strBtn == "预览")
	//{
	//	//strcpy_s(szFilter, sizeof(szFilter), "All Files (*.yuv)|*.yuv||");
	//	//CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);
	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
	//	pWnd->GetWindowText(strReferenceFile);

	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
	//	pWnd->GetWindowText(strWorkingFile);

	//	if ((strReferenceFile.GetLength() > 0) || (strWorkingFile.GetLength() > 0))
	//	{
	//		if (strWorkingFile.GetLength() > 0)
	//		{
	//			strPath = strWorkingFile;
	//		}
	//		else
	//		{
	//			strPath = strReferenceFile;
	//		}

	//		int			nSel;
	//		CString		strBtn;
	//		CWnd*		pWnd = NULL;

	//		YUV_SOURCE_PARAM_t		stYUVParams;
	//		//获得参数
	//		memset(&stYUVParams, 0x00, sizeof(YUV_SOURCE_PARAM_t));

	//		CComboBox*				pCmbBox = NULL;

	//		CMFCButton* pMfcBtn = (CMFCButton*)GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	//		pMfcBtn->SetWindowText("关闭");
	//		pMfcBtn->SetImage(IDB_BITMAP_STOP_48PIXEL);

	//		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
	//		pWnd->EnableWindow(FALSE);

	//		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
	//		pWnd->EnableWindow(FALSE);

	//		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
	//		pWnd->EnableWindow(FALSE);

	//		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
	//		pWnd->EnableWindow(FALSE);

	//		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	//		pWnd->EnableWindow(FALSE);

	//		int luma_width = -1;
	//		int luma_height = -1;
	//		ParseWandH(strPath.GetBuffer(128), &luma_width, &luma_height);

	//		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
	//		if ((luma_width == -1) || (luma_height == -1))
	//		{
	//			CString strFile = strPath;
	//			strFile.MakeUpper();

	//			if (strFile.Find("CIF") >= 0)
	//			{
	//				pCmbBox->SetCurSel(1);			//CIF
	//			}

	//			nSel = pCmbBox->GetCurSel();
	//			if (nSel != CB_ERR)
	//			{
	//				DWORD dwItemData = pCmbBox->GetItemData(nSel);
	//				stYUVParams.luma_width = ((dwItemData & 0xffff0000) >> 16);
	//				stYUVParams.luma_height = (dwItemData & 0x0000ffff);
	//			}
	//			else
	//			{
	//				stYUVParams.luma_width = 352;
	//				stYUVParams.luma_height = 288;
	//			}
	//		}
	//		else
	//		{
	//			stYUVParams.luma_width = luma_width;
	//			stYUVParams.luma_height = luma_height;

	//			CString strWH;
	//			CString strItem;
	//			strWH.Format("%dx%d", luma_width, luma_height);

	//			int nItemCount = pCmbBox->GetCount();
	//			for (int i = 0; i < nItemCount; i++)
	//			{
	//				pCmbBox->GetLBText(i, strItem);
	//				if (strItem.Find(strWH) >= 0)
	//				{
	//					pCmbBox->SetCurSel(i);
	//					break;
	//				}
	//			}
	//		}

	//		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_SAMPLESTRUCTURE);
	//		nSel = pCmbBox->GetCurSel();
	//		if (nSel != CB_ERR)
	//		{
	//			stYUVParams.dwFourCC = pCmbBox->GetItemData(nSel);
	//		}
	//		else
	//		{
	//			stYUVParams.dwFourCC = 0x30323449;			//I420
	//		}

	//		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_COLORSPACE);
	//		nSel = pCmbBox->GetCurSel();
	//		if (nSel != CB_ERR)
	//		{
	//			stYUVParams.nColorSpace = pCmbBox->GetItemData(nSel);
	//		}
	//		else
	//		{
	//			stYUVParams.nColorSpace = 709;			//ITU-R.BT.709
	//		}

	//		//char strFrameRate[16];
	//		int fps = -1;
	//		ParseFPS(strPath.GetBuffer(128), &fps);

	//		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);

	//		if (fps == -1)
	//		{
	//			nSel = pCmbBox->GetCurSel();
	//			if (nSel != CB_ERR)
	//			{
	//				int frame_rate_x_1000 = (int)(pCmbBox->GetItemData(nSel));
	//				stYUVParams.framerate = frame_rate_x_1000 / 1000.0;
	//			}
	//			else
	//			{
	//				stYUVParams.framerate = 50;
	//			}
	//		}
	//		else
	//		{
	//			stYUVParams.framerate = fps;

	//			CString strFPS;
	//			CString strItem;
	//			strFPS.Format("%dP", fps);

	//			int nItemCount = pCmbBox->GetCount();
	//			for (int i = 0; i < nItemCount; i++)
	//			{
	//				pCmbBox->GetLBText(i, strItem);
	//				if (strItem.Find(strFPS) >= 0)
	//				{
	//					pCmbBox->SetCurSel(i);
	//					break;
	//				}
	//			}
	//		}

	//		UpdateData(FALSE);

	//		stYUVParams.quantizationBits = 8;			//default value chendelin 2019.2.15

	//		m_YUVDecoder.Open((STREAM_FILE | YUV_FILE_YUV), strPath.GetBuffer(128), &stYUVParams);

	//		m_dlgVideo.AttachVideoDecoder(&m_YUVDecoder);

	//		//在第二屏全屏播放
	//		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//		if (pMainFrame->m_rectSecondDesktop.right > pMainFrame->m_rectSecondDesktop.left)
	//		{
	//			m_dlgVideo.MoveWindow(&(pMainFrame->m_rectSecondDesktop));
	//		}
	//		else
	//		{
	//			m_dlgVideo.MoveWindow(&(pMainFrame->m_rectPrimaryDesktop));
	//		}

	//		m_dlgVideo.EnlargeClientAreaToFullScreen();
	//		m_dlgVideo.ShowWindow(SW_SHOW);
	//	}
	//}
	//else
	//{
	//	if (m_dlgVideo.IsWindowVisible())
	//	{
	//		m_dlgVideo.StopVideoPlayThread();

	//		m_dlgVideo.ShowWindow(SW_HIDE);
	//		m_dlgVideo.DetachVideoDecoder(&m_YUVDecoder);
	//	}

	//	//pWnd->SetWindowText("预览");
	//	CMFCButton* pMfcBtn = (CMFCButton*)GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	//	pMfcBtn->SetWindowText("预览");
	//	pMfcBtn->SetImage(IDB_BITMAP_PREVIEW_48PIXEL);

	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
	//	pWnd->EnableWindow(TRUE);

	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
	//	pWnd->EnableWindow(TRUE);

	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PROCESS);
	//	pWnd->EnableWindow(TRUE);

	//	//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
	//	//pWnd->GetWindowText(strPath);
	//	//if (strPath.GetLength() > 0)
	//	//{
	//	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	//	//	pWnd->EnableWindow(TRUE);
	//	//}

	//	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
	//	pWnd->GetWindowText(strPath);
	//	if (strPath.GetLength() > 0)
	//	{
	//		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	//		pWnd->EnableWindow(TRUE);
	//	}

	//	//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
	//	//pWnd->SetWindowText("");

	//	m_YUVDecoder.Close();

	//	UpdateData(FALSE);
	//}
}

BOOL CDlg_YUVPreview::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dlgVideo.StopVideoPlayThread();
	m_dlgVideo.DetachVideoDecoder(&m_YUVDecoder);

	m_dlgVideo.DestroyWindow();

	return CDialog::DestroyWindow();
}

LRESULT CDlg_YUVPreview::OnReportPlayThreadExit(WPARAM wParam, LPARAM lParam)
{
	//OnBtnFileOpenOrClose();
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW, BN_CLICKED), NULL);

	return 0;
}

LRESULT CDlg_YUVPreview::OnReportPSNRThreadExit(WPARAM wParam, LPARAM lParam)
{
	m_dlgProgress.ShowWindow(SW_HIDE);
	CWnd* pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	pWnd->EnableWindow(TRUE);

	return 0;
}

void CDlg_YUVPreview::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_STATIC_MONITORS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 5, 0);
	}

	pWnd = GetDlgItem(IDC_LIST_YUV_PSNR);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 5, cy - rect.top - 5, 0);
	}
}

void CDlg_YUVPreview::ParseWandH(char* path, int *w, int *h) 
{
	//FIX
	if ((path != NULL) && (w != NULL) && (h != NULL))
	{
		char *p = NULL;
		char *filename = NULL;

		int width = -1, height = -1;

		filename = strrchr(path, '\\');

		if (filename == NULL)
			filename = path;

		for (int i = 0; i < strlen(filename); i++) {
			p = filename + i;
			char ch = filename[i];
			if (ch >= '0' && ch <= '9')
			{
				int temp = strtol(p, &p, 10);
				if (*p == 'x')
				{
					width = temp;

					p++;
					height = strtol(p, &p, 10);
					break;
				}
			}
		}

		*w = width;
		*h = height;
	}
}

void CDlg_YUVPreview::ParseFPS(char* path, int *pfps)
{
	//FIX
	if ((path != NULL) && (pfps != NULL))
	{
		char *p = NULL;
		char *filename = NULL;

		int fps = -1;

		filename = strrchr(path, '\\');

		if (filename == NULL)
			filename = path;

		for (int i = 0; i < strlen(filename); i++) {
			p = filename + i;
			char ch = filename[i];
			if (ch >= '0' && ch <= '9')
			{
				int tempfps = strtol(p, &p, 10);
				if (*p == 'f')
				{
					fps = tempfps;
					break;
				}
			}
		}

		*pfps = fps;
	}
}

void CDlg_YUVPreview::ReportFramePSNR(int nFrameNum, double psnr)
{
	int				nCount;
	char			pszText[128];

	nCount = m_listPSNRReport.GetItemCount();
	m_listPSNRReport.InsertItem(nCount, "");

	if (nFrameNum == -1)
	{
		sprintf_s(pszText, sizeof(pszText), "%s", "汇总");
		m_listPSNRReport.SetItemText(nCount, 1, pszText);

		sprintf_s(pszText, "%.2f", psnr);
		m_listPSNRReport.SetItemText(nCount, 2, pszText);
	}
	else
	{
		sprintf_s(pszText, sizeof(pszText), "%d", nFrameNum);
		m_listPSNRReport.SetItemText(nCount, 1, pszText);

		sprintf_s(pszText, "%.2f", psnr);
		m_listPSNRReport.SetItemText(nCount, 2, pszText);
	}

	m_listPSNRReport.EnsureVisible(nCount, 0);

}

uint32_t YUV_PSNR_Thread(PVOID pVoid)
{
	CDlg_YUVPreview* pdlg = (CDlg_YUVPreview*)pVoid;

	for (int i = 0; i < 100; i++)
	{
		pdlg->ReportFramePSNR(i, 0.02);
		pdlg->m_dlgProgress.SetPos(i);
		Sleep(100);
	}
	pdlg->ReportFramePSNR(-1, 0.08);

	::PostMessage(pdlg->GetSafeHwnd(), WM_VIDEO_CONTAINER_REPORT_PSNR_EXIT, 0, NULL);

	return 0;
}
