#if !defined(AFX_DLG_YUVCOMBINE_H__B2279C4B_7725_4F93_8BB6_9077A794B53B__INCLUDED_)
#define AFX_DLG_YUVCOMBINE_H__B2279C4B_7725_4F93_8BB6_9077A794B53B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_YUVCombine.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVCombine dialog
#include "..\resource.h"
#include "..\Common\Dlg_Progress.h"

class CDlg_YUVCombine : public CDialog
{
// Construction
public:
	CDlg_YUVCombine(CWnd* pParent = NULL);   // standard constructor

	void Reset(void);
// Dialog Data
	//{{AFX_DATA(CDlg_YUVCombine)
	enum { IDD = IDD_YUV_COMBINE };
	int		m_nCombineType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_YUVCombine)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CDlg_Progress	m_dlgProgress;

	// Generated message map functions
	//{{AFX_MSG(CDlg_YUVCombine)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDo();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnReset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listSourceFile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_YUVCOMBINE_H__B2279C4B_7725_4F93_8BB6_9077A794B53B__INCLUDED_)
