// Dlg_TS_WatchAV.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_PesEs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdint.h>
#include <assert.h>

#include "..\resource.h"

#include "TSMagicView.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PesEs dialog

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "MiddleWare/MiddleWare_ESDecoder/Include\ESDecoder_segment_tools.h"

#include "translate_layer/MPEG2_DVB_Section\Include\Mpeg2_StreamType.h"
#include "translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_Utilities.h"
#include "syntax_express_xml/XML_MPEG_PESPacket/Include/MPEG_PES_Packet_xml.h"

//#include "libs_ES\ES_MPV\Include\mpeg_video_sequence.h"
#include "translate_layer\ES_MPA\Include\mpeg_audio_errorcode.h"

#include "syntax_express_xml\XML_ES_MPV\Include\mpeg_video_sequence_xml.h"

//#include "libs_ES\ES_AVS\Include\avs_video_sequence.h"
#include "syntax_express_xml\XML_ES_AVS\Include\avs_video_sequence_xml.h"

#include "syntax_express_xml\XML_ES_H264\Include\h264_video_sequence_xml.h"
#include "syntax_express_xml\XML_ES_MPA\Include\mpeg_audio_sequence_xml.h"
#include "syntax_express_xml\XML_ES_AC3\Include\ac3_audio_sequence_xml.h"
#include "syntax_express_xml\XML_ES_AAC\Include\aac_audio_sequence_xml.h"
#include "syntax_express_xml\XML_ES_DRA\Include\dra_audio_sequence_xml.h"

#include "toolbox_libs\TOOL_Directory\Include\TOOL_Directory.h"

CDlg_TSAnalyzer_PesEs::CDlg_TSAnalyzer_PesEs(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_PesEs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_PesEs)
	//}}AFX_DATA_INIT

	fp_debug = NULL;

	m_pTree = NULL;
	m_pList = NULL;
}

void CDlg_TSAnalyzer_PesEs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_PesEs)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_PesEs, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_PesEs)
	ON_WM_SIZE()
//	ON_BN_CLICKED(IDC_BTN_PESES_WATCH, OnBtnWatch)
//	ON_BN_CLICKED(IDC_BTN_PESES_DTS, OnBtnDts)
//	ON_BN_CLICKED(IDC_BTN_PESES_PTS, OnBtnPts)
//	ON_BN_CLICKED(IDC_BTN_PESES_TRIGGER, OnBtnTrigger)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PesEs message handlers

BOOL CDlg_TSAnalyzer_PesEs::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 3);
		m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CNaviTree_ESs), CSize(250, 0), NULL);
		m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CTreeView_PacketSyntax), CSize(400, 0), NULL);
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
		m_wndSplitter.CreateView(0, 2, RUNTIME_CLASS(CListView_ByteBuffer), CSize(100, 0), NULL);
#else
		m_wndSplitter.CreateView(0, 2, RUNTIME_CLASS(CHexEditView_ByteBuffer), CSize(100, 0), NULL);
#endif

		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 100;

		m_wndSplitter.MoveWindow(&rect);

		m_pPane = (CNaviTree_ESs*)m_wndSplitter.GetPane(0, 0);
		//m_pTree = (CTreeView_PesEsSyntax*)m_wndSplitter.GetPane(0, 1);
		m_pTree = (CTreeView_PacketSyntax*)m_wndSplitter.GetPane(0, 1);
		m_pTree->Init("PES/ES 语法分析");
		m_pTree->m_hNotifyParent = GetSafeHwnd();
#if BYTE_BUFFER_USE_LISTCTRL_VIEW
		m_pList = (CListView_ByteBuffer*)m_wndSplitter.GetPane(0, 2);
#else
		m_pList = (CHexEditView_ByteBuffer*)m_wndSplitter.GetPane(0, 2);
		m_pList->SetBPR(16);
#endif
	}
	//m_pActiveAudDecoder = NULL;
	//m_pActiveVidDecoder = NULL;

	m_dlgTV.Create(IDD_SHOW_VIDEO, this);
	m_dlgTV.ShowWindow(SW_HIDE);

	//m_dlgDtsPts.Create(IDD_TS_DTS_PTS, this);
	//m_dlgDtsPts.ShowWindow(SW_HIDE);

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_PesEs::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 50, 0);

		m_wndSplitter.SetColumnInfo(0, 400, 0);

		int n2ColWidth = cx - 400 - 10;

		int nHexViewWidth = 740;
		if (n2ColWidth < 1480)
		{
			nHexViewWidth = n2ColWidth / 2;
		}

		m_wndSplitter.SetColumnInfo(1, n2ColWidth - nHexViewWidth, 0);
		m_wndSplitter.SetColumnInfo(2, nHexViewWidth, 0);

		m_wndSplitter.RecalcLayout();
	}

	CWnd*	pWnd;
	CRect	rect;

	pWnd = GetDlgItem(IDC_BTN_PESES_PTS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_PESES_DTS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_PESES_BUFFER);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_PESES_WATCH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, cy - rect.Height() - 10, rect.Width(), rect.Height(), 0);
	}
}

void CDlg_TSAnalyzer_PesEs::Reset(void)
{
	m_pPane->Reset();
	m_pTree->Reset();
	m_pList->Reset();

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CWnd* pWnd;
//	char pszText[128];

	pWnd = GetDlgItem(IDC_BTN_PESES_PTS);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PESES_DTS);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PESES_BUFFER);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_PESES_WATCH);
	pWnd->EnableWindow(FALSE);

	m_dwVidCode = 0x00000000;
	m_dwAudCode = 0x00000000;

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


	//if (m_pActiveVidDecoder != NULL)
	//{
	//	if (pTSMagicView != NULL)
	//	{
	//		TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), 0);
	//	}

	//	m_pActiveVidDecoder->Close();
	//	m_pActiveVidDecoder = NULL;
	//}

	//if (m_pActiveAudDecoder != NULL)
	//{
	//	if (pTSMagicView != NULL)
	//	{
	//		TSMagic_threadparams_unregist_esdecoder(&(pTSMagicView->m_kThreadParams), 1);
	//	}
	//	
	//	m_pActiveAudDecoder->Close();
	//	m_pActiveAudDecoder = NULL;
	//}

}

//void CDlg_TSAnalyzer_PesEs::Set(int offline)
//{
//	CWnd* pWnd = GetDlgItem(IDC_BTN_PESES_WATCH);
//	pWnd->EnableWindow(TRUE);
//
////	pWnd = GetDlgItem(IDC_BTN_PTS);
////	pWnd->EnableWindow(TRUE);
//
////	pWnd = GetDlgItem(IDC_BTN_DTS);
////	pWnd->EnableWindow(TRUE);
//
////	pWnd = GetDlgItem(IDC_BTN_TRIGGER);
////	pWnd->EnableWindow(TRUE);
//
//	if (fp_debug == NULL)
//	{
////		fp_debug = fopen("e:\\dts_pts.txt", "wt");
//	}
//}

void CDlg_TSAnalyzer_PesEs::DisplayPESPacket(uint32_t uiPESStyle, uint8_t* pes_buf, int pes_size, TALForXMLDoc* pxmlFatherDoc)
{
	uint8_t sub_type = (uiPESStyle & 0x000000ff);
	uint8_t stream_type = (uiPESStyle & 0x0000ff00) >> 8;
	uint16_t PID = (uiPESStyle & 0x1fff0000) >> 16;
	uint8_t class_type = (uiPESStyle & 0xE0000000) >> 29;

	if ((m_pList != NULL) && (m_pTree != NULL))
	{
		TALForXMLDoc xml2Doc;

		PES_packet_t PES_packet;
		MPEG_decode_PES_packet_to_xml(pes_buf, pes_size, &xml2Doc, &PES_packet);

		uint8_t* es_buf = PES_packet.payload_buf;
		int es_length = PES_packet.payload_length;

		int data_alignment_indicator = PES_packet.data_alignment_indicator;
		//tinyxml2::XMLElement* pxmlDataAlignmentIndicator = xmlDoc.QueryNodeByName(xmlDoc.RootElement(), "data_alignment_indicator");
		//if (pxmlDataAlignmentIndicator != NULL)
		//{
		//	data_alignment_indicator = pxmlDataAlignmentIndicator->IntAttribute("value");
		//}

		XMLElement* pxmlPayloadNode = xml2Doc.QueryNodeByKeyName(xml2Doc.RootElement(), "PAYLOAD()");
		if (pxmlPayloadNode != NULL)
		{
			if (class_type == TSPAYLOAD_CLASS_PES_VIDEO)
			{
				if ((stream_type == 0x01) || (stream_type == 0x02))
				{
					DisplayMPVPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
				}
				////else if (stream_type == 0x10)
				////{
				////	//MP4 视频
				////	DisplayMP4VPacket(es_buf, es_length);
				////}
				//else if (stream_type == 0x1B)
				//{
				//	DisplayH264Packet(es_buf, es_length, &xmlDoc, pxmlPayloadNode);
				//}
				//else if (stream_type == 0x42)
				//{
				//	DisplayAVSPacket(es_buf, es_length, &xmlDoc, pxmlPayloadNode);
				//}
				else
				{
					DisplayUnknownESPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
				}
			}
			else if (class_type == TSPAYLOAD_CLASS_PES_AUDIO)
			{
				if ((stream_type == 0x03) || (stream_type == 0x04))
				{
					REF_SYNC_t ref_sync;
					ref_sync.syncword = 0x0000;
					ref_sync.mask = 0xffff;
					ref_sync.length = -1;
					if (data_alignment_indicator)		//if the paload of PES packet is data aligned, then we can use the first two bytes as sync word
					{
						ref_sync.syncword = (es_buf[0] << 8) | es_buf[1];
						ref_sync.mask = 0xffff;
						ref_sync.length = 2;
					}
					DisplayMPAPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode, &ref_sync);
				}
				else if ((stream_type == 0x0F) || (stream_type == 0x11))
				{
					DisplayAACPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
				}
				else if ((stream_type == 0x06) || (stream_type == 0x81))
				{
					if (sub_type == STREAM_SUBTYPE_AC3)
					{
						DisplayAC3Packet(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
					}
					//else if (sub_type == STREAM_SUBTYPE_DRA)
					//{
					//	DisplayDRAPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
					//}
					else
					{
						DisplayUnknownESPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
					}
				}
				else
				{
					DisplayUnknownESPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
				}
			}
			else
			{
				DisplayUnknownESPacket(es_buf, es_length, &xml2Doc, pxmlPayloadNode);
			}

			pxmlPayloadNode->SetAttribute("display_options", "expanded");
		}

		m_pTree->Reset();
		m_pTree->ShowXMLDoc(&xml2Doc);

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

		sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\PES_packet_0x%04X.xml", pszXmlDir, PID);
		xml2Doc.SaveFile(pszFilePath);
#endif

		m_pList->Reset();
		m_pList->DisplayByteBuffer(pes_buf, pes_size);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayMPVPacket(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int			 align_offset = -1;
	ES_segment_t segment;
	uint8_t			 start_code;
	uint8_t			 slices_start_code = 0x01;
	int			 slices_length = 0;
	uint8_t*     slices_start_ptr = NULL;
	uint8_t			 extension_start_code_identifier;

	uint8_t*			rd_ptr;
	int			remain_length;

	char	pszComment[32];
	char	pszField[32];

	if ((es_buf != NULL) && (es_size > 0) && (pxmlDoc != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "MPEG视频");
		sprintf_s(pszComment, sizeof(pszComment), "MPEG视频");

		XMLElement* pxmlSlicesNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "MPGV");
		segment.sync_word = 0x00000100;
		segment.match_bytes = 4;
		segment.min_length = 4;
		segment.match_mask = 0xFFFFFF00;

		rd_ptr = es_buf;
		remain_length = es_size;
		do
		{
			//uint32_t debug_start_code = rd_ptr[0];
			//debug_start_code <<= 8;
			//debug_start_code |= rd_ptr[1];
			//debug_start_code <<= 8;
			//debug_start_code |= rd_ptr[2];
			//debug_start_code <<= 8;
			//debug_start_code |= rd_ptr[3];

			//if (debug_start_code == 0x000001B8)
			//{
			//	debug_start_code = debug_start_code;
			//}

			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, remain_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)		//发生在找第一个对齐的时候，头部未对齐数据的处理
				{
					mpgv_present_unaligned_nal_to_xml(rd_ptr, align_offset, pxmlDoc, pxmlPESPayloadNode);
				}

				rd_ptr += align_offset;
				remain_length -= align_offset;

				assert(rd_ptr == segment.nal_buf);
				assert(remain_length >= segment.nal_length);

				start_code = segment.nal_buf[3];
				switch (start_code)
				{
				case MPGV_USER_DATA_START_CODE:
					mpgv_decode_user_data_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
					break;

				case MPGV_SEQUENCE_HEADER_CODE:
					mpgv_decode_sequence_header_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
					break;

				case MPGV_EXTENSION_START_CODE:
					extension_start_code_identifier = (segment.nal_buf[4] & 0xf0) >> 4;

					switch (extension_start_code_identifier)
					{
					case MPGV_SEQUENCE_EXTENSION_ID:

						mpgv_decode_sequence_extension_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
						break;

				//	case MPGV_SEQUENCE_DISPLAY_EXTENSION_ID:

				//		mpgv_decode_sequence_display_extension_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
				//		break;

					//case QUANT_MATRIX_EXTENSION_ID:

					//	mpgv_decode_quant_matrix_extension(nal_unit.rbsp_byte, nal_unit.NumBytesInRBSP);
					//	break;

					case MPGV_PICTURE_CODING_EXTENSION_ID:

						mpgv_decode_picture_coding_extension_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
						break;

					//case MPGV_SEQUENCE_SCALABLE_EXTENSION_ID:

					//	mpgv_decode_sequence_scalable_extension(nal_unit.rbsp_byte, nal_unit.NumBytesInRBSP);
					//	break;

					//case MPGV_PICTURE_DISPLAY_EXTENSION_ID:

					//	mpgv_decode_picture_display_extension(nal_unit.rbsp_byte, nal_unit.NumBytesInRBSP);
					//	break;

					//case MPGV_COPYRIGHT_EXTENSION_ID:

					//	mpgv_decode_copyright_extension(nal_unit.rbsp_byte, nal_unit.NumBytesInRBSP);
					//	break;

					//case MPGV_PICTURE_SPATIAL_SCALABLE_EXTENSION_ID:

					//	mpgv_decode_picture_spatial_scalable_extension(nal_unit.rbsp_byte, nal_unit.NumBytesInRBSP);
					//	break;

					//case MPGV_PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID:

					//	mpgv_decode_picture_temporal_scalable_extension(nal_unit.rbsp_byte, nal_unit.NumBytesInRBSP);
					//	break;

					default:
						mpgv_present_unknown_nal_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
						break;
					}

					break;

				case MPGV_GROUP_START_CODE:
					mpgv_decode_group_of_pictures_header_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
					break;

				case MPGV_PICTURE_START_CODE:
					mpgv_decode_picture_header_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
					slices_length = 0;
					pxmlSlicesNode = NULL;
					break;

				default:

					if ((start_code >= MPGV_SLICE_START_CODE_MIN) && (start_code <= MPGV_SLICE_START_CODE_MAX))
					{
						if (pxmlSlicesNode == NULL)
						{
							slices_start_ptr = segment.nal_buf;
							slices_start_code = start_code;

							slices_length = 0;

							pxmlSlicesNode = pxmlDoc->NewBranchElement(pxmlPESPayloadNode, "SLICES()");
						}
						slices_length += segment.nal_length;

						sprintf_s(pszComment, sizeof(pszComment), "vertical_postion 0x%02X~0x%02X", slices_start_code, start_code);
						pxmlSlicesNode->SetAttribute("comment", pszComment);
						pxmlSlicesNode->SetAttribute("length", slices_length);
						pxmlSlicesNode->SetAttribute("field_length", slices_length);

						mpgv_decode_slice_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlSlicesNode);
					}
					else
					{
						mpgv_present_unknown_nal_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
					}
					break;
				}

				//指向下一个segment的位置
				rd_ptr += segment.nal_length;
				remain_length -= segment.nal_length;
			}
		} while (align_offset >= 0);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayAVSPacket(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int			 align_offset;
	ES_segment_t segment;
	uint8_t			 start_code;
	uint8_t			 slices_start_code = 0x00;
	int			 slices_length = 0;
	uint8_t*     slices_start_ptr = NULL;

	uint8_t     *rd_ptr;
	int			remain_length;

	char	pszComment[32];
	char	pszField[32];

	if ((es_buf != NULL) && (es_size > 0) && (pxmlDoc != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "AVS视频");
		sprintf_s(pszComment, sizeof(pszComment), "AVS视频");

		XMLElement* pxmlSlicesNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
			pxmlDoc->InsertEndChild(pxmlPESPayloadNode);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "AVS");
		segment.sync_word = 0x00000100;
		segment.match_bytes = 4;
		segment.min_length = 4;
		segment.match_mask = 0xFFFFFF00;

		rd_ptr = es_buf;
		remain_length = es_size;

		AVS_global_param_t avs_param;
		memset(&avs_param, 0x00, sizeof(AVS_global_param_t));
		do
		{
			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, remain_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)
				{
					avs_present_unaligned_nal_to_xml(rd_ptr, align_offset, pxmlDoc, pxmlPESPayloadNode);
				}

				rd_ptr += align_offset;
				remain_length -= align_offset;

				assert(rd_ptr == segment.nal_buf);
				assert(remain_length >= segment.nal_length);

				start_code = segment.nal_buf[3];
				switch (start_code)
				{
				//case AVS_PB_PICTURE_START_CODE:
				//	avs_decode_pb_picture_header_to_xml(segment.nal_buf, segment.nal_length, &avs_param, pxmlDoc, pxmlPESPayloadNode);
				//	break;
				default:

					//if ((start_code >= AVS_SLICE_START_CODE_MIN) && (start_code <= AVS_SLICE_START_CODE_MAX))
					//{
					//	slices_length += segment.nal_length;
					//	if (pxmlSlicesNode == NULL)
					//	{
					//		slices_start_ptr = segment.nal_buf;
					//		slices_start_code = start_code;

					//		pxmlSlicesNode = pxmlDoc->NewKeyValuePairElement(pxmlPESPayloadNode, "SLICES()");
					//	}

					//	pxmlDoc->UpdateBufMark(pxmlSlicesNode, slices_start_ptr, slices_start_ptr + slices_length);

					//	sprintf_s(pszTemp, sizeof(pszTemp), "%d字节 - vertical_postion 0x%02X~0x%02X", slices_length, slices_start_code, start_code);
					//	pxmlSlicesNode->SetAttribute("comment", pszTemp);

					//	avs_decode_slice_to_xml(segment.nal_buf, segment.nal_length, &avs_param, pxmlDoc, pxmlSlicesNode);
					//}
					//else
					//{
						avs_present_unknown_nal_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
					//}

					break;
				}

				//指向下一个segment的位置
				rd_ptr += segment.nal_length;
				remain_length -= segment.nal_length;
			}
		} while (align_offset >= 0);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayH264Packet(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int			 align_offset;
	ES_segment_t segment;
	uint8_t			 start_code;

	uint8_t*			rd_ptr;
	int			left_length;

	char	pszComment[32];
	char	pszField[32];

	if ((es_buf != NULL) && (es_size > 0) && (pxmlDoc != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "H264视频");
		sprintf_s(pszComment, sizeof(pszComment), "H264视频");

		XMLElement* pxmlSlicesNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "H264");
		segment.sync_word = 0x00000100;
		segment.match_bytes = 4;
		segment.min_length = 4;
		segment.match_mask = 0xFFFFFF00;

		rd_ptr = es_buf;
		left_length = es_size;
		do
		{
			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, left_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)
				{
					h264_present_unaligned_nal_to_xml(rd_ptr, 0, align_offset, pxmlDoc, pxmlPESPayloadNode);
				}

				rd_ptr += align_offset;
				left_length -= align_offset;

				assert(rd_ptr == segment.segment_start_ptr);
				assert(left_length >= segment.segment_length);

				start_code = segment.nal_buf[0];

				uint8_t forbidden_zero_bit = (start_code & 0x80) >> 7;
				uint8_t nal_ref_idx = (start_code & 0x60) >> 5;
				uint8_t nal_unit_type = start_code & 0x1F;

				if (forbidden_zero_bit == 0)
				{
					switch (nal_unit_type)
					{
					default:

						//if ((start_code >= AVS_SLICE_START_CODE_MIN) && (start_code <= AVS_SLICE_START_CODE_MAX))
						//{
						//	if (hPictureItem == NULL)
						//	{
						//		slice_start_code = start_code;
						//		sprintf_s(pszItem, sizeof(pszItem), "00 00 01 %02X~00 00 01 %02X - <picture_data()>", slice_start_code, start_code);
						//		hPictureItem = treeCtrl.InsertItem(pszItem, m_hESItem);
						//	}
						//	else
						//	{
						//		sprintf_s(pszItem, sizeof(pszItem), "00 00 01 %02X~00 00 01 %02X - <picture_data()>", slice_start_code, start_code);
						//		treeCtrl.SetItemText(hPictureItem, pszItem);
						//	}
						//	//mpgv_decode_slice(segment.nal_buf, segment.nal_length, &slice);
						//	//if (hPictureItem != NULL)
						//	//{
						//	//	AVS_VIDEO_ES_SLICE(&treeCtrl, hPictureItem, &slice, segment.nal_buf, segment.nal_length);
						//	//}
						//}
						//else
						//{
						h264_present_unknown_nal_to_xml(segment.segment_start_ptr, segment.prefix_length, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
						//}

						break;
					}
				}
				else
				{

				}

				//指向下一个segment的位置
				rd_ptr += segment.segment_length;
				left_length -= segment.segment_length;
			}

		} while (align_offset >= 0);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayMPAPacket(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, REF_SYNC_t* pref_sync)
{
	int			 align_offset;
	ES_segment_t segment;

	uint8_t*	rd_ptr;
	int			remain_length;

	char	pszComment[32];
	char	pszField[32];

	MPA_frame_t	mpa_frame;

	if ((es_buf != NULL) && (es_size > 128) && (pxmlDoc != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "MPEG音频");
		sprintf_s(pszComment, sizeof(pszComment), "MPEG音频");

		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "MPGA");

		//setup the initial sync info.
		segment.sync_word = 0xFFF0;
		segment.match_bytes = 2;
		segment.match_mask = 0xFFF0;
		//segment.min_length = 128;				//为什么是128字节？？？
		segment.min_length = 144;				//为什么是144字节？？？

		if (pref_sync != NULL)
		{
			if (pref_sync->length > 0)
			{
				assert(pref_sync->length == 2);

				segment.sync_word = pref_sync->syncword;
				segment.match_bytes = pref_sync->length;
				segment.match_mask = pref_sync->mask;
			}
		}

		rd_ptr = es_buf;
		remain_length = es_size;
		do
		{
			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, remain_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)
				{
					mpga_present_unaligned_nal_to_xml(rd_ptr, align_offset, pxmlDoc, pxmlPESPayloadNode);

					//需要猜测同步字
				}

				rd_ptr += align_offset;
				remain_length -= align_offset;

				assert(rd_ptr == segment.nal_buf);
				assert(remain_length >= segment.nal_length);

				int rtcode = mpga_decode_frame(segment.nal_buf, segment.nal_length, &mpa_frame);
				if (rtcode == MPA_NO_ERROR)		//if parse success, we can save the sync word
				{
					mpga_present_frame_to_xml(pxmlDoc, pxmlPESPayloadNode, &mpa_frame);

					//save the sync word for this calling 
					uint32_t sync_word = (segment.nal_buf[0] << 8) | (segment.nal_buf[1] << 0);

					segment.sync_word = (sync_word & segment.match_mask);
					segment.match_mask = 0xFFF0;
					segment.match_bytes = 2;

					//uint32_t sync_word = (segment.nal_buf[0] << 24) | (segment.nal_buf[1] << 16) | (segment.nal_buf[2] << 8) | segment.nal_buf[3];

					//segment.match_mask = 0xFFFFFDFF;
					//segment.sync_word = (sync_word & segment.match_mask);
					//segment.match_bytes = 4;

					segment.min_length = 144;			//why reset to 144 bytes?

					//指向下一个segment的位置
					rd_ptr += segment.nal_length;
					remain_length -= segment.nal_length;
				}
				else if (rtcode == MPA_PARSE_LENGTH_ERROR)
				{
					if (remain_length > segment.nal_length)
					{
						//search again from current pos, but enlarge the search range
						segment.min_length = (segment.nal_length + 1);
					}
					else
					{
						mpga_present_frame_to_xml(pxmlDoc, pxmlPESPayloadNode, &mpa_frame);
						rd_ptr += segment.nal_length;
						remain_length -= segment.nal_length;
					}
				}
				else
				{
					mpga_present_unknown_nal_to_xml(rd_ptr, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);

					//指向下一个segment的位置
					rd_ptr += segment.nal_length;
					remain_length -= segment.nal_length;
				}
			}
			else
			{
				//failed in finding syncword
				if (remain_length > 0)
				{
					mpga_present_unaligned_nal_to_xml(rd_ptr, remain_length, pxmlDoc, pxmlPESPayloadNode);
				}
				break;
			}
		} while ((align_offset >= 0) && (remain_length > 0));
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayAACPacket(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int			 align_offset;
	ES_segment_t segment;

	uint8_t*	rd_ptr;
	int			left_length;

	char	pszComment[32];
	char	pszField[32];

	//AAC_adts_fixed_header_t		aac_adts_fixed_header;

	if ((es_buf != NULL) && (es_size > 0) && (pxmlDoc != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "AAC音频");
		sprintf_s(pszComment, sizeof(pszComment), "AAC音频");

		//XMLElement* pxmlSlicesNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "AAC");
		segment.sync_word = 0xFFF0;
		segment.match_bytes = 2;
		segment.min_length = 128;			//为什么是128个字节？？？？
		segment.match_mask = 0xFFF0;

		rd_ptr = es_buf;
		left_length = es_size;
		do
		{
			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, left_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)
				{
					aac_present_unaligned_nal_to_xml(rd_ptr, align_offset, pxmlDoc, pxmlPESPayloadNode);
				}

				rd_ptr += align_offset;
				left_length -= align_offset;

				assert(rd_ptr == segment.nal_buf);
				assert(left_length >= segment.nal_length);

				aac_present_unknown_nal_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);

				//指向下一个segment的位置
				rd_ptr += segment.nal_length;
				left_length -= segment.nal_length;
			}

		} while (align_offset >= 0);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayAC3Packet(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int			 align_offset;
	ES_segment_t segment;

	uint8_t*			rd_ptr;
	int			left_length;

	char	pszComment[32];
	char	pszField[32];

	//AC3_syncinfo_t		AC3_syncinfo;

	if ((es_buf != NULL) && (es_size > 0))
	{
		sprintf_s(pszField, sizeof(pszField), "AC3音频");
		sprintf_s(pszComment, sizeof(pszComment), "AC3音频");

		XMLElement* pxmlNalNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "AC3");
		segment.sync_word = 0x0B77;
		segment.match_bytes = 2;
		segment.min_length = 5;
		segment.match_mask = 0xFFFF;

		rd_ptr = es_buf;
		left_length = es_size;
		do
		{
			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, left_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)
				{
					ac3_present_unaligned_nal_to_xml(rd_ptr, align_offset, pxmlDoc, pxmlPESPayloadNode);
				}

				rd_ptr += align_offset;
				left_length -= align_offset;

				assert(rd_ptr == segment.nal_buf);
				assert(left_length >= segment.nal_length);

				ac3_present_unknown_nal_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
				//ac3_decode_syncframe_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);

				//指向下一个segment的位置
				rd_ptr += segment.nal_length;
				left_length -= segment.nal_length;
			}

		} while (align_offset >= 0);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayDRAPacket(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int			 align_offset;
	ES_segment_t segment;

	uint8_t*	rd_ptr;
	int			left_length;

	char	pszComment[32];
	char	pszField[32];

	if ((es_buf != NULL) && (es_size > 0))
	{
		sprintf_s(pszField, sizeof(pszField), "DRA音频");
		sprintf_s(pszComment, sizeof(pszComment), "DRA音频");

		XMLElement* pxmlNalNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField, NULL, es_size);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		memset(&segment, 0x00, sizeof(ES_segment_t));
		strcpy_s(segment.pszFourCC, sizeof(segment.pszFourCC), "DRA");
		segment.sync_word = 0x7FFF;
		segment.match_bytes = 2;
		segment.min_length = 5;
		segment.match_mask = 0xFFFF;

		rd_ptr = es_buf;
		left_length = es_size;
		do
		{
			segment.nal_buf = NULL;
			segment.nal_length = 0;
			align_offset = ES_get_next_segment(rd_ptr, left_length, &segment);		//返回同步字相对于当前指针的偏移量
			if (align_offset >= 0)
			{
				if (align_offset > 0)
				{
					dra_present_unaligned_nal_to_xml(rd_ptr, align_offset, pxmlDoc, pxmlPESPayloadNode);
				}

				rd_ptr += align_offset;
				left_length -= align_offset;

				assert(rd_ptr == segment.nal_buf);
				assert(left_length >= segment.nal_length);

				//dra_decode_frame_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);
				dra_present_unknown_nal_to_xml(segment.nal_buf, segment.nal_length, pxmlDoc, pxmlPESPayloadNode);

				//指向下一个segment的位置
				rd_ptr += segment.nal_length;
				left_length -= segment.nal_length;
			}

		} while (align_offset >= 0);
	}
}

void CDlg_TSAnalyzer_PesEs::DisplayUnknownESPacket(uint8_t* es_buf, int es_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	if ((es_buf != NULL) && (es_size > 0) && (pxmlDoc != NULL))
	{
		char	pszField[64];
		char	pszComment[64];

		sprintf_s(pszField, sizeof(pszField), "未知类型");
		sprintf_s(pszComment, sizeof(pszComment), "未知类型");

		XMLElement* pxmlNalNode = NULL;
		XMLElement* pxmlPESPayloadNode = pxmlParentNode;
		if (pxmlPESPayloadNode == NULL)
		{
			pxmlPESPayloadNode = pxmlDoc->NewRootElement(pszField);
		}
		else
		{
			pxmlPESPayloadNode->SetAttribute("comment", pszComment);
		}

		pxmlNalNode = pxmlDoc->NewElementForByteBuf(pxmlPESPayloadNode, "es_buf[ ]", es_buf, es_size, NULL);
	}
}

void CDlg_TSAnalyzer_PesEs::UpdatePAT(CPAT* pPAT)
{
	m_pPane->UpdatePAT(pPAT);
}

void CDlg_TSAnalyzer_PesEs::UpdatePMT(CPMT* pPMT)
{
	m_pPane->UpdatePMT(pPMT);
}

/*
void CDlg_TSAnalyzer_PesEs::OnBtnDts() 
{
	m_dlgDtsPts.Set(DTS, m_dwVidCode, m_dwAudCode);
	m_dlgDtsPts.ShowWindow(SW_SHOWNORMAL);
}

void CDlg_TSAnalyzer_PesEs::OnBtnPts() 
{
	m_dlgDtsPts.Set(PTS, m_dwVidCode, m_dwAudCode);
	m_dlgDtsPts.ShowWindow(SW_SHOWNORMAL);
}
*/
void CDlg_TSAnalyzer_PesEs::OnBtnWatch() 
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

		//m_dlgTV.RealTimeStream(nVidStreamType, m_pActiveVidDecoder, nAudStreamType, m_pActiveAudDecoder);
	}
}



BOOL CDlg_TSAnalyzer_PesEs::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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
						DWORD uiValue = (DWORD)(item.lParam);
						//char pszTemp[64];
						//sprintf_s(pszTemp, 64, "Parent: OnTvnSelchanged(), uiValue=0x%08X\0", uiValue);
						//AfxMessageBox(pszTemp);

						uint16_t offset = (uint16_t)((uiValue & 0xffff0000) >> 16);
						uint16_t length = (uint16_t)(uiValue & 0x0000ffff);

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
