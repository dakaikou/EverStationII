// Dlg_TSAnalyzer_PacketTrigger.cpp : implementation file
//

#include "StdAfx.h"
//#include "EverStation.h"
#include "Dlg_TSAnalyzer_PacketTrigger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PacketTrigger dialog
#include <assert.h>
#include "TSMagicView.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "syntax_express_xml/XML_MPEG2_TSPacket/Include/Mpeg2_TS_packet_xml.h"
#include "translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_PID.h"
#include "toolbox_libs\TOOL_Directory\Include\TOOL_Directory.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

//using namespace std;
//using namespace tinyxml2;

CDlg_TSAnalyzer_PacketTrigger::CDlg_TSAnalyzer_PacketTrigger(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_PacketTrigger::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_PacketTrigger)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bSelectingPID = 0;

	m_bSearching = 0;

	m_pTree = NULL;
	m_pList = NULL;
}


void CDlg_TSAnalyzer_PacketTrigger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_PacketTrigger)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_PacketTrigger, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_PacketTrigger)
	ON_BN_CLICKED(IDC_BTN_TSPACKET_SEARCH, OnBtnSearch)
	ON_BN_CLICKED(IDC_BTN_TSPACKET_RESET, OnBtnReset)
	ON_BN_CLICKED(IDC_BTN_TSPACKET_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_TSPACKET_NEXT, OnBtnNext)
	ON_BN_CLICKED(IDC_BTN_TSPACKET_PRE, OnBtnPre)
	ON_CBN_DROPDOWN(IDC_CMB_TSPACKET_PID, OnDropdownCmbPid)
	ON_CBN_CLOSEUP(IDC_CMB_TSPACKET_PID, OnCloseupCmbPid)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_DISCONTINUITY_INDICATOR, OnSelchangeCmbDiscontinuityIndicator)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_RANDOM_ACCESS_INDICATOR, OnSelchangeCmbRandomAccessIndicator)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_ELEMENTARY_STREAM_PRIORITY_INDICATOR, OnSelchangeCmbElementaryStreamPriorityIndicator)
	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_PCR_FLAG, OnSelchangeCmbPcrFlag)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_OPCR_FLAG, OnSelchangeCmbOpcrFlag)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_SPLICING_POINT_FLAG, OnSelchangeCmbSplicingPointFlag)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_TRANSPORT_PRIVATE_DATA_FLAG, OnSelchangeCmbTransportPrivateDataFlag)
//	ON_CBN_SELCHANGE(IDC_CMB_TSPACKET_ADAPTATION_FIELD_EXTENSION_FLAG, OnSelchangeCmbAdaptationFieldExtensionFlag)
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PacketTrigger message handlers

//void CDlg_TSAnalyzer_PacketTrigger::Set(int offline)
//{
//}

void CDlg_TSAnalyzer_PacketTrigger::Reset(void)
{
	CComboBox*   pCtrl;
	CWnd*		 pBtn;
	CSliderCtrl* pSlider;
	int			 i;
	CString		 strTemp;
	
	pCtrl = (CComboBox*)(GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_ERROR_INDICATOR ));
	pCtrl->ResetContent();
	pCtrl->AddString( "0(B)" );
	pCtrl->AddString( "1(B))" );
	pCtrl->AddString( "X(B)" );
	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_PAYLOAD_UNIT_START_INDICATOR );
	pCtrl->ResetContent();
	pCtrl->AddString( "0(B)" );
	pCtrl->AddString( "1(B)" );
	pCtrl->AddString( "X(B)" );
	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_PRIORITY );
	pCtrl->ResetContent();
	pCtrl->AddString( "0(B)" );
	pCtrl->AddString( "1(B)" );
	pCtrl->AddString( "X(B)" );
	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_PID);
	pCtrl->ResetContent();

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_SCRAMBLING_CONTROL );
	pCtrl->ResetContent();
	pCtrl->AddString( "00(B)" );
	pCtrl->AddString( "01(B)" );
	pCtrl->AddString( "10(B)" );
	pCtrl->AddString( "11(B)" );
	pCtrl->AddString( "0X(B)" );
	pCtrl->AddString( "1X(B)" );
	pCtrl->AddString( "X0(B)" );
	pCtrl->AddString( "X1(B)" );
	pCtrl->AddString( "XX(B)" );
	pCtrl->SetCurSel( 8 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_ADAPTATION_FIELD_CONTROL );
	pCtrl->ResetContent();
	pCtrl->AddString( "00(B)" );
	pCtrl->AddString( "01(B)" );
	pCtrl->AddString( "10(B)" );
	pCtrl->AddString( "11(B)" );
	pCtrl->AddString( "0X(B)" );
	pCtrl->AddString( "1X(B)" );
	pCtrl->AddString( "X0(B)" );
	pCtrl->AddString( "X1(B)" );
	pCtrl->AddString( "XX(B)" );
	pCtrl->SetCurSel( 8 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_CONTINUITY_COUNTER );
	pCtrl->ResetContent();
	for (i = 0; i < 16; i ++ )
	{
		strTemp.Format( "%X(H)", i );
		pCtrl->AddString( strTemp );
	}
	pCtrl->AddString( "X(H)" );
	pCtrl->SetCurSel( 16 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_DISCONTINUITY_INDICATOR );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_RANDOM_ACCESS_INDICATOR );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_ELEMENTARY_STREAM_PRIORITY_INDICATOR );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_PCR_FLAG );
	pCtrl->ResetContent();
	pCtrl->AddString( "0(B)" );
	pCtrl->AddString( "1(B)" );
	pCtrl->AddString( "X(B)" );
	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_OPCR_FLAG );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_SPLICING_POINT_FLAG );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_PRIVATE_DATA_FLAG );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_ADAPTATION_FIELD_EXTENSION_FLAG );
//	pCtrl->ResetContent();
//	pCtrl->AddString( "0(B)" );
//	pCtrl->AddString( "1(B)" );
//	pCtrl->AddString( "X(B)" );
//	pCtrl->SetCurSel( 2 );

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
	pBtn->EnableWindow(FALSE);

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_CLEAR);
	pBtn->EnableWindow(FALSE);

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_PRE);
	pBtn->EnableWindow(FALSE);

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_NEXT);
	pBtn->EnableWindow(FALSE);

	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);
	pSlider->SetRange(0, 0);
	pSlider->SetPos(0);
	pSlider->EnableWindow(FALSE);

	if (m_pList != NULL)
	{
		m_pList->Reset();
	}
	if (m_pTree != NULL)
	{
		m_pTree->Reset();
	}

	UpdateData(FALSE);

	m_bSearching = 0;
}

BOOL CDlg_TSAnalyzer_PacketTrigger::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTreeView_PacketSyntax), CSize(600, 0), NULL);
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CListView_ByteBuffer), CSize(400, 0), NULL);
#else
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CHexEditView_ByteBuffer), CSize(400, 0), NULL);
#endif

		m_pTree = (CTreeView_PacketSyntax*)m_wndSplitter.GetPane(0, 0);
		m_pTree->Init("TS包语法分析");
		m_pTree->m_hNotifyParent = GetSafeHwnd();
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
		m_pList = (CListView_ByteBuffer*)m_wndSplitter.GetPane(0, 1);
#else
		m_pList = (CHexEditView_ByteBuffer*)m_wndSplitter.GetPane(0, 1);
		m_pList->SetBPR(16);
#endif
	}

	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_PacketTrigger::OnBtnSearch() 
{
	// TODO: Add your control notification handler code here
	char		 pszText[MAX_TXT_CHARS];
	CWnd*		 pWnd;
	CSliderCtrl* pSlider;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	assert(pTSMagicView->m_kThreadParams.main_thread_running == 1);

	if ((pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1) ||
		(pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1) ||
		(pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1) ||
		(pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1) ||
		(pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1) ||
		(pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1))
	{
		if (pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "TS包捕捉：未能启动，因为发现TS捕捉线程尚未结束！");
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
		}
		if (pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "TS包捕捉：未能启动，因为发现PES捕捉线程尚未结束！");
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
		}
		if (pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "TS包捕捉：未能启动，因为发现ES捕捉线程尚未结束！");
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
		}
		if (pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "TS包捕捉：未能启动，因为发现section捕捉线程尚未结束！");
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
		}
		if (pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "TS包捕捉：未能启动，因为发现DSMCC下载线程尚未结束！");
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
		}
		if (pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "TS包捕捉：未能启动，因为发现TS录制线程尚未结束！");
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
		}
	}
	else
	{
		unsigned char	ucMask[6];
		unsigned char	ucData[6];
		GetTriggerParams(ucMask, ucData, 6);

		CTrigger_TSPacket* pTriggerTSPacket = pTSMagicView->GetTSPacketTrigger();
		pTriggerTSPacket->SetMatchParams(ucMask, ucData, sizeof(ucMask), MAX_TRIGGER_COUNT);
		//设置完捕捉参数后，如果是离线分析还没有结束，在离线分析线程会进行包捕捉。若离线分析已经结束，需要重启动一个线程。
		//设置完捕捉参数后，如果是实时分析还没有结束，在实时分析线程会进行包捕捉，不需要重新启动线程。

		if (pTSMagicView->m_kThreadParams.offline == 1)
		{
			//如果是离线分析
			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_TS_TRIGGER_STATE, 1, 0);

			if (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)
			{
//				pTSMagicView->m_kThreadParams.ts_trigger_thread_stopped = 0;
				::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_packet_trigger_thread, (LPVOID)&(pTSMagicView->m_kThreadParams), 0, 0);
			}
		}

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_CLEAR);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_PRE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_NEXT);
		pWnd->EnableWindow(FALSE);

		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);
		pSlider->SetPos(0);
		pSlider->SetRange(0, 0);
		pSlider->EnableWindow(FALSE);
	}

	m_bSearching = 1;
	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_PacketTrigger::OnBtnReset() 
{
	// TODO: Add your control notification handler code here
	CComboBox*		pCtrl;
	int				item_count;
	
	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_ERROR_INDICATOR );
	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_PAYLOAD_UNIT_START_INDICATOR );
	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_PRIORITY );
	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_PID);
	item_count = pCtrl->GetCount();
	pCtrl->SetCurSel(item_count - 1);

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_TRANSPORT_SCRAMBLING_CONTROL );
	pCtrl->SetCurSel( 8 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_ADAPTATION_FIELD_CONTROL );
	pCtrl->SetCurSel( 8 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_CONTINUITY_COUNTER );
	pCtrl->SetCurSel( 16 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_DISCONTINUITY_INDICATOR );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_RANDOM_ACCESS_INDICATOR );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_ELEMENTARY_STREAM_PRIORITY_INDICATOR );
//	pCtrl->SetCurSel( 2 );

	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_PCR_FLAG );
	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_OPCR_FLAG );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_SPLICING_POINT_FLAG );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TRANSPORT_PRIVATE_DATA_FLAG );
//	pCtrl->SetCurSel( 2 );

//	pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_ADAPTATION_FIELD_EXTENSION_FLAG );
//	pCtrl->SetCurSel( 2 );
}

void CDlg_TSAnalyzer_PacketTrigger::OnBtnClear() 
{
	// TODO: Add your control notification handler code here
	CWnd*			pBtn = NULL;
	CSliderCtrl*	pSlider = NULL;
//	CTSMagicView*	pWindow = CTSMagicView::GetView();

	if (m_pList != NULL)
	{
		m_pList->Reset();
	}
	if (m_pTree != NULL)
	{
		m_pTree->Reset();
	}

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_CLEAR);
	pBtn->EnableWindow(FALSE);

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_PRE);
	pBtn->EnableWindow(FALSE);

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_NEXT);
	pBtn->EnableWindow(FALSE);

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
	pBtn->EnableWindow(TRUE);

	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);
	pSlider->SetPos(0);
	pSlider->EnableWindow(FALSE);

	m_bSearching = 0;
	
//	if (pWindow != NULL)
//	{
//		if (pWindow->GetSafeHwnd() != NULL)
//		{
//			//用户发出清除动作时，说明用户已经不关心本次捕获结果了。若此时发现捕捉线程仍在运行，应通知该线程关闭。
//			if (pWindow->m_kThreadParams.ts_trigger_thread_running == 1)
//			{
//				pWindow->m_kThreadParams.ts_trigger_thread_running = 0;
////				pWindow->m_kThreadParams.ts_trigger_thread_stopped = 0;
//			}
//		}
//	}
}

void CDlg_TSAnalyzer_PacketTrigger::CancelTrigger(void)
{
	CWnd*			pBtn = NULL;

	pBtn = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
	pBtn->EnableWindow(TRUE);

	m_bSearching = 0;
}

void CDlg_TSAnalyzer_PacketTrigger::OnBtnNext() 
{
	// TODO: Add your control notification handler code here
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);

	CTrigger_TSPacket* pTSPacketTrigger = pTSMagicView->GetTSPacketTrigger();

	int nPos = pSlider->GetPos();
	nPos += 1;
	if (nPos <= pSlider->GetRangeMax())
	{
		pSlider->SetPos(nPos);

		uint8_t* packet_buf;
		int packet_length;

		packet_buf = pTSPacketTrigger->GetCatchedDatas(nPos, &packet_length);

		if (m_pList != NULL)
		{
			m_pList->Reset();
			m_pList->DisplayByteBuffer(packet_buf, packet_length);
		}

		if (m_pTree != NULL)
		{
			TALForXMLDoc xmlDoc;
			int rtcode = MPEG_decode_TS_packet_to_XML(packet_buf, packet_length, &xmlDoc);

			m_pTree->Reset();
			m_pTree->ShowXMLDoc(&xmlDoc);
		}
	}
}

void CDlg_TSAnalyzer_PacketTrigger::OnBtnPre() 
{
	// TODO: Add your control notification handler code here
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);

	CTrigger_TSPacket* pTSPacketTrigger = pTSMagicView->GetTSPacketTrigger();

	int nPos = pSlider->GetPos();
	nPos -= 1;
	if (nPos >= pSlider->GetRangeMin())
	{
		pSlider->SetPos(nPos);

		uint8_t* packet_buf;
		int packet_length;
		packet_buf = pTSPacketTrigger->GetCatchedDatas(nPos, &packet_length);

		if (m_pList != NULL)
		{
			m_pList->Reset();
			m_pList->DisplayByteBuffer(packet_buf, packet_length);
		}
		if (m_pTree != NULL)
		{
			TALForXMLDoc xmlDoc;
			int rtcode = MPEG_decode_TS_packet_to_XML(packet_buf, packet_length, &xmlDoc);

			m_pTree->Reset();
			m_pTree->ShowXMLDoc(&xmlDoc);
		}
	}
}

void CDlg_TSAnalyzer_PacketTrigger::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_TS_PACKET );
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CTrigger_TSPacket* pTSPacketTrigger = pTSMagicView->GetTSPacketTrigger();

	if (pScrollBar == (CScrollBar*)pSlider)
	{
		int nPos = pSlider->GetPos();
		uint8_t* packet_buf;
		int packet_length;

		packet_buf = pTSPacketTrigger->GetCatchedDatas(nPos, &packet_length);

		if (m_pList != NULL)
		{
			m_pList->Reset();
			m_pList->DisplayByteBuffer(packet_buf, packet_length);
		}
		if (m_pTree != NULL)
		{
			TALForXMLDoc xmlDoc;
			int rtcode = MPEG_decode_TS_packet_to_XML(packet_buf, packet_length, &xmlDoc);

			m_pTree->Reset();
			m_pTree->ShowXMLDoc(&xmlDoc);
		}

		UpdateData( FALSE );
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlg_TSAnalyzer_PacketTrigger::UpdateCatchResult(void)
{
	CWnd*			pWnd;
	CSliderCtrl*	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);
	CTSMagicView*	pTSMagicView = CTSMagicView::GetView();
	CTrigger_TSPacket* pTSPacketTrigger = pTSMagicView->GetTSPacketTrigger();

	int	nGotCount = pTSPacketTrigger->GetCatchedCount();
	if (nGotCount > 0)
	{
		if (nGotCount == 1)
		{
			pWnd = GetDlgItem(IDC_BTN_TSPACKET_CLEAR);
			if (!pWnd->IsWindowEnabled())
			{
				pWnd->EnableWindow(TRUE);
			}

			if (!pSlider->IsWindowEnabled())
			{
				pSlider->EnableWindow(TRUE);
			}

			pSlider->SetPos(0);

			uint8_t* packet_buf;
			int packet_length;
			packet_buf = pTSPacketTrigger->GetCatchedDatas(0, &packet_length);

			if (m_pList != NULL)
			{
				m_pList->Reset();
				m_pList->DisplayByteBuffer(packet_buf, packet_length);
			}
			if (m_pTree != NULL)
			{
				TALForXMLDoc xmlDoc;
				transport_packet_t ts_packet;

				int rtcode = MPEG_decode_TS_packet_to_XML(packet_buf, packet_length, &xmlDoc, &ts_packet);

#ifdef _DEBUG
				char	pszExeFile[MAX_PATH];
				char	exeDrive[3];
				char	pszXmlDir[MAX_PATH];
				char	pszFilePath[MAX_PATH];
				GetModuleFileName(NULL, pszExeFile, MAX_PATH);
				exeDrive[0] = pszExeFile[0];
				exeDrive[1] = pszExeFile[1];
				exeDrive[2] = '\0';

				sprintf_s(pszXmlDir, sizeof(pszXmlDir), "%s\\~EverStationII\\xml", exeDrive);
				DIR_BuildDirectory(pszXmlDir);

				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TS_packet_0x%04X.xml", pszXmlDir, ts_packet.PID);
				xmlDoc.SaveFile(pszFilePath);
#endif

				m_pTree->Reset();
				m_pTree->ShowXMLDoc(&xmlDoc);
			}
		}
		else if (nGotCount == 2)
		{
			pWnd = GetDlgItem(IDC_BTN_TSPACKET_PRE);
			if (!pWnd->IsWindowEnabled())
			{
				pWnd->EnableWindow(TRUE);
			}

			pWnd = GetDlgItem(IDC_BTN_TSPACKET_NEXT);
			if (!pWnd->IsWindowEnabled())
			{
				pWnd->EnableWindow(TRUE);
			}
		}

		pSlider->SetRange(0, nGotCount - 1);
	}
}

void CDlg_TSAnalyzer_PacketTrigger::UpdatePIDList(void)
{
	int					nCurSel;
	char				pszCurText[64];
	char				pszPIDText[64];
	int					i;
	int					item_index;
	int					item_count;
	int					pid;
	RECORD_TSPacket_t	TSPacketInfo;
	int					rtcode;
	CSliderCtrl*		pSlider;
	CWnd*				pBtn;
	CWnd*				pWnd;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_TSPackets* pDB_TSPackets = pTSMagicView->GetTSPacketsDBase();
	CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_PID);

	if (pDB_TSPackets->m_total_packet_count)
	{
		if (!m_bSearching)
		{
			pWnd = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
			if (!pWnd->IsWindowEnabled())
			{
				pWnd->EnableWindow(TRUE);
			}
		}

		if (!m_bSelectingPID)
		{
			item_count = pCtrl->GetCount() - 1;
			if (item_count < 0)
			{
				pCtrl->InsertString(0, "0xXXXX(H)" );
				pCtrl->SetItemData(0, EMPTY_PID);
				pCtrl->SetCurSel(0);
				item_count = 0;
			}

			nCurSel = pCtrl->GetCurSel();
			pCtrl->GetLBText(nCurSel, pszCurText);

			item_index = 0;

			for (i = 0; i < MAX_PID_COUNT; i++)
			{
//				ppkinfo = pDB_TSPackets->m_pRecords[i];
				rtcode = pDB_TSPackets->GetRecord(i, &TSPacketInfo);
				if (rtcode == NO_ERROR)
				{
					if (item_index >= item_count)
					{
						sprintf_s(pszPIDText, sizeof(pszPIDText), "0x%04X(%d)", i, i);
						pCtrl->InsertString(item_index, pszPIDText);
						pCtrl->SetItemData(item_index, i);

						item_count ++;
					}
					else if (item_index < item_count)
					{
						pid = (uint32_t)pCtrl->GetItemData(item_index);
						if (i < pid)
						{
							sprintf_s(pszPIDText, sizeof(pszPIDText), "0x%04X(%d)", i, i);
							pCtrl->InsertString(item_index, pszPIDText);
							pCtrl->SetItemData(item_index, i);

							item_count ++;
						}
						else if (i > pid)
						{
							for(int m = pid; m < i; m ++)
							{
								pCtrl->DeleteString(item_index);
								item_count --;
							}
						}
					}

					item_index ++;
				}
			}

			nCurSel = pCtrl->FindString(-1, pszCurText);
			pCtrl->SetCurSel(nCurSel);
		}
	}
	else
	{
		pCtrl->ResetContent();
		pCtrl->AddString( "0xXXXX(H)" );
		pCtrl->SetItemData(0, EMPTY_PID);
		pCtrl->SetCurSel(0);

		if (m_pList != NULL)
		{
			m_pList->Reset();
		}
		if (m_pTree != NULL)
		{
			m_pTree->Reset();
		}

		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TS_PACKET);
		if (pSlider->GetSafeHwnd() != NULL)
		{
			pSlider->SetPos(0);
			pSlider->EnableWindow(FALSE);
		}

		pBtn = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
		if (pBtn->GetSafeHwnd() != NULL)
		{
			pBtn->EnableWindow( FALSE );
		}

		pBtn = GetDlgItem( IDC_BTN_TSPACKET_CLEAR );
		if (pBtn->GetSafeHwnd() != NULL)
		{
			pBtn->EnableWindow( FALSE );
		}

		pBtn = GetDlgItem(IDC_BTN_TSPACKET_PRE);
		if (pBtn->GetSafeHwnd() != NULL)
		{
			pBtn->EnableWindow(FALSE);
		}

		pBtn = GetDlgItem(IDC_BTN_TSPACKET_NEXT);
		if (pBtn->GetSafeHwnd() != NULL)
		{
			pBtn->EnableWindow(FALSE);
		}

		m_bSearching = 0;
	}

//	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_PacketTrigger::OnDropdownCmbPid() 
{
	// TODO: Add your control notification handler code here
	m_bSelectingPID = 1;
}

void CDlg_TSAnalyzer_PacketTrigger::OnCloseupCmbPid() 
{
	// TODO: Add your control notification handler code here
	m_bSelectingPID = 0;	
}

BOOL CDlg_TSAnalyzer_PacketTrigger::VerifySearchConditions( void )
{
	CComboBox* pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_ADAPTATION_FIELD_CONTROL );
	int index = pCtrl->GetCurSel();
	CString strContent;
	pCtrl->GetLBText( index, strContent );

	char* buffer = strContent.GetBuffer(1);
	if( buffer[0] == '1' ){
		return TRUE;
	}
	else{
		MessageBox( "adaptation_field_control not equal to \'10\' or \'11\' ", "condition error" );
		return FALSE;
	}

	return FALSE;
}

/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbDiscontinuityIndicator() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_DISCONTINUITY_INDICATOR) )->SetCurSel( 2 );
		return;
	}
}
*/
/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbRandomAccessIndicator() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_RANDOM_ACCESS_INDICATOR) )->SetCurSel( 2 );
		return;
	}
}
*/
/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbElementaryStreamPriorityIndicator() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_ELEMENTARY_STREAM_PRIORITY_INDICATOR) )->SetCurSel( 2 );
		return;
	}
}
*/
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbPcrFlag() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_TSPACKET_PCR_FLAG) )->SetCurSel( 2 );
		return;
	}
}
/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbOpcrFlag() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_OPCR_FLAG) )->SetCurSel( 2 );
		return;
	}
}
*/
/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbSplicingPointFlag() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_SPLICING_POINT_FLAG) )->SetCurSel( 2 );
		return;
	}
}
*/
/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbTransportPrivateDataFlag() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_TRANSPORT_PRIVATE_DATA_FLAG) )->SetCurSel( 2 );
		return;
	}
}
*/
/*
void CDlg_TSAnalyzer_PacketTrigger::OnSelchangeCmbAdaptationFieldExtensionFlag() 
{
	// TODO: Add your control notification handler code here
	if( !VerifySearchConditions() ){
		( (CComboBox*)GetDlgItem( IDC_CMB_ADAPTATION_FIELD_EXTENSION_FLAG) )->SetCurSel( 2 );
		return;
	}
}
*/

int CDlg_TSAnalyzer_PacketTrigger::GetTriggerParams(uint8_t* pucMask, uint8_t* pucData, int length)
{
	int rtcode = -1;

	if ((pucMask != NULL) && (pucData != NULL))
	{
		if (length >= 6)
		{
			int nCurSel;
			int nHasAdaptation = 0;

			memset(pucData, 0x00, length);
			memset(pucMask, 0x00, length);

			pucData[0] = 0x47;
			pucMask[0] = 0xFF;

			CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_TRANSPORT_ERROR_INDICATOR);
			nCurSel = pCtrl->GetCurSel();
			switch (nCurSel)
			{
			case 0:
			case 1:
				pucData[1] |= (nCurSel << 7);
				pucMask[1] |= 0x80;
				break;
			default:
				break;
			}

			pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_PAYLOAD_UNIT_START_INDICATOR);
			nCurSel = pCtrl->GetCurSel();
			switch (nCurSel)
			{
			case 0:
			case 1:
				pucData[1] |= (nCurSel << 6);
				pucMask[1] |= 0x40;
				break;
			default:
				break;
			}

			pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_TRANSPORT_PRIORITY);
			nCurSel = pCtrl->GetCurSel();
			switch (nCurSel)
			{
			case 0:
			case 1:
				pucData[1] |= (nCurSel << 5);
				pucMask[1] |= 0x20;
				break;
			default:
				break;
			}

			pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_PID);
			nCurSel = pCtrl->GetCurSel();
			int nPID = (uint32_t)pCtrl->GetItemData(nCurSel);

			switch (nPID)
			{
			case EMPTY_PID:
				break;
			default:
				pucData[1] |= (nPID >> 8) & 0x1F;
				pucMask[1] |= 0x1F;

				pucData[2] |= nPID;
				pucMask[2] |= 0xFF;
				break;
			}

			pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_TRANSPORT_SCRAMBLING_CONTROL);
			nCurSel = pCtrl->GetCurSel();
			switch (nCurSel)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				pucData[3] |= (nCurSel << 6);
				pucMask[3] |= 0xC0;
				break;
			case 4:
			case 5:
				pucData[3] |= ((nCurSel - 4) << 7);
				pucMask[3] |= 0x80;
				break;
			case 6:
			case 7:
				pucData[3] |= ((nCurSel - 6) << 6);
				pucMask[3] |= 0x40;
				break;
			default:
				break;
			}

			pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_ADAPTATION_FIELD_CONTROL);
			nCurSel = pCtrl->GetCurSel();
			switch (nCurSel)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				pucData[3] |= (nCurSel << 4);
				pucMask[3] |= 0x30;
				break;
			case 4:
			case 5:
				pucData[3] |= ((nCurSel - 4) << 5);
				pucMask[3] |= 0x20;
				break;
			case 6:
			case 7:
				pucData[3] |= ((nCurSel - 6) << 4);
				pucMask[3] |= 0x10;
				break;
			default:
				break;
			}

			if (nCurSel == 2 || nCurSel == 3 || nCurSel == 5)
			{
				nHasAdaptation = 1;
			}

			pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_CONTINUITY_COUNTER);
			nCurSel = pCtrl->GetCurSel();
			switch (nCurSel)
			{
			case 16:
				break;
			default:
				pucData[3] |= nCurSel;
				pucMask[3] |= 0x0F;
				break;
			}

			if (nHasAdaptation)
			{
				/*
						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_DISCONTINUITY_INDICATOR );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= (nCurSel << 7);
							m_ucMask[5] |= 0x80;
							break;
						default:
							break;
						}

						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_RANDOM_ACCESS_INDICATOR );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= (nCurSel << 6);
							m_ucMask[5] |= 0x40;
							break;
						default:
							break;
						}

						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_ELEMENTARY_STREAM_PRIORITY_INDICATOR );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= (nCurSel << 5);
							m_ucMask[5] |= 0x20;
							break;
						default:
							break;
						}
				*/
				pCtrl = (CComboBox*)GetDlgItem(IDC_CMB_TSPACKET_PCR_FLAG);
				nCurSel = pCtrl->GetCurSel();
				switch (nCurSel)
				{
				case 0:
				case 1:
					pucData[5] |= (nCurSel << 4);
					pucMask[5] |= 0x10;
					break;
				default:
					break;
				}
				/*
						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_OPCR_FLAG );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= (nCurSel << 3);
							m_ucMask[5] |= 0x08;
							break;
						default:
							break;
						}

						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_SPLICING_POINT_FLAG );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= (nCurSel << 2);
							m_ucMask[5] |= 0x04;
							break;
						default:
							break;
						}

						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_TRANSPORT_PRIVATE_DATA_FLAG );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= (nCurSel << 1);
							m_ucMask[5] |= 0x02;
							break;
						default:
							break;
						}

						pCtrl = (CComboBox*)GetDlgItem( IDC_CMB_ADAPTATION_FIELD_EXTENSION_FLAG );
						nCurSel = pCtrl->GetCurSel();
						switch (nCurSel)
						{
						case 0:
						case 1:
							m_ucData[5] |= nCurSel;
							m_ucMask[5] |= 0x01;
							break;
						default:
							break;
						}
				*/
			}
		}
	}

	return rtcode;
}

void CDlg_TSAnalyzer_PacketTrigger::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CWnd*	pWnd;
	CRect	rect;
	int		nTopLine, nBottomLine;

	pWnd = GetDlgItem(IDC_STATIC_TS_CONDITION);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 10, 0);
	}

	pWnd = GetDlgItem(IDC_STATIC_TS_RESULT);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 10, cy - rect.top - 10, 0);

		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		CRect rectWndSplitter;
		rectWndSplitter.left = rect.left + 10;
		rectWndSplitter.top = rect.top + 20;
		rectWndSplitter.right = rect.right - 10;
		rectWndSplitter.bottom = rect.bottom - 50;

		nBottomLine = rect.bottom - 10;
		pWnd = GetDlgItem(IDC_BTN_TSPACKET_SEARCH);
		if (pWnd->GetSafeHwnd() != NULL)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			nTopLine = nBottomLine - rect.Height();
			pWnd->SetWindowPos(NULL, rect.left, nTopLine, rect.Width(), rect.Height(), 0);
		}

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_CLEAR);
		if (pWnd->GetSafeHwnd() != NULL)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			pWnd->SetWindowPos(NULL, rect.left, nTopLine, rect.Width(), rect.Height(), 0);
		}

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_PRE);
		if (pWnd->GetSafeHwnd() != NULL)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			pWnd->SetWindowPos(NULL, rect.left, nTopLine, rect.Width(), rect.Height(), 0);
		}

		pWnd = GetDlgItem(IDC_BTN_TSPACKET_NEXT);
		if (pWnd->GetSafeHwnd() != NULL)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			pWnd->SetWindowPos(NULL, rect.left, nTopLine, rect.Width(), rect.Height(), 0);

			nBottomLine = nTopLine - 5;
		}

		pWnd = GetDlgItem(IDC_SLIDER_TS_PACKET);
		if (pWnd->GetSafeHwnd() != NULL)
		{
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);

			nTopLine = nBottomLine - rect.Height();
			pWnd->SetWindowPos(NULL, rect.left, nTopLine, cx - rect.left - 15, rect.Height(), 0);

			nBottomLine = nTopLine - 5;

		}

		rectWndSplitter.bottom = nBottomLine;
		m_wndSplitter.SetWindowPos(NULL, rectWndSplitter.left, rectWndSplitter.top, rectWndSplitter.Width(), rectWndSplitter.Height(), 0);

		int nHexViewWidth = 740;
		if (rectWndSplitter.Width() < 1480)
		{
			nHexViewWidth = rectWndSplitter.Width() / 2;
		}
		m_wndSplitter.SetColumnInfo(0, rectWndSplitter.Width() - nHexViewWidth, 0);
		m_wndSplitter.SetColumnInfo(1, nHexViewWidth, 0);

		m_wndSplitter.RecalcLayout();
	}
}

BOOL CDlg_TSAnalyzer_PacketTrigger::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	NMHDR* nmhdr = (LPNMHDR)lParam;

	if (m_pTree != NULL)
	{
		if (nmhdr->hwndFrom == m_pTree->GetSafeHwnd())
		{
			if (nmhdr->code == TVN_SELCHANGED)
			{
				LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(nmhdr);

				TVITEM item = pNMTreeView->itemNew;
				HTREEITEM hSelItem = item.hItem;

				if (hSelItem != NULL)
				{
					if (item.mask & TVIF_PARAM)
					{
						DWORD uiValue = item.lParam;
						//char pszTemp[64];
						//sprintf_s(pszTemp, 64, "Parent: OnTvnSelchanged(), uiValue=0x%08X\0", uiValue);
						//AfxMessageBox(pszTemp);

						short offset = (short)((uiValue & 0xffff0000) >> 16);
						short length = (short)(uiValue & 0x0000ffff);

						if ((offset >= 0) && (length > 0))
						{
							m_pList->SetSel(offset, offset + length);
						}
						else
						{
							m_pList->SetSel(-1, -1);
						}
					}
				}

				return false;	////如果不返回，会将TVN_SELECTED消息反射回m_pTree控件，然后又弹射回来，如此反复，无限循环
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);	
}

