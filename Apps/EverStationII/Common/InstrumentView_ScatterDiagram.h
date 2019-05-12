#pragma once

#include "InstrumentView_Base.h"
// CInstrumentView_ScatterDiagram 视图

#define SCATTER_DIAGRAM_SAMPLE_DEPTH							1024 * 16
//#define SCATTER_DIAGRAM_SAMPLE_DEPTH							1024 * 20

class CInstrumentView_ScatterDiagram : public CInstrumentView_Base
{
	DECLARE_DYNCREATE(CInstrumentView_ScatterDiagram)

protected:
	CInstrumentView_ScatterDiagram();           // 动态创建所使用的受保护的构造函数
	virtual ~CInstrumentView_ScatterDiagram();

public:

	void AppendSample(int ID, int x, int y);
	void DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);
	void DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


