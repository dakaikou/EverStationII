#if !defined(AFX_YUVMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
#define AFX_YUVMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// YUVMagicView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CYUVMagicView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//#include "..\main\Mpeg2_GuiApi.h"
#include "..\resource.h"

#define GUI_YUV_PREVIEW			1
#define GUI_YUV_EXCHANGE		0
#define GUI_YUV_COMBINE			1
#define GUI_YUV_SPLITTER		0

#if GUI_YUV_PREVIEW
#include "Dlg_YUVPreview.h"
#endif

#if GUI_YUV_EXCHANGE
#include "Dlg_YUVExchange.h"
#endif

#if GUI_YUV_COMBINE
#include "Dlg_YUVCombine.h"
#endif

class CYUVMagicView : public CFormView
{
public:
	CYUVMagicView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CYUVMagicView)

// Form Data
public:
	//{{AFX_DATA(CYUVMagicView)
	enum { IDD = IDD_MAGIC_YUV };
	//}}AFX_DATA

// Attributes
public:

#if GUI_YUV_PREVIEW
	CDlg_YUVPreview			m_dlgPreview;
#endif

#if GUI_YUV_EXCHANGE
	CDlg_YUVExchange		m_dlgExchange;
#endif

#if GUI_YUV_COMBINE
	CDlg_YUVCombine			m_dlgCombine;
#endif

	// Operations
public:

	CImageList	m_imageTab;

	CWnd*	m_pdlgWnd[16];

	static CYUVMagicView* GetView(void);

protected:
	int			m_bInitDone;

	void AdjustLayout(int cx, int cy);
	void ActivateTabPage(int nSel);
	void InitConsoleTab(void);
	void GUIReset(void);
	void TermAnalyseThread(void);

	int			TAB_YUV_PREVIEW;
	int			TAB_YUV_EXCHANGE;
	int			TAB_YUV_COMBINE;
	int			TAB_YUV_SPLITTE;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYUVMagicView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CYUVMagicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CYUVMagicView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTabConsole(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnTSMagicOfflineThreadMsg(WPARAM wParam, LPARAM lParam);
//	afx_msg void OnTSMagicRealtimeThreadMsg(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YUVMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
