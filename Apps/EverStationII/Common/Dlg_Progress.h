#if !defined(AFX_DLG_PROGRESS_H__432BBBEB_C5BA_4A38_9EBA_D8CAA6FE686B__INCLUDED_)
#define AFX_DLG_PROGRESS_H__432BBBEB_C5BA_4A38_9EBA_D8CAA6FE686B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Progress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Progress dialog
#include "..\resource.h"

class CDlg_Progress : public CDialog
{
// Construction
public:
	CDlg_Progress(CWnd* pParent = NULL);   // standard constructor

	void		SetTitle(CString strTitle);
	void		SetPos(int ratio);					//num%
	void		AddMsg(char* pszMsg);
	void		Reset(void);
	void		SetRecordMsg(int filesize);

// Dialog Data
	//{{AFX_DATA(CDlg_Progress)
	enum { IDD = IDD_ANALYSE_PROGRESS };
	CListCtrl		m_listLog;
	CProgressCtrl	m_progress;
	int				m_defSize;
	int				m_nRecordedSize;
	int				m_nOldRatio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Progress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int			m_nCount;

	// Generated message map functions
	//{{AFX_MSG(CDlg_Progress)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnDispatchMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PROGRESS_H__432BBBEB_C5BA_4A38_9EBA_D8CAA6FE686B__INCLUDED_)
