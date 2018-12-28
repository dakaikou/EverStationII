#if !defined(AFX_DLG_OVERVIEW_H__2E418F95_7BAC_42B0_A8F7_CF2975FB7E83__INCLUDED_)
#define AFX_DLG_OVERVIEW_H__2E418F95_7BAC_42B0_A8F7_CF2975FB7E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Overview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Overview dialog
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"

#include "..\Magic_TS\TSMagic_GuiApi.h"

#include "..\Common\InstrumentPanel_Histogram.h"
#include "..\Common\InstrumentPanel_Waveform.h"

#include "..\resource.h"
#include "afxcmn.h"

class CDlg_TSAnalyzer_Overview : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Overview(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Overview)
	enum { IDD = IDD_TS_ANALYZER_OVERVIEW };
	CInstrumentPanel_Histogram	m_TsRateJitterGraphHistogram;
//	CGraph_Oscilloscope	m_TsRateJitterGraphOscilloscope;
	CListCtrl			m_listMonitor;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Overview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
//	int	 m_nDeviceIndex;
	int  m_nRunning;

	void Set(int offline);
	void Reset(void);

	void UpdateNIT(CNIT* pNIT);
	void UpdatePAT(CPAT* pPAT);
	void UpdateSDT(CSDT* pSDT);
	void UpdateBitrateSample(int bitrate);
	void UpdatePacketLength(int pklength);
	void UpdatePacketInfo(void);
	void UpdatePMT(CPMT* pPMT);

//	void AppendLogInfo(int type, const char* pszItem);
	void AppendEtr290Log(int type, const char* pszItem);

protected:

	unsigned short		m_usActualTSID;

	CImageList			m_imageDebug;
	int					m_nEtr290Count;

	int					m_nServiceCount;

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_Overview)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listBandwidth;
	CListCtrl m_listTSOverview;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_OVERVIEW_H__2E418F95_7BAC_42B0_A8F7_CF2975FB7E83__INCLUDED_)
