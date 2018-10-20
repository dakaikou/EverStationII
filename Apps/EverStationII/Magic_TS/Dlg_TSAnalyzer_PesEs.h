#if !defined(AFX_DLG_TS_PESES_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
#define AFX_DLG_TS_PESES_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdint.h>

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PesEs dialog
#include "..\Magic_TS\TSMagic_GuiApi.h"

#include "..\Common\Dlg_ShowVideo.h"
#include "..\Common\TreeView_XMLBrowser.h"
#include "..\Common\HexEditView_ByteBuffer.h"

#if !MFC_CONTROL
#include "..\MFCExt\TreeList\TreeListCtrl.h"
#endif

#include "..\MFCExt\SplitWnd\SplitWnd.h"
#include "NaviTree_ESs.h"

class CDlg_TSAnalyzer_PesEs : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_PesEs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_PesEs)
	enum { IDD = IDD_TS_ANALYZER_PESES };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_PesEs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//CDlg_TS_DtsPts				m_dlgDtsPts;

	CxSplitterWnd				m_wndSplitter;
	CNaviTree_ESs*				m_pPane;
	CTreeView_PacketSyntax*		m_pTree;
	CHexEditView_ByteBuffer*	m_pList;
public:

	void Reset(void);
	void UpdatePAT(CPAT* pPAT);
	void UpdatePMT(CPMT* pPMT);
	void DisplayPESPacket(uint32_t uiPESStyle, uint8_t* PES_buf, int PES_length, HALForXMLDoc* pxmlDoc = NULL);
	void DisplayMPVPacket(U8* es_buf, S32 es_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode = NULL);
	void DisplayAVSPacket(U8* es_buf, S32 es_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode = NULL);
	void DisplayH264Packet(U8* es_buf, S32 es_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode = NULL);
	void DisplayMPAPacket(U8* es_buf, S32 es_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode = NULL, uint32_t ref_sync_word = 0, uint32_t ref_sync_mask = 0, int ref_sync_length = 0);
	void DisplayAACPacket(U8* es_buf, S32 es_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode = NULL);
	void DisplayAC3Packet(U8* es_buf, S32 es_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode = NULL);
	void DisplayDRAPacket(U8* es_buf, S32 es_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode = NULL);
	void DisplayUnknownESPacket(uint8_t* es_buf, int es_size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode = NULL);

protected:

	//void UpdateDtsPts(PES_packet_t* pPES_packet, int bAud);

	CImageList	m_ESImageList;
	CDlg_ShowVideo	m_dlgTV;

	int				m_dwAudCode;
	int				m_dwVidCode;
	unsigned short	m_usPCRPID;

	FILE*			fp_debug;

	//CMPEG_VideoDecoder		m_M2VDecoder;
	//CH264_VideoDecoder		m_H264Decoder;
	//CAVS_VideoDecoder		m_AVSDecoder;

	//CAC3_AudioDecoder		m_AC3Decoder;
	//CAAC_AudioDecoder		m_AACDecoder;
	//CMPEG_AudioDecoder		m_M2ADecoder;

	//CESDecoder*			m_pActiveAudDecoder;
	//CESDecoder*			m_pActiveVidDecoder;
	//int						m_nActiveType;				//0-- video, 1--audio

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_PesEs)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnBtnTrigger();
	afx_msg void OnBtnWatch();
//	afx_msg void OnBtnDts();
//	afx_msg void OnBtnPts();
	//}}AFX_MSG

public:
	//afx_msg LRESULT OnUpdatePesHeader(WPARAM wParam, LPARAM lParam);

	//afx_msg LRESULT OnUpdateM2VEsSequenceHeader(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsSequenceExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsSequenceDisplayExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsSequenceScalableExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsQuantMatrixExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsGroupOfPicturesHeader(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsPictureHeader(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsPictureCodingExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsPictureDisplayExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsPictureTemporalScalableExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsPictureSpatialScalableExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsCopyrightExtension(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsSlice(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VGOPTime(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsSequenceEnd(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2VEsUnknownNal(WPARAM wParam, LPARAM lParam);

	//afx_msg LRESULT OnUpdateH264EsSeqParameterSet(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateH264EsPicParameterSet(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateH264EsSliceOfAnIdrPicture(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateH264EsSliceOfANonIdrPicture(WPARAM wParam, LPARAM lParam);

	//afx_msg LRESULT OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateM2AEsErrorCheck(WPARAM wParam, LPARAM lParam);

	//afx_msg LRESULT OnUpdateAC3EsSyncinfo(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateAC3EsBsi(WPARAM wParam, LPARAM lParam);

	//afx_msg LRESULT OnUpdateAACEsHeader(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnUpdateAACEsErrorCheck(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_TS_PESES_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
