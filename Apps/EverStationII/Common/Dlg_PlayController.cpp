// Dlg_PlayerController.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "Dlg_PlayController.h"
#include "afxdialogex.h"


// CDlg_PlayerController 对话框

IMPLEMENT_DYNAMIC(CDlg_PlayController, CDialogEx)

CDlg_PlayController::CDlg_PlayController(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOW_VIDEO_CTRL, pParent)
{
}

CDlg_PlayController::~CDlg_PlayController()
{
}

void CDlg_PlayController::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_FILE, m_sldFile);
}


BEGIN_MESSAGE_MAP(CDlg_PlayController, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CDlg_PlayController::OnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_PRE_5FRAME, &CDlg_PlayController::OnClickedBtnPre5frame)
	ON_BN_CLICKED(IDC_BTN_PRE_FRAME, &CDlg_PlayController::OnClickedBtnPreFrame)
	ON_BN_CLICKED(IDC_BTN_NEXT_FRAME, &CDlg_PlayController::OnClickedBtnNextFrame)
	ON_BN_CLICKED(IDC_BTN_NEXT_5FRAME, &CDlg_PlayController::OnClickedBtnNext5frame)
	ON_BN_CLICKED(IDC_BTN_GRID, &CDlg_PlayController::OnClickedBtnGrid)
	ON_BN_CLICKED(IDC_BTN_INFO, &CDlg_PlayController::OnClickedBtnInfo)
	ON_BN_CLICKED(IDC_BTN_LAST_FRAME, &CDlg_PlayController::OnClickedBtnLastFrame)
	ON_BN_CLICKED(IDC_BTN_FIRST_FRAME, &CDlg_PlayController::OnClickedBtnFirstFrame)
	ON_BN_CLICKED(IDC_CHECK_CYCLE, &CDlg_PlayController::OnClickedCheckCycle)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_SAVE_BMP, &CDlg_PlayController::OnClickedBtnSaveBmp)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDlg_PlayController::OnClickedBtnExit)
END_MESSAGE_MAP()


// CDlg_PlayerController 消息处理程序


void CDlg_PlayController::OnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	//pFatherDlg->OnBtnPlay();

	HWND	hWnd = this->GetSafeHwnd();

	CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY);
	CString strText;

	pWnd->GetWindowText(strText);
	if (strText == "播放")
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

		pWnd->SetWindowText("停止");

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

		m_sldFile.EnableWindow(FALSE);
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

		pWnd->SetWindowText("播放");

		pFatherDlg->m_bPlaying = 0;

		m_sldFile.EnableWindow(TRUE);

		CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
		pWnd->SetWindowText("");
	}
}

void CDlg_PlayController::OnClickedBtnPre5frame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	int nPos = pFatherDlg->PreviewNextFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_PlayController::OnClickedBtnPreFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	int nPos = pFatherDlg->PreviewPreFrame();

	m_sldFile.SetPos(nPos);
}

void CDlg_PlayController::OnClickedBtnNextFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	int nPos = pFatherDlg->PreviewNextFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_PlayController::OnClickedBtnNext5frame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	int nPos = pFatherDlg->PreviewNext5Frame();

	m_sldFile.SetPos(nPos);
}


void CDlg_PlayController::OnClickedBtnGrid()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDlg_PlayController::OnClickedBtnInfo()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDlg_PlayController::OnClickedBtnLastFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	int nPos = pFatherDlg->PreviewLastFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_PlayController::OnClickedBtnFirstFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	int nPos = pFatherDlg->PreviewFirstFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_PlayController::OnClickedBtnSaveBmp()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();

	pFatherDlg->SaveSnapshot();
}


void CDlg_PlayController::OnClickedCheckCycle()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);

	pFatherDlg->m_bCycle = (pFatherDlg->m_bCycle == 0) ? 1 : 0;
	pBtn->SetCheck(pFatherDlg->m_bCycle);

	UpdateData(FALSE);
}


int CDlg_PlayController::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//m_sldFile.SetRange(0, 100);
	//m_sldFile.SetPos(0);
	//m_sldFile.SetPageSize(10);

	return 0;
}


void CDlg_PlayController::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();

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

void CDlg_PlayController::SetControls(int offline, int audioonly)
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

void CDlg_PlayController::ReportFPS(CString strFPS)
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	pWnd->SetWindowText(strFPS);
}



void CDlg_PlayController::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CYCLE);

	pBtn->SetCheck(pFatherDlg->m_bCycle);

	UpdateData(FALSE);
}


void CDlg_PlayController::OnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_ShowVideo* pFatherDlg = (CDlg_ShowVideo*)GetParent();

	pFatherDlg->m_bPlaying = 0;
	CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY);
	pWnd->SetWindowText("播放");

	pFatherDlg->SendMessage(WM_CLOSE);

	//ShowWindow(SW_HIDE);
}
