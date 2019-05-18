#if !defined(AFX_DLG_EPG_H__AFE4B73A_DD27_433E_ADB3_EA40C475B80B__INCLUDED_)
#define AFX_DLG_EPG_H__AFE4B73A_DD27_433E_ADB3_EA40C475B80B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_TSAnalyzer_Epg.h : header file
//
#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Epg dialog
#include "GuiWnd_NaviTree_ServiceGuide.h"
#include "GuiWnd_ListView_ServiceDescription.h"
#include "GuiWnd_ListView_EpgSchedule.h"
#include "..\resource.h"

class CDlg_TSAnalyzer_Semantic_Epg : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Semantic_Epg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Semantic_Epg)
	enum { IDD = IDD_TS_ANALYZER_EPG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Semantic_Epg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSplitterWnd				m_wndSplitter;
	CSplitterWnd				m_wndRightSplitter;

	CNaviTree_ServiceGuide*		m_pServiceListPane;
	CListView_ServiceDescription*	m_pServiceDescription;
	CListView_EpgSchedule*	m_pEpgScheduleList;

public:
	void Reset(void);
//	void Set(int offline);
	void UpdateSDT(CSDT* pSDT);
	void UpdateNIT(CNIT* pNIT);
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_Epg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnReportServiceSelChange(WPARAM wParam, LPARAM lParam);
	//virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_EPG_H__AFE4B73A_DD27_433E_ADB3_EA40C475B80B__INCLUDED_)
