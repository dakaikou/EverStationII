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
#include "..\MFCExt\tooltip2\tooltip2.h"
#include "..\resource.h"

typedef enum
{
	SHOWOPTION_VIDEO_AUDIO		= 0,
	SHOWOPTION_VIDEO_ONLY,
	SHOWOPTION_AUDIO_ONLY
} SHOWOPTION_TYPE_e;

class CDlg_ShowVideo : public CDialog
{
// Construction
public:
	CDlg_ShowVideo(CWnd* pParent = NULL);   // standard constructor

public:

	int				m_nVidStreamType;	
	int				m_nAudStreamType;
	PVOID			m_pVidDecoder;
	PVOID			m_pAudDecoder;

	int				m_nGraphWidth;
	int				m_nGraphHeight;
	int				m_nMbWidth;
	int				m_nMbHeight;
	char			m_pszFourCC[5];

public:

	int				m_bIsAudio;
	int				m_bPlaying;
	int				m_bCycle;
	HANDLE			m_hThread;
	DWORD			m_dwID;
	float			m_fViewRatio;
	int				m_nX;
	int				m_nY;
	RECT			m_rectDst;
	RECT			m_rectSrc;

	void			RealTimeStream(int vid_stream_type, PVOID pVidDecoder, int aud_stream_type, PVOID pAudDecoder);
	void			OfflineStream(int stream_type, PVOID pDecoder, int bAudio);

	void			AdjustLayout(int audioonly);
	void			SetControls(int offline, int audioonly);
// Dialog Data
	//{{AFX_DATA(CDlg_ShowVideo)
	enum { IDD = IDD_SHOW_VIDEO };
	CSliderCtrl	m_sldFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_ShowVideo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString					m_strFileName;

	// Generated message map functions
	//{{AFX_MSG(CDlg_ShowVideo)
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnBtnNextFrame();
	afx_msg void OnBtnNext5frame();
	afx_msg void OnBtnSaveBmp();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBtnGrid();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBtnInfo();
	afx_msg void OnBtnFirstFrame();
	afx_msg void OnBtnLastFrame();
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnPre5frame();
	afx_msg void OnBtnPreFrame();
	afx_msg void OnCheckCycle();
	//}}AFX_MSG

	afx_msg LRESULT OnReportVideoDecodeFPS(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

uint32_t VideoPlay_Thread(PVOID pVoid);
uint32_t AudioPlay_Thread(PVOID pVoid);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_M2VDECODER_H__DE798885_0D4D_4888_803A_7DCD3B72F077__INCLUDED_)
