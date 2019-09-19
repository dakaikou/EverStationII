// ESMagicView.cpp : implementation file
//

#include "stdafx.h"
#include "..\EverStationII.h"
#include "..\MainFrm.h"
#include "ESMagicView.h"
#include "ESMagicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CESMagicView
#include "ESMagicFrm.h"
//#include "ESMagic_Analyze_Offline.h"

#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_packet.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"

#include "..\resource.h"

#define WND_WIDTH_ES_FRAME_NAVI			550
#define WND_WIDTH_ES_HEX_BUFFER			650

IMPLEMENT_DYNCREATE(CESMagicView, CFormView)

CESMagicView::CESMagicView()
	: CFormView(CESMagicView::IDD)
{
	//{{AFX_DATA_INIT(CESMagicView)
	//}}AFX_DATA_INIT

	m_bInitDone = 0;
	m_bOpened = 0;
}

CESMagicView::~CESMagicView()
{
}

void CESMagicView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CESMagicView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CESMagicView, CFormView)
	//{{AFX_MSG_MAP(CESMagicView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ESFILE_OPEN_CLOSE, OnBtnOpenOrClose)
	ON_BN_CLICKED(IDC_BTN_ESFILE_PREVIEW, OnBtnPreview)
	ON_BN_CLICKED(IDC_BTN_ESFILE_2TS, OnBtnES2TS)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_ESMAGIC_APPEND_LOG, OnESAppendLog)
	ON_MESSAGE(WM_ESMAGIC_REPORT_RATIO, OnESReportRatio)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SEQUENCE_HEADER, OnUpdateM2VEsSequenceHeader)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SEQUENCE_EXTENSION, OnUpdateM2VEsSequenceExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SEQUENCE_DISPLAY_EXTENSION, OnUpdateM2VEsSequenceDisplayExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SEQUENCE_SCALABLE_EXTENSION, OnUpdateM2VEsSequenceScalableExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_QUANT_MATRIX_EXTENSION, OnUpdateM2VEsQuantMatrixExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_GROUP_OF_PICTURES_HEADER, OnUpdateM2VEsGroupOfPicturesHeader)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_PICTURE_HEADER, OnUpdateM2VEsPictureHeader)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_PICTURE_CODING_EXTENSION, OnUpdateM2VEsPictureCodingExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_PICTURE_DISPLAY_EXTENSION, OnUpdateM2VEsPictureDisplayExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_PICTURE_TEMPORAL_SCALABLE_EXTENSION, OnUpdateM2VEsPictureTemporalScalableExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_PICTURE_SPATIAL_SCALABLE_EXTENSION, OnUpdateM2VEsPictureSpatialScalableExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_COPYRIGHT_EXTENSION, OnUpdateM2VEsCopyrightExtension)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SLICE, OnUpdateM2VEsSlice)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SEQUENCE_END, OnUpdateM2VEsSequenceEnd)
	//ON_MESSAGE(WM_UPDATE_M2V_GOP_TIME, OnUpdateM2VGOPTime)

	//ON_MESSAGE(WM_UPDATE_AVS_ES_SEQUENCE_HEADER, OnUpdateAVSEsVideoSequenceHeader)
	//ON_MESSAGE(WM_UPDATE_AVS_ES_SEQUENCE_DISPLAY_EXTENSION, OnUpdateAVSEsSequenceDisplayExtension)

	//ON_MESSAGE(WM_UPDATE_H264_ES_SEQ_PARAMETER_SET, OnUpdateH264EsSeqParameterSet)

	//ON_MESSAGE(WM_UPDATE_AC3_ES_SYNCINFO, OnUpdateAC3EsSyncInfo)

	//ON_MESSAGE(WM_UPDATE_M2A_ES_HEADER, OnUpdateM2AEsHeader)

	//ON_MESSAGE(WM_UPDATE_AAC_ES_HEADER, OnUpdateAACEsHeader)

	//ON_MESSAGE(WM_UPDATE_WAVE_HEADER, OnUpdateWAVEHeader)
	ON_MESSAGE(WM_ESMAGIC_ANALYZE_THREAD, OnESMagicAnalyzeThreadMsg)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CESMagicView diagnostics

#ifdef _DEBUG
void CESMagicView::AssertValid() const
{
	CFormView::AssertValid();
}

void CESMagicView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CESMagicView message handlers

BOOL CESMagicView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CESMagicView::AdjustLayout(int cx, int cy)
{
	CWnd*	pWnd;
	CRect	rect;

	//计算最下面一排控件的高度，以及对齐位置
	int nCtrlHeight = 30;
	pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetClientRect(&rect);
		nCtrlHeight = rect.Height();
	}

	int nYAlignOffset = cy - 10 - nCtrlHeight;

	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, nYAlignOffset - 10, 0);
	}

	pWnd = GetDlgItem(IDC_CMB_ESFILE_TYPE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, nYAlignOffset + 5, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_EDIT_ESFILE_PATH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, nYAlignOffset + 5, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, nYAlignOffset + 5, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ESFILE_PREVIEW);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, nYAlignOffset + 5, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ESFILE_2TS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, nYAlignOffset + 5, rect.Width(), rect.Height(), 0);
	}
}

void CESMagicView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ESFILE_TYPE);
	pCmbCtrl->ResetContent();

	int nItem = pCmbCtrl->AddString("MPEG视频");
	pCmbCtrl->SetItemData(nItem, STREAM_MPEGVES | STREAM_FILE);
	
	nItem = pCmbCtrl->AddString("AVS视频");
	pCmbCtrl->SetItemData(nItem, STREAM_AVSVES | STREAM_FILE);

	nItem = pCmbCtrl->AddString("H264视频");
	pCmbCtrl->SetItemData(nItem, STREAM_H264VES | STREAM_FILE);

	nItem = pCmbCtrl->AddString("WAVE音频");
	pCmbCtrl->SetItemData(nItem, STREAM_WAVEAES | STREAM_FILE);

	nItem = pCmbCtrl->AddString("MPEG音频");
	pCmbCtrl->SetItemData(nItem, STREAM_MPEGAES | STREAM_FILE);
	
	nItem = pCmbCtrl->AddString("AC3音频");
	pCmbCtrl->SetItemData(nItem, STREAM_AC3AES | STREAM_FILE);

	nItem = pCmbCtrl->AddString("AAC音频");
	pCmbCtrl->SetItemData(nItem, STREAM_AACAES | STREAM_FILE);

	pCmbCtrl->SetCurSel(4);

	CRect	rectClient;
	GetClientRect(&rectClient);

	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		//计算最下面一排控件的高度，以及对齐位置
		CRect rectBtn;
		int nCtrlHeight = 30;
		CWnd* pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
		if (pWnd->GetSafeHwnd() != NULL)
		{
			pWnd->GetClientRect(&rectBtn);
			nCtrlHeight = rectBtn.Height();
		}

		int nYAlignOffset = rectClient.bottom - 10 - nCtrlHeight;

		CRect rectSplitterWnd;

		rectSplitterWnd.left = rectClient.left + 5;
		rectSplitterWnd.right = rectClient.right - 5;
		rectSplitterWnd.top = rectClient.top + 5;
		rectSplitterWnd.bottom = nYAlignOffset - 5;

		int middle_col_width = rectSplitterWnd.Width() - WND_WIDTH_ES_FRAME_NAVI - WND_WIDTH_ES_HEX_BUFFER;
		if (middle_col_width < 100) middle_col_width = 100;
		m_wndSplitter.CreateStatic(this, 1, 3);
		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CNaviList_ESFrames), CSize(WND_WIDTH_ES_FRAME_NAVI, 0), NULL);
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CTreeView_XMLBrowser), CSize(middle_col_width, 0), NULL);
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
		m_wndSplitter.CreateView(0, 2, RUNTIME_CLASS(CListView_ByteBuffer), CSize(WND_WIDTH_ES_HEX_BUFFER, 0), NULL);
#else
		m_wndSplitter.CreateView(0, 2, RUNTIME_CLASS(CHexEditView_ByteBuffer), CSize(WND_WIDTH_ES_HEX_BUFFER, 0), NULL);
#endif

		m_wndSplitter.MoveWindow(&rectSplitterWnd);

		m_pNaviPane = (CNaviList_ESFrames*)m_wndSplitter.GetPane(0, 0);
		//m_pNaviPane->Set(this->GetSafeHwnd());
		m_pNaviPane->Set();

		m_pSyntaxTree = (CTreeView_XMLBrowser*)m_wndSplitter.GetPane(0, 1);
		m_pSyntaxTree->Init("ES语法分析");
		m_pSyntaxTree->m_hNotifyParent = GetSafeHwnd();
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
		m_pHexList = (CListView_ByteBuffer*)m_wndSplitter.GetPane(0, 2);
#else
		m_pHexList = (CHexEditView_ByteBuffer*)m_wndSplitter.GetPane(0, 2);
		m_pHexList->SetBPR(16);
#endif
	}

	m_dlgVideo.Create(IDD_SHOW_VIDEO_SCREEN, this);
	m_dlgVideo.ShowWindow(SW_HIDE);

	m_dlgESAnalyzeProgress.Create(IDD_ANALYSE_PROGRESS, this);
	//m_dlgESAnalyzeProgress.SetWindowText(_T("ES分析"));
	m_dlgESAnalyzeProgress.ShowWindow(SW_HIDE);

	Reset();

	AdjustLayout(rectClient.Width(), rectClient.Height());

	m_bInitDone = 1;
}

void CESMagicView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
	{
		return;		// arrangement not needed
	}

	if (m_bInitDone)
	{
		AdjustLayout(cx, cy);
	}
}

void CESMagicView::Reset(void)
{
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_CMB_ESFILE_TYPE);			
	pWnd->EnableWindow(TRUE);

	pWnd = GetDlgItem(IDC_EDIT_ESFILE_PATH);			
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
	pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_BTN_ESFILE_PREVIEW);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_ESFILE_2TS);
	pWnd->EnableWindow(FALSE);

	m_dlgESAnalyzeProgress.Reset();
	m_dlgESAnalyzeProgress.ShowWindow(SW_HIDE);

	GetStatusBar().SetPaneText(2, "");
	GetStatusBar().SetPaneProgress(2, 0);

	UpdateData(FALSE);
}

CESMagicView* CESMagicView::GetView(void)
{
	CESMagicView* pView = NULL;

	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CMultiDocTemplate* pDocTemplate = pApp->m_pDocTemplateForES;

	POSITION posDoc = pDocTemplate->GetFirstDocPosition();
	while (posDoc != NULL)
	{
		CESMagicDoc* pDoc = (CESMagicDoc*)pDocTemplate->GetNextDoc(posDoc);
		POSITION posView = pDoc->GetFirstViewPosition();
		while (posView != NULL)
		{
			pView = (CESMagicView*)pDoc->GetNextView(posView);
			break;
		}
	}

	return pView;
}

void CESMagicView::OnBtnOpenOrClose() 
{
	// TODO: Add your control notification handler code here
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ESFILE_TYPE);
	CWnd*	pWnd;
	int		nStreamType = STREAM_UNKNOWN;
	int nSel = pCmbCtrl->GetCurSel();

	if (nSel != CB_ERR)
	{
		nStreamType = (uint32_t)pCmbCtrl->GetItemData(nSel);
	}

	if (m_bOpened == 0)
	{
		CFileDialog fileDlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "media files(*.*)|*.*|", NULL);

		CString strFileName = _T("");

		if (fileDlg.DoModal() == IDOK)
		{
			strFileName = fileDlg.GetPathName();

			if (FILE_CheckStreamType(nStreamType & (~STREAM_FILE), strFileName.GetBuffer(128)))
			{
				pWnd = GetDlgItem(IDC_EDIT_ESFILE_PATH);
				pWnd->SetWindowText(strFileName);

				pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
				pWnd->EnableWindow(FALSE);			//禁止“打开”按钮再进行操作

				//switch (nStreamType & (~STREAM_FILE))
				//{
				//case STREAM_MPEGVES:
				//	m_MPGVDecoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//case STREAM_H264VES:
				//	m_H264Decoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//case STREAM_AVSVES:
				//	m_AVSDecoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//case STREAM_MPEGAES:
				//	m_MPGADecoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//case STREAM_AC3AES:
				//	m_AC3Decoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//case STREAM_AACAES:
				//	m_AACDecoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//case STREAM_WAVEAES:
				//	m_WAVEDecoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(128));
				//	break;
				//}

				strcpy_s(m_es_thread_params.pszFileName, sizeof(m_es_thread_params.pszFileName), strFileName.GetBuffer(128));

				m_es_thread_params.offline = 1;

				////					m_kThreadParams.main_thread_running = 0;		//若线程启动，会将此值修改为1，可以据此判断主线程是否启动
				m_es_thread_params.main_thread_stopped = 0;		//若线程退出，会将此值修改为1，可以据此判断主线程是否启动

				//m_kThreadParams.ts_trigger_thread_running = 0;
				////					m_kThreadParams.ts_trigger_thread_stopped = 0;

				//m_kThreadParams.es_trigger_thread_running = 0;
				//m_kThreadParams.es_trigger_thread_stopped = 0;

				//m_kThreadParams.section_trigger_thread_running = 0;
				////					m_kThreadParams.section_trigger_thread_stopped = 0;

				//m_kThreadParams.find_signal = 0;

				//m_kThreadParams.packet_decimate_thread_running = 0;
				//m_kThreadParams.packet_decimate_thread_stopped = 0;

				//m_kThreadParams.dsmcc_download_thread_running = 0;
				//m_kThreadParams.dsmcc_download_thread_stopped = 0;

				//m_kThreadParams.monitor_thread_running = 0;
				//m_kThreadParams.monitor_thread_stopped = 0;

				m_es_thread_params.hCallerWnd = this->GetSafeHwnd();

				//m_kThreadParams.pTrigger_PESPacket = &m_Trigger_PESPacket;

				::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ESMagic_analyze_thread, (LPVOID)&m_es_thread_params, 0, 0);
			}
			else
			{
//				TSMagic_Debug(DEBUG_ERROR, "ERROR: 文件类型不正确!");

				AfxMessageBox("ERROR: 文件类型不正确!");
			}
		}
	}
	else
	{
		m_es_thread_params.main_thread_running = 0;			//强制通知主线程退出

		pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
		pWnd->EnableWindow(FALSE);			//禁止“打开”按钮再进行操作

		if (m_es_thread_params.main_thread_stopped == 1)
		{
			//若离线分析线程已经退出，此时需要补发一个消息，否则界面控件显示会不正常
			::SendMessage(this->GetSafeHwnd(), WM_ESMAGIC_ANALYZE_THREAD, 0, NULL);
		}

		//switch (nStreamType & (~STREAM_FILE))
		//{
		//case STREAM_MPEGVES:
		//	m_MPGVDecoder.Close();
		//	break;
		//case STREAM_H264VES:
		//	m_H264Decoder.Close();
		//	break;
		//case STREAM_AVSVES:
		//	m_AVSDecoder.Close();
		//	break;
		//case STREAM_MPEGAES:
		//	m_MPGADecoder.Close();
		//	break;
		//case STREAM_AC3AES:
		//	m_AC3Decoder.Close();
		//	break;
		//case STREAM_AACAES:
		//	m_AACDecoder.Close();
		//	break;
		//case STREAM_WAVEAES:
		//	m_WAVEDecoder.Close();
		//	break;
		//}
	}
}

void CESMagicView::OnBtnPreview() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strFileName;
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT_ESFILE_PATH);
	pWnd->GetWindowText(strFileName);

	if (strFileName != "")
	{
		CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ESFILE_TYPE);
		int nSel = pCmbCtrl->GetCurSel();
		int nStreamType = (int)pCmbCtrl->GetItemData(nSel);

		//switch (nStreamType & (~STREAM_FILE))
		//{
		//case STREAM_MPEGVES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_MPGVDecoder, 0);
		//	break;
		//case STREAM_H264VES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_H264Decoder, 0);
		//	break;
		//case STREAM_AVSVES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_AVSDecoder, 0);
		//	break;
		//case STREAM_MPEGAES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_MPGADecoder, 1);
		//	break;
		//case STREAM_AC3AES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_AC3Decoder, 1);
		//	break;
		//case STREAM_AACAES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_AACDecoder, 1);
		//	break;
		//case STREAM_WAVEAES:
		//	m_dlgVideo.OfflineStream(nStreamType, &m_WAVEDecoder, 1);
		//	break;
		//}
	}
}

void CESMagicView::OnBtnES2TS(void) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strFileName;
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT_ESFILE_PATH);
	pWnd->GetWindowText(strFileName);

//	strFileName = "e:\\blueangel.ac3";

	if (strFileName != "")
	{
		FILE* fp_src = NULL;
		fopen_s(&fp_src, strFileName.GetBuffer(128), "rb");
		if (fp_src != NULL)
		{
			int		filepos;

			filepos = ftell(fp_src);
			fseek(fp_src, 0, SEEK_SET);

			FILE* fp_dst = NULL;
			fopen_s(&fp_dst, "e:\\exam.ts", "wb");
			if (fp_dst != NULL)
			{
				transport_packet_t	TS_packet;
				unsigned char		payload_buf[184];
				unsigned char		ts_packet_buf[188];
				int					pklength = 188;
				size_t				rdlen = 0;

				m_dlgESAnalyzeProgress.SetWindowText("ES2TS打包进度条");
				m_dlgESAnalyzeProgress.ShowWindow(SW_SHOW);

				TS_packet.sync_byte = 0x47;
				TS_packet.PID = 0x1ff0;
				TS_packet.transport_priority = 0;
				TS_packet.payload_unit_start_indicator = 0;
				TS_packet.transport_error_indicator = 0;

				TS_packet.continuity_counter = 0;
				TS_packet.adaptation_field_control = 1;
				TS_packet.transport_scrambling_control = 0;

				TS_packet.payload_buf = payload_buf;

				rdlen = fread(TS_packet.payload_buf, sizeof(char), 184, fp_src);
				TS_packet.payload_length = (uint8_t)rdlen;

				if (MPEG_encode_TS_packet(ts_packet_buf, pklength, &TS_packet) == 0)
				{
					fwrite(ts_packet_buf, pklength, 1, fp_dst);
				}

				while (rdlen >= 184)
				{
					TS_packet.payload_unit_start_indicator = 0;

					TS_packet.continuity_counter++;
					TS_packet.continuity_counter %= 16;

					rdlen = fread(TS_packet.payload_buf, sizeof(char), 184, fp_src);
					TS_packet.payload_length = (uint8_t)rdlen;

					if (MPEG_encode_TS_packet(ts_packet_buf, pklength, &TS_packet) == 0)
					{
						fwrite(ts_packet_buf, pklength, 1, fp_dst);
					}
				}

				m_dlgESAnalyzeProgress.ShowWindow(SW_HIDE);

				fclose(fp_dst);
			}

			fseek(fp_src, filepos, SEEK_SET);
			fclose(fp_src);
		}
	}
}

LRESULT CESMagicView::OnUpdateM2VPes(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsSequenceHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//MPGV_sequence_header_t*		    psequence_header = (MPGV_sequence_header_t*)wParam;
	//
	//if (m_hRootItem != NULL)
	//{
	//	TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "MPEG视频语法分析");

	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "sequence_header()", -1, 0, NULL);
	//	//MPEG_VIDEO_ES_SEQUENCE_HEADER(&m_treeESSyntax, hItem, psequence_header);
	//	m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsSequenceExtension(WPARAM wParam, LPARAM lParam)
{
	//MPGV_sequence_extension_t*	psequence_extension = (MPGV_sequence_extension_t*)wParam;
	//TREEITEM					hItem;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "sequence_extension()", -1, 0, NULL);
	//	//MPEG_VIDEO_ES_SEQUENCE_EXTENSION(&m_treeESSyntax, hItem, psequence_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsSequenceDisplayExtension(WPARAM wParam, LPARAM lParam)
{
	//MPGV_sequence_display_extension_t*	psequence_display_extension = (MPGV_sequence_display_extension_t*)wParam;
	//TREEITEM							hItem;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "sequence_display_extension()", -1, 0, NULL);
	//	//MPEG_VIDEO_ES_SEQUENCE_DISPLAY_EXTENSION(&m_treeESSyntax, hItem, psequence_display_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsSequenceScalableExtension(WPARAM wParam, LPARAM lParam)
{
	//MPGV_sequence_scalable_extension_t*	psequence_scalable_extension = (MPGV_sequence_scalable_extension_t*)wParam;
	//TREEITEM							hItem;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "sequence_scalable_extension()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_SEQUENCE_SCALABLE_EXTENSION(&m_treeESSyntax, hItem, psequence_scalable_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsQuantMatrixExtension(WPARAM wParam, LPARAM lParam)
{
	//MPGV_quant_matrix_extension_t*	pquant_matrix_extension = (MPGV_quant_matrix_extension_t*)wParam;
	//TREEITEM						hItem;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "quant_matrix_extension()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_QUANT_MATRIX_EXTENSION(&m_treeESSyntax, hItem, pquant_matrix_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsGroupOfPicturesHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM								hItem;
	//MPGV_group_of_pictures_header_t*		pgroup_of_pictures_header = (MPGV_group_of_pictures_header_t*)wParam;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "group_of_pictures_header()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_GROUP_OF_PICTURES_HEADER(&m_treeESSyntax, hItem, pgroup_of_pictures_header);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsPictureHeader(WPARAM wParam, LPARAM lParam)
{
//	char										pszText[256];
//	TREEITEM									hItem;
//	MPGV_picture_header_t*						ppicture_header = (MPGV_picture_header_t*)wParam;
//
//	if (m_hRootItem != NULL)
//	{
//		switch (ppicture_header->picture_coding_type)
//		{
//		case MPEG_I_FRAME:
//			sprintf_s(pszText, sizeof(pszText), "picture_header()--[I]");
//			break;
//		case MPEG_P_FRAME:
//			sprintf_s(pszText, sizeof(pszText), "picture_header()--[P]");
//			break;
//		case MPEG_B_FRAME:
//			sprintf_s(pszText, sizeof(pszText), "picture_header()--[B]");
//			break;
//		default:
//			break;
//		}
//
//		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, pszText, -1, 0, NULL);
//
//		//MPEG_VIDEO_ES_PICTURE_HEADER(&m_treeESSyntax, hItem, ppicture_header);
//	}
//	return 0;
//}
//
//LRESULT CESMagicView::OnUpdateM2VEsPictureCodingExtension(WPARAM wParam, LPARAM lParam)
//{
//	TREEITEM									hItem;
//	MPGV_picture_coding_extension_t*			ppicture_coding_extension = (MPGV_picture_coding_extension_t*)wParam;
//
//	if (m_hRootItem != NULL)
//	{
////		hItem = m_treeESSyntax.InsertItem("picture_display_extension()", m_hRootItem);
//		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "picture_coding_extension()", -1, 0, NULL);
//
//		//MPEG_VIDEO_ES_PICTURE_CODING_EXTENSION(&m_treeESSyntax, hItem, ppicture_coding_extension);
//	}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsPictureDisplayExtension(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM									hItem;
	//MPGV_picture_display_extension_t*			ppicture_display_extension = (MPGV_picture_display_extension_t*)wParam;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "picture_display_extension()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_PICTURE_DISPLAY_EXTENSION(&m_treeESSyntax, hItem, ppicture_display_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsPictureTemporalScalableExtension(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM									hItem;
	//MPGV_picture_temporal_scalable_extension_t*	ppicture_temporal_scalable_extension = (MPGV_picture_temporal_scalable_extension_t*)wParam;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "picture_temporal_scalable_extension()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_PICTURE_TEMPORAL_SCALABLE_EXTENSION(&m_treeESSyntax, hItem, ppicture_temporal_scalable_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsPictureSpatialScalableExtension(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM									hItem;
	//MPGV_picture_spatial_scalable_extension_t*	ppicture_spatial_scalable_extension = (MPGV_picture_spatial_scalable_extension_t*)wParam;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "picture_spatial_scalable_extension()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_PICTURE_SPATIAL_SCALABLE_EXTENSION(&m_treeESSyntax, hItem, ppicture_spatial_scalable_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsCopyrightExtension(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM							hItem;
	//MPGV_copyright_extension_t*			pcopyright_extension = (MPGV_copyright_extension_t*)wParam;

	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "copyright_extension()", -1, 0, NULL);

	//	//MPEG_VIDEO_ES_COPYRIGHT_EXTENSION(&m_treeESSyntax, hItem, pcopyright_extension);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsSlice(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VEsSequenceEnd(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CESMagicView::OnUpdateM2VGOPTime(WPARAM wParam, LPARAM lParam)
{
/*
	char										pszText[256];
	CMPEG_VideoDecoder*							pVideoDecoder;
	MPEG2_video_group_of_pictures_header		group_of_pictures_header;

	if (M2V_REPORT.find_mpeg_video)
	{
		pVideoDecoder = &(M2V_REPORT.M2VDecoder);
		pVideoDecoder->Get_group_of_pictures_header(&group_of_pictures_header);

		sprintf_s(pszText, sizeof(pszText), "%02d:%02d:%02d", group_of_pictures_header.time_code_hours,
										   group_of_pictures_header.time_code_minutes,
										   group_of_pictures_header.time_code_seconds);

		CWnd* pWnd = GetDlgItem(IDC_EDIT_GOP_TIME);
		pWnd->SetWindowText(pszText);

		UpdateData(FALSE);
	}
*/
	return 0;
}

LRESULT CESMagicView::OnUpdateAVSEsVideoSequenceHeader(WPARAM wParam, LPARAM lParam)
{
//	TREEITEM						hItem;
//	AVS_video_sequence_header_t*    pvideo_sequence_header = (AVS_video_sequence_header_t*)wParam;
//	
//	if (m_hRootItem != NULL)
//	{
//		TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "AVS视频语法分析");
//
//		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "video_sequence_header()", -1, 0, NULL);
////		AVS_VIDEO_ES_VIDEO_SEQUENCE_HEADER(&m_treeESSyntax, hItem, pvideo_sequence_header);
//		m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
//	}
	return 0;
}

LRESULT CESMagicView::OnUpdateAVSEsSequenceDisplayExtension(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CESMagicView::OnUpdateH264EsSeqParameterSet(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//H264_seq_parameter_set_t*	    pseq_parameter_set = (H264_seq_parameter_set_t*)wParam;
	//
	//if (m_hRootItem != NULL)
	//{
	//	TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "H264视频语法分析");

	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "seq_parameter_set()", -1, 0, NULL);
	//	//H264_VIDEO_SEQ_PARAMETER_SET(&m_treeESSyntax, hItem, pseq_parameter_set);
	//	m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateAC3EsSyncInfo(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//AC3_syncinfo_t*				    psyncinfo = (AC3_syncinfo_t*)wParam;
	//
	//if (m_hRootItem != NULL)
	//{
	//	TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "AC3音频语法分析");

	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "syncinfo()", -1, 0, NULL);
	//	//AC3_AUDIO_ES_SYNCINFO(&m_treeESSyntax, hItem, psyncinfo);
	//	m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//MPA_header_t*				    pheader = (MPA_header_t*)wParam;
	//
	//if (m_hRootItem != NULL)
	//{
	//	TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "MPEG音频语法分析");

	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "header()", -1, 0, NULL);
	//	//MPEG_AUDIO_ES_HEADER(&m_treeESSyntax, hItem, pheader);
	//	m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateAACEsHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//AAC_adts_fixed_header_t*		padts_fixed_header = (AAC_adts_fixed_header_t*)wParam;

	//if (m_hRootItem != NULL)
	//{
	//	TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "AAC音频语法分析");

	//	hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "header()", -1, 0, NULL);
	//	//AAC_AUDIO_ES_ADTS_FIXED_HEADER(&m_treeESSyntax, hItem, padts_fixed_header);

	//	m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
	//}
	return 0;
}

LRESULT CESMagicView::OnUpdateWAVEHeader(WPARAM wParam, LPARAM lParam)
{
//	TREEITEM						hItem;
//	formatchunk_t*					pwave_header = (formatchunk_t*)wParam;
//
//	if (m_hRootItem != NULL)
//	{
//		TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "WAVE音频语法分析");
//
//		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "header()", -1, 0, NULL);
////		AAC_AUDIO_ES_ADTS_FIXED_HEADER(&m_treeESSyntax, hItem, padts_fixed_header);
//
//		m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
//	}
	return 0;
}

CMFCStatusBar& CESMagicView::GetStatusBar() const
{
	return ((CMainFrame*)AfxGetMainWnd())->GetStatusBar();
}

LRESULT CESMagicView::OnESAppendLog(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_DockPane_RunningLog.AppendLogForUserAction((int)lParam, (char*)wParam);

	return NULL;
}

LRESULT CESMagicView::OnESReportRatio(WPARAM wParam, LPARAM lParam)
{
	GetStatusBar().SetPaneProgress(1, (int)lParam);

	return NULL;
}

LRESULT CESMagicView::OnESMagicAnalyzeThreadMsg(WPARAM wParam, LPARAM lParam)
{
	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CWnd* pWnd = NULL;
	//char			 pszDebug[256];

	if (wParam == 1)		//线程进入
	{
		pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("停止");
			pWnd->EnableWindow(TRUE);
		}
		m_bOpened = 1;

		//pWnd = GetDlgItem(IDC_EDIT_ESFILE);
		//pWnd->SetWindowText(strFileName);

		CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ESFILE_TYPE);
		pCmbCtrl->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_ESFILE_PREVIEW);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_ESFILE_2TS);
		pWnd->EnableWindow(FALSE);

		GetStatusBar().SetPaneText(1, m_es_thread_params.pszFileName);

		pApp->SetRunningState(1);

		//if (!m_dlgESAnalyzeProgress.IsWindowVisible())
		//{
		//	strcpy_s(pszDebug, sizeof(pszDebug), "ES分析");
		//	m_dlgESAnalyzeProgress.SetWindowText(pszDebug);
		//	m_dlgESAnalyzeProgress.ShowWindow(SW_SHOW);
		//	m_dlgESAnalyzeProgress.m_progress.SetPos((int)lParam);				//lParam是分析的百分比
		//}
	}
	else if (wParam == 2)		//离线分析结束
	{
		pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("关闭");
			pWnd->EnableWindow(TRUE);
		}

		pWnd = GetDlgItem(IDC_BTN_ESFILE_PREVIEW);
		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_BTN_ESFILE_2TS);
		pWnd->EnableWindow(TRUE);
	}
	else					//线程退出
	{
		pWnd = GetDlgItem(IDC_BTN_ESFILE_OPEN_CLOSE);
		if (pWnd != NULL)
		{
			pWnd->SetWindowText("打开");
			pWnd->EnableWindow(TRUE);
		}

		pApp->SetRunningState(0);

		m_bOpened = 0;

		Reset();


		//if (m_dlgESAnalyzeProgress.IsWindowVisible())
		//{
		//	m_dlgESAnalyzeProgress.ShowWindow(SW_HIDE);
		//	m_dlgESAnalyzeProgress.Reset();
		//}

		//如果抽选线程在主线程之后退出，必须增加下面的语句，否则界面显示不正确
		//if (m_kThreadParams.main_thread_running == 0)
		//{
		//	::SendMessage(this->GetSafeHwnd(), WM_TSMAGIC_OFFLINE_THREAD, 0, NULL);
		//}
	}

	return 0;
}
