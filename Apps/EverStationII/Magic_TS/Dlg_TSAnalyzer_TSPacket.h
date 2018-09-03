#if !defined(AFX_DLG_PACKETDECIMATE_H__FBFC7F43_5E9E_4800_80C0_14AD9DBC9D22__INCLUDED_)
#define AFX_DLG_PACKETDECIMATE_H__FBFC7F43_5E9E_4800_80C0_14AD9DBC9D22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_PacketDecimate.h : header file
//
#include <afxcmn.h>
#include "..\resource.h"

#include "..\Common\Dlg_Progress.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Packets dialog

class CDlg_TSAnalyzer_Packets : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Packets(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Packets)
	enum { IDD = IDD_TS_ANALYZER_PACKETS };
	CSliderCtrl	m_sliderFile;
	CListCtrl	m_listPID;
	int		m_nStart;
	int		m_nEnd;
	int		m_nLength;
	int		m_nTotal;
	//}}AFX_DATA

	void InitPIDList(void);
	void UpdatePIDList(void);
	void ReportFileSize(__int64 filelength);

	void Set(int offline);
	void Reset(void);
	void ActionMsg(WPARAM wParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Packets)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

//	CDlg_Progress	m_dlgProgress;

//	CString m_strFileName;

	int		m_nLeftMark;
	int		m_nRightMark;
	int		m_bSetLeft;

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_Packets)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDecimateTS2TS();
	afx_msg void OnBtnDecimateTS2PES();
	afx_msg void OnBtnDecimateTS2ES();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBtnLeftmark();
	afx_msg void OnBtnRightmark();
	afx_msg void OnChangeEditStart();
	afx_msg void OnChangeEditLength();
	afx_msg void OnChangeEditEnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDecimateStop();
	afx_msg void OnRclickListPid(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PACKETDECIMATE_H__FBFC7F43_5E9E_4800_80C0_14AD9DBC9D22__INCLUDED_)
