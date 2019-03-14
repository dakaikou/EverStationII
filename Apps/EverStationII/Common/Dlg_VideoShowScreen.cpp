// Dlg_M2VDecoder.cpp : implementation file
//

#include "stdafx.h"

#include <assert.h>

#include "GuiApi_MSG.h"
#include "Dlg_VideoShowScreen.h"
#include "../Magic_YUV/Dlg_YUVPreview.h"

#include "../MainFrm.h"
#include "../Magic_TS/TSMagic_GuiApi_MSG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_ShowVideo dialog
#define TARGET_RESOLUTION		 1         // 1-millisecond target resolution

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

CDlg_VideoShowScreen::CDlg_VideoShowScreen(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_VideoShowScreen::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_ShowVideo)
	//}}AFX_DATA_INIT
	m_nAudStreamType = STREAM_UNKNOWN;

	m_bPlayResponseStatus = 0;
	m_bPlayControllStatus = 0;

	m_bIsAudio = 0;
	m_bCycle = 1;
	m_bFrameRateCtrl = 1;
//	m_pdlgInfo = NULL;

	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		m_pVidDecoder[i] = NULL;
	}
	m_nVidDecoderCount = 0;
	m_pAudDecoder = NULL;

	m_bFullScreen = false;
	m_pPanelPlayController = NULL;
	m_pPanelChromaStats = NULL;
	m_pPanelLumaStats = NULL;

	m_bForcingShowController = true;
	m_bForcingShowStats = false;

	m_dwTimerID = 0xff56f344;
	m_nPlayProgressPercent = 0;

	m_dCheckedFrameRate = 0.0;
}


void CDlg_VideoShowScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_ShowVideo)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_VideoShowScreen, CDialog)
	//{{AFX_MSG_MAP(CDlg_ShowVideo)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOVE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_REPORT_VIDEO_DECODE_FPS, OnReportVideoDecodeFPS)
	ON_MESSAGE(WM_PLAY_CONTROLLER_REPORT_EXIT, OnReportPlayThreadExit)
	ON_MESSAGE(WM_PLAY_WORKING_PROGRESS, OnReportPlayThreadWorkingProgress)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_ShowVideo message handlers

//void CDlg_VideoShowScreen::OnClose()
//{
//	// TODO: Add your message handler code here and/or call default

	//::PostMessage(m_hParentWnd, WM_PLAY_THREAD_EXIT, 0, NULL);


////	m_ToolTip.Close();
//
//	if (m_bPlaying)
//	{
//		MessageBox("请先退出播放线程", "视频预览", MB_OK);
//	}
//	else
//	{
//		//m_pPanelPlayController->ShowWindow(SW_HIDE);
//		//m_pPanelChromaStats->ShowWindow(SW_HIDE);
//		//m_pPanelLumaStats->ShowWindow(SW_HIDE);
//
//		//if (m_pVidDecoder != NULL)
//		//{
//		//	HWND hWnd = this->GetSafeHwnd();
//		//	m_pVidDecoder->DetachWnd(hWnd);
//		//	m_pVidDecoder = NULL;
//		//}
//
//		//m_nVidStreamType = STREAM_UNKNOWN;
//
//		//if (!(m_nAudStreamType & STREAM_FILE))
//		//{
//		//	switch (m_nAudStreamType & (~STREAM_FILE))
//		//	{
//		//	case STREAM_MPEGAES:
//
//		//		((CMPEG_AudioDecoder*)m_pAudDecoder)->CloseAudio();
//		//		break;
//
//		//	case STREAM_AC3AES:
//
//		//		((CAC3_AudioDecoder*)m_pAudDecoder)->CloseAudio();
//		//		break;
//
//		//	case STREAM_WAVEAES:
//
//		//		((CWAVE_AudioDecoder*)m_pAudDecoder)->CloseAudio();
//		//		break;
//
//		//	default:
//		//		break;
//		//	}
//		//}
//
//		//m_nAudStreamType = STREAM_UNKNOWN;
//
//		//m_pAudDecoder = NULL;
//
//		//m_bFullScreen = false;
//
//		ShowWindow(SW_HIDE);
//
//		CDialog::OnClose();
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

void CDlg_VideoShowScreen::AttachVideoDecoder(PVOID pDecoder)
{
	//assert(m_pVidDecoder == NULL);

	if (pDecoder != NULL)
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] == NULL)
			{
				m_pVidDecoder[i] = (CVESDecoder*)pDecoder;
				m_nVidDecoderCount++;

				//char pszTitle[256];
				//if (m_pVidDecoder[i]->GetTitle(pszTitle, sizeof(pszTitle)) == ESDECODER_NO_ERROR)
				//{
				//	SetWindowText(pszTitle);
				//}
				HWND hWnd = this->GetSafeHwnd();
				m_pVidDecoder[i]->AttachWnd(hWnd, CALLBACK_report_yuv_luma_stats, CALLBACK_report_yuv_chroma_stats);

				m_nPlayProgressPercent = 0;

				break;
			}
		}
	}
}

void CDlg_VideoShowScreen::DetachVideoDecoder(PVOID pDecoder)
{
	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			//assert(m_pVidDecoder == pDecoder);

			if (m_pVidDecoder[i] == pDecoder)
			{
				HWND hWnd = this->GetSafeHwnd();
				m_pVidDecoder[i]->DetachWnd(hWnd);

				if (m_pPanelPlayController != NULL)
				{
					m_pPanelPlayController->InformStopped();
				}

				m_pVidDecoder[i] = NULL;
				m_nVidDecoderCount--;

				break;
			}

			//inform the playing thread to exit
		}
	}
}

void CDlg_VideoShowScreen::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here

	if (bShow)
	{
		if (m_bPlayResponseStatus == 0)			
		{
			SetupDisplayWnd();

			for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
			{
				if (m_pVidDecoder[i] != NULL)
				{
					m_nPlayProgressPercent = m_pVidDecoder[i]->Preview_CurPicture();
				}
			}

			if (m_pPanelPlayController != NULL)
			{
				m_pPanelPlayController->m_sldFile.SetPos(m_nPlayProgressPercent);
			}
		}

		if (m_pPanelPlayController != NULL)
		{
			m_pPanelPlayController->ShowWindow(SW_SHOW);
		}

		if (m_pPanelLumaStats != NULL)
		{
			m_pPanelLumaStats->ShowWindow(SW_HIDE);
		}

		if (m_pPanelChromaStats != NULL)
		{
			m_pPanelChromaStats->ShowWindow(SW_HIDE);
		}

		m_bForcingShowController = true;
		m_bForcingShowStats = false;

		SetTimer(m_dwTimerID, 200, NULL);
	}
	else
	{
		KillTimer(m_dwTimerID);

		if (m_pPanelPlayController != NULL)
		{
			m_pPanelPlayController->ShowWindow(SW_HIDE);
		}
		if (m_pPanelLumaStats != NULL)
		{
			m_pPanelLumaStats->ShowWindow(SW_HIDE);
		}
		if (m_pPanelChromaStats != NULL)
		{
			m_pPanelChromaStats->ShowWindow(SW_HIDE);
		}
		m_bForcingShowStats = false;

		m_bFullScreen = false;
	}
}

void CDlg_VideoShowScreen::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			m_pVidDecoder[i]->DirectDraw_RePaint();
		}
	}
}

int CDlg_VideoShowScreen::PreviewNextFrame() 
{
	// TODO: Add your control notification handler code here
	int nPercent = -1;
	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			//switch (m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_AC3AES:

			//	nPercent = ((CAC3_AudioDecoder*)m_pAudDecoder)->Preview_NextFrame(1);
			//	break;

			//case STREAM_MPEGAES:

			//	nPercent = ((CMPEG_AudioDecoder*)m_pAudDecoder)->Preview_NextFrame(1);
			//	break;

			//case STREAM_WAVEAES:

			//	nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_NextFrame(1);
			//	break;

			//default:
			//	break;
			//}
		}
	}
	else
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			{
				nPercent = m_pVidDecoder[i]->Preview_Forward1Picture();
				m_nPlayProgressPercent = nPercent;
			}
		}
	}

	return nPercent;
}

int CDlg_VideoShowScreen::PreviewNext5Frame() 
{
	// TODO: Add your control notification handler code here
	int nPercent = -1;

	if (m_bIsAudio)
	{
	}
	else
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			{
				nPercent = m_pVidDecoder[i]->Preview_ForwardNPicture(5);
				m_nPlayProgressPercent = nPercent;
			}
		}
	}

	return nPercent;
}

BOOL CDlg_VideoShowScreen::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pPanelPlayController = new CDlg_VideoPanelPlayController;
	m_pPanelPlayController->Create(IDD_SHOW_VIDEO_CTRL, this);

	m_pPanelLumaStats = new CDlg_VideoPanelLumaStats;
	m_pPanelLumaStats->Create(IDD_SHOW_VIDEO_LUMINANCE, this);
	m_pPanelLumaStats->ShowWindow(SW_HIDE);

	m_pPanelChromaStats = new CDlg_VideoPanelChromaStats;
	m_pPanelChromaStats->Create(IDD_SHOW_VIDEO_CHROMA, this);
	m_pPanelChromaStats->ShowWindow(SW_HIDE);

	m_pPanelPlayController->m_sldFile.SetRange(0, 100);
	m_pPanelPlayController->m_sldFile.SetPos(0);
	m_pPanelPlayController->m_sldFile.SetPageSize(10);

#if USE_SURFACE_ACCESS_MUTEX
	m_hProgressDataAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif

	//m_bkBrush.CreateSolidBrush(RGB(255, 180, 100));		//prepare for background

	//::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//::SetLayeredWindowAttributes(m_hWnd, 0, 200, LWA_ALPHA); // 120是透明度，范围是0～255  

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_VideoShowScreen::SaveSnapshot(void) 
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
	
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			m_pVidDecoder[i]->SaveSnapshot(strFile.GetBuffer(128));
		}
//		DirectDraw_Save(strFile.GetBuffer(128));
	}
}

void CDlg_VideoShowScreen::CanvasEnlarge(void)
{
	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			m_pVidDecoder[i]->CanvasEnlarge();
		}
	}

	SetupDisplayWnd();

	Invalidate();		//必须增加这句调用，强制窗口重绘客户区，否则DirectDraw的窗口变化后，原来的画面还会残留在窗口客户区
}

void CDlg_VideoShowScreen::CanvasReduce(void)
{
	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			m_pVidDecoder[i]->CanvasReduce();
		}
	}

	SetupDisplayWnd();

	Invalidate();		//必须增加这句调用，强制窗口重绘客户区，否则DirectDraw的窗口变化后，原来的画面还会残留在窗口客户区
}


void CDlg_VideoShowScreen::SetupDisplayWnd(void)
{
	RECT rcWnd;
	RECT rcClient;
	GetClientRect(&rcClient);
	rcWnd = rcClient;

	int total_width = 0;
	int total_height = 0;
	int width, height;

	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			m_pVidDecoder[i]->GetCanvasWH(&width, &height);
			total_width += width;
			total_height += height;
		}
	}

	rcWnd.left = rcClient.left + ((rcClient.right - rcClient.left) - total_width) / 2;

	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			m_pVidDecoder[i]->GetCanvasWH(&width, &height);
			rcWnd.right = rcWnd.left + width;

			m_pVidDecoder[i]->SetClientRect(rcWnd);

			rcWnd.left = rcWnd.right;
		}
	}
}

//void CDlg_VideoShowScreen::ToggleView(void)
//{
//	if (m_pVidDecoder != NULL)
//	{
//		m_pVidDecoder->ToggleView();
//
//		Invalidate();		//必须增加这句调用，强制窗口重绘客户区，否则DirectDraw的窗口变化后，原来的画面还会残留在窗口客户区
//	}
//}

void CDlg_VideoShowScreen::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
}

void CDlg_VideoShowScreen::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	//if (m_bForcingShowController == 0)
	{
		if (m_rectPlayControl.PtInRect(point))
		{
			m_pPanelPlayController->ShowWindow(SW_SHOW);
		}
		else
		{
			m_pPanelPlayController->ShowWindow(SW_HIDE);
		}
	}

	//if (m_bForcingShowStats == 0)
	//{
	//	if (m_rectLumaStats.PtInRect(point))
	//	{
	//		m_pPanelLumaStats->ShowWindow(SW_SHOW);
	//	}
	//	else
	//	{
	//		m_pPanelLumaStats->ShowWindow(SW_HIDE);
	//	}

	//	if (m_rectChromaStats.PtInRect(point))
	//	{
	//		m_pPanelChromaStats->ShowWindow(SW_SHOW);
	//	}
	//	else
	//	{
	//		m_pPanelChromaStats->ShowWindow(SW_HIDE);
	//	}
	//}
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

void CDlg_VideoShowScreen::ToggleGrid(void) 
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

	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (m_pVidDecoder[i] != NULL)
		{
			m_pVidDecoder[i]->ToggleGrid();
		}
	}
}

void CDlg_VideoShowScreen::OnLButtonDblClk(UINT nFlags, CPoint point) 
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

void CDlg_VideoShowScreen::ShowStats(void)
{
	// TODO: Add your control notification handler code here
	if (m_bForcingShowStats == 0)
	{
		m_bForcingShowStats = 1;
		m_pPanelChromaStats->ShowWindow(SW_SHOW);
		m_pPanelLumaStats->ShowWindow(SW_SHOW);
	}
	else
	{
		m_bForcingShowStats = 0;
		m_pPanelChromaStats->ShowWindow(SW_HIDE);
		m_pPanelLumaStats->ShowWindow(SW_HIDE);
	}
}

int CDlg_VideoShowScreen::PreviewFirstFrame() 
{
	// TODO: Add your control notification handler code here
	int nPercent = -1;

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			//switch (m_nAudStreamType & (~STREAM_FILE))
			//{
			//case STREAM_AC3AES:
			//	nPercent = ((CAC3_AudioDecoder*)m_pAudDecoder)->Preview_FirstFrame();
			//	break;

			//case STREAM_MPEGAES:
			//	nPercent = ((CMPEG_AudioDecoder*)m_pAudDecoder)->Preview_FirstFrame();
			//	break;

			//case STREAM_WAVEAES:
			//	nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_FirstFrame();
			//	break;

			//default:
			//	break;
			//}
		}
	}
	else
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			{
				nPercent = m_pVidDecoder[i]->Preview_FirstPicture();
				m_nPlayProgressPercent = nPercent;
			}
		}
	}

	return nPercent;
}

int CDlg_VideoShowScreen::PreviewLastFrame() 
{
	// TODO: Add your control notification handler code here
	int nPercent = -1;

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
			switch (m_nAudStreamType & (~STREAM_FILE))
			{
			case STREAM_WAVEAES:
//				nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_LastFrame();
				break;

			default:
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			{
				nPercent = m_pVidDecoder[i]->Preview_LastPicture();
				m_nPlayProgressPercent = nPercent;
			}
		}
	}

	return nPercent;
}

int CDlg_VideoShowScreen::PreviewPre5Frame() 
{
	// TODO: Add your control notification handler code here
	int nPercent = -1;

	if (m_bIsAudio)
	{
/*		if (m_nAudStreamType & STREAM_FILE)
		{
			switch (m_nAudStreamType & (~STREAM_FILE))
			{
			case STREAM_WAVEAES:
				nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_LastFrame();
				break;

			default:
				break;
			}
		}
*/
	}
	else
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			{
				nPercent = m_pVidDecoder[i]->Preview_BackwardNPicture(5);
				m_nPlayProgressPercent = nPercent;
			}
		}
	}

	return nPercent;
}

int CDlg_VideoShowScreen::PreviewPreFrame() 
{
	// TODO: Add your control notification handler code here
	int nPercent = -1;

	if (m_bIsAudio)
	{
		if (m_nAudStreamType & STREAM_FILE)
		{
//			switch (m_nAudStreamType & (~STREAM_FILE))
//			{
//			case STREAM_WAVEAES:
////				nPercent = ((CWAVE_AudioDecoder*)m_pAudDecoder)->Preview_PreFrame();
//				break;
//
//			default:
//				break;
//			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
		{
			if (m_pVidDecoder[i] != NULL)
			{
				nPercent = m_pVidDecoder[i]->Preview_Backward1Picture();
				m_nPlayProgressPercent = nPercent;
			}
		}
	}

	return nPercent;
}

void CDlg_VideoShowScreen::StartVideoPlayThread(void)
{
	TIMECAPS tc;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
	{
		// Error; application can't continue.
		UINT maxResolution = max(tc.wPeriodMin, TARGET_RESOLUTION);
		m_wTimerRes = min(maxResolution, tc.wPeriodMax);

		double frame_rate = m_pVidDecoder[0]->GetDisplayFrameRate();
		UINT frame_interval = (UINT)round(1000.0 / frame_rate);

		m_mmTimerID = timeSetEvent(
			frame_interval,
			m_wTimerRes,
			VideoPlay_TimerHandler,
			(DWORD_PTR)this,
			TIME_PERIODIC);

		timeBeginPeriod(m_wTimerRes);

		m_bPlayResponseStatus = 0;
		m_bPlayControllStatus = 1;

		//::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)AudioPlay_Thread, this, 0, 0);
		//::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)VideoPlay_Thread, this, 0, 0);

		//while (m_bPlayResponseStatus == 0)
		//{
		//	Sleep(10);
		//}
	}
	else
	{

	}
}

void CDlg_VideoShowScreen::StopVideoPlayThread(void)
{
	timeEndPeriod(m_wTimerRes);
	timeKillEvent(m_mmTimerID);

	m_bPlayControllStatus = 0;

	while (m_bPlayResponseStatus == 1)
	{
		Sleep(10);
	}

	if (m_pPanelPlayController != NULL)
	{
		m_pPanelPlayController->InformStopped();
	}
}

void CALLBACK VideoPlay_TimerHandler(UINT uTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	CDlg_VideoShowScreen* pdlg = (CDlg_VideoShowScreen*)dwUser;

	//int				nOldPercent = 0;
	int				nNewPercent = -1;

	for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
	{
		if (pdlg->m_pVidDecoder[i] != NULL)
		{
			if (pdlg->m_pVidDecoder[i]->Preview_beEOF())
			{
				if (pdlg->m_bCycle)
				{
					nNewPercent = pdlg->m_pVidDecoder[i]->Preview_FirstPicture();
				}
			}
			else
			{
				if (pdlg->m_pVidDecoder[i] != NULL)
				{
					nNewPercent = pdlg->m_pVidDecoder[i]->Preview_Forward1Picture();
				}
			}
		}
	}

	if (nNewPercent >= 0)
	{
		pdlg->m_nPlayProgressPercent = nNewPercent;
		//if (nNewPercent != nOldPercent)
		//{
		//	//频繁向界面发送消息，会阻塞界面响应其他按键消息，谨慎调用该语句
		//	//::PostMessage(pdlg->GetSafeHwnd(), WM_PLAY_WORKING_PROGRESS, NULL, nNewPercent);
		//	nOldPercent = nNewPercent;

		//	//pdlg->OnReportPlayThreadWorkingProgress(NULL, nNewPercent);
		//}
	}
}

//uint32_t VideoPlay_Thread(PVOID pVoid)
//{
//	CDlg_VideoShowScreen* pdlg = (CDlg_VideoShowScreen*)pVoid;
//	int				nOldPercent = 0;
//	int				nNewPercent;
//	DWORD			dwStartTick = 0;
//	int				frameCount = 0;
//
//	//Video_decode_info_t  video_decode_info;
//	double frame_rate = pdlg->m_pVidDecoder->GetDisplayFrameRate();
//
//	double frame_interval = 1000.0 / frame_rate;
//	int	   frameDeviation = (int)(5 * frame_interval);
//
//	pdlg->m_bPlayResponseStatus = 1;
//	do
//	{
//		//if (pdlg->m_pVidDecoder != NULL)
//		//{
//		//	nNewPercent = -1;
//		//	if (pdlg->m_pVidDecoder->Preview_beEOF())
//		//	{
//		//		if (pdlg->m_bCycle)
//		//		{
//		//			nNewPercent = pdlg->m_pVidDecoder->Preview_FirstPicture();
//		//		}
//		//	}
//		//	else
//		//	{
//		//		nNewPercent = pdlg->m_pVidDecoder->Preview_Forward1Picture();
//		//	}
//
//		//	if (nNewPercent >= 0)
//		//	{
//		//		pdlg->m_nPlayProgressPercent = nNewPercent;
//
//		//		if (frameCount == 0)
//		//		{
//		//			dwStartTick = ::GetTickCount();
//		//		}
//
//		//		if (nNewPercent != nOldPercent)
//		//		{
//		//			//频繁向界面发送消息，会阻塞界面响应其他按键消息，谨慎调用该语句
//		//			//::PostMessage(pdlg->GetSafeHwnd(), WM_PLAY_WORKING_PROGRESS, NULL, nNewPercent);
//		//			nOldPercent = nNewPercent;
//
//		//			pdlg->OnReportPlayThreadWorkingProgress(NULL, nNewPercent);
//		//		}
//
//		//		//停下来等
//		//		if (pdlg->m_bFrameRateCtrl)
//		//		{
//		//			frameCount++;
//
//		//			int timeThreadHold = (int)round(frameCount * frame_interval);
//		//			while (pdlg->m_bPlayControllStatus == 1)
//		//			{
//		//				DWORD dwNowTick = ::GetTickCount();				//时间精度10~16ms，很难满足多媒体播放精度要求，待改进   chendelin 2019.3.10
//		//				int timeElapse = dwNowTick - dwStartTick;
//		//				if (timeElapse < 0)
//		//				{
//		//					//this case occured when the time tick counter overflow
//		//					frameCount = 0;
//		//					break;
//		//				}
//		//				else
//		//				{
//		//					if (timeElapse >= timeThreadHold)
//		//					{
//		//						//normally this deviation is less than 1 frame interval
//		//						if ((timeElapse - timeThreadHold) > frameDeviation)
//		//						{
//		//							frameCount = 0;
//		//						}
//		//						else 
//		//						{
//		//							if (frameCount >= 100000000)
//		//							{
//		//								frameCount = 0;
//		//								//dwStartTick = dwNowTick - (int)round(frameCount * frame_interval);
//		//							}
//		//						}
//
//		//						break;
//		//					}
//		//				}
//		//			}
//		//		}
//		//		else
//		//		{
//		//			frameCount = 0;
//		//		}
//		//	}
//		//	else
//		//	{
//		//		//assert(0);
//		//		//break;
//		//	}
//		//}
//		//else
//		//{
//		//	assert(0);
//		//	break;
//		//}
//
//		Sleep((DWORD)round(frame_interval));
//
//	} while(pdlg->m_bPlayControllStatus == 1);
//
//	pdlg->m_bPlayResponseStatus = 0;
//
//	return 0;
//}

//uint32_t AudioPlay_Thread(PVOID pVoid)
//{
//	CDlg_VideoShowScreen* pdlg = (CDlg_VideoShowScreen*)pVoid;
//	int				nPercent = 0;
//	int				frame_count = 0;
//
//	pdlg->m_bPlayResponseStatus = 1;
//
//	do
//	{
//		if (pdlg->m_nAudStreamType & STREAM_FILE)
//		{
//			//switch (pdlg->m_nAudStreamType & (~STREAM_FILE))
//			//{
//			//case STREAM_AC3AES:
//			//	nPercent = ((CAC3_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_NextFrame(0);
//			//	break;
//
//			//case STREAM_MPEGAES:
//			//	nPercent = ((CMPEG_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_NextFrame(0);
//			//	break;
//
//			//case STREAM_WAVEAES:
//			//	nPercent = ((CWAVE_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_NextFrame(0);
//			//	break;
//
//			//default:
//			//	break;
//			//}
//		}
//
//		if (pdlg->m_bCycle)
//		{
//			if (pdlg->m_nAudStreamType & STREAM_FILE)
//			{
//				//switch (pdlg->m_nAudStreamType & (~STREAM_FILE))
//				//{
//				//case STREAM_AC3AES:
//				//	if (((CAC3_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_EOF())
//				//	{
//				//		nPercent = ((CAC3_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_FirstFrame();
//				//	}
//				//	break;
//
//				//case STREAM_MPEGAES:
//				//	if (((CMPEG_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_EOF())
//				//	{
//				//		nPercent = ((CMPEG_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_FirstFrame();
//				//	}
//				//	break;
//
//				//case STREAM_WAVEAES:
//				//	if (((CWAVE_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_EOF())
//				//	{
//				//		nPercent = ((CWAVE_AudioDecoder*)pdlg->m_pAudDecoder)->Preview_FirstFrame();
//				//	}
//				//	break;
//
//				//default:
//				//	break;
//				//}
//			}
//		}
//
//		//pdlg->m_pPanelPlayController->m_sldFile.SetPos(nPercent);
//
//		frame_count ++;
//
////		Sleep(10);
//
////	} while(pdlg->m_bPlaying && (frame_count < 3000));
//	} while(pdlg->m_bPlayControllStatus);
//
//	pdlg->m_bPlayResponseStatus = 0;
//
//	return 0;
//}

int CALLBACK_report_yuv_luma_stats(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CDlg_VideoShowScreen* pdlg = (CDlg_VideoShowScreen*)CWnd::FromHandle(hWnd);

	pdlg->m_pPanelLumaStats->ReportStats(wParam, lParam);

	return 0;
}

int CALLBACK_report_yuv_chroma_stats(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CDlg_VideoShowScreen* pdlg = (CDlg_VideoShowScreen*)CWnd::FromHandle(hWnd);

	pdlg->m_pPanelChromaStats->ReportStats(wParam, lParam);

	return 0;
}

LRESULT CDlg_VideoShowScreen::OnReportVideoDecodeFPS(WPARAM wParam, LPARAM lParam)
{
	uint32_t	fps = (uint32_t)lParam;
	m_dCheckedFrameRate = 0.001 * fps;

	return 0;
}

LRESULT CDlg_VideoShowScreen::OnReportPlayThreadExit(WPARAM wParam, LPARAM lParam)
{
	//消息接力
	::PostMessage(m_hParentWnd, WM_VIDEO_CONTAINER_REPORT_PLAY_EXIT, 0, NULL);

	return 0;
}

LRESULT CDlg_VideoShowScreen::OnReportPlayThreadWorkingProgress(WPARAM wParam, LPARAM lParam)
{
	//m_pPanelPlayController->m_sldFile.SetPos((int)lParam);
#if USE_PROGRESS_DATA_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hProgressDataAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		//To Do anything here
		m_nPlayProgressPercent = (int)lParam;

#if USE_SURFACE_ACCESS_MUTEX
		::ReleaseMutex(m_hProgressDataAccess);
	}
#endif

	return 0;
}

void CDlg_VideoShowScreen::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
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

void CDlg_VideoShowScreen::EnlargeClientAreaToFullScreen(void)
{
	if (m_bFullScreen == false)
	{
		//用m_struOldWndpl得到当前窗口的显示状态和窗体位置，以供退出全屏后使用  
		GetWindowPlacement(&m_stOldWndPlacement);

		//计算出窗口全屏显示客户端所应该设置的窗口大小，主要为了将不需要显示的窗体边框等部分排除在屏幕外  
		CRect rectWholeDlg;
		CRect rectClient;
		CRect rectDlg;

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

		GetClientRect(&rectClient);		//获得的是相对坐标

		if (m_pPanelPlayController != NULL)
		{
			m_pPanelPlayController->GetWindowRect(&rectDlg);

			int nXOffset = (rectClient.Width() - PLAYCTRL_BKBMP_WIDTH) / 2;

			m_rectPlayControl.left = nXOffset;
			//m_rectPlayControl.top = rectClient.bottom - rectDlg.Height();
			m_rectPlayControl.top = rectClient.bottom - PLAYCTRL_BKBMP_HEIGHT - 70;
			m_rectPlayControl.right = m_rectPlayControl.left + PLAYCTRL_BKBMP_WIDTH;
			m_rectPlayControl.bottom = rectClient.bottom;

			CRect rectPlayControl = m_rectPlayControl;
			ClientToScreen(&rectPlayControl);

			struWndpl.length = sizeof(WINDOWPLACEMENT);
			struWndpl.flags = 0;
			struWndpl.showCmd = SW_SHOWNORMAL;
			struWndpl.rcNormalPosition = rectPlayControl;
			BOOL bStatus = m_pPanelPlayController->SetWindowPlacement(&struWndpl);

			m_pPanelPlayController->ShowWindow(SW_SHOW);
		}

		if (m_pPanelLumaStats != NULL)
		{
			m_pPanelLumaStats->GetWindowRect(&rectDlg);

			m_rectLumaStats.left = rectClient.right - rectDlg.Width();
			m_rectLumaStats.top = rectClient.top;
			m_rectLumaStats.right = rectClient.right;
			m_rectLumaStats.bottom = m_rectLumaStats.top + rectDlg.Height();

			CRect rectLumaStats = m_rectLumaStats;
			ClientToScreen(&rectLumaStats);

			struWndpl.length = sizeof(WINDOWPLACEMENT);
			struWndpl.flags = 0;
			struWndpl.showCmd = SW_SHOWNORMAL;
			struWndpl.rcNormalPosition = rectLumaStats;
			m_pPanelLumaStats->SetWindowPlacement(&struWndpl);

			m_pPanelLumaStats->ShowWindow(SW_HIDE);
		}

		if (m_pPanelChromaStats != NULL)
		{
			m_pPanelChromaStats->GetWindowRect(&rectDlg);

			m_rectChromaStats.left = rectClient.right - rectDlg.Width();
			m_rectChromaStats.top = m_rectLumaStats.bottom;
			m_rectChromaStats.right = rectClient.right;
			m_rectChromaStats.bottom = m_rectChromaStats.top + rectDlg.Height();

			CRect rectChromaStats = m_rectChromaStats;
			ClientToScreen(&rectChromaStats);

			struWndpl.length = sizeof(WINDOWPLACEMENT);
			struWndpl.flags = 0;
			struWndpl.showCmd = SW_SHOWNORMAL;
			struWndpl.rcNormalPosition = rectChromaStats;
			m_pPanelChromaStats->SetWindowPlacement(&struWndpl);

			m_pPanelChromaStats->ShowWindow(SW_HIDE);
		}

		m_bFullScreen = true;
	}
}

void CDlg_VideoShowScreen::RestoreClientAreaToInitial(void)
{
	if (m_bFullScreen)
	{
		SetWindowPlacement(&m_stOldWndPlacement);

		WINDOWPLACEMENT struWndpl;
		CRect rectClient;
		CRect rectDlg;
		GetClientRect(&rectClient);

		if (m_pPanelPlayController != NULL)
		{
			m_pPanelPlayController->GetWindowRect(&rectDlg);

			int nXOffset = (rectClient.Width() - PLAYCTRL_BKBMP_WIDTH) / 2;

			m_rectPlayControl.left = nXOffset;
			//m_rectPlayControl.top = rectClient.bottom - rectDlg.Height();
			m_rectPlayControl.top = rectClient.bottom - PLAYCTRL_BKBMP_HEIGHT - 70;
			//m_rectPlayControl.right = m_rectPlayControl.left + rectDlg.Width();
			m_rectPlayControl.right = m_rectPlayControl.left + PLAYCTRL_BKBMP_WIDTH;
			m_rectPlayControl.bottom = rectClient.bottom;

			CRect rectPlayControl = m_rectPlayControl;
			ClientToScreen(&rectPlayControl);

			struWndpl.length = sizeof(WINDOWPLACEMENT);
			struWndpl.flags = 0;
			struWndpl.showCmd = SW_SHOWNORMAL;
			struWndpl.rcNormalPosition = rectPlayControl;
			BOOL bStatus = m_pPanelPlayController->SetWindowPlacement(&struWndpl);

			m_pPanelPlayController->ShowWindow(SW_SHOW);
		}

		if (m_pPanelLumaStats != NULL)
		{
			m_pPanelLumaStats->GetWindowRect(&rectDlg);

			m_rectLumaStats.left = rectClient.right - rectDlg.Width();
			m_rectLumaStats.top = rectClient.top;
			m_rectLumaStats.right = rectClient.right;
			m_rectLumaStats.bottom = m_rectLumaStats.top + rectDlg.Height();

			CRect rectLumaStats = m_rectLumaStats;
			ClientToScreen(&rectLumaStats);

			struWndpl.length = sizeof(WINDOWPLACEMENT);
			struWndpl.flags = 0;
			struWndpl.showCmd = SW_SHOWNORMAL;
			struWndpl.rcNormalPosition = rectLumaStats;
			m_pPanelLumaStats->SetWindowPlacement(&struWndpl);

			m_pPanelLumaStats->ShowWindow(SW_HIDE);
		}

		if (m_pPanelChromaStats != NULL)
		{
			m_pPanelChromaStats->GetWindowRect(&rectDlg);

			m_rectChromaStats.left = rectClient.right - rectDlg.Width();
			m_rectChromaStats.top = m_rectLumaStats.bottom;
			m_rectChromaStats.right = rectClient.right;
			m_rectChromaStats.bottom = m_rectChromaStats.top + rectDlg.Height();

			CRect rectChromaStats = m_rectChromaStats;
			ClientToScreen(&rectChromaStats);

			struWndpl.length = sizeof(WINDOWPLACEMENT);
			struWndpl.flags = 0;
			struWndpl.showCmd = SW_SHOWNORMAL;
			struWndpl.rcNormalPosition = rectChromaStats;
			m_pPanelChromaStats->SetWindowPlacement(&struWndpl);

			m_pPanelChromaStats->ShowWindow(SW_HIDE);
		}

		m_bFullScreen = false;

		//Invalidate();
	}
}

void CDlg_VideoShowScreen::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (m_pPanelPlayController != NULL)
	{
		delete m_pPanelPlayController;
		m_pPanelPlayController = NULL;
	}

	if (m_pPanelLumaStats != NULL)
	{
		delete m_pPanelLumaStats;
		m_pPanelLumaStats = NULL;
	}

	if (m_pPanelChromaStats != NULL)
	{
		delete m_pPanelChromaStats;
		m_pPanelChromaStats = NULL;
	}

#if USE_PROGRESS_DATA_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hProgressDataAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		//To Do anything here

#if USE_SURFACE_ACCESS_MUTEX
		::ReleaseMutex(m_hProgressDataAccess);
		::CloseHandle(m_hProgressDataAccess);
		m_hProgressDataAccess = NULL;
	}
#endif

}


void CDlg_VideoShowScreen::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == m_dwTimerID)
	{
#if USE_PROGRESS_DATA_ACCESS_MUTEX
		uint32_t wait_state = ::WaitForSingleObject(m_hProgressDataAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
#endif
			m_pPanelPlayController->m_sldFile.SetPos(m_nPlayProgressPercent);

			CString strFPS;
			strFPS.Format("%.3f fps", m_dCheckedFrameRate);

			//CDC* pDC = GetDC();
			//pDC->TextOutA(10, 10, strFPS);
			//ReleaseDC(pDC);

			m_pPanelPlayController->ReportFPS(strFPS);

#if USE_PROGRESS_DATA_ACCESS_MUTEX
			::ReleaseMutex(m_hProgressDataAccess);
		}
#endif
	}

	//m_pPanelPlayController->ShowWindow(SW_HIDE);
	////m_pPanelChromaStats->ShowWindow(SW_HIDE);
	////m_pPanelLumaStats->ShowWindow(SW_HIDE);

	//m_bForcingShowController = false;
	////m_bForcingShowStats = false;

	CDialog::OnTimer(nIDEvent);
}


int CDlg_VideoShowScreen::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_hParentWnd = lpCreateStruct->hwndParent;

	return 0;
}


void CDlg_VideoShowScreen::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	::PostMessage(m_hParentWnd, WM_VIDEO_CONTAINER_REPORT_PLAY_EXIT, 0, NULL);

	CDialog::OnClose();
}


HBRUSH CDlg_VideoShowScreen::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	//if (nCtlColor == CTLCOLOR_DLG)   // 判断是否是对话框  
	//{
	//	return   m_bkBrush; // 返回刚才创建的背景刷子  
	//}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
