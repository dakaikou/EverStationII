#if !defined(AFX_DLG_DECIMATE_H__3B794E99_2FBC_4CDE_8CD0_675A8E50975F__INCLUDED_)
#define AFX_DLG_DECIMATE_H__3B794E99_2FBC_4CDE_8CD0_675A8E50975F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Decimate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Decimate dialog
#include "..\resource.h"

typedef struct
{
	int		format;
	char	pszFormat[16];			//VCD.DVD.
	int		bitrate;

	char	pszFile[MAX_PATH];

	int		dispatch_option;

} PS_PACKINFO_t;

typedef struct
{
	int		stream_id;
	char	pszFormat[16];
	int		weight;
	int		height;
//	int		aspect;
	char	pszAspect[16];
//	int		framerate;
	char	pszFramerate[16];
	int		bitrate;

	char	pszFile[MAX_PATH];

} PS_VIDEOINFO_t;

typedef struct
{
	int		stream_id;
//	int		format;
	char	pszFormat[32];
//	int		bitrate;
	char	pszBitrate[16];
//	int		samplerate;
	char	pszSamplerate[16];
//	int		channel;
	char	pszChannel[16];

	char	pszFile[MAX_PATH];

} PS_AUDIOINFO_t;

class CDlg_Decimate : public CDialog
{
// Construction
public:
	CDlg_Decimate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_Decimate)
	enum { IDD = IDD_DECIMATE };
	CComboBox	m_cmbDispatchOption;
	CListCtrl	m_listESInfo;
	CString	m_strAFile;
	CString	m_strVFile;
	//}}AFX_DATA

	PS_PACKINFO_t	m_packinfo;
	PS_VIDEOINFO_t	m_videoinfo;
	PS_AUDIOINFO_t	m_audioinfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Decimate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

public:

	// Generated message map functions
	//{{AFX_MSG(CDlg_Decimate)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnBrowseAfile();
	afx_msg void OnBtnBrowseVfile();
	afx_msg void OnSelchangeCmbDispatchOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_DECIMATE_H__3B794E99_2FBC_4CDE_8CD0_675A8E50975F__INCLUDED_)
