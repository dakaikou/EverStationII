// TSMagicFrm.h : interface of the CTSMagicFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TSMAGICFRM_H__6BEF92BE_5587_4485_9B88_E3756BB657C3__INCLUDED_)
#define AFX_TSMAGICFRM_H__6BEF92BE_5587_4485_9B88_E3756BB657C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "TSMagicView.h"

class CTSMagicFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CTSMagicFrame)
public:
	CTSMagicFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTSMagicFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	// view for the client area of the frame.
	//CTSMagicView*	m_pwndView;

	virtual ~CTSMagicFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CTSMagicFrame)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSMAGICFRM_H__6BEF92BE_5587_4485_9B88_E3756BB657C3__INCLUDED_)
