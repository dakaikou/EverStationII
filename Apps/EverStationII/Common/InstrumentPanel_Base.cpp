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
#define SCREEN_BKGROUNDCOLOR			RGB(0, 0, 0)
#define SCREEN_BKWAVEFORMCOLOR			RGB(50, 50, 50)
#define SCREEN_BKMEASUREPANELCOLOR		RGB(0, 0, 0)

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

#define X_SEPARATOR						4
#define Y_SEPARATOR						4

#define RECT_MEASURE_WIDTH				160
#define RECT_XMARK_HEIGHT				30
#define RECT_XMARK_WIDTH				160
#define RECT_YMARK_HEIGHT				30
#define RECT_YMARK_WIDTH				20
#define RECT_TITLE_HEIGHT				40

#define FONT_TITLE_HEIGHT				20
#define FONT_MARK_HEIGHT				16
#define FONT_MEASURE_HEIGHT				16

#define GRID_DIVISION_VERTICAL			6
#define GRID_DIVISION_HORIZONTAL		20

#define UNCREDITABLE_MAX_VALUE				-123456789
#define UNCREDITABLE_MIN_VALUE				123456789

CInstrumentPanel_Base::CInstrumentPanel_Base()
{
	m_pMemDC = NULL;

	m_pBkgroundBrush = NULL;
	m_pWaveformBrush = NULL;
	m_pMeasurePanelBrush = NULL;

	m_pBkgroundBmp = NULL;
	m_pWaveformBmp = NULL;
	m_pMeasurePanelBmp = NULL;
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

	m_bNeedRedrawAllBmp = 1;
	m_bNeedRedrawWaveformBmp = 1;
	m_bNeedRedrawMeasurePanelBmp = 1;

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

#if ON_PAINTING_USE_MUTEX
	m_hPaintingAccess = NULL;
#endif

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

	if (m_pMeasurePanelBmp != NULL)
	{
		delete m_pMeasurePanelBmp;
		m_pMeasurePanelBmp = NULL;
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

	if( m_pBkgroundBrush != NULL )
	{
		delete m_pBkgroundBrush;
		m_pBkgroundBrush = NULL;
	}

	if (m_pWaveformBrush != NULL)
	{
		delete m_pWaveformBrush;
		m_pWaveformBrush = NULL;
	}

	if (m_pMeasurePanelBrush != NULL)
	{
		delete m_pMeasurePanelBrush;
		m_pMeasurePanelBrush = NULL;
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
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Base message handlers
void CInstrumentPanel_Base::DisplayBkGridInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectPicture)
{
	int i;
	int x, y;

	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);
		pMemDC->SelectObject(m_pBkgroundBrush);
		pMemDC->FillRect(rectPicture, m_pBkgroundBrush);
		pMemDC->SetBkColor(SCREEN_BKGROUNDCOLOR);

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

		//pMemDC->SelectObject(m_pBkBrush);
		pMemDC->FillRect(m_rectTitle, m_pBkgroundBrush);

		pMemDC->SelectObject(m_pTitleFont);
		pMemDC->SetTextColor(SCREEN_TITLECOLOR);
		pMemDC->DrawText(m_strTitle, m_strTitle.GetLength(), &m_rectTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		pMemDC->SelectObject(pOldPen);
	}
}

void CInstrumentPanel_Base::DisplayMeasurePanelInMemory(CDC* pMemDC, CBitmap* pMemBmp)
{
	char	pszText[64];

	if ((pMemDC != NULL) && (pMemBmp != NULL))
	{
		BITMAP bm;
		CRect rectPicture;

		pMemBmp->GetBitmap(&bm);

		rectPicture.left = 0;
		rectPicture.top = 0;
		rectPicture.right = bm.bmWidth;
		rectPicture.bottom = bm.bmHeight;

		pMemDC->SelectObject(m_pMeasureFont);
		pMemDC->SelectObject(pMemBmp);
		pMemDC->SelectObject(m_pMeasurePanelBrush);
		pMemDC->FillRect(&rectPicture, m_pMeasurePanelBrush);

		pMemDC->SetBkColor(SCREEN_BKMEASUREPANELCOLOR);

		int nYOffset = rectPicture.top;

		if (m_nYMarkShownOption == RANGE_MARK_SHOWN)
		{
			pMemDC->SetTextColor(SCREEN_UNITCOLOR);
			sprintf_s(pszText, sizeof(pszText), "Y轴（UNIT: %s）\0", m_pszYUnits);
			pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
			nYOffset += (FONT_MEASURE_HEIGHT + 1);

			pMemDC->SetTextColor(SCREEN_TEXTCOLOR);
			if (m_nMeasuredYRmsValue == UNCREDITABLE_MAX_VALUE)
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
			else
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: %d\0", m_nMeasuredYMaxValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: %d\0", m_nMeasuredYMinValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: %d\0", m_nMeasuredYMeanValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: %d\0", m_nMeasuredYRmsValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
			nYOffset += (FONT_MEASURE_HEIGHT + 1);
		}

		if (m_nXMarkShownOption == RANGE_MARK_SHOWN)
		{
			sprintf_s(pszText, sizeof(pszText), "X轴（UNIT：%s）\0", m_pszXUnits);
			pMemDC->SetTextColor(SCREEN_UNITCOLOR);
			pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
			nYOffset += (FONT_MEASURE_HEIGHT + 1);

			pMemDC->SetTextColor(SCREEN_TEXTCOLOR);
			if (m_nMeasuredXRmsValue == UNCREDITABLE_MAX_VALUE)
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: \0");
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
			else
			{
				sprintf_s(pszText, sizeof(pszText), "最大值: %d\0", m_nMeasuredXMaxValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "最小值: %d\0", m_nMeasuredXMinValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "均  值: %d\0", m_nMeasuredXMeanValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);

				sprintf_s(pszText, sizeof(pszText), "标准差: %d\0", m_nMeasuredXRmsValue);
				pMemDC->TextOut(rectPicture.left, nYOffset, pszText);
				nYOffset += (FONT_MEASURE_HEIGHT + 1);
			}
		}
	}
}

void CInstrumentPanel_Base::DisplayMeasureScaleInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectMark, int nMark)
{
	char	pszText[64];
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);

		pMemDC->SelectObject(m_pBkgroundBrush);
		pMemDC->FillRect(&rectMark, m_pBkgroundBrush);

		pMemDC->SetBkColor(SCREEN_BKGROUNDCOLOR);
		pMemDC->SetTextColor(SCREEN_TEXTCOLOR);

		pMemDC->SelectObject(m_pMarkFont);

		sprintf_s(pszText, sizeof(pszText), "%d\n", nMark);
		CString strMark = pszText;
		pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
}

void CInstrumentPanel_Base::DisplayXAlarmLineInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine)
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

void CInstrumentPanel_Base::DisplayYAlarmLineInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine)
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
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		BITMAP bm;

		if (m_bNeedRedrawAllBmp == 1)
		{
			DisplayBkGridInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);

			DisplayXAlarmLineInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
			DisplayYAlarmLineInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);

			if (m_nXMarkShownOption == RANGE_MARK_SHOWN)
			{
				DisplayMeasureScaleInMemory(m_pMemDC, m_pBkgroundBmp, m_rectXLeftMark, m_nXNegtiveMark);
				DisplayMeasureScaleInMemory(m_pMemDC, m_pBkgroundBmp, m_rectXMidMark, (m_nXNegtiveMark + m_nXPositiveMark) / 2);
				DisplayMeasureScaleInMemory(m_pMemDC, m_pBkgroundBmp, m_rectXRightMark, m_nXPositiveMark);
			}

			if (m_nYMarkShownOption == RANGE_MARK_SHOWN)
			{
				DisplayMeasureScaleInMemory(m_pMemDC, m_pBkgroundBmp, m_rectYBottomMark, m_nYNegtiveMark);
				DisplayMeasureScaleInMemory(m_pMemDC, m_pBkgroundBmp, m_rectYMidMark, (m_nYNegtiveMark + m_nYPositiveMark) / 2);
				DisplayMeasureScaleInMemory(m_pMemDC, m_pBkgroundBmp, m_rectYTopMark, m_nYPositiveMark);
			}

			if (m_pBkgroundBmp != NULL)
			{
				if (m_pBkgroundBmp->GetSafeHandle() != NULL)
				{
					m_pBkgroundBmp->GetBitmap(&bm);
					m_pMemDC->SelectObject(m_pBkgroundBmp);
					dc.StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
				}
			}

			ClearWaveformInMemory(m_pMemDC, m_pWaveformBmp);
			DisplayTheWholeSamplesInMemory(m_pMemDC, m_pWaveformBmp);

			m_bNeedRedrawWaveformBmp = 1;
			m_bNeedRedrawMeasurePanelBmp = 1;

			m_bNeedRedrawAllBmp = 0;
		}

		if (m_bNeedRedrawWaveformBmp == 1)
		{
			DisplayTheNewSamplesInMemory(m_pMemDC, m_pWaveformBmp);

			if (m_pWaveformBmp != NULL)
			{
				if (m_pWaveformBmp->GetSafeHandle() != NULL)
				{
					m_pWaveformBmp->GetBitmap(&bm);
					m_pMemDC->SelectObject(m_pWaveformBmp);
					dc.StretchBlt(m_rectWaveform.left, m_rectWaveform.top, m_rectWaveform.Width(), m_rectWaveform.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
				}
			}

			m_bNeedRedrawWaveformBmp = 0;
		}

		if (m_bNeedRedrawMeasurePanelBmp == 1)
		{
			DisplayMeasurePanelInMemory(m_pMemDC, m_pMeasurePanelBmp);

			if (m_pMeasurePanelBmp != NULL)
			{
				if (m_pMeasurePanelBmp->GetSafeHandle() != NULL)
				{
					m_pMeasurePanelBmp->GetBitmap(&bm);
					m_pMemDC->SelectObject(m_pMeasurePanelBmp);
					//dc.StretchBlt(m_rectMeasurePanel.left, m_rectMeasurePanel.top, m_rectMeasurePanel.Width(), m_rectMeasurePanel.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
					dc.StretchBlt(m_rectMeasurePanel.left, m_rectMeasurePanel.top, m_rectMeasurePanel.Width(), m_rectMeasurePanel.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
				}
			}

			m_bNeedRedrawMeasurePanelBmp = 0;
		}

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif

	// Do not call CStatic::OnPaint() for painting messages
}

void CInstrumentPanel_Base::DisplayTheWholeSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp)
{
	//虚函数，实际的绘图功能由继承类实现
}

void CInstrumentPanel_Base::DisplayTheNewSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp)
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

	if (m_pBkgroundBrush == NULL)
	{
		m_pBkgroundBrush = new CBrush;
		m_pBkgroundBrush->CreateSolidBrush(SCREEN_BKGROUNDCOLOR);
	}

	if (m_pWaveformBrush == NULL)
	{
		m_pWaveformBrush = new CBrush;
		m_pWaveformBrush->CreateSolidBrush(SCREEN_BKWAVEFORMCOLOR);
	}

	if (m_pMeasurePanelBrush == NULL)
	{
		m_pMeasurePanelBrush = new CBrush;
		m_pMeasurePanelBrush->CreateSolidBrush(SCREEN_BKMEASUREPANELCOLOR);
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

#if ON_PAINTING_USE_MUTEX
	m_hPaintingAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif

	srand(GetTickCount());
	m_uiTimerID = rand();
	SetTimer(m_uiTimerID, 1000, NULL);

	return 0;
}


void CInstrumentPanel_Base::Reset(void)
{
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
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
			//if (m_pChannel[i]->pnXSampleArray != NULL)
			//{
			//	delete m_pChannel[i]->pnXSampleArray;
			//}
			//if (m_pChannel[i]->pnYSampleArray != NULL)
			//{
			//	delete m_pChannel[i]->pnYSampleArray;
			//}

			if (m_pChannel[i]->pstSampleArray != NULL)
			{
				delete m_pChannel[i]->pstSampleArray;
				m_pChannel[i]->pstSampleArray = NULL;
			}

			delete m_pChannel[i];
			m_pChannel[i] = NULL;
		}
		m_nChannleCount = 0;
		m_bNeedRedrawAllBmp = 1;

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif

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
			//pChannel->pnXSampleArray = new int[m_nChannleDepth];
			//memset(pChannel->pnXSampleArray, 0, sizeof(int) * m_nChannleDepth);
			//pChannel->pnYSampleArray = NULL;
			pChannel->pstSampleArray = new SAMPLE_VALUE_t[m_nChannleDepth];
			memset(pChannel->pstSampleArray, 0x00, sizeof(SAMPLE_VALUE_t)*m_nChannleDepth);
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

		//pChannel->pnXSampleArray[pChannel->nSampleIndex] = x;
		pChannel->pstSampleArray[pChannel->nSampleIndex].x = x;
		pChannel->pstSampleArray[pChannel->nSampleIndex].bConsumed = 0;

		pChannel->nSampleIndex++;
		pChannel->nSampleIndex %= m_nChannleDepth;
		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		pChannel->bNeedRedrawing = 1;

		m_bNeedRedrawWaveformBmp = 1;
		m_bNeedRedrawMeasurePanelBmp = 1;

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif
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
			//pChannel->pnXSampleArray = NULL;
			//pChannel->pnYSampleArray = new int[m_nChannleDepth];
			//memset(pChannel->pnYSampleArray, 0, sizeof(int) * m_nChannleDepth);
			pChannel->pstSampleArray = new SAMPLE_VALUE_t[m_nChannleDepth];
			memset(pChannel->pstSampleArray, 0x00, sizeof(SAMPLE_VALUE_t)*m_nChannleDepth);
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

		//pChannel->pnYSampleArray[pChannel->nSampleIndex] = y;
		pChannel->pstSampleArray[pChannel->nSampleIndex].y = y;
		pChannel->pstSampleArray[pChannel->nSampleIndex].bConsumed = 0;

		pChannel->nSampleIndex++;
		pChannel->nSampleIndex %= m_nChannleDepth;
		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		pChannel->bNeedRedrawing = 1;

		m_bNeedRedrawWaveformBmp = 1;
		m_bNeedRedrawMeasurePanelBmp = 1;

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif
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

			//pChannel->pnXSampleArray = new int[m_nChannleDepth];
			//memset(pChannel->pnXSampleArray, 0, sizeof(int) * m_nChannleDepth);

			//pChannel->pnYSampleArray = new int[m_nChannleDepth];
			//memset(pChannel->pnYSampleArray, 0, sizeof(int) * m_nChannleDepth);

			pChannel->pstSampleArray = new SAMPLE_VALUE_t[m_nChannleDepth];
			memset(pChannel->pstSampleArray, 0x00, sizeof(SAMPLE_VALUE_t)*m_nChannleDepth);

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

		//pChannel->pnXSampleArray[pChannel->nSampleIndex] = x;
		//pChannel->pnYSampleArray[pChannel->nSampleIndex] = y;
		pChannel->pstSampleArray[pChannel->nSampleIndex].x = x;
		pChannel->pstSampleArray[pChannel->nSampleIndex].y = y;
		pChannel->pstSampleArray[pChannel->nSampleIndex].bConsumed = 0;

		pChannel->nSampleIndex++;
		pChannel->nSampleIndex %= m_nChannleDepth;
		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		pChannel->bNeedRedrawing = 1;

		m_bNeedRedrawWaveformBmp = 1;
		m_bNeedRedrawMeasurePanelBmp = 1;

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif
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

void CInstrumentPanel_Base::OnDestroy()
{
	CStatic::OnDestroy();
	
	// TODO: Add your message handler code here
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
#if ON_PAINTING_USE_MUTEX
		::CloseHandle(m_hPaintingAccess);
		m_hPaintingAccess = NULL;
	}
#endif

	KillTimer(m_uiTimerID);
}

void CInstrumentPanel_Base::AdjustLayout(CRect rectContainer)
{
	int markWidth = max(RECT_YMARK_WIDTH, RECT_XMARK_WIDTH/2);
	m_rectTitle.left = rectContainer.left + X_SEPARATOR + markWidth + X_SEPARATOR;
	m_rectTitle.top = rectContainer.top + Y_SEPARATOR;
	m_rectTitle.right = rectContainer.right - X_SEPARATOR - RECT_MEASURE_WIDTH - X_SEPARATOR - X_SEPARATOR;
	m_rectTitle.bottom = rectContainer.top + Y_SEPARATOR + RECT_TITLE_HEIGHT;

	m_rectWaveform.left = m_rectTitle.left;
	m_rectWaveform.right = m_rectTitle.right;
	m_rectWaveform.top = m_rectTitle.bottom + Y_SEPARATOR;
	m_rectWaveform.bottom = rectContainer.bottom - Y_SEPARATOR - RECT_XMARK_HEIGHT - Y_SEPARATOR - RECT_YMARK_HEIGHT/2;

	m_rectMeasurePanel.left = rectContainer.right - X_SEPARATOR - RECT_MEASURE_WIDTH;
	m_rectMeasurePanel.right = rectContainer.right - X_SEPARATOR;
	m_rectMeasurePanel.top = m_rectWaveform.top;
	m_rectMeasurePanel.bottom = m_rectWaveform.bottom;

	m_rectXLeftMark.left = m_rectWaveform.left - RECT_XMARK_WIDTH / 2;
	m_rectXLeftMark.right = m_rectWaveform.left + RECT_XMARK_WIDTH / 2;
	m_rectXLeftMark.top = m_rectWaveform.bottom + Y_SEPARATOR + RECT_XMARK_HEIGHT / 2;
	m_rectXLeftMark.bottom = rectContainer.bottom - Y_SEPARATOR;

	m_rectXRightMark.left = m_rectWaveform.right - RECT_XMARK_WIDTH / 2;
	m_rectXRightMark.right = m_rectWaveform.right + RECT_XMARK_WIDTH / 2;
	m_rectXRightMark.top = m_rectWaveform.bottom + Y_SEPARATOR + RECT_XMARK_HEIGHT / 2;
	m_rectXRightMark.bottom = rectContainer.bottom - Y_SEPARATOR;

	m_rectXMidMark.left = m_rectWaveform.left + m_rectWaveform.Width() / 2 - RECT_XMARK_WIDTH / 2;
	m_rectXMidMark.right = m_rectWaveform.left + m_rectWaveform.Width() / 2 + RECT_XMARK_WIDTH / 2;
	m_rectXMidMark.top = m_rectWaveform.bottom + Y_SEPARATOR + RECT_YMARK_HEIGHT / 2;
	m_rectXMidMark.bottom = rectContainer.bottom - Y_SEPARATOR;

	m_rectYTopMark.left = rectContainer.left + X_SEPARATOR;
	m_rectYTopMark.right = m_rectWaveform.left - X_SEPARATOR;
	m_rectYTopMark.top = m_rectWaveform.top - RECT_YMARK_HEIGHT / 2;
	m_rectYTopMark.bottom = m_rectWaveform.top + RECT_YMARK_HEIGHT / 2;

	m_rectYBottomMark.left = rectContainer.left + X_SEPARATOR;
	m_rectYBottomMark.right = m_rectWaveform.left - X_SEPARATOR;
	m_rectYBottomMark.top = m_rectWaveform.bottom - RECT_YMARK_HEIGHT / 2;
	m_rectYBottomMark.bottom = m_rectWaveform.bottom + RECT_YMARK_HEIGHT / 2;

	m_rectYMidMark.left = rectContainer.left + X_SEPARATOR;
	m_rectYMidMark.right = m_rectWaveform.left - X_SEPARATOR;
	m_rectYMidMark.top = m_rectWaveform.top + m_rectWaveform.Height() / 2 - RECT_YMARK_HEIGHT / 2;
	m_rectYMidMark.bottom = m_rectWaveform.top + m_rectWaveform.Height() / 2 + RECT_YMARK_HEIGHT / 2;

	m_dGridDeltx = (double)m_rectWaveform.Width() / GRID_DIVISION_HORIZONTAL;
	m_dGridDelty = (double)m_rectWaveform.Height() / GRID_DIVISION_VERTICAL;
}

void CInstrumentPanel_Base::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == m_uiTimerID)
	{
		Invalidate(FALSE);
	}

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
		m_pBkgroundBmp->CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());

		if (m_pWaveformBmp != NULL)
		{
			delete m_pWaveformBmp;
		}
		m_pWaveformBmp = new CBitmap;
		m_pWaveformBmp->CreateCompatibleBitmap(pDC, m_rectWaveform.Width(), m_rectWaveform.Height());

		if (m_pMeasurePanelBmp != NULL)
		{
			delete m_pMeasurePanelBmp;
		}
		m_pMeasurePanelBmp = new CBitmap;
		m_pMeasurePanelBmp->CreateCompatibleBitmap(pDC, m_rectMeasurePanel.Width(), m_rectMeasurePanel.Height());

		ReleaseDC(pDC);

		m_bNeedRedrawAllBmp = 1;
	}
}

void CInstrumentPanel_Base::ClearWaveformInMemory(CDC* pMemDC, CBitmap* pWaveformBmp)
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

BOOL CInstrumentPanel_Base::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bNeedRedrawAllBmp = 1;

	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}
