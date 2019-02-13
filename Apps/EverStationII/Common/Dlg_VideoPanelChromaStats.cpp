// Dlg_ShowChromaStats.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "Dlg_VideoPanelChromaStats.h"
#include "afxdialogex.h"


// Dlg_ShowChromaStats 对话框

IMPLEMENT_DYNAMIC(CDlg_VideoPanelChromaStats, CDialogEx)

CDlg_VideoPanelChromaStats::CDlg_VideoPanelChromaStats(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOW_VIDEO_CHROMA, pParent)
{

}

CDlg_VideoPanelChromaStats::~CDlg_VideoPanelChromaStats()
{
}

void CDlg_VideoPanelChromaStats::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_VideoPanelChromaStats, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDlg_VideoPanelChromaStats::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectChromaDiagram;
	GetClientRect(&rectChromaDiagram);

	if (!m_ChromaDiagramGraph.Create("色度散点图", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, rectChromaDiagram, this))
	{
		TRACE0("未能创建色度散点图窗口\n");
		return FALSE; // 未能创建
	}
	m_ChromaDiagramGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 1000, "", 0, 1000);				//直角坐标，单向从小到大型
	m_ChromaDiagramGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 1000, "", 0, 1000);				//直角坐标，单向从小到大型

	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlg_VideoPanelChromaStats::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_ChromaDiagramGraph.DestroyWindow();
}

void CDlg_VideoPanelChromaStats::Reset(void)
{
	m_ChromaDiagramGraph.Reset();
}
