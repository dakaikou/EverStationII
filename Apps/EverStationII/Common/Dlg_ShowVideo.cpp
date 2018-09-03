// Dlg_M2VDecoder.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_ShowVideo.h"

#include "../Magic_TS/TSMagic_GuiApi_MSG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_ShowVideo dialog

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
//#include "MiddleWare_ESDecoder/Include/Decoder_MPEG2_PS.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_MPEG2.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_MPEG2.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_H264.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_AVS.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AAC.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AC3.h"
#include "MiddleWare/MiddleWare_ESDecoder/Include/VideoDecoder_YUV.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_WAVE.h"

CDlg_ShowVideo::CDlg_ShowVideo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_ShowVideo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_ShowVideo)
	//}}AFX_DATA_INIT
	m_nVidStreamType = STREAM_UNKNOWN;
	m_nAudStreamType = STREAM_UNKNOWN;
	m_nGraphWidth = 0;
	m_nGraphHeight = 0;
	m_nMbWidth = 16;
	m_nMbHeight = 16;

	m_bPlaying = 0;
	m_bIsAudio = 0;
//	m_pdlgInfo = NULL;

	m_pVidDecoder = NULL;
	m_pAudDecoder = NULL;

	m_fViewRatio = 1.0f;
	
	strcpy_s(m_pszFourCC, sizeof(m_pszFourCC), "");
}


void CDlg_ShowVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_ShowVideo)
	DDX_Control(pDX, IDC_SLIDER_FILE, m_sldFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_ShowVideo, CDialog)
	//{{AFX_MSG_MAP(CDlg_ShowVideo)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_NEXT_FRAME, OnBtnNextFrame)
	ON_BN_CLICKED(IDC_BTN_NEXT_5FRAME, OnBtnNext5frame)
	ON_BN_CLICKED(IDC_BTN_SAVE_BMP, OnBtnSaveBmp)
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_GRID, OnBtnGrid)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BTN_INFO, OnBtnInfo)
	ON_BN_CLICKED(IDC_BTN_FIRST_FRAME, OnBtnFirstFrame)
	ON_BN_CLICKED(IDC_BTN_LAST_FRAME, OnBtnLastFrame)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(IDC_BTN_PRE_5FRAME, OnBtnPre5frame)
	ON_BN_CLICKED(IDC_BTN_PRE_FRAME, OnBtnPreFrame)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_CHECK_CYCLE, OnCheckCycle)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_UPDATE_PICTURE, OnUpdatePicture)
	ON_MESSAGE(WM_REPORT_VIDEO_DECODE_FPS, OnReportVideoDecodeFPS)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_ShowVideo message handlers

void CDlg_ShowVideo::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	m_ToolTip.Close();

	if (m_bPlaying)
	{
		MessageBox("请先退出播放线程", "视频预览", MB_OK);
	}
	else
	{
		switch (m_nVidStreamType & (~STREAM_FILE))
		{
		//case STREAM_PS:

		//	((CMPEG_PS_Decoder*)m_pVidDecoder)->CloseVideo();
		//	break;

		//case STREAM_MPEGVES:

		//	((CMPEG_VideoDecoder*)m_pVidDecoder)->CloseVideo();

		//	break;
		//	
		//case STREAM_AVSVES:

		//	((CAVS_VideoDecoder*)m_pVidDecoder)->CloseVideo();
		//	break;

		//case STREAM_H264VES:

		//	((CH264_VideoDecoder*)m_pVidDecoder)->CloseVideo();
		//	break;

		case STREAM_YUVVES:

			((CYUV_VideoDecoder*)m_pVidDecoder)->CloseVideo();
			break;

		default:
			break;
		}

		m_nVidStreamType = STREAM_UNKNOWN;

		//if (!(m_nAudStreamType & STREAM_FILE))
		//{
		//	switch (m_nAudStreamType & (~STREAM_FILE))
		//	{
		//	case STREAM_MPEGAES:

		//		((CMPEG_AudioDecoder*)m_pAudDecoder)->CloseAudio();
		//		break;

		//	case STREAM_AC3AES:

		//		((CAC3_AudioDecoder*)m_pAudDecoder)->CloseAudio();
		//		break;

		//	case STREAM_WAVEAES:

		//		((CWAVE_AudioDecoder*)m_pAudDecoder)->CloseAudio();
		//		break;

		//	default:
		//		break;
		//	}
		//}

		m_nAudStreamType = STREAM_UNKNOWN;

		m_pVidDecoder = NULL;
		m_pAudDecoder = NULL;

	//	if (m_pdlgInfo != NULL)
	//	{
	//		delete m_pdlgInfo;
	//		m_pdlgInfo = NULL;
	//	}

		CDialog::OnClose();
	}
}

void CDlg_ShowVideo::SetControls(int offline, int audioonly)
{
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_CHECK_CYCLE);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_SLIDER_FILE);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_FIRST_FRAME);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_PRE_5FRAME);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_PRE_FRAME);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_PLAY);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_NEXT_FRAME);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_NEXT_5FRAME);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_LAST_FRAME);
	pWnd->EnableWindow(offline);

	pWnd = GetDlgItem(IDC_BTN_INFO);
	pWnd->EnableWindow(offline & (!audioonly));

	pWnd = GetDlgItem(IDC_BTN_GRID);
	pWnd->EnableWindow(offline & (!audioonly));

	pWnd = GetDlgItem(IDC_BTN_SAVE_BMP);
	pWnd->EnableWindow(offline & (!audioonly));

//	pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
//	pWnd->EnableWindow(!audioonly);
}

void CDlg_ShowVideo::AdjustLayout(int audioonly)
{
	CWnd*	pWnd;

	CRect rcWindow;
	CRect rcClient;

	int		nScreenX = GetSystemMetrics(SM_CXSCREEN);
	int		nScreenY = GetSystemMetrics(SM_CYSCREEN);
	int		nXDelt;
	int		nYDelt;
	int		width;
	int		height;

	GetWindowRect(&rcWindow);
	GetClientRect(&rcClient);

	nYDelt = rcWindow.Height() - rcClient.Height() + 80;
	if (audioonly)
	{
		height = nYDelt;
	}
	else
	{
		height = 576 + nYDelt;
	}

	nXDelt = rcWindow.Width() - rcClient.Width();
	width = 960 + nXDelt;

	rcWindow.left = (nScreenX - width) / 2;
	rcWindow.top = (nScreenY - height) / 2;
	rcWindow.right = rcWindow.left + width;
	rcWindow.bottom = rcWindow.top + height;

	MoveWindow(&rcWindow);

	m_bCycle = TRUE;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);
	if (::IsWindow(pBtn->GetSafeHwnd()))
	{
		pBtn->ShowWindow(SW_SHOW);
		pBtn->SetCheck(m_bCycle);
	}

	CRect	rectClient;
	CRect	rectCtrl;
	int		yOffset;
	int		xOffset;

	GetClientRect(&rectClient);

	yOffset = rectClient.bottom - 5;

	pWnd = GetDlgItem(IDC_SLIDER_FILE);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(10, yOffset - rectCtrl.Height(), rectClient.Width() - 20, rectCtrl.Height());
		yOffset -= rectCtrl.Height();
	}

	yOffset -= 5;
	xOffset = 10;
	
	pWnd = GetDlgItem(IDC_BTN_FIRST_FRAME);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_PRE_5FRAME);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_PRE_FRAME);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_PLAY);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_NEXT_FRAME);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_NEXT_5FRAME);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_LAST_FRAME);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset += rectCtrl.Width() + 5;
	}

	xOffset = rectClient.right - 10;
	pWnd = GetDlgItem(IDC_BTN_SAVE_BMP);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset -= rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_INFO);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset -= rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_GRID);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset -= rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_CHECK_CYCLE);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset -= rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->ShowWindow(SW_SHOW);
		pWnd->GetClientRect(&rectCtrl);
		pWnd->MoveWindow(xOffset - rectCtrl.Width() - 10, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
		xOffset -= rectCtrl.Width() + 5;
	}
}

void CDlg_ShowVideo::RealTimeStream(int vid_stream_type, PVOID pVidDecoder, int aud_stream_type, PVOID pAudDecoder)
{
	SetControls(0, 0);
	AdjustLayout(0);

	m_nVidStreamType = vid_stream_type;
	m_nAudStreamType = aud_stream_type;

	m_pVidDecoder = pVidDecoder;
	m_pAudDecoder = pAudDecoder;

	m_bIsAudio = 0;

	ShowWindow(SW_SHOW);
}

void CDlg_ShowVideo::OfflineStream(int stream_type, PVOID pDecoder, int bAudio)
{
	SetControls(1, bAudio);
	AdjustLayout(bAudio);

	if (bAudio)
	{
		m_nAudStreamType = stream_type;
		m_pAudDecoder = pDecoder;
	}
	else
	{
		m_nVidStreamType = stream_type;
		m_pVidDecoder = pDecoder;
	}

	m_bIsAudio = bAudio;

	ShowWindow(SW_SHOW);
}

void CDlg_ShowVideo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	HWND		hWnd;
	int			nPos = 0;

	if (bShow)
	{
		CenterWindow();

		hWnd = this->GetSafeHwnd();

		switch (m_nVidStreamType & (~STREAM_FILE))
		{
//		case STREAM_PS:
//	
//			((CMPEG_PS_Decoder*)m_pVidDecoder)->OpenVideo(hWnd, m_pszFourCC, sizeof(m_pszFourCC));
//			
//			if (m_nVidStreamType & STREAM_FILE)
//			{
////				nPos = ((CMPEG_PS_Decoder*)m_pVidDecoder)->Preview_FirstPicture();
//			}
//
//			break;
//
//		case STREAM_MPEGVES:
//	
//			((CMPEG_VideoDecoder*)m_pVidDecoder)->OpenVideo(hWnd, m_pszFourCC, sizeof(m_pszFourCC));
//			
//			if (m_nVidStreamType & STREAM_FILE)
//			{
//				nPos = ((CMPEG_VideoDecoder*)m_pVidDecoder)->Preview_FirstPicture();
//			}
//
//			break;
//
//		case STREAM_H264VES:
//
//			((CH264_VideoDecoder*)m_pVidDecoder)->OpenVideo(hWnd, m_pszFourCC, sizeof(m_pszFourCC));
//
//			if (m_nVidStreamType & STREAM_FILE)
//			{
//				nPos = ((CH264_VideoDecoder*)m_pVidDecoder)->Preview_FirstPicture();
//			}
//			break;
//
//		case STREAM_AVSVES:
//
//			((CAVS_VideoDecoder*)m_pVidDecoder)->OpenVideo(hWnd, m_pszFourCC, sizeof(m_pszFourCC));
//
//			if (m_nVidStreamType & STREAM_FILE)
//			{
//				nPos = ((CAVS_VideoDecoder*)m_pVidDecoder)->Preview_FirstPicture();
//			}
//			break;
//
		case STREAM_YUVVES:
			
			((CYUV_VideoDecoder*)m_pVidDecoder)->OpenVideo(hWnd, m_pszFourCC, sizeof(m_pszFourCC));

			if (m_nVidStreamType & STREAM_FILE)
			{
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_FirstPicture();
			}

			break;

		default:
			break;
		}

		m_sldFile.SetPos(nPos);

//		if (!(m_nAudStreamType & STREAM_FILE))
//		{
//			switch (m_nAudStreamType & (~STREAM_FILE))
//			{
//			case STREAM_MPEGAES:
//
//				((CMPEG_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
//				break;
//
//			case STREAM_WAVEAES:
//
//				((CWAVE_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
//				break;
//
//			case STREAM_AC3AES:
//				((CAC3_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
//				break;
//
//			default:
//				break;
//			}
//		}
	}
	else
	{
	}

	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	switch (m_nVidStreamType & (~STREAM_FILE))
	{
	case STREAM_PS:
		break;
	default:
		if (m_pVidDecoder != NULL)
		{
			((CVESDecoder*)m_pVidDecoder)->DirectDraw_Paint();
		}
		break;
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlg_ShowVideo::OnBtnNextFrame() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nPos = m_sldFile.GetPos();

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			//switch (m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_AC3AES:

			//	nPos = ((CAC3_AudioDecoder*)m_pAudDecoder)->Preview_NextFrame(1);
			//	break;

			//case STREAM_MPEGAES:

			//	nPos = ((CMPEG_AudioDecoder*)m_pAudDecoder)->Preview_NextFrame(1);
			//	break;

			//case STREAM_WAVEAES:

			//	nPos = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_NextFrame(1);
			//	break;

			//default:
			//	break;
			//}
		}
	}
	else
	{
		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			//case STREAM_PS:
			//	break;
			//case STREAM_MPEGVES:
			//	nPos = ((CMPEG_VideoDecoder*)m_pVidDecoder)->Preview_NextPicture();
			//	break;
			//
			//case STREAM_AVSVES:
			//	nPos = ((CAVS_VideoDecoder*)m_pVidDecoder)->Preview_NextPicture();
			//	break;

			//case STREAM_H264VES:
			//	nPos = ((CH264_VideoDecoder*)m_pVidDecoder)->Preview_NextPicture();
			//	break;

			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_NextPicture();
				break;

			default:
				break;
			}
		}
	}

	m_sldFile.SetPos(nPos);
	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnBtnNext5frame() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nPos = m_sldFile.GetPos();

	if (m_bIsAudio)
	{
	}
	else
	{
		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_ForwardPicture();
				break;

			default:
				break;
			}
		}
	}

	m_sldFile.SetPos(nPos);
	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int min;
	int max;

	UpdateData(TRUE);

	int nPercent;

	if (pScrollBar == (CScrollBar*)&m_sldFile)
	{
		nPercent = m_sldFile.GetPos();
	
		m_sldFile.GetRange(min, max);
		if (nPercent < min)
		{
			nPercent = min;
		}
		else if (nPercent > max)
		{
			nPercent = max;
		}

		if (m_bIsAudio)
		{
			//switch (m_nAudStreamType & ~STREAM_FILE)
			//{
			//case STREAM_AC3AES:
			//	nPercent = ((CAC3_AudioDecoder*)m_pAudDecoder)->Preview_AtPercent(nPercent);
			//	break;

			//case STREAM_MPEGAES:
			//	nPercent = ((CMPEG_AudioDecoder*)m_pAudDecoder)->Preview_AtPercent(nPercent);
			//	break;

			//case STREAM_WAVEAES:
			//	nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_AtPercent(nPercent);
			//	break;

			//default:
			//	break;
			//}
		}
		else
		{
			switch (m_nVidStreamType & ~STREAM_FILE)
			{
			//case STREAM_PS:
			//	nPercent = ((CMPEG_PS_Decoder*)m_pVidDecoder)->Preview_AtPercent(nPercent);
			//	break;
			//	break;

			//case STREAM_MPEGVES:
			//	nPercent = ((CMPEG_VideoDecoder*)m_pVidDecoder)->Preview_AtPercent(nPercent);
			//	break;

			//case STREAM_AVSVES:
			//	nPercent = ((CAVS_VideoDecoder*)m_pVidDecoder)->Preview_AtPercent(nPercent);
			//	break;
			//
			//case STREAM_H264VES:
			//	nPercent = ((CH264_VideoDecoder*)m_pVidDecoder)->Preview_AtPercent(nPercent);
			//	break;

			case STREAM_YUVVES:
				nPercent = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_AtPercent(nPercent);
				break;

			default:
				break;
			}
		}

		m_sldFile.SetPos(nPercent);
		UpdateData(FALSE);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CDlg_ShowVideo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_sldFile.SetRange(0, 100);
	m_sldFile.SetPos(0);
	m_sldFile.SetPageSize(10);

//	AdjustLayout(0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_ShowVideo::OnBtnSaveBmp() 
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME		systime;
	char			pszFileName[128];
	char szFilter[] = "All Files (*.bmp)|*.bmp||";

	::GetSystemTime(&systime);

	sprintf_s(pszFileName, sizeof(pszFileName), "CDL_%04d%02d%02d%02d%02d%02d.bmp", 
						systime.wYear,
						systime.wMonth,
						systime.wDay,
						systime.wHour + 8,
						systime.wMinute,
						systime.wSecond);

	CFileDialog dlg(FALSE, "bmp", pszFileName, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString strFile = dlg.GetFileName();
	
//		DirectDraw_Save(strFile.GetBuffer(128));
	}
}


void CDlg_ShowVideo::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
/*
	char	pszText[MAX_TXT_CHARS];
	
	strcpy_s(pszText, sizeof(pszText), "Unknown stream type");

	if (m_nVidStreamType & STREAM_FILE)
	{
		switch (m_nVidStreamType & (~STREAM_FILE))
		{
		case STREAM_MPEGVES:
			strcpy_s(pszText, sizeof(pszText), "MPEG Video Decoding from file");
			break;

		case STREAM_AVSVES:
			strcpy_s(pszText, sizeof(pszText), "AVS Video Decoding from file");
			break;

		case STREAM_H264VES:
			strcpy_s(pszText, sizeof(pszText), "H264 Video Decoding from file");
			break;

		case STREAM_YUVVES:
			strcpy_s(pszText, sizeof(pszText), "YUV序列");
			break;

		default:
			break;
		}
	}
	else
	{
		switch (m_nVidStreamType & (~STREAM_FILE))
		{
		case STREAM_MPEGVES:
			strcpy_s(pszText, sizeof(pszText), "MPEG Video Decoding realtime");
			break;

		case STREAM_AVSVES:
			strcpy_s(pszText, sizeof(pszText), "AVS Video Decoding realtime");
			break;

		case STREAM_H264VES:
			strcpy_s(pszText, sizeof(pszText), "H264 Video Decoding realtime");
			break;

		default:
			break;
		}
	}

	SetWindowText(pszText);
*/
}

void CDlg_ShowVideo::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*
	int		 mb_row;
	int		 mb_col;
	char	 pszText[64];

	if (m_nStreamType & STREAM_FILE)
	{
		CDC* pDC = GetDC();

		mb_row = point.y / m_nMbHeight;
		mb_col = point.x / m_nMbWidth;

		sprintf_s(pszText, sizeof(pszText), "mb_row = %d", mb_row);
		pDC->TextOut(10, 10, pszText);

		sprintf_s(pszText, sizeof(pszText), "mb_col = %d", mb_col);
		pDC->TextOut(10, 30, pszText);

		ReleaseDC(pDC);
	}
*/	
	CDialog::OnMouseMove(nFlags, point);
}

void CDlg_ShowVideo::OnBtnGrid() 
{
	// TODO: Add your control notification handler code here
	//switch (m_nVidStreamType & (~STREAM_FILE))
	//{
	//case STREAM_MPEGVES:
	//	((CMPEG_VideoDecoder*)m_pVidDecoder)->SetGrid();
	//	break;

	//case STREAM_AVSVES:
	//	((CAVS_VideoDecoder*)m_pVidDecoder)->SetGrid();
	//	break;

	//case STREAM_H264VES:
	//	((CH264_VideoDecoder*)m_pVidDecoder)->SetGrid();
	//	break;

	//default:
	//	break;
	//}
}

void CDlg_ShowVideo::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*
	int		 mb_row;
	int		 mb_col;
	int		 mb_width;
	int		 mb_index;
	CString  str;

	MPEG_MB_INFO_t		mpeg_mb_info;
	AVS_MB_INFO_t		avs_mb_info;
//	H264_MB_INFO_t		h264_mb_info;

	if (m_nStreamType & STREAM_FILE)
	{
//		if (!m_pdlgInfo->IsWindowVisible())
//		{
//			m_pdlgInfo->ShowWindow(SW_SHOW);
//		}

		mb_width = m_nGraphWidth / m_nMbWidth;

		mb_row = point.y / m_nMbHeight;
		mb_col = point.x / m_nMbWidth;

		mb_index = mb_row * mb_width + mb_col;

//		m_ToolTip.Close();
//		m_ToolTip.ClearText();

		switch (m_nStreamType & (~STREAM_FILE))
		{
		case STREAM_MPEGVES:
			M2V_REPORT.M2VDecoder.Get_macroblock(mb_index, &mpeg_mb_info);
			
			str.Format("Position = (%d,%d)", mb_row, mb_col);
//			m_ToolTip.AppendText(str);

			str.Format("Type = %d", mpeg_mb_info.Type);
			m_ToolTip.AppendText(str);

			str.Format("Quant = %d", mpeg_mb_info.Quant);
			m_ToolTip.AppendText(str);

			str.Format("DCT = %d", mpeg_mb_info.DCT);
			m_ToolTip.AppendText(str);

			str.Format("cbp = %d", mpeg_mb_info.Cbp);
			m_ToolTip.AppendText(str);

			str.Format("MotionType = %d", mpeg_mb_info.MotionType);
			m_ToolTip.AppendText(str);

			str.Format("Skipped = %d", mpeg_mb_info.Skipped);
			m_ToolTip.AppendText(str);

			break;

		case STREAM_AVSVES:
			AVS_VIDEO_REPORT.AVSDecoder.Get_macroblock(mb_index, &avs_mb_info);

			str.Format("Position = (%d,%d)", mb_row, mb_col);
			m_ToolTip.AppendText(str);
			
			break;

		case STREAM_H264VES:

//			H264_VIDEO_REPORT.H264Decoder.Get_macroblock(mb_index, &h264_mb_info);

			str.Format("Position = (%d,%d)", mb_row, mb_col);
			m_ToolTip.AppendText(str);
			
			break;

		default:
			break;
		}

		m_ToolTip.Show(point);
	}
*/	
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDlg_ShowVideo::OnBtnInfo() 
{
	// TODO: Add your control notification handler code here
//	m_ToolTip.Close();

//	if (m_pdlgInfo == NULL)
//	{
//		m_pdlgInfo = new CDlg_DecodeInfo;
//		m_pdlgInfo->Create(IDD_DECODE_INFO, this);
//		m_pdlgInfo->ShowWindow(SW_SHOW);
//	}
//	else
//	{
//		if (m_pdlgInfo->IsWindowVisible())
//		{
//			m_pdlgInfo->ShowWindow(SW_HIDE);
//		}
//		else
//		{
//			m_pdlgInfo->ShowWindow(SW_SHOW);
//		}
//	}
}

void CDlg_ShowVideo::OnBtnFirstFrame() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nPos = m_sldFile.GetPos();

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			//switch (m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_AC3AES:
			//	nPos = ((CAC3_AudioDecoder*)m_pAudDecoder)->Preview_FirstFrame();
			//	break;

			//case STREAM_MPEGAES:
			//	nPos = ((CMPEG_AudioDecoder*)m_pAudDecoder)->Preview_FirstFrame();
			//	break;

			//case STREAM_WAVEAES:
			//	nPos = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_FirstFrame();
			//	break;

			//default:
			//	break;
			//}
		}
	}
	else
	{
		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			//case STREAM_MPEGVES:
			//	nPos = ((CMPEG_VideoDecoder*)m_pVidDecoder)->Preview_FirstPicture();
			//	break;

			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_FirstPicture();
				break;

			default:
				break;
			}
		}
	}

	m_sldFile.SetPos(nPos);
	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnBtnLastFrame() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nPos = m_sldFile.GetPos();

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			switch (m_nAudStreamType & (~STREAM_FILE))
			{
			case STREAM_WAVEAES:
//				nPos = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_LastFrame();
				break;

			default:
				break;
			}
		}
	}
	else
	{
		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_LastPicture();
				break;

			default:
				break;
			}
		}
	}

	m_sldFile.SetPos(nPos);
	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnBtnPlay() 
{
	// TODO: Add your control notification handler code here
	HWND	hWnd = this->GetSafeHwnd();
//	int		nPos;

	UpdateData(TRUE);

	CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY);
	CString strText;

	pWnd->GetWindowText(strText);
	if (strText == "播放")
	{
		if (m_nAudStreamType & STREAM_FILE)		//如果是播放音频文件
		{
			//switch (m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_MPEGAES:

			//	((CMPEG_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
			//	break;

			//case STREAM_WAVEAES:

			//	((CWAVE_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
			//	break;

			//case STREAM_AC3AES:
			//	((CAC3_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
			//	break;

			//default:
			//	break;
			//}
		}

		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			case STREAM_PS:
				//((CMPEG_PS_Decoder*)m_pVidDecoder)->OpenAudio(hWnd);
//				((CMPEG_PS_Decoder*)m_pVidDecoder)->ExtDebug();
				break;
			}
		}

		pWnd->SetWindowText("停止");

		m_bPlaying = 1;

		if (m_bIsAudio)
		{
			m_hThread = ::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)AudioPlay_Thread, this, 0, &m_dwID);
		}
		else
		{
			m_hThread = ::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)VideoPlay_Thread, this, 0, &m_dwID);
		}

		m_sldFile.EnableWindow(FALSE);
	}
	else
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			//switch (m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_MPEGAES:

			//	((CMPEG_AudioDecoder*)m_pAudDecoder)->CloseAudio();
			//	break;

			//case STREAM_AC3AES:

			//	((CAC3_AudioDecoder*)m_pAudDecoder)->CloseAudio();
			//	break;

			//case STREAM_WAVEAES:

			//	((CWAVE_AudioDecoder*)m_pAudDecoder)->CloseAudio();
			//	break;

			//default:
			//	break;
			//}
		}

		if (m_nVidStreamType & STREAM_FILE)
		{
			//switch (m_nVidStreamType & (~STREAM_FILE))
			//{
			//case STREAM_PS:
			//	((CMPEG_PS_Decoder*)m_pVidDecoder)->CloseAudio();
			//	break;
			//}
		}

		pWnd->SetWindowText("播放");

		m_bPlaying = 0;

		m_sldFile.EnableWindow(TRUE);

		CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
		pWnd->SetWindowText("");
	}

	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnBtnPre5frame() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nPos = m_sldFile.GetPos();

	if (m_bIsAudio)
	{
/*		if (m_nAudStreamType & STREAM_FILE)
		{
			switch (m_nAudStreamType & (~STREAM_FILE))
			{
			case STREAM_WAVEAES:
				nPos = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_LastFrame();
				break;

			default:
				break;
			}
		}
*/
	}
	else
	{
		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_BackwardPicture();
				break;

			default:
				break;
			}
		}
	}

	m_sldFile.SetPos(nPos);
	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnBtnPreFrame() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nPos = m_sldFile.GetPos();

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
//			switch (m_nAudStreamType & (~STREAM_FILE))
//			{
//			case STREAM_WAVEAES:
////				nPos = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_PreFrame();
//				break;
//
//			default:
//				break;
//			}
		}
	}
	else
	{
		if (m_nVidStreamType & STREAM_FILE)
		{
			switch (m_nVidStreamType & (~STREAM_FILE))
			{
			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)m_pVidDecoder)->Preview_PrePicture();
				break;

			default:
				break;
			}
		}
	}
	m_sldFile.SetPos(nPos);
	UpdateData(FALSE);
}

U32 VideoPlay_Thread(PVOID pVoid)
{
	CDlg_ShowVideo* pdlg = (CDlg_ShowVideo*)pVoid;
	int				nPos = 0;

	do
	{
		if (pdlg->m_nVidStreamType & STREAM_FILE)
		{
			switch (pdlg->m_nVidStreamType & (~STREAM_FILE))
			{
			//case STREAM_PS:
			//	
			//	nPos = ((CMPEG_PS_Decoder*)pdlg->m_pVidDecoder)->Preview_NextPack();
			//	break;

			case STREAM_YUVVES:
				nPos = ((CYUV_VideoDecoder*)pdlg->m_pVidDecoder)->Preview_NextPicture();
				break;
			
			//case STREAM_MPEGVES:
			//	nPos = ((CMPEG_VideoDecoder*)pdlg->m_pVidDecoder)->Preview_NextPicture();
			//	break;

			default:
				break;
			}
		}

//		if (pdlg->m_bCycle)

		{
			if (pdlg->m_nVidStreamType & STREAM_FILE)
			{
				switch (pdlg->m_nVidStreamType & (~STREAM_FILE))
				{
				//case STREAM_PS:

				//	if (((CMPEG_PS_Decoder*)pdlg->m_pVidDecoder)->Preview_EOF())
				//	{
				//		nPos = ((CMPEG_PS_Decoder*)pdlg->m_pVidDecoder)->Preview_AtPercent(0);
				//	}

				//	break;

				case STREAM_YUVVES:
					if (((CYUV_VideoDecoder*)pdlg->m_pVidDecoder)->Preview_EOF())
					{
						nPos = ((CYUV_VideoDecoder*)pdlg->m_pVidDecoder)->Preview_FirstPicture();
					}
					break;

				case STREAM_MPEGVES:
					break;

				default:
					break;
				}
			}
		}

		pdlg->m_sldFile.SetPos(nPos);

//		Sleep(10);

	} while(pdlg->m_bPlaying);

	return 0;
}

U32 AudioPlay_Thread(PVOID pVoid)
{
	CDlg_ShowVideo* pdlg = (CDlg_ShowVideo*)pVoid;
	int				nPos = 0;
	int				frame_count = 0;

	do
	{
		if (pdlg->m_nAudStreamType & STREAM_FILE)
		{
			//switch (pdlg->m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_AC3AES:
			//	nPos = ((CAC3_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_NextFrame(0);
			//	break;

			//case STREAM_MPEGAES:
			//	nPos = ((CMPEG_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_NextFrame(0);
			//	break;

			//case STREAM_WAVEAES:
			//	nPos = ((CWAVE_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_NextFrame(0);
			//	break;

			//default:
			//	break;
			//}
		}

//		if (pdlg->m_bCycle)
		{
			if (pdlg->m_nAudStreamType & STREAM_FILE)
			{
				//switch (pdlg->m_nAudStreamType & (~STREAM_FILE))
				//{
				//case STREAM_AC3AES:
				//	if (((CAC3_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_EOF())
				//	{
				//		nPos = ((CAC3_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_FirstFrame();
				//	}
				//	break;

				//case STREAM_MPEGAES:
				//	if (((CMPEG_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_EOF())
				//	{
				//		nPos = ((CMPEG_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_FirstFrame();
				//	}
				//	break;

				//case STREAM_WAVEAES:
				//	if (((CWAVE_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_EOF())
				//	{
				//		nPos = ((CWAVE_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_FirstFrame();
				//	}
				//	break;

				//default:
				//	break;
				//}
			}
		}

		pdlg->m_sldFile.SetPos(nPos);

		frame_count ++;

//		Sleep(10);

//	} while(pdlg->m_bPlaying && (frame_count < 3000));
	} while(pdlg->m_bPlaying);

	return 0;
}

void CDlg_ShowVideo::OnCheckCycle() 
{
	// TODO: Add your control notification handler code here
//	int bCycle;

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);
//	bCycle = pBtn->GetCheck();

	m_bCycle = (m_bCycle == 0) ? 1 : 0;
	pBtn->SetCheck(m_bCycle);

	UpdateData(FALSE);
}


LRESULT CDlg_ShowVideo::OnReportVideoDecodeFPS(WPARAM wParam, LPARAM lParam)
{
	U32	FPS = (U32)wParam;
	float fps;
	CString strFPS;

	fps = 0.001f * FPS;
	strFPS.Format("%.3f fps", fps);
	CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	pWnd->SetWindowText(strFPS);

	return 0;
}