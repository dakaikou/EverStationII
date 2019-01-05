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

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_SI_Utilities.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"

#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"

#include "..\Common\define.h"

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo

IMPLEMENT_DYNCREATE(CListView_ServiceInfo, CListView)

CListView_ServiceInfo::CListView_ServiceInfo()
{
}

CListView_ServiceInfo::~CListView_ServiceInfo()
{
}


BEGIN_MESSAGE_MAP(CListView_ServiceInfo, CListView)
	//{{AFX_MSG_MAP(CListView_ServiceInfo)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo drawing

void CListView_ServiceInfo::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo diagnostics

#ifdef _DEBUG
void CListView_ServiceInfo::AssertValid() const
{
	CListView::AssertValid();
}

void CListView_ServiceInfo::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo message handlers

BOOL CListView_ServiceInfo::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CListView_ServiceInfo::Set(int offline)
{
}

void CListView_ServiceInfo::Reset(void)
{
	CListCtrl&		listCtrl = GetListCtrl();

	listCtrl.DeleteAllItems();
}
/*
	uint16_t										service_id;
	uint8_t										EIT_schedule_flag;
	uint8_t										EIT_present_following_flag;
	uint8_t										running_status;
	uint8_t										free_CA_mode;

	uint16_t										descriptors_loop_length;

	service_descriptor_t					service_descriptor;
	bouquet_name_descriptor_t				bouquet_name_descriptor;
	CA_identifier_descriptor_t				CA_identifier_descriptor;
	country_availability_descriptor_t		country_availability_descriptor;
	data_broadcast_descriptor_t				data_broadcast_descriptor;
	linkage_descriptor_t					linkage_descriptor;
	mosaic_descriptor_t						mosaic_descriptor;
	multilingual_service_name_descriptor_t	multilingual_service_name_descriptor;
	NVOD_reference_descriptor_t				NVOD_reference_descriptor;
	telephone_descriptor_t					telephone_descriptor;
	time_shifted_service_descriptor_t		time_shifted_service_descriptor;
*/
void CListView_ServiceInfo::Update(uint16_t network_id, uint16_t transport_stream_id, uint16_t service_id)
{
	CSDT*							pSDT;
	CNIT*							pNIT;

	SERVICE_INFO_t					stServiceInfo;
	STREAM_INFO_t					stStreamInfo;

	data_broadcast_descriptor_t*	pdata_broadcast_descriptor;
	data_carousel_info_t*			pdata_carousel_info;
	object_carousel_info_t*			pobject_carousel_info;
	int								broadcast_index;

	//char	pszNetworkID[64];
	char	pszNetworkInfo[256];

	char	pszStreamID[64];
	char	pszFrequency[64];
	char	pszSymbolRate[64];
	char	pszModulation[64];
	char	pszPolarization[64];

	char	pszTSInfo[256];

	char	pszServiceID[64];
	char	pszServiceType[64];
	char    pszServiceInfo[256];

	char    pszEitInfo[256];

	CListCtrl&				listCtrl = GetListCtrl();
	char					pszText[MAX_TXT_CHARS];
	char					pszTemp[64];

	int						nItem = 0;
	//char*					pstr_dst;
	//char*					pstr_service_name;
	//char*					pstr_service_provider;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();

	if (network_id != 0xFFFF)
	{
		pNIT = pDB_PsiSiObjs->QueryNITByID(network_id);

		if (pNIT != NULL)
		{
			listCtrl.InsertItem(nItem, "");
			listCtrl.SetItemText(nItem, 1, "网络信息");

			pNIT->GetNetworkName(pszTemp, sizeof(pszTemp));
			sprintf_s(pszNetworkInfo, sizeof(pszNetworkInfo), "ID:0x%04x(%d), 名称:%s", network_id, network_id, pszTemp);

			listCtrl.SetItemText(nItem, 2, pszNetworkInfo);
			nItem++;

			if (transport_stream_id != 0xFFFF)
			{
				pNIT->GetStreamByID(transport_stream_id, &stStreamInfo);
				sprintf_s(pszStreamID, sizeof(pszStreamID), "0x%04x(%d)", stStreamInfo.transport_stream_id, stStreamInfo.transport_stream_id);

				memset(pszTSInfo, 0x00, sizeof(pszTSInfo));
				if (stStreamInfo.uDelivery.cable_delivery_system_descriptor.descriptor_tag == DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR)
				{
					cable_delivery_system_descriptor_t* pcable_delivery_system_descriptor = &(stStreamInfo.uDelivery.cable_delivery_system_descriptor);

					listCtrl.InsertItem(nItem, "");
					listCtrl.SetItemText(nItem, 1, "TS流信息");
					sprintf_s(pszFrequency, sizeof(pszFrequency), "%x.%04xMHz", 
						(pcable_delivery_system_descriptor->frequency & 0xFFFF0000) >> 16, 
						pcable_delivery_system_descriptor->frequency & 0x0000FFFF);
					sprintf_s(pszSymbolRate, sizeof(pszSymbolRate), "%x.%04xMsps", 
						(pcable_delivery_system_descriptor->symbol_rate & 0xFFFF0000) >> 16, 
						pcable_delivery_system_descriptor->symbol_rate & 0x0000FFFF);
					DVB_SI_NumericCoding2Text_CableModulation(pcable_delivery_system_descriptor->modulation, pszModulation, sizeof(pszModulation));
					sprintf_s(pszTSInfo, sizeof(pszTSInfo), "ID:%s, 频率:%s, 符号率:%s, 调制:%s", pszStreamID, pszFrequency, pszSymbolRate, pszModulation);

					listCtrl.SetItemText(nItem, 2, pszTSInfo);
					nItem++;
				}
				else if (stStreamInfo.uDelivery.satellite_delivery_system_descriptor.descriptor_tag == DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR)
				{
					satellite_delivery_system_descriptor_t* psatellite_delivery_system_descriptor = &(stStreamInfo.uDelivery.satellite_delivery_system_descriptor);
					listCtrl.InsertItem(nItem, "");
					listCtrl.SetItemText(nItem, 1, "TS流信息");
					sprintf_s(pszFrequency, sizeof(pszFrequency), "%X.%05XGHz", 
						(psatellite_delivery_system_descriptor->frequency & 0xFFF00000) >> 20, 
						psatellite_delivery_system_descriptor->frequency & 0x000FFFFF);
					sprintf_s(pszSymbolRate, sizeof(pszSymbolRate), "%X.%04XMsps", 
						(psatellite_delivery_system_descriptor->symbol_rate & 0xFFFF0000) >> 16, 
						psatellite_delivery_system_descriptor->symbol_rate & 0x0000FFFF);
					DVB_SI_NumericCoding2Text_SatelliteModulationType(psatellite_delivery_system_descriptor->modulation_type, pszModulation, sizeof(pszModulation));
					DVB_SI_NumericCoding2Text_SatellitePolarization(psatellite_delivery_system_descriptor->polarization, pszPolarization, sizeof(pszPolarization));
					sprintf_s(pszTSInfo, sizeof(pszTSInfo), "ID:%s, 频率:%s, 符号率:%s, 调制:%s, 极化方式:%s", pszStreamID, pszFrequency, pszSymbolRate, pszModulation, pszPolarization);

					listCtrl.SetItemText(nItem, 2, pszTSInfo);
					nItem++;
				}
				else
				{
					listCtrl.InsertItem(nItem, "");
					listCtrl.SetItemText(nItem, 1, "TS流信息");
					sprintf_s(pszTSInfo, sizeof(pszTSInfo), "ID:%s", pszStreamID);

					listCtrl.SetItemText(nItem, 2, pszTSInfo);
					nItem++;
				}

				pSDT = pDB_PsiSiObjs->QuerySDTBy2ID(network_id, transport_stream_id);

				if (pSDT != NULL)
				{
					int rtcode = pSDT->GetServiceInfoByID(service_id, &stServiceInfo);
					if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
					{
						listCtrl.InsertItem(nItem, "");
						listCtrl.SetItemText(nItem, 1, "业务信息");
						sprintf_s(pszServiceID, sizeof(pszServiceID), "0x%04X (%d)", stServiceInfo.service_id, stServiceInfo.service_id);
						DVB_SI_NumericCoding2Text_ServiceType(stServiceInfo.service_type, pszServiceType, sizeof(pszServiceType));
						sprintf_s(pszServiceInfo, sizeof(pszServiceInfo), "ID:%s, 业务类型:%s, free_CA_mode:%d, 名称:%s, 提供者:%s", 
							pszServiceID, pszServiceType, stServiceInfo.free_CA_mode, stServiceInfo.pszServiceName, stServiceInfo.pszServiceProviderName);
						listCtrl.SetItemText(nItem, 2, pszServiceInfo);
						nItem++;

						listCtrl.InsertItem(nItem, "");
						listCtrl.SetItemText(nItem, 1, "EIT信息");
						sprintf_s(pszEitInfo, sizeof(pszEitInfo), "schedule_flag:%d, PF_flag:%d", stServiceInfo.EIT_schedule_flag, stServiceInfo.EIT_present_following_flag);
						listCtrl.SetItemText(nItem, 2, pszEitInfo);
						nItem++;

						listCtrl.InsertItem(nItem, "");
						nItem++;

						for (broadcast_index = 0; broadcast_index < stServiceInfo.data_broadcast_count; broadcast_index++)
						{
							if (stServiceInfo.data_broadcast_descriptor[broadcast_index].descriptor_tag == DVB_SI_DATA_BROADCAST_DESCRIPTOR)
							{
								pdata_broadcast_descriptor = &(stServiceInfo.data_broadcast_descriptor[broadcast_index]);

								listCtrl.InsertItem(nItem, "");
								listCtrl.SetItemText(nItem, 1, "data_broadcast_id");
								DVB_SI_NumericCoding2Text_DataBroadcastID(pdata_broadcast_descriptor->data_broadcast_id, pszTemp, sizeof(pszTemp));
								sprintf_s(pszText, sizeof(pszText), "0x%04X [%s]", pdata_broadcast_descriptor->data_broadcast_id, pszTemp);
								listCtrl.SetItemText(nItem, 2, pszText);
								nItem++;

								listCtrl.InsertItem(nItem, "");
								listCtrl.SetItemText(nItem, 1, "component_tag");
								sprintf_s(pszText, sizeof(pszText), "0x%02X(%d)", pdata_broadcast_descriptor->component_tag, pdata_broadcast_descriptor->component_tag);
								listCtrl.SetItemText(nItem, 2, pszText);
								nItem++;

								listCtrl.InsertItem(nItem, "");
								listCtrl.SetItemText(nItem, 1, "ISO_639_language_code");
								listCtrl.SetItemText(nItem, 2, pdata_broadcast_descriptor->ISO_639_language_code_char);
								nItem++;

								if (pdata_broadcast_descriptor->selector_length > 0)
								{
									if (pdata_broadcast_descriptor->data_broadcast_id == 0x0006)			//数据轮播
									{
										pdata_carousel_info = &(pdata_broadcast_descriptor->u.data_carousel_info);

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "carousel_type_id");
										DVB_SI_NumericCoding2Text_CarouselTypeID(pdata_carousel_info->carousel_type_id, pszTemp, sizeof(pszTemp));
										sprintf_s(pszText, sizeof(pszText), "0x%04X [%s]", pdata_carousel_info->carousel_type_id, pszTemp);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "transaction_id");
										sprintf_s(pszText, sizeof(pszText), "0x%08X", pdata_carousel_info->transaction_id);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "time_out_value_DSI");
										sprintf_s(pszText, sizeof(pszText), "0x%08X(%d)", pdata_carousel_info->time_out_value_DSI, pdata_carousel_info->time_out_value_DSI);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "time_out_value_DII");
										sprintf_s(pszText, sizeof(pszText), "0x%08X(%d)", pdata_carousel_info->time_out_value_DII, pdata_carousel_info->time_out_value_DII);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "leak_rate");
										sprintf_s(pszText, sizeof(pszText), "0x%06X(%d)", pdata_carousel_info->leak_rate, pdata_carousel_info->leak_rate);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;
									}
									else if (pdata_broadcast_descriptor->data_broadcast_id == 0x0007)			//对象轮播
									{
										pobject_carousel_info = &(pdata_broadcast_descriptor->u.object_carousel_info);

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "Carousel_type_id");
										DVB_SI_NumericCoding2Text_CarouselTypeID(pobject_carousel_info->carousel_type_id, pszTemp, sizeof(pszTemp));
										sprintf_s(pszText, sizeof(pszText), "0x%04X [%s]", pobject_carousel_info->carousel_type_id, pszTemp);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "Transaction_id");
										sprintf_s(pszText, sizeof(pszText), "0x%08X", pobject_carousel_info->transaction_id);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "time_out_value_DSI");
										sprintf_s(pszText, sizeof(pszText), "0x%08X(%d)", pobject_carousel_info->time_out_value_DSI, pobject_carousel_info->time_out_value_DSI);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "time_out_value_DII");
										sprintf_s(pszText, sizeof(pszText), "0x%08X(%d)", pobject_carousel_info->time_out_value_DII, pobject_carousel_info->time_out_value_DII);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;

										listCtrl.InsertItem(nItem, "");
										listCtrl.SetItemText(nItem, 1, "leak_rate");
										sprintf_s(pszText, sizeof(pszText), "0x%06X(%d)", pobject_carousel_info->leak_rate, pobject_carousel_info->leak_rate);
										listCtrl.SetItemText(nItem, 2, pszText);
										nItem++;
									}
								}

								listCtrl.InsertItem(nItem, "");
								nItem++;
							}
						}
					}
				}
			}
		}
	}
}

int CListView_ServiceInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CListCtrl&		listCtrl = GetListCtrl();

	listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	listCtrl.InsertColumn(0, "", LVCFMT_LEFT, 20, -1);
	listCtrl.InsertColumn(1, "条目", LVCFMT_RIGHT, 240, -1);
	listCtrl.InsertColumn(2, "描述", LVCFMT_LEFT, 400, -1);
	
	return 0;
}


void CListView_ServiceInfo::OnSize(UINT nType, int cx, int cy)
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
