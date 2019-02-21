#pragma once


// Dlg_ShowChromaStats 对话框
#include "InstrumentPanel_ColorSpaceDiagram.h"

class CDlg_VideoPanelChromaStats : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg_VideoPanelChromaStats)

public:
	CDlg_VideoPanelChromaStats(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg_VideoPanelChromaStats();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOW_VIDEO_CHROMA };
#endif

	CInstrumentPanel_ColorSpaceDiagram	m_ChromaDiagramGraph;

	void Reset(void);
	void ReportStats(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
