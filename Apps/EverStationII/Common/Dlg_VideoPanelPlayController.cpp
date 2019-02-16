// Dlg_PlayerController.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "GuiApi_MSG.h"
#include "Dlg_VideoPanelPlayController.h"
#include "afxdialogex.h"


// CDlg_PlayerController 对话框

IMPLEMENT_DYNAMIC(CDlg_VideoPanelPlayController, CDialogEx)

CDlg_VideoPanelPlayController::CDlg_VideoPanelPlayController(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOW_VIDEO_CTRL, pParent)
{
}

CDlg_VideoPanelPlayController::~CDlg_VideoPanelPlayController()
{
}

void CDlg_VideoPanelPlayController::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_FILE, m_sldFile);
}


BEGIN_MESSAGE_MAP(CDlg_VideoPanelPlayController, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_PLAY_OR_STOP, &CDlg_VideoPanelPlayController::OnClickedBtnPlayOrStop)
	ON_BN_CLICKED(IDC_BTN_PRE_5FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnPre5frame)
	ON_BN_CLICKED(IDC_BTN_PRE_FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnPreFrame)
	ON_BN_CLICKED(IDC_BTN_NEXT_FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnNextFrame)
	ON_BN_CLICKED(IDC_BTN_NEXT_5FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnNext5frame)
	ON_BN_CLICKED(IDC_BTN_GRID, &CDlg_VideoPanelPlayController::OnClickedBtnGrid)
	ON_BN_CLICKED(IDC_BTN_INFO, &CDlg_VideoPanelPlayController::OnClickedBtnInfo)
	ON_BN_CLICKED(IDC_BTN_LAST_FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnLastFrame)
	ON_BN_CLICKED(IDC_BTN_FIRST_FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnFirstFrame)
	ON_BN_CLICKED(IDC_CHECK_CYCLE, &CDlg_VideoPanelPlayController::OnClickedCheckCycle)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_SAVE_BMP, &CDlg_VideoPanelPlayController::OnClickedBtnSaveBmp)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDlg_VideoPanelPlayController::OnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_ENLARGE_SCREEN, &CDlg_VideoPanelPlayController::OnClickedBtnEnlargeScreen)
END_MESSAGE_MAP()


// CDlg_PlayerController 消息处理程序


void CDlg_VideoPanelPlayController::OnClickedBtnPlayOrStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	if (pFatherDlg->m_bPlaying == 0)
	{
		//if (m_nAudStreamType & STREAM_FILE)		//如果是播放音频文件
		//{
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
		//}

		//if (m_nVidStreamType & STREAM_FILE)
		//{
		//			switch (m_nVidStreamType & (~STREAM_FILE))
		//			{
		//			case STREAM_PS:
		//				//((CMPEG_PS_Decoder*)m_pVidDecoder)->OpenAudio(hWnd);
		////				((CMPEG_PS_Decoder*)m_pVidDecoder)->ExtDebug();
		//				break;
		//			}
		//}

		//pWnd->SetWindowText("停止");

		pFatherDlg->m_bPlaying = 1;

		DWORD dwID;
		if (pFatherDlg->m_bIsAudio)
		{
			::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)AudioPlay_Thread, pFatherDlg, 0, &dwID);
		}
		else
		{
			::CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)VideoPlay_Thread, pFatherDlg, 0, &dwID);
		}

		//m_sldFile.EnableWindow(FALSE);
	}
	else
	{
		//if (m_nAudStreamType & STREAM_FILE)
		//{
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
		//}

		//if (m_nVidStreamType & STREAM_FILE)
		//{
		//switch (m_nVidStreamType & (~STREAM_FILE))
		//{
		//case STREAM_PS:
		//	((CMPEG_PS_Decoder*)m_pVidDecoder)->CloseAudio();
		//	break;
		//}
		//}

		//pWnd->SetWindowText("播放");

		pFatherDlg->m_bPlaying = 0;

		//m_sldFile.EnableWindow(TRUE);

		CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
		pWnd->SetWindowText("");
	}

	SetCtrlButtonStates(1, pFatherDlg->m_bPlaying, 0);
}

void CDlg_VideoPanelPlayController::OnClickedBtnPre5frame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewPre5Frame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnPreFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewPreFrame();

	m_sldFile.SetPos(nPos);
}

void CDlg_VideoPanelPlayController::OnClickedBtnNextFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewNextFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnNext5frame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewNext5Frame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnGrid()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->ToggleGrid();
}


void CDlg_VideoPanelPlayController::OnClickedBtnInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->ShowStats();
}


void CDlg_VideoPanelPlayController::OnClickedBtnLastFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewLastFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnFirstFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewFirstFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnSaveBmp()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->SaveSnapshot();
}


void CDlg_VideoPanelPlayController::OnClickedCheckCycle()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);

	pFatherDlg->m_bCycle = (pFatherDlg->m_bCycle == 0) ? 1 : 0;
	pBtn->SetCheck(pFatherDlg->m_bCycle);

	UpdateData(FALSE);
}


int CDlg_VideoPanelPlayController::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//m_sldFile.SetRange(0, 100);
	//m_sldFile.SetPos(0);
	//m_sldFile.SetPageSize(10);

	return 0;
}


void CDlg_VideoPanelPlayController::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

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

		if (pFatherDlg->m_bIsAudio)
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
			if (pFatherDlg->m_pVidDecoder != NULL)
			{
				nPercent = pFatherDlg->m_pVidDecoder->Preview_SeekAtPercent(nPercent);
			}
		}

		m_sldFile.SetPos(nPercent);
		UpdateData(FALSE);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlg_VideoPanelPlayController::SetCtrlButtonStates(int offline, int playing, int audioonly)
{
	CWnd* pWnd;

	bool bActive = (offline == 1) && (playing == 0);

	pWnd = GetDlgItem(IDC_CHECK_CYCLE);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_SLIDER_FILE);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_FIRST_FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_PRE_5FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_PRE_FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_PLAY_OR_STOP);
	pWnd->EnableWindow((offline == 1));
	if (playing == 1)
	{
		pWnd->SetWindowText("停止");
	}
	else
	{
		pWnd->SetWindowText("播放");
	}

	pWnd = GetDlgItem(IDC_BTN_NEXT_FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_NEXT_5FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_LAST_FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_BTN_INFO);
	pWnd->EnableWindow(offline & (!audioonly));

	pWnd = GetDlgItem(IDC_BTN_GRID);
	pWnd->EnableWindow(offline & (!audioonly));

	pWnd = GetDlgItem(IDC_BTN_SAVE_BMP);
	pWnd->EnableWindow(offline & (!audioonly));

	//	pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	//	pWnd->EnableWindow(!audioonly);
}

void CDlg_VideoPanelPlayController::ReportFPS(CString strFPS)
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	pWnd->SetWindowText(strFPS);
}



void CDlg_VideoPanelPlayController::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);
	pBtn->SetCheck(pFatherDlg->m_bCycle);

	UpdateData(FALSE);
}


void CDlg_VideoPanelPlayController::OnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->m_bPlaying = 0;
	//CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY_OR_STOP);
	//pWnd->SetWindowText("播放");

	SetCtrlButtonStates(1, pFatherDlg->m_bPlaying, 0);

	pFatherDlg->SendMessage(WM_PLAY_THREAD_EXIT);
}


void CDlg_VideoPanelPlayController::OnClickedBtnEnlargeScreen()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->EnlargeVideo();
}

void CDlg_VideoPanelPlayController::InformStopped(void)
{
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY_OR_STOP);
	pWnd->SetWindowText("播放");

	SetCtrlButtonStates(1, pFatherDlg->m_bPlaying, 0);
}
