// Dlg_M2VDecoder.cpp : implementation file
//

#include "stdafx.h"

#include <assert.h>

#include "Dlg_ShowVideo.h"

#include "../MainFrm.h"
#include "../Magic_TS/TSMagic_GuiApi_MSG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_ShowVideo dialog

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
//#include "MiddleWare_ESDecoder/Include/Decoder_MPEG2_PS.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_MPEG2.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_MPEG2.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_H264.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_AVS.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AAC.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AC3.h"
#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder_ErrorCode.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_WAVE.h"

CDlg_ShowVideo::CDlg_ShowVideo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_ShowVideo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_ShowVideo)
	//}}AFX_DATA_INIT
	//m_nVidStreamType = STREAM_UNKNOWN;
	m_nAudStreamType = STREAM_UNKNOWN;
	m_nGraphWidth = 0;
	m_nGraphHeight = 0;
	m_nMbWidth = 16;
	m_nMbHeight = 16;

	m_bPlaying = 0;
	m_bIsAudio = 0;
	m_bCycle = 1;
//	m_pdlgInfo = NULL;

	m_pVidDecoder = NULL;
	m_pAudDecoder = NULL;

	m_fViewRatio = 1.0f;
	
	strcpy_s(m_pszFourCC, sizeof(m_pszFourCC), "");

	m_bFullScreen = false;
	m_pdlgPlayController = NULL;

	m_bForcingShowController = false;
}


void CDlg_ShowVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_ShowVideo)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_ShowVideo, CDialog)
	//{{AFX_MSG_MAP(CDlg_ShowVideo)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDC_BTN_NEXT_FRAME, OnBtnNextFrame)
	//ON_BN_CLICKED(IDC_BTN_NEXT_5FRAME, OnBtnNext5frame)
	//ON_BN_CLICKED(IDC_BTN_SAVE_BMP, OnBtnSaveBmp)
	//ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	//ON_BN_CLICKED(IDC_BTN_GRID, OnBtnGrid)
	ON_WM_LBUTTONDBLCLK()
	//ON_BN_CLICKED(IDC_BTN_INFO, OnBtnInfo)
	//ON_BN_CLICKED(IDC_BTN_FIRST_FRAME, OnBtnFirstFrame)
	//ON_BN_CLICKED(IDC_BTN_LAST_FRAME, OnBtnLastFrame)
	//ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	//ON_BN_CLICKED(IDC_BTN_PRE_5FRAME, OnBtnPre5frame)
	//ON_BN_CLICKED(IDC_BTN_PRE_FRAME, OnBtnPreFrame)
	ON_WM_MOVE()
	//ON_BN_CLICKED(IDC_CHECK_CYCLE, OnCheckCycle)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REPORT_VIDEO_DECODE_FPS, OnReportVideoDecodeFPS)
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_TIMER()
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
		if (m_pVidDecoder != NULL)
		{
			m_pVidDecoder->CloseDirectxWnd();
			m_pVidDecoder = NULL;
		}

		//m_nVidStreamType = STREAM_UNKNOWN;

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

		m_pAudDecoder = NULL;

		m_pdlgPlayController->ShowWindow(SW_HIDE);

		m_bFullScreen = false;

		CDialog::OnClose();
	}
}


//void CDlg_ShowVideo::AdjustLayout(int videoWidth, int videoHeight)
//{
//	CWnd*	pWnd;
//
//	//CRect rcWindow;
//	//CRect rcClient;
//	CRect rcLeftBtn;
//	CRect rcRightBtn;
//
//	int		nScreenX = GetSystemMetrics(SM_CXSCREEN);
//	int		nScreenY = GetSystemMetrics(SM_CYSCREEN);
//	//int		nXDelt;
//	//int		nYDelt;
//	//int		width;
//	//int		height;
//
//	//GetWindowRect(&rcWindow);
//	//GetClientRect(&rcClient);
//
//	CWnd* pLeftBtnWnd = GetDlgItem(IDC_BTN_FIRST_FRAME);
//	pLeftBtnWnd->GetWindowRect(&rcLeftBtn);
//
//	CWnd* pRightBtnWnd = GetDlgItem(IDC_BTN_SAVE_BMP);
//	pRightBtnWnd->GetWindowRect(&rcRightBtn);
//
//	//nYDelt = rcWindow.Height() - rcClient.Height() + rcLeftBtn.Height() + 30;
//	//height = videoHeight + nYDelt;
//	//if (audioonly)
//	//{
//	//	height = nYDelt;
//	//}
//	//else
//	//{
//	//	height = 576 + nYDelt;
//	//}
//
//	//nXDelt = rcWindow.Width() - rcClient.Width();
//	//width = videoWidth + nXDelt + 6;
//	//if (width < 640) width = 640;
//
//	//rcWindow.left = (nScreenX - width) / 2;
//	//rcWindow.top = (nScreenY - height) / 2;
//	//rcWindow.right = rcWindow.left + width;
//	//rcWindow.bottom = rcWindow.top + height;
//
//	//MoveWindow(&rcWindow);
//
//	m_bCycle = TRUE;
//	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);
//	if (::IsWindow(pBtn->GetSafeHwnd()))
//	{
//		pBtn->ShowWindow(SW_SHOW);
//		pBtn->SetCheck(m_bCycle);
//	}
//
//	//CRect	rectClient;
//	CRect	rectCtrl;
//	int		yOffset;
//	int		xOffset;
//
//	//GetClientRect(&rectClient);
//
//	yOffset = nScreenY - 25;
//
//	pWnd = GetDlgItem(IDC_SLIDER_FILE);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(10, yOffset - rectCtrl.Height(), nScreenX - 20, rectCtrl.Height());
//		yOffset -= rectCtrl.Height();
//	}
//
//	yOffset -= 5;
//	xOffset = 10;
//	
//	pWnd = GetDlgItem(IDC_BTN_FIRST_FRAME);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_PRE_5FRAME);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_PRE_FRAME);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_PLAY);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_NEXT_FRAME);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_NEXT_5FRAME);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_LAST_FRAME);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset += rectCtrl.Width() + 5;
//	}
//
//	xOffset = nScreenX - 20;
//	pWnd = GetDlgItem(IDC_BTN_SAVE_BMP);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset -= rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_INFO);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset -= rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_BTN_GRID);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset -= rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_CHECK_CYCLE);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset - rectCtrl.Width(), yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset -= rectCtrl.Width() + 5;
//	}
//
//	pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
//	if (::IsWindow(pWnd->GetSafeHwnd()))
//	{
//		pWnd->ShowWindow(SW_SHOW);
//		pWnd->GetClientRect(&rectCtrl);
//		pWnd->MoveWindow(xOffset - rectCtrl.Width() - 10, yOffset - rectCtrl.Height(), rectCtrl.Width(), rectCtrl.Height());
//		xOffset -= rectCtrl.Width() + 5;
//	}
//}

//void CDlg_ShowVideo::RealTimeStream(int vid_stream_type, PVOID pVidDecoder, int aud_stream_type, PVOID pAudDecoder)
//{
//	SetControls(0, 0);
//	AdjustLayout(1000, 600);
//
//	m_nVidStreamType = vid_stream_type;
//	m_nAudStreamType = aud_stream_type;
//
//	m_pVidDecoder = (CVESDecoder*)pVidDecoder;
//	m_pAudDecoder = pAudDecoder;
//
//	m_bIsAudio = 0;
//
//	ShowWindow(SW_SHOW);
//}

void CDlg_ShowVideo::AttachVideoDecoder(PVOID pDecoder)
{
	assert(pDecoder != NULL);

	int videoWidth = 500;
	int videoHeight = 300;

	m_pdlgPlayController->SetControls(1, 0);

	//if (bAudio)
	//{
	//	m_nAudStreamType = stream_type;
	//	m_pAudDecoder = pDecoder;
	//}
	//else
	//{
	//	m_nVidStreamType = stream_type;
		m_pVidDecoder = (CVESDecoder*)pDecoder;

		Video_decode_info_t decode_info;
		m_pVidDecoder->GetDecodeInfo(&decode_info);
		videoWidth = decode_info.display_width;
		videoHeight = decode_info.display_height;
	//}

	//AdjustLayout(videoWidth, videoHeight);

	//m_bIsAudio = bAudio;

	//ShowWindow(SW_SHOW);
}

void CDlg_ShowVideo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	HWND		hWnd;
	int			nPos = 0;

	if (bShow)
	{
		//EnlargeClientAreaToFullScreen();
		//CenterWindow();

		if (m_pVidDecoder != NULL)
		{
			if (m_pVidDecoder->GetWndHandle() == NULL)
			{
				char pszTitle[256];
				if (m_pVidDecoder->GetTitle(pszTitle, sizeof(pszTitle)) == ESDECODER_NO_ERROR)
				{
					SetWindowText(pszTitle);
				}

				hWnd = this->GetSafeHwnd();
				m_pVidDecoder->OpenDirectxWnd(hWnd);

				nPos = m_pVidDecoder->Preview_FirstPicture();
				m_pdlgPlayController->m_sldFile.SetPos(nPos);
			}
		}

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

		if (m_pdlgPlayController != NULL)
		{
			m_pdlgPlayController->ShowWindow(SW_SHOW);

			m_bForcingShowController = true;

			m_dwTimerID = 0xff56f344;
			SetTimer(m_dwTimerID, 2000, NULL);
		}
	}
	else
	{
		if (m_pdlgPlayController != NULL)
		{
			m_pdlgPlayController->ShowWindow(SW_HIDE);
		}
	}

	UpdateData(FALSE);
}

void CDlg_ShowVideo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	//switch (m_nVidStreamType & (~STREAM_FILE))
	//{
	//case STREAM_PS:
	//	break;
	//default:
		if (m_pVidDecoder != NULL)
		{
			m_pVidDecoder->DirectDraw_Paint();
		}
	//	break;
	//}
	// Do not call CDialog::OnPaint() for painting messages
}

int CDlg_ShowVideo::PreviewNextFrame() 
{
	// TODO: Add your control notification handler code here
	int nPos = -1;
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
		//if (m_nVidStreamType & STREAM_FILE)
		//{
			if (m_pVidDecoder != NULL)
			{
				nPos = m_pVidDecoder->Preview_Forward1Picture();
			}
		//}
	}

	return nPos;
}

int CDlg_ShowVideo::PreviewNext5Frame() 
{
	// TODO: Add your control notification handler code here
	int nPos = -1;

	if (m_bIsAudio)
	{
	}
	else
	{
		//if (m_nVidStreamType & STREAM_FILE)
		//{
			if (m_pVidDecoder != NULL)
			{
				nPos = m_pVidDecoder->Preview_ForwardNPicture(5);
			}
		//}
	}

	return nPos;
}

//void CDlg_ShowVideo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//{
//	// TODO: Add your message handler code here and/or call default
//	int min;
//	int max;
//
//	UpdateData(TRUE);
//
//	int nPercent;
//
//	if (pScrollBar == (CScrollBar*)&m_sldFile)
//	{
//		nPercent = m_sldFile.GetPos();
//	
//		m_sldFile.GetRange(min, max);
//		if (nPercent < min)
//		{
//			nPercent = min;
//		}
//		else if (nPercent > max)
//		{
//			nPercent = max;
//		}
//
//		if (m_bIsAudio)
//		{
//			//switch (m_nAudStreamType & ~STREAM_FILE)
//			//{
//			//case STREAM_AC3AES:
//			//	nPercent = ((CAC3_AudioDecoder*)m_pAudDecoder)->Preview_AtPercent(nPercent);
//			//	break;
//
//			//case STREAM_MPEGAES:
//			//	nPercent = ((CMPEG_AudioDecoder*)m_pAudDecoder)->Preview_AtPercent(nPercent);
//			//	break;
//
//			//case STREAM_WAVEAES:
//			//	nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_AtPercent(nPercent);
//			//	break;
//
//			//default:
//			//	break;
//			//}
//		}
//		else
//		{
//			if (m_pVidDecoder != NULL)
//			{
//				nPercent = m_pVidDecoder->Preview_SeekAtPercent(nPercent);
//			}
//		}
//
//		m_sldFile.SetPos(nPercent);
//		UpdateData(FALSE);
//	}
//
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
//}

BOOL CDlg_ShowVideo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	AdjustLayout(0);

	m_pdlgPlayController = new CDlg_PlayController;
	m_pdlgPlayController->Create(IDD_SHOW_VIDEO_CTRL, this);

	m_pdlgPlayController->m_sldFile.SetRange(0, 100);
	m_pdlgPlayController->m_sldFile.SetPos(0);
	m_pdlgPlayController->m_sldFile.SetPageSize(10);

	//UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_ShowVideo::SaveSnapshot(void) 
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

	CFileDialog dlg(true, "bmp", pszFileName, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

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

	if (m_bForcingShowController == 0)
	{
		if (m_rectPlayControl.PtInRect(point))
		{
			m_pdlgPlayController->ShowWindow(SW_SHOW);
		}
		else
		{
			m_pdlgPlayController->ShowWindow(SW_HIDE);
		}
	}
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

	if (m_pVidDecoder != NULL)
	{
		//m_pVidDecoder->SetGrid();
	}
}

void CDlg_ShowVideo::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bFullScreen)
	{
		EnlargeClientAreaToFullScreen();
	}
	else
	{
		RestoreClientAreaToInitial();
	}
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
	//Invalidate(FALSE);

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

int CDlg_ShowVideo::PreviewFirstFrame() 
{
	// TODO: Add your control notification handler code here
	int nPos = -1;

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
		//if (m_nVidStreamType & STREAM_FILE)
		//{
			if (m_pVidDecoder != NULL)
			{
				nPos = m_pVidDecoder->Preview_FirstPicture();
			}
		//}
	}

	return nPos;
}

int CDlg_ShowVideo::PreviewLastFrame() 
{
	// TODO: Add your control notification handler code here
	int nPos = -1;

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
		//if (m_nVidStreamType & STREAM_FILE)
		//{
			if (m_pVidDecoder != NULL)
			{
				nPos = m_pVidDecoder->Preview_LastPicture();
			}
		//}
	}

	return nPos;
}

//void CDlg_ShowVideo::OnBtnPlay() 
//{
//	// TODO: Add your control notification handler code here
//	HWND	hWnd = this->GetSafeHwnd();
////	int		nPos;
//
//	UpdateData(TRUE);
//
//	CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY);
//	CString strText;
//
//	pWnd->GetWindowText(strText);
//	if (strText == "播放")
//	{
//		//if (m_nAudStreamType & STREAM_FILE)		//如果是播放音频文件
//		//{
//			//switch (m_nAudStreamType & (~STREAM_FILE))
//			//{
//			//case STREAM_MPEGAES:
//
//			//	((CMPEG_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
//			//	break;
//
//			//case STREAM_WAVEAES:
//
//			//	((CWAVE_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
//			//	break;
//
//			//case STREAM_AC3AES:
//			//	((CAC3_AudioDecoder*)m_pAudDecoder)->OpenAudio(hWnd);
//			//	break;
//
//			//default:
//			//	break;
//			//}
//		//}
//
//		//if (m_nVidStreamType & STREAM_FILE)
//		//{
////			switch (m_nVidStreamType & (~STREAM_FILE))
////			{
////			case STREAM_PS:
////				//((CMPEG_PS_Decoder*)m_pVidDecoder)->OpenAudio(hWnd);
//////				((CMPEG_PS_Decoder*)m_pVidDecoder)->ExtDebug();
////				break;
////			}
//		//}
//
//		pWnd->SetWindowText("停止");
//
//		m_bPlaying = 1;
//
//		if (m_bIsAudio)
//		{
//			m_hThread = ::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)AudioPlay_Thread, this, 0, &m_dwID);
//		}
//		else
//		{
//			m_hThread = ::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)VideoPlay_Thread, this, 0, &m_dwID);
//		}
//
//		m_sldFile.EnableWindow(FALSE);
//	}
//	else
//	{
//		if (m_nAudStreamType & STREAM_FILE)
//		{
//			//switch (m_nAudStreamType & (~STREAM_FILE))
//			//{
//			//case STREAM_MPEGAES:
//
//			//	((CMPEG_AudioDecoder*)m_pAudDecoder)->CloseAudio();
//			//	break;
//
//			//case STREAM_AC3AES:
//
//			//	((CAC3_AudioDecoder*)m_pAudDecoder)->CloseAudio();
//			//	break;
//
//			//case STREAM_WAVEAES:
//
//			//	((CWAVE_AudioDecoder*)m_pAudDecoder)->CloseAudio();
//			//	break;
//
//			//default:
//			//	break;
//			//}
//		}
//
//		//if (m_nVidStreamType & STREAM_FILE)
//		//{
//			//switch (m_nVidStreamType & (~STREAM_FILE))
//			//{
//			//case STREAM_PS:
//			//	((CMPEG_PS_Decoder*)m_pVidDecoder)->CloseAudio();
//			//	break;
//			//}
//		//}
//
//		pWnd->SetWindowText("播放");
//
//		m_bPlaying = 0;
//
//		m_sldFile.EnableWindow(TRUE);
//
//		CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
//		pWnd->SetWindowText("");
//	}
//
//	UpdateData(FALSE);
//}

int CDlg_ShowVideo::PreviewPre5Frame() 
{
	// TODO: Add your control notification handler code here
	int nPos = -1;

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
		//if (m_nVidStreamType & STREAM_FILE)
		//{
			if (m_pVidDecoder != NULL)
			{
				nPos = m_pVidDecoder->Preview_BackwardNPicture(5);
			}
		//}
	}

	return nPos;
}

int CDlg_ShowVideo::PreviewPreFrame() 
{
	// TODO: Add your control notification handler code here
	int nPos = -1;

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
		//if (m_nVidStreamType & STREAM_FILE)
		//{
			if (m_pVidDecoder != NULL)
			{
				nPos = m_pVidDecoder->Preview_Backward1Picture();
			}
		//}
	}

	return nPos;
}

uint32_t VideoPlay_Thread(PVOID pVoid)
{
	CDlg_ShowVideo* pdlg = (CDlg_ShowVideo*)pVoid;
	int				nPos = 0;

	DWORD			dwStartTick = ::GetTickCount();
	int				frameCount = 0;

	Video_decode_info_t  video_decode_info;
	pdlg->m_pVidDecoder->GetDecodeInfo(&video_decode_info);

	double frame_interval = 1000.0 / video_decode_info.framerate;

	do
	{
		nPos = pdlg->m_pVidDecoder->Preview_Forward1Picture();
		frameCount++;

		if (pdlg->m_bCycle)
		{
			if (pdlg->m_pVidDecoder->Preview_beEOF())
			{
				nPos = pdlg->m_pVidDecoder->Preview_FirstPicture();
			}
		}

		pdlg->m_pdlgPlayController->m_sldFile.SetPos(nPos);

		int timeThread = (int)round(frameCount * frame_interval);
		while (pdlg->m_bPlaying)
		{
			DWORD dwNowTick = ::GetTickCount();
			int timeElapse = dwNowTick - dwStartTick;
			if (timeElapse >= timeThread)
			{
				if (frameCount == 1000000)
				{
					frameCount = 0;
					dwStartTick = dwNowTick;
				}
				break;
			}
		}

	} while(pdlg->m_bPlaying);

	return 0;
}

uint32_t AudioPlay_Thread(PVOID pVoid)
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

		if (pdlg->m_bCycle)
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

		pdlg->m_pdlgPlayController->m_sldFile.SetPos(nPos);

		frame_count ++;

//		Sleep(10);

//	} while(pdlg->m_bPlaying && (frame_count < 3000));
	} while(pdlg->m_bPlaying);

	return 0;
}

//void CDlg_ShowVideo::OnCheckCycle() 
//{
//	// TODO: Add your control notification handler code here
////	int bCycle;
//
//	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);
////	bCycle = pBtn->GetCheck();
//
//	m_bCycle = (m_bCycle == 0) ? 1 : 0;
//	pBtn->SetCheck(m_bCycle);
//
//	UpdateData(FALSE);
//}


LRESULT CDlg_ShowVideo::OnReportVideoDecodeFPS(WPARAM wParam, LPARAM lParam)
{
	uint32_t	FPS = (uint32_t)wParam;
	float fps;
	CString strFPS;

	fps = 0.001f * FPS;
	strFPS.Format("%.3f fps", fps);

	//CDC* pDC = GetDC();
	//pDC->TextOutA(10, 10, strFPS);
	//ReleaseDC(pDC);

	m_pdlgPlayController->ReportFPS(strFPS);

	return 0;
}

void CDlg_ShowVideo::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bFullScreen)
	{
		lpMMI->ptMaxSize.x = m_rectFullScreen.Width();
		lpMMI->ptMaxSize.y = m_rectFullScreen.Height();
		lpMMI->ptMaxPosition.x = m_rectFullScreen.left;
		lpMMI->ptMaxPosition.y = m_rectFullScreen.top;
		lpMMI->ptMaxTrackSize.x = m_rectFullScreen.Width();
		lpMMI->ptMaxTrackSize.y = m_rectFullScreen.Height();
	}

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlg_ShowVideo::EnlargeClientAreaToFullScreen(void)
{
	if (m_bFullScreen == false)
	{
		//获取系统屏幕宽高  
		//int g_iCurScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		//int g_iCurScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		//用m_struOldWndpl得到当前窗口的显示状态和窗体位置，以供退出全屏后使用  
		GetWindowPlacement(&m_stOldWndPlacement);

		//计算出窗口全屏显示客户端所应该设置的窗口大小，主要为了将不需要显示的窗体边框等部分排除在屏幕外  
		CRect rectWholeDlg;
		CRect rectClient;

		GetWindowRect(&rectWholeDlg);//得到当前窗体的总的相对于屏幕的坐标  
		RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &rectClient);//得到窗口客户区坐标  
		ClientToScreen(&rectClient);//将客户区相对窗体的坐标转为相对屏幕坐标  
		m_rectFullScreen.left = m_stOldWndPlacement.rcNormalPosition.left + (rectWholeDlg.left - rectClient.left);
		m_rectFullScreen.top = m_stOldWndPlacement.rcNormalPosition.top + (rectWholeDlg.top - rectClient.top);
		m_rectFullScreen.right = m_stOldWndPlacement.rcNormalPosition.right + (rectWholeDlg.right - rectClient.right);
		m_rectFullScreen.bottom = m_stOldWndPlacement.rcNormalPosition.bottom + (rectWholeDlg.bottom - rectClient.bottom);

		//设置窗口对象参数，为全屏做好准备并进入全屏状态  
		WINDOWPLACEMENT struWndpl;
		struWndpl.length = sizeof(WINDOWPLACEMENT);
		struWndpl.flags = 0;
		struWndpl.showCmd = SW_SHOWNORMAL;
		struWndpl.rcNormalPosition = m_rectFullScreen;
		SetWindowPlacement(&struWndpl);//该函数设置指定窗口的显示状态和显示大小位置等，是我们该程序最为重要的函数  

		if (m_pdlgPlayController != NULL)
		{
			CRect rectClient;
			GetClientRect(&rectClient);

			CRect rectDlg;
			m_pdlgPlayController->GetWindowRect(&rectDlg);

			m_pdlgPlayController->ShowWindow(SW_SHOW);

			int nXOffset = (rectClient.Width() - rectDlg.Width()) / 2;
			//int nYOffset = rectClient.bottom - rectDlg.Height();

			m_rectPlayControl.left = nXOffset;
			m_rectPlayControl.top = rectClient.bottom - rectDlg.Height();
			m_rectPlayControl.right = m_rectPlayControl.left + rectDlg.Width();
			m_rectPlayControl.bottom = rectClient.bottom;

			m_pdlgPlayController->MoveWindow(&m_rectPlayControl);
		}

		m_bFullScreen = true;
	}
}

void CDlg_ShowVideo::RestoreClientAreaToInitial(void)
{
	if (m_bFullScreen)
	{
		SetWindowPlacement(&m_stOldWndPlacement);

		//CenterWindow();
		if (m_pdlgPlayController != NULL)
		{
			CRect rectClient;
			GetClientRect(&rectClient);

			CRect rectDlg;
			m_pdlgPlayController->GetWindowRect(&rectDlg);

			m_pdlgPlayController->ShowWindow(SW_SHOW);

			int nXOffset = (rectClient.Width() - rectDlg.Width()) / 2;
			//int nYOffset = rectClient.bottom - rectDlg.Height();

			m_rectPlayControl.left = nXOffset;
			m_rectPlayControl.top = rectClient.bottom - rectDlg.Height();
			m_rectPlayControl.right = m_rectPlayControl.left + rectDlg.Width();
			m_rectPlayControl.bottom = rectClient.bottom;

			m_pdlgPlayController->MoveWindow(&m_rectPlayControl);
		}

		m_bFullScreen = false;
	}
}

void CDlg_ShowVideo::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (m_pdlgPlayController != NULL)
	{
		delete m_pdlgPlayController;
		m_pdlgPlayController = NULL;
	}
}


void CDlg_ShowVideo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pdlgPlayController->ShowWindow(SW_HIDE);
	KillTimer(m_dwTimerID);

	m_bForcingShowController = false;

	CDialog::OnTimer(nIDEvent);
}
