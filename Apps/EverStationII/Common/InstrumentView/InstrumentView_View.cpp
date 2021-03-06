﻿// InstrumentView_Kernel.cpp: 实现文件
//
#include "stdafx.h"
#include "../EverStationII.h"
#include "InstrumentView_View.h"


// CInstrumentView_Kernel

IMPLEMENT_DYNCREATE(CInstrumentView_View, CView)

CInstrumentView_View::CInstrumentView_View()
{
	m_pMemDC = NULL;

	m_pBkgroundBrush = NULL;
	m_pWaveformBrush = NULL;
	m_pMeasurePanelBrush = NULL;

	m_pBkgroundBmp = NULL;
	m_pWaveformBmp = NULL;
	m_pMeasurePanelBmp = NULL;

	m_nXPanelShownOption = MEASURE_PANEL_HIDE;
	m_nXMarkShownOption = RANGE_MARK_HIDE;
	m_nXAxisStyle = AXIS_STYLE_UNKNOWN;

	m_nYPanelShownOption = MEASURE_PANEL_HIDE;
	m_nYMarkShownOption = RANGE_MARK_HIDE;
	m_nYAxisStyle = AXIS_STYLE_UNKNOWN;

	m_bNeedRedrawAllBmp = 0;
	m_bNeedRedrawWaveformBmp = 0;
	m_bNeedRedrawMeasurePanelBmp = 0;

	strcpy_s(m_pszXUnits, sizeof(m_pszXUnits), "-");
	strcpy_s(m_pszYUnits, sizeof(m_pszYUnits), "-");

#if ON_PAINTING_USE_MUTEX
	m_hPaintingAccess = NULL;
#endif
}

CInstrumentView_View::~CInstrumentView_View()
{
}

BEGIN_MESSAGE_MAP(CInstrumentView_View, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CInstrumentView_View 绘图
void CInstrumentView_View::DisplayBkGridInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectGridArea)
{
	CString strMark;
	double dGridDeltx, dGridDelty;

	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		BITMAP bm;
		CRect rectBkGround;

		pBkBmp->GetBitmap(&bm);
		rectBkGround.left = 0;
		rectBkGround.top = 0;
		rectBkGround.right = bm.bmWidth;
		rectBkGround.bottom = bm.bmHeight;

		pMemDC->SelectObject(pBkBmp);
		pMemDC->SelectObject(m_pBkgroundBrush);
		pMemDC->FillRect(rectBkGround, m_pBkgroundBrush);
		pMemDC->SetBkColor(SCREEN_BKGROUNDCOLOR);

		double dHalfHeight = rectGridArea.Height() / 2.0;
		double dHalfWidth = rectGridArea.Width() / 2.0;

		CPen* pFramePen = new CPen;
		pFramePen->CreatePen(PS_SOLID, 1, SCREEN_GRIDCOLOR);

		CPen* pDotPen = new CPen;
		pDotPen->CreatePen(PS_DOT, 1, SCREEN_GRIDCOLOR);

		CPen* pAxisPen = new CPen;
		pAxisPen->CreatePen(PS_SOLID, 2, SCREEN_AXISCOLOR);

		CPen* pOldPen = pMemDC->SelectObject(pFramePen);
		pMemDC->Rectangle(rectGridArea);

		CFont* pMarkFont = new CFont;
		pMarkFont->CreateFont(FONT_MARK_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);

		CFont* pTitleFont = new CFont;
		pTitleFont->CreateFont(FONT_TITLE_HEIGHT, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);

		CRect rectTitle;
		rectTitle.left = rectGridArea.left + 5;
		rectTitle.top = Y_SEPARATOR;
		rectTitle.right = rectGridArea.right - 5;
		rectTitle.bottom = rectGridArea.top - Y_SEPARATOR;

		pMemDC->FillRect(rectTitle, m_pBkgroundBrush);
		pMemDC->SelectObject(pTitleFont);
		pMemDC->SetTextColor(SCREEN_TITLECOLOR);
		pMemDC->DrawText(m_strTitle, m_strTitle.GetLength(), &rectTitle, DT_CENTER | DT_SINGLELINE | DT_TOP);

		dGridDeltx = (double)rectGridArea.Width() / HORIZONTAL_GRID_DIVISION;
		dGridDelty = (double)rectGridArea.Height() / VERTICAL_GRID_DIVISION;

		//drawing the x & y grids
		for (int i = 1; i < HORIZONTAL_GRID_DIVISION; i++)
		{
			int x = (int)(rectGridArea.left + i * dGridDeltx + 0.5);		//加0.5主要是为了四舍五入控制

			pMemDC->SelectObject(pDotPen);
			pMemDC->MoveTo(x, rectGridArea.top);
			pMemDC->LineTo(x, rectGridArea.bottom);
		}

		for (int i = 1; i < VERTICAL_GRID_DIVISION; i++)
		{
			int y = (int)(rectGridArea.top + i * dGridDelty + 0.5);		//加0.5主要是为了四舍五入控制

			pMemDC->SelectObject(pDotPen);
			pMemDC->MoveTo(rectGridArea.left, y);
			pMemDC->LineTo(rectGridArea.right, y);
		}

		//drawing the x & y axis
		pMemDC->SelectObject(pAxisPen);
		if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			pMemDC->MoveTo(rectGridArea.left - 5, rectGridArea.bottom);
			pMemDC->LineTo(rectGridArea.right + 20, rectGridArea.bottom);

			pMemDC->MoveTo(rectGridArea.right + 20, rectGridArea.bottom);
			pMemDC->LineTo(rectGridArea.right + 5, rectGridArea.bottom + 5);

			pMemDC->MoveTo(rectGridArea.right + 20, rectGridArea.bottom);
			pMemDC->LineTo(rectGridArea.right + 5, rectGridArea.bottom - 5);
		}
		else if ((m_nYAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY) ||
			(m_nYAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY))
		{
			pMemDC->MoveTo(rectGridArea.left - 5, (int)(rectGridArea.top + dHalfHeight));
			pMemDC->LineTo(rectGridArea.right + 20, (int)(rectGridArea.top + dHalfHeight));

			pMemDC->MoveTo(rectGridArea.right + 20, (int)(rectGridArea.top + dHalfHeight));
			pMemDC->LineTo(rectGridArea.right + 5, (int)(rectGridArea.top + dHalfHeight + 5));

			pMemDC->MoveTo(rectGridArea.right + 20, (int)(rectGridArea.top + dHalfHeight));
			pMemDC->LineTo(rectGridArea.right + 5, (int)(rectGridArea.top + dHalfHeight - 5));
		}

		if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			pMemDC->MoveTo(rectGridArea.left, rectGridArea.bottom + 20);
			pMemDC->LineTo(rectGridArea.left, rectGridArea.top - 20);

			pMemDC->MoveTo(rectGridArea.left, rectGridArea.top - 20);
			pMemDC->LineTo(rectGridArea.left + 5, rectGridArea.top - 5);

			pMemDC->MoveTo(rectGridArea.left, rectGridArea.top - 20);
			pMemDC->LineTo(rectGridArea.left - 5, rectGridArea.top - 5);
		}
		else if ((m_nXAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY) ||
			(m_nXAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY))
		{
			pMemDC->MoveTo((int)(rectGridArea.left + dHalfWidth), rectGridArea.bottom + 20);
			pMemDC->LineTo((int)(rectGridArea.left + dHalfWidth), rectGridArea.top - 20);

			pMemDC->MoveTo((int)(rectGridArea.left + dHalfWidth), rectGridArea.top - 20);
			pMemDC->LineTo((int)(rectGridArea.left + dHalfWidth) - 5, rectGridArea.top - 5);

			pMemDC->MoveTo((int)(rectGridArea.left + dHalfWidth), rectGridArea.top - 20);
			pMemDC->LineTo((int)(rectGridArea.left + dHalfWidth) + 5, rectGridArea.top - 5);
		}

		// write axis mark
		pMemDC->SelectObject(pMarkFont);
		pMemDC->SetTextColor(SCREEN_TEXTCOLOR);

		CRect rectYMark(X_SEPARATOR, 0, rectGridArea.left - X_SEPARATOR, 0);

		strMark.Format("%d\n", m_nYPositiveMark);
		rectYMark.top = rectGridArea.top - RECT_YMARK_HEIGHT / 2;
		rectYMark.bottom = rectGridArea.top + RECT_YMARK_HEIGHT / 2;
		pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		strMark.Format("%d\n", m_nYNegtiveMark);
		rectYMark.top = rectGridArea.bottom - RECT_YMARK_HEIGHT / 2;
		rectYMark.bottom = rectGridArea.bottom + RECT_YMARK_HEIGHT / 2;
		pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			CRect rectYMark(X_SEPARATOR, 0, rectGridArea.left - X_SEPARATOR, 0);

			double deltYMark = (m_nYPositiveMark - m_nYNegtiveMark) / VERTICAL_GRID_DIVISION;

			pMemDC->SelectObject(pMarkFont);
			pMemDC->SetTextColor(SCREEN_TEXTCOLOR);

			//start point 
			//if (m_nYMarkShownOption != RANGE_MARK_HIDE)
			{
				strMark.Format("%d\n", m_nYNegtiveMark);
				rectYMark.top = rectGridArea.bottom - RECT_YMARK_HEIGHT / 2;
				rectYMark.bottom = rectGridArea.bottom + RECT_YMARK_HEIGHT / 2;
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

				strMark.Format("%d\n", m_nYPositiveMark);
				rectYMark.top = rectGridArea.top - RECT_YMARK_HEIGHT / 2;
				rectYMark.bottom = rectGridArea.top + RECT_YMARK_HEIGHT / 2;
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			}

			dGridDelty = (double)rectGridArea.Height() / VERTICAL_GRID_DIVISION;

			for (int i = 1; i < VERTICAL_GRID_DIVISION; i++)
			{
				int y = (int)(rectGridArea.bottom - i * dGridDelty);

				pMemDC->SelectObject(pDotPen);
				pMemDC->MoveTo(rectGridArea.left, y);
				pMemDC->LineTo(rectGridArea.right, y);

				if (m_nYMarkShownOption == RANGE_MARK_SHOWN_TOTAL)
				{
					rectYMark.top = y - RECT_YMARK_HEIGHT / 2;
					rectYMark.bottom = y + RECT_YMARK_HEIGHT / 2;

					strMark.Format("%d\n", (int)(m_nYNegtiveMark + i * deltYMark));
					pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
				}
				else if (m_nYMarkShownOption == RANGE_MARK_SHOWN_PARTIAL)
				{
					if (i % 2 == 0)
					{
						rectYMark.top = y - RECT_YMARK_HEIGHT / 2;
						rectYMark.bottom = y + RECT_YMARK_HEIGHT / 2;

						strMark.Format("%d\n", (int)(m_nYNegtiveMark + i * deltYMark));
						pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
					}
				}
			}

			pMemDC->SelectObject(pDotPen);
		}
		else if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY)
		{
			double deltYMark = (m_nYPositiveMark - m_nYNegtiveMark) / VERTICAL_GRID_DIVISION;

			int y0 = (int)(rectGridArea.top + dHalfHeight);

			rectYMark.top = y0 - RECT_YMARK_HEIGHT / 2;
			rectYMark.bottom = y0 + RECT_YMARK_HEIGHT / 2;
			strMark.Format("%d\n", m_nYZeroMark);
			pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			dGridDelty = (double)rectGridArea.Height() / VERTICAL_GRID_DIVISION;

			int y = y0;
			for (int i = 1; i < VERTICAL_GRID_DIVISION / 2; i++)
			{
				y = (int)(y0 - i * dGridDelty);

				rectYMark.top = y - RECT_YMARK_HEIGHT / 2;
				rectYMark.bottom = y + RECT_YMARK_HEIGHT / 2;

				strMark.Format("%d\n", (int)(m_nYZeroMark + i * deltYMark));
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			}

			y = y0;
			for (int i = 1; i < VERTICAL_GRID_DIVISION / 2; i++)
			{
				y = (int)(y0 + i * dGridDelty);

				rectYMark.top = y - RECT_YMARK_HEIGHT / 2;
				rectYMark.bottom = y + RECT_YMARK_HEIGHT / 2;

				strMark.Format("%d\n", (int)(m_nYZeroMark - i * deltYMark));
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			}
		}
		else if (m_nYAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)
		{
			int y0 = (int)(rectGridArea.top + dHalfHeight);

			rectYMark.top = y0 - RECT_YMARK_HEIGHT / 2;
			rectYMark.bottom = y0 + RECT_YMARK_HEIGHT / 2;
			strMark.Format("%d\n", m_nYZeroMark);
			pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

			double dLogArithmicCoeff = (m_nYPositiveMark - m_nYZeroMark) / pow(10, LOGARITHMIC_RANK);
			dGridDelty = (double)rectGridArea.Height() / (2.0 * LOGARITHMIC_RANK);

			for (int i = 1; i < LOGARITHMIC_RANK; i++)
			{
				int y = (int)(y0 - i * dGridDelty);

				rectYMark.top = y - RECT_YMARK_HEIGHT / 2;
				rectYMark.bottom = y + RECT_YMARK_HEIGHT / 2;

				int markValue = (int)(m_nYZeroMark + dLogArithmicCoeff * pow(10, i));
				if (markValue > 1)
				{
					strMark.Format("%d\n", markValue);
					pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
				}
			}

			for (int i = 1; i < LOGARITHMIC_RANK; i++)
			{
				int y = (int)(y0 + i * dGridDelty);

				rectYMark.top = y - RECT_YMARK_HEIGHT / 2;
				rectYMark.bottom = y + RECT_YMARK_HEIGHT / 2;

				int markValue = (int)(m_nYZeroMark - dLogArithmicCoeff * pow(10, i));
				if (markValue < -1)
				{
					strMark.Format("%d\n", markValue);
					pMemDC->DrawText(strMark, strMark.GetLength(), &rectYMark, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
				}
			}

			pMemDC->SelectObject(pDotPen);
		}


		if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			int x0 = rectGridArea.left;
			double deltXMark = (m_nXPositiveMark - m_nXNegtiveMark) / HORIZONTAL_GRID_DIVISION;

			CRect rectXMark(0, rectGridArea.bottom + RECT_YMARK_HEIGHT / 2 + Y_SEPARATOR, 0, bm.bmHeight - Y_SEPARATOR);

			//start point
			//if (m_nXMarkShownOption != RANGE_MARK_HIDE)
			{
				rectXMark.left = rectGridArea.left - RECT_XMARK_WIDTH / 2;
				rectXMark.right = rectGridArea.left + RECT_XMARK_WIDTH / 2;

				strMark.Format("%d\n", m_nXNegtiveMark);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

				rectXMark.left = rectGridArea.right - RECT_XMARK_WIDTH / 2;
				rectXMark.right = rectGridArea.right + RECT_XMARK_WIDTH / 2;

				strMark.Format("%d\n", m_nXPositiveMark);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
			}

			pMemDC->SelectObject(pDotPen);
			for (int i = 1; i < HORIZONTAL_GRID_DIVISION; i++)
			{
				int x = (int)(rectGridArea.left + i * dGridDeltx);

				rectXMark.left = x - RECT_XMARK_WIDTH / 2;
				rectXMark.right = x + RECT_XMARK_WIDTH / 2;

				strMark.Format("%d\n", (int)(m_nXNegtiveMark + i * deltXMark));
				if (m_nXMarkShownOption == RANGE_MARK_SHOWN_TOTAL)
				{
					pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
				}
				else if (m_nXMarkShownOption == RANGE_MARK_SHOWN_PARTIAL)
				{
					if ((i % 2) == 0)
					{
						pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
					}
				}
			}
		}
		else if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY)
		{
			double deltXMark = (m_nXPositiveMark - m_nXNegtiveMark) / HORIZONTAL_GRID_DIVISION;

			strMark.Format("%d\n", m_nXZeroMark);

			CRect rectXMark(0, rectGridArea.bottom + RECT_YMARK_HEIGHT / 2 + Y_SEPARATOR, 0, bm.bmHeight - Y_SEPARATOR);

			int x0 = (int)(rectGridArea.left + dHalfWidth);
			rectXMark.left = x0 - RECT_XMARK_WIDTH / 2;
			rectXMark.right = x0 + RECT_XMARK_WIDTH / 2;

			pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

			strMark.Format("%d\n", m_nXNegtiveMark);

			rectXMark.left = rectGridArea.left - RECT_XMARK_WIDTH / 2;
			rectXMark.right = rectGridArea.left + RECT_XMARK_WIDTH / 2;

			pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

			strMark.Format("%d\n", m_nXPositiveMark);

			rectXMark.left = rectGridArea.right - RECT_XMARK_WIDTH / 2;
			rectXMark.right = rectGridArea.right + RECT_XMARK_WIDTH / 2;

			pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

			pMemDC->SelectObject(pDotPen);
			for (int i = 1; i < HORIZONTAL_GRID_DIVISION / 2; i++)
			{
				int x = (int)(x0 + i * dGridDeltx);
				pMemDC->MoveTo(x, rectGridArea.top);
				pMemDC->LineTo(x, rectGridArea.bottom);

				rectXMark.left = x - RECT_XMARK_WIDTH / 2;
				rectXMark.right = x + RECT_XMARK_WIDTH / 2;

				strMark.Format("%d\n", (int)(m_nXZeroMark + i * deltXMark));

				if (m_nXMarkShownOption == RANGE_MARK_SHOWN_TOTAL)
				{
					pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
				}
				else if (m_nXMarkShownOption == RANGE_MARK_SHOWN_PARTIAL)
				{
					if ((i % 2) == 0)
					{
						pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
					}
				}
			}

			for (int i = 1; i < HORIZONTAL_GRID_DIVISION / 2; i++)
			{
				int x = (int)(x0 - i * dGridDeltx);
				pMemDC->MoveTo(x, rectGridArea.top);
				pMemDC->LineTo(x, rectGridArea.bottom);

				rectXMark.left = x - RECT_XMARK_WIDTH / 2;
				rectXMark.right = x + RECT_XMARK_WIDTH / 2;

				strMark.Format("%d\n", (int)(m_nXZeroMark - i * deltXMark));

				if (m_nXMarkShownOption == RANGE_MARK_SHOWN_TOTAL)
				{
					pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
				}
				else if (m_nXMarkShownOption == RANGE_MARK_SHOWN_PARTIAL)
				{
					if ((i % 2) == 0)
					{
						pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
					}
				}
			}
		}
		else if (m_nXAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)
		{
			double dLogArithmicCoeff = (m_nXPositiveMark - m_nXZeroMark) / pow(10, LOGARITHMIC_RANK);

			int x0 = (int)(rectGridArea.left + dHalfWidth);

			pMemDC->SelectObject(pAxisPen);
			pMemDC->MoveTo(x0, rectGridArea.top);
			pMemDC->LineTo(x0, rectGridArea.bottom);

			strMark.Format("%d\n", m_nXZeroMark);

			CRect rectXMark(0, rectGridArea.bottom + RECT_YMARK_HEIGHT / 2 + Y_SEPARATOR, 0, bm.bmHeight - Y_SEPARATOR);

			rectXMark.left = x0 - RECT_XMARK_WIDTH / 2;
			rectXMark.right = x0 + RECT_XMARK_WIDTH / 2;

			pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

			strMark.Format("%d\n", m_nXNegtiveMark);

			rectXMark.left = rectGridArea.left - RECT_XMARK_WIDTH / 2;
			rectXMark.right = rectGridArea.left + RECT_XMARK_WIDTH / 2;

			pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

			strMark.Format("%d\n", m_nXPositiveMark);

			rectXMark.left = rectGridArea.right - RECT_XMARK_WIDTH / 2;
			rectXMark.right = rectGridArea.right + RECT_XMARK_WIDTH / 2;

			pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

			dGridDeltx = (double)rectGridArea.Width() / (2.0 * LOGARITHMIC_RANK);

			pMemDC->SelectObject(pDotPen);
			for (int i = 1; i < LOGARITHMIC_RANK; i++)
			{
				int x = (int)(x0 + i * dGridDeltx);

				rectXMark.left = x - RECT_XMARK_WIDTH / 2;
				rectXMark.right = x + RECT_XMARK_WIDTH / 2;

				int xmark = (int)(m_nXZeroMark + dLogArithmicCoeff * pow(10, i));
				if (xmark > 1)
				{
					strMark.Format("%d\n", xmark);

					if (m_nXMarkShownOption == RANGE_MARK_SHOWN_TOTAL)
					{
						pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
					}
					else if (m_nXMarkShownOption == RANGE_MARK_SHOWN_PARTIAL)
					{
						if ((i % 2) == 0)
						{
							pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
						}
					}
				}
			}

			for (int i = 1; i < LOGARITHMIC_RANK; i++)
			{
				int x = (int)(x0 - i * dGridDeltx);

				rectXMark.left = x - RECT_XMARK_WIDTH / 2;
				rectXMark.right = x + RECT_XMARK_WIDTH / 2;

				int xmark = (int)(m_nXZeroMark - dLogArithmicCoeff * pow(10, i));
				if (xmark < -1)
				{
					strMark.Format("%d\n", xmark);
					if (m_nXMarkShownOption == RANGE_MARK_SHOWN_TOTAL)
					{
						pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
					}
					else if (m_nXMarkShownOption == RANGE_MARK_SHOWN_PARTIAL)
					{
						if ((i % 2) == 0)
						{
							pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
						}
					}
				}
			}
		}

		pMemDC->SelectObject(pOldPen);

		delete pFramePen;
		delete pDotPen;
		delete pAxisPen;

		delete pMarkFont;
		delete pTitleFont;
	}
}

//绘制图例，此处为虚函数，实质性绘制工作由继承类实现
void CInstrumentView_View::DisplayLegendInMemory(CDC * pMemDC, CBitmap * pBkBmp, CRect rectGridArea)
{
}

void CInstrumentView_View::DisplayMeasurePanelInMemory(CDC * pMemDC, CBitmap * pMemBmp)
{
	char	pszText[64];

	if ((pMemDC != NULL) && (pMemBmp != NULL))
	{
		BITMAP bm;
		CRect rectPicture;

		CFont* pMeasureFont = new CFont;
		pMeasureFont->CreateFont(FONT_MEASURE_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);

		pMemBmp->GetBitmap(&bm);

		rectPicture.left = 0;
		rectPicture.top = 0;
		rectPicture.right = bm.bmWidth;
		rectPicture.bottom = bm.bmHeight;

		pMemDC->SelectObject(pMeasureFont);
		pMemDC->SelectObject(pMemBmp);
		pMemDC->SelectObject(m_pMeasurePanelBrush);
		pMemDC->FillRect(&rectPicture, m_pMeasurePanelBrush);

		pMemDC->SetBkColor(SCREEN_BKMEASUREPANELCOLOR);

		int nYOffset = rectPicture.top;

		if (m_nYPanelShownOption == MEASURE_PANEL_SHOWN)
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

		if (m_nXPanelShownOption == MEASURE_PANEL_SHOWN)
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

		delete pMeasureFont;
	}
}

void CInstrumentView_View::DisplayXAlarmLineInMemory(CDC * pMemDC, CBitmap * pBkBmp, CRect rectAlarmLine)
{
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		BITMAP bm;
		pBkBmp->GetBitmap(&bm);

		pMemDC->SelectObject(pBkBmp);

		int x0, x1;
		int y0, y1;
		double ratio;

		CRect rectMark;
		CString strMark;

		CPen* pMaxLimitPen = new CPen;
		pMaxLimitPen->CreatePen(PS_SOLID, 2, SCREEN_MAXLIMITCOLOR);

		CPen* pMinLimitPen = new CPen;
		pMinLimitPen->CreatePen(PS_SOLID, 2, SCREEN_MINLIMITCOLOR);

		CFont* pMarkFont = new CFont;
		pMarkFont->CreateFont(FONT_MARK_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);

		y0 = rectAlarmLine.top;
		y1 = rectAlarmLine.bottom;

		double xoffset = rectAlarmLine.left;

		pMemDC->SelectObject(pMarkFont);

		if (m_nXAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)
		{
			double dLogArithmicCoeff = (m_nXPositiveMark - m_nXZeroMark) / pow(10, LOGARITHMIC_RANK);

			double half_width = rectAlarmLine.Width() / 2.0;
			double xoffset = rectAlarmLine.left + half_width;

			if (m_nXNegtiveMark < m_nXAlarmMinLimit)
			{
				ratio = log10((m_nXZeroMark - m_nXAlarmMinLimit) / dLogArithmicCoeff) / LOGARITHMIC_RANK;
				x0 = (int)(xoffset - ratio * half_width);
				if (x0 < rectAlarmLine.left)
				{
					x0 = rectAlarmLine.left;
				}
				pMemDC->SelectObject(pMinLimitPen);
				pMemDC->MoveTo(x0, y0);
				pMemDC->LineTo(x0, y1);

				rectMark.left = x0 - RECT_XMARK_WIDTH / 2;
				rectMark.right = x0 + RECT_XMARK_WIDTH / 2;
				rectMark.top = bm.bmHeight - Y_SEPARATOR - RECT_YMARK_HEIGHT;
				rectMark.bottom = bm.bmHeight - Y_SEPARATOR;

				strMark.Format("%d\n", m_nXAlarmMinLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			}

			if (m_nXPositiveMark > m_nXAlarmMaxLimit)
			{
				ratio = log10((m_nXAlarmMaxLimit - m_nXZeroMark) / dLogArithmicCoeff) / LOGARITHMIC_RANK;
				x1 = (int)(xoffset + ratio * half_width);
				if (x1 > rectAlarmLine.right)
				{
					x1 = rectAlarmLine.top;
				}
				pMemDC->SelectObject(pMaxLimitPen);
				pMemDC->MoveTo(x1, y0);
				pMemDC->LineTo(x1, y1);

				rectMark.left = x1 - RECT_XMARK_WIDTH / 2;
				rectMark.right = x1 + RECT_XMARK_WIDTH / 2;
				rectMark.top = bm.bmHeight - Y_SEPARATOR - RECT_YMARK_HEIGHT;
				rectMark.bottom = bm.bmHeight - Y_SEPARATOR;

				strMark.Format("%d\n", m_nXAlarmMaxLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			}
		}
		else
		{
			if (m_nXNegtiveMark < m_nXAlarmMinLimit)
			{
				ratio = (double)(m_nXAlarmMinLimit - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
				x0 = (int)(xoffset + ratio * rectAlarmLine.Width());

				pMemDC->SelectObject(pMaxLimitPen);
				pMemDC->MoveTo(x0, y0);
				pMemDC->LineTo(x0, y1);

				rectMark.left = x0 - RECT_XMARK_WIDTH / 2;
				rectMark.right = x0 + RECT_XMARK_WIDTH / 2;
				rectMark.top = bm.bmHeight - Y_SEPARATOR - RECT_YMARK_HEIGHT;
				rectMark.bottom = bm.bmHeight - Y_SEPARATOR;

				strMark.Format("%d\n", m_nXAlarmMinLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			}

			if (m_nXPositiveMark > m_nXAlarmMaxLimit)
			{
				ratio = (double)(m_nXAlarmMaxLimit - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
				x1 = (int)(xoffset + ratio * rectAlarmLine.Width());

				pMemDC->SelectObject(pMaxLimitPen);
				pMemDC->MoveTo(x1, y0);
				pMemDC->LineTo(x1, y1);

				rectMark.left = x1 - RECT_XMARK_WIDTH / 2;
				rectMark.right = x1 + RECT_XMARK_WIDTH / 2;
				rectMark.top = bm.bmHeight - Y_SEPARATOR - RECT_YMARK_HEIGHT;
				rectMark.bottom = bm.bmHeight - Y_SEPARATOR;

				strMark.Format("%d\n", m_nXAlarmMaxLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			}
		}

		delete pMaxLimitPen;
		delete pMinLimitPen;

		delete pMarkFont;
	}
}

void CInstrumentView_View::DisplayYAlarmLineInMemory(CDC * pMemDC, CBitmap * pBkBmp, CRect rectAlarmLine)
{
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		pMemDC->SelectObject(pBkBmp);

		int x0, x1;
		int y0, y1;
		double ratio;

		CRect rectMark;
		CString strMark;

		CPen* pMaxLimitPen = new CPen;
		pMaxLimitPen->CreatePen(PS_SOLID, 2, SCREEN_MAXLIMITCOLOR);

		CPen* pMinLimitPen = new CPen;
		pMinLimitPen->CreatePen(PS_SOLID, 2, SCREEN_MINLIMITCOLOR);

		CFont* pMarkFont = new CFont;
		pMarkFont->CreateFont(FONT_MARK_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);

		pMemDC->SelectObject(pMarkFont);

		x0 = rectAlarmLine.left;
		x1 = rectAlarmLine.right;

		if (m_nYAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)
		{
			double dLogArithmicCoeff = (m_nYPositiveMark - m_nYZeroMark) / pow(10, LOGARITHMIC_RANK);
			double half_height = rectAlarmLine.Height() / 2.0;
			double yoffset = rectAlarmLine.top + half_height;

			if (m_nYPositiveMark > m_nYAlarmMaxLimit)
			{
				ratio = log10((m_nYAlarmMaxLimit - m_nYZeroMark) / dLogArithmicCoeff) / LOGARITHMIC_RANK;
				y0 = (int)(yoffset - ratio * half_height);
				if (y0 < rectAlarmLine.top)
				{
					y0 = rectAlarmLine.top;
				}
				pMemDC->SelectObject(pMaxLimitPen);
				pMemDC->MoveTo(x0, y0);
				pMemDC->LineTo(x1, y0);

				rectMark.left = X_SEPARATOR;
				rectMark.right = x0 - X_SEPARATOR;
				rectMark.top = y0 - RECT_YMARK_HEIGHT / 2;
				rectMark.bottom = y0 + RECT_YMARK_HEIGHT / 2;

				strMark.Format("%d\n", m_nYAlarmMaxLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			}
			if (m_nYNegtiveMark < m_nYAlarmMinLimit)
			{
				ratio = log10((m_nYZeroMark - m_nYAlarmMinLimit) / dLogArithmicCoeff) / LOGARITHMIC_RANK;
				y1 = (int)(yoffset + ratio * half_height);
				if (y1 > rectAlarmLine.bottom)
				{
					y1 = rectAlarmLine.bottom;
				}
				pMemDC->SelectObject(pMinLimitPen);
				pMemDC->MoveTo(x0, y1);
				pMemDC->LineTo(x1, y1);

				rectMark.left = X_SEPARATOR;
				rectMark.right = x0 - X_SEPARATOR;
				rectMark.top = y1 - RECT_YMARK_HEIGHT / 2;
				rectMark.bottom = y1 + RECT_YMARK_HEIGHT / 2;

				strMark.Format("%d\n", m_nYAlarmMinLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			}
		}
		else
		{
			double yoffset = rectAlarmLine.bottom;

			if (m_nYPositiveMark > m_nYAlarmMaxLimit)
			{
				ratio = (double)(m_nYAlarmMaxLimit - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
				y0 = (int)(yoffset - ratio * rectAlarmLine.Height());
				if (y0 < rectAlarmLine.top)
				{
					y0 = rectAlarmLine.top;
				}
				pMemDC->SelectObject(pMaxLimitPen);
				pMemDC->MoveTo(x0, y0);
				pMemDC->LineTo(x1, y0);

				rectMark.left = X_SEPARATOR;
				rectMark.right = x0 - X_SEPARATOR;
				rectMark.top = y0 - RECT_YMARK_HEIGHT / 2;
				rectMark.bottom = y0 + RECT_YMARK_HEIGHT / 2;

				strMark.Format("%d\n", m_nYAlarmMaxLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			}
			if (m_nYNegtiveMark < m_nYAlarmMinLimit)
			{
				ratio = (double)(m_nYAlarmMinLimit - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
				y1 = (int)(yoffset - ratio * rectAlarmLine.Height());
				if (y1 > rectAlarmLine.bottom)
				{
					y1 = rectAlarmLine.bottom;
				}
				pMemDC->SelectObject(pMinLimitPen);
				pMemDC->MoveTo(x0, y1);
				pMemDC->LineTo(x1, y1);

				rectMark.left = X_SEPARATOR;
				rectMark.right = x0 - X_SEPARATOR;
				rectMark.top = y1 - RECT_YMARK_HEIGHT / 2;
				rectMark.bottom = y1 + RECT_YMARK_HEIGHT / 2;

				strMark.Format("%d\n", m_nYAlarmMinLimit);

				pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
				pMemDC->DrawText(strMark, strMark.GetLength(), &rectMark, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			}
		}

		delete pMaxLimitPen;
		delete pMinLimitPen;

		delete pMarkFont;
	}
}

void CInstrumentView_View::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		BITMAP bm;

		//if (m_bNeedRedrawAllBmp == 1)
		{
			m_nYZeroMark = (m_nYPositiveMark + m_nYNegtiveMark) / 2;
			m_nXZeroMark = (m_nXPositiveMark + m_nXNegtiveMark) / 2;
			m_dYLogArithmicCoeff = (m_nYPositiveMark - m_nYZeroMark) / pow(10, LOGARITHMIC_RANK);
			m_dXLogArithmicCoeff = (m_nXPositiveMark - m_nXZeroMark) / pow(10, LOGARITHMIC_RANK);

			DisplayBkGridInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
			DisplayLegendInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);

			DisplayXAlarmLineInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
			DisplayYAlarmLineInMemory(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);

			DisplayTheWholeSamplesInMemory(m_pMemDC, m_pWaveformBmp);

			m_bNeedRedrawWaveformBmp = 1;
			m_bNeedRedrawMeasurePanelBmp = 1;

			m_bNeedRedrawAllBmp = 0;
		}

		if (m_bNeedRedrawWaveformBmp == 1)
		{
			if (m_pBkgroundBmp != NULL)
			{
				if (m_pBkgroundBmp->GetSafeHandle() != NULL)
				{
					m_pBkgroundBmp->GetBitmap(&bm);
					m_pMemDC->SelectObject(m_pBkgroundBmp);
					pDC->StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
				}
			}

			DisplayTheNewSamplesInMemory(m_pMemDC, m_pWaveformBmp);

			if (m_pWaveformBmp != NULL)
			{
				if (m_pWaveformBmp->GetSafeHandle() != NULL)
				{
					m_pWaveformBmp->GetBitmap(&bm);
					m_pMemDC->SelectObject(m_pWaveformBmp);
					pDC->StretchBlt(m_rectWaveform.left, m_rectWaveform.top, m_rectWaveform.Width(), m_rectWaveform.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
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
					pDC->StretchBlt(m_rectMeasurePanel.left, m_rectMeasurePanel.top, m_rectMeasurePanel.Width(), m_rectMeasurePanel.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);
					//dc.StretchBlt(m_rectMeasurePanel.left, m_rectMeasurePanel.top, m_rectMeasurePanel.Width(), m_rectMeasurePanel.Height(), m_pMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
				}
			}

			m_bNeedRedrawMeasurePanelBmp = 0;
		}

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif
}

void CInstrumentView_View::DisplayTheWholeSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp)
{
	//虚函数，实际的绘图功能由继承类实现
}

void CInstrumentView_View::DisplayTheNewSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp)
{
	//虚函数，实际的绘图功能由继承类实现
}

void CInstrumentView_View::Set(void)
{
	srand(GetTickCount());
	m_uiTimerID = rand();
	SetTimer(m_uiTimerID, 1000, NULL);
}

void CInstrumentView_View::Reset(void)
{
	KillTimer(m_uiTimerID);

#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		m_nMeasuredYMeanValue = UNCREDITABLE_MAX_VALUE;
		m_nMeasuredYRmsValue = UNCREDITABLE_MAX_VALUE;			//
		m_nMeasuredYMinValue = UNCREDITABLE_MIN_VALUE;
		m_nMeasuredYMaxValue = UNCREDITABLE_MAX_VALUE;

		m_nYNegtiveMark = m_nYAlarmMinLimit;
		m_nYPositiveMark = m_nYAlarmMaxLimit;
		m_nYZeroMark = (m_nYNegtiveMark + m_nYPositiveMark) / 2;

		m_nMeasuredXMeanValue = UNCREDITABLE_MAX_VALUE;
		m_nMeasuredXRmsValue = UNCREDITABLE_MAX_VALUE;			//
		m_nMeasuredXMinValue = UNCREDITABLE_MIN_VALUE;
		m_nMeasuredXMaxValue = UNCREDITABLE_MAX_VALUE;

		m_nXNegtiveMark = m_nXAlarmMinLimit;
		m_nXPositiveMark = m_nXAlarmMaxLimit;
		m_nXZeroMark = (m_nXNegtiveMark + m_nXPositiveMark) / 2;

		m_bNeedRedrawAllBmp = 1;

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif

	Invalidate(FALSE);
}

void CInstrumentView_View::Clear(void)
{
	BITMAP bm;
	CRect rectPicture;

	m_pWaveformBmp->GetBitmap(&bm);

	rectPicture.left = 0;
	rectPicture.top = 0;
	rectPicture.right = bm.bmWidth;
	rectPicture.bottom = bm.bmHeight;

	m_pMemDC->SelectObject(m_pWaveformBmp);
	m_pMemDC->FillRect(&rectPicture, m_pBkgroundBrush);
}

void CInstrumentView_View::SetTitle(char* pszTitle)
{
	m_strTitle = pszTitle;
}

void CInstrumentView_View::Init_X_Axis(int nXAxisStyle, int nXShownOption, int nXMinAlarm, int nXMaxAlarm, char* pszXUnits, int nXFloor, int nXCeil, int nXStep)
{
	strcpy_s(m_pszXUnits, sizeof(m_pszXUnits), pszXUnits);

	m_nXPanelShownOption = (nXShownOption & 0xffff0000);
	m_nXMarkShownOption = nXShownOption & 0x0000ffff;
	m_nXAxisStyle = nXAxisStyle;

	m_nXAlarmMinLimit = nXMinAlarm;
	m_nXAlarmMaxLimit = nXMaxAlarm;

	m_nXFloor = nXFloor;
	m_nXCeil = nXCeil;
	m_nXStep = nXStep;

	//AdjustLayout();
}

void CInstrumentView_View::Init_Y_Axis(int nYAxisStyle, int nYShownOption, int nYMinAlarm, int nYMaxAlarm, char* pszYUnits, int nYFloor, int nYCeil, int nYStep)
{
	strcpy_s(m_pszYUnits, sizeof(m_pszYUnits), pszYUnits);

	m_nYPanelShownOption = (nYShownOption & 0xffff0000);
	m_nYMarkShownOption = nYShownOption & 0x0000ffff;
	m_nYAxisStyle = nYAxisStyle;

	m_nYAlarmMinLimit = nYMinAlarm;
	m_nYAlarmMaxLimit = nYMaxAlarm;

	m_nYFloor = nYFloor;
	m_nYCeil = nYCeil;
	m_nYStep = nYStep;

	//AdjustLayout();
}


void CInstrumentView_View::AdjustLayout(void)
{
	int markWidth = max(RECT_YMARK_WIDTH, RECT_XMARK_WIDTH / 2);

	if ((m_nXPanelShownOption == MEASURE_PANEL_HIDE) && (m_nYPanelShownOption == MEASURE_PANEL_HIDE))
	{
		m_rectWaveform.left = m_rectClient.left + X_SEPARATOR + markWidth + X_SEPARATOR;
		m_rectWaveform.right = m_rectClient.right - X_SEPARATOR - RECT_XMARK_WIDTH / 2;
		m_rectWaveform.top = m_rectClient.top + Y_SEPARATOR + RECT_TITLE_HEIGHT + Y_SEPARATOR;
		m_rectWaveform.bottom = m_rectClient.bottom - Y_SEPARATOR - RECT_XMARK_HEIGHT - Y_SEPARATOR - RECT_YMARK_HEIGHT / 2;

		m_rectMeasurePanel.left = 0;
		m_rectMeasurePanel.right = 0;
		m_rectMeasurePanel.top = 0;
		m_rectMeasurePanel.bottom = 0;
	}
	else
	{
		m_rectWaveform.left = m_rectClient.left + X_SEPARATOR + markWidth + X_SEPARATOR;
		m_rectWaveform.right = m_rectClient.right - X_SEPARATOR - RECT_MEASURE_WIDTH - X_SEPARATOR - X_SEPARATOR;
		m_rectWaveform.top = m_rectClient.top + Y_SEPARATOR + RECT_TITLE_HEIGHT + Y_SEPARATOR;
		m_rectWaveform.bottom = m_rectClient.bottom - Y_SEPARATOR - RECT_XMARK_HEIGHT - Y_SEPARATOR - RECT_YMARK_HEIGHT / 2;

		m_rectMeasurePanel.left = m_rectClient.right - X_SEPARATOR - RECT_MEASURE_WIDTH;
		m_rectMeasurePanel.right = m_rectClient.right - X_SEPARATOR;
		m_rectMeasurePanel.top = m_rectWaveform.top;
		m_rectMeasurePanel.bottom = m_rectWaveform.bottom;
	}

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

int CInstrumentView_View::XMAP_Value2Pos(int xvalue, CRect rectPicture)
{
	int xPos = -1;

	if ((xvalue >= m_nXNegtiveMark) && (xvalue <= m_nXPositiveMark))
	{
		if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			double ratio = double(xvalue - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
			xPos = (int)(rectPicture.left + ratio * rectPicture.Width());
		}
		else if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY)		//对称型
		{
			if (xvalue > m_nXZeroMark)
			{
				double ratio = double(xvalue - m_nXZeroMark) / (m_nXPositiveMark - m_nXZeroMark);
				xPos = (int)(rectPicture.left + 0.5 * (1 + ratio) * rectPicture.Width());
			}
			else if (xvalue < m_nXZeroMark)
			{
				double ratio = double(m_nXZeroMark - xvalue) / (m_nXZeroMark - m_nXNegtiveMark);
				xPos = (int)(rectPicture.left + 0.5 * (1 - ratio) * rectPicture.Width());
			}
		}
		else if (m_nXAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)		//对称型
		{
			if (xvalue > m_nXZeroMark)
			{
				double ratio = log10((xvalue - m_nXZeroMark) / m_dXLogArithmicCoeff) / LOGARITHMIC_RANK;
				xPos = (int)(rectPicture.left + 0.5 * (1 + ratio) * rectPicture.Width());
			}
			else if (xvalue < m_nXZeroMark)
			{
				double ratio = log10((m_nXZeroMark - xvalue) / m_dXLogArithmicCoeff) / LOGARITHMIC_RANK;
				xPos = (int)(rectPicture.left + 0.5 * (1 - ratio) * rectPicture.Width());
			}
		}
	}

	return xPos;
}

int CInstrumentView_View::YMAP_Value2Pos(int yvalue, CRect rectPicture)
{
	int yPos = -1;

	if ((yvalue >= m_nYNegtiveMark) && (yvalue <= m_nYPositiveMark))
	{
		if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			double ratio = double(yvalue - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
			yPos = (int)(rectPicture.bottom - ratio * rectPicture.Height());
		}
		else if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY)		//对称型
		{
			if (yvalue > m_nYZeroMark)
			{
				double ratio = double(yvalue - m_nYZeroMark) / (m_nYPositiveMark - m_nYZeroMark);
				yPos = (int)(rectPicture.top + 0.5 * (1 - ratio) * rectPicture.Height());
			}
			else if (yvalue < m_nYZeroMark)
			{
				double ratio = double(m_nYZeroMark - yvalue) / (m_nYZeroMark - m_nYNegtiveMark);
				yPos = (int)(rectPicture.top + 0.5 * (1 + ratio) * rectPicture.Height());
			}
		}
		else if (m_nYAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)		//对称型
		{
			if (yvalue > m_nYZeroMark)
			{
				double ratio = log10((yvalue - m_nYZeroMark) / m_dYLogArithmicCoeff) / LOGARITHMIC_RANK;
				yPos = (int)(rectPicture.top + 0.5 * (1 - ratio) * rectPicture.Height());
			}
			else if (yvalue < m_nYZeroMark)
			{
				double ratio = log10((m_nYZeroMark - yvalue) / m_dYLogArithmicCoeff) / LOGARITHMIC_RANK;
				yPos = (int)(rectPicture.top + 0.5 * (1 + ratio) * rectPicture.Height());
			}
		}
	}

	return yPos;
}

// CInstrumentView_Kernel 诊断

#ifdef _DEBUG
void CInstrumentView_View::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CInstrumentView_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInstrumentView_Kernel 消息处理程序


int CInstrumentView_View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
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

	ReleaseDC(pDC);

#if ON_PAINTING_USE_MUTEX
	m_hPaintingAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif

	//srand(GetTickCount());
	//m_uiTimerID = rand();
	//SetTimer(m_uiTimerID, 1000, NULL);

	return 0;
}


void CInstrumentView_View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//KillTimer(m_uiTimerID);

#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		if (m_pMemDC != NULL)
		{
			delete m_pMemDC;
			m_pMemDC = NULL;
		}

		if (m_pBkgroundBmp != NULL)
		{
			delete m_pBkgroundBmp;
			m_pBkgroundBmp = NULL;
		}

		if (m_pWaveformBmp != NULL)
		{
			delete m_pWaveformBmp;
			m_pWaveformBmp = NULL;
		}

		if (m_pMeasurePanelBmp != NULL)
		{
			delete m_pMeasurePanelBmp;
			m_pMeasurePanelBmp = NULL;
		}

		if (m_pBkgroundBrush != NULL)
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

#if ON_PAINTING_USE_MUTEX
		::CloseHandle(m_hPaintingAccess);
		m_hPaintingAccess = NULL;
	}
#endif

}


void CInstrumentView_View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == m_uiTimerID)
	{
		Invalidate(FALSE);
	}

	CView::OnTimer(nIDEvent);
}


void CInstrumentView_View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (cx > 0 && cy > 0)
	{
		if (m_pMemDC != NULL)
		{
			GetClientRect(&m_rectClient);
			AdjustLayout();
		}
	}
}


