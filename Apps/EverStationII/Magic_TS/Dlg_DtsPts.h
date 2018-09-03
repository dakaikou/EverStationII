#if !defined(AFX_DLG_DTSPTS_H__A4D33676_F767_4DC7_B74B_BB17525809D2__INCLUDED_)
#define AFX_DLG_DTSPTS_H__A4D33676_F767_4DC7_B74B_BB17525809D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_DtsPts.h : header file
//

#include "..\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TS_DtsPts dialog
#include "..\Common\Graph_Waveform.h"

#define DTS					0
#define PTS					1

class CDlg_TS_DtsPts : public CDialog
{
// Construction
public:
	CDlg_TS_DtsPts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TS_DtsPts)
	enum { IDD = IDD_TS_DTS_PTS };
	CGraph_Oscilloscope	m_graphVidJitter;
	CGraph_Oscilloscope	m_graphVidInterval;
	CGraph_Oscilloscope	m_graphAudJitter;
	CGraph_Oscilloscope	m_graphAudInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TS_DtsPts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_nMode;
public:
	void	Reset(void);
	void	Set(int mode, DWORD dwVidCode, DWORD dwAudCode);
	int		GetMode(void);
	// Generated message map functions
	//{{AFX_MSG(CDlg_TS_DtsPts)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_DTSPTS_H__A4D33676_F767_4DC7_B74B_BB17525809D2__INCLUDED_)
