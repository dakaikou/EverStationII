#if !defined(AFX_SIGNALSTATUSDLG_H__4236FFA3_15DA_446B_B5F5_152D0089B78F__INCLUDED_)
#define AFX_SIGNALSTATUSDLG_H__4236FFA3_15DA_446B_B5F5_152D0089B78F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SignalStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_SignalStatus dialog
#include <afxwin.h>
#include "..\resource.h"

class CDlg_TSAssist_SignalStatus : public CDialog
{
// Construction
public:
	CDlg_TSAssist_SignalStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAssist_SignalStatus)
	enum { IDD = IDD_SIGNAL_STATUS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAssist_SignalStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAssist_SignalStatus)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIGNALSTATUSDLG_H__4236FFA3_15DA_446B_B5F5_152D0089B78F__INCLUDED_)
