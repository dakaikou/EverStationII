#if !defined(AFX_DLG_WATCH_SUBTITLE_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
#define AFX_DLG_WATCH_SUBTITLE_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_WatchSubtitle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Subtitle dialog
#include "..\resource.h"
#include "..\Magic_TS\TSMagic_GuiApi.h"
//#include "..\Magic_TS\Dlg_DtsPts.h"
//#include "..\Common\Dlg_ShowVideo.h"
//#include "..\Common\GuiCommon_TreeFunction.h"

#if !MFC_CONTROL
//#include "..\MFCExt\TreeList\TreeListCtrl.h"
#endif

class CDlg_TSAnalyzer_Subtitle : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Subtitle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Subtitle)
	enum { IDD = IDD_TS_ANALYZER_SUBTITLE };
	CListCtrl		m_listSubtitlePID;
//	CComboBox		m_cmbServices;
	#if MFC_CONTROL
	CTreeCtrl		m_treeSubtitleSyntax;
	#else
	//CTreeListCtrl	m_treeSubtitleSyntax;
	#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Subtitle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
//	CDlg_TS_DtsPts		m_dlgDtsPts;
public:

	void Reset(void);
//	void Set(void);
	void InitTree(void);
	void InitList(void);

protected:
	void SetPIDInfo(CPMT* pPMT);
	void SetESInfo(void);

//	void UpdateDtsPts(PES_packet_t* pPES_packet, int bAud);

	CImageList	m_ESImageList;

//	CDlg_ShowVideo	m_dlgTV;

//	int				m_dwAudCode;
//	int				m_dwVidCode;
//	unsigned short	m_usPCRPID;

	//TREEITEM		m_hTeletextPesItem;
	//TREEITEM		m_hTeletextEsItem;
//	TREEITEM		m_hAudPesItem;
//	TREEITEM		m_hAudEsItem;

	FILE*			fp_debug;

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_Subtitle)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtnSubtitleTrigger();
//	afx_msg void OnSelchangeCmbServices();
	afx_msg void OnDblclkListSubtitlePid(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
//	afx_msg LRESULT OnUpdatePAT(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePMT(WPARAM wParam, LPARAM lParam);

public:
	afx_msg LRESULT OnUpdatePesHeader(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_WATCH_SUBTITLE_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
