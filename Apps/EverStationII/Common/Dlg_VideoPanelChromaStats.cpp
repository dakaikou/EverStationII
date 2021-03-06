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

	if (!m_ChromaDiagramGraph.Create(NULL, "色度测试图", WS_VISIBLE | WS_BORDER | WS_CHILD, rectChromaDiagram, this, 0L))
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
	m_ChromaDiagramGraph.Clear();
}

void CDlg_VideoPanelChromaStats::ReportStats(uint8_t* pucFrameBuf, INPUT_YUV_SEQUENCE_PARAM_t* pstFrameParam)
{
	uint8_t* pucY = NULL;
	uint8_t* pucCb = NULL;
	uint8_t* pucCr = NULL;

	if ((pucFrameBuf != NULL) && (pstFrameParam != NULL))
	{
		m_ChromaDiagramGraph.Clear();

		if ((pstFrameParam->dwFourCC == MAKEFOURCC('Y', '4', '4', 'B')) ||			//4:4:4
			(pstFrameParam->dwFourCC == MAKEFOURCC('4', '4', '4', 'P')))
		{
			pucY = pucFrameBuf;
			if (pstFrameParam->dwFourCC == MAKEFOURCC('Y', '4', '4', 'B'))	//YUV Planner
			{
				pucCb = pucY + pstFrameParam->luma_plane_size;
				pucCr = pucCb + pstFrameParam->chroma_plane_size;
			}
			else if (pstFrameParam->dwFourCC == MAKEFOURCC('4', '4', '4', 'P'))	//YVU Planner
			{
				pucCr = pucY + pstFrameParam->luma_plane_size;
				pucCb = pucCr + pstFrameParam->chroma_plane_size;
			}
		}
		else if ((pstFrameParam->dwFourCC == MAKEFOURCC('Y', '4', '2', 'B')) ||			//4:2:2
			(pstFrameParam->dwFourCC == MAKEFOURCC('4', '2', '2', 'P')))
		{
			pucY = pucFrameBuf;
			if (pstFrameParam->dwFourCC == MAKEFOURCC('Y', '4', '2', 'B'))	//YUV Planner
			{
				pucCb = pucY + pstFrameParam->luma_plane_size;
				pucCr = pucCb + pstFrameParam->chroma_plane_size;
			}
			else if (pstFrameParam->dwFourCC == MAKEFOURCC('4', '2', '2', 'P'))	//YVU Planner
			{
				pucCr = pucY + pstFrameParam->luma_plane_size;
				pucCb = pucCr + pstFrameParam->chroma_plane_size;
			}
		}
		else if (
			(pstFrameParam->dwFourCC == MAKEFOURCC('Y', 'V', '1', '2')) ||			//YUY2	4:2:0, Planar Mode, FourCC code = 0x32315659
			(pstFrameParam->dwFourCC == MAKEFOURCC('I', '4', '2', '0')) ||			//I420	4:2:0, Planar Mode, FourCC code = 0x30323449
			(pstFrameParam->dwFourCC == MAKEFOURCC('I', 'Y', 'U', 'V'))				//IYUV	4:2:0, Planar Mode, FourCC code = 0x56555949
			)
		{
			pucY = pucFrameBuf;
			if (pstFrameParam->dwFourCC == MAKEFOURCC('Y', 'V', '1', '2'))	//YVU Planner
			{
				pucCr = pucY + pstFrameParam->luma_plane_size;
				pucCb = pucCr + pstFrameParam->chroma_plane_size;
			}
			else
			{
				pucCb = pucY + pstFrameParam->luma_plane_size;
				pucCr = pucCb + pstFrameParam->chroma_plane_size;
			}
		}

		m_ChromaDiagramGraph.AppendOneFrame(pucY, pstFrameParam->luma_width, pstFrameParam->luma_height,
			pucCb, pucCr, pstFrameParam->chroma_width, pstFrameParam->chroma_height, pstFrameParam->nColorSpace);

		Invalidate(FALSE);
	}
}
