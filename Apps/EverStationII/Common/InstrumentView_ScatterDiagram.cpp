// InstrumentView_ScatterDiagram.cpp: 实现文件
//
#include "stdafx.h"
//#include "pch.h"
#include "../EverStationII.h"
#include "InstrumentView_ScatterDiagram.h"


// CInstrumentView_ScatterDiagram

IMPLEMENT_DYNCREATE(CInstrumentView_ScatterDiagram, CInstrumentView_Base)

CInstrumentView_ScatterDiagram::CInstrumentView_ScatterDiagram()
{
	m_nChannleDepth = SCATTER_DIAGRAM_SAMPLE_DEPTH;
}

CInstrumentView_ScatterDiagram::~CInstrumentView_ScatterDiagram()
{
}

BEGIN_MESSAGE_MAP(CInstrumentView_ScatterDiagram, CInstrumentView_Base)
END_MESSAGE_MAP()

// CInstrumentView_ScatterDiagram 诊断

#ifdef _DEBUG
void CInstrumentView_ScatterDiagram::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CInstrumentView_ScatterDiagram::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInstrumentView_ScatterDiagram 消息处理程序
void CInstrumentView_ScatterDiagram::DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
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

void CInstrumentView_ScatterDiagram::DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
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
void CInstrumentView_ScatterDiagram::AppendSample(int ID, int x, int y)
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

		CInstrumentView_Base::AppendXYSample(ID, x, y);

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif
}

