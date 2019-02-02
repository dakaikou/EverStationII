#pragma once

#include "..\resource.h"

// CDlg_PlayerController 对话框

class CDlg_PlayController : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg_PlayController)

public:
	CDlg_PlayController(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg_PlayController();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOW_VIDEO_CTRL };
#endif
	CSliderCtrl	m_sldFile;

	void SetControls(int offline, int audioonly);
	void ReportFPS(CString strFPS);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedBtnPlay();
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
};
