// Mpeg2_ServiceInfoListView.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation.h"
#include "ListView_ServiceInfo.h"
#include "TSMagicView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_SI_Utilities.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_PSI_Section.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"

#include "libs_Utilities\Include\XStream_Utilities.h"

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"

#include "TSMagic_Decimate_Dsmcc.h"

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo

IMPLEMENT_DYNCREATE(CNaviList_DSMCCs, CListView)

//extern PSISI_REPORT_t			PSISI_REPORT;

CNaviList_DSMCCs::CNaviList_DSMCCs()
{
}

CNaviList_DSMCCs::~CNaviList_DSMCCs()
{
}


BEGIN_MESSAGE_MAP(CNaviList_DSMCCs, CListView)
	//{{AFX_MSG_MAP(CListView_ServiceInfo)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CNaviList_DSMCCs::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CNaviList_DSMCCs::OnNMRClick)
	ON_COMMAND(ID_OCDC_DOWNLOAD, &CNaviList_DSMCCs::OnOcdcDownload)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo drawing

void CNaviList_DSMCCs::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo diagnostics

#ifdef _DEBUG
void CNaviList_DSMCCs::AssertValid() const
{
	CListView::AssertValid();
}

void CNaviList_DSMCCs::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo message handlers

BOOL CNaviList_DSMCCs::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CNaviList_DSMCCs::Set(int offline)
{
}

void CNaviList_DSMCCs::Reset(void)
{
	CListCtrl&		listCtrl = GetListCtrl();

	listCtrl.DeleteAllItems();
}

int CNaviList_DSMCCs::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CListCtrl&		listCtrl = GetListCtrl();

	int i;

	m_stColHeader[LISTITEM_COL_INDEX_MARK] = { "", 20, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_TS_PID] = { "TS_PID", 80, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_SERVICE_ID] = { "所属业务", 80, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_SERVICE_NAME] = { "业务名称", 140, LVCFMT_LEFT };
	m_stColHeader[LISTITEM_COL_INDEX_COMPONENT_TAG] = { "组件标签", 80, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_ASSOCIATION_TAG] = { "关联标签", 80, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_TRANSACTION_ID] = { "交易ID", 110, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_CAROUSEL_ID] = { "轮播ID", 110, LVCFMT_CENTER };
	m_stColHeader[LISTITEM_COL_INDEX_BROADCAST_ID] = { "数据广播类型", 140, LVCFMT_LEFT };
	m_stColHeader[LISTITEM_COL_INDEX_CAROUSEL_TYPE_ID] = { "轮播层次", 140, LVCFMT_CENTER };

	//	m_ESImageList.Create(IDB_ES, 20, 1, RGB(0xab,0xcd,0xef));
	//	listCtrl.SetImageList(&m_ESImageList, LVSIL_SMALL);

	listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (i = 0; i < LISTITEM_COL_COUNT; i++)
	{
		listCtrl.InsertColumn(i, m_stColHeader[i].strTitle, m_stColHeader[i].uiAlign, m_stColHeader[i].nWidth, -1);
	}

	return 0;
}


void CNaviList_DSMCCs::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CListCtrl& listCtrl = GetListCtrl();
	CRect rectList;
	listCtrl.GetWindowRect(&rectList);
	int listwidth = rectList.Width();

	CHeaderCtrl* pHeaderCtrl = listCtrl.GetHeaderCtrl();
	int columns = pHeaderCtrl->GetItemCount();

	for (int i = 0; i < columns - 1; i++)
	{
		listwidth -= listCtrl.GetColumnWidth(i);
		listwidth -= 3;
	}
	listCtrl.SetColumnWidth(columns - 1, listwidth);
}

void CNaviList_DSMCCs::UpdatePMT(CPMT* pPMT)
{
	char			pszTemp[64];
	int				bfound;
	int				insert_index;
	int				rtcode;
	ES_INFO_t		stEsInfo;
	SERVICE_INFO_t  stServiceInfo;

	CListCtrl& listCtrl = GetListCtrl();

	if (pPMT != NULL)
	{
		uint16_t usProgramNumber = pPMT->GetProgramNumber();

		int es_count = pPMT->GetESCount();
		for (int es_index = 0; es_index < es_count; es_index++)
		{
			rtcode = pPMT->GetESInfo(es_index, &stEsInfo);
			if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
			{
				//0x0B = DSM-CC U-N Messages（ISO/IEC 13818-6 type B)
				//0x0C = DSM-CC 流描述符(ISO/IEC 13818-6 type C)
				if ((stEsInfo.stream_type == 0x0B) || (stEsInfo.stream_type == 0x0C))			//DSM-CC
				{
					//查重
					bfound = 0;
					int item_count = listCtrl.GetItemCount();
					int insert_pos = item_count;
					for (int item_index = 0; item_index < item_count; item_index++)
					{
						//char* str;
						//listCtrl.GetItemText(j, LISTITEM_COL_INDEX_TS_PID, pszTemp, sizeof(pszTemp));
						//usItemPID = (uint16_t)strtol(pszTemp, &str, 16);
						//sscanf_s(pszTemp, "%x", &usPID, sizeof(usPID));
						uint32_t usItemPID = (uint32_t)(listCtrl.GetItemData(item_index));
						if (stEsInfo.elementary_PID < usItemPID)	//按照PID从小到大排序
						{
							insert_pos = item_index;
							break;
						}
						else if (stEsInfo.elementary_PID == usItemPID)
						{
							bfound = 1;
							insert_index = item_index;
							break;
						}
					}

					if (bfound == 0)	//新插入一条记录
					{
						insert_index = listCtrl.InsertItem(insert_pos, "");
						listCtrl.SetItemData(insert_index, stEsInfo.elementary_PID);

						sprintf_s(pszTemp, sizeof(pszTemp), "0x%04X", stEsInfo.elementary_PID);				//PID
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_TS_PID, pszTemp);
					}

					//更新记录状态
					sprintf_s(pszTemp, sizeof(pszTemp), "0x%04X", usProgramNumber);				//业务号
					listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_SERVICE_ID, pszTemp);

					if (stEsInfo.component_tag != 0xFF)
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "0x%02X", stEsInfo.component_tag);					//组件标签
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_COMPONENT_TAG, pszTemp);
					}

					if (stEsInfo.association_tag != 0xFFFF)
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "0x%04X", stEsInfo.association_tag);				//关联标签
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_ASSOCIATION_TAG, pszTemp);
					}

					if (stEsInfo.transaction_id != 0xFFFFFFFF)
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "0x%08X", stEsInfo.transaction_id);				//交易ID
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_TRANSACTION_ID, pszTemp);
					}

					if (stEsInfo.carousel_id != 0xFFFFFFFF)
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "0x%08X", stEsInfo.carousel_id);				//轮播ID
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_CAROUSEL_ID, pszTemp);
					}

					if (stEsInfo.data_broadcast_id != 0xFFFF)
					{
						char pszStyle[32];

						DVB_SI_NumericCoding2Text_DataBroadcastID(stEsInfo.data_broadcast_id, pszStyle, sizeof(pszStyle));
						sprintf_s(pszTemp, sizeof(pszTemp), "%s", pszStyle);				//轮播ID
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_BROADCAST_ID, pszTemp);
					}

					CTSMagicView* pView = CTSMagicView::GetView();
					CDB_PsiSiTables* pDB_PsiSiTables = pView->GetPsiSiTablesDBase();

					//从SDT表中倒查业务名称
					CSDT* pSDT = pDB_PsiSiTables->QueryActualSDT();		
					if (pSDT != NULL)
					{
						rtcode = pSDT->GetServiceInfoByID(usProgramNumber, &stServiceInfo);
						if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
						{
							listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_SERVICE_NAME, stServiceInfo.pszServiceName);
						}
					}

					//在已有库中查询一下DSMCC表的状态，必须要查到DSMCC_UNM DSI的存在
					CPVT* pPVT = pDB_PsiSiTables->QueryDsmccUNM_DSI(stEsInfo.elementary_PID);	//直接查DSI
					if (pPVT != NULL)
					{
						listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_MARK, "+");
					}
				}
			}
		}
	}
}

void CNaviList_DSMCCs::UpdateSDT(CSDT* pSDT)
{
	int				service_index;
	int				nCount;
	int				nItem;
	int				broadcast_index;
	char			pszTemp[64];
	CString			strTemp;

	data_broadcast_descriptor_t*	pdata_broadcast_descriptor;
	object_carousel_info_t*			pobject_carousel_info;
	data_carousel_info_t*			pdata_carousel_info;

	CListCtrl& listCtrl = GetListCtrl();

	if (pSDT != NULL)
	{
		if (pSDT->GetTableID() == TABLE_ID_SDT_ACTUAL)
		{
			nCount = listCtrl.GetItemCount();
			for (nItem = 0; nItem < nCount; nItem++)
			{
				char* str;
				listCtrl.GetItemText(nItem, LISTITEM_COL_INDEX_SERVICE_ID, pszTemp, sizeof(pszTemp));
				uint16_t usServiceID = (uint16_t)strtol(pszTemp, &str, 16);

				listCtrl.GetItemText(nItem, LISTITEM_COL_INDEX_COMPONENT_TAG, pszTemp, sizeof(pszTemp));
				uint8_t ucComponentTag = (uint8_t)strtol(pszTemp, &str, 16);

				for (service_index = 0; service_index < pSDT->GetServiceCount(); service_index++)
				{
					SERVICE_INFO_t stServiceInfo;
					int rtcode = pSDT->GetServiceInfoByID(usServiceID, &stServiceInfo);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						for (broadcast_index = 0; broadcast_index < stServiceInfo.data_broadcast_count; broadcast_index++)
						{
							pdata_broadcast_descriptor = &(stServiceInfo.data_broadcast_descriptor[broadcast_index]);
							if (pdata_broadcast_descriptor->descriptor_tag == DVB_SI_DATA_BROADCAST_DESCRIPTOR)
							{
								if (pdata_broadcast_descriptor->component_tag == ucComponentTag)
								{
									if (pdata_broadcast_descriptor->selector_length > 0)
									{
										switch (pdata_broadcast_descriptor->data_broadcast_id)
										{
										case 0x0006:

											pdata_carousel_info = &(pdata_broadcast_descriptor->selector_byte.data_carousel_info);

											DVB_SI_NumericCoding2Text_CarouselTypeID(pdata_carousel_info->carousel_type_id, pszTemp, sizeof(pszTemp));
											listCtrl.SetItemText(nItem, LISTITEM_COL_INDEX_CAROUSEL_TYPE_ID, pszTemp);

											strTemp = listCtrl.GetItemText(nItem, LISTITEM_COL_INDEX_TRANSACTION_ID);			//transaction_id
											if (strTemp == "")
											{
												sprintf_s(pszTemp, sizeof(pszTemp), "0x%08X", pdata_carousel_info->transaction_id);
												listCtrl.SetItemText(nItem, LISTITEM_COL_INDEX_TRANSACTION_ID, pszTemp);
											}

											break;

										case 0x0007:

											pobject_carousel_info = &(pdata_broadcast_descriptor->selector_byte.object_carousel_info);

											DVB_SI_NumericCoding2Text_CarouselTypeID(pobject_carousel_info->carousel_type_id, pszTemp, sizeof(pszTemp));
											listCtrl.SetItemText(nItem, LISTITEM_COL_INDEX_CAROUSEL_TYPE_ID, pszTemp);

											strTemp = listCtrl.GetItemText(nItem, LISTITEM_COL_INDEX_TRANSACTION_ID);			//transaction_id
											if (strTemp == "")
											{
												sprintf_s(pszTemp, sizeof(pszTemp), "0x%08X", pobject_carousel_info->transaction_id);
												listCtrl.SetItemText(nItem, LISTITEM_COL_INDEX_TRANSACTION_ID, pszTemp);
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

						strTemp = listCtrl.GetItemText(nItem, LISTITEM_COL_INDEX_SERVICE_NAME);
						if (strTemp == "")
						{
							listCtrl.SetItemText(nItem, LISTITEM_COL_INDEX_SERVICE_NAME, stServiceInfo.pszServiceName);
						}

						break;
					}
				}
			}
		}
	}
}

void CNaviList_DSMCCs::UpdateDSMCC(CPVT* pPVT)
{
	char			pszTemp[64];

	CListCtrl& listCtrl = GetListCtrl();
	if (pPVT != NULL)
	{
		//if ((pPVT->GetTableID() == TABLE_ID_DSMCC_UNM) && (pPVT->GetTableIDExtension() == 0x0000))
		{
			int bfound = 0;
			int dsmcc_pid = pPVT->GetPID();

			int item_count = listCtrl.GetItemCount();
			int op_item_pos = item_count;

			//首先检查一遍是否已经存在
			for (int item_index = 0; item_index < item_count; item_index++)
			{
				char* str;
				listCtrl.GetItemText(item_index, LISTITEM_COL_INDEX_TS_PID, pszTemp, sizeof(pszTemp));
				uint16_t usItemPID = (uint16_t)strtol(pszTemp, &str, 16);

				if (dsmcc_pid < usItemPID)
				{
					op_item_pos = item_index;
					bfound = 0;
					break;
				}
				else if (dsmcc_pid == usItemPID)
				{
					op_item_pos = item_index;
					bfound = 1;
					break;
				}
			}

			if (bfound == 0)		//新插入一条记录
			{
				listCtrl.InsertItem(op_item_pos, "");
				listCtrl.SetItemData(op_item_pos, dsmcc_pid);

				sprintf_s(pszTemp, sizeof(pszTemp), "0x%04X", dsmcc_pid);				//PID
				listCtrl.SetItemText(op_item_pos, LISTITEM_COL_INDEX_TS_PID, pszTemp);

				bfound = 1;
			}

			//更新记录状态
			if (bfound == 1)
			{
				//if ((pPVT->GetTableID() == TABLE_ID_DSMCC_UNM) && (pPVT->GetTableIDExtension() == 0x0000))
				if (pPVT->GetTableID() == TABLE_ID_DSMCC_UNM)
				{
					CDSMCC_UNM* pDSMCC = (CDSMCC_UNM*)pPVT;
					if (pDSMCC->GetMessageID() == 0x1006)
					{
						//只有遇到DSMCC_UNM时才置“+”标记，这是DSMCC DSI入口
						listCtrl.SetItemText(op_item_pos, LISTITEM_COL_INDEX_MARK, "+");
					}
				}
			}
		}
	}
}

void CNaviList_DSMCCs::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	char	    pszTemp[48];
	uint32_t	usPID;
	//uint32_t	dwTransactionID;
	//uint32_t	usBroadcastID;
	//uint32_t	usServiceID = 0xFFFF;
	//uint8_t		ucCarouselTypeID = 0xFF;

	CListCtrl& listCtrl = GetListCtrl();
	int nSel = listCtrl.GetSelectionMark();
	if (nSel >= 0)
	{
		m_pInfoTree->Reset();

		//char* str;
		//listCtrl.GetItemText(nSel, LISTITEM_COL_INDEX_TS_PID, pszTemp, sizeof(pszTemp));
		////usPID = (uint16_t)strtol(pszTemp, &str, 16);
		//sscanf_s(pszTemp, "0x%04X", &usPID);
		usPID = (uint16_t)listCtrl.GetItemData(nSel);

		//listCtrl.GetItemText(nSel, LISTITEM_COL_INDEX_SERVICE_ID, pszTemp, sizeof(pszTemp));
		//if (strlen(pszTemp) > 0)
		//{
		//	//usServiceID = (uint16_t)strtol(pszTemp, &str, 16);
		//	sscanf_s(pszTemp, "0x%04X", &usServiceID);
		//}

		//listCtrl.GetItemText(nSel, LISTITEM_COL_INDEX_TRANSACTION_ID, pszTemp, sizeof(pszTemp));
		////dwTransactionID = (uint32_t)strtol(pszTemp, &str, 16);
		//sscanf_s(pszTemp, "0x%08X", &dwTransactionID);

		//listCtrl.GetItemText(nSel, LISTITEM_COL_INDEX_CAROUSEL_TYPE_ID, pszTemp, sizeof(pszTemp));
		//if (strlen(pszTemp) > 0)
		//{
		//	if (strcmp(pszTemp, "one layer carousel") == 0)
		//	{
		//		ucCarouselTypeID = 1;
		//	}
		//	else if (strcmp(pszTemp, "two layer carousel") == 0)
		//	{
		//		ucCarouselTypeID = 2;
		//	}
		//	else
		//	{
		//		ucCarouselTypeID = 0;
		//	}
		//}

		//listCtrl.GetItemText(nSel, LISTITEM_COL_INDEX_BROADCAST_ID, pszTemp, sizeof(pszTemp));
		//sscanf_s(pszTemp, "0x%04X", &usBroadcastID);

		CTSMagicView* pTSMagicView = CTSMagicView::GetView();
		CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();

		XMLDocForMpegSyntax xmlDoc;
		pDB_PsiSiTables->BuildDsmccTree(usPID, &xmlDoc);

		sprintf_s(pszTemp, sizeof(pszTemp), "e:\\temp\\DSMCC-sematics-0x%04X.xml", usPID);
		xmlDoc.SaveFile(pszTemp);

		m_pInfoTree->ShowXMLDoc(&xmlDoc);
	}

	*pResult = 0;
}


void CNaviList_DSMCCs::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	menu, *pSubMenu;
	int		nSel;

	CListCtrl& listCtrl = GetListCtrl();

	CTSMagicView* pWindow = CTSMagicView::GetView();

	if (((pWindow->m_kThreadParams.offline == 1) && (pWindow->m_kThreadParams.main_thread_stopped == 1)) ||
		(pWindow->m_kThreadParams.offline == 0))
	{
		nSel = listCtrl.GetSelectionMark();
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

void CNaviList_DSMCCs::OC_BuildDownloadTable(uint16_t PID, uint32_t downloadId, uint16_t moduleId, uint32_t objectKey_data, char* pszDir)
{
#if DEBUG_DSMCC
	CDSMCC_DDM*					pDSMCC_DDM;
	DirectoryMessage_t*			pDirectoryMessage;
	Bindings_t*					pBindings;
	//BIOP::Name_t*				pName;
	BIOP::ObjectLocation_t*		pObjectLocation;

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

	pDSMCC_DDM = (CDSMCC_DDM*)pDB_PsiSiTables->QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, moduleId);
	if (pDSMCC_DDM != NULL)
	{
		for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index++)
		{
			pDirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];
			if (pDirectoryMessage != NULL)
			{
				if (pDirectoryMessage->objectKey_data == objectKey_data)
				{
					for (binding_index = 0; binding_index < pDirectoryMessage->bindings_count; binding_index++)
					{
						pBindings = pDirectoryMessage->bindings + binding_index;
						//pName = &(pBindings->Name);
						pObjectLocation = &(pBindings->IOR.taggedProfile[0].u.BIOPProfileBody.ObjectLocation);

						sprintf_s(pszText, sizeof(pszText), "%s", pBindings->Name.id_data_byte[0]);
						if (strcmp(pBindings->IOR.type_id_byte, "dir") == 0)
						{
							sprintf_s(pszPath, sizeof(pszPath), "%s%s\\", pszDir, pszText);

							sub_moduleId = pObjectLocation->moduleId;
							sub_objectKey_data = pObjectLocation->objectKey_data;

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
								PID,
								pObjectLocation->carouselId,
								pObjectLocation->objectKey_data,
								pObjectLocation->moduleId,
								pszPath
							);
						}
					}

					break;
				}
			}
		}
	}
#endif
}

void CNaviList_DSMCCs::OnOcdcDownload(void)
{
	// TODO: 在此添加命令处理程序代码
	int		item_index;
	int		item_count;
	DWORD	state;

	CDSMCC_UNM*		pDSMCC_DSI;
	CDSMCC_UNM*		pDSMCC_DII;
	CDSMCC_DDM*		pDSMCC_DDM;
	DSMCC_DSI_t*	pDSI;
	DSMCC_DII_t*	pDII;
	//int				dsi_table_index;
	//int				dii_table_index;
	//int				ddm_table_index;
	int				group_index;
	int				module_index;

	char	    pszTemp[48];
	uint32_t	usPID;
	//uint32_t	dwTransactionID;
	//uint32_t	usBroadcastID;
	//uint32_t	usServiceID;
	//uint8_t		ucCarouselTypeID;

	char				pszOCDir[MAX_PATH];
	//int					PID = 0xFFFF;
	//int					data_broadcast_id = 0x0006;
	//unsigned int		transaction_id = 0x00000000;
	//int					carousel_type_id = 0x0002;
	//	int					dsi_table_id_extension;
	int					dii_table_id_extension;
	char				pszModuleName[MAX_PATH];

	//unsigned char		component_tag;

	int					exceed_max_count = 0;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();
	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();

	pDB_OCDCs->Reset();

	CListCtrl& listCtrl = GetListCtrl();

	item_count = listCtrl.GetItemCount();

	for (item_index = 0; item_index < item_count; item_index++)
	{
		state = listCtrl.GetItemState(item_index, LVIS_SELECTED);
		if ((state & LVIS_SELECTED) == LVIS_SELECTED)
		{
			//listCtrl.GetItemText(item_index, LISTITEM_COL_INDEX_TS_PID, pszTemp, sizeof(pszTemp));
			//usPID = (uint16_t)strtol(pszTemp, &str, 16);
			//sscanf_s(pszTemp, "0x%04X", &usPID);
			usPID = (uint16_t)listCtrl.GetItemData(item_index);

			pDSMCC_DSI = (CDSMCC_UNM*)pDB_PsiSiTables->QueryDsmccUNM_DSI(usPID);
			if (pDSMCC_DSI != NULL)
			{
				assert(pDSMCC_DSI->GetMessageID() == 0x1006);

				pDSI = &(pDSMCC_DSI->u.m_DSI);

				if (pDSI->data_broadcast_type == 0x0006)				//DC
				{
					for (group_index = 0; group_index < pDSI->NumberOfGroups; group_index++)
					{
						dii_table_id_extension = (pDSI->astGroupInfo[group_index].GroupId & 0x0000ffff);

						pDSMCC_DII = (CDSMCC_UNM*)pDB_PsiSiTables->QueryBy3ID(usPID, TABLE_ID_DSMCC_UNM, dii_table_id_extension);

						if (pDSMCC_DII != NULL)
						{
							pDII = &(pDSMCC_DII->u.m_DII);

							for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
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
										sprintf_s(pszModuleName, sizeof(pszModuleName), "virtual_file_0x%04X-0x%04X.dat", usPID, pDII->astModuleInfo[module_index].moduleId);
									}

									pDB_OCDCs->AddDCDownloadInfo(0, usPID, pDII->downloadId, pDII->blockSize,
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
						}
					}
				}
				else			//OC
				{
					dii_table_id_extension = pDSI->table_id_extension_for_dii;
					pDSMCC_DII = (CDSMCC_UNM*)pDB_PsiSiTables->QueryBy3ID(usPID, TABLE_ID_DSMCC_UNM, dii_table_id_extension);

					if (pDSMCC_DII != NULL)
					{
						pDII = &(pDSMCC_DII->u.m_DII);

						for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
						{
							pDSMCC_DDM = (CDSMCC_DDM*)pDB_PsiSiTables->QueryBy3ID(usPID, TABLE_ID_DSMCC_DDM, pDII->astModuleInfo[module_index].moduleId);
							//检查一下是否为有效的module
							if (pDSMCC_DDM != NULL)
							{
								int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
								if (dc_download_count < 500)
								{
									sprintf_s(pszModuleName, sizeof(pszModuleName), "virtual_file_0x%04X-0x%04X.dat", usPID, pDII->astModuleInfo[module_index].moduleId);

									pDB_OCDCs->AddDCDownloadInfo(1, usPID, pDII->downloadId, pDII->blockSize,
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
						}
					}

					memset(pszOCDir, 0x00, MAX_PATH);
					OC_BuildDownloadTable(usPID, pDSI->carouselId, pDSI->moduleId_for_srg, pDSI->objectKey_data_for_srg, pszOCDir);
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
			pTSMagicView->m_kThreadParams.dsmcc_download_thread_running = 0;
			pTSMagicView->m_kThreadParams.dsmcc_download_thread_stopped = 0;
			::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_dsmcc_download_thread, (LPVOID)&(pTSMagicView->m_kThreadParams), 0, 0);
		}
	}
}
