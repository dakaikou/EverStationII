// InstrumentView_ScatterDiagram.cpp: 实现文件
//
#include "stdafx.h"
//#include "pch.h"
//#include "../EverStationII.h"
#include "InstrumentView_Histogram.h"


// CInstrumentView_Histogram

IMPLEMENT_DYNCREATE(CInstrumentView_Histogram, CInstrumentView_Base)

CInstrumentView_Histogram::CInstrumentView_Histogram()
{
	m_nChannleDepth = HISTGRAM_SAMPLE_DEPTH;

}

CInstrumentView_Histogram::~CInstrumentView_Histogram()
{
}

BEGIN_MESSAGE_MAP(CInstrumentView_Histogram, CInstrumentView_Base)
END_MESSAGE_MAP()




// CInstrumentView_Histogram 诊断

#ifdef _DEBUG
void CInstrumentView_Histogram::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CInstrumentView_Histogram::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInstrumentView_Histogram 消息处理程序
void CInstrumentView_Histogram::DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
{
	int		i;
	CRect	rectPaint;
	int		max_count = 0;
	int		max_pos = 0;

	int    n = 0;
	double sample_sum = 0;
	double power_sum = 0;

	for (int ch = 0; ch < m_nChannleCount; ch++)
	{
		SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];
		if (pChannel->nSampleCount > 0) {

			for (i = 0; i < pChannel->nSampleCount; i++)
			{
				double normal_sample = (double)pChannel->pnXArray[i] / m_nMeasuredXMaxValue;
				sample_sum += normal_sample;
				power_sum += (normal_sample * normal_sample);
			}
		}

		n += pChannel->nSampleCount;
	}

	if (n > 1)
	{
		m_nMeasuredXMeanValue = (int)round(m_nMeasuredXMaxValue * sample_sum / n);

		double power_sigma = power_sum - (sample_sum * sample_sum) / n;

		if (power_sigma < 0)
		{
			power_sigma = 0.0;
		}

		double sigma = sqrt(power_sigma / (n - 1));

		m_nMeasuredXRmsValue = (int)round(m_nMeasuredXMaxValue*sigma);
	}

	int		nHistGram[HISTGRAM_HORZ_DIVISION];
	memset(nHistGram, 0x00, sizeof(nHistGram));

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
			pMemDC->FillRect(&rectPicture, m_pWaveformBrush);

			double deltX = (double)rectPicture.Width() / HISTGRAM_HORZ_DIVISION;

			for (int ch = 0; ch < m_nChannleCount; ch++)
			{
				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];

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
						int xPos = XMAP_Value2Pos(pChannel->pnXArray[i], rectPicture);
						if (xPos >= 0)
						{
							int index = (int)(xPos / deltX);
							assert((index >= 0) && (index < HISTGRAM_HORZ_DIVISION));
							nHistGram[index] ++;

							if (nHistGram[index] > max_count)
							{
								max_count = nHistGram[index];
								max_pos = index;
							}
						}
					}

					if (max_count > 0)
					{
						rectPaint.bottom = rectPicture.bottom;

						for (i = 0; i < HISTGRAM_HORZ_DIVISION; i++)
						{
							double ratio = 0.9 * nHistGram[i] / max_count;
							assert(ratio < 1.0);

							rectPaint.left = (int)(rectPicture.left + i * deltX);
							rectPaint.right = (int)(rectPicture.left + (i + 1)*deltX);
							rectPaint.top = rectPicture.bottom - (int)(ratio * rectPicture.Height());
							//assert(rectPaint.top < rectContainer.top);
							pMemDC->FillRect(&rectPaint, pPaintBrush);
						}

						//int mark_x = (int)(rectPicture.left + max_pos * deltX);

						//CString strMark;
						//CRect rectXMark;

						//strMark.Format("%d\n", (int)(m_nXNegtiveMark+ (max_pos+0.5) * deltX));

						//rectXMark.left = mark_x - RECT_XMARK_WIDTH / 2;
						//rectXMark.right = mark_x + RECT_XMARK_WIDTH / 2;
						//rectXMark.top = rectPicture.top + Y_SEPARATOR;
						//rectXMark.bottom = rectXMark.top + RECT_XMARK_HEIGHT;

						//CFont* pMarkFont = new CFont;
						//pMarkFont->CreateFont(FONT_MARK_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);

						//pMemDC->SelectObject(pMarkFont);
						//pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
						//pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);

						//strMark.Format("%d\n", m_nXNegtiveMark);

						//delete pMarkFont;
					}
				}

#if INSTRUMENT_PANEL_USE_MUTEX
				if (pChannel->hSampleAccess != NULL)
				{
					::SetEvent(pChannel->hSampleAccess);
				}
#endif
				pMemDC->SelectObject(pOldBrush);
				delete pPaintBrush;
			}
		}
	}
}

void CInstrumentView_Histogram::DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp)
{
	DisplayTheWholeSamplesInMemory(pMemDC, pGraphBmp);
//	int		i;
//	CRect	rectPaint;
//	int		max_count = 0;
//	int		max_pos = 0;
//
//	int		nHistGram[HISTGRAM_HORZ_DIVISION];
//	memset(nHistGram, 0x00, sizeof(nHistGram));
//
//	BITMAP bm;
//	if ((pGraphBmp != NULL) && (pMemDC != NULL))
//	{
//		if (pGraphBmp->GetSafeHandle() != NULL)
//		{
//			pGraphBmp->GetBitmap(&bm);
//			pMemDC->SelectObject(pGraphBmp);
//
//			CRect rectPicture;
//
//			rectPicture.left = 0;
//			rectPicture.top = 0;
//			rectPicture.right = bm.bmWidth;
//			rectPicture.bottom = bm.bmHeight;
//			pMemDC->FillRect(&rectPicture, m_pWaveformBrush);
//
//			double deltX = (double)rectPicture.Width() / HISTGRAM_HORZ_DIVISION;
//
//			for (int ch = 0; ch < m_nChannleCount; ch++)
//			{
//				SAMPLE_CHANNEL_t* pChannel = m_pChannel[ch];
//
//#if INSTRUMENT_PANEL_USE_MUTEX
//				if (pChannel->hSampleAccess != NULL)
//				{
//					::WaitForSingleObject(pChannel->hSampleAccess, INFINITE);
//				}
//#endif
//
//				if (pChannel->nSampleCount > 0) 
//				{
//					CBrush* pPaintBrush = new CBrush;
//					pPaintBrush->CreateSolidBrush(pChannel->color);
//					CBrush* pOldBrush = pMemDC->SelectObject(pPaintBrush);
//
//					for (i = 0; i < pChannel->nSampleCount; i++)
//					{
//						int xPos = XMAP_Value2Pos(pChannel->pnXArray[i], rectPicture);
//						if (xPos >= 0)
//						{
//							int index = (int)(xPos / deltX);
//							assert((index >= 0) && (index < HISTGRAM_HORZ_DIVISION));
//							nHistGram[index] ++;
//
//							if (nHistGram[index] > max_count)
//							{
//								max_count = nHistGram[index];
//								max_pos = index;
//							}
//						}
//					}
//
//					if (max_count > 0)
//					{
//						rectPaint.bottom = rectPicture.bottom;
//
//						for (i = 0; i < HISTGRAM_HORZ_DIVISION; i++)
//						{
//							double ratio = 0.9 * nHistGram[i] / max_count;
//							assert(ratio < 1.0);
//
//							rectPaint.left = (int)(rectPicture.left + i * deltX);
//							rectPaint.right = (int)(rectPicture.left + (i + 1)*deltX);
//							rectPaint.top = rectPicture.bottom - (int)(ratio * rectPicture.Height());
//							//assert(rectPaint.top < rectContainer.top);
//							pMemDC->FillRect(&rectPaint, pPaintBrush);
//						}
//
//						//int mark_x = (int)(rectPicture.left + max_pos * deltX);
//
//						//CString strMark;
//						//CRect rectXMark;
//
//						//strMark.Format("%d\n", (int)(m_nXNegtiveMark + (max_pos + 0.5) * deltX));
//
//						//rectXMark.left = mark_x - RECT_XMARK_WIDTH / 2;
//						//rectXMark.right = mark_x + RECT_XMARK_WIDTH / 2;
//						//rectXMark.top = rectPicture.top + Y_SEPARATOR;
//						//rectXMark.bottom = rectXMark.top + RECT_XMARK_HEIGHT;
//
//						//CFont* pMarkFont = new CFont;
//						//pMarkFont->CreateFont(FONT_MARK_HEIGHT, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
//
//						//pMemDC->SelectObject(pMarkFont);
//						//pMemDC->SetTextColor(SCREEN_MAXLIMITCOLOR);
//						//pMemDC->DrawText(strMark, strMark.GetLength(), &rectXMark, DT_CENTER | DT_SINGLELINE | DT_TOP);
//
//						//strMark.Format("%d\n", m_nXNegtiveMark);
//
//						//delete pMarkFont;
//					}
//
//					pMemDC->SelectObject(pOldBrush);
//					delete pPaintBrush;
//				}
//
//#if INSTRUMENT_PANEL_USE_MUTEX
//				if (pChannel->hSampleAccess != NULL)
//				{
//					::SetEvent(pChannel->hSampleAccess);
//				}
//#endif
//			}
//		}
//	}
}

//注：这里varValue是相对于offset的标准方差
//ideaValue -- 期望接近真值的一个理想值，实际也是存在误差的估计值
//nSampleCurValue -- 样本当前测量值
//nSampleMeanValue -- 样本长期测量均值
//nSampleVarValue -- 样本长期最大偏离度
void CInstrumentView_Histogram::AppendSample(int ID, int sampleValue)
{
	int nNegtiveBias, nPositiveBias;

#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		//if (attr != NULL)
		{
			//m_nMeasuredXMeanValue = attr->mean;
			//m_nMeasuredXRmsValue = attr->rms;

			if (sampleValue < m_nMeasuredXMinValue)
			{
				m_nMeasuredXMinValue = sampleValue;
			}
			if (sampleValue > m_nMeasuredXMaxValue)
			{
				m_nMeasuredXMaxValue = sampleValue;
			}

			if (m_nXAxisStyle == AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY)
			{
				nNegtiveBias = m_nXNegtiveMark;
				if (m_nMeasuredXMinValue < m_nXNegtiveMark)
				{
					if (m_nMeasuredXMinValue > m_nXFloor)
					{
						nNegtiveBias = (int)(floor(m_nMeasuredXMinValue / (double)m_nXStep) * m_nXStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}

				nPositiveBias = m_nXPositiveMark;
				if (m_nMeasuredXMaxValue > m_nXPositiveMark)
				{
					if (m_nMeasuredXMaxValue < m_nXCeil)
					{
						nPositiveBias = (int)(ceil(m_nMeasuredXMaxValue / (double)m_nXStep) * m_nXStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}

				if (m_bNeedRedrawAllBmp)
				{
					int bias = max(abs(nNegtiveBias), abs(nPositiveBias));
					m_nXNegtiveMark = -bias;
					m_nXPositiveMark = bias;
				}
			}
			else if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY)
			{
				nNegtiveBias = m_nXNegtiveMark;
				if (m_nMeasuredXMinValue < m_nXNegtiveMark)
				{
					if (m_nMeasuredXMinValue > m_nXFloor)
					{
						nNegtiveBias = (int)(floor(m_nMeasuredXMinValue / (double)m_nXStep) * m_nXStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}

				nPositiveBias = m_nXPositiveMark;
				if (m_nMeasuredXMaxValue > m_nXPositiveMark)
				{
					if (m_nMeasuredXMaxValue < m_nXCeil)
					{
						nPositiveBias = (int)(ceil(m_nMeasuredXMaxValue / (double)m_nXStep) * m_nXStep);
						m_bNeedRedrawAllBmp = 1;
					}
				}

				if (m_bNeedRedrawAllBmp)
				{
					int bias = max(abs(nNegtiveBias), abs(nPositiveBias));
					m_nXNegtiveMark = -bias;
					m_nXPositiveMark = bias;
				}
			}
			else if (m_nXAxisStyle == AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX)
			{
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
			}
		}

		CInstrumentView_Base::AppendXSample(ID, sampleValue);

#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif
}
