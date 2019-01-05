#if !defined(AFX_DLG_PACKETTRIGGER_H__A4998815_51A6_4A65_86C6_34B15B3FF01E__INCLUDED_)
#define AFX_DLG_PACKETTRIGGER_H__A4998815_51A6_4A65_86C6_34B15B3FF01E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_PacketTrigger.h : header file
//
#include "..\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PacketTrigger dialog
#include "syntax_translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

#include "TSMagic_Trigger_TSPacket.h"

#include "..\MFCExt\SplitWnd\SplitWnd.h"
#include "..\Common\TreeView_XMLBrowser.h"
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
#include "..\Common\ListView_ByteBuffer.h"
#else
#include "..\Common\HexEditView_ByteBuffer.h"
#endif

class CDlg_TSAnalyzer_PacketTrigger : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_PacketTrigger(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_PacketTrigger)
	enum { IDD = IDD_TS_ANALYZER_PACKET_TRIGGER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_PacketTrigger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//unsigned char	m_ucMask[6];
	//unsigned char	m_ucData[6];
	int				m_bSelectingPID;
	int				m_bSearching;

	CxSplitterWnd				m_wndSplitter;
	CTreeView_PacketSyntax*	m_pTree;
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
	CListView_ByteBuffer*		m_pList;
#else
	CHexEditView_ByteBuffer*	m_pList;
#endif

public:
//	void Set(int offline);
	void Reset(void);
	void UpdateCatchResult(void);
	void UpdatePIDList(void);
	//void DisplayTSPacket(transport_packet_t* ppacket);
	//void DisplayTSPacket(uint8_t* packet_buf, int packet_length);
	//void ClearPacketList( BOOL bFirstTime = FALSE );
	//void ClearPacketTree( BOOL bFirstTime = FALSE );
	void CancelTrigger(void);

protected:
	BOOL VerifySearchConditions( void );
	int GetTriggerParams(uint8_t* ucMask, uint8_t* ucData, int length);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_PacketTrigger)
	virtual BOOL OnInitDialog();
	void OnBtnSearch();
	void OnBtnReset();
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnBtnClear();
	void OnBtnNext();
	void OnBtnPre();
	void OnDropdownCmbPid();
	void OnCloseupCmbPid();
	//void OnSelchangeCmbDiscontinuityIndicator();
	//void OnSelchangeCmbRandomAccessIndicator();
	//void OnSelchangeCmbElementaryStreamPriorityIndicator();
	void OnSelchangeCmbPcrFlag();
	//void OnSelchangeCmbOpcrFlag();
	//void OnSelchangeCmbSplicingPointFlag();
	//void OnSelchangeCmbTransportPrivateDataFlag();
	//void OnSelchangeCmbAdaptationFieldExtensionFlag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PACKETTRIGGER_H__A4998815_51A6_4A65_86C6_34B15B3FF01E__INCLUDED_)
