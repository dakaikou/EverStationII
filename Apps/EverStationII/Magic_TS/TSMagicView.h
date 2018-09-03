#if !defined(AFX_TSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
#define AFX_TSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TSMagicView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTSMagicView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_Trigger.h"
#include "TSMagic_Trigger_Section.h"
#include "TSMagic_Trigger_PesEs.h"

#include "TabView_TSMagic.h"
#include "..\Common\Dlg_Progress.h"

#if GUI_TS_ANALYZER_PACKETS
#include "Dlg_TSAnalyzer_TSPacket.h"
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
#include "Dlg_TSAnalyzer_PacketTrigger.h"
#endif

#if GUI_TS_ANALYZER_PSISI
#include "Dlg_TSAnalyzer_PsiSi.h"
#endif

//#if GUI_TS_SEC_TRIGGER
//#include "Dlg_SectionTrigger.h"
//#endif

#if GUI_TS_ANALYZER_BOUQUETS
#include "Dlg_TSAnalyzer_Bouquet.h"
#endif

#if GUI_TS_ANALYZER_EPG
#include "Dlg_TSAnalyzer_Epg.h"
#endif

#if GUI_TS_ANALYZER_OVERVIEW
#include "Dlg_TSAnalyzer_Overview.h"
#endif

#if GUI_TS_ANALYZER_NETWORK
#include "Dlg_TSAnalyzer_Services.h"
#endif

//#if GUI_TS_PK_MODIFY
//#include "Dlg_PacketModify.h"
//#endif

#if GUI_TS_ANALYZER_PCR
#include "Dlg_TSAnalyzer_Pcr.h"
#endif

#if GUI_TS_ANALYZER_PESES
#include "Dlg_TSAnalyzer_PesEs.h"
#endif

#if GUI_TS_ANALYZER_ES
#include "Dlg_TSAnalyzer_ES.h"
#endif

#if GUI_TS_ANALYZER_TELETEXT
#include "Dlg_TSAnalyzer_Teletext.h"
#endif

#if GUI_TS_ANALYZER_SUBTITLE
#include "Dlg_TSAnalyzer_Subtitle.h"
#endif

#if GUI_TS_ANALYZER_OCDC
//#include "Dlg_TSAnalyzer_OCDC.h"
#include "Dlg_TSAnalyzer_Dsmcc.h"
#endif

#include "Dlg_TriggerWaiting.h"
#include "Dlg_SignalStatus.h"

#include "afxcmn.h"

class CTSMagicView : public CFormView
{
public:
	CTSMagicView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTSMagicView)

// Form Data
public:
	//{{AFX_DATA(CTSMagicView)
	enum { IDD = IDD_MAGIC_TS };
	//CListCtrl m_listLog;
	//}}AFX_DATA

// Attributes
public:

#if GUI_TS_ANALYZER_PACKETS
	CDlg_TSAnalyzer_Packets	m_dlgTSAnalyzerPackets;
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
	CDlg_TSAnalyzer_PacketTrigger  m_dlgPacketTrigger;
#endif

#if GUI_TS_PK_MODIFY
	CDlg_PacketModify	m_dlgPacketModify;
#endif

#if GUI_TS_ANALYZER_PSISI
	CDlg_TSAnalyzer_PsiSi	m_dlgTSAnalyzerPsiSi;
#endif

#if GUI_TS_ANALYZER_BOUQUETS
	CDlg_TS_Analyzer_Bouquets		m_dlgTSAnalyzerBouquets;
#endif

#if GUI_TS_ANALYZER_EPG
	CDlg_TSAnalyzer_Epg			m_dlgTSAnalyzerEpg;
#endif

#if GUI_TS_ANALYZER_OVERVIEW
	CDlg_TSAnalyzer_Overview		m_dlgTSAnalyzerOverview;
#endif

#if GUI_TS_ANALYZER_NETWORK
	CDlg_TSAnalyzer_Services		m_dlgTSAnalyzerServices;
#endif

#if GUI_TS_ANALYZER_PCR
	CDlg_TSAnalyzer_Pcr			m_dlgTSAnalyzerPcr;
#endif

#if GUI_TS_ANALYZER_PESES
	CDlg_TSAnalyzer_PesEs			m_dlgTSAnalyzerPesEs;
#endif

#if GUI_TS_ANALYZER_ES
	CDlg_TSAnalyzer_Es			m_dlgTSAnalyzerEs;
#endif

#if GUI_TS_ANALYZER_TELETEXT
	CDlg_TSAnalyzer_Teletext	m_dlgTSAnalyzerTeletext;
#endif

#if GUI_TS_ANALYZER_SUBTITLE
	CDlg_TSAnalyzer_Subtitle	m_dlgTSAnalyzerSubtitle;
#endif

#if GUI_TS_ANALYZER_OCDC
	//CDlg_TSAnalyzer_OCDC		m_dlgTSAnalyzerOCDC;
	CDlg_TSAnalyzer_Dsmcc		m_dlgTSAnalyzerDsmcc;
#endif

	CDlg_TriggerWaiting		m_dlgTSTriggerWaiting;
	CDlg_TriggerWaiting		m_dlgPESTriggerWaiting;
	CDlg_TriggerWaiting		m_dlgESTriggerWaiting;
	CDlg_TriggerWaiting		m_dlgSectionTriggerWaiting;
	CDlg_TriggerWaiting		m_dlgDsmccDownloadWaiting;

	CDlg_Progress			m_dlgTSDecimateProgress;

	CDlg_SignalStatus		m_dlgSignalStatus;

// Operations
public:

	CImageList	m_imageDebug;

	CImageList	m_imageTab;
//	CImageList	m_imageLog;

	CWnd*		m_pdlgWnd[16];

	static CTSMagicView* GetView(void);

	CDB_TSPackets* GetTSPacketsDBase(void);
	CDB_Pcrs* GetPcrsDBase(void);
	CDB_PsiSiTables* GetPsiSiTablesDBase(void);
	CDB_OCDCs* GetOCDCsDBase(void);

	CTransportStream* GetTStream(void);

	CTrigger_PsiSiSection* GetSectionTrigger(void);
	CTrigger_TSPacket*   GetTSPacketTrigger(void);
	CTrigger_PESPacket*   GetPESPacketTrigger(void);

	thread_params_t		m_kThreadParams;

protected:

	CTabView_TSMagic*			m_pMagicTab;

	int			TAB_OVERVIEW;
	int			TAB_SERVICES;
	int			TAB_EPG;
	int			TAB_PCR;
	int			TAB_PTSDTS;
	int			TAB_PACKET;
	int			TAB_PK_TRIGGER;
	int			TAB_PK_MODIFY;
	int			TAB_SECTION;
	int			TAB_SEC_TRIGGER;
	int			TAB_BOUQUET;
	int			TAB_PESES;
	int			TAB_ES;
	int			TAB_WATCH_TELETEXT;
	int			TAB_WATCH_SUBTITLE;
	int			TAB_WATCH_OCDC;

	CTransportStream	m_transport_stream;

	CDB_TSPackets		m_DB_TSPackets;
	CDB_Pcrs			m_DB_Pcrs;
	CDB_PsiSiTables		m_DB_PsiSiTables;
	CDB_OCDCs			m_DB_OCDCs;

	CTrigger_PsiSiSection	m_Trigger_Section;
	CTrigger_TSPacket		m_Trigger_TSPacket;
	CTrigger_PESPacket		m_Trigger_PESPacket;

	int			m_bInitDone;
	CString		m_strTSInputOption;
	CString		m_strTSInputAttribute;

	CSize		m_lastClientSize;

	void AdjustLayout(int cx, int cy);
	void ActivateTabPage(int nSel);
	void InitConsoleTab(void);
	void GUIReset(void);

	void loadCfg(void);

	CMFCStatusBar& GetStatusBar(void) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTSMagicView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTSMagicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTSMagicView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtnOpen();
	afx_msg void OnBtnStream();
	afx_msg void OnSelchangeTabConsole(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolPcrDebug();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnTSMagicOfflineThreadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSMagicRealtimeThreadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSMagicDownloadThreadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSMagicDecimateThreadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSMagicRecordThreadMsg(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnTSAppendLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSEtr290Log(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSReportRatio(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSReportFileSize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSDownloadLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSDownloadRatio(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSDecimateLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSDecimateRatio(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSRecordLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSRecordRatio(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSRecordSize(WPARAM wParam, LPARAM lParam);

//	afx_msg LRESULT OnTSReportBitrate(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnTSReportBitrateMap(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTSReportPacketLength(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnTSReportSignalStatus(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnReportTSTriggerStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReportPESTriggerStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReportESTriggerStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReportSectionTriggerStatus(WPARAM wParam, LPARAM lParam);

//	afx_msg void OnDestroy();
	//afx_msg void OnTimer(UINT nIDEvent);
	//CProgressCtrl m_progressFile;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
