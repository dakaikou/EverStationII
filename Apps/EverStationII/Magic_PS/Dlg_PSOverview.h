#if !defined(AFX_DLG_PSOVERVIEW_H__BF84E0E4_DDA2_4966_8534_96A7EEFB90D2__INCLUDED_)
#define AFX_DLG_PSOVERVIEW_H__BF84E0E4_DDA2_4966_8534_96A7EEFB90D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_PSOverview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_PSOverview dialog
#include "..\resource.h"

class CDlg_PSOverview : public CDialog
{
// Construction
public:
	CDlg_PSOverview(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_PSOverview)
	enum { IDD = IDD_PS_OVERVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void Reset(void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_PSOverview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_PSOverview)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PSOVERVIEW_H__BF84E0E4_DDA2_4966_8534_96A7EEFB90D2__INCLUDED_)
