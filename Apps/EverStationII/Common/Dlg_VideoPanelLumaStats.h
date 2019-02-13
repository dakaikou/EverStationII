#pragma once


// CDlg_ShowLuminaceStats 对话框
#include "InstrumentPanel_Histogram.h"

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

	CInstrumentPanel_Histogram	m_LumaHistgram;

	void Reset(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
