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

#define GUI_TS_ANALYZER_OVERVIEW			1
#define GUI_TS_ANALYZER_NETWORK				0
#define GUI_TS_ANALYZER_EPG					1
#define GUI_TS_ANALYZER_PCR					1
#define GUI_TS_ANALYZER_PACKETS				1
#define GUI_TS_ANALYZER_PSISI				1
#define GUI_TS_ANALYZER_BOUQUETS			1
#define GUI_TS_ANALYZER_PESES				1
#define GUI_TS_ANALYZER_ES					0
#define GUI_TS_ANALYZER_TELETEXT			0
#define GUI_TS_ANALYZER_SUBTITLE			0
#define GUI_TS_ANALYZER_OCDC				1
#define GUI_TS_ANALYZER_PK_TRIGGER			1
#define GUI_TS_PTSDTS						1
#define GUI_TS_PK_MODIFY					0
#define GUI_TS_SEC_TRIGGER					0

#define BYTE_BUFFER_USE_LISTCTRL_VIEW				0

#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "TSMagic_AnalyseThread.h"
#include "TSMagic_Trigger.h"
#include "TSMagic_Trigger_Section.h"
#include "TSMagic_Trigger_PesEs.h"

#include "..\Common\Dlg_Progress.h"

#if GUI_TS_ANALYZER_OVERVIEW
#include "Dlg_TSAnalyzer_Metering_Overview.h"
#endif

#if GUI_TS_ANALYZER_PCR
#include "Dlg_TSAnalyzer_Metering_Pcr.h"
#endif

#if GUI_TS_ANALYZER_PACKETS
#include "Dlg_TSAnalyzer_Metering_TSPackets.h"
#endif

#if GUI_TS_ANALYZER_EPG
#include "Dlg_TSAnalyzer_Semantic_Epg.h"
#endif

#if GUI_TS_ANALYZER_BOUQUETS
#include "Dlg_TSAnalyzer_Semantic_Bouquet.h"
#endif

#if GUI_TS_ANALYZER_NETWORK
#include "Dlg_TSAnalyzer_Semantic_Services.h"
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
#include "Dlg_TSAnalyzer_Syntax_TSPacket.h"
#endif

#if GUI_TS_ANALYZER_PSISI
#include "Dlg_TSAnalyzer_Syntax_PsiSi.h"
#endif

#if GUI_TS_ANALYZER_PESES
#include "Dlg_TSAnalyzer_Syntax_PesEs.h"
#endif

#if GUI_TS_ANALYZER_OCDC
#include "Dlg_TSAnalyzer_SemanticEx_Dsmcc.h"
#endif

#if GUI_TS_ANALYZER_TELETEXT
#include "Dlg_TSAnalyzer_SemanticEx_Teletext.h"
#endif

#if GUI_TS_ANALYZER_SUBTITLE
#include "Dlg_TSAnalyzer_SemanticEx_Subtitle.h"
#endif

#include "Dlg_TSAssist_TriggerWaiting.h"
#include "Dlg_TSAssist_SignalStatus.h"

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
	CDlg_TSAnalyzer_Metering_TSPackets	m_dlgTSAnalyzerPackets;
#endif

#if GUI_TS_ANALYZER_PK_TRIGGER
	CDlg_TSAnalyzer_Syntax_TSPacket  m_dlgPacketTrigger;
#endif

#if GUI_TS_PK_MODIFY
	CDlg_PacketModify	m_dlgPacketModify;
#endif

#if GUI_TS_ANALYZER_PSISI
	CDlg_TSAnalyzer_Syntax_PsiSi	m_dlgTSAnalyzerPsiSi;
#endif

#if GUI_TS_ANALYZER_BOUQUETS
	CDlg_TSAnalyzer_Semantic_Bouquets		m_dlgTSAnalyzerBouquets;
#endif

#if GUI_TS_ANALYZER_EPG
	CDlg_TSAnalyzer_Semantic_Epg			m_dlgTSAnalyzerEpg;
#endif

#if GUI_TS_ANALYZER_OVERVIEW
	CDlg_TSAnalyzer_Metering_Overview		m_dlgTSAnalyzerOverview;
#endif

#if GUI_TS_ANALYZER_NETWORK
	CDlg_TSAnalyzer_Services		m_dlgTSAnalyzerServices;
#endif

#if GUI_TS_ANALYZER_PCR
	CDlg_TSAnalyzer_Metering_Pcr			m_dlgTSAnalyzerPcr;
#endif

#if GUI_TS_ANALYZER_PESES
	CDlg_TSAnalyzer_Syntax_PesEs			m_dlgTSAnalyzerPesEs;
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
	CDlg_TSAnalyzer_SemanticEx_Dsmcc		m_dlgTSAnalyzerDsmcc;
#endif

	CDlg_TSAssist_TriggerWaiting		m_dlgTSTriggerWaiting;
	CDlg_TSAssist_TriggerWaiting		m_dlgPESTriggerWaiting;
	//CDlg_TriggerWaiting		m_dlgESTriggerWaiting;
	CDlg_TSAssist_TriggerWaiting		m_dlgSectionTriggerWaiting;

	CDlg_TSAssist_TriggerWaiting		m_dlgDsmccDownloadWaiting;
	CDlg_Progress			m_dlgTSDecimateProgress;

	CDlg_TSAssist_SignalStatus		m_dlgSignalStatus;

// Operations
public:

	CImageList	m_imageDebug;

	CImageList	m_imageTab;
//	CImageList	m_imageLog;

	CWnd*		m_pdlgWnd[16];

	static CTSMagicView* GetView(void);

	CDB_TSPackets* GetTSPacketsDBase(void);
	CDB_Pcrs* GetPcrsDBase(void);
	CDB_PsiSiObjs* GetPsiSiObjsDBase(void);
	CDB_OCDCs* GetOCDCsDBase(void);

	CTransportStream* GetTStream(void);

	CTrigger_PsiSiSection* GetSectionTrigger(void);
	CTrigger_TSPacket*   GetTSPacketTrigger(void);
	CTrigger_PESPacket*   GetPESPacketTrigger(void);

	ts_thread_params_t		m_kThreadParams;

protected:

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
	int			m_nCurActiveTab;

	CTransportStream	m_transport_stream;

	CDB_TSPackets		m_DB_TSPackets;
	CDB_Pcrs			m_DB_Pcrs;
	CDB_PsiSiObjs		m_DB_PsiSiObjs;
	CDB_OCDCs			m_DB_OCDCs;

	CTrigger_PsiSiSection	m_Trigger_Section;
	CTrigger_TSPacket		m_Trigger_TSPacket;
	CTrigger_PESPacket		m_Trigger_PESPacket;

	int			m_bInitDone;
	CString		m_strTSInputOption;
	CString		m_strTSInputAttribute;

	//CSize		m_lastClientSize;

	void AdjustLayout(int cx, int cy);
	void ActivateTabPage(int nOldSel, int nNewSel);
	void InitConsoleTab(void);
	void InitThreadparams(void);

	void GUIReset(void);

	void LoadInterfaceCfg(void);

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
	afx_msg void OnBtnFileOpenOrClose();
	afx_msg void OnBtnStreamOpenOrClose();
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
	//afx_msg LRESULT OnReportESTriggerStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReportSectionTriggerStatus(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSMAGICVIEW_H__FB2BAB5A_70A0_40FB_93C2_D8E22027F42B__INCLUDED_)
