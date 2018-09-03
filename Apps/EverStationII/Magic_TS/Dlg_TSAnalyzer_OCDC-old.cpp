// Dlg_TS_WatchOCDC.cpp : implementation file
//

#include "stdafx.h"

#include <assert.h>
#include <math.h>

#include "Dlg_TSAnalyzer_OCDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\Magic_TS\TSMagic_GuiApi_MSG.h"
#include "..\Magic_TS\TSMagic_Decimate_Dsmcc.h"
#include "..\resource.h"

#include "TSMagicView.h"

#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_PSI_Utilities.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_DSMCC_Utilities.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_SI_Utilities.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_OCDC dialog

#include "..\Common\GuiCommon_TreeFunction.h"
#include ".\Dlg_TSAnalyzer_OCDC.h"

CDlg_TSAnalyzer_OCDC::CDlg_TSAnalyzer_OCDC(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_OCDC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_OCDC)
	//}}AFX_DATA_INIT

	fp_debug = NULL;
}

void CDlg_TSAnalyzer_OCDC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_OCDC)
	DDX_Control(pDX, IDC_LIST_OCDC_SERVICES, m_listOCDCServices);
	DDX_Control(pDX, IDC_TREE_OCDC_SYNTAX, m_treeOCDCSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_OCDC, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_OCDC)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OCDC_SERVICES, OnDblclkListEsPid)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_PMT, OnUpdatePMT)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OCDC_SERVICES, OnNMRclickListOcdcServices)
	ON_COMMAND(ID_OCDC_DOWNLOAD, OnDownloadAllModule)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_OCDC message handlers

BOOL CDlg_TSAnalyzer_OCDC::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_ImageList.Create(IDB_TREE_INFO, 20, 1, RGB(255, 255, 255));
//	m_treeOCDCSyntax.SetImageList(&m_ImageList, TVSIL_NORMAL);

	InitList();
	InitTree();

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_OCDC::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	CRect	rect;

	pWnd = GetDlgItem(IDC_LIST_OCDC_SERVICES);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 10, 0);

		CRect rectList;
		m_listOCDCServices.GetWindowRect(&rectList);
		int listwidth = rectList.Width();

		CHeaderCtrl* pHeaderCtrl = m_listOCDCServices.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			listwidth -= m_listOCDCServices.GetColumnWidth(i);
			listwidth -= 1;
		}
		m_listOCDCServices.SetColumnWidth(columns - 1, listwidth);
	}

	pWnd = GetDlgItem(IDC_TREE_OCDC_SYNTAX);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 10, cy - rect.top - 10, 0);
	}
}

void CDlg_TSAnalyzer_OCDC::Reset(void)
{
	int			i;

	CTSMagicView* pView = CTSMagicView::GetView();

	m_listOCDCServices.DeleteAllItems();
	m_treeOCDCSyntax.DeleteAllItems();

	for (i = 0; i < MAX_SERVICES_PER_STREAM; i++)
	{
		m_usPID_in_OCDCArray[i] = 0x0000;
		m_usServiceID_in_OCDCArray[i] = 0x0000;
		m_ucComponentTag_in_OCDCArray[i] = 0xFF;
		m_usCarouselTypeID_in_OCDCArray[i] = 0xFFFF;
		m_uiTransactionID_in_OCDCArray[i] = 0xFFFFFFFF;
		m_ucComponentTag_in_OCDCArray[i] = 0;
	}

	if (fp_debug != NULL)
	{
		fclose(fp_debug);
		fp_debug = NULL;
	}
}

void CDlg_TSAnalyzer_OCDC::Set(int offline)
{
	if (fp_debug == NULL)
	{
//		fp_debug = fopen("e:\\dts_pts.txt", "wt");
	}
}

void CDlg_TSAnalyzer_OCDC::InitList(void)
{
	int i;
	int	nColWidth[] = {20, 80, 80, 100, 100, 100, 120, 160, 100};
	CString str[] = {"", "业务ID", "TS_PID", "组件标签", "关联标签", "交易ID", "轮播类型", "轮播层次", "轮播ID"};

	UINT	uiAlign[] = {LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER};

//	m_ESImageList.Create(IDB_ES, 20, 1, RGB(0xab,0xcd,0xef));
//	m_listOCDCServices.SetImageList(&m_ESImageList, LVSIL_SMALL);

	m_listOCDCServices.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (i = 0; i < sizeof(nColWidth) / sizeof(int); i++)
	{
		m_listOCDCServices.InsertColumn(i, str[i], uiAlign[i], nColWidth[i], -1);
	}
}

void CDlg_TSAnalyzer_OCDC::OnDblclkListEsPid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	char				pszText[MAX_TXT_CHARS];
//	int					service_id;

	int					PID = 0xFFFF;
	int					data_broadcast_id = 0x0006;
	unsigned int		transaction_id = 0x00000000;
	int					carousel_type_id = 0x0002;

	int nSel = m_listOCDCServices.GetSelectionMark();
	if (nSel >= 0)
	{
		m_treeOCDCSyntax.DeleteAllItems();

		PID = m_usPID_in_OCDCArray[nSel];

		data_broadcast_id = m_usDataBroadcastID_in_OCDCArray[nSel];

		if (m_uiTransactionID_in_OCDCArray[nSel] != 0xFFFFFFFF)
		{
			transaction_id = m_uiTransactionID_in_OCDCArray[nSel];
		}

		if (m_usCarouselTypeID_in_OCDCArray[nSel] != 0xFFFF)
		{
			carousel_type_id = m_usCarouselTypeID_in_OCDCArray[nSel];
		}

		sprintf_s(pszText, sizeof(pszText), "业务ID=0x%04X(%d)<-->TS_PID=0x%04X(%d)", m_usServiceID_in_OCDCArray[nSel], m_usServiceID_in_OCDCArray[nSel], PID, PID);
		m_hRootItem = TreeFun_InsertItem(&m_treeOCDCSyntax, TVI_ROOT, pszText, -1, 0, NULL);

		if (data_broadcast_id == 0x0006)				//DC
		{
			DC_GatherInfo(PID, (transaction_id & 0x0000ffff), carousel_type_id);
		}
		else											//OC
		{
			OC_GatherInfo(PID, (transaction_id & 0x0000ffff));
		}
	}

	*pResult = 0;
}

LRESULT CDlg_TSAnalyzer_OCDC::OnUpdatePMT(WPARAM wParam, LPARAM lParam)
{
	CPMT*			pPMT = (CPMT*)wParam;
	int				program_index = (int)lParam;
	int				nCount;
	int				i, j;
	int				PID;
	char			pszText[MAX_TXT_CHARS];
	CString			strTemp;
	int				dsmcc_index;
//	CSDT*			pSDT;
//	int				stream_index;
//	int				service_index;
//	DWORD			dwData;
	ES_INFO_t*		pEsInfo;
	int				bfound;
	int				insert_index;
	CDSMCC*			pDSMCC;

//	object_carousel_info_t*	pobject_carousel_info;
//	data_carousel_info_t*	pdata_carousel_info;

	if (pPMT != NULL)
	{
		for (i = 0; i < pPMT->m_nEsCount; i++)
		{
			pEsInfo = pPMT->m_astEsInfo + i;

			//0x0B = DSM-CC U-N Messages（ISO/IEC 13818-6 type B)
			//0x0C = DSM-CC 流描述符(ISO/IEC 13818-6 type C)
			if ((pEsInfo->stream_type == 0x0B) || (pEsInfo->stream_type == 0x0C))			//DSM-CC
			{
				PID = pEsInfo->elementary_PID;

				nCount = m_listOCDCServices.GetItemCount();

				bfound = 0;
				for (j = 0; j < nCount; j++)
				{
					if (m_usPID_in_OCDCArray[j] == PID)
					{
						bfound = 1;
						break;
					}
				}
				
				if (bfound == 0)
				{
					insert_index = m_listOCDCServices.InsertItem(nCount, "");

					m_usPID_in_OCDCArray[insert_index] = PID;
					m_usServiceID_in_OCDCArray[insert_index] = pPMT->m_usProgramNumber;
					m_ucComponentTag_in_OCDCArray[insert_index] = pEsInfo->component_tag;
					m_usDataBroadcastID_in_OCDCArray[insert_index] = pEsInfo->data_broadcast_id;
//					m_usCarouselTypeID_in_OCDCArray[insert_index] = pEsInfo->carousel_type_id;
					m_uiTransactionID_in_OCDCArray[insert_index] = pEsInfo->transaction_id;

					sprintf_s(pszText, sizeof(pszText), "0x%04X", pPMT->m_usProgramNumber);				//业务号
					m_listOCDCServices.SetItemText(insert_index, 1, pszText);

					sprintf_s(pszText, sizeof(pszText), "0x%04X", pEsInfo->elementary_PID);				//PID
					m_listOCDCServices.SetItemText(insert_index, 2, pszText);

					if (pEsInfo->component_tag != 0xFF)
					{
						sprintf_s(pszText, sizeof(pszText), "0x%02X", pEsInfo->component_tag);					//组件标签
						m_listOCDCServices.SetItemText(insert_index, 3, pszText);
					}

					if (pEsInfo->association_tag != 0xFFFF)
					{
						sprintf_s(pszText, sizeof(pszText), "0x%04X", pEsInfo->association_tag);				//关联标签
						m_listOCDCServices.SetItemText(insert_index, 4, pszText);
					}

					if (pEsInfo->transaction_id != 0xFFFFFFFF)
					{
						sprintf_s(pszText, sizeof(pszText), "0x%08X", pEsInfo->transaction_id);				//交易ID
						m_listOCDCServices.SetItemText(insert_index, 5, pszText);
					}

//					sprintf_s(pszText, sizeof(pszText), "0x%04X", pEsInfo->data_broadcast_id);				//数据广播标识
//					m_listOCDCServices.SetItemText(insert_index, 6, pszText);

					if (pPMT->m_astEsInfo[i].data_broadcast_id != 0xFFFF)
					{
						DVB_SI_NumericCoding2Text_DataBroadcastID(pPMT->m_astEsInfo[i].data_broadcast_id, pszText, sizeof(pszText));
						m_listOCDCServices.SetItemText(insert_index, 6, pszText);
					}

					if (pEsInfo->carousel_id != 0xFFFFFFFF)
					{
						sprintf_s(pszText, sizeof(pszText), "0x%08X", pEsInfo->carousel_id);				//轮播ID
						m_listOCDCServices.SetItemText(insert_index, 8, pszText);
					}

#if DEBUG_DSMCC
					CTSMagicView* pView = CTSMagicView::GetView();
					CDB_PsiSiTables* pDB_PsiSiTables = pView->GetPsiSiTablesDBase();

					int dsmcc_count = pDB_PsiSiTables->GetDSMCCCount();

					for (dsmcc_index = 0; dsmcc_index < dsmcc_count; dsmcc_index ++)
					{
						pDSMCC = pDB_PsiSiTables->GetDSMCC(dsmcc_index);

						if (pDSMCC->m_usPID == PID)
						{
							m_listOCDCServices.SetItemText(insert_index, 0, "+");
							break;
						}
					}
#endif
				}
			}
		}
	}

	return 0;
}

void CDlg_TSAnalyzer_OCDC::UpdateSDT(CSDT* pSDT)
{
	int				service_index;
	int				nCount;
	int				nItem;
	int				broadcast_index;
//	int				j;
	char			pszText[MAX_TXT_CHARS];
	CString			strTemp;
//	DWORD			dwData;

	data_broadcast_descriptor_t*	pdata_broadcast_descriptor;
	object_carousel_info_t*			pobject_carousel_info;
	data_carousel_info_t*			pdata_carousel_info;

	if (pSDT != NULL)
	{
		if (pSDT->m_ucTableID == TABLE_ID_SDT_ACTUAL)
		{
			nCount = m_listOCDCServices.GetItemCount();
			for (nItem = 0; nItem < nCount; nItem++)
			{
				strTemp = m_listOCDCServices.GetItemText(nItem, 7);			//Carousel_id_type
				if (strTemp ==  "")
				{
					for (service_index = 0; service_index < pSDT->m_nServiceCount; service_index ++)
					{
						if (pSDT->m_astServiceInfo[service_index].service_id == m_usServiceID_in_OCDCArray[nItem])
						{
							for (broadcast_index = 0; broadcast_index < pSDT->m_astServiceInfo[service_index].data_broadcast_count; broadcast_index++)
							{
								pdata_broadcast_descriptor = &(pSDT->m_astServiceInfo[service_index].data_broadcast_descriptor[broadcast_index]);
								if (pdata_broadcast_descriptor->descriptor_tag == DVB_SI_DATA_BROADCAST_DESCRIPTOR)
								{
									if (pdata_broadcast_descriptor->component_tag == m_ucComponentTag_in_OCDCArray[nItem])
									{
										if (pdata_broadcast_descriptor->selector_length > 0)
										{
											switch (pdata_broadcast_descriptor->data_broadcast_id)
											{
											case 0x0006:

												pdata_carousel_info = &(pdata_broadcast_descriptor->selector_byte.data_carousel_info);

												DVB_SI_NumericCoding2Text_CarouselTypeID(pdata_carousel_info->carousel_type_id, pszText, sizeof(pszText));
												m_listOCDCServices.SetItemText(nItem, 7, pszText);

												m_usCarouselTypeID_in_OCDCArray[nItem] = pdata_carousel_info->carousel_type_id;

												strTemp = m_listOCDCServices.GetItemText(nItem, 5);			//transaction_id
												if (strTemp ==  "")
												{
													sprintf_s(pszText, sizeof(pszText), "0x%08X", pdata_carousel_info->transaction_id);
													m_listOCDCServices.SetItemText(nItem, 5, pszText);

													m_uiTransactionID_in_OCDCArray[nItem] = pdata_carousel_info->transaction_id;
												}

												break;

											case 0x0007:

												pobject_carousel_info = &(pdata_broadcast_descriptor->selector_byte.object_carousel_info);

												DVB_SI_NumericCoding2Text_CarouselTypeID(pobject_carousel_info->carousel_type_id, pszText, sizeof(pszText));
												m_listOCDCServices.SetItemText(nItem, 7, pszText);

												m_usCarouselTypeID_in_OCDCArray[nItem] = pobject_carousel_info->carousel_type_id;

												strTemp = m_listOCDCServices.GetItemText(nItem, 5);			//transaction_id
												if (strTemp ==  "")
												{
													sprintf_s(pszText, sizeof(pszText), "0x%08X", pobject_carousel_info->transaction_id);
													m_listOCDCServices.SetItemText(nItem, 5, pszText);

													m_uiTransactionID_in_OCDCArray[nItem] = pobject_carousel_info->transaction_id;
												}

												break;

											default:
												break;
											}

											break;
										}

										break;
									}
								}
							}
/*
							if (pSDT->m_astServiceInfo[service_index].service_descriptor.descriptor_tag == SI_SERVICE_DESCRIPTOR)
							{
//								sprintf_s(pszText, sizeof(pszText), "0x%02X", pSDT->m_astServiceInfo[service_index].service_descriptor.service_type);
//								m_listOCDCServices.SetItemText(j, 6, pszText);

								m_listOCDCServices.SetItemText(j, 7, pSDT->m_astServiceInfo[service_index].service_descriptor.service_name);
							}
*/
							break;
						}
					}
				}
			}
		}
	}
}

void CDlg_TSAnalyzer_OCDC::UpdateDSMCC(CDSMCC* pDSMCC)
{
	char			pszText[MAX_TXT_CHARS];
	int				nCount;
//	DWORD			dwData = (dsmcc_index << 16) | 0x0000;
//	int				pmt_index, es_index, service_index, stream_index;
//	CPMT*			pPMT;
//	CSDT*			pSDT;
	int				bfound;
	CString			strTemp;
//	int				insert_index;
	int				item_index;
//	int				i;
//	int				temp;
//	ES_INFO_t*		pEsInfo;

//	object_carousel_info_t*	pobject_carousel_info;
//	data_carousel_info_t*	pdata_carousel_info;

	if (pDSMCC != NULL)
	{
		nCount = m_listOCDCServices.GetItemCount();

		//首先检查一遍是否已经存在
		bfound = 0;
		for (item_index = 0; item_index < nCount; item_index ++)
		{
			if (pDSMCC->m_usPID == m_usPID_in_OCDCArray[item_index])
			{
				strcpy_s(pszText, sizeof(pszText), "+");
				m_listOCDCServices.SetItemText(item_index, 0, pszText);
	//			m_listOCDCServices.SetItemData(item_index, dwData);

				bfound = 1;
				break;
			}
		}
	}
/*
	if (bfound == 0)
	{
		//排序
		insert_index = nCount;
		for (item_index = 0; item_index < nCount; item_index ++)
		{
			if (PID < m_usPIDTable[item_index])
			{
				insert_index = item_index;

				//后面的需要搬移
				for (i = (nCount - 1); i >= insert_index; i--)
				{
					temp = m_usPIDTable[i];
					m_usPIDTable[i + 1] = temp;

					temp = m_usServiceIDTable[i];
					m_usServiceIDTable[i + 1] = temp;

					temp = m_ucComponentTagTable[i];
					m_ucComponentTagTable[i + 1] = temp;
				}
				break;
			}
		}

		m_usPIDTable[insert_index] = PID;

//		sprintf_s(pszText, sizeof(pszText), "%d", pid_index);
		strcpy_s(pszText, sizeof(pszText), "");
		m_listOCDCServices.InsertItem(insert_index, pszText);
		m_listOCDCServices.SetItemData(insert_index, dwData);

		sprintf_s(pszText, sizeof(pszText), "0x%04X", PID);
		m_listOCDCServices.SetItemText(insert_index, 1, pszText);

		strTemp = m_listOCDCServices.GetItemText(insert_index, 3);
		if (strTemp ==  "")
		{
			for (pmt_index = 0; pmt_index < PSISI_REPORT.program_count_in_pmt; pmt_index++)
			{
				pPMT = PSISI_REPORT.astPMT + pmt_index;

				bfound = 0;
				for (es_index = 0; es_index < pPMT->m_nEsCount; es_index++)
				{
					pEsInfo = pPMT->m_astEsInfo + es_index;
					if (pEsInfo->elementary_PID == PID)
					{
						m_ucComponentTagTable[insert_index] = pEsInfo->component_tag;

						if (pEsInfo->component_tag != 0xFF)
						{
							sprintf_s(pszText, sizeof(pszText), "0x%02X", pEsInfo->component_tag);
							m_listOCDCServices.SetItemText(insert_index, 2, pszText);
						}

						m_usServiceIDTable[insert_index] = pPMT->m_usProgramNumber;

						sprintf_s(pszText, sizeof(pszText), "0x%04X", pPMT->m_usProgramNumber);
						m_listOCDCServices.SetItemText(insert_index, 4, pszText);

						DVB_DataBroadcastIDLookup(pPMT->m_astEsInfo[es_index].data_broadcast_id, pszText);
						m_listOCDCServices.SetItemText(insert_index, 5, pszText);

						//根据program_number再查一下SDT中的信息
						for (stream_index = 0; stream_index < PSISI_REPORT.stream_count_in_sdt; stream_index ++)
						{
							pSDT = PSISI_REPORT.astSDT + stream_index;

							if (pSDT->m_ucTableID == TABLE_ID_SDT_ACTUAL)
							{
								for (service_index = 0; service_index < pSDT->m_nServiceCount; service_index ++)
								{
									if (pSDT->m_astServiceInfo[service_index].service_id == pPMT->m_usProgramNumber)
									{
										dwData = (pid_index << 16) | (stream_index << 8) | service_index;
										m_listOCDCServices.SetItemData(insert_index, dwData);

										if (pSDT->m_astServiceInfo[service_index].data_broadcast_descriptor.descriptor_tag == SI_DATA_BROADCAST_DESCRIPTOR)
										{
											switch (pSDT->m_astServiceInfo[service_index].data_broadcast_descriptor.data_broadcast_id)
											{
											case 0x0006:

												pdata_carousel_info = &(pSDT->m_astServiceInfo[service_index].data_broadcast_descriptor.selector_byte.data_carousel_info);

												CarouselTypeIdLookup(pdata_carousel_info->carousel_type_id, pszText);
												m_listOCDCServices.SetItemText(insert_index, 6, pszText);

												break;

											case 0x0007:

												pobject_carousel_info = &(pSDT->m_astServiceInfo[service_index].data_broadcast_descriptor.selector_byte.object_carousel_info);

												CarouselTypeIdLookup(pobject_carousel_info->Carousel_type_id, pszText);
												m_listOCDCServices.SetItemText(insert_index, 6, pszText);

												break;

											default:
												break;
											}
										}

										if (pSDT->m_astServiceInfo[service_index].service_descriptor.descriptor_tag == SI_SERVICE_DESCRIPTOR)
										{
//											sprintf_s(pszText, sizeof(pszText), "0x%02X", pSDT->m_astServiceInfo[service_index].service_descriptor.service_type);
//											m_listOCDCServices.SetItemText(insert_index, 6, pszText);

											m_listOCDCServices.SetItemText(insert_index, 7, pSDT->m_astServiceInfo[service_index].service_descriptor.service_name);
										}

										break;
									}
								}

								break;
							}
						}

						bfound = 1;
						break;
					}
				}

				if (bfound)
				{
					break;
				}
			}
		}
	}
*/
}

void CDlg_TSAnalyzer_OCDC::InitTree(void)
{
	TreeFun_InitTree(&m_treeOCDCSyntax, this, IDC_TREE_OCDC_SYNTAX);
}

void CDlg_TSAnalyzer_OCDC::DC_GatherInfo(int PID, int table_id_extension, int carousel_type_id)
{
#if DEBUG_DSMCC
	CDSMCC*			pDSMCC_DSI;
	CDSMCC*			pDSMCC_DII;
	DSMCC_DSI_t*	pDSI;
	DSMCC_DII_t*	pDII;

	DC_moduleInfo_t*	pDC_moduleInfo;

	int				dsi_table_index;
	int				dii_table_index;
	char			pszText[MAX_TXT_CHARS];
	int				group_index;
	int				module_index;
	int				blockCount;

	TREEITEM	hDsiItem;
	TREEITEM	hChildItem;
	TREEITEM	hGroupItem;
	TREEITEM	hModuleItem;


	if (carousel_type_id == 0x02)			//2层数据轮播
	{
		CTSMagicView* pView = CTSMagicView::GetView();
		CDB_PsiSiTables* pDB_PsiSiTables = pView->GetPsiSiTablesDBase();

		int dsmcc_count = pDB_PsiSiTables->GetDSMCCCount();
		for (dsi_table_index = 0; dsi_table_index < dsmcc_count; dsi_table_index ++)
		{
			pDSMCC_DSI = pDB_PsiSiTables->GetDSMCC(dsi_table_index);

			if ((pDSMCC_DSI->m_usPID == PID) &&
				(pDSMCC_DSI->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
				(pDSMCC_DSI->m_usTableIDExtension == table_id_extension))
			{
				if (pDSMCC_DSI->m_usMessageId == 0x1006)			//DSI
				{
					pDSI = &(pDSMCC_DSI->m_DSI);

					hDsiItem = TreeFun_InsertItem(&m_treeOCDCSyntax, m_hRootItem, "SuperGroup()", -1, 0, NULL);

					hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDsiItem, "NumberOfGroups", 16, pDSI->NumberOfGroups, NULL);

					for (group_index = 0; group_index < pDSI->NumberOfGroups; group_index++)
					{
						sprintf_s(pszText, sizeof(pszText), "GROUP(%d)", group_index);
						hGroupItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDsiItem, pszText, -1, 0, NULL);

						hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "GroupId", 32, pDSI->astGroupInfo[group_index].GroupId, NULL);
						hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "GroupSize", 32, pDSI->astGroupInfo[group_index].GroupSize, NULL);
						hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "GroupName", -1, 0, pDSI->astGroupInfo[group_index].name_descriptor.text_char);

						for (dii_table_index = 0; dii_table_index < dsmcc_count; dii_table_index ++)
						{
							if (dii_table_index != dsi_table_index)
							{
								pDSMCC_DII = pDB_PsiSiTables->GetDSMCC(dii_table_index);

								if ((pDSMCC_DII->m_usPID == PID) &&
									(pDSMCC_DII->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
									(pDSMCC_DII->m_usTableIDExtension == (pDSI->astGroupInfo[group_index].GroupId & 0x0000ffff)))
								{
									pDII = &(pDSMCC_DII->m_DII);

									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "downloadId", 32, pDII->downloadId, NULL);
									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "blockSize", 16, pDII->blockSize, NULL);
									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "windowSize", 8, pDII->windowSize, NULL);
									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "ackPeriod", 8, pDII->ackPeriod, NULL);
									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "tCDownloadWindow", 32, pDII->tCDownloadWindow, NULL);
									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "tCDownloadScenario", 32, pDII->tCDownloadScenario, NULL);

									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, "numberOfModules", 16, pDII->numberOfModules, NULL);

									for (module_index = 0; module_index < pDII->numberOfModules; module_index ++)
									{
										sprintf_s(pszText, sizeof(pszText), "MODULE(%d - 0x%04X)", module_index, pDII->astModuleInfo[module_index].moduleId);
										hModuleItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hGroupItem, pszText, -1, 0, NULL);

										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleId", 16, pDII->astModuleInfo[module_index].moduleId, NULL);
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleSize", 32, pDII->astModuleInfo[module_index].moduleSize, NULL);
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleVersion", 8, pDII->astModuleInfo[module_index].moduleVersion, NULL);

										//计算参数
										blockCount = (S32)(ceil((double)pDII->astModuleInfo[module_index].moduleSize / pDII->blockSize));
										sprintf_s(pszText, sizeof(pszText), "%d", blockCount);
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "blockCount", -1, 0, pszText);

										pDC_moduleInfo = &(pDII->astModuleInfo[module_index].u.DC_moduleInfo);
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleName", -1, 0, pDC_moduleInfo->name_descriptor.text_char);
									}

									break;
								}
							}
						}

						m_treeOCDCSyntax.Expand(hGroupItem, TVE_EXPAND);
					}

					m_treeOCDCSyntax.Expand(hDsiItem, TVE_EXPAND);
					break;
				}
			}
		}
	}

	m_treeOCDCSyntax.Expand(m_hRootItem, TVE_EXPAND);
#endif
}

void CDlg_TSAnalyzer_OCDC::OC_BuildDownloadTable(int PID, unsigned int downloadId, unsigned short moduleId, unsigned int objectKey_data, char* pszDir)
{
#if DEBUG_DSMCC
	CDSMCC*						pDSMCC_DDM;
	BIOP_DirectoryMessage_t*	pBIOP_DirectoryMessage;
	BIOP_Bindings_t*			pBindings;
	BIOP_Name_t*				pBIOP_Name;
	BIOP_ObjectLocation_t*		pBIOP_ObjectLocation;

	int							ddm_table_index;
	int							binding_index;
	int							object_index;
	unsigned short				sub_moduleId;
	unsigned int				sub_objectKey_data;

	char						pszText[MAX_TXT_CHARS];
	char						pszPath[MAX_TXT_CHARS];

	//OC_DOWNLOAD_INFO_t*			pOCDownloadInfo;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();
	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();

	int dsmcc_count = pDB_PsiSiTables->GetDSMCCCount();
	for (ddm_table_index = 0; ddm_table_index < dsmcc_count; ddm_table_index ++)
	{
		pDSMCC_DDM = pDB_PsiSiTables->GetDSMCC(ddm_table_index);

		if ((pDSMCC_DDM->m_usPID == PID) &&
			(pDSMCC_DDM->m_ucTableID == TABLE_ID_DSM_CC_DDM) &&
			(pDSMCC_DDM->m_usTableIDExtension == moduleId))
		{
			for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index++)
			{
				pBIOP_DirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];
				if (pBIOP_DirectoryMessage != NULL)
				{
					if (pBIOP_DirectoryMessage->objectKey_data == objectKey_data)
					{
						for (binding_index = 0; binding_index < pBIOP_DirectoryMessage->bindings_count; binding_index ++)
						{
							pBindings = pBIOP_DirectoryMessage->bindings + binding_index;
							pBIOP_Name = &(pBindings->BIOP_Name);
							pBIOP_ObjectLocation = &(pBindings->IOP_IOR.IOP_taggedProfile[0].u.BIOPProfileBody.BIOP_ObjectLocation);

							sprintf_s(pszText, sizeof(pszText), "%s", pBIOP_Name->id_data_byte[0]);
							if (strcmp(pBindings->IOP_IOR.type_id_byte, "dir") == 0)
							{
								sprintf_s(pszPath, sizeof(pszPath), "%s%s\\", pszDir, pszText);

								sub_moduleId = pBIOP_ObjectLocation->moduleId;
								sub_objectKey_data = pBIOP_ObjectLocation->objectKey_data;

								OC_BuildDownloadTable(PID, downloadId, sub_moduleId, sub_objectKey_data, pszPath);
							}
							else
							{
								//PSISI_REPORT.oc_download_count ++;
								//PSISI_REPORT.astOCDownloadInfo = (OC_DOWNLOAD_INFO_t*)realloc(PSISI_REPORT.astOCDownloadInfo, PSISI_REPORT.oc_download_count * sizeof(OC_DOWNLOAD_INFO_t));

								//if (PSISI_REPORT.astOCDownloadInfo != NULL)
								//{
								//	PSISI_REPORT.memory_for_dsmcc_oc_download = PSISI_REPORT.oc_download_count * sizeof(OC_DOWNLOAD_INFO_t);

								//	pOCDownloadInfo = &(PSISI_REPORT.astOCDownloadInfo[PSISI_REPORT.oc_download_count - 1]);

								//	memset(pOCDownloadInfo, 0x00, sizeof(OC_DOWNLOAD_INFO_t));

								//	pBIOP_ObjectLocation = &(pBindings->IOP_IOR.IOP_taggedProfile[0].u.BIOPProfileBody.BIOP_ObjectLocation);

								//	sprintf_s(pOCDownloadInfo->pszFileName, sizeof(pOCDownloadInfo->pszFileName), "%s%s", pszDir, pszText);

								//	pOCDownloadInfo->PID = pDSMCC_DDM->m_usPID;
								//	pOCDownloadInfo->downloadId = pBIOP_ObjectLocation->carouselId;
								//	pOCDownloadInfo->objectKey_data = pBIOP_ObjectLocation->objectKey_data;
								//	pOCDownloadInfo->moduleId = pBIOP_ObjectLocation->moduleId;
								//}
								sprintf_s(pszPath, sizeof(pszPath), "%s%s", pszDir, pszText);

								pDB_OCDCs->AddOCDownloadInfo(
									pDSMCC_DDM->m_usPID,
									pBIOP_ObjectLocation->carouselId,
									pBIOP_ObjectLocation->objectKey_data,
									pBIOP_ObjectLocation->moduleId,
									pszPath
									);
							}
						}

						break;
					}
				}
			}

			break;
		}
	}
#endif
}

void CDlg_TSAnalyzer_OCDC::OC_BuildDirectory(int PID, HTREEITEM hParent, unsigned short moduleId, unsigned int objectKey_data)
{
	CDSMCC*						pDSMCC_DDM;
	BIOP_DirectoryMessage_t*	pBIOP_DirectoryMessage;
	BIOP_Bindings_t*			pBindings;
	BIOP_Name_t*				pBIOP_Name;
//	IOP_IOR_t*					pIOR;
//	BIOP_ProfileBody_t*			pBIOP_ProfileBody;
	BIOP_ObjectLocation_t*		pBIOP_ObjectLocation;
//	DSM_ConnBinder_t*			pDSM_ConnBinder;

	int							ddm_table_index;
	int							binding_index;
	int							object_index;
	unsigned short				sub_moduleId;
	unsigned int				sub_objectKey_data;

	char			pszText[MAX_TXT_CHARS];
	TV_INSERTSTRUCT TreeCtrlItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	TREEITEM		hChildItem;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();
	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();

	int dsmcc_count = pDB_PsiSiTables->GetDSMCCCount();
	for (ddm_table_index = 0; ddm_table_index < dsmcc_count; ddm_table_index ++)
	{
		pDSMCC_DDM = pDB_PsiSiTables->GetDSMCC(ddm_table_index);

		if ((pDSMCC_DDM->m_usPID == PID) &&
			(pDSMCC_DDM->m_ucTableID == TABLE_ID_DSM_CC_DDM) &&
			(pDSMCC_DDM->m_usTableIDExtension == moduleId))
		{
			for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index++)
			{
				pBIOP_DirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];
				if (pBIOP_DirectoryMessage != NULL)
				{
					if (pBIOP_DirectoryMessage->objectKey_data == objectKey_data)
					{
						for (binding_index = 0; binding_index < pBIOP_DirectoryMessage->bindings_count; binding_index ++)
						{
							pBindings = pBIOP_DirectoryMessage->bindings + binding_index;
							pBIOP_Name = &(pBindings->BIOP_Name);

							sprintf_s(pszText, sizeof(pszText), "%s:%s", pBIOP_Name->kind_data_byte[0], pBIOP_Name->id_data_byte[0]);
			//				sprintf_s(pszText, sizeof(pszText), "%s", pBIOP_Name->id_data_byte);

							TreeCtrlItem.hParent = hParent;

							hChildItem = m_treeOCDCSyntax.InsertItem(&TreeCtrlItem);
//							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hParent, pszText, -1, 0, NULL);
							if (strcmp(pBindings->IOP_IOR.type_id_byte, "dir") == 0)
							{
								m_treeOCDCSyntax.SetItemImage(hChildItem, 0, 1);

								pBIOP_ObjectLocation = &(pBindings->IOP_IOR.IOP_taggedProfile[0].u.BIOPProfileBody.BIOP_ObjectLocation);

								sub_moduleId = pBIOP_ObjectLocation->moduleId;
								sub_objectKey_data = pBIOP_ObjectLocation->objectKey_data;

								OC_BuildDirectory(PID, hChildItem, sub_moduleId, sub_objectKey_data);
							}
							else
							{
								m_treeOCDCSyntax.SetItemImage(hChildItem, 2, 3);
							}
						}

						m_treeOCDCSyntax.SortChildren(hParent);

						break;
					}
				}
			}

			break;
		}
	}
}

void CDlg_TSAnalyzer_OCDC::OC_GatherInfo(int PID, int table_id_extension)
{
	CDSMCC*			pDSMCC_DSI;
	CDSMCC*			pDSMCC_DII;
	CDSMCC*			pDSMCC_DDM;

	DSMCC_DSI_t*				pDSI;
	DSMCC_DII_t*				pDII;
	BIOP_DirectoryMessage_t*	pBIOP_DirectoryMessage;
	BIOP_FileMessage_t*			pBIOP_FileMessage;
	BIOP_Bindings_t*			pBindings;
	BIOP_Name_t*				pBIOP_Name;
	IOP_IOR_t*					pIOR;
	BIOPProfileBody_t*			pBIOPProfileBody;
	LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
	BIOP_ObjectLocation_t*		pBIOP_ObjectLocation;
	DSM_ConnBinder_t*			pDSM_ConnBinder;

	int				dsi_table_index;
	int				dii_table_index;
	int				ddm_table_index;
	char			pszText[MAX_TXT_CHARS];
	char			pszTemp[64];
	int				module_index;
	int				binding_index;
	int				blockCount;
	int				component_index;
	int				profile_index;
	int				object_index;
	int				i;

	TREEITEM	hDsiItem;
	TREEITEM	hDiiItem;
	TREEITEM	hDdbItem;
	TREEITEM	hSrgItem;
	TREEITEM	hDirItem;
	TREEITEM	hChildItem;
	TREEITEM	hModuleItem;
	HTREEITEM	hBindingItem;
	HTREEITEM	hNameComponentItem;
	HTREEITEM	hIORItem;
	HTREEITEM	hIOP_taggedProfileItem;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();

	int dsmcc_count = pDB_PsiSiTables->GetDSMCCCount();
	for (dsi_table_index = 0; dsi_table_index < dsmcc_count; dsi_table_index ++)
	{
		pDSMCC_DSI = pDB_PsiSiTables->GetDSMCC(dsi_table_index);

		if ((pDSMCC_DSI->m_usPID == PID) &&
			(pDSMCC_DSI->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
			(pDSMCC_DSI->m_usTableIDExtension == table_id_extension))
		{
			if (pDSMCC_DSI->m_usMessageId == 0x1006)			//DSI
			{
				pDSI = &(pDSMCC_DSI->m_DSI);

//				m_treeOCDCSyntax.SetImageList(NULL, TVSIL_NORMAL);

				hDsiItem = TreeFun_InsertItem(&m_treeOCDCSyntax, m_hRootItem, "DSI()", -1, 0, NULL);

				//DSI信息
				hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDsiItem, "carouselId", 32, pDSI->carouselId, NULL);
				hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDsiItem, "moduleId_for_srg", 16, pDSI->moduleId_for_srg, NULL);
				hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDsiItem, "objectKey_data_for_srg", 32, pDSI->objectKey_data_for_srg, NULL);

				hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDsiItem, "table_id_extension_for_dii", 16, pDSI->table_id_extension_for_dii, NULL);

				//DII信息
				hDiiItem = TreeFun_InsertItem(&m_treeOCDCSyntax, m_hRootItem, "DII()", -1, 0, NULL);
				for (dii_table_index = 0; dii_table_index < dsmcc_count; dii_table_index ++)
				{
					if (dii_table_index != dsi_table_index)
					{
						pDSMCC_DII = pDB_PsiSiTables->GetDSMCC(dii_table_index);

						if ((pDSMCC_DII->m_usPID == PID) &&
							(pDSMCC_DII->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
							(pDSMCC_DII->m_usTableIDExtension == pDSI->table_id_extension_for_dii))
						{
							pDII = &(pDSMCC_DII->m_DII);

							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "downloadId", 32, pDII->downloadId, NULL);
							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "blockSize", 16, pDII->blockSize, NULL);
							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "windowSize", 8, pDII->windowSize, NULL);
							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "ackPeriod", 8, pDII->ackPeriod, NULL);
							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "tCDownloadWindow", 32, pDII->tCDownloadWindow, NULL);
							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "tCDownloadScenario", 32, pDII->tCDownloadScenario, NULL);

							hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, "numberOfModules", 16, pDII->numberOfModules, NULL);

							for (module_index = 0; module_index < pDII->numberOfModules; module_index ++)
							{
								sprintf_s(pszText, sizeof(pszText), "MODULE (%d - 0x%04X)", module_index, pDII->astModuleInfo[module_index].moduleId);
								hModuleItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDiiItem, pszText, -1, 0, NULL);

								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleId", 16, pDII->astModuleInfo[module_index].moduleId, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleSize", 32, pDII->astModuleInfo[module_index].moduleSize, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "moduleVersion", 8, pDII->astModuleInfo[module_index].moduleVersion, NULL);

								//计算参数
								blockCount = (S32)(ceil((double)pDII->astModuleInfo[module_index].moduleSize / pDII->blockSize));
								sprintf_s(pszText, sizeof(pszText), "%d", blockCount);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hModuleItem, "blockCount", -1, 0, pszText);
							}

//							m_treeOCDCSyntax.Expand(hSyntaxItem, TVE_EXPAND);

							break;
						}
					}
				}

				//DDB信息
				hDdbItem = TreeFun_InsertItem(&m_treeOCDCSyntax, m_hRootItem, "DDB()", -1, 0, NULL);

				for (ddm_table_index = 0; ddm_table_index < dsmcc_count; ddm_table_index ++)
				{
					pDSMCC_DDM = pDB_PsiSiTables->GetDSMCC(ddm_table_index);
					if ((pDSMCC_DDM->m_usPID == PID) &&
						(pDSMCC_DDM->m_ucTableID == TABLE_ID_DSM_CC_DDM))
					{
						for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index ++)
						{
							pBIOP_DirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];

							if (pBIOP_DirectoryMessage != NULL)
							{
								if (strcmp(pBIOP_DirectoryMessage->objectKind_data, "srg") == 0)
								{
									sprintf_s(pszText, sizeof(pszText), "BIOP::ServiceGatewayMessage (objectKey_data_byte=0x%08X)", pBIOP_DirectoryMessage->objectKey_data);
								}
								else if (strcmp(pBIOP_DirectoryMessage->objectKind_data, "dir") == 0)
								{
									sprintf_s(pszText, sizeof(pszText), "BIOP::DirectoryMessage (objectKey_data_byte=0x%08X)", pBIOP_DirectoryMessage->objectKey_data);
								}
								else
								{
	//								assert(0);
								}
								hSrgItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDdbItem, pszText, -1, 0, NULL);

								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "magic", -1, 0, pBIOP_DirectoryMessage->magic);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "biop_version.major", 8, pBIOP_DirectoryMessage->biop_version.major, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "biop_version.minor", 8, pBIOP_DirectoryMessage->biop_version.minor, NULL);

								DSMCC_byteOrderLookup(pBIOP_DirectoryMessage->byte_order, pszTemp, sizeof(pszTemp));
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "byte_order", 8, pBIOP_DirectoryMessage->byte_order, pszTemp);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "message_type", 8, pBIOP_DirectoryMessage->message_type, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "message_size", 32, pBIOP_DirectoryMessage->message_size, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKey_length", 8, pBIOP_DirectoryMessage->objectKey_length, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKey_data_byte", 32, pBIOP_DirectoryMessage->objectKey_data, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKind_length", 8, pBIOP_DirectoryMessage->objectKind_length, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKind_data", -1, 0, pBIOP_DirectoryMessage->objectKind_data);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectInfo_length", 16, pBIOP_DirectoryMessage->objectInfo_length, NULL);

								if (pBIOP_DirectoryMessage->objectInfo_length > 0)
								{
									memset(pszText, 0x00, MAX_TXT_CHARS);
									for (i = 0; i < min(8, pBIOP_DirectoryMessage->objectInfo_length); i++)
									{
										sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, pBIOP_DirectoryMessage->objectInfo_data_byte[i]);
									}
									hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectInfo_data_byte", -1, 0, pszText);
								}

								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "serviceContextList_count", 8, pBIOP_DirectoryMessage->serviceContextList_count, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "messageBody_length", 32, pBIOP_DirectoryMessage->messageBody_length, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "bindings_count", 16, pBIOP_DirectoryMessage->bindings_count, NULL);

								if (pBIOP_DirectoryMessage->bindings_count > 0)
								{
									for (binding_index = 0; binding_index < pBIOP_DirectoryMessage->bindings_count; binding_index++)
									{
										pBindings = pBIOP_DirectoryMessage->bindings + binding_index;
										pBIOP_Name = &(pBindings->BIOP_Name);

										if (pBIOP_Name->nameComponents_count > 0)
										{
											sprintf_s(pszText, sizeof(pszText), "binding (%d)----%s:%s", binding_index, pBIOP_Name->kind_data_byte[0], pBIOP_Name->id_data_byte[0]);
										}
										else
										{
											sprintf_s(pszText, sizeof(pszText), "binding (%d)", binding_index);
										}
										hBindingItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, pszText, -1, 0, NULL);

										sprintf_s(pszText, sizeof(pszText), "BIOP::Name()");
										hNameComponentItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, pszText, -1, 0, NULL);

										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hNameComponentItem, "nameComponents_count", 8, pBIOP_Name->nameComponents_count, NULL);

										for (component_index = 0; component_index < pBIOP_Name->nameComponents_count; component_index++)
										{
											sprintf_s(pszText, sizeof(pszText), "NameComponent (%d)", component_index);
											hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hNameComponentItem, pszText, -1, 0, NULL);

											TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "id_length", 8, pBIOP_Name->id_length[component_index], NULL);
											if (pBIOP_Name->id_length[component_index] > 0)
											{
												TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "id_data_byte", -1, 0, pBIOP_Name->id_data_byte[component_index]);
											}

											TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "kind_length", 8, pBIOP_Name->kind_length[component_index], NULL);
											if (pBIOP_Name->kind_length[component_index] > 0)
											{
												TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "kind_data_byte", -1, 0, pBIOP_Name->kind_data_byte[component_index]);
											}
										}

										DSMCC_bindingTypeLookup(pBindings->bindingType, pszTemp, sizeof(pszTemp));
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, "bindingType", 8, pBindings->bindingType, pszTemp);

										sprintf_s(pszText, sizeof(pszText), "IOP::IOR()");
										hIORItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, pszText, -1, 0, NULL);

										pIOR = &(pBindings->IOP_IOR);

										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIORItem, "type_id_length", 8, pIOR->type_id_length, NULL);
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIORItem, "type_id_byte", -1, 0, pIOR->type_id_byte);
										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIORItem, "taggedProfiles_count", 8, pIOR->taggedProfiles_count, NULL);

										if (pIOR->taggedProfiles_count > 0)
										{
											for (profile_index = 0; profile_index < (S32)(pIOR->taggedProfiles_count); profile_index ++)
											{
												if (pIOR->IOP_taggedProfile[profile_index].tag == 0x49534F06)
												{
													pBIOPProfileBody = &(pIOR->IOP_taggedProfile[profile_index].u.BIOPProfileBody);

													hIOP_taggedProfileItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIORItem, "BIOPProfileBody()", -1, 0, NULL);

													DSMCC_tagIdLookup(pBIOPProfileBody->profileId_tag, pszTemp, sizeof(pszTemp));
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "profileId_tag", 32, pBIOPProfileBody->profileId_tag, pszTemp);
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "profile_data_length", 32, pBIOPProfileBody->profile_data_length, NULL);

													DSMCC_byteOrderLookup(pBIOPProfileBody->profile_data_byte_order, pszTemp, sizeof(pszTemp));
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "profile_data_byte_order", 8, pBIOPProfileBody->profile_data_byte_order, pszTemp);
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "liteComponents_count", 8, pBIOPProfileBody->liteComponents_count, NULL);

													if (pBIOPProfileBody->BIOP_ObjectLocation.componentId_tag == 0x49534F50)
													{
														pBIOP_ObjectLocation = &(pBIOPProfileBody->BIOP_ObjectLocation);

														sprintf_s(pszText, sizeof(pszText), "BIOP::ObjectLocation()");
														hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, pszText, -1, 0, NULL);

														DSMCC_tagIdLookup(pBIOP_ObjectLocation->componentId_tag, pszTemp, sizeof(pszTemp));
														sprintf_s(pszText, sizeof(pszText), "componentId_tag = 0x%08X [%s]", pBIOP_ObjectLocation->componentId_tag, pszTemp);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, pszText, -1, 0, NULL);

														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "component_data_length", 8, pBIOP_ObjectLocation->component_data_length, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "carouselId", 32, pBIOP_ObjectLocation->carouselId, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "moduleId", 16, pBIOP_ObjectLocation->moduleId, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "version.major", 8, pBIOP_ObjectLocation->version.major, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "version.minor", 8, pBIOP_ObjectLocation->version.minor, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "objectKey_length", 8, pBIOP_ObjectLocation->objectKey_length, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "objectKey_data_byte", 32, pBIOP_ObjectLocation->objectKey_data, NULL);
													}

													if (pBIOPProfileBody->DSM_ConnBinder.componentId_tag == 0x49534F40)
													{
														pDSM_ConnBinder = &(pBIOPProfileBody->DSM_ConnBinder);

														sprintf_s(pszText, sizeof(pszText), "DSM::ConnBinder()");
														hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, pszText, -1, 0, NULL);

														DSMCC_tagIdLookup(pDSM_ConnBinder->componentId_tag, pszTemp, sizeof(pszTemp));
														sprintf_s(pszText, sizeof(pszText), "componentId_tag = 0x%08X [%s]", pDSM_ConnBinder->componentId_tag, pszTemp);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, pszText, -1, 0, NULL);

														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "component_data_length", 8, pDSM_ConnBinder->component_data_length, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "taps_count", 8, pDSM_ConnBinder->taps_count, NULL);

														sprintf_s(pszText, sizeof(pszText), "DSM::Tap()");
														hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, pszText, -1, 0, NULL);

														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "id", 16, pDSM_ConnBinder->BIOP_Tap.id, NULL);
														DSMCC_tapUseLookup(pDSM_ConnBinder->BIOP_Tap.use, pszTemp, sizeof(pszTemp));
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "use", 16, pDSM_ConnBinder->BIOP_Tap.use, pszTemp);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "association_tag", 16, pDSM_ConnBinder->BIOP_Tap.association_tag, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "selector_length", 8, pDSM_ConnBinder->BIOP_Tap.selector_length, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "selector_type", 16, pDSM_ConnBinder->BIOP_Tap.selector_type, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "transactionId", 32, pDSM_ConnBinder->BIOP_Tap.transactionId, NULL);
														TreeFun_InsertItem(&m_treeOCDCSyntax, hChildItem, "timeout", 32, pDSM_ConnBinder->BIOP_Tap.timeout, NULL);
													}
												}
												else if (pIOR->IOP_taggedProfile[profile_index].tag == 0x49534F05)
												{
													pLiteOptionsProfileBody = &(pIOR->IOP_taggedProfile[profile_index].u.LiteOptionsProfileBody);

													hIOP_taggedProfileItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIORItem, "LiteOptionsProfileBody()", -1, 0, NULL);

													DSMCC_tagIdLookup(pLiteOptionsProfileBody->profileId_tag, pszTemp, sizeof(pszTemp));
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "profileId_tag", 32, pLiteOptionsProfileBody->profileId_tag, pszTemp);
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "profile_data_length", 32, pLiteOptionsProfileBody->profile_data_length, NULL);

													DSMCC_byteOrderLookup(pLiteOptionsProfileBody->profile_data_byte_order, pszTemp, sizeof(pszTemp));
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hIOP_taggedProfileItem, "profile_data_byte_order", 8, pLiteOptionsProfileBody->profile_data_byte_order, pszTemp);
												}
											}
										}

										hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, "objectInfo_length", 16, pBindings->objectInfo_length, NULL);
										if (pBindings->objectInfo_length > 0)
										{
											if (strcmp(pBIOP_Name->kind_data_byte[0], "fil") == 0)
											{
												sprintf_s(pszText, sizeof(pszText), "0x");
												for (i = 0; i < 8; i++)
												{
													sprintf_s(pszText, sizeof(pszText), "%s%02X", pszText, pBindings->objectInfo_data_byte[i]);
												}
												hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, "DSM::File::ContentSize", -1, 0, pszText);

												if (pBindings->objectInfo_length > 8)
												{
													hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, "descriptor()", -1, 0, pszText);
												}
											}
											else
											{
												sprintf_s(pszText, sizeof(pszText), "");
												for (i = 0; i < min(16, pBindings->objectInfo_length); i++)
												{
													sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, pBindings->objectInfo_data_byte[i]);
												}
												hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hBindingItem, "objectInfo_data_byte", -1, 0, pszText);
											}
										}
									}
								}
							}
						}

						for (object_index = 0; object_index < pDSMCC_DDM->m_nFileMessageCount; object_index ++)
						{
							pBIOP_FileMessage = pDSMCC_DDM->m_pFileMessage[object_index];

							if (pBIOP_FileMessage != NULL)
							{
								sprintf_s(pszText, sizeof(pszText), "BIOP::FileMessage (objectKey_data_byte=0x%08X)", pBIOP_FileMessage->objectKey_data);
								hSrgItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hDdbItem, pszText, -1, 0, NULL);

								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "magic", -1, 0, pBIOP_FileMessage->magic);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "biop_version.major", 8, pBIOP_FileMessage->biop_version.major, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "biop_version.minor", 8, pBIOP_FileMessage->biop_version.minor, NULL);
								
								DSMCC_byteOrderLookup(pBIOP_FileMessage->byte_order, pszTemp, sizeof(pszTemp));
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "byte_order", 8, pBIOP_FileMessage->byte_order, pszTemp);
								
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "message_type", 8, pBIOP_FileMessage->message_type, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "message_size", 32, pBIOP_FileMessage->message_size, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKey_length", 8, pBIOP_FileMessage->objectKey_length, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKey_data_byte", 32, pBIOP_FileMessage->objectKey_data, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKind_length", 8, pBIOP_FileMessage->objectKind_length, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectKind_data", -1, 0, pBIOP_FileMessage->objectKind_data);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "objectInfo_length", 16, pBIOP_FileMessage->objectInfo_length, NULL);

	//							memset(pszText, 0x00, MAX_TXT_CHARS);
								sprintf_s(pszText, sizeof(pszText), "0x");
								for (i = 0; i < 8; i++)
								{
									sprintf_s(pszText, sizeof(pszText), "%s%02X", pszText, pBIOP_FileMessage->ContentSize[i]);
								}
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "DSM::File::ContentSize", -1, 0, pszText);

								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "serviceContextList_count", 8, pBIOP_FileMessage->serviceContextList_count, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "messageBody_length", 32, pBIOP_FileMessage->messageBody_length, NULL);
								hChildItem = TreeFun_InsertItem(&m_treeOCDCSyntax, hSrgItem, "content_length", 32, pBIOP_FileMessage->content_length, NULL);
							}
						}
					}
				}

				hDirItem = TreeFun_InsertItem(&m_treeOCDCSyntax, m_hRootItem, "文件目录结构", -1, 0, NULL);

				OC_BuildDirectory(PID, hDirItem, pDSI->moduleId_for_srg, pDSI->objectKey_data_for_srg);

				m_treeOCDCSyntax.Expand(hDirItem, TVE_EXPAND);

				break;
			}
		}
	}

	m_treeOCDCSyntax.Expand(m_hRootItem, TVE_EXPAND);
}

void CDlg_TSAnalyzer_OCDC::OnNMRclickListOcdcServices(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CMenu	menu, *pSubMenu;
	int		nSel;

	CTSMagicView* pWindow = CTSMagicView::GetView();

	if (((pWindow->m_kThreadParams.offline == 1) && (pWindow->m_kThreadParams.main_thread_stopped == 1)) ||
		(pWindow->m_kThreadParams.offline == 0))
	{
		nSel = m_listOCDCServices.GetSelectionMark();
		if (nSel >= 0)
		{
			menu.LoadMenu(IDR_RIGHT_KEY_MENU);//装载自定义的右键菜单

			pSubMenu = menu.GetSubMenu(2);//获取第一个弹出菜单，所以第一个菜单必须有子菜单

			CPoint oPoint;//定义一个用于确定光标位置的位置
			GetCursorPos(&oPoint);//获取当前光标的位置，以便使得菜单可以跟随光标

			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this); //在指定位置显示弹出菜单
		}
	}

	*pResult = 0;
}

void CDlg_TSAnalyzer_OCDC::OnDownloadAllModule()
{
	int		item_index;
	int		item_count;
	DWORD	state;

	CDSMCC*			pDSMCC_DSI;
	CDSMCC*			pDSMCC_DII;
	CDSMCC*			pDSMCC_DDM;
	DSMCC_DSI_t*	pDSI;
	DSMCC_DII_t*	pDII;
	int				dsi_table_index;
	int				dii_table_index;
	int				ddm_table_index;
	int				group_index;
	int				module_index;

	char				pszOCDir[MAX_PATH];
	int					PID = 0xFFFF;
	int					data_broadcast_id = 0x0006;
	unsigned int		transaction_id = 0x00000000;
	int					carousel_type_id = 0x0002;
//	int					dsi_table_id_extension;
	int					dii_table_id_extension;
	char				pszModuleName[MAX_PATH];

	unsigned char		component_tag;

	int					exceed_max_count = 0;

	//DC_DOWNLOAD_INFO_t*	pDCDownloadInfo;
//	OC_DOWNLOAD_INFO_t*	pOCDownloadInfo;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();
	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();

	pDB_OCDCs->Reset();

	item_count = m_listOCDCServices.GetItemCount();

	for (item_index = 0; item_index < item_count; item_index ++)
	{
		state = m_listOCDCServices.GetItemState(item_index, LVIS_SELECTED);
		if ((state & LVIS_SELECTED) == LVIS_SELECTED)
		{
//			code = m_listOCDCServices.GetItemData(item_index);
//			pid_index = (code & 0xFFFF0000) >> 16;

			PID = m_usPID_in_OCDCArray[item_index];
			data_broadcast_id = m_usDataBroadcastID_in_OCDCArray[item_index];
			component_tag = m_ucComponentTag_in_OCDCArray[item_index];

			if (m_usCarouselTypeID_in_OCDCArray[item_index] != 0xFFFF)
			{
				carousel_type_id = m_usCarouselTypeID_in_OCDCArray[item_index];
			}

			if (m_uiTransactionID_in_OCDCArray[item_index] != 0xFFFFFFFF)
			{
				transaction_id = m_uiTransactionID_in_OCDCArray[item_index];
			}

			int dsmcc_count = pDB_PsiSiTables->GetDSMCCCount();
			for (dsi_table_index = 0; dsi_table_index < dsmcc_count; dsi_table_index ++)
			{
				pDSMCC_DSI = pDB_PsiSiTables->GetDSMCC(dsi_table_index);

				//若找到DSI则退出
				if ((pDSMCC_DSI->m_usPID == PID) &&
					(pDSMCC_DSI->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
					(pDSMCC_DSI->m_usTableIDExtension == 0x0000) &&
				    (pDSMCC_DSI->m_usMessageId == 0x1006))			//DSI
				{
					pDSI = &(pDSMCC_DSI->m_DSI);

					if (pDSI->payload_type == 0x00)				//DC
					{
						for (group_index = 0; group_index < pDSI->NumberOfGroups; group_index++)
						{
							dii_table_id_extension = (pDSI->astGroupInfo[group_index].GroupId & 0x0000ffff);

							for (dii_table_index = 0; dii_table_index < dsmcc_count; dii_table_index ++)
							{
								if (dii_table_index != dsi_table_index)
								{
									pDSMCC_DII = pDB_PsiSiTables->GetDSMCC(dii_table_index);

									if ((pDSMCC_DII->m_usPID == PID) &&
										(pDSMCC_DII->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
										(pDSMCC_DII->m_usTableIDExtension == dii_table_id_extension))
									{
										pDII = &(pDSMCC_DII->m_DII);

										for (module_index = 0; module_index < pDII->numberOfModules; module_index ++)
										{
											int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
											if (dc_download_count < 500)
											{
												if (pDII->astModuleInfo[module_index].u.DC_moduleInfo.name_descriptor.descriptor_tag != 0x00)
												{
													strcpy_s(pszModuleName, sizeof(pszModuleName), pDII->astModuleInfo[module_index].u.DC_moduleInfo.name_descriptor.text_char);
												}
												else
												{
													sprintf_s(pszModuleName, sizeof(pszModuleName), "virtual_file_0x%04X-0x%04X.dat", PID, pDII->astModuleInfo[module_index].moduleId);
												}

												pDB_OCDCs->AddDCDownloadInfo(0, PID, pDII->downloadId, pDII->blockSize, 
													pDII->astModuleInfo[module_index].moduleId,
													pDII->astModuleInfo[module_index].moduleSize, 
													pDII->astModuleInfo[module_index].moduleVersion,
													pszModuleName);
											}
											else
											{
												exceed_max_count = 1;
											}
										}

										break;
									}
								}
							}
						}
					}
					else			//OC
					{
						for (dii_table_index = 0; dii_table_index < dsmcc_count; dii_table_index ++)
						{
							if (dii_table_index != dsi_table_index)
							{
								pDSMCC_DII = pDB_PsiSiTables->GetDSMCC(dii_table_index);
								dii_table_id_extension = pDSI->table_id_extension_for_dii;

								if ((pDSMCC_DII->m_usPID == PID) &&
									(pDSMCC_DII->m_ucTableID == TABLE_ID_DSM_CC_UNM) &&
									(pDSMCC_DII->m_usTableIDExtension == dii_table_id_extension))
								{
									pDII = &(pDSMCC_DII->m_DII);

									for (module_index = 0; module_index < pDII->numberOfModules; module_index ++)
									{
										//检查一下是否为有效的module
										for (ddm_table_index = 0; ddm_table_index < dsmcc_count; ddm_table_index ++)
										{
											pDSMCC_DDM = pDB_PsiSiTables->GetDSMCC(ddm_table_index);

											if ((pDSMCC_DDM->m_usPID == PID) &&
												(pDSMCC_DDM->m_ucTableID == TABLE_ID_DSM_CC_DDM) &&
												(pDSMCC_DDM->m_usTableIDExtension == pDII->astModuleInfo[module_index].moduleId))
											{
//												if (pDSMCC_DDM->m_nFileMessageCount > 0)
												{
													int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
													if (dc_download_count < 500)
													{
														sprintf_s(pszModuleName, sizeof(pszModuleName), "virtual_file_0x%04X-0x%04X.dat", PID, pDII->astModuleInfo[module_index].moduleId);

														pDB_OCDCs->AddDCDownloadInfo(1, PID, pDII->downloadId, pDII->blockSize, 
															pDII->astModuleInfo[module_index].moduleId, 
															pDII->astModuleInfo[module_index].moduleSize,
															pDII->astModuleInfo[module_index].moduleVersion,
															pszModuleName);
													}
													else
													{
														exceed_max_count = 1;
													}
												}

												break;
											}
										}
									}

									break;
								}
							}
						}

						memset(pszOCDir, 0x00, MAX_PATH);
						OC_BuildDownloadTable(PID, pDSI->carouselId, pDSI->moduleId_for_srg, pDSI->objectKey_data_for_srg, pszOCDir);
					}

					break;
				}
			}
		}
	}

	int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
	if (dc_download_count > 0)
	{
		if (exceed_max_count)
		{
			AfxMessageBox("需要下载的文件数量超出软件支持的范围，只下载前500个文件!", IDOK);
		}
		else
		{
//			if (pWindow->m_kThreadParams.offline == 1)
//			{
				pTSMagicView->m_kThreadParams.dsmcc_download_thread_running = 0;
				pTSMagicView->m_kThreadParams.dsmcc_download_thread_stopped = 0;
				::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_dsmcc_download_thread, (LPVOID)&(pTSMagicView->m_kThreadParams), 0, 0);
//			}
//			else
//			{
//			}
		}
	}
}
