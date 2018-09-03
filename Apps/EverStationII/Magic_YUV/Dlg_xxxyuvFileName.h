#if !defined(AFX_DLG_XXXYUVFILENAME_H__AA8AFA32_C620_4718_99CA_8EB6C7F878A1__INCLUDED_)
#define AFX_DLG_XXXYUVFILENAME_H__AA8AFA32_C620_4718_99CA_8EB6C7F878A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_xxxyuvFileName.h : header file
//
#include "..\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_xxxyuvFileName dialog

class CDlg_xxxyuvFileName : public CDialog
{
// Construction
public:
	CDlg_xxxyuvFileName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_xxxyuvFileName)
	enum { IDD = IDD_DYUV_FILE };
	int		m_nNumWidth;
	CString	m_strFilePrefix;
	int		m_nStartIndex;
	int		m_nEndIndex;
	//}}AFX_DATA

public:
	CString m_strFileName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_xxxyuvFileName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_xxxyuvFileName)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLookup();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_XXXYUVFILENAME_H__AA8AFA32_C620_4718_99CA_8EB6C7F878A1__INCLUDED_)
