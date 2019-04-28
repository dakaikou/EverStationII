#include "stdafx.h"
#include "SplitterWnd_PsiSiAnalyzer.h"

#include "syntax_translate_layer\MPEG2_DVB_Section\Include\Mpeg2_table_id.h"
#include "syntax_translate_layer\MPEG2_DVB_Section\Include\DVB_table_id.h"

#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\Mpeg2_DSMCC_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\Mpeg2_PSI_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\DVB_SI_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\DVB_IPDC_section_XML.h"
#include "syntax_express_xml\XML_MPEG2_DVB_Section\Include\DVB_MHP_section_XML.h"

//#include "Utilities\Directory\Include\TOOL_Directory.h"


CSplitterWnd_PsiSiAnalyzer::CSplitterWnd_PsiSiAnalyzer()
{
}


CSplitterWnd_PsiSiAnalyzer::~CSplitterWnd_PsiSiAnalyzer()
{
}

BEGIN_MESSAGE_MAP(CSplitterWnd_PsiSiAnalyzer, CSplitterWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CSplitterWnd_PsiSiAnalyzer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSplitterWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//CreateView(0, 0, RUNTIME_CLASS(CNaviTree_PsiSiTables), CSize(100, 0), NULL);
	//CreateView(0, 1, RUNTIME_CLASS(CTreeView_PacketSyntax), CSize(100, 0), NULL);
	//CreateView(0, 2, RUNTIME_CLASS(CHexEditView_ByteBuffer), CSize(0, 0), NULL);

	return 0;
}

void CSplitterWnd_PsiSiAnalyzer::OnSize(UINT nType, int cx, int cy)
{
	CSplitterWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);
	//if (GetSafeHwnd() != NULL)
	//{

	//	SetColumnInfo(0, 500, 0);

	//	int n2ColWidth = cx - 500 - 10;

	//	int nHexViewWidth = 740;
	//	if (n2ColWidth < 1480)
	//	{
	//		nHexViewWidth = n2ColWidth / 2;
	//	}

	//	SetColumnInfo(1, n2ColWidth - nHexViewWidth, 0);
	//	SetColumnInfo(2, nHexViewWidth, 0);

	//	RecalcLayout();
	//}
}

void CSplitterWnd_PsiSiAnalyzer::Reset(void)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	CTreeView_PacketSyntax* pSyntaxTree = (CTreeView_PacketSyntax*)GetPane(0, 1);
	CHexEditView_ByteBuffer* pHexList = (CHexEditView_ByteBuffer*)GetPane(0, 2);
	pNaviTree->Reset();
	pSyntaxTree->Reset();
	pHexList->Reset();
}

//void CDlg_TSAnalyzer_PsiSi::Set(int offline)
//{
//	m_pNaviTree->Set(offline);
//	m_pSyntaxTree->Set(offline);
//	m_pHexList->Set(offline);
//}

void CSplitterWnd_PsiSiAnalyzer::DisplaySection(uint8_t* section_buf, int section_length)
{
	//if (m_pHexList != NULL)
	//{
	//	m_pHexList->Reset();
	//	m_pHexList->DisplayByteBuffer(section_buf, section_length);
	//}

//	if (m_pSyntaxTree != NULL)
//	{
//#ifdef _DEBUG
//		char	pszExeFile[MAX_PATH];
//		char	exeDrive[3];
//		char	pszXmlDir[MAX_PATH];
//		char	pszFilePath[MAX_PATH];
//		GetModuleFileName(NULL, pszExeFile, MAX_PATH);
//		exeDrive[0] = pszExeFile[0];
//		exeDrive[1] = pszExeFile[1];
//		exeDrive[2] = '\0';
//
//		sprintf_s(pszXmlDir, sizeof(pszXmlDir), "%s\\~EverStationII\\xml", exeDrive);
//		DIR_BuildDirectory(pszXmlDir);
//#endif
//		uint8_t	table_id;
//		if ((section_buf != NULL) && (section_length >= 3))
//		{
//			TALForXMLDoc xml2Doc;
//
//			table_id = section_buf[0];
//
//			if (table_id == TABLE_ID_PAT)
//			{
//				MPEG2_PSI_PAT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\PAT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_PMT)
//			{
//				MPEG2_PSI_PMT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\PMT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_CAT)
//			{
//				MPEG2_PSI_CAT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\CAT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_NIT_ACTUAL || table_id == TABLE_ID_NIT_OTHER)
//			{
//				DVB_SI_NIT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\NIT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_SDT_ACTUAL || table_id == TABLE_ID_SDT_OTHER)
//			{
//				DVB_SI_SDT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\SDT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_BAT)
//			{
//				DVB_SI_BAT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\BAT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if ((table_id >= TABLE_ID_EIT_PF_ACTUAL) && (table_id <= TABLE_ID_EIT_SCH_OTHER_F))
//			{
//				DVB_SI_EIT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\EIT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_TSDT)
//			{
//				MPEG2_PSI_TSDT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TSDT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_TDT)
//			{
//				DVB_SI_TDT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TDT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_TOT)
//			{
//				DVB_SI_TOT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\TOT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_RST)
//			{
//				DVB_SI_RST_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\RST-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if ((table_id >= TABLE_ID_ECM_MIN) && (table_id <= TABLE_ID_EMM_MAX))
//			{
//				DVB_SI_CMT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\CMT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_INT)
//			{
//				DVB_IPDC_INT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\INT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_AIT)
//			{
//				DVB_MHP_AIT_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\AIT-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_DSMCC_MPE)
//			{
//				DVB_IPDC_MPE_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\MPE-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_DSMCC_UNM)
//			{
//				MPEG2_DSMCC_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_UNM-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if (table_id == TABLE_ID_DSMCC_DDM)
//			{
//				MPEG2_DSMCC_DecodeSection_to_XML(section_buf, section_length, &xml2Doc);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_DDM-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//			else if ((table_id >= TABLE_ID_DSMCC_SD) && (table_id <= TABLE_ID_DSMCC_RSV))
//			{
//				//MPEG2_DSMCC_DecodeSection_to_XML(section_buf, section_length, &xmlDoc);
//				assert(0);
//
//#ifdef _DEBUG
//				sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_RSV-0x%02X.xml", pszXmlDir, table_id);
//#endif
//			}
//#ifdef _DEBUG
//			xml2Doc.SaveFile(pszFilePath);
//#endif
//			m_pSyntaxTree->Reset();
//			m_pSyntaxTree->ShowXMLDoc(&xml2Doc);
//		}
//	}
}

void CSplitterWnd_PsiSiAnalyzer::UpdatePAT(int section_number, CPAT* pPAT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdatePAT(section_number, pPAT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdatePMT(int section_number, CPMT* pPMT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdatePMT(section_number, pPMT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateBAT(int section_number, CBAT* pBAT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateBAT(section_number, pBAT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateSDT(int section_number, CSDT* pSDT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateSDT(section_number, pSDT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateNIT(int section_number, CNIT* pNIT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	//pNaviTree->UpdateNIT(section_number, pNIT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateAIT(int section_number, CAIT* pAIT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateAIT(section_number, pAIT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateINT(int section_number, CINT* pINT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateINT(section_number, pINT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateEIT(int section_number, CEIT* pEIT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateEIT(section_number, pEIT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateDSMCC(int section_number, CPVT* pPVT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateDSMCC(section_number, pPVT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateMPE(int section_number, CMPE* pMPE)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateMPE(section_number, pMPE);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateCAT(int section_number, CCAT* pCAT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateCAT(section_number, pCAT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateTSDT(int section_number, CTSDT* pTSDT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateTSDT(section_number, pTSDT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateTDT(CTDT* pTDT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateTDT(pTDT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateTOT(CTOT* pTOT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateTOT(pTOT);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateRST(CRST* pRST)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateRST(pRST);
}

void CSplitterWnd_PsiSiAnalyzer::UpdateCMT(CCMT* pCMT)
{
	CNaviTree_PsiSiTables* pNaviTree = (CNaviTree_PsiSiTables*)GetPane(0, 0);
	pNaviTree->UpdateCMT(pCMT);
}

//BOOL CSplitterWnd_PsiSiAnalyzer::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	NMHDR* nmhdr = (LPNMHDR)lParam;
//
//	if (m_pSyntaxTree != NULL)
//	{
//		if (nmhdr->hwndFrom == m_pSyntaxTree->GetSafeHwnd())
//		{
//			if (nmhdr->code == TVN_SELCHANGED)
//			{
//				LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(nmhdr);
//
//				TVITEM item = pNMTreeView->itemNew;
//				HTREEITEM hSelItem = item.hItem;
//
//				if (hSelItem != NULL)
//				{
//					if (item.mask & TVIF_PARAM)
//					{
//						DWORD uiValue = (DWORD)(item.lParam);
//						//char pszTemp[64];
//						//sprintf_s(pszTemp, 64, "Parent: OnTvnSelchanged(), uiValue=0x%08X\0", uiValue);
//						//AfxMessageBox(pszTemp);
//
//						short offset = (short)((uiValue & 0xffff0000) >> 16);
//						short length = (short)(uiValue & 0x0000ffff);
//
//						if ((offset >= 0) && (length > 0))
//						{
//							m_pHexList->SetSel(offset, offset + length);
//						}
//						else
//						{
//							m_pHexList->SetSel(-1, -1);
//						}
//					}
//				}
//
//				return false;	////如果不返回，会将TVN_SELECTED消息反射回m_pSyntaxTree控件，然后又弹射回来，如此反复，无限循环
//			}
//		}
//	}
//
//	return CDialog::OnNotify(wParam, lParam, pResult);
//}
