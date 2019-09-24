// InstrumentView_Kernel.cpp: 实现文件
//
#include "stdafx.h"
#include "../EverStationII.h"
#include "InstrumentView_Base.h"


// CInstrumentView_Base

IMPLEMENT_DYNCREATE(CInstrumentView_Base, CInstrumentView_View)

CInstrumentView_Base::CInstrumentView_Base()
{
	for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
	{
		m_pChannel[i] = NULL;
	}
	m_nChannleCount = 0;
	m_nChannleDepth = -1;		//默认值

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

CInstrumentView_Base::~CInstrumentView_Base()
{
}

BEGIN_MESSAGE_MAP(CInstrumentView_Base, CInstrumentView_View)
END_MESSAGE_MAP()


// CInstrumentView_Base 绘图



// CInstrumentView_Base 诊断

#ifdef _DEBUG
void CInstrumentView_Base::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CInstrumentView_Base::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInstrumentView_Base 消息处理程序
void CInstrumentView_Base::Reset(void)
{
#if ON_PAINTING_USE_MUTEX
	DWORD wait_state = ::WaitForSingleObject(m_hPaintingAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
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
			if (m_pChannel[i]->pnXArray != NULL)
			{
				delete m_pChannel[i]->pnXArray;
				m_pChannel[i]->pnXArray = NULL;
			}
			if (m_pChannel[i]->pnYArray != NULL)
			{
				delete m_pChannel[i]->pnYArray;
				m_pChannel[i]->pnYArray = NULL;
			}

			delete m_pChannel[i];
			m_pChannel[i] = NULL;
		}
		m_nChannleCount = 0;
#if ON_PAINTING_USE_MUTEX
		::ReleaseMutex(m_hPaintingAccess);
	}
#endif

	CInstrumentView_View::Reset();
}

void CInstrumentView_Base::AppendXSample(int ID, int x)
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
			pChannel->pnXArray = new int[m_nChannleDepth];
			memset(pChannel->pnXArray, 0, sizeof(int) * m_nChannleDepth);
			pChannel->pnYArray = NULL;
			pChannel->nSampleCount = 0;
			pChannel->nWrIndex = 0;
			pChannel->nRdIndex = 0;
			pChannel->bEmpty = 1;
			pChannel->bFull = 0;

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

		pChannel->pnXArray[pChannel->nWrIndex] = x;

		pChannel->nWrIndex++;
		pChannel->nWrIndex %= m_nChannleDepth;

		if (pChannel->bEmpty == 1)pChannel->bEmpty = 0;
		if (pChannel->nWrIndex == pChannel->nRdIndex)
		{
			pChannel->bFull = 1;
		}

		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		//pChannel->bNeedRedrawing = 1;

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

void CInstrumentView_Base::AppendYSample(int ID, int y)
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
			pChannel->pnXArray = NULL;
			pChannel->pnYArray = new int[m_nChannleDepth];
			memset(pChannel->pnYArray, 0, sizeof(int) * m_nChannleDepth);
 
			pChannel->nSampleCount = 0;
			pChannel->nWrIndex = 0;
			pChannel->nRdIndex = 0;
			pChannel->bEmpty = 1;
			pChannel->bFull = 0;

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

		pChannel->pnYArray[pChannel->nWrIndex] = y;

		pChannel->nWrIndex++;
		pChannel->nWrIndex %= m_nChannleDepth;
		if (pChannel->bEmpty == 1)pChannel->bEmpty = 0;
		if (pChannel->nWrIndex == pChannel->nRdIndex)
		{
			pChannel->bFull = 1;
		}

		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		if (pChannel->nSampleCount < m_nChannleDepth)
		{
			pChannel->nSampleCount++;
		}

		//pChannel->bNeedRedrawing = 1;

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

void CInstrumentView_Base::AppendXYSample(int ID, int x, int y)
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

			pChannel->pnXArray = new int[m_nChannleDepth];
			memset(pChannel->pnXArray, 0, sizeof(int) * m_nChannleDepth);

			pChannel->pnYArray = new int[m_nChannleDepth];
			memset(pChannel->pnYArray, 0, sizeof(int) * m_nChannleDepth);

			pChannel->nSampleCount = 0;
			pChannel->nWrIndex = 0;
			pChannel->nRdIndex = 0;
			pChannel->bFull = 0;
			pChannel->bEmpty = 1;

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

		if (pChannel->bFull == 0)
		{
			pChannel->pnXArray[pChannel->nWrIndex] = x;
			pChannel->pnYArray[pChannel->nWrIndex] = y;

			pChannel->nWrIndex++;
			pChannel->nWrIndex %= m_nChannleDepth;

			if (pChannel->bEmpty == 1)pChannel->bEmpty = 0;
			if (pChannel->nWrIndex == pChannel->nRdIndex)
			{
				pChannel->bFull = 1;
			}

			if (pChannel->nSampleCount < m_nChannleDepth)
			{
				pChannel->nSampleCount++;
			}

			//pChannel->bNeedRedrawing = 1;

			m_bNeedRedrawWaveformBmp = 1;
			m_bNeedRedrawMeasurePanelBmp = 1;
		}
		else
		{
			//assert(0);
		}

#if INSTRUMENT_PANEL_USE_MUTEX
		if (pChannel->hSampleAccess != NULL)
		{
			::SetEvent(pChannel->hSampleAccess);
		}
#endif
	}
}
