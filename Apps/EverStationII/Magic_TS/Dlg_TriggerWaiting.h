#if !defined(AFX_TRIGGERWAITINGDLG_H__4236FFA3_15DA_446B_B5F5_152D0089B78F__INCLUDED_)
#define AFX_TRIGGERWAITINGDLG_H__4236FFA3_15DA_446B_B5F5_152D0089B78F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TriggerWaitingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TriggerWaiting dialog
#include "..\resource.h"

class CDlg_TriggerWaiting : public CDialog
{
// Construction
public:
	CDlg_TriggerWaiting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TriggerWaiting)
	enum { IDD = IDD_TRIGGER_WAITING };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TriggerWaiting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_nWaitingTime;					//units:	s
	// Generated message map functions
	//{{AFX_MSG(CDlg_TriggerWaiting)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGERWAITINGDLG_H__4236FFA3_15DA_446B_B5F5_152D0089B78F__INCLUDED_)
