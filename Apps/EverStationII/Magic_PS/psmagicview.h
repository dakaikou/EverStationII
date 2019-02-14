#if !defined(AFX_PSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
#define AFX_PSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSMagicView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPSMagicView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "..\resource.h"

#include "..\Common\Dlg_Progress.h"
#include "..\Common\Dlg_VideoShowScreen.h"
#include "..\Common\Dlg_Decimate.h"
//#include "..\Common\GuiCommon_TreeFunction.h"

class CPSMagicView : public CFormView
{
public:
	CPSMagicView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPSMagicView)

// Form Data
public:
	//{{AFX_DATA(CPSMagicView)
	enum { IDD = IDD_MAGIC_PS};
	//}}AFX_DATA

// Attributes
public:
	//TreeCtrl	m_treePSSyntax;

	//TREEITEM	m_hRootItem;
	//TREEITEM	m_hVidItem;
	//TREEITEM	m_hAudItem;

// Operations
public:

	CDlg_Progress	m_dlgProgress;

	//CMPEG_PS_Decoder		m_MPGPSDecoder;

	static CPSMagicView* GetView(void);

protected:
	int					m_bInitDone;
	CDlg_VideoShowScreen		m_dlgVideo;

	PS_PACKINFO_t		m_packinfo;
	PS_VIDEOINFO_t		m_videoinfo;
	PS_AUDIOINFO_t		m_audioinfo;

	void ReLayoutWindow(int cx, int cy);
	void Reset(void);
	void TermAnalyseThread(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSMagicView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPSMagicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPSMagicView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtnOpenOrClose();
	afx_msg void OnBtnEsDispatch();
	afx_msg void OnBtnPsPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	afx_msg LRESULT OnTSMagicOfflineThreadMsg(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnTSMagicRealtimeThreadMsg(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateMPEGPackHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateMPEGSystemHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSequenceHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateAC3EsSyncInfo(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
