#pragma once

#include "InstrumentView_Base.h"
// CInstrumentView_Histogram 视图

#define HISTGRAM_SAMPLE_DEPTH							1024 * 20
#define HISTGRAM_HORZ_DIVISION							100								//根据被测对象的分辨率确定

class CInstrumentView_Histogram : public CInstrumentView_Base
{
	DECLARE_DYNCREATE(CInstrumentView_Histogram)

protected:
	CInstrumentView_Histogram();           // 动态创建所使用的受保护的构造函数
	virtual ~CInstrumentView_Histogram();

public:
	void DisplayTheWholeSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp);
	void DisplayTheNewSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp);
	void AppendSample(int ID, int sampleValue);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


