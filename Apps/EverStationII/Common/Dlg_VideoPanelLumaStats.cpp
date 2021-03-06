// Dlg_ShowLuminaceStats.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "Dlg_VideoPanelLumaStats.h"
#include "afxdialogex.h"


// CDlg_ShowLuminaceStats 对话框

IMPLEMENT_DYNAMIC(CDlg_VideoPanelLumaStats, CDialogEx)

CDlg_VideoPanelLumaStats::CDlg_VideoPanelLumaStats(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOW_VIDEO_LUMINANCE, pParent)
{

}

CDlg_VideoPanelLumaStats::~CDlg_VideoPanelLumaStats()
{
}

void CDlg_VideoPanelLumaStats::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_VideoPanelLumaStats, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlg_ShowLuminaceStats 消息处理程序


BOOL CDlg_VideoPanelLumaStats::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectHistDiagram;
	GetClientRect(&rectHistDiagram);

	if (!m_LumaDiagramGraph.Create(NULL, "亮度测试图", WS_CHILD | WS_VISIBLE, rectHistDiagram, this, 0L))
	{
		TRACE0("未能创建亮度直方图监控窗口\n");
		return FALSE; // 未能创建
	}
	m_LumaDiagramGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_KEYPOINT, 0, 1024, "", 0, 1024, 128);			//
	m_LumaDiagramGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 256, "", 0, 256, 16);						//

	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlg_VideoPanelLumaStats::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_LumaDiagramGraph.DestroyWindow();
}

void CDlg_VideoPanelLumaStats::Reset(void)
{
	m_LumaDiagramGraph.Reset();
	m_LumaDiagramGraph.Clear();
}

void CDlg_VideoPanelLumaStats::ReportStats(uint8_t* pucFrameBuf, INPUT_YUV_SEQUENCE_PARAM_t* pstFrameParam)
{
	if ((pucFrameBuf != NULL) && (pstFrameParam != NULL))
	{
		m_LumaDiagramGraph.Clear();
		m_LumaDiagramGraph.AppendOneFrame(pucFrameBuf, pstFrameParam->luma_width, pstFrameParam->luma_height);

		Invalidate(FALSE);
	}
}
