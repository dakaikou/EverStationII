// PSMagicView.cpp : implementation file
//

#include "stdafx.h"
#include "..\EverStationII.h"
#include "PSMagicView.h"
#include "PSMagicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSMagicView
//#include "..\main\define.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "PSMagicFrm.h"

#include "..\resource.h"

#include "..\Common\Dlg_Decimate.h"
//#include "..\Common\GuiCommon_TreeFunction.h"
//#include "..\Common\GuiCommon_MpegPSSyntax.h"
//#include "..\Common\GuiCommon_MpegVideoSyntax.h"
//#include "..\Common\GuiCommon_MpegAudioSyntax.h"
//#include "..\Common\GuiCommon_AC3AudioSyntax.h"

IMPLEMENT_DYNCREATE(CPSMagicView, CFormView)

CPSMagicView::CPSMagicView()
	: CFormView(CPSMagicView::IDD)
{
	//{{AFX_DATA_INIT(CPSMagicView)
	//}}AFX_DATA_INIT

	m_bInitDone = 0;
}

CPSMagicView::~CPSMagicView()
{
}

void CPSMagicView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSMagicView)
	//DDX_Control(pDX, IDC_TREE_PS_SYNTAX, m_treePSSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSMagicView, CFormView)
	//{{AFX_MSG_MAP(CPSMagicView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpenOrClose)
	ON_BN_CLICKED(IDC_BTN_DISPATCH, OnBtnEsDispatch)
	ON_BN_CLICKED(IDC_BTN_PS_PREVIEW, OnBtnPsPreview)
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_UPDATE_MPEG_PACK_HEADER, OnUpdateMPEGPackHeader)
	//ON_MESSAGE(WM_UPDATE_MPEG_SYSTEM_HEADER, OnUpdateMPEGSystemHeader)
	//ON_MESSAGE(WM_UPDATE_M2V_ES_SEQUENCE_HEADER, OnUpdateM2VEsSequenceHeader)
	//ON_MESSAGE(WM_UPDATE_M2A_ES_HEADER, OnUpdateM2AEsHeader)
	//ON_MESSAGE(WM_UPDATE_AC3_ES_SYNCINFO, OnUpdateAC3EsSyncInfo)
//	ON_MESSAGE(WM_TSMAGIC_OFFLINE_THREAD, OnTSMagicOfflineThreadMsg)
//	ON_MESSAGE(WM_TSMAGIC_REALTIME_THREAD, OnTSMagicRealtimeThreadMsg)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSMagicView diagnostics

#ifdef _DEBUG
void CPSMagicView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPSMagicView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPSMagicView message handlers

BOOL CPSMagicView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CPSMagicView::ReLayoutWindow(int cx, int cy)
{
	CWnd*	pWnd;
	CRect	rect;
	int		xoffset;
	int		yoffset;

	yoffset = 0;

	pWnd = GetDlgItem(IDC_TREE_PS_SYNTAX);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->SetWindowPos(NULL, 5, 5, cx - 130, cy - 50, 0);

		yoffset = cy - 40;
	}

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		pWnd->SetWindowPos(NULL, 5, yoffset, rect.Width(), rect.Height(), 0);

		xoffset = 10 + rect.Width();
	}

	pWnd = GetDlgItem(IDC_EDIT_FILE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		yoffset += 4;
		pWnd->GetWindowRect(&rect);
		pWnd->SetWindowPos(NULL, xoffset, yoffset, rect.Width(), rect.Height(), 0);

		xoffset += rect.Width() + 40;
	}

	pWnd = GetDlgItem(IDC_BTN_PS_PREVIEW);
	if (pWnd->GetSafeHwnd() != NULL)
	{
//		yoffset -= 4;
		pWnd->GetWindowRect(&rect);
		pWnd->SetWindowPos(NULL, xoffset, yoffset, rect.Width(), rect.Height(), 0);

		xoffset += rect.Width() + 10;
	}

	pWnd = GetDlgItem(IDC_BTN_PS2TS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		pWnd->SetWindowPos(NULL, xoffset, yoffset, rect.Width(), rect.Height(), 0);

		xoffset += rect.Width() + 10;
	}

//	pWnd = GetDlgItem(IDC_BTN_PS2VES);
//	if (pWnd->GetSafeHwnd() != NULL)
//	{
//		pWnd->GetWindowRect(&rect);
//		ScreenToClient(&rect);

//		pWnd->SetWindowPos(NULL, cx - rect.Width() - 5, rect.top, rect.Width(), rect.Height(), 0);
//	}

//	pWnd = GetDlgItem(IDC_BTN_PS2AES);
//	if (pWnd->GetSafeHwnd() != NULL)
//	{
//		pWnd->GetWindowRect(&rect);
//		ScreenToClient(&rect);

//		pWnd->SetWindowPos(NULL, cx - rect.Width() - 5, rect.top, rect.Width(), rect.Height(), 0);
//	}

//	pWnd = GetDlgItem(IDC_BTN_PS2AC3);
//	if (pWnd->GetSafeHwnd() != NULL)
//	{
//		pWnd->GetWindowRect(&rect);
//		ScreenToClient(&rect);

//		pWnd->SetWindowPos(NULL, cx - rect.Width() - 5, rect.top, rect.Width(), rect.Height(), 0);
//	}

	pWnd = GetDlgItem(IDC_BTN_DISPATCH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, cx - rect.Width() - 5, rect.top, rect.Width(), rect.Height(), 0);
	}
}

void CPSMagicView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	//TreeFun_InitTree(&m_treePSSyntax, this, IDC_TREE_PS_SYNTAX);

	//m_hRootItem = TreeFun_InsertItem(&m_treePSSyntax, TREE_ROOT, "PS语法分析", -1, 0, NULL);
	//m_hVidItem = NULL;
	//m_hAudItem = NULL;
//	m_hVidItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "视频语法分析", -1, 0, NULL);
//	m_hAudItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "音频语法分析", -1, 0, NULL);

	CWnd* pWnd;

//	CWnd* pWnd = GetDlgItem(IDC_BTN_PS2VES);
//	pWnd->EnableWindow(FALSE);
	
//	pWnd = GetDlgItem(IDC_BTN_PS2AES);
//	pWnd->EnableWindow(FALSE);

//	pWnd = GetDlgItem(IDC_BTN_PS2AC3);
//	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_DISPATCH);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PS_PREVIEW);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PS2TS);
	pWnd->EnableWindow(FALSE);

	m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	m_dlgProgress.ShowWindow(SW_HIDE);

	m_bInitDone = 1;

	m_dlgVideo.Create(IDD_SHOW_VIDEO_SCREEN, this);
	m_dlgVideo.ShowWindow(SW_HIDE);
}

void CPSMagicView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
	{
		return;		// arrangement not needed
	}

	if (m_bInitDone)
	{
		ReLayoutWindow(cx, cy);
	}
}

void CPSMagicView::Reset(void)
{
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_EDIT_FILE);			//说明窗口已经撤销
	pWnd->SetWindowText("");

//	pWnd = GetDlgItem(IDC_BTN_PS2VES);
//	pWnd->EnableWindow(FALSE);
	
//	pWnd = GetDlgItem(IDC_BTN_PS2AES);
//	pWnd->EnableWindow(FALSE);

//	pWnd = GetDlgItem(IDC_BTN_PS2AC3);
//	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_DISPATCH);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PS_PREVIEW);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PS2TS);
	pWnd->EnableWindow(FALSE);

	//TreeFun_DeleteChildItems(&m_treePSSyntax, m_hRootItem);
//	TreeFun_DeleteChildItems(&m_treePSSyntax, m_hRootItem);
//	TreeFun_DeleteChildItems(&m_treePSSyntax, m_hRootItem);

	//m_hVidItem = NULL;
	//m_hAudItem = NULL;

	m_dlgProgress.ShowWindow(SW_HIDE);
}

CPSMagicView* CPSMagicView::GetView(void)
{
	CPSMagicView* pView = NULL;

	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CMultiDocTemplate* pDocTemplate = pApp->m_pDocTemplateForPS;

	POSITION posDoc = pDocTemplate->GetFirstDocPosition();
	while (posDoc != NULL)
	{
		CPSMagicDoc* pDoc = (CPSMagicDoc*)pDocTemplate->GetNextDoc(posDoc);
		POSITION posView = pDoc->GetFirstViewPosition();
		while (posView != NULL)
		{
			pView = (CPSMagicView*)pDoc->GetNextView(posView);
			break;
		}
	}

	return pView;
}


void CPSMagicView::OnBtnOpenOrClose() 
{
	// TODO: Add your control notification handler code here

//	int		nStreamType = STREAM_PS | STREAM_FILE;
//
//	CWnd*	pWnd;
//
//	CString strTitle;
//	pWnd = GetDlgItem(IDC_BTN_OPEN);
//	pWnd->GetWindowText(strTitle);
//
//	if (strTitle == "打开")
//	{
//		CFileDialog fileDlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "mpeg video files(*.*)|*.*|", NULL);
//
//		CString strFileName = _T("");
//
//		if (fileDlg.DoModal() == IDOK)
//		{
//			strFileName = fileDlg.GetPathName();
//
//			if (FILE_CheckStreamType(nStreamType & (~STREAM_FILE), strFileName.GetBuffer(256)))
//			{
//				pWnd = GetDlgItem(IDC_BTN_OPEN);
//				pWnd->SetWindowText("关闭");
//
//				pWnd = GetDlgItem(IDC_EDIT_FILE);
//				pWnd->SetWindowText(strFileName);
//
////				pWnd = GetDlgItem(IDC_BTN_PS2VES);
////				pWnd->EnableWindow(TRUE);
//				
////				pWnd = GetDlgItem(IDC_BTN_PS2AES);
////				pWnd->EnableWindow(TRUE);
//
////				pWnd = GetDlgItem(IDC_BTN_PS2AC3);
////				pWnd->EnableWindow(TRUE);
//
//				pWnd = GetDlgItem(IDC_BTN_DISPATCH);
//				pWnd->EnableWindow(TRUE);
//
//				pWnd = GetDlgItem(IDC_BTN_PS_PREVIEW);
//				pWnd->EnableWindow(TRUE);
//
//				pWnd = GetDlgItem(IDC_BTN_PS2TS);
//				pWnd->EnableWindow(TRUE);
//
//				memset(&m_packinfo, 0x00, sizeof(PS_PACKINFO_t));
//				memset(&m_videoinfo, 0x00, sizeof(PS_VIDEOINFO_t));
//				memset(&m_audioinfo, 0x00, sizeof(PS_AUDIOINFO_t));
//
//				m_MPGPSDecoder.Open(this->GetSafeHwnd(), nStreamType, strFileName.GetBuffer(256));
//
//				UpdateData(FALSE);
//			}
//			else
//			{
//				AfxMessageBox("ERROR: 文件类型不正确!");
//			}
//		}
//	}
//	else
//	{
//		m_MPGPSDecoder.Close();
//
//		Reset();
//	}
}

LRESULT CPSMagicView::OnUpdateMPEGPackHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//MPEG_pack_header_t*			    ppack_header = (MPEG_pack_header_t*)wParam;
	//
	//if (m_hRootItem != NULL)
	//{
	//	hItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "pack_header()", -1, 0, NULL);
	//	PS_PACK_HEADER(&m_treePSSyntax, hItem, ppack_header);
	//	m_treePSSyntax.Expand(m_hRootItem, TREE_EXPAND);

	//	//存储打包信息]
	//	if ((ppack_header->nextbits & 0x0f) == 0x2)				//VCD
	//	{
	//		strcpy_s(m_packinfo.pszFormat, sizeof(m_packinfo.pszFormat), "VCD");
	//		m_packinfo.bitrate = ppack_header->u.mpeg1_ps.program_mux_rate * 400;
	//	}
	//	else
	//	{
	//		strcpy_s(m_packinfo.pszFormat, sizeof(m_packinfo.pszFormat), "DVD");
	//		m_packinfo.bitrate = ppack_header->u.mpeg2_ps.program_mux_rate * 400;
	//	}
	//}
	return 0;
}

LRESULT CPSMagicView::OnUpdateMPEGSystemHeader(WPARAM wParam, LPARAM lParam)
{
//	TREEITEM						hItem;
//	MPEG_system_header_t*		    psystem_header = (MPEG_system_header_t*)wParam;
//	
//	if (m_hRootItem != NULL)
//	{
//		hItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "system_header()", -1, 0, NULL);
//		PS_SYSTEM_HEADER(&m_treePSSyntax, hItem, psystem_header);
////		m_treeESSyntax.Expand(m_hRootItem, TREE_EXPAND);
//	}
	return 0;
}

LRESULT CPSMagicView::OnUpdateM2VEsSequenceHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//MPGV_sequence_header_t*		    psequence_header = (MPGV_sequence_header_t*)wParam;
	//
	//if (m_hVidItem == NULL)
	//{
	//	m_hVidItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "MPEG视频语法分析()", -1, 0, NULL);
	//}

	//hItem = TreeFun_InsertItem(&m_treePSSyntax, m_hVidItem, "sequence_header()", -1, 0, NULL);
	////MPEG_VIDEO_ES_SEQUENCE_HEADER(&m_treePSSyntax, hItem, psequence_header);
	//m_treePSSyntax.Expand(m_hVidItem, TREE_EXPAND);

	////存储视频信息
	//strcpy_s(m_videoinfo.pszFormat, sizeof(m_videoinfo.pszFormat), "MPEG");
	//m_videoinfo.weight = psequence_header->horizontal_size_value;
	//m_videoinfo.height = psequence_header->vertical_size_value;
	//m_videoinfo.bitrate = psequence_header->bit_rate_value* 400;
	//MPGV_DecodeAspectRatioCodetoText(psequence_header->aspect_ratio_information, m_videoinfo.pszAspect, sizeof(m_videoinfo.pszAspect));
	//MPGV_DecodeFrameRateCodetoText(psequence_header->frame_rate_code, m_videoinfo.pszFramerate, sizeof(m_videoinfo.pszFramerate));
	return 0;
}

LRESULT CPSMagicView::OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//M2A_header_t*				    pheader = (M2A_header_t*)wParam;
	//
	//if (m_hAudItem == NULL)
	//{
	//	m_hAudItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "音频语法分析()", -1, 0, NULL);
	//}

	//hItem = TreeFun_InsertItem(&m_treePSSyntax, m_hAudItem, "header()", -1, 0, NULL);
	//MPEG_AUDIO_ES_HEADER(&m_treePSSyntax, hItem, pheader);
	//m_treePSSyntax.Expand(m_hAudItem, TREE_EXPAND);

	//char	pszText[16];
	////存储音频频信息
	////ID
	//M2A_DecodeID(pheader->ID, m_audioinfo.pszFormat, sizeof(m_audioinfo.pszFormat));
	////layer
	//M2A_DecodeLayer(pheader->layer, pszText, sizeof(pszText));
	//sprintf_s(m_audioinfo.pszFormat, sizeof(m_audioinfo.pszFormat), "%s %s\n", m_audioinfo.pszFormat, pszText);

	//M2A_DecodeSamplingFrequency(pheader->ID, pheader->sampling_frequency, m_audioinfo.pszSamplerate, sizeof(m_audioinfo.pszSamplerate));
	//M2A_DecodeMode(pheader->mode, m_audioinfo.pszChannel, sizeof(m_audioinfo.pszChannel));
	//M2A_DecodeBitrate(pheader->ID, pheader->layer, pheader->bitrate_index, m_audioinfo.pszBitrate, sizeof(m_audioinfo.pszBitrate));

	return 0;
}

LRESULT CPSMagicView::OnUpdateAC3EsSyncInfo(WPARAM wParam, LPARAM lParam)
{
	//TREEITEM						hItem;
	//AC3_syncinfo_t*				    psyncinfo = (AC3_syncinfo_t*)wParam;
	//
	//if (m_hAudItem == NULL)
	//{
	//	m_hAudItem = TreeFun_InsertItem(&m_treePSSyntax, m_hRootItem, "AC3音频语法分析()", -1, 0, NULL);
	//}

	//hItem = TreeFun_InsertItem(&m_treePSSyntax, m_hAudItem, "syncinfo()", -1, 0, NULL);
	//AC3_AUDIO_ES_SYNCINFO(&m_treePSSyntax, hItem, psyncinfo);
	//m_treePSSyntax.Expand(m_hAudItem, TREE_EXPAND);

	////存储音频频信息
	////ID
	//strcpy_s(m_audioinfo.pszFormat, sizeof(m_audioinfo.pszFormat), "AC3");

	////fscod
	//AC3_DecodeSamplingFrequency(psyncinfo->fscod, m_audioinfo.pszSamplerate, sizeof(m_audioinfo.pszSamplerate));
	////frmsizecod
	//AC3_DecodeFrameSize(psyncinfo->frmsizecod, psyncinfo->fscod, m_audioinfo.pszBitrate, sizeof(m_audioinfo.pszBitrate));

	return 0;
}

void CPSMagicView::OnBtnEsDispatch() 
{
	// TODO: Add your control notification handler code here
	//CDlg_Decimate		dlg;

	//char	pszDstFile[MAX_PATH];
	//int		length;
	//int		i;

	//strcpy_s(m_packinfo.pszFile, sizeof(m_packinfo.pszFile), m_MPGPSDecoder.m_pszFileName);
	//length = strlen(m_packinfo.pszFile);
	//for (i = length - 1; i >= 0; i--)
	//{
	//	if (m_packinfo.pszFile[i] == '.')
	//	{
	//		memset(pszDstFile, 0x00, sizeof(pszDstFile));
	//		memcpy(pszDstFile, m_packinfo.pszFile, i);
	//		break;
	//	}
	//}

	//sprintf_s(m_videoinfo.pszFile, sizeof(m_videoinfo.pszFile), "%s.mpv", pszDstFile);
	//if (strcmp(m_packinfo.pszFormat, "VCD") == 0)
	//{
	//	sprintf_s(m_audioinfo.pszFile, sizeof(m_audioinfo.pszFile), "%s.mpa", pszDstFile);
	//}
	//else
	//{
	//	sprintf_s(m_audioinfo.pszFile, sizeof(m_audioinfo.pszFile), "%s.ac3", pszDstFile);
	//}

	//m_videoinfo.stream_id = m_MPGPSDecoder.m_ucVidID;
	//m_audioinfo.stream_id = m_MPGPSDecoder.m_ucAudID;

	//memcpy(&dlg.m_packinfo, &m_packinfo, sizeof(PS_PACKINFO_t));
	//memcpy(&dlg.m_videoinfo, &m_videoinfo, sizeof(PS_VIDEOINFO_t));
	//memcpy(&dlg.m_audioinfo, &m_audioinfo, sizeof(PS_AUDIOINFO_t));

	//if (dlg.DoModal() == IDOK)
	//{
	//	memcpy(&m_packinfo, &dlg.m_packinfo, sizeof(PS_PACKINFO_t));
	//	memcpy(&m_videoinfo, &dlg.m_videoinfo, sizeof(PS_VIDEOINFO_t));
	//	memcpy(&m_audioinfo, &dlg.m_audioinfo, sizeof(PS_AUDIOINFO_t));

	//	if (m_packinfo.dispatch_option > 0)
	//	{
	//		m_dlgProgress.ShowWindow(SW_SHOW);
	//	}

	//	if (dlg.m_packinfo.dispatch_option == 1)				//导出视频
	//	{
	//		m_dlgProgress.SetWindowText("导出视频");
	//		m_MPGPSDecoder.PackDecimate(m_dlgProgress.GetSafeHwnd(), m_videoinfo.pszFile, NULL);
	//	}
	//	else if (m_packinfo.dispatch_option == 2)				//导出音频
	//	{
	//		m_dlgProgress.SetWindowText("导出音频");
	//		m_MPGPSDecoder.PackDecimate(m_dlgProgress.GetSafeHwnd(), NULL, m_audioinfo.pszFile);
	//	}
	//	else if (m_packinfo.dispatch_option == 3)				//导出视频和音频
	//	{
	//		m_dlgProgress.SetWindowText("导出音视频");
	//		m_MPGPSDecoder.PackDecimate(m_dlgProgress.GetSafeHwnd(), m_videoinfo.pszFile, m_audioinfo.pszFile);
	//	}
	//}
}

void CPSMagicView::OnBtnPsPreview() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	//CString strFileName;
	//
	//CWnd* pWnd = GetDlgItem(IDC_EDIT_FILE);
	//pWnd->GetWindowText(strFileName);

	//if (strFileName != "")
	//{
	//	m_dlgVideo.OfflineStream((STREAM_PS | STREAM_FILE), &m_MPGPSDecoder, 0);
	//}
}
