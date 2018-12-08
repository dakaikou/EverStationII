// Dlg_Overview.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_TSAnalyzer_Overview.h"
#include "TSMagicView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <math.h>

#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Utilities.h"
#include "translate_layer/MPEG2_DVB_Section/Include/DVB_SI_Utilities.h"

#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"

#include "..\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Overview dialog

CDlg_TSAnalyzer_Overview::CDlg_TSAnalyzer_Overview(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Overview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Overview)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_usActualTSID = 0x0000;
	m_nServiceCount = 0;
}


void CDlg_TSAnalyzer_Overview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Overview)
	// NOTE: the ClassWizard will add DDX and DDV calls here
//	DDX_Control(pDX, IDC_STATIC_TSRATE_OSCILLOSCOPE, m_TsRateJitterGraphOscilloscope);
	//DDX_Control(pDX, IDC_STATIC_TSRATE_HISTOGRAM, m_TsRateJitterGraphHistogram);
	DDX_Control(pDX, IDC_LIST_TS_MONITOR, m_listMonitor);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_TS_BANDWIDTH, m_listBandwidth);
	DDX_Control(pDX, IDC_LIST_TS_OVERVIEW, m_listTSOverview);
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Overview, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Overview)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_RADIO_SMARTTS, OnBnClickedRadioSmartts)
//	ON_BN_CLICKED(IDC_RADIO_DEKTEC, OnBnClickedRadioDektec)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TS_BANDWIDTH, &CDlg_TSAnalyzer_Overview::OnLvnItemchangedListTsBandwidth)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Overview message handlers

BOOL CDlg_TSAnalyzer_Overview::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_nRunning = 0;

	CRect rectHisgram;
	CRect rectTemp;

	CWnd* pWnd = GetDlgItem(IDC_LIST_TS_MONITOR);
	pWnd->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectHisgram.top = rectTemp.top;

	pWnd = GetDlgItem(IDC_LIST_TS_BANDWIDTH);
	pWnd->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectHisgram.right = rectTemp.right;

	pWnd = GetDlgItem(IDC_LIST_TS_OVERVIEW);
	pWnd->GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectHisgram.left = rectTemp.left;
	rectHisgram.bottom = rectTemp.top - 40;

	if (!m_TsRateJitterGraphHistogram.Create("TS流码率统计分析", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, rectHisgram, this))
	{
		TRACE0("未能创建TS流码率监控窗口\n");
		return FALSE; // 未能创建
	}
	m_TsRateJitterGraphHistogram.Init_X_Axis(AXIS_STYLE_MEAN_SYMMETRY, RANGE_MARK_SHOWN, -100, 100, "bps", -10000, 10000);	//X轴（-1000， 1000）bps，Y轴（0， 100%）比例			
	m_TsRateJitterGraphHistogram.Init_Y_Axis(AXIS_STYLE_FROM_MIN_TO_MAX, RANGE_MARK_HIDE, 0, 100, "%", 0, 100);	//X轴（-1000， 1000）bps，Y轴（0， 100%）比例			

	m_nEtr290Count = 0;

	m_listMonitor.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	
	m_listMonitor.InsertColumn(0, "", LVCFMT_CENTER, 20, -1);					//ICON

	m_listMonitor.InsertColumn(1, "序号", LVCFMT_LEFT, 60, -1);					//ICON
	m_listMonitor.InsertColumn(2, "时间", LVCFMT_LEFT, 180, -1);
	m_listMonitor.InsertColumn(3, "信息", LVCFMT_LEFT, 560, -1);

	m_imageDebug.Create(IDB_LOG, 16, 1, RGB(0xab, 0xcd, 0xef));
	m_listMonitor.SetImageList(&m_imageDebug, LVSIL_SMALL);


	m_listBandwidth.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	
	m_listBandwidth.InsertColumn(0, "", LVCFMT_CENTER, 16, -1);					//ICON
	m_listBandwidth.InsertColumn(1, "业务ID", LVCFMT_RIGHT, 120, -1);					
	m_listBandwidth.InsertColumn(2, "比例(%)", LVCFMT_RIGHT, 80, -1);
	m_listBandwidth.InsertColumn(3, "带宽(Kbps)", LVCFMT_RIGHT, 100, -1);

	m_listTSOverview.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	
	m_listTSOverview.InsertColumn(0, "", LVCFMT_CENTER, 16, -1);					//ICON
	m_listTSOverview.InsertColumn(1, "", LVCFMT_RIGHT, 75, -1);					//ICON
	m_listTSOverview.InsertColumn(2, "条目", LVCFMT_LEFT, 75, -1);					
	m_listTSOverview.InsertColumn(3, "描述", LVCFMT_LEFT, 100, -1);

	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Overview::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	CRect	rect;

	//if (m_TsRateJitterGraphHistogram.GetSafeHwnd() != NULL)
	//{
	//	m_TsRateJitterGraphHistogram.GetWindowRect(&rect);
	//	ScreenToClient(&rect);

	//	m_TsRateJitterGraphHistogram.MoveWindow(&rect);			//保持原来的位置和大小，触发OnSize()函数的调用
	//}

	pWnd = GetDlgItem(IDC_LIST_TS_OVERVIEW);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 5, 0);

		pWnd->GetWindowRect(&rect);
		int width = rect.Width();

		CHeaderCtrl* pHeaderCtrl = m_listTSOverview.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			width -= m_listTSOverview.GetColumnWidth(i);
			width -= 2;
		}
		m_listTSOverview.SetColumnWidth(columns - 1, width);
	}

	pWnd = GetDlgItem(IDC_LIST_TS_BANDWIDTH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cy - rect.top - 5, 0);
	}

	pWnd = GetDlgItem(IDC_LIST_TS_MONITOR);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top, cx - rect.left - 10, cy - rect.top - 5, 0);

		pWnd->GetWindowRect(&rect);

		int width = rect.Width();

		CHeaderCtrl* pHeaderCtrl = m_listMonitor.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			width -= m_listMonitor.GetColumnWidth(i);
			width -= 2;
		}
		m_listMonitor.SetColumnWidth(columns - 1, width);
	}
}

void CDlg_TSAnalyzer_Overview::Reset(void)
{
	CWnd* pWnd = NULL;

	m_listMonitor.DeleteAllItems();
	m_listBandwidth.DeleteAllItems();
	m_listTSOverview.DeleteAllItems();

	int nCount = 0;
	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "当前网络");
	nCount++;

	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "+--");
	m_listTSOverview.SetItemText(nCount, 2, "网络名");
	m_listTSOverview.SetItemText(nCount, 3, "");
	nCount++;

	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "+--");
	m_listTSOverview.SetItemText(nCount, 2, "ID");
	m_listTSOverview.SetItemText(nCount, 3, "");
	nCount++;

	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "当前TS流");
	nCount++;

	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "+--");
	m_listTSOverview.SetItemText(nCount, 2, "包长度");
	m_listTSOverview.SetItemText(nCount, 3, "");
	nCount++;

	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "+--");
	m_listTSOverview.SetItemText(nCount, 2, "ID");
	m_listTSOverview.SetItemText(nCount, 3, "");
	nCount++;

	m_listTSOverview.InsertItem(nCount, "");
	m_listTSOverview.SetItemText(nCount, 1, "+--");
	m_listTSOverview.SetItemText(nCount, 2, "调制参数");
	m_listTSOverview.SetItemText(nCount, 3, "");
	nCount++;

	m_TsRateJitterGraphHistogram.Reset();

	m_nServiceCount = 0;

	m_nRunning = 0;
}

void CDlg_TSAnalyzer_Overview::Set(int offline)
{
	CWnd* pWnd = NULL;

//	pWnd = (CWnd*)GetDlgItem(IDC_RADIO_SMARTTS);
//	pWnd->EnableWindow(FALSE);

//	pWnd = (CWnd*)GetDlgItem(IDC_RADIO_DEKTEC);
//	pWnd->EnableWindow(FALSE);

	m_nRunning = 1;
}

void CDlg_TSAnalyzer_Overview::UpdateSDT(CSDT* pSDT)
{
/*
	CWnd*	pWnd = (CWnd*)GetDlgItem(IDC_STATIC_SERVICE_COUNT);
	CString	str;

	if (m_nRunning == 1)
	{
		if (pSDT != NULL)
		{
			m_nServiceCount += pSDT->m_nServiceCount;

			str.Format("%d", m_nServiceCount);
			pWnd->SetWindowText(str);
		}
		else
		{
			m_nServiceCount = 0;
			pWnd->SetWindowText(_T(""));
		}
	}
*/
}

void CDlg_TSAnalyzer_Overview::UpdateNIT(CNIT* pNIT)
{
	char	pszText[64];
	char	pszNetworkName[64];
	char	pszFrequency[64];
	char	pszSymbolRate[64];
	char	pszModulation[64];
	char	pszPolarization[64];
	char	pszModulationParam[256];
	STREAM_INFO_t	stStreamInfo;
	int			nCount;
	int			i;
	CString		strTemp;
	CString		strNewID;
	CString		strNewName;
	CString		strNewModulation;
	CString		strOld;
	int			find_item = 0;

	//char*			pstr_dst;

	CString		strClassNetwork = _T("当前网络");
	CString		strClassTS = _T("当前TS流");

	CString     strItemTSID = _T("ID");
	CString     strItemModulation = _T("调制参数");

	CString     strItemNetworkID = _T("ID");
	CString     strItemNetworkName = _T("网络名");

	if (m_nRunning == 1)
	{
		if (pNIT != NULL)
		{
			pNIT->GetNetworkName(pszNetworkName, sizeof(pszNetworkName));
			strNewName = pszNetworkName;

			uint16_t usNetworkID = pNIT->GetNetworkID();
			sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", usNetworkID, usNetworkID);
			strNewID = pszText;

			memset(pszModulationParam, 0x00, sizeof(pszModulationParam));
			int rtcode = pNIT->GetStreamByID(m_usActualTSID, &stStreamInfo);
			if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
			{
				if (stStreamInfo.uDelivery.cable_delivery_system_descriptor.descriptor_tag == DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR)
				{
					cable_delivery_system_descriptor_t* pcable_delivery_system_descriptor = &(stStreamInfo.uDelivery.cable_delivery_system_descriptor);
					sprintf_s(pszFrequency, sizeof(pszFrequency), "%x.%04xMHz", 
						(pcable_delivery_system_descriptor->frequency & 0xFFFF0000) >> 16, 
						pcable_delivery_system_descriptor->frequency & 0x0000FFFF);
					sprintf_s(pszSymbolRate, sizeof(pszSymbolRate), "%x.%04xMsps", 
						(pcable_delivery_system_descriptor->symbol_rate & 0xFFFF0000) >> 16, 
						pcable_delivery_system_descriptor->symbol_rate & 0x0000FFFF);

					DVB_SI_NumericCoding2Text_CableModulation(pcable_delivery_system_descriptor->modulation, pszModulation, sizeof(pszModulation));
					sprintf_s(pszModulationParam, sizeof(pszModulationParam), "%s,%s,%s", pszFrequency, pszSymbolRate, pszModulation);
				}
				else if (stStreamInfo.uDelivery.satellite_delivery_system_descriptor.descriptor_tag == DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR)
				{
					satellite_delivery_system_descriptor_t* psatellite_delivery_system_descriptor = &(stStreamInfo.uDelivery.satellite_delivery_system_descriptor);
					sprintf_s(pszFrequency, sizeof(pszFrequency), "%X.%05XGHz", 
						(psatellite_delivery_system_descriptor->frequency & 0xFFF00000) >> 20, 
						psatellite_delivery_system_descriptor->frequency & 0x000FFFFF);
					sprintf_s(pszSymbolRate, sizeof(pszSymbolRate), "%X.%04XMsps", 
						(psatellite_delivery_system_descriptor->symbol_rate & 0xFFFF0000) >> 16, 
						psatellite_delivery_system_descriptor->symbol_rate & 0x0000FFFF);

					DVB_SI_NumericCoding2Text_SatelliteModulationType(psatellite_delivery_system_descriptor->modulation_type, pszModulation, sizeof(pszModulation));
					DVB_SI_NumericCoding2Text_SatellitePolarization(psatellite_delivery_system_descriptor->polarization, pszPolarization, sizeof(pszPolarization));
					sprintf_s(pszModulationParam, sizeof(pszModulationParam), "%s,%s,%s,%s", pszFrequency, pszSymbolRate, pszModulation, pszPolarization);
				}
				strNewModulation = pszModulationParam;
			}

			nCount = m_listTSOverview.GetItemCount();
			for (i = 0; i < nCount; i++)
			{
				strTemp = m_listTSOverview.GetItemText(i, 1);
				if (strTemp == strClassNetwork)
				{
					for (int j = i + 1; j < nCount; j++)
					{
						strTemp = m_listTSOverview.GetItemText(j, 2);
						if (strTemp == strItemNetworkName)
						{
							strOld = m_listTSOverview.GetItemText(j, 3);

							if (strOld != strNewName)
							{
								m_listTSOverview.SetItemText(j, 3, strNewName);
							}

							break;
						}
					}

					for (int j = i + 1; j < nCount; j++)
					{
						strTemp = m_listTSOverview.GetItemText(j, 2);
						if (strTemp == strItemNetworkID)
						{
							strOld = m_listTSOverview.GetItemText(j, 3);

							if (strOld != strNewID)
							{
								m_listTSOverview.SetItemText(j, 3, strNewID);
							}

							break;
						}
					}

					for (int j = i + 1; j < nCount; j++)
					{
						strTemp = m_listTSOverview.GetItemText(j, 2);
						if (strTemp == strItemModulation)
						{
							strOld = m_listTSOverview.GetItemText(j, 3);

							if (strOld != strNewModulation)
							{
								m_listTSOverview.SetItemText(j, 3, strNewModulation);
							}

							break;
						}
					}

					break;
				}
			}

		}
	}
}

void CDlg_TSAnalyzer_Overview::UpdatePAT(CPAT* pPAT)
{
	char		pszText[64];
	int			nCount;
	int			i;
	CString		strTemp;
	CString		strNew;
	CString		strOld;
	int			find_item = 0;

	CString     strClass = _T("当前TS流");
	//CString     strItemPakcetLength = _T("包长度");
	CString     strItemID = _T("ID");

	if (m_nRunning == 1)
	{
		if (pPAT != NULL)
		{
			m_usActualTSID = pPAT->GetTransportStreamID();

			sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", m_usActualTSID, m_usActualTSID);
			strNew = pszText;

			nCount = m_listTSOverview.GetItemCount();
			for (i = 0; i < nCount; i++)
			{
				strTemp = m_listTSOverview.GetItemText(i, 1);
				if (strTemp == strClass)
				{
					for (int j = i+1; j < nCount; j++)
					{
						strTemp = m_listTSOverview.GetItemText(j, 2);
						if (strTemp == strItemID)
						{
							strOld = m_listTSOverview.GetItemText(j, 3);

							if (strOld != strNew)
							{
								m_listTSOverview.SetItemText(j, 3, strNew);
							}

							find_item = 1;
							break;
						}
					}

					break;
				}
			}

			//if (find_item == 0)
			//{
			//	int find_class = 0;
			//	for (i = 0; i < nCount; i++)
			//	{
			//		strTemp = m_listTSOverview.GetItemText(i, 2);
			//		if (strTemp == strItemPakcetLength)					//固定插在包长度条目的下一条
			//		{
			//			int insert_index = i + 1;

			//			m_listTSOverview.InsertItem(insert_index, "");
			//			m_listTSOverview.SetItemText(insert_index, 1, "+--");
			//			m_listTSOverview.SetItemText(insert_index, 2, strItemID);
			//			m_listTSOverview.SetItemText(insert_index, 3, pszText);

			//			find_class = 1;
			//			break;
			//		}
			//	}

			//	if (find_class == 0)
			//	{
			//		m_listTSOverview.InsertItem(nCount, "");
			//		m_listTSOverview.SetItemText(nCount, 1, strClass);
			//		nCount++;

			//		m_listTSOverview.InsertItem(nCount, "");
			//		m_listTSOverview.SetItemText(nCount, 1, "+--");
			//		m_listTSOverview.SetItemText(nCount, 2, strItemID);
			//		m_listTSOverview.SetItemText(nCount, 3, pszText);
			//	}
			//}
		}
	}
}

void CDlg_TSAnalyzer_Overview::UpdatePacketLength(int packet_length)
{
	char					pszText[64];
	int						nCount;
	int						i;

	CString     strClass = _T("当前TS流");
	CString     strItemPakcetLength = _T("包长度");
	//CString     strItemID = _T("ID");

	CString		strTemp;
	CString		strNew;
	CString		strOld;
	int			find_item = 0;

	sprintf_s(pszText, sizeof(pszText), "%d字节", packet_length);
	strNew = pszText;

	if (m_nRunning == 1)
	{
		nCount = m_listTSOverview.GetItemCount();
		for (i = 0; i < nCount; i++)
		{
			strTemp = m_listTSOverview.GetItemText(i, 1);
			if (strTemp == strClass)
			{
				for (int j = i+1; j < nCount; j++)
				{
					strTemp = m_listTSOverview.GetItemText(j, 2);
					if (strTemp == strItemPakcetLength)
					{
						strOld = m_listTSOverview.GetItemText(j, 3);

						if (strOld != strNew)
						{
							m_listTSOverview.SetItemText(j, 3, strNew);
						}

						find_item = 1;
						break;
					}
				}

				break;
			}
		}

		//if (find_item == 0)
		//{
		//	int find_class = 0;
		//	for (i = 0; i < nCount; i++)
		//	{
		//		strTemp = m_listTSOverview.GetItemText(i, 1);
		//		if (strTemp == strClass)
		//		{
		//			int insert_index = i + 1;
		//			m_listTSOverview.InsertItem(insert_index, "");
		//			m_listTSOverview.SetItemText(insert_index, 1, "+--");
		//			m_listTSOverview.SetItemText(insert_index, 2, strItemPakcetLength);
		//			m_listTSOverview.SetItemText(insert_index, 3, pszText);

		//			find_class = 1;
		//			break;
		//		}
		//	}

		//	if (find_class == 0)
		//	{
		//		m_listTSOverview.InsertItem(nCount, "");
		//		m_listTSOverview.SetItemText(nCount, 1, strClass);
		//		nCount++;

		//		m_listTSOverview.InsertItem(nCount, "");
		//		m_listTSOverview.SetItemText(nCount, 1, "+--");
		//		m_listTSOverview.SetItemText(nCount, 2, strItemPakcetLength);
		//		m_listTSOverview.SetItemText(nCount, 3, pszText);
		//	}
		//}
	}
}

void CDlg_TSAnalyzer_Overview::UpdateBitrateSample(int current, BITRATE_ATTRIBUTE_t* pattr)
{
	if (m_nRunning == 1)
	{
		SAMPLE_ATTRIBUTE_t attr;
		attr.max = pattr->max;
		attr.min = pattr->min;
		attr.mean = pattr->mean;
		attr.rms = pattr->rms;
		m_TsRateJitterGraphHistogram.AppendSample(0, current, &attr);
	}
}

void CDlg_TSAnalyzer_Overview::AppendEtr290Log(int type, const char* pszItem)
{
	CString			strTime;
	COleDateTime	timeCurrent;
	int				nCount;
	char			pszText[MAX_TXT_CHARS];

	if (pszItem != NULL)
	{
		nCount = m_listMonitor.GetItemCount();
		if (nCount < MAX_LOG_COUNT)
		{
			timeCurrent = COleDateTime::GetCurrentTime();

			m_listMonitor.InsertItem(nCount, "", type);

			sprintf_s(pszText, sizeof(pszText), "%d", m_nEtr290Count ++);
			m_listMonitor.SetItemText(nCount, 1, pszText);

			strTime = timeCurrent.Format("%Y-%m-%d-%H:%M:%S");
			m_listMonitor.SetItemText(nCount, 2, strTime);

			m_listMonitor.SetItemText(nCount, 3, pszItem);

			m_listMonitor.EnsureVisible(nCount, 0);
		}
	}
}

void CDlg_TSAnalyzer_Overview::UpdatePacketInfo(void)
{
	int				nItemCount;
	int				item_index;
	//int				es_index;
	unsigned int	data;
	int				program_number;
	int				program_index;
	//CPMT*			pPMT;
	//int				PID;
	int				nTotalPkCount;
	//int				nServicePkCount;
	//float			fratio;
	int				nTotalDataRate;
	//float			fServiceDataRate;
	//char			pszText[MAX_TXT_CHARS];

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	//RECORD_TSPacket_t	TSPacketRecord;
	CDB_TSPackets*	pDB_TSPackets = pTSMagicView->GetTSPacketsDBase();
	CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();

	if (m_nRunning == 1)
	{
		nItemCount = m_listBandwidth.GetItemCount();
		nTotalPkCount = pDB_TSPackets->m_total_packet_count;

		if (pDB_TSPackets->m_total_bitrate_from_hardware >= 1000000000)
		{
			nTotalDataRate = pDB_TSPackets->m_total_bitrate_from_software;
		}
		else
		{
			nTotalDataRate = pDB_TSPackets->m_total_bitrate_from_hardware;
		}

		for (item_index = 0; item_index < nItemCount; item_index ++)
		{
			data = (uint32_t)m_listBandwidth.GetItemData(item_index);

			program_index = (data & 0xFFFF0000) >> 16;
			program_number = (data & 0x0000FFFF);

#if DEBUG_PMT
//			pPMT = pDB_PsiSiTables->GetPMT(program_index);
//
//			nServicePkCount = 0;
//			for (es_index = 0; es_index < pPMT->m_nEsCount; es_index ++)
//			{
//				PID = pPMT->m_astEsInfo[es_index].elementary_PID;
//
////				pTSPacketRecord = pDB_TSPackets->m_pRecords[PID];
//				rtcode = pDB_TSPackets->GetRecord(PID, &TSPacketRecord);
//				if (rtcode == NO_ERROR)
//				{
//					nServicePkCount += TSPacketRecord.count;
//				}
//			}
//
//			fratio = (float)nServicePkCount / nTotalPkCount;
//			sprintf_s(pszText, sizeof(pszText), "%5.2f", fratio * 100);
//			m_listBandwidth.SetItemText(item_index, 2, pszText);
//
//			fServiceDataRate = fratio * nTotalDataRate;
//
//			sprintf_s(pszText, sizeof(pszText), "%10.3f", fServiceDataRate / 1000);
//			m_listBandwidth.SetItemText(item_index, 3, pszText);
#endif
		}
	}
}

void CDlg_TSAnalyzer_Overview::UpdatePMT(CPMT* pPMT)
{
	char			pszText[MAX_TXT_CHARS];
	int				nItemCount;
	unsigned int	data;
	int				item_index;
	uint16_t		program_number;

	if (m_nRunning == 1)
	{
		if (pPMT != NULL)
		{
			uint16_t usProgramNumber = pPMT->GetProgramNumber();
			nItemCount = m_listBandwidth.GetItemCount();

			for (item_index = 0; item_index < nItemCount; item_index ++)
			{
				data = (uint32_t)m_listBandwidth.GetItemData(item_index);
				program_number = (data & 0x0000FFFF);

				if (usProgramNumber < program_number)
				{
					break;
				}
			}

			sprintf_s(pszText, sizeof(pszText), "%d-(0x%04X)", usProgramNumber, usProgramNumber);
			m_listBandwidth.InsertItem(item_index, "");
			m_listBandwidth.SetItemText(item_index, 1, pszText);
			data = pPMT->GetKey();
			data <<= 16;
			data |= usProgramNumber;
			m_listBandwidth.SetItemData(item_index, data);

	//		for (i = 0; i < pPMT->m_nEsCount; i++)
	//		{
	//			nItemCount = m_listBandwidth.GetItemCount();
	//			sprintf_s(pszText, sizeof(pszText), "0x%04X(%d)", pPMT->m_astESMap[i].elementary_PID, pPMT->m_astESMap[i].elementary_PID);
	//			m_listBandwidth.InsertItem(nItemCount, "");
	//			m_listBandwidth.SetItemText(nItemCount, 2, pszText);
	//			data = (0xFFFF0000 | pPMT->m_astESMap[i].elementary_PID);
	//			m_listBandwidth.SetItemData(nItemCount, data);
	//		}
		}
	}
}

/*
void CDlg_TSAnalyzer_Overview::OnBnClickedRadioSmartts()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nDeviceIndex = 0;
}

void CDlg_TSAnalyzer_Overview::OnBnClickedRadioDektec()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nDeviceIndex = 1;
}
*/



