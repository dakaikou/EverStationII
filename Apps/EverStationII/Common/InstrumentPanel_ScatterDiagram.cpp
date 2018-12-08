//

#include "stdafx.h"
#include <afxwin.h>
#include <assert.h>
#include "InstrumentPanel_ScatterDiagram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Histogram
CInstrumentPanel_ScatterDiagram::CInstrumentPanel_ScatterDiagram()
{
	m_nChannleDepth = SCATTER_DIAGRAM_SAMPLE_DEPTH;
}

CInstrumentPanel_ScatterDiagram::~CInstrumentPanel_ScatterDiagram()
{
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_ScatterDiagram, CInstrumentPanel_Base)
	//{{AFX_MSG_MAP(CInstrumentPanel_Histogram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstrumentPanel_ScatterDiagram::DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int		i;
	double  ratio;
	CRect	rectPaint;

	//assert(m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX);
	//assert(m_nYAxisStyle == AXIS_STYLE_MEAN_SYMMETRY);

	BITMAP bm;
	if ((pGraphBmp != NULL) && (pMemDC != NULL))
	{
		if (pGraphBmp->GetSafeHandle() != NULL)
		{
			pGraphBmp->GetBitmap(&bm);
			pMemDC->SelectObject(pGraphBmp);

			CRect rectPicture;

			rectPicture.left = 0;
			rectPicture.top = 0;
			rectPicture.right = bm.bmWidth;
			rectPicture.bottom = bm.bmHeight;
			//pMemDC->FillRect(&rectPicture, m_pBkBrush);

			CPoint	point;
			double yoffset = rectPicture.bottom - (double)rectPicture.Height() / 2;
			double diameter = rectPicture.Width() / (double)(m_nXPositiveMark - m_nXNegtiveMark);
			int radis = (int)ceil(diameter / 4.0);

			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

				//if (pChannel->bNeedRedrawing)
				{
					CPen* pWaveformPen = new CPen;
					pWaveformPen->CreatePen(PS_SOLID, 1, pChannel->color);
					pMemDC->SelectObject(pWaveformPen);

					CBrush* pPaintBrush = new CBrush;
					pPaintBrush->CreateSolidBrush(pChannel->color);
					CBrush* pOldBrush = pMemDC->SelectObject(pPaintBrush);

#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
					}
#endif
					if (pChannel->nSampleCount > 0) {

						for (i = 0; i < pChannel->nSampleCount; i++)
						{
							ratio = double(pChannel->pstSampleArray[i].x - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
							point.x = (int)(rectPicture.left + ratio * rectPicture.Width());
							if (point.x < rectPicture.left)
							{
								point.x = rectPicture.left;
							}
							else if (point.x > rectPicture.right)
							{
								point.x = rectPicture.right;
							}

							ratio = (double)pChannel->pstSampleArray[i].y / m_nYPositiveMark;
							point.y = (int)(yoffset - ratio * rectPicture.Height() / 2);
							if (point.y < rectPicture.top)
							{
								point.y = rectPicture.top;
							}
							else if (point.y > rectPicture.bottom)
							{
								point.y = rectPicture.bottom;
							}

							//pMemDC->TextOutA(point.x, point.y, "O");
							//pMemDC->SetPixel(point, pChannel->color);
							RECT rectPoint;
							rectPoint.left = point.x - radis;
							rectPoint.top = point.y - radis;
							rectPoint.right = point.x + radis;
							rectPoint.bottom = point.y + radis;
							pMemDC->Ellipse(&rectPoint);
							//pMemDC->FillRect(&rectPoint, pPaintBrush);

							pChannel->pstSampleArray[i].bConsumed = 1;
						}
					}

					pChannel->bNeedRedrawing = 0;

#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::SetEvent(pChannel->hSampleAccess);
					}
#endif
					delete pWaveformPen;
					delete pPaintBrush;
				}

			}
		}
	}
}

void CInstrumentPanel_ScatterDiagram::DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int		i;
	double  ratio;
	CRect	rectPaint;

	//assert(m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX);
	//assert(m_nYAxisStyle == AXIS_STYLE_MEAN_SYMMETRY);

	BITMAP bm;
	if ((pGraphBmp != NULL) && (pMemDC != NULL))
	{
		if (pGraphBmp->GetSafeHandle() != NULL)
		{
			pGraphBmp->GetBitmap(&bm);
			pMemDC->SelectObject(pGraphBmp);

			CRect rectPicture;

			rectPicture.left = 0;
			rectPicture.top = 0;
			rectPicture.right = bm.bmWidth;
			rectPicture.bottom = bm.bmHeight;
			//pMemDC->FillRect(&rectPicture, m_pBkBrush);

			CPoint	point;
			double yoffset = rectPicture.bottom - (double)rectPicture.Height() / 2;
			double diameter = rectPicture.Width() / (double)(m_nXPositiveMark - m_nXNegtiveMark);
			int radis = (int)ceil(diameter / 4.0);

			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

				if (pChannel->bNeedRedrawing)
				{
					CPen* pWaveformPen = new CPen;
					pWaveformPen->CreatePen(PS_SOLID, 1, pChannel->color);
					pMemDC->SelectObject(pWaveformPen);

					CBrush* pPaintBrush = new CBrush;
					pPaintBrush->CreateSolidBrush(pChannel->color);
					CBrush* pOldBrush = pMemDC->SelectObject(pPaintBrush);

#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
					}
#endif
					if (pChannel->nSampleCount > 0) {

						for (i = 0; i < pChannel->nSampleCount; i++)
						{
							if (pChannel->pstSampleArray[i].bConsumed == 0)
							{
								ratio = double(pChannel->pstSampleArray[i].x - m_nXNegtiveMark) / (m_nXPositiveMark - m_nXNegtiveMark);
								point.x = (int)(rectPicture.left + ratio * rectPicture.Width());
								if (point.x < rectPicture.left)
								{
									point.x = rectPicture.left;
								}
								else if (point.x > rectPicture.right)
								{
									point.x = rectPicture.right;
								}

								ratio = (double)pChannel->pstSampleArray[i].y / m_nYPositiveMark;
								point.y = (int)(yoffset - ratio * rectPicture.Height() / 2);
								if (point.y < rectPicture.top)
								{
									point.y = rectPicture.top;
								}
								else if (point.y > rectPicture.bottom)
								{
									point.y = rectPicture.bottom;
								}

								//pMemDC->TextOutA(point.x, point.y, "O");
								//pMemDC->SetPixel(point, pChannel->color);
								RECT rectPoint;
								rectPoint.left = point.x - radis;
								rectPoint.top = point.y - radis;
								rectPoint.right = point.x + radis;
								rectPoint.bottom = point.y + radis;
								pMemDC->Ellipse(&rectPoint);
								//pMemDC->FillRect(&rectPoint, pPaintBrush);

								pChannel->pstSampleArray[i].bConsumed = 1;
							}
						}
					}

#if INSTRUMENT_PANEL_USE_MUTEX
					if (pChannel->hSampleAccess != NULL)
					{
						::SetEvent(pChannel->hSampleAccess);
					}
#endif
					delete pWaveformPen;
					delete pPaintBrush;

					pChannel->bNeedRedrawing = 0;
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
void CInstrumentPanel_ScatterDiagram::AppendSample(int ID, int xsampleValue, int ysampleValue, SAMPLE_ATTRIBUTE_t* xattr, SAMPLE_ATTRIBUTE_t* yattr)
{
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		int nNegtiveBias, nPositiveBias;

		if (xattr != NULL)
		{
			m_nMeasuredXMeanValue = xattr->mean;
			m_nMeasuredXRmsValue = xattr->rms;

			if (xattr->min < m_nMeasuredXMinValue)
			{
				m_nMeasuredXMinValue = xattr->min;
			}
			if (xattr->max > m_nMeasuredXMaxValue)
			{
				m_nMeasuredXMaxValue = xattr->max;
			}

			assert(m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX);

			if (xattr->min < m_nXNegtiveMark)
			{
				if (xattr->min > m_nXFloor)
				{
					m_nXNegtiveMark = (int)(floor(xattr->min / (double)m_nXStep) * m_nXStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}
			if (xattr->max > m_nXPositiveMark)
			{
				if (xattr->max < m_nXCeil)
				{
					m_nXPositiveMark = (int)(ceil(xattr->max / (double)m_nXStep) * m_nXStep);
					m_bNeedRedrawAllBmp = 1;
				}
			}
		}

		if (yattr != NULL)
		{
			m_nMeasuredYMeanValue = yattr->mean;
			m_nMeasuredYRmsValue = yattr->rms;

			if (yattr->min < m_nMeasuredYMinValue)
			{
				m_nMeasuredYMinValue = yattr->min;
			}
			if (yattr->max > m_nMeasuredYMaxValue)
			{
				m_nMeasuredYMaxValue = yattr->max;
			}

			if (m_nYAxisStyle == AXIS_STYLE_MEAN_SYMMETRY)
			{
				nNegtiveBias = m_nYNegtiveMark;
				if (yattr->min < m_nYNegtiveMark)
				{
					if (yattr->min > m_nYFloor)
					{
						nNegtiveBias = (int)(floor(yattr->min / (double)m_nYStep) * m_nYStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}

				nPositiveBias = m_nYPositiveMark;
				if (yattr->max > m_nYPositiveMark)
				{
					if (yattr->max < m_nYCeil)
					{
						nPositiveBias = (int)(ceil(yattr->max / (double)m_nYStep) * m_nYStep);
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
			else if (m_nYAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)
			{
				if (yattr->min < m_nYNegtiveMark)
				{
					if (yattr->min > m_nYFloor)
					{
						m_nYNegtiveMark = (int)(floor(yattr->min / (double)m_nYStep) * m_nYStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}
				if (yattr->max > m_nYPositiveMark)
				{
					if (yattr->max < m_nYCeil)
					{
						m_nYPositiveMark = (int)(ceil(yattr->max / (double)m_nYStep) * m_nYStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}
			}
		}

		CInstrumentPanel_Base::AppendXYSample(ID, xsampleValue, ysampleValue);

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif
}

