// Dlg_PlayerController.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "GuiApi_MSG.h"
#include "Dlg_VideoPanelPlayController.h"
#include "Dlg_VideoShowScreen.h"
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
	DDX_Control(pDX, IDC_PLAYCTRL_PLAY_OR_STOP, m_btnPlayOrStop);
	DDX_Control(pDX, IDC_PLAYCTRL_POWEROFF, m_btnPowerOff);
	DDX_Control(pDX, IDC_PLAYCTRL_SHOWGRID, m_btnShowGrid);
	DDX_Control(pDX, IDC_PLAYCTRL_NEXT1FRAME, m_btnNext1Frame);
	DDX_Control(pDX, IDC_PLAYCTRL_NEXTNFRAME, m_btnNextNFrame);
	DDX_Control(pDX, IDC_PLAYCTRL_PRE1FRAME, m_btnPre1Frame);
	DDX_Control(pDX, IDC_PLAYCTRL_PRENFRAME, m_btnPreNFrame);
	DDX_Control(pDX, IDC_PLAYCTRL_TAILFRAME, m_btnTailFrame);
	DDX_Control(pDX, IDC_PLAYCTRL_HEADFRAME, m_btnHeadFrame);
	DDX_Control(pDX, IDC_PLAYCTRL_SNAPSHOT, m_btnSnapshot);
	DDX_Control(pDX, IDC_PLAYCTRL_INFOCTRL, m_btnInfoCtrl);
	DDX_Control(pDX, IDC_PLAYCTRL_SCREEN_ENLARGE, m_btnScreenEnlarge);
	DDX_Control(pDX, IDC_PLAYCTRL_SCREEN_REDUCE, m_btnScreenReduce);
	DDX_Control(pDX, IDC_PLAYCTRL_CYCLECTRL, m_btnCycleCtrl);
	DDX_Control(pDX, IDC_PLAYCTRL_FRAMECTRL, m_btnFrameRateCtrl);
}


BEGIN_MESSAGE_MAP(CDlg_VideoPanelPlayController, CDialogEx)
	ON_BN_CLICKED(IDC_PLAYCTRL_PLAY_OR_STOP, &CDlg_VideoPanelPlayController::OnClickedBtnPlayOrStop)
	ON_BN_CLICKED(IDC_PLAYCTRL_POWEROFF, &CDlg_VideoPanelPlayController::OnClickedBtnPowerOff)
	ON_BN_CLICKED(IDC_PLAYCTRL_SHOWGRID, &CDlg_VideoPanelPlayController::OnClickedBtnShowGrid)
	ON_BN_CLICKED(IDC_PLAYCTRL_PRENFRAME, &CDlg_VideoPanelPlayController::OnClickedBtnPre5frame)
	ON_BN_CLICKED(IDC_PLAYCTRL_PRE1FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnPreFrame)
	ON_BN_CLICKED(IDC_PLAYCTRL_NEXT1FRAME, &CDlg_VideoPanelPlayController::OnClickedBtnNextFrame)
	ON_BN_CLICKED(IDC_PLAYCTRL_NEXTNFRAME, &CDlg_VideoPanelPlayController::OnClickedBtnNext5frame)
	ON_BN_CLICKED(IDC_PLAYCTRL_TAILFRAME, &CDlg_VideoPanelPlayController::OnClickedBtnTailFrame)
	ON_BN_CLICKED(IDC_PLAYCTRL_HEADFRAME, &CDlg_VideoPanelPlayController::OnClickedBtnHeadFrame)
	ON_BN_CLICKED(IDC_PLAYCTRL_INFOCTRL, &CDlg_VideoPanelPlayController::OnClickedBtnInfoCtrl)
	ON_BN_CLICKED(IDC_PLAYCTRL_SNAPSHOT, &CDlg_VideoPanelPlayController::OnClickedBtnSnapshot)
	ON_BN_CLICKED(IDC_PLAYCTRL_SCREEN_ENLARGE, &CDlg_VideoPanelPlayController::OnClickedBtnScreenEnlarge)
	ON_BN_CLICKED(IDC_PLAYCTRL_SCREEN_REDUCE, &CDlg_VideoPanelPlayController::OnClickedBtnScreenReduce)
	ON_BN_CLICKED(IDC_PLAYCTRL_CYCLECTRL, &CDlg_VideoPanelPlayController::OnClickedBtnCycleCtrl)
	ON_BN_CLICKED(IDC_PLAYCTRL_FRAMECTRL, &CDlg_VideoPanelPlayController::OnClickedBtnFrameRateCtrl)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlg_PlayerController 消息处理程序


void CDlg_VideoPanelPlayController::OnClickedBtnPlayOrStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	if (pFatherDlg->m_bPlayControllStatus == 0)
	{
		//pWnd->SetWindowText("停止");
		//m_sldFile.EnableWindow(FALSE);

		pFatherDlg->StartVideoPlayThread();

		m_btnPlayOrStop.LoadBitmaps(IDB_PLAYCTRL_STOP_NORMAL, IDB_PLAYCTRL_STOP_DOWN, IDB_PLAYCTRL_STOP_FOCUS, IDB_PLAYCTRL_STOP_DISABLED);   //载入  
	}
	else if (pFatherDlg->m_bPlayControllStatus == 1)
	{
		pFatherDlg->StopVideoPlayThread();

		//pWnd->SetWindowText("播放");
		//m_sldFile.EnableWindow(TRUE);

		CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
		pWnd->SetWindowText("");

		m_btnPlayOrStop.LoadBitmaps(IDB_PLAYCTRL_PLAY_NORMAL, IDB_PLAYCTRL_PLAY_DOWN, IDB_PLAYCTRL_PLAY_FOCUS, IDB_PLAYCTRL_PLAY_DISABLED);   //载入  
	}

	SetCtrlButtonStates(1, pFatherDlg->m_bPlayControllStatus, 0);
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


void CDlg_VideoPanelPlayController::OnClickedBtnShowGrid()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->ToggleGrid();
}


void CDlg_VideoPanelPlayController::OnClickedBtnInfoCtrl()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->ShowStats();
}


void CDlg_VideoPanelPlayController::OnClickedBtnTailFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewLastFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnHeadFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	int nPos = pFatherDlg->PreviewFirstFrame();

	m_sldFile.SetPos(nPos);
}


void CDlg_VideoPanelPlayController::OnClickedBtnSnapshot()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	//pFatherDlg->SaveSnapshot();

	if (m_btnPlayOrStop.IsWindowEnabled())
	{
		m_btnPlayOrStop.EnableWindow(false);
	}
	else
	{
		m_btnPlayOrStop.EnableWindow(true);
	}
}


void CDlg_VideoPanelPlayController::OnClickedBtnCycleCtrl()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	//CButton* pBtn = (CButton*)GetDlgItem(IDC_PLAYCTRL_CYCLECTRL);

	pFatherDlg->m_bCycle = (pFatherDlg->m_bCycle == 0) ? 1 : 0;
	//pBtn->SetCheck(pFatherDlg->m_bCycle);
	if (pFatherDlg->m_bCycle)
	{
		m_btnCycleCtrl.LoadBitmaps(IDB_PLAYCTRL_CYCLECTRL_CHECKED_NORMAL, IDB_PLAYCTRL_CYCLECTRL_CHECKED_DOWN, IDB_PLAYCTRL_CYCLECTRL_CHECKED_FOCUS, IDB_PLAYCTRL_CYCLECTRL_CHECKED_DISABLED);   //载入  
	}
	else
	{
		m_btnCycleCtrl.LoadBitmaps(IDB_PLAYCTRL_CYCLECTRL_UNCHECKED_NORMAL, IDB_PLAYCTRL_CYCLECTRL_UNCHECKED_DOWN, IDB_PLAYCTRL_CYCLECTRL_UNCHECKED_FOCUS, IDB_PLAYCTRL_CYCLECTRL_UNCHECKED_DISABLED);   //载入  
	}

	UpdateData(FALSE);
}

void CDlg_VideoPanelPlayController::OnClickedBtnFrameRateCtrl()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();
	//CButton* pBtn = (CButton*)GetDlgItem(IDC_PLAYCTRL_FRAMECTRL);

	pFatherDlg->m_bFrameRateCtrl = (pFatherDlg->m_bFrameRateCtrl == 0) ? 1 : 0;
	//pBtn->SetCheck(pFatherDlg->m_bFrameRateCtrl);
	if (pFatherDlg->m_bFrameRateCtrl)
	{
		m_btnFrameRateCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_CHECKED_NORMAL, IDB_PLAYCTRL_FRAMECTRL_CHECKED_DOWN, IDB_PLAYCTRL_FRAMECTRL_CHECKED_FOCUS, IDB_PLAYCTRL_FRAMECTRL_CHECKED_DISABLED);   //载入  
	}
	else
	{
		m_btnFrameRateCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_NORMAL, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_DOWN, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_FOCUS, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_DISABLED);   //载入  
	}

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

	m_btnPlayOrStop.LoadBitmaps(IDB_PLAYCTRL_PLAY_NORMAL, IDB_PLAYCTRL_PLAY_DOWN, IDB_PLAYCTRL_PLAY_FOCUS, IDB_PLAYCTRL_PLAY_DISABLED);   //载入  

	m_btnPowerOff.LoadBitmaps(IDB_PLAYCTRL_POWEROFF_NORMAL, IDB_PLAYCTRL_POWEROFF_DOWN, IDB_PLAYCTRL_POWEROFF_FOCUS, IDB_PLAYCTRL_POWEROFF_DISABLED);   //载入  
	m_btnShowGrid.LoadBitmaps(IDB_PLAYCTRL_SHOWGRID_NORMAL, IDB_PLAYCTRL_SHOWGRID_DOWN, IDB_PLAYCTRL_SHOWGRID_FOCUS, IDB_PLAYCTRL_SHOWGRID_DISABLED);   //载入  

	m_btnNext1Frame.LoadBitmaps(IDB_PLAYCTRL_NEXT1FRAME_NORMAL, IDB_PLAYCTRL_NEXT1FRAME_DOWN, IDB_PLAYCTRL_NEXT1FRAME_FOCUS, IDB_PLAYCTRL_NEXT1FRAME_DISABLED);   //载入  
	m_btnNextNFrame.LoadBitmaps(IDB_PLAYCTRL_NEXTNFRAME_NORMAL, IDB_PLAYCTRL_NEXTNFRAME_DOWN, IDB_PLAYCTRL_NEXTNFRAME_FOCUS, IDB_PLAYCTRL_NEXTNFRAME_DISABLED);   //载入  
	m_btnPre1Frame.LoadBitmaps(IDB_PLAYCTRL_PRE1FRAME_NORMAL, IDB_PLAYCTRL_PRE1FRAME_DOWN, IDB_PLAYCTRL_PRE1FRAME_FOCUS, IDB_PLAYCTRL_PRE1FRAME_DISABLED);   //载入  
	m_btnPreNFrame.LoadBitmaps(IDB_PLAYCTRL_PRENFRAME_NORMAL, IDB_PLAYCTRL_PRENFRAME_DOWN, IDB_PLAYCTRL_PRENFRAME_FOCUS, IDB_PLAYCTRL_PRENFRAME_DISABLED);   //载入  
	m_btnTailFrame.LoadBitmaps(IDB_PLAYCTRL_TAILFRAME_NORMAL, IDB_PLAYCTRL_TAILFRAME_DOWN, IDB_PLAYCTRL_TAILFRAME_FOCUS, IDB_PLAYCTRL_TAILFRAME_DISABLED);   //载入  
	m_btnHeadFrame.LoadBitmaps(IDB_PLAYCTRL_HEADFRAME_NORMAL, IDB_PLAYCTRL_HEADFRAME_DOWN, IDB_PLAYCTRL_HEADFRAME_FOCUS, IDB_PLAYCTRL_HEADFRAME_DISABLED);   //载入  
	//m_btnCycleCtrl.LoadBitmaps(IDB_PLAYCTRL_CYCLECTRL_NORMAL, IDB_PLAYCTRL_CYCLECTRL_DOWN, IDB_PLAYCTRL_CYCLECTRL_FOCUS, IDB_PLAYCTRL_CYCLECTRL_DISABLED);   //载入  
	//m_btnFrameRateCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_NORMAL, IDB_PLAYCTRL_FRAMECTRL_DOWN, IDB_PLAYCTRL_FRAMECTRL_FOCUS, IDB_PLAYCTRL_FRAMECTRL_DISABLED);   //载入  
	m_btnSnapshot.LoadBitmaps(IDB_PLAYCTRL_SNAPSHOT_NORMAL, IDB_PLAYCTRL_SNAPSHOT_DOWN, IDB_PLAYCTRL_SNAPSHOT_FOCUS, IDB_PLAYCTRL_SNAPSHOT_DISABLED);   //载入  
	m_btnInfoCtrl.LoadBitmaps(IDB_PLAYCTRL_INFOCTRL_NORMAL, IDB_PLAYCTRL_INFOCTRL_DOWN, IDB_PLAYCTRL_INFOCTRL_FOCUS, IDB_PLAYCTRL_INFOCTRL_DISABLED);   //载入  
	m_btnScreenEnlarge.LoadBitmaps(IDB_PLAYCTRL_SCREEN_ENLARGE_NORMAL, IDB_PLAYCTRL_SCREEN_ENLARGE_DOWN, IDB_PLAYCTRL_SCREEN_ENLARGE_FOCUS, IDB_PLAYCTRL_SCREEN_ENLARGE_DISABLED);   //载入  
	m_btnScreenReduce.LoadBitmaps(IDB_PLAYCTRL_SCREEN_REDUCE_NORMAL, IDB_PLAYCTRL_SCREEN_REDUCE_DOWN, IDB_PLAYCTRL_SCREEN_REDUCE_FOCUS, IDB_PLAYCTRL_SCREEN_REDUCE_DISABLED);   //载入  

	//m_bkBrush.CreateSolidBrush(RGB(255, 180, 100));		//prepare for background

	//::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//::SetLayeredWindowAttributes(m_hWnd, 0, 200, LWA_ALPHA); // 120是透明度，范围是0～255  

	//m_strMeasuredDuration = "Unknown";

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
			for (int i = 0; i < MAX_ATTACHED_VIDEO_DECODERS; i++)
			{
				if (pFatherDlg->m_pVidDecoder[i] != NULL)
				{
					nPercent = pFatherDlg->m_pVidDecoder[i]->Preview_SeekAtPercent(nPercent);
					pFatherDlg->m_nPlayProgressPercent = nPercent;
				}
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

	//pWnd = GetDlgItem(IDC_PLAYCTRL_CYCLECTRL);
	//pWnd->EnableWindow(bActive);

	//pWnd = GetDlgItem(IDC_CHECK_FRAMERATE_CTRL);
	//pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_SLIDER_FILE);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_HEADFRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_PRE1FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_PRENFRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_PLAY_OR_STOP);
	pWnd->EnableWindow((offline == 1));
	if (playing == 1)
	{
		pWnd->SetWindowText("停止");
	}
	else
	{
		pWnd->SetWindowText("播放");
	}

	pWnd = GetDlgItem(IDC_PLAYCTRL_NEXT1FRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_NEXTNFRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_TAILFRAME);
	pWnd->EnableWindow(bActive);

	pWnd = GetDlgItem(IDC_PLAYCTRL_INFOCTRL);
	pWnd->EnableWindow(offline & (!audioonly));

	pWnd = GetDlgItem(IDC_PLAYCTRL_SHOWGRID);
	pWnd->EnableWindow(offline & (!audioonly));

	pWnd = GetDlgItem(IDC_PLAYCTRL_SNAPSHOT);
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

	//CButton* pBtn = (CButton*)GetDlgItem(IDC_PLAYCTRL_CYCLECTRL);
	//pBtn->SetCheck(pFatherDlg->m_bCycle);

	//pBtn = (CButton*)GetDlgItem(IDC_PLAYCTRL_FRAMECTRL);
	//pBtn->SetCheck(pFatherDlg->m_bFrameRateCtrl);

	m_btnPlayOrStop.SizeToContent();  //使按钮适应图片大小  
	m_btnPlayOrStop.MoveWindow(130, 135, 40, 30);
	//m_btnPlayOrStop.SetButtonStyle(BS_3STATE);

	m_btnPowerOff.SizeToContent();  //使按钮适应图片大小  
	m_btnPowerOff.MoveWindow(120, 230, 60, 60);

	m_btnShowGrid.SizeToContent();  //使按钮适应图片大小  
	m_btnShowGrid.MoveWindow(138, 193, 25, 20);

	m_btnNext1Frame.SizeToContent();  //使按钮适应图片大小  
	m_btnNext1Frame.MoveWindow(189, 116, 18, 18);

	m_btnNextNFrame.SizeToContent();  //使按钮适应图片大小  
	m_btnNextNFrame.MoveWindow(194, 138, 20, 20);
	
	m_btnTailFrame.SizeToContent();  //使按钮适应图片大小  
	m_btnTailFrame.MoveWindow(188, 160, 24, 24);

	m_btnPre1Frame.SizeToContent();  //使按钮适应图片大小  
	m_btnPre1Frame.MoveWindow(90, 118, 20, 20);

	m_btnPreNFrame.SizeToContent();  //使按钮适应图片大小  
	m_btnPreNFrame.MoveWindow(86, 141, 20, 20);
	
	m_btnHeadFrame.SizeToContent();  //使按钮适应图片大小  
	m_btnHeadFrame.MoveWindow(91, 161, 24, 24);

	if (pFatherDlg->m_bFrameRateCtrl)
	{
		m_btnFrameRateCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_CHECKED_NORMAL, IDB_PLAYCTRL_FRAMECTRL_CHECKED_DOWN, IDB_PLAYCTRL_FRAMECTRL_CHECKED_FOCUS, IDB_PLAYCTRL_FRAMECTRL_CHECKED_DISABLED);   //载入  
	}
	else
	{
		m_btnFrameRateCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_NORMAL, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_DOWN, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_FOCUS, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_DISABLED);   //载入  
	}
	m_btnFrameRateCtrl.SizeToContent();  //使按钮适应图片大小  
	m_btnFrameRateCtrl.MoveWindow(205, 207, 20, 20);

	if (pFatherDlg->m_bCycle)
	{
		m_btnCycleCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_CHECKED_NORMAL, IDB_PLAYCTRL_FRAMECTRL_CHECKED_DOWN, IDB_PLAYCTRL_FRAMECTRL_CHECKED_FOCUS, IDB_PLAYCTRL_FRAMECTRL_CHECKED_DISABLED);   //载入  
	}
	else
	{
		m_btnCycleCtrl.LoadBitmaps(IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_NORMAL, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_DOWN, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_FOCUS, IDB_PLAYCTRL_FRAMECTRL_UNCHECKED_DISABLED);   //载入  
	}
	m_btnCycleCtrl.SizeToContent();  //使按钮适应图片大小  
	m_btnCycleCtrl.MoveWindow(180, 223, 20, 20);

	m_btnSnapshot.SizeToContent();  //使按钮适应图片大小  
	m_btnSnapshot.MoveWindow(169, 188, 18, 18);

	m_btnInfoCtrl.SizeToContent();  //使按钮适应图片大小  
	m_btnInfoCtrl.MoveWindow(114, 188, 20, 20);

	m_btnScreenEnlarge.SizeToContent();  //使按钮适应图片大小  
	m_btnScreenEnlarge.MoveWindow(174, 97, 14, 14);

	m_btnScreenReduce.SizeToContent();  //使按钮适应图片大小  
	m_btnScreenReduce.MoveWindow(119, 95, 14, 14);

	m_sldFile.MoveWindow(5, PLAYCTRL_BKBMP_HEIGHT + 10, PLAYCTRL_BKBMP_WIDTH - 10, 20);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	pWnd->MoveWindow(5, PLAYCTRL_BKBMP_HEIGHT + 40, PLAYCTRL_BKBMP_WIDTH - 10, 20);

	UpdateData(FALSE);
}


void CDlg_VideoPanelPlayController::OnClickedBtnPowerOff()
{
	// TODO: 在此添加控件通知处理程序代码
	//模拟一个“播放”按键的消息
	//PostMessage(WM_COMMAND, MAKEWPARAM(IDC_PLAYCTRL_PLAY_OR_STOP, BN_CLICKED), NULL);

	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	//pFatherDlg->m_bCommandPlay = 0;
	//CWnd* pWnd = GetDlgItem(IDC_BTN_PLAY_OR_STOP);
	//pWnd->SetWindowText("播放");
	m_btnPlayOrStop.LoadBitmaps(IDB_PLAYCTRL_PLAY_NORMAL, IDB_PLAYCTRL_PLAY_DOWN, IDB_PLAYCTRL_PLAY_FOCUS, IDB_PLAYCTRL_PLAY_DISABLED);   //载入  

	CWnd* pWnd = GetDlgItem(IDC_STATIC_INFO_FPS);
	pWnd->SetWindowText("");

	//SetCtrlButtonStates(1, 0, 0);

	//pFatherDlg->SendMessage(WM_PLAY_THREAD_EXIT);
	::PostMessage(pFatherDlg->GetSafeHwnd(), WM_PLAY_CONTROLLER_REPORT_EXIT, NULL, NULL);
}


void CDlg_VideoPanelPlayController::OnClickedBtnScreenEnlarge()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->CanvasEnlarge();
}

void CDlg_VideoPanelPlayController::OnClickedBtnScreenReduce()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	pFatherDlg->CanvasReduce();
}

void CDlg_VideoPanelPlayController::InformStopped(void)
{
	CDlg_VideoShowScreen* pFatherDlg = (CDlg_VideoShowScreen*)GetParent();

	CWnd* pWnd = GetDlgItem(IDC_PLAYCTRL_PLAY_OR_STOP);
	pWnd->SetWindowText("播放");

	m_btnPlayOrStop.LoadBitmaps(IDB_PLAYCTRL_PLAY_NORMAL, IDB_PLAYCTRL_PLAY_DOWN, IDB_PLAYCTRL_PLAY_FOCUS, IDB_PLAYCTRL_PLAY_DISABLED);   //载入  

	SetCtrlButtonStates(1, pFatherDlg->m_bPlayControllStatus, 0);
}




void CDlg_VideoPanelPlayController::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CDC* pDC = GetDC();
	//CString str;
	//str.Format("%d, %d", point.x, point.y);
	//pDC->TextOutA(0, 0, str);
	//ReleaseDC(pDC);

	//if (((point.x > rectBtnPlayOrStop.left) && (point.x < rectBtnPlayOrStop.right)) &&
	//	((point.y > rectBtnPlayOrStop.top) && (point.y < rectBtnPlayOrStop.bottom)))
	//{
	//	m_btnPlayOrStop.SetFocus();
	//}

	//if (rectBtnPlayOrStop.PtInRect(point))
	//{
	//	m_btnPlayOrStop.SetFocus();
	//}
	//else
	//{
	//	//m_btnPlayOrStop;
	//}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CDlg_VideoPanelPlayController::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}


//BOOL CDlg_VideoPanelPlayController::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	if (pMsg->message == WM_MOUSEMOVE)
//	{
//		POINT point;
//		GetCursorPos(&point);
//
//		if (((point.x > m_rectInScreenForBtnPlayOrStop.left) && (point.x < m_rectInScreenForBtnPlayOrStop.right)) &&
//			((point.y > m_rectInScreenForBtnPlayOrStop.top) && (point.y < m_rectInScreenForBtnPlayOrStop.bottom)))
//		{
//			m_btnPlayOrStop.SetFocus();
//		}
//		else
//		{
//			::SendMessage(m_btnPlayOrStop.GetSafeHwnd(), WM_KILLFOCUS, -1, 0);
//		}
//
//		//CDC* pDC = GetDC();
//		//CString str;
//		//str.Format("%d, %d", point.x, point.y);
//		//pDC->TextOutA(0, 30, str);
//		//ReleaseDC(pDC);
//
//		//pMsg->hwnd = WindowFromPoint(point)->GetSafeHwnd();
//	}
//
//	return CDialogEx::PreTranslateMessage(pMsg);
//}


void CDlg_VideoPanelPlayController::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	CRect	rect;
	GetClientRect(&rect);

	CBitmap bmpBackground;
	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, "..\\..\\res\\playctrl_background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpBackground.Attach(hBitmap);

	BITMAP bitmap;
	bmpBackground.GetObject(sizeof(bitmap), &bitmap);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	//CBitmap* pbmpOld = dcMem.SelectObject(&bmpBackground);
	CBitmap* pbmpOld = dcMem.SelectObject(&bmpBackground);

	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcMem, 0, 0, SRCCOPY);
	//dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);

	//dc.TextOutA(0, 0, m_strMeasuredDuration);

	dcMem.SelectObject(pbmpOld);
	dcMem.DeleteDC();
}


HBRUSH CDlg_VideoPanelPlayController::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	//if (nCtlColor == CTLCOLOR_DLG)   // 判断是否是对话框  
	//{
	//	return   m_bkBrush; // 返回刚才创建的背景刷子  
	//}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
