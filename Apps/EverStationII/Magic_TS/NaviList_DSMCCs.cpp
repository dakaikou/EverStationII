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

#include "translate_layer/MPEG2_DVB_Section/Include/DVB_SI_Utilities.h"
#include "translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Section.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"

#include "toolbox_libs\TOOL_Directory\Include\TOOL_Directory.h"

#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"

#include "TSMagic_Dsmcc_Download.h"

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo

IMPLEMENT_DYNCREATE(CNaviList_DSMCCs, CListView)

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
	//listCtrl.SetExtendedStyle((LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT) & ~LVS_SINGLESEL);
	listCtrl.ModifyStyle(LVS_SINGLESEL, 0);

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
					CDB_PsiSiObjs* pDB_PsiSiObjs = pView->GetPsiSiObjsDBase();

					//从SDT表中倒查业务名称
					CSDT* pSDT = pDB_PsiSiObjs->QueryActualSDT();
					if (pSDT != NULL)
					{
						rtcode = pSDT->GetServiceInfoByID(usProgramNumber, &stServiceInfo);
						if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
						{
							listCtrl.SetItemText(insert_index, LISTITEM_COL_INDEX_SERVICE_NAME, stServiceInfo.pszServiceName);
						}
					}

					//在已有库中查询一下DSMCC表的状态，必须要查到DSMCC_UNM DSI的存在
					CPVT* pPVT = pDB_PsiSiObjs->QueryDsmccUNM_DSI(stEsInfo.elementary_PID);	//直接查DSI
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

											pdata_carousel_info = &(pdata_broadcast_descriptor->u.data_carousel_info);

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

											pobject_carousel_info = &(pdata_broadcast_descriptor->u.object_carousel_info);

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
	uint32_t	usPID;

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

		CTSMagicView* pTSMagicView = CTSMagicView::GetView();
		CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();

		TALForXMLDoc xmlDoc;
		pDB_PsiSiObjs->BuildDsmccTree(usPID, &xmlDoc);
		m_pInfoTree->ShowXMLDoc(&xmlDoc);

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

		sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\DSMCC_sematics_0x%04X.xml", pszXmlDir, usPID);
		xmlDoc.SaveFile(pszFilePath);
#endif
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

	//CTSMagicView* pWindow = CTSMagicView::GetView();

	//if (((pWindow->m_kThreadParams.offline == 1) && (pWindow->m_kThreadParams.main_thread_stopped == 1)) ||
	//	(pWindow->m_kThreadParams.offline == 0))
	//{
		nSel = listCtrl.GetSelectionMark();
		if (nSel >= 0)
		{
			menu.LoadMenu(IDR_RIGHT_KEY_MENU);//装载自定义的右键菜单

			pSubMenu = menu.GetSubMenu(2);//获取第一个弹出菜单，所以第一个菜单必须有子菜单

			CPoint oPoint;//定义一个用于确定光标位置的位置
			GetCursorPos(&oPoint);//获取当前光标的位置，以便使得菜单可以跟随光标

			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this); //在指定位置显示弹出菜单
		}
	//}

	*pResult = 0;
}

void CNaviList_DSMCCs::OnOcdcDownload(void)
{
	// TODO: 在此添加命令处理程序代码
	int		item_index;
	int		item_count;
	DWORD	state;

	uint32_t	usPID;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	//CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();
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

			DOWNLOAD_INFO_t stDownloadInfo;
			stDownloadInfo.usCandidatePID = usPID;
			pDB_OCDCs->AppendDownloadInfo(&stDownloadInfo);
		}
	}

	pTSMagicView->m_kThreadParams.dsmcc_download_thread_running = 0;
	pTSMagicView->m_kThreadParams.dsmcc_download_thread_stopped = 0;
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_dsmcc_download_thread, (LPVOID)&(pTSMagicView->m_kThreadParams), 0, 0);
}
