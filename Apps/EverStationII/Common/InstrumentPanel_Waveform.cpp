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
#include "InstrumentPanel_Waveform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Waveform
CInstrumentPanel_Waveform::CInstrumentPanel_Waveform()
{
	m_nChannleDepth = WAVEFORM_SAMPLE_DEPTH;
}

CInstrumentPanel_Waveform::~CInstrumentPanel_Waveform()
{
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_Waveform, CInstrumentPanel_Base)
	//{{AFX_MSG_MAP(CInstrumentPanel_Waveform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//注：这里varValue是相对于offset的标准方差
//ideaValue -- 期望接近真值的一个理想值，实际也是存在误差的估计值
//nSampleCurValue -- 样本当前测量值
//nSampleMeanValue -- 样本长期测量均值
//nSampleVarValue -- 样本长期最大偏离度
void CInstrumentPanel_Waveform::AppendSample(int ID, int sampleValue, SAMPLE_ATTRIBUTE_t* attr)
{
	int nNegtiveBias, nPositiveBias;

	if (attr != NULL)
	{
		m_nMeasuredYMeanValue = attr->mean;
		m_nMeasuredYRmsValue = attr->rms;

		if (attr->min < m_nMeasuredYMinValue)
		{
			m_nMeasuredYMinValue = attr->min;
		}
		if (attr->max > m_nMeasuredYMaxValue)
		{
			m_nMeasuredYMaxValue = attr->max;
		}

		int bRedraw = 0;
		if (m_nYAxisStyle == AXIS_STYLE_MEAN_SYMMETRY)
		{
			nNegtiveBias = m_nYNegtiveMark;
			if (attr->min < m_nYNegtiveMark)
			{
				if (attr->min > m_nYFloor)
				{
					nNegtiveBias = (int)(floor(attr->min / (double)m_nYStep) * m_nYStep);
					bRedraw = 1;
				}
			}

			nPositiveBias = m_nYPositiveMark;
			if (attr->max > m_nYPositiveMark)
			{
				if (attr->max < m_nYCeil)
				{
					nPositiveBias = (int)(ceil(attr->max / (double)m_nYStep) * m_nYStep);
					bRedraw = 1;
				}
			}

			if (bRedraw)
			{
				int bias = max(abs(nNegtiveBias), abs(nPositiveBias));
				m_nYNegtiveMark = -bias;
				m_nYPositiveMark = bias;
			}
		}
		else if (m_nYAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)
		{
			if (attr->min < m_nYNegtiveMark)
			{
				if (attr->min > m_nYFloor)
				{
					m_nYNegtiveMark = (int)(floor(attr->min / (double)m_nYStep) * m_nYStep);
					bRedraw = 1;
				}
			}
			if (attr->max > m_nYPositiveMark)
			{
				if (attr->max < m_nYCeil)
				{
					m_nYPositiveMark = (int)(ceil(attr->max / (double)m_nYStep) * m_nYStep);
					bRedraw = 1;
				}
			}
		}

		if (bRedraw)
		{
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYBottomMark, m_nYNegtiveMark);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYMidMark, (m_nYNegtiveMark + m_nYPositiveMark) / 2);
			DisplayMeasureScale(m_pMemDC, m_pBkgroundBmp, m_rectYTopMark, m_nYPositiveMark);

			DisplayBkGrid(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
			DisplayYAlarmLine(m_pMemDC, m_pBkgroundBmp, m_rectWaveform);
		}
	}

	CInstrumentPanel_Base::AppendYSample(ID, sampleValue);
}

void CInstrumentPanel_Waveform::DisplayMeasureGraph(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int		i;
	int		max_count = 0;

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
			pMemDC->FillRect(&rectPicture, m_pBkBrush);

			double  deltX = (double)rectPicture.Width() / WAVEFORM_SAMPLE_COUNT;
			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

				CPen* pWaveformPen = new CPen;
				pWaveformPen->CreatePen(PS_SOLID, 1, pChannel->color);

				::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
				if (pChannel->nSampleCount > 0)
				{
					CPoint	point[WAVEFORM_SAMPLE_COUNT];
					double	x;
					double 	yoffset;
					double  ratio;
					int		rdindex;

					yoffset = rectPicture.bottom - (double)rectPicture.Height() / 2;

					CPen* pOldPen = pMemDC->SelectObject(pWaveformPen);
					x = rectPicture.left;

					if (m_nXAxisStyle == AXIS_STYLE_MEAN_SYMMETRY)
					{
						if (pChannel->nSampleCount < WAVEFORM_SAMPLE_COUNT)
						{
							for (i = 0; i < pChannel->nSampleCount; i++)
							{
								point[i].x = (int)x;

								ratio = (double)pChannel->pnYSampleArray[i] / m_nYPositiveMark;
								point[i].y = (int)(yoffset - ratio * rectPicture.Height() / 2);
								if (point[i].y < rectPicture.top)
								{
									point[i].y = rectPicture.top;
								}
								else if (point[i].y > rectPicture.bottom)
								{
									point[i].y = rectPicture.bottom;
								}

								x += deltX;
							}
						}
						else
						{
							rdindex = pChannel->nSampleIndex;
							for (i = 0; i < WAVEFORM_SAMPLE_COUNT; i++)
							{
								point[i].x = (int)x;
								ratio = (double)pChannel->pnYSampleArray[rdindex] / m_nYPositiveMark;
								point[i].y = (int)(yoffset - ratio * rectPicture.Height() / 2);
								if (point[i].y < rectPicture.top)
								{
									point[i].y = rectPicture.top;
								}
								else if (point[i].y > rectPicture.bottom)
								{
									point[i].y = rectPicture.bottom;
								}

								x += deltX;

								rdindex++;
								rdindex %= WAVEFORM_SAMPLE_COUNT;
							}
						}
					}
					else if (m_nXAxisStyle == AXIS_STYLE_FROM_MIN_TO_MAX)
					{
						yoffset = rectPicture.bottom;

						if (pChannel->nSampleCount < WAVEFORM_SAMPLE_COUNT)
						{
							for (i = 0; i < pChannel->nSampleCount; i++)
							{
								point[i].x = (int)x;

								ratio = (double)(pChannel->pnYSampleArray[i] - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
								point[i].y = (int)(yoffset - ratio * rectPicture.Height());
								if (point[i].y < rectPicture.top)
								{
									point[i].y = rectPicture.top;
								}

								x += deltX;
							}
						}
						else
						{
							rdindex = pChannel->nSampleIndex;
							for (i = 0; i < WAVEFORM_SAMPLE_COUNT; i++)
							{
								point[i].x = (int)x;
								ratio = (double)(pChannel->pnYSampleArray[rdindex] - m_nYNegtiveMark) / (m_nYPositiveMark - m_nYNegtiveMark);
								point[i].y = (int)(yoffset - ratio * rectPicture.Height());
								if (point[i].y < rectPicture.top)
								{
									point[i].y = rectPicture.top;
								}

								x += deltX;

								rdindex++;
								rdindex %= WAVEFORM_SAMPLE_COUNT;
							}
						}
					}

					pMemDC->Polyline(point, pChannel->nSampleCount);
				}

				m_bNeedUpdate = 0;

				::SetEvent(pChannel->hSampleAccess);

				delete pWaveformPen;
			}
		}
	}
}

