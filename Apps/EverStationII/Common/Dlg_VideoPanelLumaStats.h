#pragma once


// CDlg_VideoPanelLumaStats 对话框
#include "MiddleWare/MiddleWare_ESDecoder/Include/VESDecoder.h"
#include "InstrumentCtrl/InstrumentCtrl_LumaGamutDiagram.h"

class CDlg_VideoPanelLumaStats : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg_VideoPanelLumaStats)

public:
	CDlg_VideoPanelLumaStats(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg_VideoPanelLumaStats();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOW_VIDEO_LUMINANCE };
#endif

	CInstrumentPanel_LumaGamutDiagram	m_LumaDiagramGraph;

	void Reset(void);
	void ReportStats(uint8_t* pucFrameBuf, INPUT_YUV_SEQUENCE_PARAM_t* pstFrameParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
