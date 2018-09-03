// Dlg_TS_Analyzer_Es.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Es.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\Magic_TS\TSMagic_DBase_Pcrs.h"
#include "..\Magic_TS\TSMagic_Trigger_PESES.h"
#include "..\Magic_TS\TSMagic_GuiApi_MSG.h"
#include "..\resource.h"

#include "TSMagicView.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Es dialog

#include "..\Common\GuiCommon_TreeFunction.h"
#include "..\Common\GuiCommon_H264VideoSyntax.h"
#include "..\Common\GuiCommon_MpegVideoSyntax.h"
#include "..\Common\GuiCommon_MpegAudioSyntax.h"
#include "..\Common\GuiCommon_AACAudioSyntax.h"
#include "..\Common\GuiCommon_AC3AudioSyntax.h"
#include "..\Common\GuiCommon_PesSyntax.h"

#include "MiddleWare_Tools/Include/MiddleWare_Tools.h"
#include "Mpeg_TSPacket\Include\Mpeg2_TS_PayloadClass.h"

CDlg_TSAnalyzer_Es::CDlg_TSAnalyzer_Es(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Es::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Es)
	//}}AFX_DATA_INIT

	fp_debug = NULL;
}

void CDlg_TSAnalyzer_Es::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_PesEs)
	DDX_Control(pDX, IDC_TREE_TS_PROGRAMS, m_treePrograms);
	DDX_Control(pDX, IDC_TREE_ES_SYNTAX, m_treeESSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Es, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Es)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ES_WATCH, OnBtnWatch)
//	ON_BN_CLICKED(IDC_BTN_ES_DTS, OnBtnDts)
//	ON_BN_CLICKED(IDC_BTN_ES_PTS, OnBtnPts)
//	ON_BN_CLICKED(IDC_BTN_ES_TRIGGER, OnBtnTrigger)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_PAT, OnUpdatePAT)
	ON_MESSAGE(WM_UPDATE_PMT, OnUpdatePMT)

	//ON_MESSAGE(WM_UPDATE_PES_HEADER, OnUpdatePesHeader)

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

	//ON_MESSAGE(WM_UPDATE_H264_ES_SEQ_PARAMETER_SET, OnUpdateH264EsSeqParameterSet)
	//ON_MESSAGE(WM_UPDATE_H264_ES_PIC_PARAMETER_SET, OnUpdateH264EsPicParameterSet)

	//ON_MESSAGE(WM_UPDATE_AAC_ES_HEADER, OnUpdateAACEsHeader)

	//ON_MESSAGE(WM_UPDATE_M2A_ES_HEADER, OnUpdateM2AEsHeader)

	//ON_MESSAGE(WM_UPDATE_AC3_ES_SYNCINFO, OnUpdateAC3EsSyncinfo)
	//ON_MESSAGE(WM_UPDATE_AC3_ES_BSI, OnUpdateAC3EsBsi)

	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TS_PROGRAMS, &CDlg_TSAnalyzer_Es::OnDblclkTreeTsPrograms)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_TS_PROGRAMS, &CDlg_TSAnalyzer_Es::OnRclickTreeTsPrograms)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Es message handlers

BOOL CDlg_TSAnalyzer_Es::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_pActiveDecoder = NULL;
	m_pActiveAudDecoder = NULL;
	m_pActiveVidDecoder = NULL;

	m_dlgTV.Create(IDD_SHOW_VIDEO, this);
	m_dlgTV.ShowWindow(SW_HIDE);

	m_dlgDtsPts.Create(IDD_TS_DTS_PTS, this);
	m_dlgDtsPts.ShowWindow(SW_HIDE);

	InitProgramTree();
	InitESTree();

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Es::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	CRect	rect;

	pWnd = GetDlgItem(IDC_TREE_TS_PROGRAMS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 50, 0);
	}

	pWnd = GetDlgItem(IDC_TREE_ES_SYNTAX);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 10, cy - rect.top - 50, 0);
	}

	pWnd = GetDlgItem(IDC_STATIC_ES_AUDIO);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}
	
	pWnd = GetDlgItem(IDC_EDIT_ES_AUDIO);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_STATIC_ES_VIDEO);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_EDIT_ES_VIDEO);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ES_TRIGGER);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_STATIC_ES_GOP_TIME);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_EDIT_ES_GOP_TIME);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ES_PTS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ES_DTS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left , cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ES_BUFFER);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_ES_WATCH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}
}

void CDlg_TSAnalyzer_Es::Reset(void)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CWnd* pWnd;
	char pszText[128];

	pWnd = GetDlgItem(IDC_BTN_ES_PTS);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_ES_DTS);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_ES_BUFFER);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_ES_WATCH);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_ES_TRIGGER);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_EDIT_ES_AUDIO);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_EDIT_ES_VIDEO);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_EDIT_ES_GOP_TIME);
	pWnd->SetWindowText("");

	m_dwVidCode = 0x00000000;
	m_dwAudCode = 0x00000000;

	TreeFun_DeleteChildItems(&m_treePrograms, m_hProgramsItem);
#if CHINESE_VERSION
	sprintf_s(pszText, sizeof(pszText), "现行TS流中的节目");
#else
	sprintf_s(pszText, sizeof(pszText), "programs in actural TS");
#endif
	TreeFun_SetItemText(&m_treePrograms, m_hProgramsItem, pszText);
	m_treePrograms.SetItemState(m_hProgramsItem, ~TVIS_BOLD, TVIS_BOLD);

	TreeFun_DeleteChildItems(&m_treeESSyntax, m_hPESItem);
//	TreeFun_DeleteChildItems(&m_treeESSyntax, m_hVidPesItem);
//	TreeFun_DeleteChildItems(&m_treeESSyntax, m_hVidEsItem);
//	TreeFun_DeleteChildItems(&m_treeESSyntax, m_hAudPesItem);
//	TreeFun_DeleteChildItems(&m_treeESSyntax, m_hAudEsItem);

	if (m_dlgTV.IsWindowVisible())
	{
		m_dlgTV.ShowWindow(SW_HIDE);
	}

//	if (m_dlgDtsPts.IsWindowVisible())
//	{
//		m_dlgDtsPts.ShowWindow(SW_HIDE);
//	}

	if (fp_debug != NULL)
	{
		fclose(fp_debug);
		fp_debug = NULL;
	}

//	if (m_pActiveDecoder != NULL)
//	{
//		if (pTSMagicView != NULL)
//		{
//			TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), m_nActiveType);
//		}

//		m_pActiveDecoder->Close();
//		m_pActiveDecoder = NULL;
//	}

//	m_nActiveType = 0;


	if (m_pActiveVidDecoder != NULL)
	{
		if (pTSMagicView != NULL)
		{
			TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), 0);
		}

		m_pActiveVidDecoder->Close();
		m_pActiveVidDecoder = NULL;
	}

	if (m_pActiveAudDecoder != NULL)
	{
		if (pTSMagicView != NULL)
		{
			TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), 1);
		}
		
		m_pActiveAudDecoder->Close();
		m_pActiveAudDecoder = NULL;
	}

}

void CDlg_TSAnalyzer_Es::Set(int offline)
{
	CWnd* pWnd = GetDlgItem(IDC_BTN_ES_WATCH);
	pWnd->EnableWindow(TRUE);

//	pWnd = GetDlgItem(IDC_BTN_PTS);
//	pWnd->EnableWindow(TRUE);

//	pWnd = GetDlgItem(IDC_BTN_DTS);
//	pWnd->EnableWindow(TRUE);

//	pWnd = GetDlgItem(IDC_BTN_TRIGGER);
//	pWnd->EnableWindow(TRUE);

	if (fp_debug == NULL)
	{
//		fp_debug = fopen("e:\\dts_pts.txt", "wt");
	}
}

void CDlg_TSAnalyzer_Es::InitProgramTree(void)
{
	TreeFun_InitTree(&m_treePrograms, this, IDC_TREE_TS_PROGRAMS);
#if CHINESE_VERSION
	m_hProgramsItem = TreeFun_InsertItem(&m_treePrograms, TREE_ROOT, "现行TS流中的节目", -1, 0, NULL);
#else
	m_hProgramsItem = TreeFun_InsertItem(&m_treePrograms, TREE_ROOT, "programs in actural TS", -1, 0, NULL);
#endif
	m_treePrograms.ModifyStyle(0, TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVIS_STATEIMAGEMASK | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT);
	m_treePrograms.SetItemState(m_hProgramsItem, ~TVIS_BOLD, TVIS_BOLD);

	m_ESImageList.Create(IDB_ES, 20, 1, RGB(0xab, 0xcd, 0xef));
	m_treePrograms.SetImageList(&m_ESImageList, LVSIL_NORMAL);

	m_treePrograms.SetItemData(m_hProgramsItem, 0x00000000);
	m_treePrograms.SetItemImage(m_hProgramsItem, 0, 0);

//	m_treePrograms.Expand(m_hProgramsItem, TREE_EXPAND);
}

void CDlg_TSAnalyzer_Es::OnBtnWatch()
{
	// TODO: Add your control notification handler code here
	int nVidType;
	int nAudType;
	int nVidStreamType = STREAM_UNKNOWN;
	int nAudStreamType = STREAM_UNKNOWN;

	if (!m_dlgTV.IsWindowVisible())
	{
		nVidType = (m_dwVidCode & 0xFC000000) >> 26;
		nAudType = (m_dwAudCode & 0xFC000000) >> 26;

		switch (nVidType)
		{
		case 1:
			nVidStreamType = STREAM_MPEGVES;
			break;
		case 6:
			nVidStreamType = STREAM_H264VES;
			break;
		}

		switch (nAudType)
		{
		case 2:
			nAudStreamType = STREAM_MPEGAES;
			break;
		case 3:
			nAudStreamType = STREAM_AC3AES;
			break;
		case 4:
			nAudStreamType = STREAM_AACAES;
			break;
		}

		m_dlgTV.RealTimeStream(nVidStreamType, m_pActiveVidDecoder, nAudStreamType, m_pActiveAudDecoder);
	}
}

void CDlg_TSAnalyzer_Es::SetESInfo(void)
{
/*
	int		nVidType;
	int		nAudType;
//	char	pszItem[128];

	Video_decode_info_t	VideoDecodeInfo;
	Audio_decode_info_t AudioDecodeInfo;

	nVidType = (m_dwVidCode & 0xFC000000) >> 26;
	nAudType = (m_dwAudCode & 0xFC000000) >> 26;

	int nCount = m_listESInfo.GetItemCount();
	int nItem = m_listESInfo.InsertItem(nCount, "");

	switch (nVidType)
	{
	case 1:
		m_listESInfo.SetItemText(nItem, 1, "MPEG VIDEO");

		M2V_REPORT.M2VDecoder.Get_decode_info(&VideoDecodeInfo);
		break;
	case 6:
		m_listESInfo.SetItemText(nItem, 1, "H264 VIDEO");

		H264_VIDEO_REPORT.H264Decoder.Get_decode_info(&VideoDecodeInfo);
		break;
	default:
		m_listESInfo.SetItemText(nItem, 1, "Unknown VIDEO");
		break;
	}

	nCount = m_listESInfo.GetItemCount();
	nItem = m_listESInfo.InsertItem(nCount, "");
	m_listESInfo.SetItemText(nItem, 1, "W x H");

	nCount = m_listESInfo.GetItemCount();
	nItem = m_listESInfo.InsertItem(nCount, "");

	nCount = m_listESInfo.GetItemCount();
	nItem = m_listESInfo.InsertItem(nCount, "");

	switch (nAudType)
	{
	case 2:
		m_listESInfo.SetItemText(nItem, 1, "MPEG AUDIO");
		break;
	case 3:
		m_listESInfo.SetItemText(nItem, 1, "AAC AUDIO");
		break;
	case 4:
		m_listESInfo.SetItemText(nItem, 1, "AC3 AUDIO");
		break;
	}
*/
}

void CDlg_TSAnalyzer_Es::SetPIDInfo(CPMT* pPMT)
{
#if 0
	char			pszText[MAX_TXT_CHARS];
	int				j;
	U32				usPCR_PID;
	int				nItem;
	U32				code;
	int				nPIDStyle = 0xffffffff;
	int				main_type;
	int				sub_type;

	CTSMagicView* pView = CTSMagicView::GetView();

	m_listESPID.DeleteAllItems();

	if (m_hVidPesItem != NULL)
	{
		TreeFun_DeleteChildItems(&m_treeESSyntax, m_hVidPesItem);
	}
	if (m_hVidEsItem != NULL)
	{
		TreeFun_DeleteChildItems(&m_treeESSyntax, m_hVidEsItem);
	}
	if (m_hAudPesItem != NULL)
	{
		TreeFun_DeleteChildItems(&m_treeESSyntax, m_hAudPesItem);
	}
	if (m_hAudEsItem != NULL)
	{
		TreeFun_DeleteChildItems(&m_treeESSyntax, m_hAudEsItem);
	}

	if (m_pActiveVidDecoder != NULL)
	{
		TSMagic_threadparams_unregist_esdecoder(&(pView->m_kThreadParams), 0);

		m_pActiveVidDecoder->Close();
		m_pActiveVidDecoder = NULL;
	}

	if (m_pActiveAudDecoder != NULL)
	{
		TSMagic_threadparams_unregist_esdecoder(&(pView->m_kThreadParams), 1);

		m_pActiveAudDecoder->Close();
		m_pActiveAudDecoder = NULL;
	}

	m_dwVidCode = 0x00000000;
	m_dwAudCode = 0x00000000;

	usPCR_PID = pPMT->m_usPCR_PID;
	usPCR_PID <<= 13;

	m_usPCRPID = pPMT->m_usPCR_PID;

	m_listESPID.InsertItem(0, "", 5);

#if CHINESE_VERSION
	sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_usPCR_PID, pPMT->m_usPCR_PID);
#else
	sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_usPCR_PID, pPMT->m_usPCR_PID);
#endif
	m_listESPID.SetItemData(0, pPMT->m_usPCR_PID);
	m_listESPID.SetItemText(0, 1, pszText);
	m_listESPID.SetItemText(0, 2, "PCR_PID");

	nItem = 1;
	for (j = 0; j < pPMT->m_nEsCount; j++)
	{
//		int nPIDStyle = PIDSTYLE_MPEG_PRIVATE;
//		U8	ucSubType = STREAM_SUBTYPE_UNKNOWN;
		
		m_listESPID.InsertItem(nItem, "", 1);

		nPIDStyle = StreamTypeLookup(pPMT->m_astEsInfo[j].stream_type, pPMT->m_astEsInfo[j].stream_subtype, pszText, sizeof(pszText));
		m_listESPID.SetItemText(nItem, 2, pszText);

		main_type = (nPIDStyle & 0xffff0000) >> 16;
		sub_type = (nPIDStyle & 0x0000ffff);

		sprintf_s(pszText, sizeof(pszText), "0x%04X (%d)", pPMT->m_astEsInfo[j].elementary_PID, pPMT->m_astEsInfo[j].elementary_PID);
		m_listESPID.SetItemText(nItem, 1, pszText);

		if (main_type == PID_MAINTYPE_VIDEO)
		{
			if ((sub_type == VIDEO_MPEG1) ||
				(sub_type == VIDEO_MPEG2))
			{
				code = usPCR_PID | (pPMT->m_astEsInfo[j].elementary_PID) | 0x04000000;
				m_listESPID.SetItemData(nItem, code);
				m_listESPID.SetItem(nItem, 0, TVIF_IMAGE, "", 2, 0, 0, NULL);

				if (m_dwVidCode == 0x00000000)
				{
					m_pActiveVidDecoder = &m_M2VDecoder;

					m_pActiveVidDecoder->SetParams(pPMT->m_astEsInfo[j].elementary_PID, usPCR_PID);
					m_pActiveVidDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
					TSMagic_threadparams_regist_esdecoder(&(pView->m_kThreadParams), 0, m_pActiveVidDecoder);

					m_dwVidCode = code;
					CWnd* pWnd = GetDlgItem(IDC_EDIT_VIDEO);
					pWnd->SetWindowText(pszText);
				}
			}
			else if (sub_type == VIDEO_H264)
			{
	/*
				//h.264 decoder has some bugs				chendelin 20090205 

				code = usPCR_PID | (pPMT->m_astESMap[j].elementary_PID) | 0x18000000;
				m_listESPID.SetItemData(nItem, code);
				m_listESPID.SetItem(nItem, 0, TVIF_IMAGE, "", 2, 0, 0, NULL);
				
				if (m_dwVidCode == 0x00000000)
				{
					m_pActiveVidDecoder = &m_H264Decoder;

					m_pActiveVidDecoder->SetParams(pPMT->m_astESMap[j].elementary_PID, usPCR_PID);
					m_pActiveVidDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
					TSMagic_threadparams_regist_esdecoder(&(pView->m_kThreadParams), 0, m_pActiveVidDecoder);

					m_dwVidCode = code;
					CWnd* pWnd = GetDlgItem(IDC_EDIT_VIDEO);
					pWnd->SetWindowText(pszText);
				}
	*/
			}
		}
		else if (main_type == PID_MAINTYPE_AUDIO)
		{
			if ((sub_type == AUDIO_MPEG1)	 ||
				(sub_type == AUDIO_MPEG1_L1) ||
				(sub_type == AUDIO_MPEG1_L2) ||
				(sub_type == AUDIO_MPEG1_L3) ||
				(sub_type == AUDIO_MPEG2)	 ||
				(sub_type == AUDIO_MPEG2_L1) ||
				(sub_type == AUDIO_MPEG2_L2) ||
				(sub_type == AUDIO_MPEG2_L3))
			{
				code = usPCR_PID | (pPMT->m_astEsInfo[j].elementary_PID) | 0x08000000;
				m_listESPID.SetItemData(nItem, code);
				m_listESPID.SetItem(nItem, 0, TVIF_IMAGE, "", 3, 0, 0, NULL);

				if (m_dwAudCode == 0x00000000)
				{
					m_pActiveAudDecoder = &m_M2ADecoder;

					m_pActiveAudDecoder->SetParams(pPMT->m_astEsInfo[j].elementary_PID, usPCR_PID);
					m_pActiveAudDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
					TSMagic_threadparams_regist_esdecoder(&(pView->m_kThreadParams), 1, m_pActiveAudDecoder);

					m_dwAudCode = code;
					CWnd* pWnd = GetDlgItem(IDC_EDIT_AUDIO);
					pWnd->SetWindowText(pszText);
				}
			}
			else if (sub_type == AUDIO_AC3)
			{
				code = usPCR_PID | (pPMT->m_astEsInfo[j].elementary_PID) | 0x0c000000;
				m_listESPID.SetItemData(nItem, code);
				m_listESPID.SetItem(nItem, 0, TVIF_IMAGE, "", 3, 0, 0, NULL);
				
				if (m_dwAudCode == 0x00000000)
				{
					m_pActiveAudDecoder = &m_AC3Decoder;

					m_pActiveAudDecoder->SetParams(pPMT->m_astEsInfo[j].elementary_PID, usPCR_PID);
					m_pActiveAudDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
					TSMagic_threadparams_regist_esdecoder(&(pView->m_kThreadParams), 1, m_pActiveAudDecoder);

					m_dwAudCode = code;
					CWnd* pWnd = GetDlgItem(IDC_EDIT_AUDIO);
					pWnd->SetWindowText(pszText);
				}
			}
			else if (sub_type == AUDIO_MPEG2_AAC)
			{
				code = usPCR_PID | (pPMT->m_astEsInfo[j].elementary_PID) | 0x10000000;
				m_listESPID.SetItemData(nItem, code);
				m_listESPID.SetItem(nItem, 0, TVIF_IMAGE, "", 3, 0, 0, NULL);

				if (m_dwAudCode == 0x00000000)
				{
					m_pActiveAudDecoder = &m_AACDecoder;
			
					m_pActiveAudDecoder->SetParams(pPMT->m_astEsInfo[j].elementary_PID, usPCR_PID);
					m_pActiveAudDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
					TSMagic_threadparams_regist_esdecoder(&(pView->m_kThreadParams), 1, m_pActiveAudDecoder);

					m_dwAudCode = code;
					CWnd* pWnd = GetDlgItem(IDC_EDIT_AUDIO);
					pWnd->SetWindowText(pszText);
				}
			}
		}
		else
		{
			m_listESPID.SetItem(nItem, 0, TVIF_IMAGE, "", 4, 0, 0, NULL);
		}

		nItem ++;
	}

	SetESInfo();
#endif
}

/*
void CDlg_TSAnalyzer_Es::OnSelchangeCmbServices() 
{
	// TODO: Add your control notification handler code here
	int		nSel;
	CPMT*	pPMT;

	m_dwAudCode = 0x00000000;
	m_dwVidCode = 0x00000000;

	CWnd* pWnd = GetDlgItem(IDC_EDIT_VIDEO);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_EDIT_AUDIO);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_EDIT_GOP_TIME);
	pWnd->SetWindowText("");

	nSel = m_cmbServices.GetCurSel();
	if (nSel != CB_ERR)
	{
		pPMT = &(PSISI_REPORT.astPMT[nSel]);

		SetPIDInfo(pPMT);
	}
}
*/
/*
void CDlg_TSAnalyzer_Es::OnDblclkListEsPid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	CEverStation_TSMagicDlg* pFrame = (CEverStation_TSMagicDlg*)AfxGetMainWnd();

	int nSel = m_listESPID.GetSelectionMark();
	if (nSel >= 0)
	{
//		m_listESInfo.DeleteAllItems();

		DWORD code = m_listESPID.GetItemData(nSel);

		int nESStyle = (code & 0xFC000000) >> 26;
		int nESPID = code & 0x00001FFF;
	//	int nPCRPID = (code & 0x03ffC000) >> 13;

		CString strItem;
		CWnd*	pWnd;

		switch (nESStyle)
		{
		case 1:				//MPEG Video
//			gkThreadParams.usPID_MPEGVideo = nESPID;
			
			m_dwVidCode = code;
			strItem = m_listESPID.GetItemText(nSel, 1);
			pWnd = GetDlgItem(IDC_EDIT_VIDEO);
			pWnd->SetWindowText(strItem);

			break;
		case 2:				//MPEG Audio
//			gkThreadParams.usPID_MPEGAudio = nESPID;

			m_dwAudCode = code;
			strItem = m_listESPID.GetItemText(nSel, 1);
			pWnd = GetDlgItem(IDC_EDIT_AUDIO);
			pWnd->SetWindowText(strItem);
			
			break;
		case 3:				//AC3 Audio
//			gkThreadParams.usPID_AC3Audio = nESPID;

			m_dwAudCode = code;
			strItem = m_listESPID.GetItemText(nSel, 1);
			pWnd = GetDlgItem(IDC_EDIT_AUDIO);
			pWnd->SetWindowText(strItem);

			break;
		case 4:				//AAC_Audio
//			gkThreadParams.usPID_AACAudio = nESPID;

			m_dwAudCode = code;
			strItem = m_listESPID.GetItemText(nSel, 1);
			pWnd = GetDlgItem(IDC_EDIT_AUDIO);
			pWnd->SetWindowText(strItem);

			break;
		case 5:				//AIT: MHP
	//		gkThreadParams.usPID_MPEGAudio = nESPID;
	//		pFrame->OnFramePsisiSyntaxInfo(0x00000000);
			break;
		case 6:
//			gkThreadParams.usPID_H264Video = nESPID;
	//		pFrame->OnFrameH264VideoSyntaxInfo(nESPID, nPCRPID);

			m_dwVidCode = code;
			strItem = m_listESPID.GetItemText(nSel, 1);
			pWnd = GetDlgItem(IDC_EDIT_VIDEO);
			pWnd->SetWindowText(strItem);
			break;

		default:
			break;
		}

		SetESInfo();
	}

	*pResult = 0;
}
*/

LRESULT CDlg_TSAnalyzer_Es::OnUpdatePAT(WPARAM wParam, LPARAM lParam)
{
	char			pszText[MAX_TXT_CHARS];
	int				program_index;
	int				valid_program_count;
	CPAT*			pPAT = (CPAT*)wParam;		

	PROGRAM_MAP_DESCRIPTION_t*		pProgram;

	HTREEITEM		hItem;

	if (pPAT != NULL)
	{
		valid_program_count = 0;
		for (program_index = 0; program_index < pPAT->m_nProgramCount; program_index++)
		{
			pProgram = &(pPAT->m_astProgram[program_index]);

			if (pProgram->program_number != 0)
			{
#if CHINESE_VERSION
				sprintf_s(pszText, sizeof(pszText), "节目 0x%04X(%d)", pProgram->program_number, pProgram->program_number);
#else
				sprintf_s(pszText, sizeof(pszText), "PROGRAM 0x%04X(%d)", pProgram->program_number, pProgram->program_number);
#endif
				hItem = TreeFun_InsertItem(&m_treePrograms, m_hProgramsItem, pszText, -1, 0, NULL);
				m_treePrograms.SetItemData(hItem, pProgram->program_number & 0x0000FFFF);
				m_treePrograms.SetItemImage(hItem, 1, 1);

				valid_program_count++;
			}
		}
		m_treePrograms.SetItemState(m_hProgramsItem, TVIS_BOLD, TVIS_BOLD);
		m_treePrograms.Expand(m_hProgramsItem, TREE_EXPAND);

#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "现行流<TSID=0x%04X(%d)>中的节目(共%d个)", pPAT->m_usTransportStreamID, pPAT->m_usTransportStreamID, valid_program_count);
#else
		sprintf_s(pszText, sizeof(pszText), "programs in actural stream<TSID=0x%04X(%d)>-(total:%d)", pPAT->m_usTransportStreamID, pPAT->m_usTransportStreamID, valid_program_count);
#endif
		TreeFun_SetItemText(&m_treePrograms, m_hProgramsItem, pszText);
	}

	return 0;
}

//
LRESULT CDlg_TSAnalyzer_Es::OnUpdatePMT(WPARAM wParam, LPARAM lParam)
{
//	int				nSel;
	CPMT*			pPMT = (CPMT*)wParam;
//	int				program_index = (int)lParam;
	char			pszText[MAX_TXT_CHARS];
	char			pszTemp[MAX_TXT_CHARS];
	int				program_number;
	U32				pid_style_code;
	int				main_type;
	int				sub_type;
	HTREEITEM		hESItem;
	DWORD			dwItemData;

	if (pPMT != NULL)
	{
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "节目 0x%04X(%d)", pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
#else
		sprintf_s(pszText, sizeof(pszText), "PROGRAM 0x%04X(%d)", pPMT->m_usProgramNumber, pPMT->m_usProgramNumber);
#endif
		if (m_treePrograms.ItemHasChildren(m_hProgramsItem))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_treePrograms.GetChildItem(m_hProgramsItem);
			while (hChildItem != NULL)
			{
				program_number = m_treePrograms.GetItemData(hChildItem) & 0x0000ffff;
				if (program_number == pPMT->m_usProgramNumber)
				{
					m_treePrograms.SetItemState(hChildItem, TVIS_BOLD, TVIS_BOLD);

					for (int es_index = 0; es_index < pPMT->m_nEsCount; es_index++)
					{
						GetDescriptionByStreamType(pPMT->m_astEsInfo[es_index].stream_type, pPMT->m_astEsInfo[es_index].stream_subtype, pszTemp, sizeof(pszTemp));

						pid_style_code = pPMT->m_astEsInfo[es_index].elementary_PID;
						pid_style_code <<= 3;
						pid_style_code |= TSPAYLOAD_CLASS_UNKNOWN;			//这么判断是错误的
						pid_style_code <<= 8;
						pid_style_code |= pPMT->m_astEsInfo[es_index].stream_type;
						pid_style_code <<= 8;
						pid_style_code |= pPMT->m_astEsInfo[es_index].stream_subtype;

//						if (main_type == PID_MAINTYPE_VIDEO_STREAM)
//						{
//#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "视频:PID=0x%04X(%d)[%s]", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID, pszTemp);
//#else
//							sprintf_s(pszText, sizeof(pszText), "Video:PID=0x%04X(%d)", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID);
//#endif
//							hESItem = TreeFun_InsertItem(&m_treePrograms, hChildItem, pszText, -1, 0, NULL);
//							m_treePrograms.SetItemImage(hESItem, 2, 2);
//						}
//						else if (main_type == PID_MAINTYPE_AUDIO_STREAM)
//						{
//#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "音频:PID=0x%04X(%d)[%s]", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID, pszTemp);
//#else
//							sprintf_s(pszText, sizeof(pszText), "Audio:PID=0x%04X(%d)", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID);
//#endif
//							hESItem = TreeFun_InsertItem(&m_treePrograms, hChildItem, pszText, -1, 0, NULL);
//							m_treePrograms.SetItemImage(hESItem, 3, 3);
//						}
//						else
						{
#if CHINESE_VERSION
//							sprintf_s(pszText, sizeof(pszText), "数据:PID=0x%04X(%d)[%s]", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID, pszTemp);
							sprintf_s(pszText, sizeof(pszText), "ES:PID=0x%04X(%d)[%s]", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID, pszTemp);
#else
							sprintf_s(pszText, sizeof(pszText), "Data:PID=0x%04X(%d)", pPMT->m_astEsInfo[es_index].elementary_PID, pPMT->m_astEsInfo[es_index].elementary_PID);
#endif
							hESItem = TreeFun_InsertItem(&m_treePrograms, hChildItem, pszText, -1, 0, NULL);
							m_treePrograms.SetItemImage(hESItem, 4, 4);
						}

						m_treePrograms.SetItemData(hESItem, pid_style_code);
					}
					break;
				}
				else
				{
					hNextItem = m_treePrograms.GetNextItem(hChildItem, TVGN_NEXT);
					hChildItem = hNextItem;
				}
			}
		}
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2AEsHeader(WPARAM wParam, LPARAM lParam)
{
	TREEITEM						hItem;
	M2A_header_t*				    pheader = (M2A_header_t*)wParam;
	
	if (m_hESItem != NULL)
	{
		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hESItem, "header()", -1, 0, NULL);

		MPEG_AUDIO_ES_HEADER(&m_treeESSyntax, hItem, pheader);
		m_treeESSyntax.Expand(m_hESItem, TVE_EXPAND);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2AEsErrorCheck(WPARAM wParam, LPARAM lParam)
{
/*
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];
	CMPEG_Audio*	pAudio;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	if (MPEG_AUDIO_REPORT.find_mpeg_audio)
	{
		pAudio = &(MPEG_AUDIO_REPORT.MPEG_Audio);

		if (m_hEsErrorCheckItem == NULL)
		{
			TreeCtrlItem.hParent = m_hEsItem;
			strcpy_s(pszText, sizeof(pszText), "error_check()");
			m_hEsErrorCheckItem = m_treeESSyntax.InsertItem(&TreeCtrlItem);
		}

		if (m_treeESSyntax.ItemHasChildren(m_hEsErrorCheckItem))
		{
//			ES_ERROR_CHECK_Modify(m_hEsErrorCheckItem, &(pAudio->error_check));
		}
		else
		{
//			ES_ERROR_CHECK_Insert(m_hEsErrorCheckItem, &(pAudio->error_check));
		}
	}
	else
	{
		DeleteChildItems(m_hEsItem);
		m_hEsErrorCheckItem = NULL;
	}
*/
	return 0;
}

/*
void CDlg_TSAnalyzer_Es::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFormView::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if (bShow)
	{
		if (!m_treeESSyntax.m_hWnd)
		{
			if (m_treeESSyntax.SubclassDlgItem(IDC_TREE_PS_SYNTAX, this))
			{
				TreeFun_InitTree(&m_treeESSyntax);

				m_hRootItem = m_treeESSyntax.InsertItem("program_stream()");

				m_hPackItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "pack()", -1, 0, NULL);
				m_hVidItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "video()", -1, 0, NULL);
				m_hAudItem = TreeFun_InsertItem(&m_treeESSyntax, m_hRootItem, "audio()", -1, 0, NULL);

				m_hVidPesItem = NULL;
				m_hVidEsItem = NULL;

				m_hAudPesItem = NULL;
			}
		}
	}
}
*/

void CDlg_TSAnalyzer_Es::InitESTree(void)
{
//	TREEITEM hVidItem;
//	TREEITEM hAudItem;

	TreeFun_InitTree(&m_treeESSyntax, this, IDC_TREE_ES_SYNTAX);

	m_hPESItem = TreeFun_InsertItem(&m_treeESSyntax, TREE_ROOT, "PES_packet()", -1, 0, NULL);

//	hVidItem = TreeFun_InsertItem(&m_treeESSyntax, TREE_ROOT, "video_stream()", -1, 0, NULL);
//	m_hVidPesItem = TreeFun_InsertItem(&m_treeESSyntax, hVidItem, "PES_packet()", -1, 0, NULL);
//	m_hVidEsItem = TreeFun_InsertItem(&m_treeESSyntax, hVidItem, "ES()", -1, 0, NULL);

//	hAudItem = TreeFun_InsertItem(&m_treeESSyntax, TREE_ROOT, "audio_stream()", -1, 0, NULL);
//	m_hAudPesItem = TreeFun_InsertItem(&m_treeESSyntax, hAudItem, "PES_packet()", -1, 0, NULL);
//	m_hAudEsItem = TreeFun_InsertItem(&m_treeESSyntax, hAudItem, "ES()", -1, 0, NULL);

//	m_treeESSyntax.Expand(hVidItem, TREE_EXPAND);
//	m_treeESSyntax.Expand(hAudItem, TREE_EXPAND);

}

LRESULT CDlg_TSAnalyzer_Es::OnUpdatePesHeader(WPARAM wParam, LPARAM lParam)
{
	PES_packet_t*		pPES_packet = (PES_packet_t*)wParam;
	unsigned char		stream_id = (unsigned char)lParam;
	TREEITEM			hParentItem = NULL;

//	CWnd*				pWnd = GetDlgItem(IDC_BTN_TRIGGER); 

//	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
//	pTSMagicView->m_kThreadParams.es_trigger_thread_running = 0;

//	pWnd->EnableWindow(TRUE);

/*
	if (stream_id == 0xBD)		//AC3 stream
	{
		hParentItem = m_hAudPesItem;
	}
	else if ((stream_id >= 0xC0) && (stream_id <= 0xDF))
	{
		hParentItem = m_hAudPesItem;
	}
	else if ((stream_id >= 0xE0) && (stream_id <= 0xEF))
	{
		hParentItem = m_hVidPesItem;
	}
*/
	hParentItem = m_hPESItem;

	if (hParentItem != NULL)
	{
		if (!TreeFun_HasChild(&m_treeESSyntax, hParentItem))
		{
			PES_HEADER(&m_treeESSyntax, hParentItem, pPES_packet);
			m_hESItem = TreeFun_InsertItem(&m_treeESSyntax, hParentItem, "PES_packet_data_byte()", -1, 0, NULL);
			m_treeESSyntax.Expand(hParentItem, TVE_EXPAND);
		}

//		UpdateDtsPts(pPES_packet, 0);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsSequenceHeader(WPARAM wParam, LPARAM lParam)
{

	TREEITEM						hItem;
	MPGV_sequence_header_t*		    psequence_header = (MPGV_sequence_header_t*)wParam;
	
	if (m_hESItem != NULL)
	{
		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hESItem, "sequence_header()", -1, 0, NULL);
		//MPEG_VIDEO_ES_SEQUENCE_HEADER(&m_treeESSyntax, hItem, psequence_header);
		m_treeESSyntax.Expand(hItem, TVE_EXPAND);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsSequenceExtension(WPARAM wParam, LPARAM lParam)
{
	TREEITEM					hItem;
	MPGV_sequence_extension_t*	psequence_extension = (MPGV_sequence_extension_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hESItem, "sequence_extension()", -1, 0, NULL);
		//MPEG_VIDEO_ES_SEQUENCE_EXTENSION(&m_treeESSyntax, hItem, psequence_extension);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsSequenceDisplayExtension(WPARAM wParam, LPARAM lParam)
{
	TREEITEM					hItem;
	MPGV_sequence_display_extension_t*	psequence_display_extension = (MPGV_sequence_display_extension_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hESItem, "sequence_display_extension()", -1, 0, NULL);
		//MPEG_VIDEO_ES_SEQUENCE_DISPLAY_EXTENSION(&m_treeESSyntax, hItem, psequence_display_extension);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsSequenceScalableExtension(WPARAM wParam, LPARAM lParam)
{
	TREEITEM							hItem;
	MPGV_sequence_scalable_extension_t*	psequence_scalable_extension = (MPGV_sequence_scalable_extension_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = m_treeESSyntax.InsertItem("sequence_scalable_extension()", m_hESItem);

		//MPEG_VIDEO_ES_SEQUENCE_SCALABLE_EXTENSION(&m_treeESSyntax, hItem, psequence_scalable_extension);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsQuantMatrixExtension(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsGroupOfPicturesHeader(WPARAM wParam, LPARAM lParam)
{
	TREEITEM					hItem;
	MPGV_group_of_pictures_header_t*				pgroup_of_pictures_header = (MPGV_group_of_pictures_header_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = m_treeESSyntax.InsertItem("group_of_pictures_header()", m_hESItem);

		//MPEG_VIDEO_ES_GROUP_OF_PICTURES_HEADER(&m_treeESSyntax, hItem, pgroup_of_pictures_header);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsPictureHeader(WPARAM wParam, LPARAM lParam)
{
	char							pszText[MAX_TXT_CHARS];
	TREEITEM						hItem;
	MPGV_picture_header_t*			ppicture_header = (MPGV_picture_header_t*)wParam;

	if (m_hESItem != NULL)
	{
		switch (ppicture_header->picture_coding_type)
		{
		case MPEG_I_FRAME:
			sprintf_s(pszText, sizeof(pszText), "picture_header()--[I]");
			break;
		case MPEG_P_FRAME:
			sprintf_s(pszText, sizeof(pszText), "picture_header()--[P]");
			break;
		case MPEG_B_FRAME:
			sprintf_s(pszText, sizeof(pszText), "picture_header()--[B]");
			break;
		default:
			break;
		}

		hItem = m_treeESSyntax.InsertItem(pszText, m_hESItem);

		//MPEG_VIDEO_ES_PICTURE_HEADER(&m_treeESSyntax, hItem, ppicture_header);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsPictureCodingExtension(WPARAM wParam, LPARAM lParam)
{
	TREEITEM					hItem;
	MPGV_picture_coding_extension_t*			ppicture_coding_extension = (MPGV_picture_coding_extension_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = m_treeESSyntax.InsertItem("picture_coding_extension()", m_hESItem);

		//MPEG_VIDEO_ES_PICTURE_CODING_EXTENSION(&m_treeESSyntax, hItem, ppicture_coding_extension);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsPictureDisplayExtension(WPARAM wParam, LPARAM lParam)
{
	TREEITEM					hItem;
	MPGV_picture_display_extension_t*			ppicture_display_extension = (MPGV_picture_display_extension_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = m_treeESSyntax.InsertItem("picture_display_extension()", m_hESItem);

		//MPEG_VIDEO_ES_PICTURE_DISPLAY_EXTENSION(&m_treeESSyntax, hItem, ppicture_display_extension);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsPictureTemporalScalableExtension(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsPictureSpatialScalableExtension(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsCopyrightExtension(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsSlice(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VEsSequenceEnd(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateAC3EsSyncinfo(WPARAM wParam, LPARAM lParam)
{
	TREEITEM						hItem;
	AC3_syncinfo_t*				    psyncinfo = (AC3_syncinfo_t*)wParam;
	
	if (m_hESItem != NULL)
	{
		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hESItem, "syncinfo()", -1, 0, NULL);
		//AC3_AUDIO_ES_SYNCINFO(&m_treeESSyntax, hItem, psyncinfo);
		m_treeESSyntax.Expand(m_hESItem, TREE_EXPAND);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateAC3EsBsi(WPARAM wParam, LPARAM lParam)
{
	TREEITEM						hItem;
	AC3_bsi_t*					    pbsi = (AC3_bsi_t*)wParam;
	
	if (m_hESItem != NULL)
	{
		hItem = TreeFun_InsertItem(&m_treeESSyntax, m_hESItem, "bsi()", -1, 0, NULL);
		//AC3_AUDIO_ES_BSI(&m_treeESSyntax, hItem, pbsi);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateH264EsSeqParameterSet(WPARAM wParam, LPARAM lParam)
{
	H264_seq_parameter_set_t*		pseq_parameter_set = (H264_seq_parameter_set_t*)wParam;
	TREEITEM						hItem;

	hItem = m_treeESSyntax.InsertItem("seq_parameter_set_rbsp()", m_hESItem);
	//H264_VIDEO_SEQ_PARAMETER_SET(&m_treeESSyntax, hItem, pseq_parameter_set);

	m_treeESSyntax.Expand(m_hESItem, TVE_EXPAND);
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateH264EsPicParameterSet(WPARAM wParam, LPARAM lParam)
{
	H264_pic_parameter_set_t*		ppic_parameter_set = (H264_pic_parameter_set_t*)wParam;
	TREEITEM						hItem;

	if (m_hESItem != NULL)
	{
		hItem = m_treeESSyntax.InsertItem("pic_parameter_set_rbsp()", m_hESItem);
		//H264_VIDEO_PIC_PARAMETER_SET(&m_treeESSyntax, hItem, ppic_parameter_set);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateH264EsSliceOfAnIdrPicture(WPARAM wParam, LPARAM lParam)
{
/*
	char							pszText[MAX_TXT_CHARS];
	CH264_VideoDecoder*				pVideo;
	H264_seq_parameter_set_t		seq_parameter_set;
	H264_pic_parameter_set_t		pic_parameter_set;
	H264_slice_header_t				slice_header;
	CTreeListItem*					hItem;
	S32								nal_unit_type = (S32)wParam;

	if (m_hEsItem != NULL)
	{
		if (H264_VIDEO_REPORT.find_h264_video)
		{
			sprintf_s(pszText, sizeof(pszText), "slice_layer_without_partitioning_rbsp(IDR picture)");
			hItem = m_treeVideoSyntax.InsertItem(pszText, m_hEsItem);

			pVideo = &(H264_VIDEO_REPORT.H264Decoder);
			pVideo->Get_seq_parameter_set(&seq_parameter_set);
			pVideo->Get_pic_parameter_set(&pic_parameter_set);
			pVideo->Get_slice_header(&slice_header);

			ES_SLICE_HEADER_Insert(hItem, &slice_header, nal_unit_type,
											&seq_parameter_set,
											&pic_parameter_set);
		}
	}
*/
	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateH264EsSliceOfANonIdrPicture(WPARAM wParam, LPARAM lParam)
{
/*
	char							pszText[MAX_TXT_CHARS];
	CH264_VideoDecoder*				pVideo;
	H264_seq_parameter_set_t		seq_parameter_set;
	H264_pic_parameter_set_t		pic_parameter_set;
	H264_slice_header_t				slice_header;
	CTreeListItem*					hItem;
	S32								nal_unit_type = (S32)wParam;

	if (m_hEsItem != NULL)
	{
		if (H264_VIDEO_REPORT.find_h264_video)
		{
			sprintf_s(pszText, sizeof(pszText), "slice_layer_without_partitioning_rbsp(Non-IDR picture)");
			hItem = m_treeVideoSyntax.InsertItem(pszText, m_hEsItem);

			pVideo = &(H264_VIDEO_REPORT.H264Decoder);
			pVideo->Get_seq_parameter_set(&seq_parameter_set);
			pVideo->Get_pic_parameter_set(&pic_parameter_set);
			pVideo->Get_slice_header(&slice_header);

			ES_SLICE_HEADER_Insert(hItem, &slice_header, nal_unit_type,
											&seq_parameter_set,
											&pic_parameter_set);
		}
	}
*/
	return 0;
}
/*
LRESULT CDlg_TSAnalyzer_Es::OnUpdateAACPes(WPARAM wParam, LPARAM lParam)
{
	PES_packet_t*				    pPES_packet = (PES_packet_t*)wParam;
	
	if (m_hAudPesItem != NULL)
	{
		if (!TreeFun_HasChild(&m_treeESSyntax, m_hAudPesItem))
		{
			PES_HEADER(&m_treeESSyntax, m_hAudPesItem, pPES_packet);
			m_treeESSyntax.Expand(m_hAudPesItem, TVE_EXPAND);
		}

		UpdateDtsPts(pPES_packet, 1);
	}
	return 0;
}
*/
LRESULT CDlg_TSAnalyzer_Es::OnUpdateAACEsHeader(WPARAM wParam, LPARAM lParam)
{
	TREEITEM						hItem;
	AAC_adts_fixed_header_t*		padts_fixed_header = (AAC_adts_fixed_header_t*)wParam;

	if (m_hESItem != NULL)
	{
		hItem = m_treeESSyntax.InsertItem("adts_fixed_header()", m_hESItem);
//		AAC_AUDIO_ES_ADTS_FIXED_HEADER(&m_treeESSyntax, hItem, padts_fixed_header);

		m_treeESSyntax.Expand(m_hESItem, TVE_EXPAND);
	}

	return 0;
}

LRESULT CDlg_TSAnalyzer_Es::OnUpdateM2VGOPTime(WPARAM wParam, LPARAM lParam)
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

void CDlg_TSAnalyzer_Es::UpdateDtsPts(PES_packet_t* pPES_packet, int bAud)
{
	int								DTSPTS_32_30;
	int								DTSPTS_29_15;
	int								DTSPTS_14_0;

	int								PCR_base_32_30;
	int								PCR_base_29_15;
	int								PCR_base_14_0;

	int								nPcrIndex = -1;
	int								jitter;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_Pcrs*						pDBPcrs = pTSMagicView->GetPcrsDBase();
	DBRecord_PCR_t*					pPcrInfo;
//	int								pcr_index;

	//update the pts-dts-pcr 
	if (m_dlgDtsPts.IsWindowVisible())
	{
		nPcrIndex = pDBPcrs->GetPcrSlot(m_usPCRPID);
		if (nPcrIndex >= 0)
		{
			pPcrInfo = pDBPcrs->m_pPCR_info[nPcrIndex];
//			pcr_index = pPcrInfo->pcr_index;

			PCR_base_32_30 = pPcrInfo->cur_PCR_base_32_30;
			PCR_base_29_15 = pPcrInfo->cur_PCR_base_29_15;
			PCR_base_14_0 = pPcrInfo->cur_PCR_base_14_0;

			if ((fp_debug != NULL) && (bAud == 0))
			{
				fprintf(fp_debug, "PCR_base: %d, %d, %d; pos = %lld\n",
									pPcrInfo->cur_PCR_base_32_30,
									pPcrInfo->cur_PCR_base_29_15,
									pPcrInfo->cur_PCR_base_14_0,
									pPcrInfo->cur_PCR_pos);
			}

			if (m_dlgDtsPts.GetMode() == DTS)
			{
				if ((pPES_packet->PTS_DTS_flags & 0x1) == 0x1)
				{
					DTSPTS_32_30 = pPES_packet->DTS_32_30;
					DTSPTS_29_15 = pPES_packet->DTS_29_15;
					DTSPTS_14_0  = pPES_packet->DTS_14_0;

//					jitter = MPEG_33bits_minus(DTSPTS_32_30, DTSPTS_29_15, DTSPTS_14_0, PCR_base_32_30, PCR_base_29_15, PCR_base_14_0);
					jitter = 0;

					if ((fp_debug != NULL) && (bAud == 0))
					{
						fprintf(fp_debug, "DTS: %d, %d, %d; pos = %d\n",
											pPES_packet->DTS_32_30,
											pPES_packet->DTS_29_15,
											pPES_packet->DTS_14_0,
											0
							);

						fprintf(fp_debug, "code_diff = %d, pos_diff = %lld\n", jitter, 0 - pPcrInfo->cur_PCR_pos);
					}

					if (bAud)
					{
						m_dlgDtsPts.m_graphAudJitter.AppendData(jitter);
					}
					else
					{
						m_dlgDtsPts.m_graphVidJitter.AppendData(jitter);
					}
				}
			}
			else
			{
				if ((pPES_packet->PTS_DTS_flags & 0x2) == 0x2)
				{
					DTSPTS_32_30 = pPES_packet->PTS_32_30;
					DTSPTS_29_15 = pPES_packet->PTS_29_15;
					DTSPTS_14_0  = pPES_packet->PTS_14_0;

//					jitter = MPEG_33bits_minus(DTSPTS_32_30, DTSPTS_29_15, DTSPTS_14_0, PCR_base_32_30, PCR_base_29_15, PCR_base_14_0);
					jitter = 0;

					if ((fp_debug != NULL) && (bAud == 0))
					{
						fprintf(fp_debug, "PTS: %d, %d, %d; pos = %d\n",
											pPES_packet->PTS_32_30,
											pPES_packet->PTS_29_15,
											pPES_packet->PTS_14_0,
											0
										);

						fprintf(fp_debug, "code_diff = %d, pos_diff = %lld\n", jitter, 0 - pPcrInfo->cur_PCR_pos);
					}

					if (bAud)
					{
						m_dlgDtsPts.m_graphAudJitter.AppendData(jitter);
					}
					else
					{
						m_dlgDtsPts.m_graphVidJitter.AppendData(jitter);
					}
				}
			}
		}
	}
}

void CDlg_TSAnalyzer_Es::CancelTrigger(void)
{
	CWnd*	pWnd = NULL;

	pWnd = GetDlgItem(IDC_BTN_ES_TRIGGER);
	pWnd->EnableWindow(TRUE);
}

void CDlg_TSAnalyzer_Es::OnDblclkTreeTsPrograms(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	char		  pszText[MAX_TXT_CHARS];
	int			  nPID;
	int			  nStreamType;
	int			  nSubType;

	HTREEITEM hSelItem = m_treePrograms.GetSelectedItem();
	DWORD dwItemData = m_treePrograms.GetItemData(hSelItem);

	if ((dwItemData & 0xffff0000) != 0x00000000)
	{
		nPID = (dwItemData & 0xffff0000) >> 16;
		nStreamType = (dwItemData & 0x0000ff00) >> 8;
		nSubType = (dwItemData & 0x000000ff);

		if (pTSMagicView->m_kThreadParams.main_thread_running)
		{
			TreeFun_DeleteChildItems(&m_treeESSyntax, m_hPESItem);
//			TreeFun_DeleteChildItems(&m_treeESSyntax, m_hVidPesItem);
//			TreeFun_DeleteChildItems(&m_treeESSyntax, m_hVidEsItem);
//			TreeFun_DeleteChildItems(&m_treeESSyntax, m_hAudPesItem);
//			TreeFun_DeleteChildItems(&m_treeESSyntax, m_hAudEsItem);

			if (m_pActiveVidDecoder != NULL)
			{
				TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), 0);
				m_pActiveVidDecoder->Close();
				m_pActiveVidDecoder = NULL;
			}
			if (m_pActiveAudDecoder != NULL)
			{
				TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), 1);
				m_pActiveAudDecoder->Close();
				m_pActiveAudDecoder = NULL;
			}

			if ((pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1) ||
				(pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1) ||
				(pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1) ||
				(pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1) ||
				(pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1) ||
				(pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1))
			{
				if (pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1)
				{
					sprintf_s(pszText, sizeof(pszText), "ES捕捉：未能启动，因为发现TS捕捉线程尚未结束！");
					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
				if (pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1)
				{
					sprintf_s(pszText, sizeof(pszText), "ES捕捉：未能启动，因为发现PES捕捉线程尚未结束！");
					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
				if (pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1)
				{
					sprintf_s(pszText, sizeof(pszText), "ES捕捉：未能启动，因为发现ES捕捉线程尚未结束！");
					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
				if (pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1)
				{
					sprintf_s(pszText, sizeof(pszText), "ES捕捉：未能启动，因为发现section捕捉线程尚未结束！");
					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
				if (pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1)
				{
					sprintf_s(pszText, sizeof(pszText), "ES捕捉：未能启动，因为发现DSMCC下载线程尚未结束！");
					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
				if (pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1)
				{
					sprintf_s(pszText, sizeof(pszText), "ES捕捉：未能启动，因为发现TS录制线程尚未结束！");
					::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
				}
			}
			else
			{
				if (pTSMagicView->m_kThreadParams.offline == 1)
				{
					if (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)
					{
						//U32 nPIDStyle = StreamTypeLookup(nStreamType, nSubType, NULL, 0);
						//U8 class_type = (nPIDStyle & 0x00070000) >> 16;
						//U8 main_type = (nPIDStyle & 0x0000ff00) >> 8;
						//U8 sub_type = (nPIDStyle & 0x000000ff);
						//CESDecoder* pNewDecoder = NULL;
						//int video_audio_type = 0;

						//if (main_type == PID_MAINTYPE_VIDEO_STREAM)
						//{
						//	if ((sub_type == VIDEO_MPEG1) ||
						//		(sub_type == VIDEO_MPEG2))
						//	{
						//		m_pActiveVidDecoder = &m_M2VDecoder;
						//	}
						//	else if (sub_type == VIDEO_H264)
						//	{
						//		m_pActiveVidDecoder = &m_H264Decoder;
						//	}

						//	if (m_pActiveVidDecoder != NULL)
						//	{
						//		m_pActiveVidDecoder->SetTrigger(ES_TRIGGER_ONESHOT);
						//		m_pActiveVidDecoder->SetParams(nPID, 0xFFFF);
						//		m_pActiveVidDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
						//		TSMagic_threadparams_regist_esdecoder(&(pTSMagicView->m_kThreadParams), 0, m_pActiveVidDecoder);
						//	}

						//}
						//else if (main_type == PID_MAINTYPE_AUDIO_STREAM)
						//{
						//	if ((sub_type == AUDIO_MPEG1) ||
						//		(sub_type == AUDIO_MPEG1_L1) ||
						//		(sub_type == AUDIO_MPEG1_L2) ||
						//		(sub_type == AUDIO_MPEG1_L3) ||
						//		(sub_type == AUDIO_MPEG2) ||
						//		(sub_type == AUDIO_MPEG2_L1) ||
						//		(sub_type == AUDIO_MPEG2_L2) ||
						//		(sub_type == AUDIO_MPEG2_L3))
						//	{
						//		m_pActiveAudDecoder = &m_M2ADecoder;
						//	}
						//	else if (sub_type == AUDIO_AC3)
						//	{
						//		m_pActiveAudDecoder = NULL;
						//	}
						//	else if (sub_type == AUDIO_MPEG2_AAC)
						//	{
						//		m_pActiveAudDecoder = &m_AACDecoder;
						//	}

						//	if (m_pActiveAudDecoder != NULL)
						//	{
						//		m_pActiveAudDecoder->SetTrigger(ES_TRIGGER_ONESHOT);
						//		m_pActiveAudDecoder->SetParams(nPID, 0xFFFF);
						//		m_pActiveAudDecoder->Open(this->GetSafeHwnd(), STREAM_TS, NULL);
						//		TSMagic_threadparams_regist_esdecoder(&(pTSMagicView->m_kThreadParams), 0, m_pActiveAudDecoder);
						//	}

						//}
						//else
						//{
						//}

						//if ((m_pActiveVidDecoder != NULL) || (m_pActiveAudDecoder != NULL))
						//{
						//	pTSMagicView->m_kThreadParams.es_trigger_thread_running = 0;
						//	pTSMagicView->m_kThreadParams.es_trigger_thread_stopped = 0;
						//	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_es_trigger_thread, (LPVOID)&(pTSMagicView->m_kThreadParams), 0, 0);
						//}
					}
				}
			}
		}
	}

	*pResult = 0;
}


void CDlg_TSAnalyzer_Es::OnRclickTreeTsPrograms(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
