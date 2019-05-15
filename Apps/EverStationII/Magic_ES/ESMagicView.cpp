// ESMagicView.cpp : implementation file
//

#include "stdafx.h"
#include "..\EverStationII.h"
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
#include "../Magic_TS/TSMagic_GuiApi_MSG.h"

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"

//#include "..\Common\GuiCommon_TreeFunction.h"
//#include "..\Common\GuiCommon_AVSVideoSyntax.h"
//#include "..\Common\GuiCommon_H264VideoSyntax.h"
//#include "..\Common\GuiCommon_MpegVideoSyntax.h"
//#include "..\Common\GuiCommon_MpegAudioSyntax.h"
//#include "..\Common\GuiCommon_AACAudioSyntax.h"
//#include "..\Common\GuiCommon_AC3AudioSyntax.h"

#include "..\resource.h"

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
	//DDX_Control(pDX, IDC_TREE_ES_SYNTAX, m_treeESSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CESMagicView, CFormView)
	//{{AFX_MSG_MAP(CESMagicView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpenOrClose)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, OnBtnPreview)
	ON_BN_CLICKED(IDC_BTN_ES2TS, OnBtnES2TS)
	//}}AFX_MSG_MAP

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

	ON_MESSAGE(WM_UPDATE_WAVE_HEADER, OnUpdateWAVEHeader)

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
	int		xoffset;
	int		yoffset;
	int		width;
	int		height;

	yoffset = 0;

	pWnd = GetDlgItem(IDC_TREE_ES_SYNTAX);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		height = cy - 35;
		pWnd->SetWindowPos(NULL, 5, 5, cx - 10, height - 10, 0);

		yoffset = height + 5; 
/*
		CRect		rectTab;
		CRect		rectItem;

		((CTabCtrl*)pWnd)->GetItemRect(0, &rectItem);

		pWnd->GetClientRect(&rectTab);
		rectTab.top += rectItem.Height() + 5;
		rectTab.bottom -= 1;
		rectTab.left += 1;
		rectTab.right -= 1;

#if GUI_ES_OVERVIEW
		m_dlgTSAnalyzerOverview.MoveWindow(&rectTab);
#endif
*/
	}

	pWnd = GetDlgItem(IDC_CMB_ES_TYPE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		width = rect.Width();
		height = rect.Height();
		pWnd->SetWindowPos(NULL, 5, yoffset + 5, width, height, 0);

		xoffset = width + 10;
	}

	pWnd = GetDlgItem(IDC_EDIT_FILE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		width = rect.Width();
		height = rect.Height();
		pWnd->SetWindowPos(NULL, xoffset + 5, yoffset + 5, width, height, 0);

		xoffset += width + 10;
	}

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		width = rect.Width();
		height = rect.Height();
		pWnd->SetWindowPos(NULL, xoffset + 5, yoffset + 5, width, height, 0);

		xoffset += width + 10;
	}

	pWnd = GetDlgItem(IDC_BTN_PREVIEW);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		width = rect.Width();
		height = rect.Height();
		pWnd->SetWindowPos(NULL, xoffset + 5, yoffset + 5, width, height, 0);

		xoffset += width + 10;
	}

	pWnd = GetDlgItem(IDC_BTN_ES2TS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		width = rect.Width();
		height = rect.Height();
		pWnd->SetWindowPos(NULL, xoffset + 5, yoffset + 5, width, height, 0);

		xoffset += width + 10;
	}
}

void CESMagicView::InitTree(void)
{
	//TreeFun_InitTree(&m_treeESSyntax, this, IDC_TREE_ES_SYNTAX);

	//m_hRootItem = TreeFun_InsertItem(&m_treeESSyntax, TREE_ROOT, "ES语法分析", -1, 0, NULL);
}

void CESMagicView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class

	CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ES_TYPE);

	int nItem;

	pCmbCtrl->ResetContent();

	nItem = pCmbCtrl->AddString("MPEG视频");
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

	m_bInitDone = 1;

	CRect	rect;
	GetClientRect(&rect);
	AdjustLayout(rect.Width(), rect.Height());

	InitTree();

	m_dlgVideo.Create(IDD_SHOW_VIDEO_SCREEN, this);
	m_dlgVideo.ShowWindow(SW_HIDE);

	m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	m_dlgProgress.ShowWindow(SW_HIDE);

	Reset();
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

	pWnd = GetDlgItem(IDC_EDIT_FILE);			//说明窗口已经撤销
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_BTN_PREVIEW);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_ES2TS);
	pWnd->EnableWindow(FALSE);

	//TreeFun_DeleteChildItems(&m_treeESSyntax, m_hRootItem);

	//TreeFun_SetItemText(&m_treeESSyntax, m_hRootItem, "ES语法分析");

	m_dlgProgress.ShowWindow(SW_HIDE);

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
	CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ES_TYPE);
	CWnd*	pWnd;
	int		nStreamType = STREAM_UNKNOWN;
	int nSel = pCmbCtrl->GetCurSel();

	if (nSel != CB_ERR)
	{
		nStreamType = (uint32_t)pCmbCtrl->GetItemData(nSel);
	}

	if (m_bOpened == 0)
	{
		CFileDialog fileDlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "mpeg video files(*.*)|*.*|", NULL);

		CString strFileName = _T("");

		if (fileDlg.DoModal() == IDOK)
		{
			strFileName = fileDlg.GetPathName();

			if (FILE_CheckStreamType(nStreamType & (~STREAM_FILE), strFileName.GetBuffer(128)))
			{
				pWnd = GetDlgItem(IDC_BTN_OPEN);
				pWnd->SetWindowText("关闭");
				m_bOpened = 1;

				pWnd = GetDlgItem(IDC_EDIT_FILE);
				pWnd->SetWindowText(strFileName);

				pCmbCtrl->EnableWindow(FALSE);

				pWnd = GetDlgItem(IDC_BTN_PREVIEW);
				pWnd->EnableWindow(TRUE);

				pWnd = GetDlgItem(IDC_BTN_ES2TS);
				pWnd->EnableWindow(TRUE);

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
				//strcpy_s(m_kThreadParams.pszFileName, sizeof(m_kThreadParams.pszFileName), pszURL);

				//m_kThreadParams.offline = 1;

				////					m_kThreadParams.main_thread_running = 0;		//若线程启动，会将此值修改为1，可以据此判断主线程是否启动
				//m_kThreadParams.main_thread_stopped = 0;		//若线程退出，会将此值修改为1，可以据此判断主线程是否启动

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

				//m_kThreadParams.hMainWnd = this->GetSafeHwnd();

				//m_kThreadParams.pTrigger_PESPacket = &m_Trigger_PESPacket;

				//::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_offline_thread, (LPVOID)&m_kThreadParams, 0, 0);

				UpdateData(FALSE);
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

		pCmbCtrl->EnableWindow(TRUE);

		m_bOpened = 0;

		Reset();
	}
}

void CESMagicView::OnBtnPreview() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strFileName;
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT_FILE);
	pWnd->GetWindowText(strFileName);

	if (strFileName != "")
	{
		CComboBox* pCmbCtrl = (CComboBox*)GetDlgItem(IDC_CMB_ES_TYPE);
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
	
	CWnd* pWnd = GetDlgItem(IDC_EDIT_FILE);
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

				m_dlgProgress.SetWindowText("ES2TS打包进度条");
				m_dlgProgress.ShowWindow(SW_SHOW);

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

				m_dlgProgress.ShowWindow(SW_HIDE);

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
	char										pszText[MAX_TXT_CHARS];
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
