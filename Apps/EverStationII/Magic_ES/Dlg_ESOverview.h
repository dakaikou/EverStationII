#if !defined(AFX_DLG_ESOVERVIEW_H__98012E88_0C0F_44BD_817F_189BA64FCC46__INCLUDED_)
#define AFX_DLG_ESOVERVIEW_H__98012E88_0C0F_44BD_817F_189BA64FCC46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_ESOverview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_ESOverview dialog
#include "..\resource.h"

class CDlg_ESOverview : public CDialog
{
// Construction
public:
	CDlg_ESOverview(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_ESOverview)
	enum { IDD = IDD_ES_OVERVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void Reset(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_ESOverview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_ESOverview)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_ESOVERVIEW_H__98012E88_0C0F_44BD_817F_189BA64FCC46__INCLUDED_)
