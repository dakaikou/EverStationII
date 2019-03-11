#if !defined(AFX_DLG_YUVPREVIEW_H__221BFB20_CA21_47EC_8A18_334E5A1D5896__INCLUDED_)
#define AFX_DLG_YUVPREVIEW_H__221BFB20_CA21_47EC_8A18_334E5A1D5896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_YUVPreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVPreview dialog
#include <afxcmn.h>
#include "..\resource.h"
#include "..\Common\Dlg_VideoShowScreen.h"
#include "..\Common\Dlg_Progress.h"

#include "..\Common\YUVPlayer.h"

#include "MiddleWare/MiddleWare_ESDecoder/Include/VideoDecoder_YUV.h"

#define WM_VIDEO_CONTAINER_REPORT_PSNR_EXIT	WM_USER + 0x6af9

class CDlg_YUVPreview : public CDialog
{
// Construction
public:
	CDlg_YUVPreview(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_YUVPreview)
	enum { IDD = IDD_YUV_PREVIEW };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_YUVPreview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	CDlg_Progress	m_dlgProgress;

	void Reset(void);
//	void Set(void);

	void ReportFramePSNR(int nFrameNum, double psnr);

protected:

	CDlg_VideoShowScreen m_dlgVideo;
	CYUV_VideoDecoder	 m_YUVDecoder;
	//CButton		 m_splitbtnPreview;

	void ParseWandH(char* path, int *w, int *h);
	void ParseFPS(char* path, int *fps);

	void InitComboxForFrameWH(CComboBox* pCmbBox);
	void InitComboxForSampleStructure(CComboBox* pCmbBox);
	void InitComboxForColorSpace(CComboBox* pCmbBox);
	void InitComboxForFrameRate(CComboBox* pCmbBox);

	// Generated message map functions
	//{{AFX_MSG(CDlg_YUVPreview)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnYuvReferenceFileOpen();
	afx_msg void OnBtnYuvWorkingFileOpen();
	afx_msg void OnBtnYuvFilePreview();
	afx_msg void OnBtnYuvFilePSNR();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnReportPlayThreadExit(WPARAM, LPARAM);
	afx_msg LRESULT OnReportPSNRThreadExit(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	CListCtrl m_listPSNRReport;
};

uint32_t YUV_PSNR_Thread(PVOID pVoid);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_YUVPREVIEW_H__221BFB20_CA21_47EC_8A18_334E5A1D5896__INCLUDED_)
