#if !defined(AFX_DLG_M2VDECODER_H__DE798885_0D4D_4888_803A_7DCD3B72F077__INCLUDED_)
#define AFX_DLG_M2VDECODER_H__DE798885_0D4D_4888_803A_7DCD3B72F077__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_M2VDecoder.h : header file
//
#include <stdint.h>
/////////////////////////////////////////////////////////////////////////////
// CDlg_ShowVideo dialog
#include "..\resource.h"

#include "Dlg_VideoPanelPlayController.h"
#include "Dlg_VideoPanelChromaStats.h"
#include "Dlg_VideoPanelLumaStats.h"

#include "MiddleWare/MiddleWare_ESDecoder/Include/VESDecoder.h"

#define USE_PROGRESS_DATA_ACCESS_MUTEX			1
#define FRAME_RATE_CONTROLL_IN_GUI_THREAD		0

#define WM_PLAY_CONTROLLER_REPORT_EXIT	WM_USER + 0x4203
#define WM_VIDEO_CONTAINER_REPORT_PLAY_EXIT	WM_USER + 0x81D8

#define MAX_ATTACHED_VIDEO_DECODERS				3

typedef enum
{
	SHOWOPTION_VIDEO_AUDIO		= 0,
	SHOWOPTION_VIDEO_ONLY,
	SHOWOPTION_AUDIO_ONLY
} SHOWOPTION_TYPE_e;

class CDlg_VideoShowScreen : public CDialog
{
// Construction
public:
	CDlg_VideoShowScreen(CWnd* pParent = NULL);   // standard constructor

public:
	int				m_nAudStreamType;

	CVESDecoder*	m_pVidDecoder[MAX_ATTACHED_VIDEO_DECODERS];
	int				m_nVidDecoderCount;
	PVOID			m_pAudDecoder;

public:

	int				m_bPlayControllStatus;		//播放控制命令

	int				m_nPlayProgressPercent;
	double			m_dCheckedFrameRate;

	int				m_bIsAudio;
	int				m_bCycle;
	int				m_bFrameRateCtrl;

	BOOL			m_bForcingShowStats;
	BOOL			m_bForcingShowController;

	CDlg_VideoPanelPlayController*	m_pPanelPlayController;
	CDlg_VideoPanelLumaStats*		m_pPanelLumaStats;
	CDlg_VideoPanelChromaStats*     m_pPanelChromaStats;

	void			AttachVideoDecoder(PVOID pDecoder);
	void			DetachVideoDecoder(PVOID pDecoder);

	void			StartVideoPlayThread(void);
	void			StopVideoPlayThread(void);

	// Dialog Data
	//{{AFX_DATA(CDlg_ShowVideo)
	enum { IDD = IDD_SHOW_VIDEO_SCREEN };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_ShowVideo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int						m_bPlayResponseStatus;		//播放状态反馈

	BOOL					m_bFullScreen;
	CRect					m_rectFullScreen;
	CRect					m_rectPlayControl;
	CRect					m_rectChromaStats;
	CRect					m_rectLumaStats;
	//CBrush					m_bkBrush;
	WINDOWPLACEMENT			m_stOldWndPlacement;		//结构中包含了有关窗口在屏幕上位置的信息

	CString					m_strFileName;

	HWND					m_hParentWnd;

	DWORD					m_dwTimerID;
	MMRESULT				m_mmTimerID;
	UINT				    m_wTimerRes;

#if USE_SURFACE_ACCESS_MUTEX
	HANDLE		m_hProgressDataAccess;
#endif

public:
	void EnlargeClientAreaToFullScreen(void);
	void RestoreClientAreaToInitial(void);
	void ToggleGrid(void);
	//void ToggleCanvas(void);
	void CanvasEnlarge(void);
	void CanvasReduce(void);
	//void ToggleView(void);
	void ShowStats(void);
	void SaveSnapshot(void);

	void SetupDisplayWnd(void);

	int PreviewNextFrame(void);
	int PreviewNext5Frame(void);
	int PreviewLastFrame(void);
	int PreviewPreFrame(void);
	int PreviewPre5Frame(void);
	int PreviewFirstFrame(void);

	// Generated message map functions
	//{{AFX_MSG(CDlg_ShowVideo)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	afx_msg LRESULT OnReportVideoDecodeFPS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReportPlayThreadWorkingProgress(WPARAM, LPARAM);
	afx_msg LRESULT OnReportPlayThreadExit(WPARAM, LPARAM);

	afx_msg LRESULT OnStatisticLuma(WPARAM, LPARAM);
	afx_msg LRESULT OnStatisticChroma(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//int CALLBACK_report_yuv_luma_stats(HWND hWnd, WPARAM wParam, LPARAM lParam);
//int CALLBACK_report_yuv_chroma_stats(HWND hWnd, uint8_t* pucFrameBuf, int width, int height, DWORD dwFourCC);

//uint32_t VideoPlay_Thread(PVOID pVoid);
//uint32_t AudioPlay_Thread(PVOID pVoid);
void CALLBACK VideoPlay_TimerHandler(UINT uTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_M2VDECODER_H__DE798885_0D4D_4888_803A_7DCD3B72F077__INCLUDED_)
