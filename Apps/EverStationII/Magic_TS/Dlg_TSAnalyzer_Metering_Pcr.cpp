// Dlg_Pcr.cpp : implementation file
//

#include <stdafx.h>
#include <assert.h>

#include "TSMagicView.h"
#include "Dlg_TSAnalyzer_Metering_Pcr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Metering_Pcr dialog
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"

#define ITEM_TEXT_ALARM_COLOR		RGB(255, 0, 0)
#define ITEM_TEXT_NORMAL_COLOR		RGB(0, 0, 0)

enum _PCR_ITEM_e
{
	PCR_ITEM_PCR_PID = 0,
	PCR_ITEM_H32_30_CODE_VALUE,
	PCR_ITEM_H29_15_CODE_VALUE,
	PCR_ITEM_NEGATIVE_DISCONTINUITY,
	PCR_ITEM_POSITIVE_DISCONTINUITY,
	PCR_ITEM_JITTER_MEAN_VALUE,
	PCR_ITEM_JITTER_VAR_VALUE,
	PCR_ITEM_JITTER_ERROR_COUNT,
	PCR_ITEM_INTERVAL_MEAN_VALUE,
	PCR_ITEM_INTERVAL_MAX_VALUE,
	PCR_ITEM_INTERVAL_ERROR_COUNT,
	PCR_ITEM_DATARATE_MEAN_VALUE,
	PCR_ITEM_DATARATE_VAR_VALUE,
} PCR_ITEM_e;

CDlg_TSAnalyzer_Metering_Pcr::CDlg_TSAnalyzer_Metering_Pcr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Metering_Pcr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Metering_Pcr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	m_nPcrIndex = -1;
}


void CDlg_TSAnalyzer_Metering_Pcr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Metering_Pcr)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Metering_Pcr, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Metering_Pcr)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Metering_Pcr message handlers

BOOL CDlg_TSAnalyzer_Metering_Pcr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		int		nColIndex = 0;

		m_nLayoutColCount = 1;
#if SHOW_PCR_SCATTER_DIAGRAM
		m_nLayoutColCount++;
#endif // SHOW_PCR_SCATTER_DIAGRAM

#if SHOW_PCR_JITTER_HISTGRAM || SHOW_PCR_INTERVAL_HISTGRAM
		m_nLayoutColCount++;
#endif

		m_wndSplitter.CreateStatic(this, 1, m_nLayoutColCount);

		m_wndSplitter.CreateView(0, nColIndex, RUNTIME_CLASS(CNaviList_PcrGuide), CSize(NAVI_PANE_WIDTH, 0), NULL);
		m_pNaviPane = (CNaviList_PcrGuide*)m_wndSplitter.GetPane(0, nColIndex);
		nColIndex++;

#if SHOW_PCR_SCATTER_DIAGRAM
		m_wndSplitter.CreateView(0, nColIndex, RUNTIME_CLASS(CInstrumentView_ScatterDiagram), CSize(400, 0), NULL);
		m_pPcrScatterDiagramGraph = (CInstrumentView_ScatterDiagram*)m_wndSplitter.GetPane(0, nColIndex);
		nColIndex++;
#endif

#if (SHOW_PCR_JITTER_HISTGRAM || SHOW_PCR_INTERVAL_HISTGRAM)
#if (SHOW_PCR_JITTER_HISTGRAM && SHOW_PCR_INTERVAL_HISTGRAM)
		m_wndRightSplitter.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, nColIndex));
		m_wndRightSplitter.CreateView(0, 0, RUNTIME_CLASS(CInstrumentView_Histogram), CSize(0, 400), NULL);
		m_wndRightSplitter.CreateView(1, 0, RUNTIME_CLASS(CInstrumentView_Histogram), CSize(0, 0), NULL);
		m_pPcrJitterHistgramGraph = (CInstrumentView_Histogram*)m_wndRightSplitter.GetPane(0, 0);
		m_pPcrIntervalHistgramGraph = (CInstrumentView_Histogram*)m_wndRightSplitter.GetPane(1, 0);
#else
#if SHOW_PCR_JITTER_HISTGRAM
		m_wndSplitter.CreateView(0, nColIndex, RUNTIME_CLASS(CInstrumentView_Histogram), CSize(0, 0), NULL);
		m_pPcrJitterHistgramGraph = (CInstrumentView_Histogram*)m_wndSplitter.GetPane(0, nColIndex);
#else
		m_wndSplitter.CreateView(0, nColIndex, RUNTIME_CLASS(CInstrumentView_Histogram), CSize(0, 0), NULL);
		m_pPcrIntervalHistgramGraph = (CInstrumentView_Histogram*)m_wndSplitter.GetPane(0, nColIndex);
#endif
#endif
#endif

		CRect rectClient;

		GetClientRect(&rectClient);
		rectClient.left += 5;
		rectClient.right -= 5;
		rectClient.top += 5;
		rectClient.bottom -= 5;

		m_wndSplitter.MoveWindow(&rectClient);

		//m_pNaviPane->Set(this->GetSafeHwnd());

#if SHOW_PCR_JITTER_HISTGRAM
		m_pPcrJitterHistgramGraph->SetTitle("PCR抖动 直方图");
		//m_pPcrJitterHistgramGraph->Init_X_Axis(AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, -500, 500, "ns", -100000, 100000, 1000);				//均值点对称型
		m_pPcrJitterHistgramGraph->Init_X_Axis(AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_KEYPOINT, -500, 500, "ns", -100000, 100000, 1000);				//均值点对称型
		m_pPcrJitterHistgramGraph->Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 100, "%", 0, 100);				//
#endif

//#if SHOW_PCR_JITTER_WAVEFORM
//	if (!m_PcrJitterWaveformGraph.Create("PCR_AC 波形图", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, rectPcrJitterWaveform, this))
//	{
//		TRACE0("未能创建TS流码率监控窗口\n");
//		return FALSE; // 未能创建
//	}
//	m_PcrJitterWaveformGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_HIDE, 0, -1, "s", 0, -1);				//0到正无穷
//	m_PcrJitterWaveformGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY, RANGE_MARK_SHOWN, -500, 500, "ns", -10000, 10000);				//0点对称型
//#endif

#if SHOW_PCR_SCATTER_DIAGRAM
		m_pPcrScatterDiagramGraph->SetTitle("PCR_AC 散点图");
		m_pPcrScatterDiagramGraph->Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_SHOWN | RANGE_MARK_SHOWN_PARTIAL, 0, 40000, "us", 0, 160000, 20000);				//单向从小到大型
		m_pPcrScatterDiagramGraph->Init_Y_Axis(AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY, MEASURE_PANEL_SHOWN | RANGE_MARK_SHOWN_PARTIAL, -500, 500, "ns", -100000, 100000, 1000);				//0点对称型
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
		m_pPcrIntervalHistgramGraph->SetTitle("PCR间隔 直方图");
		m_pPcrIntervalHistgramGraph->Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_KEYPOINT, 0, 40000, "ms", 0, 160000, 20000);			//非对称型，DVB要求40ms，MPEG-2要求100ms
		m_pPcrIntervalHistgramGraph->Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 100, "%", 0, 100);			//非对称型，0-100%
#endif

//#if SHOW_PCR_INTERVAL_WAVEFORM
//	if (!m_PcrIntervalWaveformGraph.Create("PCR间隔 波形图", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, rectPcrIntervalWaveform, this))
//	{
//		TRACE0("未能创建TS流码率监控窗口\n");
//		return FALSE; // 未能创建
//	}
//	m_PcrIntervalWaveformGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_HIDE, 0, -1, "ms", 0, -1);			//0到正无穷
//	m_PcrIntervalWaveformGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_SHOWN, 0, 40, "ms", 0, 150, 10);			//非对称型
//#endif

		Reset();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Metering_Pcr::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);

		m_wndSplitter.SetColumnInfo(0, NAVI_PANE_WIDTH, 100);

		int n2ndPaneWidth = cx - NAVI_PANE_WIDTH - 10;
		if (m_nLayoutColCount == 3)
		{
			n2ndPaneWidth = cx - NAVI_PANE_WIDTH - JITTER_OR_INTERVAL_PANE_WIDTH - 10;
		}

		m_wndSplitter.SetColumnInfo(1, n2ndPaneWidth, 100);

		if (m_nLayoutColCount == 3)
		{
			m_wndSplitter.SetColumnInfo(2, JITTER_OR_INTERVAL_PANE_WIDTH, 100);
		}

		int rowHeight = (cy - 10) / 2;
		if (m_wndRightSplitter.GetSafeHwnd() != NULL)
		{
			m_wndRightSplitter.SetRowInfo(0, rowHeight, 100);
			m_wndRightSplitter.SetRowInfo(1, rowHeight, 100);
			m_wndRightSplitter.RecalcLayout();
		}

		m_wndSplitter.RecalcLayout();
	}
}


void CDlg_TSAnalyzer_Metering_Pcr::UpdatePCRDiagnosis(RECORD_PCR_t* pCurPcrInfo)
{
	char			pszText[MAX_PATH];
	int				nItemCount;
	int				nOffset;

	if (pCurPcrInfo != NULL)
	{
		sprintf_s(pszText, MAX_PATH, "0x%04X", pCurPcrInfo->PCR_PID);

		LVFINDINFO info;
		info.flags = LVFI_PARTIAL | LVFI_STRING;
		info.psz = pszText;

		CListCtrl& listPcrLog = m_pNaviPane->GetListCtrl();
		nItemCount = listPcrLog.GetItemCount();
		nOffset = listPcrLog.FindItem(&info);
		if (nOffset != -1)
		{
			//找到匹配PCR_PID
			DWORD oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_H32_30_CODE_VALUE);
			if (pCurPcrInfo->PCR_code.base_32_30 != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%X", pCurPcrInfo->PCR_code.base_32_30);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_H32_30_CODE_VALUE, pCurPcrInfo->PCR_code.base_32_30);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_H29_15_CODE_VALUE);
			if (pCurPcrInfo->PCR_code.base_29_15 != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%04X", pCurPcrInfo->PCR_code.base_29_15);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_H29_15_CODE_VALUE, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_H29_15_CODE_VALUE, pCurPcrInfo->PCR_code.base_29_15);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY);
			if (pCurPcrInfo->pcr_code_negative_discontinuity_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_code_negative_discontinuity_count);
				listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_negative_discontinuity_count);
				listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY);
			if (pCurPcrInfo->pcr_code_positive_discontinuity_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_code_positive_discontinuity_count);
				listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_positive_discontinuity_count);
				listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_JITTER_MEAN_VALUE);
			if (pCurPcrInfo->jitter_mean_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_mean_value);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, pCurPcrInfo->jitter_mean_value);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_JITTER_VAR_VALUE);
			if (pCurPcrInfo->jitter_rms_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_rms_value);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_VAR_VALUE, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_VAR_VALUE, pCurPcrInfo->jitter_rms_value);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_JITTER_ERROR_COUNT);
			if (pCurPcrInfo->pcr_ac_error_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_ac_error_count);
				listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, pCurPcrInfo->pcr_ac_error_count);
				listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE);
			if (pCurPcrInfo->interval_mean_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_mean_value);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, pCurPcrInfo->interval_mean_value);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE);
			if (pCurPcrInfo->interval_max_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_max_value);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, pCurPcrInfo->interval_max_value);
			}

			oldValue = (uint32_t)listPcrLog.GetItemData(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT);
			if (pCurPcrInfo->pcr_interval_error_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_interval_error_count);
				listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, 3, pszText);
				listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, pCurPcrInfo->pcr_interval_error_count);
				listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			//oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE);
			//if (pCurPcrInfo->encoder_bitrate_mean_value != oldValue)
			//{
			//	sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_mean_value);
			//	listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 3, pszText);
			//	listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, pCurPcrInfo->encoder_bitrate_mean_value);
			//}

			//oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_DATARATE_VAR_VALUE);
			//if (pCurPcrInfo->encoder_bitrate_rms_value != oldValue)
			//{
			//	sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_rms_value);
			//	listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, 3, pszText);
			//	listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, pCurPcrInfo->encoder_bitrate_rms_value);
			//}
		}
		else
		{
			//未找到匹配PCR_PID
			nOffset = nItemCount;

			listPcrLog.InsertItem(nOffset + PCR_ITEM_PCR_PID, pszText);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_PCR_PID, 1, "------");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_PCR_PID, 2, "--------");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_PCR_PID, 3, "---------------");
			listPcrLog.SetItemData(nOffset + PCR_ITEM_PCR_PID, pCurPcrInfo->PCR_PID);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_H32_30_CODE_VALUE, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 1, "PCR编码");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 2, "(32-30位)");
			sprintf_s(pszText, sizeof(pszText), "%X", pCurPcrInfo->PCR_code.base_32_30);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_H32_30_CODE_VALUE, pCurPcrInfo->PCR_code.base_32_30);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_H29_15_CODE_VALUE, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_H29_15_CODE_VALUE, 2, "(29-15位)");
			sprintf_s(pszText, sizeof(pszText), "%04X", pCurPcrInfo->PCR_code.base_29_15);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_H29_15_CODE_VALUE, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_H29_15_CODE_VALUE, pCurPcrInfo->PCR_code.base_29_15);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, 2, "负跳变");
			sprintf_s(pszText, MAX_PATH, "%d 次", pCurPcrInfo->pcr_code_negative_discontinuity_count);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_negative_discontinuity_count);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, 2, "正跳变");
			sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_code_positive_discontinuity_count);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_positive_discontinuity_count);


			listPcrLog.InsertItem(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 1, "PCR抖动");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 2, "均值");
			sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_mean_value);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, pCurPcrInfo->jitter_mean_value);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_JITTER_VAR_VALUE, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_VAR_VALUE, 2, "方差");
			sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_rms_value);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_VAR_VALUE, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_VAR_VALUE, pCurPcrInfo->jitter_rms_value);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, 2, "异常");
			sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_ac_error_count);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, pCurPcrInfo->pcr_ac_error_count);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 1, "PCR间隔");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 2, "均值");
			sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_mean_value);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, pCurPcrInfo->interval_mean_value);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, 2, "最大值");
			sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_max_value);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, pCurPcrInfo->interval_max_value);

			listPcrLog.InsertItem(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, "");
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, 2, "异常");
			sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_interval_error_count);
			listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, 3, pszText);
			listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, pCurPcrInfo->pcr_interval_error_count);

			//listPcrLog.InsertItem(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, "");
			//listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 1, "码率");
			//listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 2, "均值");
			//sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_mean_value);
			//listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 3, pszText);
			//listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, pCurPcrInfo->encoder_bitrate_mean_value);

			//listPcrLog.InsertItem(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, "");
			//listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, 2, "方差");
			//sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_rms_value);
			//listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, 3, pszText);
			//listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, pCurPcrInfo->encoder_bitrate_rms_value);
		}
	}
}


void CDlg_TSAnalyzer_Metering_Pcr::UpdatePCRObservation(int ID, int curInterval, int curJitter)
{
#if SHOW_PCR_INTERVAL_HISTGRAM
	if (m_pPcrIntervalHistgramGraph != NULL)
	{
		if (m_pPcrIntervalHistgramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrIntervalHistgramGraph->AppendSample(ID, curInterval);
		}
	}
#endif

#if SHOW_PCR_INTERVAL_WAVEFORM
	if (m_pPcrIntervalWaveformGraph != NULL)
	{
		if (m_pPcrIntervalWaveformGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrIntervalWaveformGraph->AppendSample(ID, curInterval, &attrInterval);
		}
	}
#endif

#if SHOW_PCR_JITTER_HISTGRAM
	if (m_pPcrJitterHistgramGraph != NULL)
	{
		if (m_pPcrJitterHistgramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrJitterHistgramGraph->AppendSample(ID, curJitter);
		}
	}
#endif

#if SHOW_PCR_JITTER_WAVEFORM
	if (m_pPcrJitterWaveformGraph != NULL)
	{
		if (m_pPcrJitterWaveformGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrJitterWaveformGraph->AppendSample(ID, curJitter, &attrJitter);
		}
	}
#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	if (m_pPcrScatterDiagramGraph != NULL)
	{
		if (m_pPcrScatterDiagramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrScatterDiagramGraph->AppendSample(ID, curInterval, curJitter);
		}
	}
#endif
}

void CDlg_TSAnalyzer_Metering_Pcr::Set(int offline)
{
#if SHOW_PCR_JITTER_HISTGRAM
	if (m_pPcrJitterHistgramGraph != NULL)
	{
		if (m_pPcrJitterHistgramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrJitterHistgramGraph->Set();
		}
	}
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
	if (m_pPcrIntervalHistgramGraph != NULL)
	{
		if (m_pPcrIntervalHistgramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrIntervalHistgramGraph->Set();
		}
	}
#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	if (m_pPcrScatterDiagramGraph != NULL)
	{
		if (m_pPcrScatterDiagramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrScatterDiagramGraph->Set();
		}
	}
#endif

#if SHOW_PCR_JITTER_WAVEFORM
	if (m_pPcrJitterWaveformGraph != NULL)
	{
		if (m_pPcrJitterWaveformGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrJitterWaveformGraph->Set();
		}
	}
#endif

#if SHOW_PCR_INTERVAL_WAVEFORM
	if (m_pPcrIntervalWaveformGraph != NULL)
	{
		if (m_pPcrIntervalWaveformGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrIntervalWaveformGraph->Set();
		}
	}
#endif
}

void CDlg_TSAnalyzer_Metering_Pcr::Reset(void)
{
#if SHOW_PCR_JITTER_HISTGRAM
	if (m_pPcrJitterHistgramGraph != NULL)
	{
		if (m_pPcrJitterHistgramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrJitterHistgramGraph->Reset();
		}
	}
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
	if (m_pPcrIntervalHistgramGraph != NULL)
	{
		if (m_pPcrIntervalHistgramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrIntervalHistgramGraph->Reset();
		}
	}
#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	if (m_pPcrScatterDiagramGraph != NULL)
	{
		if (m_pPcrScatterDiagramGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrScatterDiagramGraph->Reset();
		}
	}
#endif

#if SHOW_PCR_JITTER_WAVEFORM
	if (m_pPcrJitterWaveformGraph != NULL)
	{
		if (m_pPcrJitterWaveformGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrJitterWaveformGraph->Reset();
		}
	}
#endif

#if SHOW_PCR_INTERVAL_WAVEFORM
	if (m_pPcrIntervalWaveformGraph != NULL)
	{
		if (m_pPcrIntervalWaveformGraph->GetSafeHwnd() != NULL)
		{
			m_pPcrIntervalWaveformGraph->Reset();
		}
	}
#endif

	m_pNaviPane->Reset();
}



void CDlg_TSAnalyzer_Metering_Pcr::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

