#if !defined(AFX_DLG_TS_ANALYZER_ES_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
#define AFX_DLG_TS_ANALYZER_ES_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_TSPrograms.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PesEs dialog
#include "..\Magic_TS\TSMagic_GuiApi.h"
#include "..\Magic_TS\Dlg_DtsPts.h"
#include "..\Common\Dlg_ShowVideo.h"
#include "..\Common\GuiCommon_TreeFunction.h"

#include "MiddleWare_ESDecoder/Include/VideoDecoder_Mpeg2.h"
#include "MiddleWare_ESDecoder/Include/VideoDecoder_H264.h"
#include "MiddleWare_ESDecoder/Include/VideoDecoder_AVS.h"
#include "MiddleWare_ESDecoder/Include/VideoDecoder_YUV.h"
#include "MiddleWare_ESDecoder/Include/AudioDecoder_Mpeg2.h"
#include "MiddleWare_ESDecoder/Include/AudioDecoder_AC3.h"
#include "MiddleWare_ESDecoder/Include/AudioDecoder_AAC.h"
#include "MiddleWare_ESDecoder/Include/AudioDecoder_WAVE.h"
#include "MiddleWare_ESDecoder/Include/Decoder_MPEG2_PS.h"

#if !MFC_CONTROL
#include "..\MFCExt\TreeList\TreeListCtrl.h"
#endif

class CDlg_TSAnalyzer_Es : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Es(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Es)
	enum { IDD = IDD_TS_ANALYZER_ES };
	CTreeCtrl		m_treePrograms;
	#if MFC_CONTROL
	CTreeCtrl		m_treeESSyntax;
	#else
	CTreeListCtrl	m_treeESSyntax;
	#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Es)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CDlg_TS_DtsPts				m_dlgDtsPts;
public:

	void Reset(void);
	void Set(int offline);
	void InitESTree(void);
	void InitProgramTree(void);

	void CancelTrigger(void);

protected:

	void SetPIDInfo(CPMT* pPMT);
	void SetESInfo(void);

	void UpdateDtsPts(PES_packet_t* pPES_packet, int bAud);

	CImageList	m_ESImageList;
	CDlg_ShowVideo	m_dlgTV;

	int				m_dwAudCode;
	int				m_dwVidCode;
	unsigned short	m_usPCRPID;

	TREEITEM		m_hProgramsItem;
	TREEITEM		m_hPESItem;
	TREEITEM		m_hESItem;

//	TREEITEM		m_hVidPesItem;
//	TREEITEM		m_hVidEsItem;
//	TREEITEM		m_hAudPesItem;
//	TREEITEM		m_hAudEsItem;

	FILE*			fp_debug;

	CMPEG_VideoDecoder		m_M2VDecoder;
	CH264_VideoDecoder		m_H264Decoder;
	CAVS_VideoDecoder		m_AVSDecoder;

	CAC3_AudioDecoder		m_AC3Decoder;
	CAAC_AudioDecoder		m_AACDecoder;
	CMPEG_AudioDecoder		m_M2ADecoder;

//	CESDecoder*			m_pActiveDecoder;
	CESDecoder*			m_pActiveAudDecoder;
	CESDecoder*			m_pActiveVidDecoder;
	int						m_nActiveType;				//0-- video, 1--audio

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_PesEs)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnBtnTrigger();
	afx_msg void OnBtnWatch();
//	afx_msg void OnBtnDts();
//	afx_msg void OnBtnPts();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdatePAT(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePMT(WPARAM wParam, LPARAM lParam);

public:
	afx_msg LRESULT OnUpdatePesHeader(WPARAM wParam, LPARAM lParam);

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

	afx_msg LRESULT OnUpdateH264EsSeqParameterSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateH264EsPicParameterSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateH264EsSliceOfAnIdrPicture(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateH264EsSliceOfANonIdrPicture(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateM2AEsErrorCheck(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateAC3EsSyncinfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateAC3EsBsi(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateAACEsHeader(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateAACEsErrorCheck(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDblclkTreeTsPrograms(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickTreeTsPrograms(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_TS_ANALYZER_ES_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
