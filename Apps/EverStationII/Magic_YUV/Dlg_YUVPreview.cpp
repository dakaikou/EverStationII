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
#include <fcntl.h>
#include <io.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_Video.h"

#include "..\MainFrm.h"
#include "..\Common\Dlg_VideoShowScreen.h"
#include "..\Common\GuiApi_MSG.h"
#include "..\Magic_YUV\GuiApi_YUV.h"

double psnr(uint8_t* reference, uint8_t* working, int width, int height);
uint32_t YUV_PSNR_Thread(PVOID pVoid);
uint32_t YUV_TransCode_Thread(PVOID pVoid);

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
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_SAVING_OPEN, OnBtnYuvSavingFileOpen)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW, OnBtnYuvFilePreview)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR, OnBtnYuvFileCalculatePSNR)
	ON_BN_CLICKED(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE, &CDlg_YUVPreview::OnBtnYuvFileTranscode)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VIDEO_CONTAINER_REPORT_PLAY_EXIT, OnReportPlayThreadExit)
	ON_MESSAGE(WM_VIDEO_CONTAINER_REPORT_PSNR_EXIT, OnReportPSNRThreadExit)
	ON_MESSAGE(WM_VIDEO_CONTAINER_REPORT_TRANSCODE_EXIT, OnReportTranscodeThreadExit)
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
	m_strReferenceFile = "";

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
	pWnd->SetWindowText("");
	m_strWorkingFile = "";

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_SAVING_FILE);
	pWnd->SetWindowText("");
	m_strSavingFile = "";

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
	pWnd->EnableWindow(TRUE);
	//pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
	pWnd->EnableWindow(TRUE);
	//pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
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
	m_listPSNRReport.InsertColumn(2, "PSNR_Y(dB)", LVCFMT_CENTER, 100, -1);
	m_listPSNRReport.InsertColumn(3, "PSNR_U(dB)", LVCFMT_CENTER, 100, -1);
	m_listPSNRReport.InsertColumn(4, "PSNR_V(dB)", LVCFMT_CENTER, 100, -1);
	m_listPSNRReport.InsertColumn(5, "备注", LVCFMT_LEFT,200, -1);

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
	CComboBox* pCmbBox = NULL;

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
	pWnd->GetWindowTextA(strBtn);

	if (strBtn == "选择文件")
	{
		strcpy_s(szFilter, sizeof(szFilter), "YUV Files (*.yuv)|*.yuv|All Files (*.*)|*.*||");
		CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

		if (dlg.DoModal() == IDOK)
		{
			m_strReferenceFile = dlg.GetPathName();

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
			pWnd->SetWindowTextA(m_strReferenceFile);

			char* path = m_strReferenceFile.GetBuffer();
			char* reference_filename = strrchr(path, '\\');
			if (reference_filename == NULL)
			{
				reference_filename = path;
			}
			else
			{
				reference_filename++;
			}

			int luma_width = -1;
			int luma_height = -1;
			ParseWandH(reference_filename, &luma_width, &luma_height);

			if ((luma_width == -1) || (luma_height == -1))
			{
				CString strFile = reference_filename;
				strFile.MakeUpper();

				if (strFile.Find("CIF") >= 0)
				{
					pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
					pCmbBox->SetCurSel(1);			//CIF
				}
			}
			else
			{
				CString strWH;
				CString strItem;
				strWH.Format("%dx%d", luma_width, luma_height);

				pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
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

			int fps = -1;
			ParseFPS(reference_filename, &fps);

			if (fps > 0)
			{
				CString strFPS;
				CString strItem;

				strFPS.Format("%dP", fps);
				pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);
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

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
			pWnd->SetWindowText("关闭文件");

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(TRUE);

			//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
			//pWnd->GetWindowText(strPath);

			if (m_strWorkingFile.GetLength() > 0)
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

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
		pWnd->SetWindowText("");
		m_strReferenceFile = "";

		//CString strPath;

		//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		//pWnd->GetWindowText(strPath);
		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
		if (m_strWorkingFile.GetLength() > 0)
		{
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd->EnableWindow(FALSE);
		}

		m_listPSNRReport.DeleteAllItems();
	}
}

void CDlg_YUVPreview::OnBtnYuvWorkingFileOpen()
{
	// TODO: Add your control notification handler code here
	char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;
	CComboBox* pCmbBox = NULL;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
	pWnd->GetWindowText(strBtn);

	if (strBtn == "选择文件")
	{
		strcpy_s(szFilter, sizeof(szFilter), "YUV Files (*.yuv)|*.yuv|All Files (*.*)|*.*||");
		CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

		if (dlg.DoModal() == IDOK)
		{
			m_strWorkingFile = dlg.GetPathName();
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
			pWnd->SetWindowText(m_strWorkingFile);

			char* path = m_strWorkingFile.GetBuffer();
			char* working_filename = strrchr(path, '\\');
			if (working_filename == NULL)
			{
				working_filename = path;
			}
			else
			{
				working_filename++;
			}

			int luma_width = -1;
			int luma_height = -1;
			ParseWandH(working_filename, &luma_width, &luma_height);

			if ((luma_width == -1) || (luma_height == -1))
			{
				CString strFile = working_filename;
				strFile.MakeUpper();

				if (strFile.Find("CIF") >= 0)
				{
					pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
					pCmbBox->SetCurSel(1);			//CIF
				}
			}
			else
			{
				CString strWH;
				CString strItem;
				strWH.Format("%dx%d", luma_width, luma_height);

				pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
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

			int fps = -1;
			ParseFPS(working_filename, &fps);

			if (fps > 0)
			{
				CString strFPS;
				CString strItem;

				strFPS.Format("%dP", fps);
				pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);
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

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
			pWnd->SetWindowText("关闭文件");

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pWnd->EnableWindow(TRUE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
			pWnd->EnableWindow(TRUE);

			//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
			//pWnd->GetWindowText(strPath);
			if (m_strReferenceFile.GetLength() > 0)
			{
				pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
				pWnd->EnableWindow(TRUE);
			}
		}
	}
	else
	{
		pWnd->SetWindowText("选择文件");

		//CString strPath;

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_WORKING_FILE);
		pWnd->SetWindowText("");

		//pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_REFERENCE_FILE);
		//pWnd->GetWindowText(strPath);
		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
		if (m_strReferenceFile.GetLength() > 0)
		{
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd->EnableWindow(FALSE);
		}

		m_listPSNRReport.DeleteAllItems();
	}

	UpdateData(FALSE);
}

void CDlg_YUVPreview::OnBtnYuvSavingFileOpen()
{
	char	szFilter[128];
	CWnd*	pWnd = NULL;

	strcpy_s(szFilter, sizeof(szFilter), "YUV Files (*.yuv)|*.yuv|All Files (*.*)|*.*||");
	CFileDialog dlg(FALSE, "yuv", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if (dlg.DoModal() == IDOK)
	{
		m_strSavingFile = dlg.GetPathName();
		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_SAVING_FILE);
		pWnd->SetWindowText(m_strSavingFile);
	}
}

void CDlg_YUVPreview::OnBtnYuvFilePreview()
{
	// TODO: Add your control notification handler code here
	//char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
	pWnd->GetWindowText(strBtn);

	if (strBtn == "预览")
	{
		if ((m_strReferenceFile.GetLength() > 0) || (m_strWorkingFile.GetLength() > 0))
		{
			CMFCButton* pMfcBtn = (CMFCButton*)GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
			pMfcBtn->SetWindowText("关闭");
			pMfcBtn->SetImage(IDB_BITMAP_STOP_48PIXEL);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
			pWnd->EnableWindow(FALSE);

			CheckSrcFrameParameters(&m_stSrcYUVParams);

			if (m_strReferenceFile.GetLength() > 0)
			{
				m_ReferenceYUVDecoder.Open((STREAM_FILE | YUV_FILE_YUV), m_strReferenceFile.GetBuffer(128), &m_stSrcYUVParams);
				m_dlgVideo.AttachVideoDecoder(&m_ReferenceYUVDecoder);
			}
			if (m_strWorkingFile.GetLength() > 0)
			{
				m_WorkingYUVDecoder.Open((STREAM_FILE | YUV_FILE_YUV), m_strWorkingFile.GetBuffer(128), &m_stSrcYUVParams);
				m_dlgVideo.AttachVideoDecoder(&m_WorkingYUVDecoder);
			}

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

			if (m_strReferenceFile.GetLength() > 0)
			{
				m_dlgVideo.DetachVideoDecoder(&m_ReferenceYUVDecoder);
				m_ReferenceYUVDecoder.Close();
			}
			if (m_strWorkingFile.GetLength() > 0)
			{
				m_dlgVideo.DetachVideoDecoder(&m_WorkingYUVDecoder);
				m_WorkingYUVDecoder.Close();
			}
		}

		CMFCButton* pMfcBtn = (CMFCButton*)GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_PREVIEW);
		pMfcBtn->SetWindowText("预览");
		pMfcBtn->SetImage(IDB_BITMAP_PREVIEW_48PIXEL);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_REFERENCE_OPEN);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_OPEN);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
		pWnd->EnableWindow(TRUE);

		if (m_strReferenceFile.GetLength() > 0)
		{
			pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
			pWnd->EnableWindow(TRUE);
		}
	}

	UpdateData(FALSE);
}

void CDlg_YUVPreview::CheckSrcFrameParameters(YUV_SERIAL_PARAM_t* pstYuvParams)
{
	if (pstYuvParams != NULL)
	{
		memset(pstYuvParams, 0x00, sizeof(YUV_SERIAL_PARAM_t));

		int				nSel;
		CComboBox*		pCmbBox = NULL;

		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			DWORD dwItemData = pCmbBox->GetItemData(nSel);
			pstYuvParams->luma_width = ((dwItemData & 0xffff0000) >> 16);
			pstYuvParams->luma_height = (dwItemData & 0x0000ffff);
		}
		else
		{
			pstYuvParams->luma_width = 352;
			pstYuvParams->luma_height = 288;
		}

		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_SAMPLESTRUCTURE);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			pstYuvParams->dwFourCC = pCmbBox->GetItemData(nSel);
		}
		else
		{
			pstYuvParams->dwFourCC = 0x30323449;			//I420
		}

		switch (pstYuvParams->dwFourCC)
		{
		case 0x56555949:		//IYUV
			pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
			pstYuvParams->chroma_height = (pstYuvParams->luma_height >> 1);

			break;

		case 0x30323449:		//I420
			pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
			pstYuvParams->chroma_height = (pstYuvParams->luma_height >> 1);

			break;

		case 0x32315659:		//YV12
			pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
			pstYuvParams->chroma_height = pstYuvParams->luma_height;

			break;

		default:
			pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
			pstYuvParams->chroma_height = (pstYuvParams->luma_height >> 1);
			break;
		}
		//else if (strcmp(m_VidDecodeInfo.source_pszFourCC, "YUY2") == 0)
		//{
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_2;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = m_VidDecodeInfo.source_luma_height;
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		//}
		//else
		//{
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		//}

		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_COLORSPACE);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			pstYuvParams->nColorSpace = pCmbBox->GetItemData(nSel);
		}
		else
		{
			pstYuvParams->nColorSpace = 709;			//ITU-R.BT.709
		}

		pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			int frame_rate_x_1000 = (int)(pCmbBox->GetItemData(nSel));
			pstYuvParams->framerate = frame_rate_x_1000 / 1000.0;
		}
		else
		{
			pstYuvParams->framerate = 50;
		}

		pstYuvParams->quantizationBits = 8;
	}
}

void CDlg_YUVPreview::CheckDstFrameParameters(YUV_SERIAL_PARAM_t* pstYuvParams)
{
	memset(pstYuvParams, 0x00, sizeof(YUV_SERIAL_PARAM_t));

	int				nSel;
	CComboBox*		pCmbBox = NULL;

	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_WH);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		DWORD dwItemData = pCmbBox->GetItemData(nSel);
		pstYuvParams->luma_width = ((dwItemData & 0xffff0000) >> 16);
		pstYuvParams->luma_height = (dwItemData & 0x0000ffff);
	}
	else
	{
		pstYuvParams->luma_width = 352;
		pstYuvParams->luma_height = 288;
	}

	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_SAMPLESTRUCTURE);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		pstYuvParams->dwFourCC = pCmbBox->GetItemData(nSel);
	}
	else
	{
		pstYuvParams->dwFourCC = 0x30323449;			//I420
	}

	switch (pstYuvParams->dwFourCC)
	{
	case 0x56555949:		//IYUV
		pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
		pstYuvParams->chroma_height = (pstYuvParams->luma_height >> 1);

		break;

	case 0x30323449:		//I420
		pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
		pstYuvParams->chroma_height = (pstYuvParams->luma_height >> 1);

		break;

	case 0x32315659:		//YV12
		pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
		pstYuvParams->chroma_height = pstYuvParams->luma_height;

		break;

	default:
		pstYuvParams->chroma_width = (pstYuvParams->luma_width >> 1);
		pstYuvParams->chroma_height = (pstYuvParams->luma_height >> 1);
		break;
	}
	//else if (strcmp(m_VidDecodeInfo.source_pszFourCC, "YUY2") == 0)
	//{
	//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_2;
	//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
	//	m_VidDecodeInfo.source_chroma_height = m_VidDecodeInfo.source_luma_height;
	//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
	//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
	//}
	//else
	//{
	//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
	//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
	//	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
	//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
	//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
	//}

	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_COLORSPACE);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		pstYuvParams->nColorSpace = pCmbBox->GetItemData(nSel);
	}
	else
	{
		pstYuvParams->nColorSpace = 709;			//ITU-R.BT.709
	}

	pCmbBox = (CComboBox*)GetDlgItem(IDC_YUVPREVIEWDLG_CMB_SRC_FRAMERATE);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		int frame_rate_x_1000 = (int)(pCmbBox->GetItemData(nSel));
		pstYuvParams->framerate = frame_rate_x_1000 / 1000.0;
	}
	else
	{
		pstYuvParams->framerate = 50;
	}

	pstYuvParams->quantizationBits = 8;
}

void CDlg_YUVPreview::OnBtnYuvFileCalculatePSNR()
{
	// TODO: Add your control notification handler code here

	CWnd*	pWnd = NULL;
	CString strTitle;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_PSNR);
	pWnd->EnableWindow(FALSE);

	char* path = m_strReferenceFile.GetBuffer();
	char* reference_filename = strrchr(path, '\\');
	if (reference_filename == NULL)
	{
		reference_filename = path;
	}
	else
	{
		reference_filename++;
	}

	path = m_strWorkingFile.GetBuffer();
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

	CheckSrcFrameParameters(&m_stSrcYUVParams);

	::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)YUV_PSNR_Thread, this, 0, 0);
}

BOOL CDlg_YUVPreview::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dlgVideo.StopVideoPlayThread();
	m_dlgVideo.DetachVideoDecoder(&m_ReferenceYUVDecoder);
	m_dlgVideo.DetachVideoDecoder(&m_WorkingYUVDecoder);

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

LRESULT CDlg_YUVPreview::OnReportTranscodeThreadExit(WPARAM wParam, LPARAM lParam)
{
	m_dlgProgress.ShowWindow(SW_HIDE);
	CWnd* pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
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

void CDlg_YUVPreview::ReportFramePSNR(int nFrameNum, double Ypsnr, double Upsnr, double Vpsnr)
{
	int				nCount;
	char			pszText[128];

	nCount = m_listPSNRReport.GetItemCount();
	m_listPSNRReport.InsertItem(nCount, "");

	if (nFrameNum == -1)
	{
		sprintf_s(pszText, sizeof(pszText), "%s", "平均");
		m_listPSNRReport.SetItemText(nCount, 1, pszText);

		sprintf_s(pszText, "%.2f", Ypsnr);
		m_listPSNRReport.SetItemText(nCount, 2, pszText);

		sprintf_s(pszText, "%.2f", Upsnr);
		m_listPSNRReport.SetItemText(nCount, 3, pszText);

		sprintf_s(pszText, "%.2f", Vpsnr);
		m_listPSNRReport.SetItemText(nCount, 4, pszText);
	}
	else
	{
		sprintf_s(pszText, sizeof(pszText), "%d", nFrameNum);
		m_listPSNRReport.SetItemText(nCount, 1, pszText);

		sprintf_s(pszText, "%.2f", Ypsnr);
		m_listPSNRReport.SetItemText(nCount, 2, pszText);

		sprintf_s(pszText, "%.2f", Upsnr);
		m_listPSNRReport.SetItemText(nCount, 3, pszText);

		sprintf_s(pszText, "%.2f", Vpsnr);
		m_listPSNRReport.SetItemText(nCount, 4, pszText);
	}

	m_listPSNRReport.EnsureVisible(nCount, 0);
}

uint32_t YUV_PSNR_Thread(PVOID pVoid)
{
	CDlg_YUVPreview* pdlg = (CDlg_YUVPreview*)pVoid;

	YUV_SERIAL_PARAM_t* pYuvSerialParam = &(pdlg->m_stSrcYUVParams);
	char* pszReferenceFile = pdlg->m_strReferenceFile.GetBuffer();
	char* pszWorkingFile = pdlg->m_strWorkingFile.GetBuffer();

	int luma_buf_size = pYuvSerialParam->luma_width * pYuvSerialParam->luma_height * pYuvSerialParam->quantizationBits / 8;
	int chroma_buf_size = pYuvSerialParam->chroma_width * pYuvSerialParam->chroma_height * pYuvSerialParam->quantizationBits / 8;
	int frame_buf_size = luma_buf_size + chroma_buf_size + chroma_buf_size;

	uint8_t* pucReferenceFrameBuf = (uint8_t*)malloc(frame_buf_size);
	memset(pucReferenceFrameBuf, 0x00, frame_buf_size);

	uint8_t* pucReferenceY = pucReferenceFrameBuf;
	uint8_t* pucReferenceU = pucReferenceFrameBuf + luma_buf_size;
	uint8_t* pucReferenceV = pucReferenceFrameBuf + luma_buf_size + chroma_buf_size;

	uint8_t* pucWorkingFrameBuf = (uint8_t*)malloc(frame_buf_size);
	memset(pucWorkingFrameBuf, 0x00, frame_buf_size);

	uint8_t* pucWorkingY = pucWorkingFrameBuf;
	uint8_t* pucWorkingU = pucWorkingFrameBuf + luma_buf_size;
	uint8_t* pucWorkingV = pucWorkingFrameBuf + luma_buf_size + chroma_buf_size;

	int hReferenceFile, hWorkingFile;
	_sopen_s(&hReferenceFile, pszReferenceFile, _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
	_lseeki64(hReferenceFile, 0, SEEK_END);
	int64_t nReferenceFileTotalSize = _telli64(hReferenceFile);
	int m_nReferenceTotalFrameCount = (int)(nReferenceFileTotalSize / frame_buf_size);
	_lseeki64(hReferenceFile, 0, SEEK_SET);

	_sopen_s(&hWorkingFile, pszWorkingFile, _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
	_lseeki64(hWorkingFile, 0, SEEK_END);
	int64_t nWorkingFileTotalSize = _telli64(hWorkingFile);
	int nWorkingTotalFrameCount = (int)(nWorkingFileTotalSize / frame_buf_size);
	_lseeki64(hWorkingFile, 0, SEEK_SET);

	int nFrameCount = min(m_nReferenceTotalFrameCount, nWorkingTotalFrameCount);

	double sumY = 0.0, sumU = 0.0, sumV = 0.0;
	for (int frame_num = 0; frame_num < nFrameCount; frame_num++)
	{
		int rdsize = _read(hReferenceFile, pucReferenceFrameBuf, frame_buf_size);
		assert(rdsize == frame_buf_size);

		rdsize = _read(hWorkingFile, pucWorkingFrameBuf, frame_buf_size);
		assert(rdsize == frame_buf_size);

		double Ypsnr = psnr(pucReferenceY, pucWorkingY, pYuvSerialParam->luma_width, pYuvSerialParam->luma_height);
		double Upsnr = psnr(pucReferenceU, pucWorkingU, pYuvSerialParam->chroma_width, pYuvSerialParam->chroma_height);
		double Vpsnr = psnr(pucReferenceV, pucWorkingV, pYuvSerialParam->chroma_width, pYuvSerialParam->chroma_height);
		sumY += Ypsnr;
		sumU += Upsnr;
		sumV += Vpsnr;

		pdlg->ReportFramePSNR(frame_num, Ypsnr, Upsnr, Vpsnr);

		int percent = (int)(100.0 * frame_num / nFrameCount);
		pdlg->m_dlgProgress.SetPos(percent);
	}
	double aveY = sumY / nFrameCount;
	double aveU = sumU / nFrameCount;
	double aveV = sumV / nFrameCount;

	pdlg->ReportFramePSNR(-1, aveY, aveU, aveV);

	free(pucReferenceFrameBuf);
	free(pucWorkingFrameBuf);

	_close(hReferenceFile);
	_close(hWorkingFile);

	::PostMessage(pdlg->GetSafeHwnd(), WM_VIDEO_CONTAINER_REPORT_PSNR_EXIT, 0, NULL);

	return 0;
}

uint32_t YUV_TransCode_Thread(PVOID pVoid)
{
	CDlg_YUVPreview* pdlg = (CDlg_YUVPreview*)pVoid;

	char* pszWorkingFile = pdlg->m_strWorkingFile.GetBuffer();
	char* pszSavingFile = pdlg->m_strSavingFile.GetBuffer();

	YUV_SERIAL_PARAM_t* pSrcSerialParam = &(pdlg->m_stSrcYUVParams);

	int src_luma_buf_size = pSrcSerialParam->luma_width * pSrcSerialParam->luma_height * pSrcSerialParam->quantizationBits / 8;
	int src_chroma_buf_size = pSrcSerialParam->chroma_width * pSrcSerialParam->chroma_height * pSrcSerialParam->quantizationBits / 8;
	int src_frame_buf_size = src_luma_buf_size + src_chroma_buf_size + src_chroma_buf_size;

	uint8_t* pucWorkingFrameBuf = (uint8_t*)malloc(src_frame_buf_size);
	memset(pucWorkingFrameBuf, 0x00, src_frame_buf_size);

	uint8_t* pucWorkingY = pucWorkingFrameBuf;
	uint8_t* pucWorkingU = pucWorkingFrameBuf + src_luma_buf_size;
	uint8_t* pucWorkingV = pucWorkingFrameBuf + src_luma_buf_size + src_chroma_buf_size;

	YUV_SERIAL_PARAM_t* pDstSerialParam = &(pdlg->m_stDstYUVParams);

	int dst_luma_buf_size = pDstSerialParam->luma_width * pDstSerialParam->luma_height * pDstSerialParam->quantizationBits / 8;
	int dst_chroma_buf_size = pDstSerialParam->chroma_width * pDstSerialParam->chroma_height * pDstSerialParam->quantizationBits / 8;
	int dst_frame_buf_size = dst_luma_buf_size + dst_chroma_buf_size + dst_chroma_buf_size;

	uint8_t* pucSavingFrameBuf = (uint8_t*)malloc(dst_frame_buf_size);
	memset(pucSavingFrameBuf, 0x00, dst_frame_buf_size);

	uint8_t* pucSavingY = pucSavingFrameBuf;
	uint8_t* pucSavingU = pucSavingFrameBuf + dst_luma_buf_size;
	uint8_t* pucSavingV = pucSavingFrameBuf + dst_luma_buf_size + dst_chroma_buf_size;

	int hSavingFile, hWorkingFile;
	_sopen_s(&hSavingFile, pszSavingFile, _O_CREAT | _O_BINARY | _O_WRONLY, _SH_DENYWR, _S_IWRITE);

	_sopen_s(&hWorkingFile, pszWorkingFile, _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
	_lseeki64(hWorkingFile, 0, SEEK_END);
	int64_t nWorkingFileTotalSize = _telli64(hWorkingFile);
	int nWorkingTotalFrameCount = (int)(nWorkingFileTotalSize / src_frame_buf_size);
	_lseeki64(hWorkingFile, 0, SEEK_SET);

	for (int frame_num = 0; frame_num < nWorkingTotalFrameCount; frame_num++)
	{
		int rdsize = _read(hWorkingFile, pucWorkingFrameBuf, src_frame_buf_size);
		assert(rdsize == src_frame_buf_size);

		memcpy(pucSavingFrameBuf, pucWorkingFrameBuf, rdsize);

		int wrsize = _write(hSavingFile, pucSavingFrameBuf, dst_frame_buf_size);
		assert(wrsize == dst_frame_buf_size);

		int percent = (int)(100.0 * frame_num / nWorkingTotalFrameCount);
		pdlg->m_dlgProgress.SetPos(percent);
	}

	free(pucSavingFrameBuf);
	free(pucWorkingFrameBuf);

	_close(hSavingFile);
	_close(hWorkingFile);

	::PostMessage(pdlg->GetSafeHwnd(), WM_VIDEO_CONTAINER_REPORT_TRANSCODE_EXIT, 0, NULL);

	return 0;
}

double psnr(uint8_t* reference, uint8_t* working, int width, int height)
{
	unsigned char*  pOrg = reference;
	unsigned char*  pRec = working;
	double          ssd = 0;
	int             diff;

	for (int r = 0; r < height; r++)
	{
		for (int c = 0; c < width; c++)
		{
			diff = pRec[c] - pOrg[c];
			ssd += (double)(diff * diff);
		}
		pRec += width;
		pOrg += width;
	}

	if (ssd == 0.0)
	{
		return 99.99;
	}
	return (10.0 * log10((double)width * (double)height * 65025.0 / ssd));
}


void CDlg_YUVPreview::OnBtnYuvFileTranscode()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd*	pWnd = NULL;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_EDIT_SAVING_FILE);
	pWnd->GetWindowText(m_strSavingFile);

	if (m_strSavingFile.GetLength() > 0)
	{
		CString strTitle;

		char* path = m_strWorkingFile.GetBuffer();
		char* working_filename = strrchr(path, '\\');

		if (working_filename == NULL)
		{
			working_filename = path;
		}
		else
		{
			working_filename++;
		}

		path = m_strSavingFile.GetBuffer();
		char* saving_filename = strrchr(path, '\\');
		if (saving_filename == NULL)
		{
			saving_filename = path;
		}
		else
		{
			saving_filename++;
		}

		strTitle.Format("格式转换: %s -> %s", working_filename, saving_filename);

		m_dlgProgress.SetTitle(strTitle);
		m_dlgProgress.ShowWindow(SW_SHOW);

		CheckSrcFrameParameters(&m_stSrcYUVParams);
		CheckDstFrameParameters(&m_stDstYUVParams);

		pWnd = GetDlgItem(IDC_YUVPREVIEWDLG_MFCBUTTON_WORKING_TRANSCODE);
		pWnd->EnableWindow(FALSE);

		::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)YUV_TransCode_Thread, this, 0, 0);
	}
	else
	{
		AfxMessageBox("目标文件不能为空!", MB_OK);
	}
}
