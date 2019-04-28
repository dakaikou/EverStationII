#if !defined(AFX_DLG_DSMCCC_H__AFE4B73A_DD27_433E_ADB3_EA40C475B80B__INCLUDED_)
#define AFX_DLG_DSMCCC_H__AFE4B73A_DD27_433E_ADB3_EA40C475B80B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Epg.h : header file
//
#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Epg dialog
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_DSMCC_Table.h"

//#include "..\MFCExt\SplitWnd\SplitWnd.h"
#include "NaviList_DSMCCs.h"
#include "../Common/TreeView_XMLBrowser.h"
#include "..\resource.h"

class CDlg_TSAnalyzer_Dsmcc : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Dsmcc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Dsmcc)
	enum { IDD = IDD_TS_ANALYZER_DSMCC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Dsmcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSplitterWnd				m_wndSplitter;

	CNaviList_DSMCCs*			m_pNaviPane;
	CTreeView_PacketSyntax*		m_pInfoTree;

public:
	void Reset(void);
//	void Set(int offline);
	void UpdatePMT(CPMT* pPMT);
	void UpdateDSMCC(CPVT* pPVT);
	void UpdateSDT(CSDT* pSDT);
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_Epg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_DSMCCC_H__AFE4B73A_DD27_433E_ADB3_EA40C475B80B__INCLUDED_)
