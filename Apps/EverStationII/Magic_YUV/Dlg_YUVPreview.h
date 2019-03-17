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

#define WM_VIDEO_CONTAINER_REPORT_PSNR_EXIT			WM_USER + 0x6af9
#define WM_VIDEO_CONTAINER_REPORT_TRANSCODE_EXIT	WM_USER + 0x6d2a


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

	CDlg_Progress		 m_dlgProgress;

	YUV_SEQUENCE_PARAM_t	 m_stSrcSequenceParams;
	YUV_SEQUENCE_PARAM_t	 m_stDstSequenceParams;
	CString				 m_strReferenceFile;
	CString				 m_strWorkingFile;
	CString				 m_strSavingFile;

	void Reset(void);
//	void Set(void);

	void ReportFramePSNR(int nFrameNum, double Ypsnr, double Upsnr, double Vpsnr);

protected:

	CYUV_VideoDecoder	 m_ReferenceYUVDecoder;
	CYUV_VideoDecoder	 m_WorkingYUVDecoder;

	CDlg_VideoShowScreen m_dlgVideo;
	//CButton		 m_splitbtnPreview;
	//YUV_PSNR_ThreadParams_t m_psnr_thread_params;

	void CheckSrcFrameParameters(YUV_SEQUENCE_PARAM_t* pstYuvParams);
	void CheckDstFrameParameters(YUV_SEQUENCE_PARAM_t* pstYuvParams);

	void ParseWandH(char* path, int *w, int *h);
	void ParseFPS(char* path, int *fps);

	void InitComboxForFileType(CComboBox* pCmbBox);
	void InitComboxForFrameWH(CComboBox* pCmbBox);
	void InitComboxForSampleStructure(CComboBox* pCmbBox);
	void InitComboxForColorSpace(CComboBox* pCmbBox);
	void InitComboxForFrameRate(CComboBox* pCmbBox);

	// Generated message map functions
	//{{AFX_MSG(CDlg_YUVPreview)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnYuvReferenceFileOpen();
	afx_msg void OnBtnYuvWorkingFileOpen();
	afx_msg void OnBtnYuvSavingFileOpen();
	afx_msg void OnBtnYuvFilePreview();
	afx_msg void OnBtnYuvFileCalculatePSNR();
	afx_msg void OnBtnYuvFileTranscode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnReportPlayThreadExit(WPARAM, LPARAM);
	afx_msg LRESULT OnReportPSNRThreadExit(WPARAM, LPARAM);
	afx_msg LRESULT OnReportTranscodeThreadExit(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	CListCtrl m_listPSNRReport;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_YUVPREVIEW_H__221BFB20_CA21_47EC_8A18_334E5A1D5896__INCLUDED_)
