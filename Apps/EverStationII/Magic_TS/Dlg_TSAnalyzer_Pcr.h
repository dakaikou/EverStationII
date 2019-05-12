#if !defined(AFX_DLG_PCR_H__0C6947E2_7174_4FB5_B3F2_97DFC80C6917__INCLUDED_)
#define AFX_DLG_PCR_H__0C6947E2_7174_4FB5_B3F2_97DFC80C6917__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Pcr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Pcr dialog
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"

#include "..\Common\InstrumentView_ScatterDiagram.h"
#include "..\Common\InstrumentView_Histogram.h"

#include "..\resource.h"

#include "TSMagic_GuiApi.h"
#include "NaviList_PCRs.h"
#include "afxcmn.h"

//#define SHOW_PCR_JITTER_WAVEFORM			0
//#define SHOW_PCR_INTERVAL_WAVEFORM			0
#define SHOW_PCR_JITTER_HISTGRAM			1
#define SHOW_PCR_INTERVAL_HISTGRAM			1
#define SHOW_PCR_SCATTER_DIAGRAM			1

#define NAVI_PANE_WIDTH						400
#define JITTER_OR_INTERVAL_PANE_WIDTH		600

class CDlg_TSAnalyzer_Pcr : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Pcr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Pcr)
	enum { IDD = IDD_TS_ANALYZER_PCR };

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Pcr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSplitterWnd		m_wndSplitter;
	CSplitterWnd		m_wndRightSplitter;

	int					m_nLayoutColCount;

	CNaviList_PCRs*		m_pNaviPane;
#if SHOW_PCR_JITTER_HISTGRAM
	CInstrumentView_Histogram*	m_pPcrJitterHistgramGraph;
#endif
	
#if SHOW_PCR_INTERVAL_HISTGRAM
	CInstrumentView_Histogram*	m_pPcrIntervalHistgramGraph;
#endif
	
#if SHOW_PCR_SCATTER_DIAGRAM
	CInstrumentView_ScatterDiagram*	m_pPcrScatterDiagramGraph;
#endif
	
	//#if SHOW_PCR_JITTER_WAVEFORM
	//	CInstrumentPanel_Waveform		m_PcrJitterWaveformGraph;
	//#endif
	//
	//#if SHOW_PCR_INTERVAL_WAVEFORM
	//	CInstrumentPanel_Waveform		m_PcrIntervalWaveformGraph;
	//#endif

public:
	void UpdatePCRDiagnosis(RECORD_PCR_t* pCurPcrInfo);
	void UpdatePCRObservation(int ID, int curInterval, int curJitter);
	void Set(int offline = 1);
	void Reset(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_Pcr)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PCR_H__0C6947E2_7174_4FB5_B3F2_97DFC80C6917__INCLUDED_)
