//

#include "stdafx.h"
#include <afxwin.h>
#include <assert.h>
#include "InstrumentPanel_ColorSpaceDiagram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Histogram
CInstrumentPanel_ColorSpaceDiagram::CInstrumentPanel_ColorSpaceDiagram()
{
	m_nChannleDepth = COLORSPACE_DIAGRAM_SAMPLE_DEPTH;
}

CInstrumentPanel_ColorSpaceDiagram::~CInstrumentPanel_ColorSpaceDiagram()
{
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_ColorSpaceDiagram, CInstrumentPanel_Base)
	//{{AFX_MSG_MAP(CInstrumentPanel_ColorSpaceDiagram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstrumentPanel_ColorSpaceDiagram::DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int    xn = 0;
	double xsample_sum = 0;
	double xpower_sum = 0;

	for (int ch = 0; ch < m_nChannleCount; ch++)
	{
		SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];
		if (pChannel->nSampleCount > 0) {

			for (int i = 0; i < pChannel->nSampleCount; i++)
			{
				double normal_sample = (double)pChannel->pnXArray[i] / m_nMeasuredXMaxValue;
				xsample_sum += normal_sample;
				xpower_sum += (normal_sample * normal_sample);
			}
		}

		xn += pChannel->nSampleCount;
	}

	if (xn > 1)
	{
		m_nMeasuredXMeanValue = (int)round(m_nMeasuredXMaxValue * xsample_sum / xn);

		double power_sigma = xpower_sum - (xsample_sum * xsample_sum) / xn;

		if (power_sigma < 0)
		{
			power_sigma = 0.0;
		}

		double sigma = sqrt(power_sigma / (xn - 1));

		m_nMeasuredXRmsValue = (int)round(m_nMeasuredXMaxValue*sigma);
	}

	int    yn = 0;
	double ysample_sum = 0;
	double ypower_sum = 0;

	for (int ch = 0; ch < m_nChannleCount; ch++)
	{
		SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];
		if (pChannel->nSampleCount > 0) {

			for (int i = 0; i < pChannel->nSampleCount; i++)
			{
				double normal_sample = (double)pChannel->pnYArray[i] / m_nMeasuredYMaxValue;
				ysample_sum += normal_sample;
				ypower_sum += (normal_sample * normal_sample);
			}
		}

		yn += pChannel->nSampleCount;
	}

	if (yn > 1)
	{
		m_nMeasuredYMeanValue = (int)round(m_nMeasuredYMaxValue * ysample_sum / yn);

		double power_sigma = ypower_sum - (ysample_sum * ysample_sum) / yn;

		if (power_sigma < 0)
		{
			power_sigma = 0.0;
		}

		double sigma = sqrt(power_sigma / (yn - 1));

		m_nMeasuredYRmsValue = (int)round(m_nMeasuredYMaxValue*sigma);
	}

	BITMAP bm;
	if ((pGraphBmp != NULL) && (pMemDC != NULL))
	{
		if (pGraphBmp->GetSafeHandle() != NULL)
		{
			pGraphBmp->GetBitmap(&bm);
			CRect rectPicture;

			rectPicture.left = 0;
			rectPicture.top = 0;
			rectPicture.right = bm.bmWidth;
			rectPicture.bottom = bm.bmHeight;

			pMemDC->SelectObject(pGraphBmp);
			pMemDC->SetBkColor(SCREEN_BKWAVEFORMCOLOR);
			pMemDC->FillRect(&rectPicture, m_pWaveformBrush);

			CPoint	point;

			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

				if (pChannel->nSampleCount > 0)
				{
#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
					}
#endif
					CPen* pWaveformPen = new CPen;
					pWaveformPen->CreatePen(PS_SOLID, 3, pChannel->color);
					pMemDC->SelectObject(pWaveformPen);

					CBrush* pPaintBrush = new CBrush;
					pPaintBrush->CreateSolidBrush(pChannel->color);
					CBrush* pOldBrush = pMemDC->SelectObject(pPaintBrush);

					if (pChannel->nSampleCount < m_nChannleDepth)
					{
						assert(pChannel->nRdIndex <= pChannel->nWrIndex);

						for (int rdIndex = 0; rdIndex < pChannel->nRdIndex; rdIndex++)
						{
							point.x = XMAP_Value2Pos(pChannel->pnXArray[rdIndex], rectPicture);
							point.y = YMAP_Value2Pos(pChannel->pnYArray[rdIndex], rectPicture);

							if ((point.x >= 0) && (point.y >= 0))
							{
								//pMemDC->TextOutA(point.x, point.y, "O");
								//pMemDC->SetPixel(point, pChannel->color);
								//pMemDC->SetPixel(point.x + 1, point.y - 1, pChannel->color);
								//pMemDC->SetPixel(point.x - 1, point.y - 1, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x - 1, point.y + 1, pChannel->color);
								//pMemDC->SetPixel(point.x + 1, point.y + 1, pChannel->color);

								//pMemDC->SetPixel(point.x - 2, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x - 1, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x + 1, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x + 2, point.y, pChannel->color);

								//pMemDC->SetPixel(point.x, point.y - 2, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y - 1, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y + 1, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y + 2, pChannel->color);
								pMemDC->MoveTo(point.x - 2, point.y);
								pMemDC->LineTo(point.x + 2, point.y);
								pMemDC->MoveTo(point.x, point.y - 2);
								pMemDC->LineTo(point.x, point.y + 2);
								//RECT rectPoint;
								//rectPoint.left = point.x - radis;
								//rectPoint.top = point.y - radis;
								//rectPoint.right = point.x + radis;
								//rectPoint.bottom = point.y + radis;
								//pMemDC->Ellipse(&rectPoint);
								//pMemDC->FillRect(&rectPoint, pPaintBrush);
							}

							//pChannel->pstSampleArray[i].bConsumed = 1;
							//pChannel->pbConsumed[i] = 1;
						}
					}
					else
					{
						int rdIndex = pChannel->nWrIndex;
						do
						{
							point.x = XMAP_Value2Pos(pChannel->pnXArray[rdIndex], rectPicture);
							point.y = YMAP_Value2Pos(pChannel->pnYArray[rdIndex], rectPicture);

							if ((point.x >= 0) && (point.y >= 0))
							{
								//pMemDC->TextOutA(point.x, point.y, "O");
								//pMemDC->SetPixel(point, pChannel->color);
								//pMemDC->SetPixel(point.x + 1, point.y - 1, pChannel->color);
								//pMemDC->SetPixel(point.x - 1, point.y - 1, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x - 1, point.y + 1, pChannel->color);
								//pMemDC->SetPixel(point.x + 1, point.y + 1, pChannel->color);

								//pMemDC->SetPixel(point.x - 2, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x - 1, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x + 1, point.y, pChannel->color);
								//pMemDC->SetPixel(point.x + 2, point.y, pChannel->color);

								//pMemDC->SetPixel(point.x, point.y - 2, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y - 1, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y + 1, pChannel->color);
								//pMemDC->SetPixel(point.x, point.y + 2, pChannel->color);
								pMemDC->MoveTo(point.x - 2, point.y);
								pMemDC->LineTo(point.x + 2, point.y);
								pMemDC->MoveTo(point.x, point.y - 2);
								pMemDC->LineTo(point.x, point.y + 2);
								//RECT rectPoint;
								//rectPoint.left = point.x - radis;
								//rectPoint.top = point.y - radis;
								//rectPoint.right = point.x + radis;
								//rectPoint.bottom = point.y + radis;
								//pMemDC->Ellipse(&rectPoint);
								//pMemDC->FillRect(&rectPoint, pPaintBrush);
							}

							//pChannel->pstSampleArray[i].bConsumed = 1;
							//pChannel->pbConsumed[rdIndex] = 1;

							rdIndex++;
							rdIndex %= m_nChannleDepth;
							if (rdIndex == pChannel->nRdIndex)
							{
								break;
							}

						} while (1);
					}

					delete pWaveformPen;
					delete pPaintBrush;

#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::SetEvent(pChannel->hSampleAccess);
					}
#endif
				}
			}
		}
	}
}

void CInstrumentPanel_ColorSpaceDiagram::DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int    xn = 0;
	double xsample_sum = 0;
	double xpower_sum = 0;

	for (int ch = 0; ch < m_nChannleCount; ch++)
	{
		SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];
		if (pChannel->nSampleCount > 0) {

			for (int i = 0; i < pChannel->nSampleCount; i++)
			{
				double normal_sample = (double)pChannel->pnXArray[i] / m_nMeasuredXMaxValue;
				xsample_sum += normal_sample;
				xpower_sum += (normal_sample * normal_sample);
			}
		}

		xn += pChannel->nSampleCount;
	}

	if (xn > 1)
	{
		m_nMeasuredXMeanValue = (int)round(m_nMeasuredXMaxValue * xsample_sum / xn);

		double power_sigma = xpower_sum - (xsample_sum * xsample_sum) / xn;

		if (power_sigma < 0)
		{
			power_sigma = 0.0;
		}

		double sigma = sqrt(power_sigma / (xn - 1));

		m_nMeasuredXRmsValue = (int)round(m_nMeasuredXMaxValue*sigma);
	}

	int    yn = 0;
	double ysample_sum = 0;
	double ypower_sum = 0;

	for (int ch = 0; ch < m_nChannleCount; ch++)
	{
		SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];
		if (pChannel->nSampleCount > 0) {

			for (int i = 0; i < pChannel->nSampleCount; i++)
			{
				double normal_sample = (double)pChannel->pnYArray[i] / m_nMeasuredYMaxValue;
				ysample_sum += normal_sample;
				ypower_sum += (normal_sample * normal_sample);
			}
		}

		yn += pChannel->nSampleCount;
	}

	if (yn > 1)
	{
		m_nMeasuredYMeanValue = (int)round(m_nMeasuredYMaxValue * ysample_sum / yn);

		double power_sigma = ypower_sum - (ysample_sum * ysample_sum) / yn;

		if (power_sigma < 0)
		{
			power_sigma = 0.0;
		}

		double sigma = sqrt(power_sigma / (yn - 1));

		m_nMeasuredYRmsValue = (int)round(m_nMeasuredYMaxValue*sigma);
	}

	BITMAP bm;
	if ((pGraphBmp != NULL) && (pMemDC != NULL))
	{
		if (pGraphBmp->GetSafeHandle() != NULL)
		{
			pGraphBmp->GetBitmap(&bm);

			CRect rectPicture;

			rectPicture.left = 0;
			rectPicture.top = 0;
			rectPicture.right = bm.bmWidth;
			rectPicture.bottom = bm.bmHeight;

			pMemDC->SelectObject(pGraphBmp);

			CPoint	point;
			//double half_height = rectPicture.Height() / 2.0;
			//double yoffset = rectPicture.bottom - half_height;
			//double diameter = rectPicture.Width() / (double)(m_nXPositiveMark - m_nXNegtiveMark);
			//int radis = (int)ceil(diameter / 4.0);

			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

				if (pChannel->bEmpty == 0)
				{
#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
					}
#endif
					CPen* pWaveformPen = new CPen;
					pWaveformPen->CreatePen(PS_SOLID, 3, pChannel->color);
					pMemDC->SelectObject(pWaveformPen);

					//CBrush* pPaintBrush = new CBrush;
					//pPaintBrush->CreateSolidBrush(pChannel->color);
					//CBrush* pOldBrush = pMemDC->SelectObject(pPaintBrush);

					do
					{
						int rdIndex = pChannel->nRdIndex;
						point.x = XMAP_Value2Pos(pChannel->pnXArray[rdIndex], rectPicture);
						point.y = YMAP_Value2Pos(pChannel->pnYArray[rdIndex], rectPicture);

						if ((point.x >= 0) && (point.y >= 0))
						{
							//pMemDC->TextOutA(point.x, point.y, "O");
							//pMemDC->SetPixel(point, pChannel->color);
							//pMemDC->SetPixel(point.x + 1, point.y - 1, pChannel->color);
							//pMemDC->SetPixel(point.x - 1, point.y - 1, pChannel->color);
							//pMemDC->SetPixel(point.x, point.y, pChannel->color);
							//pMemDC->SetPixel(point.x - 1, point.y + 1, pChannel->color);
							//pMemDC->SetPixel(point.x + 1, point.y + 1, pChannel->color);

							//pMemDC->SetPixel(point.x - 2, point.y, pChannel->color);
							//pMemDC->SetPixel(point.x - 1, point.y, pChannel->color);
							//pMemDC->SetPixel(point.x + 1, point.y, pChannel->color);
							//pMemDC->SetPixel(point.x + 2, point.y, pChannel->color);

							//pMemDC->SetPixel(point.x, point.y - 2, pChannel->color);
							//pMemDC->SetPixel(point.x, point.y - 1, pChannel->color);
							//pMemDC->SetPixel(point.x, point.y + 1, pChannel->color);
							//pMemDC->SetPixel(point.x, point.y + 2, pChannel->color);

							pMemDC->MoveTo(point.x - 2, point.y);
							pMemDC->LineTo(point.x + 2, point.y);
							pMemDC->MoveTo(point.x, point.y - 2);
							pMemDC->LineTo(point.x, point.y + 2);

							//RECT rectPoint;
							//rectPoint.left = point.x - radis;
							//rectPoint.top = point.y - radis;
							//rectPoint.right = point.x + radis;
							//rectPoint.bottom = point.y + radis;
							//pMemDC->Ellipse(&rectPoint);

							//pMemDC->FillRect(&rectPoint, pPaintBrush);
						}

						//pChannel->pstSampleArray[i].bConsumed = 1;
						//pChannel->pbConsumed[rdIndex] = 1;

						if (pChannel->bFull == 1) pChannel->bFull = 0;

						pChannel->nRdIndex++;
						pChannel->nRdIndex %= m_nChannleDepth;

						if (pChannel->nRdIndex == pChannel->nWrIndex)
						{
							pChannel->bEmpty = 1;
							break;
						}

					} while (1);

					delete pWaveformPen;
					//delete pPaintBrush;

					//pChannel->bNeedRedrawing = 0;

#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::SetEvent(pChannel->hSampleAccess);
					}
#endif
				}
			}
		}
	}
}

//注：这里varValue是相对于offset的标准方差
//ideaValue -- 期望接近真值的一个理想值，实际也是存在误差的估计值
//nSampleCurValue -- 样本当前测量值
//nSampleMeanValue -- 样本长期测量均值
//nSampleVarValue -- 样本长期最大偏离度
void CInstrumentPanel_ColorSpaceDiagram::AppendSample(int ID, int x, int y)
{
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		int nNegtiveBias, nPositiveBias;

		if (x < m_nMeasuredXMinValue)
		{
			m_nMeasuredXMinValue = x;
		}
		if (x > m_nMeasuredXMaxValue)
		{
			m_nMeasuredXMaxValue = x;
		}

		assert(m_nXAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX);

		if (m_nMeasuredXMinValue < m_nXNegtiveMark)
		{
			if (m_nMeasuredXMinValue > m_nXFloor)
			{
				m_nXNegtiveMark = (int)(floor(m_nMeasuredXMinValue / (double)m_nXStep) * m_nXStep);
				m_bNeedRedrawAllBmp = 1;
			}
		}
		if (m_nMeasuredXMaxValue > m_nXPositiveMark)
		{
			if (m_nMeasuredXMaxValue < m_nXCeil)
			{
				m_nXPositiveMark = (int)(ceil(m_nMeasuredXMaxValue / (double)m_nXStep) * m_nXStep);
				m_bNeedRedrawAllBmp = 1;
			}
		}

		if (y < m_nMeasuredYMinValue)
		{
			m_nMeasuredYMinValue = y;
		}
		if (y > m_nMeasuredYMaxValue)
		{
			m_nMeasuredYMaxValue = y;
		}

		if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY)
		{
			nNegtiveBias = m_nYNegtiveMark;
			if (m_nMeasuredYMinValue < m_nYNegtiveMark)
			{
				if (m_nMeasuredYMinValue > m_nYFloor)
				{
					nNegtiveBias = (int)(floor(m_nMeasuredYMinValue / (double)m_nYStep) * m_nYStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}

			nPositiveBias = m_nYPositiveMark;
			if (m_nMeasuredYMaxValue > m_nYPositiveMark)
			{
				if (m_nMeasuredYMaxValue < m_nYCeil)
				{
					nPositiveBias = (int)(ceil(m_nMeasuredYMaxValue / (double)m_nYStep) * m_nYStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}

			if (m_bNeedRedrawAllBmp)
			{
				int bias = max(abs(nNegtiveBias), abs(nPositiveBias));
				m_nYNegtiveMark = -bias;
				m_nYPositiveMark = bias;
			}
		}
		else if (m_nYAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)
		{
			nNegtiveBias = m_nYNegtiveMark;
			if (m_nMeasuredYMinValue < m_nYNegtiveMark)
			{
				if (m_nMeasuredYMinValue > m_nYFloor)
				{
					nNegtiveBias = (int)(floor(m_nMeasuredYMinValue / (double)m_nYStep) * m_nYStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}

			nPositiveBias = m_nYPositiveMark;
			if (m_nMeasuredYMaxValue > m_nYPositiveMark)
			{
				if (m_nMeasuredYMaxValue < m_nYCeil)
				{
					nPositiveBias = (int)(ceil(m_nMeasuredYMaxValue / (double)m_nYStep) * m_nYStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}

			if (m_bNeedRedrawAllBmp)
			{
				int bias = max(abs(nNegtiveBias), abs(nPositiveBias));
				m_nYNegtiveMark = -bias;
				m_nYPositiveMark = bias;
			}
		}
		else if (m_nYAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
		{
			if (m_nMeasuredYMinValue < m_nYNegtiveMark)
			{
				if (m_nMeasuredYMinValue > m_nYFloor)
				{
					m_nYNegtiveMark = (int)(floor(m_nMeasuredYMinValue / (double)m_nYStep) * m_nYStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}
			if (m_nMeasuredYMaxValue > m_nYPositiveMark)
			{
				if (m_nMeasuredYMaxValue < m_nYCeil)
				{
					m_nYPositiveMark = (int)(ceil(m_nMeasuredYMaxValue / (double)m_nYStep) * m_nYStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}
		}

		CInstrumentPanel_Base::AppendXYSample(ID, x, y);

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif
}

void CInstrumentPanel_ColorSpaceDiagram::DisplayBkGridInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectGridArea)
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
			int x = (int)(rectGridArea.left + i * dGridDeltx);

			pMemDC->SelectObject(pDotPen);
			pMemDC->MoveTo(x, rectGridArea.top);
			pMemDC->LineTo(x, rectGridArea.bottom);
		}

		for (int i = 1; i < VERTICAL_GRID_DIVISION; i++)
		{
			int y = (int)(rectGridArea.top + i * dGridDelty);

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

		POINT ptRed, ptBlue, ptGreen, ptWhite;
		
		CPen* pColorSpacePen = new CPen;
		pColorSpacePen->CreatePen(PS_SOLID, 1, RGB(160, 160, 160));

		pMemDC->SelectObject(pColorSpacePen);

		//4K UHD color space ITU-T BT.2020
		ptWhite.x = XMAP_Value2Pos(313, rectGridArea);
		ptWhite.y = YMAP_Value2Pos(329, rectGridArea);

		ptRed.x = XMAP_Value2Pos(708, rectGridArea);
		ptRed.y = YMAP_Value2Pos(292, rectGridArea);

		ptGreen.x = XMAP_Value2Pos(170, rectGridArea);
		ptGreen.y = YMAP_Value2Pos(797, rectGridArea);

		ptBlue.x = XMAP_Value2Pos(131, rectGridArea);
		ptBlue.y = YMAP_Value2Pos(46, rectGridArea);

		pMemDC->MoveTo(ptRed);
		pMemDC->LineTo(ptGreen);
		pMemDC->LineTo(ptBlue);
		pMemDC->LineTo(ptRed);
		pMemDC->SetPixel(ptWhite, RGB(255, 255, 255));
		pMemDC->SetPixel(ptRed, RGB(255, 0, 0));
		pMemDC->SetPixel(ptGreen, RGB(0, 255, 0));
		pMemDC->SetPixel(ptBlue, RGB(0, 0, 255));

		//HD color space ITU-T BT.709
		ptRed.x = XMAP_Value2Pos(640, rectGridArea);
		ptRed.y = YMAP_Value2Pos(330, rectGridArea);

		ptGreen.x = XMAP_Value2Pos(300, rectGridArea);
		ptGreen.y = YMAP_Value2Pos(600, rectGridArea);

		ptBlue.x = XMAP_Value2Pos(150, rectGridArea);
		ptBlue.y = YMAP_Value2Pos(60, rectGridArea);

		pMemDC->MoveTo(ptRed);
		pMemDC->LineTo(ptGreen);
		pMemDC->LineTo(ptBlue);
		pMemDC->LineTo(ptRed);

		pMemDC->SetPixel(ptWhite, RGB(255, 255, 255));
		pMemDC->SetPixel(ptRed, RGB(255, 0, 0));
		pMemDC->SetPixel(ptGreen, RGB(0, 255, 0));
		pMemDC->SetPixel(ptBlue, RGB(0, 0, 255));

		pMemDC->SelectObject(pOldPen);

		delete pFramePen;
		delete pDotPen;
		delete pAxisPen;

		delete pMarkFont;
		delete pTitleFont;
	}
}
