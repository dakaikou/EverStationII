#if !defined(AFX_ESMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
#define AFX_ESMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ESMagicView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CESMagicView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//#include "..\resource.h"
#include "..\Common\Dlg_VideoShowScreen.h"
#include "..\Common\Dlg_Progress.h"
#include "..\Common\TreeView_XMLBrowser.h"
#include "..\Common\HexEditView_ByteBuffer.h"

//#include "NaviTree_ESFrames.h"
#include "NaviList_ESFrames.h"
#include "ESMagic_Analyze_Offline.h"


class CESMagicView : public CFormView
{
public:
	CESMagicView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CESMagicView)

// Form Data
public:
	//{{AFX_DATA(CESMagicView)
	enum { IDD = IDD_MAGIC_ES };
	//}}AFX_DATA

// Attributes
public:

	CDlg_Progress			 m_dlgESAnalyzeProgress;

	CSplitterWnd			 m_wndSplitter;
	CNaviList_ESFrames*		 m_pNaviPane;
	CTreeView_XMLBrowser*	 m_pSyntaxTree;
	CHexEditView_ByteBuffer* m_pHexList;

// Operations
public:

	static CESMagicView* GetView(void);

protected:
	int					m_bInitDone;
	int					m_bOpened;

	es_thread_params_t			m_es_thread_params;

	CDlg_VideoShowScreen		m_dlgVideo;

	//CMPEG_VideoDecoder	m_MPGVDecoder;
	//CH264_VideoDecoder	m_H264Decoder;
	//CAVS_VideoDecoder	m_AVSDecoder;

	//CMPEG_AudioDecoder	m_MPGADecoder;
	//CAC3_AudioDecoder	m_AC3Decoder;
	//CAAC_AudioDecoder	m_AACDecoder;

	//CWAVE_AudioDecoder	m_WAVEDecoder;

	void AdjustLayout(int cx, int cy);
	void Reset(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CESMagicView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CESMagicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CESMagicView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtnOpenOrClose();
	afx_msg void OnBtnPreview();
	afx_msg void OnBtnES2TS();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnUpdateM2VPes(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSequenceHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSequenceExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSequenceDisplayExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSequenceScalableExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsQuantMatrixExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsGroupOfPicturesHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsPictureHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsPictureCodingExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsPictureDisplayExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsPictureTemporalScalableExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsPictureSpatialScalableExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsCopyrightExtension(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSlice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VEsSequenceEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2VGOPTime(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateAVSEsVideoSequenceHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateAVSEsSequenceDisplayExtension(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateH264EsSeqParameterSet(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2AEsErrorCheck(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateAC3EsSyncInfo(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateAACEsHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateAACEsErrorCheck(WPARAM wParam, LPARAM lParam);

	afx_msg	LRESULT OnUpdateWAVEHeader(WPARAM wParam, LPARAM lParam);

	afx_msg	LRESULT OnESMagicAnalyzeThreadMsg(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ESMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
