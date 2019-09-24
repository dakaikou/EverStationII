#pragma once

#include "InstrumentView_View.h"
// CInstrumentView_Base 视图

#define SCREEN_WAVECOLOR0		RGB(250, 250, 0)
#define SCREEN_WAVECOLOR1		RGB(50, 250, 50)
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

#ifndef SAMPLE_ATTRIBUTE
#define SAMPLE_ATTRIBUTE
typedef struct
{
	int min, mean, max, rms;
} SAMPLE_ATTRIBUTE_t;
#endif		//SAMPLE_ATTRIBUTE

#ifndef SAMPLE_CHANNEL
#define SAMPLE_CHANNEL
typedef struct
{
	int* pnXArray;
	int* pnYArray;

	int nSampleCount;
	int nWrIndex;
	int nRdIndex;
	int bFull;
	int bEmpty;
	int ID;
	DWORD color;

#if INSTRUMENT_PANEL_USE_MUTEX
	HANDLE	hSampleAccess;
#endif

} SAMPLE_CHANNEL_t;
#endif		//SAMPLE_CHANNEL

#define MAX_CHANNEL_COUNT		12
#define DEFAULT_CHANNEL_DEPTH	256

class CInstrumentView_Base : public CInstrumentView_View
{
	DECLARE_DYNCREATE(CInstrumentView_Base)

protected:
	CInstrumentView_Base();           // 动态创建所使用的受保护的构造函数
	virtual ~CInstrumentView_Base();

	SAMPLE_CHANNEL_t* m_pChannel[MAX_CHANNEL_COUNT];
	int				  m_nChannleCount;
	int				  m_nChannleDepth;

	COLORREF  m_Palette[MAX_CHANNEL_COUNT];
public:
	void AppendXSample(int chID, int x);
	void AppendYSample(int chID, int y);
	void AppendXYSample(int chID, int x, int y);

	void Reset(void);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


