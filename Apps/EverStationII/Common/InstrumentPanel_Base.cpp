//

#include "stdafx.h"
#include <Winuser.h>
#include <Windows.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include <wingdi.h>
#include <afxwin.h>
#include <assert.h>
#include "InstrumentPanel_Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Base
#define SCREEN_BKCOLOR			RGB(0, 0, 0)
#define SCREEN_TEXTCOLOR		RGB(0, 255, 0)
#define SCREEN_TITLECOLOR		RGB(225, 255, 0)
#define SCREEN_UNITCOLOR		RGB(100, 220, 128)
#define SCREEN_GRIDCOLOR		RGB(128, 128, 128)
#define SCREEN_AXISCOLOR		RGB(220, 220, 220)
#define SCREEN_WAVECOLOR0		RGB(250, 250, 0)
#define SCREEN_WAVECOLOR1		RGB(200, 250, 50)
#define SCREEN_WAVECOLOR2		RGB(150, 250, 100)
#define SCREEN_WAVECOLOR3		RGB(100, 250, 150)
#define SCREEN_WAVECOLOR4		RGB(250, 200, 0)
#define SCREEN_WAVECOLOR5		RGB(200, 200, 50)
#define SCREEN_WAVECOLOR6		RGB(150, 200, 100)
#define SCREEN_WAVECOLOR7		RGB(100, 200, 150)
#define SCREEN_WAVECOLOR8		RGB(250, 150, 0)
#define SCREEN_WAVECOLOR9		RGB(200, 150, 50)
#define SCREEN_WAVECOLOR10		RGB(150, 150, 100)
#define SCREEN_WAVECOLOR11		RGB(100, 150, 150)
#define SCREEN_MAXLIMITCOLOR	RGB(160, 0, 0)
#define SCREEN_MINLIMITCOLOR	RGB(120, 0, 0)
#define SCREEN_PAINTCOLOR		RGB(220, 220, 0)

#define UNCREDITABLE_MAX_VALUE				-123456789
#define UNCREDITABLE_MIN_VALUE				123456789

CInstrumentPanel_Base::CInstrumentPanel_Base()
{
	m_pMemDC = NULL;
	m_pBkBrush = NULL;
	m_pWaveformBrush = NULL;
	m_pBkgroundBmp = NULL;
	m_pWaveformBmp = NULL;
	//m_pAlarmLineBmp = NULL;
	//m_pValueBmp = NULL;
	//m_pLeftMarkBmp = NULL;
	//m_pMidMarkBmp = NULL;
	//m_pRightMarkBmp = NULL;
	m_pAxisPen = NULL;
	m_pGridPen = NULL;
	m_pDotPen = NULL;
	m_pDashPen = NULL;

	m_pMeasureFont = NULL;
	m_pMarkFont = NULL;
	m_pTitleFont = NULL;

	m_pMaxLimitPen = NULL;
	m_pMinLimitPen = NULL;

	m_nXMarkShownOption = RANGE_MARK_HIDE;
	m_nXAxisStyle = AXIS_STYLE_UNKNOWN;

	m_nYMarkShownOption = RANGE_MARK_HIDE;
	m_nYAxisStyle = AXIS_STYLE_UNKNOWN;

	m_dGridDeltx = 0;
	m_dGridDelty = 0;
	m_dMeasureDeltX = 0;
	m_dMeasureDeltY = 0;

	for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
	{
		m_pChannel[i] = NULL;
	}
	m_nChannleCount = 0;
	m_nChannleDepth = -1;		//默认值

	m_bNeedUpdate = 0;

	strcpy_s(m_pszXUnits, sizeof(m_pszXUnits), "ms");
	strcpy_s(m_pszYUnits, sizeof(m_pszYUnits), "ms");

	m_Palette[0] = SCREEN_WAVECOLOR0;
	m_Palette[1] = SCREEN_WAVECOLOR1;
	m_Palette[2] = SCREEN_WAVECOLOR2;
	m_Palette[3] = SCREEN_WAVECOLOR3;
	m_Palette[4] = SCREEN_WAVECOLOR4;
	m_Palette[5] = SCREEN_WAVECOLOR5;
	m_Palette[6] = SCREEN_WAVECOLOR6;
	m_Palette[7] = SCREEN_WAVECOLOR7;
	m_Palette[8] = SCREEN_WAVECOLOR8;
	m_Palette[9] = SCREEN_WAVECOLOR9;
	m_Palette[10] = SCREEN_WAVECOLOR10;
	m_Palette[11] = SCREEN_WAVECOLOR11;
}

CInstrumentPanel_Base::~CInstrumentPanel_Base()
{
	if (m_pMemDC != NULL)
	{
		delete m_pMemDC;
		m_pMemDC = NULL;
	}

	if( m_pBkgroundBmp != NULL )
	{
		delete m_pBkgroundBmp;
		m_pBkgroundBmp = NULL;
	}

	if( m_pWaveformBmp != NULL )
	{
		delete m_pWaveformBmp;
		m_pWaveformBmp = NULL;
	}

	//if (m_pAlarmLineBmp != NULL)
	//{
	//	delete m_pAlarmLineBmp;
	//	m_pAlarmLineBmp = NULL;
	//}

	//if (m_pValueBmp != NULL)
	//{
	//	delete m_pValueBmp;
	//	m_pValueBmp = NULL;
	//}

	//if (m_pLeftMarkBmp != NULL)
	//{
	//	delete m_pLeftMarkBmp;
	//	m_pLeftMarkBmp = NULL;
	//}

	//if (m_pMidMarkBmp != NULL)
	//{
	//	delete m_pMidMarkBmp;
	//	m_pMidMarkBmp = NULL;
	//}

	//if (m_pRightMarkBmp != NULL)
	//{
	//	delete m_pRightMarkBmp;
	//	m_pRightMarkBmp = NULL;
	//}

	if( m_pBkBrush != NULL )
	{
		delete m_pBkBrush;
		m_pBkBrush = NULL;
	}

	if (m_pWaveformBrush != NULL)
	{
		delete m_pWaveformBrush;
		m_pWaveformBrush = NULL;
	}

	if (m_pAxisPen != NULL)
	{
		delete m_pAxisPen;
		m_pAxisPen = NULL;
	}

	if (m_pGridPen != NULL)
	{
		delete m_pGridPen;
		m_pGridPen = NULL;
	}

	if (m_pDotPen != NULL)
	{
		delete m_pDotPen;
		m_pDotPen = NULL;
	}

	if (m_pDashPen != NULL)
	{
		delete m_pDashPen;
		m_pDashPen = NULL;
	}

	if( m_pMaxLimitPen != NULL )
	{
		delete m_pMaxLimitPen;
		m_pMaxLimitPen = NULL;
	}
	if (m_pMinLimitPen != NULL)
	{
		delete m_pMinLimitPen;
		m_pMinLimitPen = NULL;
	}

	if (m_pTitleFont != NULL)
	{
		delete m_pTitleFont;
		m_pTitleFont = NULL;
	}
	if (m_pMeasureFont != NULL)
	{
		delete m_pMeasureFont;
		m_pMeasureFont = NULL;
	}
	if (m_pMarkFont != NULL)
	{
		delete m_pMarkFont;
		m_pMarkFont = NULL;
	}
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_Base, CStatic)
	//{{AFX_MSG_MAP(CInstrumentPanel_Base)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Base message handlers
void CInstrumentPanel_Base::DisplayBkGrid(CDC* pMemDC, CBitmap* pBkBmp, CRect rectPicture)
{
	int i;
	int x, y;

	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);
		pMemDC->SelectObject(m_pBkBrush);
		pMemDC->FillRect(rectPicture, m_pBkBrush);
		pMemDC->SetBkColor(SCREEN_BKCOLOR);

		CPen* pOldPen = pMemDC->SelectObject(m_pGridPen);
		pMemDC->Rectangle(rectPicture);

		pMemDC->SelectObject(m_pDotPen);
		for (i = 1; i < GRID_DIVISION_VERTICAL; i++)
		{
			y = (int)(rectPicture.top + i * m_dGridDelty);

			pMemDC->MoveTo(rectPicture.left, y);
			pMemDC->LineTo(rectPicture.right, y);
		}
		pMemDC->SelectObject(m_pAxisPen);
		if (m_nYAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)
		{
			y = rectPicture.bottom;
		}
		else
		{
			y = rectPicture.top + rectPicture.Height() / 2;
		}
		pMemDC->MoveTo(rectPicture.left, y);
		pMemDC->LineTo(rectPicture.right, y);

		pMemDC->SelectObject(m_pDotPen);
		for (i = 1; i < GRID_DIVISION_HORIZONTAL; i++)
		{
			x = (int)(rectPicture.left + i * m_dGridDeltx);
			pMemDC->MoveTo(x, rectPicture.top);
			pMemDC->LineTo(x, rectPicture.bottom);
		}

		pMemDC->SelectObject(m_pAxisPen);
		if (m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)
		{
			x = rectPicture.left;
		}
		else
		{
			x = rectPicture.left + rectPicture.Width() / 2;
		}
		pMemDC->MoveTo(x, rectPicture.top);
		pMemDC->LineTo(x, rectPicture.bottom);

		pMemDC->SelectObject(m_pTitleFont);
		pMemDC->SetTextColor(SCREEN_TITLECOLOR);
		pMemDC->DrawText(m_strTitle, m_strTitle.GetLength(), &m_rectTitle, DT_CENTER | DT_VCENTER);

		pMemDC->SelectObject(pOldPen);
	}
}

void CInstrumentPanel_Base::DisplayMeasuredValue(CDC* pMemDC, CBitmap* pBkBmp, CRect rectMeasure)
{
	char	pszText[64];

	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(m_pMeasureFont);
		pMemDC->SelectObject(pBkBmp);
		pMemDC->SelectObject(m_pBkBrush);
		pMemDC->FillRect(&rectMeasure, m_pBkBrush);

		pMemDC->SetBkColor(SCREEN_BKCOLOR);

		int nYOffset = rectMeasure.top;

		if (m_nYMarkShownOption == RANGE_MARK_SHOWN)
		{
			pMemDC->SetTextColor(SCREEN_UNITCOLOR);
			sprintf_s(pszText, sizeof(pszText), "Y轴（UNIT: %s）\0", m_pszYUnits);
			pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
			nYOffset += (FONT_MEASURE_HEIGHT + 1);

			pMemDC->SetTextColor(SCREEN_TEXTCOLOR);
			if (m_nMeasuredYRmsValue == UNCREDITABLE_MAX_VALUE)
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
			else
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: %d\0", m_nMeasuredYMaxValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: %d\0", m_nMeasuredYMinValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: %d\0", m_nMeasuredYMeanValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: %d\0", m_nMeasuredYRmsValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
			nYOffset += (FONT_MEASURE_HEIGHT + 1);
		}

		if (m_nXMarkShownOption == RANGE_MARK_SHOWN)
		{
			sprintf_s(pszText, sizeof(pszText), "X轴（UNIT：%s）\0", m_pszXUnits);
			pMemDC->SetTextColor(SCREEN_UNITCOLOR);
			pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
			nYOffset += (FONT_MEASURE_HEIGHT + 1);

			pMemDC->SetTextColor(SCREEN_TEXTCOLOR);
			if (m_nMeasuredXRmsValue == UNCREDITABLE_MAX_VALUE)
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: \0");
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
			else
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: %d\0", m_nMeasuredXMaxValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: %d\0", m_nMeasuredXMinValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: %d\0", m_nMeasuredXMeanValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: %d\0", m_nMeasuredXRmsValue);
				pMemDC->TextOut(rectMeasure.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
		}
	}
}

void CInstrumentPanel_Base::DisplayMeasureScale(CDC* pMemDC, CBitmap* pBkBmp, CRect rectMark, int nMark)
{
	char	pszText[64];
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);

		pMemDC->SelectObject(m_pBkBrush);
		pMemDC->FillRect(&rectMark, m_pBkBrush);

		pMemDC->SetBkColor(SCREEN_BKCOLOR);
		pMemDC->SetTextColor(SCREEN_TEXTCOLOR);

		pMemDC->SelectObject(m_pMarkFont);

		sprintf_s(pszText, sizeof(pszText), "%d\n", nMark);
		pMemDC->DrawText(pszText, sizeof(pszText), &rectMark, DT_CENTER | DT_VCENTER);
	}
}

void CInstrumentPanel_Base::DisplayXAlarmLine(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine)
{
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);

		int x0, x1;
		int y0, y1;
		double ratio;

		y0 = rectAlarmLine.top;
		y1 = rectAlarmLine.bottom;

		double xoffset = rectAlarmLine.left;

		if (m_nXNegtiveMark < m_nXAlarmMinLimit)
		{
			ratio = (double)(m_nXAlarmMinLimit - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
			x0 = (int)(xoffset + ratio * rectAlarmLine.Width());

			pMemDC->SelectObject(m_pMaxLimitPen);
			pMemDC->MoveTo(x0, y0);
			pMemDC->LineTo(x0, y1);
		}

		if (m_nXPositiveMark > m_nXAlarmMaxLimit)
		{
			ratio = (double)(m_nXAlarmMaxLimit - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
			x1 = (int)(xoffset + ratio * rectAlarmLine.Width());

			pMemDC->SelectObject(m_pMaxLimitPen);
			pMemDC->MoveTo(x1, y0);
			pMemDC->LineTo(x1, y1);
		}
	}
}

void CInstrumentPanel_Base::DisplayYAlarmLine(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine)
{
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);

		int x0, x1;
		int y0, y1;
		double ratio;

		x0 = rectAlarmLine.left;
		x1 = rectAlarmLine.right;

		double yoffset = rectAlarmLine.bottom;

		if (m_nYPositiveMark > m_nYAlarmMaxLimit)
		{
			ratio = (double)(m_nYAlarmMaxLimit - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
			y0 = (int)(yoffset - ratio * rectAlarmLine.Height());
			if (y0 < rectAlarmLine.top)
			{
				y0 = rectAlarmLine.top;
			}
			pMemDC->SelectObject(m_pMaxLimitPen);
			pMemDC->MoveTo(x0, y0);
			pMemDC->LineTo(x1, y0);
		}
		if (m_nYNegtiveMark < m_nYAlarmMinLimit)
		{
			ratio = (double)(m_nYAlarmMinLimit - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
			y1 = (int)(yoffset - ratio * rectAlarmLine.Height());
			if (y1 > rectAlarmLine.bottom)
			{
				y1 = rectAlarmLine.bottom;
			}
			pMemDC->SelectObject(m_pMinLimitPen);
			pMemDC->MoveTo(x0, y1);
			pMemDC->LineTo(x1, y1);
		}
	}
}

void CInstrumentPanel_Base::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	if (m_bNeedUpdate == 1)
	{
		//CombineDraw();
		BITMAP bm;

		CDC* pDC = GetDC();

		if (m_pBkgroundBmp != NULL)
		{
			m_pBkgroundBmp->GetBitmap(&bm);
			m_pMemDC->SelectObject(m_pBkgroundBmp);
			dc.StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}

		if (m_pWaveformBmp != NULL)
		{
			if (m_pWaveformBmp->GetSafeHandle() != NULL)
			{
				m_pWaveformBmp->GetBitmap(&bm);
				m_pMemDC->SelectObject(m_pWaveformBmp);
				dc.StretchBlt(m_rectWaveform.left, m_rectWaveform.top, m_rectWaveform.Width(), m_rectWaveform.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
			}
		}

		m_bNeedUpdate = 0;
	}

	// Do not call CStatic::OnPaint() for painting messages
}

void CInstrumentPanel_Base::DisplayMeasureGraph(CDC* pDC, CBitmap* pGraphBmp)
{
	//虚函数，实际的绘图功能由继承类实现
}

int CInstrumentPanel_Base::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_strTitle = lpCreateStruct->lpszName;

	CDC* pDC = GetDC();

	if (m_pMemDC == NULL)
	{
		m_pMemDC = new CDC;
		m_pMemDC->CreateCompatibleDC(pDC);
	}

	if (m_pBkBrush == NULL)
	{
		m_pBkBrush = new CBrush;
		m_pBkBrush->CreateSolidBrush(SCREEN_BKCOLOR);
	}

	if (m_pWaveformBrush == NULL)
	{
		m_pWaveformBrush = new CBrush;
		m_pWaveformBrush->CreateSolidBrush(RGB(0x00, 0x00, 0x00));
	}

	if (m_pAxisPen == NULL)
	{
		m_pAxisPen = new CPen;
		m_pAxisPen->CreatePen(PS_SOLID, 2, SCREEN_AXISCOLOR);
	}

	if (m_pGridPen == NULL)
	{
		m_pGridPen = new CPen;
		m_pGridPen->CreatePen(PS_SOLID, 1, SCREEN_GRIDCOLOR);
	}

	if (m_pDotPen == NULL)
	{
		m_pDotPen = new CPen;
		m_pDotPen->CreatePen(PS_DOT, 1, SCREEN_GRIDCOLOR);
	}

	if (m_pDashPen == NULL)
	{
		m_pDashPen = new CPen;
		m_pDashPen->CreatePen(PS_DASH, 1, SCREEN_GRIDCOLOR);
	}

	//if (m_pWavePen == NULL)
	//{
	//	m_pWavePen = new CPen;
	//	m_pWavePen->CreatePen(PS_SOLID, 1, SCREEN_WAVECOLOR);
	//}

	if (m_pMaxLimitPen == NULL)
	{
		m_pMaxLimitPen = new CPen;
		m_pMaxLimitPen->CreatePen(PS_SOLID, 2, SCREEN_MAXLIMITCOLOR);
	}

	if (m_pMinLimitPen == NULL)
	{
		m_pMinLimitPen = new CPen;
		m_pMinLimitPen->CreatePen(PS_SOLID, 2, SCREEN_MINLIMITCOLOR);
	}

	if (m_pTitleFont == NULL)
	{
		m_pTitleFont = new CFont;
		m_pTitleFont->CreateFont(FONT_TITLE_HEIGHT, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
	}
	if (m_pMeasureFont == NULL)
	{
		m_pMeasureFont = new CFont;
		m_pMeasureFont->CreateFont(FONT_MEASURE_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
	}
	if (m_pMarkFont == NULL)
	{
		m_pMarkFont = new CFont;
		m_pMarkFont->CreateFont(FONT_MARK_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
	}

	ReleaseDC(pDC);

	//srand(GetTickCount());
	m_uiTimerID = rand();
	//SetTimer(m_uiTimerID, 500, NULL);

	return 0;
}


void CInstrumentPanel_Base::Reset(void)
{
	m_nMeasuredYMeanValue = UNCREDITABLE_MAX_VALUE;
	m_nMeasuredYRmsValue = UNCREDITABLE_MAX_VALUE;			//默认正负1bps
	m_nMeasuredYMinValue = UNCREDITABLE_MIN_VALUE;
	m_nMeasuredYMaxValue = UNCREDITABLE_MAX_VALUE;

	m_nYNegtiveMark = m_nYAlarmMinLimit;
	m_nYPositiveMark = m_nYAlarmMaxLimit;

	m_nMeasuredXMeanValue = UNCREDITABLE_MAX_VALUE;
	m_nMeasuredXRmsValue = UNCREDITABLE_MAX_VALUE;			//默认正负1bps
	m_nMeasuredXMinValue = UNCREDITABLE_MIN_VALUE;
	m_nMeasuredXMaxValue = UNCREDITABLE_MAX_VALUE;

	m_nXNegtiveMark = m_nXAlarmMinLimit;
	m_nXPositiveMark = m_nXAlarmMaxLimit;

	//m_nObserveMinValue = UNCREDITABLE_MIN_VALUE;
	//m_nObserveMaxValue = UNCREDITABLE_MAX_VALUE;


	for (int i = 0; i < m_nChannleCount; i++)
	{
#if INSTRUMENT_PANEL_USE_MUTEX
		if (m_pChannel[i]->hSampleAccess != NULL)
		{
			::SetEvent(m_pChannel[i]->hSampleAccess);
			::CloseHandle(m_pChannel[i]->hSampleAccess);
			m_pChannel[i]->hSampleAccess = NULL;
		}
#endif
		if (m_pChannel[i]->pnXSampleArray != NULL)
		{
			delete m_pChannel[i]->pnXSampleArray;
		}
		if (m_pChannel[i]->pnYSampleArray != NULL)
		{
			delete m_pChannel[i]->pnYSampleArray;
		}

		delete m_pChannel[i];
		m_pChannel[i] = NULL;
	}
	m_nChannleCount = 0;

	DisplayBkGrid(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
	DisplayXAlarmLine(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
	DisplayYAlarmLine(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);

	if (m_nXMarkShownOption == RANGE_MARK_SHOWN)
	{
		DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXLeftMark, m_nXNegtiveMark);
		DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXMidMark, (m_nXNegtiveMark + m_nXPositiveMark) / 2);
		DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXRightMark, m_nXPositiveMark);
	}

	if (m_nYMarkShownOption == RANGE_MARK_SHOWN)
	{
		DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYBottomMark, m_nYNegtiveMark);
		DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYMidMark, (m_nYNegtiveMark + m_nYPositiveMark) / 2);
		DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYTopMark, m_nYPositiveMark);
	}

	DisplayMeasuredValue(m_pMemDC, m_pBkgroundBmp, m_rectMeasuredValue);

	ClearWaveform(m_pMemDC, m_pWaveformBmp);
	//DisplayMeasureGraph(m_pMemDC, m_pWaveformBmp);

	m_bNeedUpdate = 1;
	Invalidate(FALSE);
}



void CInstrumentPanel_Base::AppendXSample(int ID, int x)
{
	SAMPLE_CHANNEL_t* pChannel = NULL;
	for (int i = 0; i < m_nChannleCount; i++)
	{
		if (m_pChannel[i] != NULL)
		{
			if (m_pChannel[i]->ID == ID)
			{
				pChannel = m_pChannel[i];
				break;
			}
		}
	}

	if (pChannel == NULL)
	{
		if (m_nChannleCount < MAX_CHANNEL_COUNT)
		{
			pChannel = new SAMPLE_CHANNEL_t;

			pChannel->ID = ID;
			pChannel->pnXSampleArray = new int[m_nChannleDepth];
			memset(pChannel->pnXSampleArray, 0, sizeof(int) * m_nChannleDepth);
			pChannel->pnYSampleArray = NULL;
			pChannel->nSampleCount = 0;
			pChannel->nSampleIndex = 0;
			pChannel->bNeedRedrawing = 0;

#if INSTRUMENT_PANEL_USE_MUTEX
			pChannel->hSampleAccess = ::CreateEvent(NULL, FALSE, TRUE, NULL);
#endif
			int color_index = m_nChannleCount % MAX_CHANNEL_COUNT;
			pChannel->color = m_Palette[color_index];

			m_pChannel[m_nChannleCount] = pChannel;
			m_nChannleCount++;
			assert(m_nChannleCount <= MAX_CHANNEL_COUNT);
		}
	}

	if (pChannel != NULL)
	{
#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
		}
#endif

		pChannel->pnXSampleArray[pChannel->nSampleIndex] = x;

		pChannel->nSampleIndex++;
		pChannel->nSampleIndex %= m_nChannleDepth;
		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		pChannel->bNeedRedrawing = 1;
		m_bNeedUpdate = 1;

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif

		DisplayMeasuredValue(m_pMemDC, m_pBkgroundBmp, m_rectMeasuredValue);
		DisplayMeasureGraph(m_pMemDC, m_pWaveformBmp);

		Invalidate(FALSE);
	}
}

void CInstrumentPanel_Base::AppendYSample(int ID, int y)
{
	SAMPLE_CHANNEL_t* pChannel = NULL;
	for (int i = 0; i < m_nChannleCount; i++)
	{
		if (m_pChannel[i] != NULL)
		{
			if (m_pChannel[i]->ID == ID)
			{
				pChannel = m_pChannel[i];
				break;
			}
		}
	}

	if (pChannel == NULL)
	{
		if (m_nChannleCount < MAX_CHANNEL_COUNT)
		{
			pChannel = new SAMPLE_CHANNEL_t;

			pChannel->ID = ID;
			pChannel->pnXSampleArray = NULL;
			pChannel->pnYSampleArray = new int[m_nChannleDepth];
			memset(pChannel->pnYSampleArray, 0, sizeof(int) * m_nChannleDepth);
			pChannel->nSampleCount = 0;
			pChannel->nSampleIndex = 0;
			pChannel->bNeedRedrawing = 0;

#if INSTRUMENT_PANEL_USE_MUTEX
			pChannel->hSampleAccess = ::CreateEvent(NULL, FALSE, TRUE, NULL);
#endif

			int color_index = m_nChannleCount % MAX_CHANNEL_COUNT;
			pChannel->color = m_Palette[color_index];

			m_pChannel[m_nChannleCount] = pChannel;
			m_nChannleCount++;
			assert(m_nChannleCount <= MAX_CHANNEL_COUNT);
		}
	}

	if (pChannel != NULL)
	{
#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
		}
#endif

		pChannel->pnYSampleArray[pChannel->nSampleIndex] = y;

		pChannel->nSampleIndex++;
		pChannel->nSampleIndex %= m_nChannleDepth;
		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		pChannel->bNeedRedrawing = 1;
		m_bNeedUpdate = 1;

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif

		DisplayMeasuredValue(m_pMemDC, m_pBkgroundBmp, m_rectMeasuredValue);
		DisplayMeasureGraph(m_pMemDC, m_pWaveformBmp);

		Invalidate(FALSE);
	}
}

void CInstrumentPanel_Base::AppendXYSample(int ID, int x, int y)
{
	//step1: look up the old record
	SAMPLE_CHANNEL_t* pChannel = NULL;
	for (int i = 0; i < m_nChannleCount; i++)
	{
		if (m_pChannel[i] != NULL)
		{
			if (m_pChannel[i]->ID == ID)
			{
				pChannel = m_pChannel[i];
				break;
			}
		}
	}

	//step 2: create a new record if not exist
	if (pChannel == NULL)
	{
		if (m_nChannleCount < MAX_CHANNEL_COUNT)
		{
			pChannel = new SAMPLE_CHANNEL_t;

			pChannel->ID = ID;

			pChannel->pnXSampleArray = new int[m_nChannleDepth];
			memset(pChannel->pnXSampleArray, 0, sizeof(int) * m_nChannleDepth);

			pChannel->pnYSampleArray = new int[m_nChannleDepth];
			memset(pChannel->pnYSampleArray, 0, sizeof(int) * m_nChannleDepth);

			pChannel->nSampleCount = 0;
			pChannel->nSampleIndex = 0;
			pChannel->bNeedRedrawing = 0;

#if INSTRUMENT_PANEL_USE_MUTEX
			pChannel->hSampleAccess = ::CreateEvent(NULL, FALSE, TRUE, NULL);
#endif

			int color_index = m_nChannleCount % MAX_CHANNEL_COUNT;
			pChannel->color = m_Palette[color_index];

			m_pChannel[m_nChannleCount] = pChannel;
			m_nChannleCount++;
			assert(m_nChannleCount <= MAX_CHANNEL_COUNT);
		}
	}

	if (pChannel != NULL)
	{
#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
		}
#endif

		pChannel->pnXSampleArray[pChannel->nSampleIndex] = x;
		pChannel->pnYSampleArray[pChannel->nSampleIndex] = y;

		pChannel->nSampleIndex++;
		pChannel->nSampleIndex %= m_nChannleDepth;
		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		pChannel->bNeedRedrawing = 1;
		m_bNeedUpdate = 1;

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif

		DisplayMeasuredValue(m_pMemDC, m_pBkgroundBmp, m_rectMeasuredValue);
		DisplayMeasureGraph(m_pMemDC, m_pWaveformBmp);

		Invalidate(FALSE);
	}
}

void CInstrumentPanel_Base::Init_X_Axis(int nXAxisStyle, int nXShownOption, int nXMinAlarm, int nXMaxAlarm, char* pszXUnits, int nXFloor, int nXCeil, int nXStep)
{
	strcpy_s(m_pszXUnits, sizeof(m_pszXUnits), pszXUnits);

	m_nXMarkShownOption = nXShownOption;
	m_nXAxisStyle = nXAxisStyle;

	m_nXAlarmMinLimit = nXMinAlarm;
	m_nXAlarmMaxLimit = nXMaxAlarm;

	m_nXFloor = nXFloor;
	m_nXCeil = nXCeil;
	m_nXStep = nXStep;
}

void CInstrumentPanel_Base::Init_Y_Axis(int nYAxisStyle, int nYMarkShownOption, int nYMinAlarm, int nYMaxAlarm, char* pszYUnits, int nYFloor, int nYCeil, int nYStep)
{
	strcpy_s(m_pszYUnits, sizeof(m_pszYUnits), pszYUnits);

	m_nYMarkShownOption = nYMarkShownOption;
	m_nYAxisStyle = nYAxisStyle;

	m_nYAlarmMinLimit = nYMinAlarm;
	m_nYAlarmMaxLimit = nYMaxAlarm;

	m_nYFloor = nYFloor;
	m_nYCeil = nYCeil;
	m_nYStep = nYStep;
}

//void CInstrumentPanel_Base::CombineDraw(void)
//{
//	BITMAP bm;
//
//	CDC* pDC = GetDC();
//
//	if (m_pBkgroundBmp != NULL)
//	{
//		m_pBkgroundBmp->GetBitmap(&bm);
//		m_pMemDC->SelectObject(m_pBkgroundBmp);
//		pDC->StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
//	}
//
//	//if (m_pLeftMarkBmp != NULL)
//	//{
//	//	m_pLeftMarkBmp->GetBitmap(&bm);
//	//	m_pMemDC->SelectObject(m_pLeftMarkBmp);
//	//	pDC->StretchBlt(m_rectLeftMark.left, m_rectLeftMark.top, m_rectLeftMark.Width(), m_rectLeftMark.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
//	//}
//	//if (m_pMidMarkBmp != NULL)
//	//{
//	//	m_pMidMarkBmp->GetBitmap(&bm);
//	//	m_pMemDC->SelectObject(m_pMidMarkBmp);
//	//	pDC->StretchBlt(m_rectMidMark.left, m_rectMidMark.top, m_rectMidMark.Width(), m_rectMidMark.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
//	//}
//	//if (m_pRightMarkBmp != NULL)
//	//{
//	//	m_pRightMarkBmp->GetBitmap(&bm);
//	//	m_pMemDC->SelectObject(m_pRightMarkBmp);
//	//	pDC->StretchBlt(m_rectRightMark.left, m_rectRightMark.top, m_rectRightMark.Width(), m_rectRightMark.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
//	//}
//
//	//if (m_pAlarmLineBmp != NULL)
//	//{
//	//	m_pAlarmLineBmp->GetBitmap(&bm);
//	//	m_pMemDC->SelectObject(m_pAlarmLineBmp);
//	//	pDC->StretchBlt(m_rectWaveform.left, m_rectWaveform.top, m_rectWaveform.Width(), m_rectWaveform.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
//	//}
//
//	//if (m_pValueBmp != NULL)
//	//{
//	//	m_pValueBmp->GetBitmap(&bm);
//	//	m_pMemDC->SelectObject(m_pValueBmp);
//	//	pDC->StretchBlt(m_rectMeasureValue.left, m_rectMeasureValue.top, m_rectMeasureValue.Width(), m_rectMeasureValue.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
//	//}
//
//	if (m_pWaveformBmp != NULL)
//	{
//		if (m_pWaveformBmp->GetSafeHandle() != NULL)
//		{
//			m_pWaveformBmp->GetBitmap(&bm);
//			m_pMemDC->SelectObject(m_pWaveformBmp);
//			pDC->StretchBlt(m_rectWaveform.left, m_rectWaveform.top, m_rectWaveform.Width(), m_rectWaveform.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
//		}
//	}
//
//	ReleaseDC(pDC);
//}

void CInstrumentPanel_Base::OnDestroy()
{
	CStatic::OnDestroy();
	
	// TODO: Add your message handler code here

	//KillTimer(m_uiTimerID);
}

void CInstrumentPanel_Base::AdjustLayout(CRect rectContainer)
{
	m_rectTitle.left = rectContainer.left + RECT_MARK_WIDTH + X_SEPARATOR;
	m_rectTitle.top = rectContainer.top + Y_SEPARATOR;
	m_rectTitle.right = rectContainer.right - RECT_MEASURE_WIDTH - X_SEPARATOR - X_SEPARATOR;
	m_rectTitle.bottom = rectContainer.top + RECT_TITLE_HEIGHT;

	m_rectWaveform.left = m_rectTitle.left;
	m_rectWaveform.right = m_rectTitle.right;
	m_rectWaveform.top = m_rectTitle.bottom + Y_SEPARATOR;
	m_rectWaveform.bottom = rectContainer.bottom - RECT_MARK_HEIGHT - Y_SEPARATOR;

	m_rectMeasuredValue.left = rectContainer.right - RECT_MEASURE_WIDTH;
	m_rectMeasuredValue.right = rectContainer.right;
	m_rectMeasuredValue.top = m_rectWaveform.top;
	m_rectMeasuredValue.bottom = m_rectWaveform.bottom;

	m_rectXLeftMark.left = m_rectWaveform.left - RECT_MARK_WIDTH / 2;
	m_rectXLeftMark.right = m_rectWaveform.left + RECT_MARK_WIDTH / 2;
	m_rectXLeftMark.top = rectContainer.bottom - RECT_MARK_HEIGHT;
	m_rectXLeftMark.bottom = rectContainer.bottom;

	m_rectXRightMark.left = m_rectWaveform.right - RECT_MARK_WIDTH / 2;
	m_rectXRightMark.right = m_rectWaveform.right + RECT_MARK_WIDTH / 2;
	m_rectXRightMark.top = rectContainer.bottom - RECT_MARK_HEIGHT;
	m_rectXRightMark.bottom = rectContainer.bottom;

	m_rectXMidMark.left = m_rectWaveform.left + m_rectWaveform.Width() / 2 - RECT_MARK_WIDTH / 2;
	m_rectXMidMark.right = m_rectWaveform.left + m_rectWaveform.Width() / 2 + RECT_MARK_WIDTH / 2;
	m_rectXMidMark.top = rectContainer.bottom -20;
	m_rectXMidMark.bottom = rectContainer.bottom;

	m_rectYTopMark.left = rectContainer.left;
	m_rectYTopMark.right = m_rectWaveform.left - X_SEPARATOR;
	m_rectYTopMark.top = m_rectWaveform.top - RECT_MARK_HEIGHT / 2;
	m_rectYTopMark.bottom = m_rectWaveform.top + RECT_MARK_HEIGHT / 2;

	m_rectYBottomMark.left = rectContainer.left;
	m_rectYBottomMark.right = m_rectWaveform.left - X_SEPARATOR;
	m_rectYBottomMark.top = m_rectWaveform.bottom - RECT_MARK_HEIGHT / 2;
	m_rectYBottomMark.bottom = m_rectWaveform.bottom + RECT_MARK_HEIGHT / 2;

	m_rectYMidMark.left = rectContainer.left;
	m_rectYMidMark.right = m_rectWaveform.left - X_SEPARATOR;
	m_rectYMidMark.top = m_rectWaveform.top + m_rectWaveform.Height() / 2 - RECT_MARK_HEIGHT / 2;
	m_rectYMidMark.bottom = m_rectWaveform.top + m_rectWaveform.Height() / 2 + RECT_MARK_HEIGHT / 2;

	m_dGridDeltx = (double)m_rectWaveform.Width() / GRID_DIVISION_HORIZONTAL;
	m_dGridDelty = (double)m_rectWaveform.Height() / GRID_DIVISION_VERTICAL;
}

void CInstrumentPanel_Base::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (nIDEvent == m_uiTimerID)
	//{
	//	if (m_bNeedUpdate)
	//	{
	//		DisplayMeasureValue(m_pBkMemDC, m_rectMeasure);
	//		DisplayMeasureScale(m_pBkMemDC);
	//		DisplayMeasureGraph(m_pPicMemDC, m_rectPicture);
	//		Invalidate(FALSE);
	//	}
	//}

	CStatic::OnTimer(nIDEvent);
}



void CInstrumentPanel_Base::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (m_pMemDC != NULL)
	{
		GetClientRect(&m_rectClient);
		AdjustLayout(m_rectClient);

		CDC* pDC = GetDC();

		if (m_pBkgroundBmp != NULL)
		{
			delete m_pBkgroundBmp;
		}
		m_pBkgroundBmp = new CBitmap;
		m_pBkgroundBmp->CreateCompatibleBitmap(pDC, cx, cy);

		if (m_pWaveformBmp != NULL)
		{
			delete m_pWaveformBmp;
		}
		m_pWaveformBmp = new CBitmap;
		m_pWaveformBmp->CreateCompatibleBitmap(pDC, m_rectWaveform.Width(), m_rectWaveform.Height());

		ReleaseDC(pDC);

		DisplayBkGrid(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
		DisplayXAlarmLine(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
		DisplayYAlarmLine(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);

		if (m_nXMarkShownOption == RANGE_MARK_SHOWN)
		{
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXLeftMark, m_nXNegtiveMark);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXMidMark, (m_nXNegtiveMark + m_nXPositiveMark) / 2);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectXRightMark, m_nXPositiveMark);
		}

		if (m_nYMarkShownOption == RANGE_MARK_SHOWN)
		{
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYTopMark, m_nYPositiveMark);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYMidMark, (m_nYNegtiveMark + m_nYPositiveMark) / 2);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYBottomMark, m_nYNegtiveMark);
		}

		DisplayMeasuredValue(m_pMemDC, m_pBkgroundBmp, m_rectMeasuredValue);

		ClearWaveform(m_pMemDC, m_pWaveformBmp);
		for (int i = 0; i < m_nChannleCount; i++)
		{
			m_pChannel[i]->bNeedRedrawing = 1;
		}
		DisplayMeasureGraph(m_pMemDC, m_pWaveformBmp);

		m_bNeedUpdate = 1;
	}
}

void CInstrumentPanel_Base::ClearWaveform(CDC* pMemDC, CBitmap* pWaveformBmp)
{
	if ((pMemDC != NULL) && (pWaveformBmp != NULL))
	{
		BITMAP bm;
		CRect rectPicture;

		pWaveformBmp->GetBitmap(&bm);

		rectPicture.left = 0;
		rectPicture.top = 0;
		rectPicture.right = bm.bmWidth;
		rectPicture.bottom = bm.bmHeight;

		pMemDC->SelectObject(pWaveformBmp);
		pMemDC->FillRect(&rectPicture, m_pWaveformBrush);
	}
}


