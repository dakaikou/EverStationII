#pragma once

#include "..\resource.h"
#include "BitmapButton_HoverEffect.h"
// CDlg_PlayerController 对话框

#define PLAYCTRL_BKBMP_WIDTH			305
#define PLAYCTRL_BKBMP_HEIGHT			310


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
	CSliderCtrl		m_sldFile;

	CBitmapButton_HoverEffect	m_btnPlayOrStop;
	CBitmapButton_HoverEffect	m_btnPowerOff;
	CBitmapButton_HoverEffect	m_btnShowGrid;

	CBitmapButton_HoverEffect	m_btnNext1Frame;
	CBitmapButton_HoverEffect	m_btnNextNFrame;
	CBitmapButton_HoverEffect	m_btnPre1Frame;
	CBitmapButton_HoverEffect	m_btnPreNFrame;
	CBitmapButton_HoverEffect	m_btnTailFrame;
	CBitmapButton_HoverEffect	m_btnHeadFrame;

	CBitmapButton_HoverEffect	m_btnSnapshot;
	CBitmapButton_HoverEffect	m_btnInfoCtrl;
	CBitmapButton_HoverEffect	m_btnScreenEnlarge;
	CBitmapButton_HoverEffect	m_btnScreenReduce;

	CBitmapButton_HoverEffect	m_btnCycleCtrl;
	CBitmapButton_HoverEffect	m_btnFrameRateCtrl;

	CBrush					m_bkBrush;

	//CString					m_strMeasuredDuration;

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
	afx_msg void OnClickedBtnShowGrid();
	afx_msg void OnClickedBtnInfoCtrl();
	afx_msg void OnClickedBtnTailFrame();
	afx_msg void OnClickedBtnHeadFrame();
	afx_msg void OnClickedBtnCycleCtrl();
	afx_msg void OnClickedBtnFrameRateCtrl();
	afx_msg void OnClickedBtnSnapshot();
	afx_msg void OnClickedBtnPowerOff();
	afx_msg void OnClickedBtnScreenEnlarge();
	afx_msg void OnClickedBtnScreenReduce();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
