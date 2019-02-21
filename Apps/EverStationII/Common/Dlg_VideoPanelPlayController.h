#pragma once

#include "..\resource.h"

// CDlg_PlayerController 对话框

class CDlg_VideoPanelPlayController : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg_VideoPanelPlayController)

public:
	CDlg_VideoPanelPlayController(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg_VideoPanelPlayController();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOW_VIDEO_CTRL };
#endif
	CSliderCtrl	m_sldFile;

	void SetCtrlButtonStates(int offline, int playing, int audioonly);
	void ReportFPS(CString strFPS);
	void InformStopped(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedBtnPlayOrStop();
	afx_msg void OnClickedBtnPre5frame();
	afx_msg void OnClickedBtnPreFrame();
	afx_msg void OnClickedBtnNextFrame();
	afx_msg void OnClickedBtnNext5frame();
	afx_msg void OnClickedBtnGrid();
	afx_msg void OnClickedBtnInfo();
	afx_msg void OnClickedBtnLastFrame();
	afx_msg void OnClickedBtnFirstFrame();
	afx_msg void OnClickedCheckCycle();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClickedBtnSaveBmp();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickedBtnExit();
	afx_msg void OnClickedBtnToggleCanvas();
	afx_msg void OnClickedBtnToggleView();
};
