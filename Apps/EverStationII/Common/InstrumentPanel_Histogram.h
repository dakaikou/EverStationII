#if !defined(AFX_TIME_HISTOGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
#define AFX_TIME_HISTOGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include "InstrumentPanel_Base.h"

#define HISTGRAM_SAMPLE_DEPTH							1024 * 20
#define HISTGRAM_HORZ_DIVISION							100								//根据被测对象的分辨率确定

/////////////////////////////////////////////////////////////////////////////
// CGraph_Oscilloscope window
class CInstrumentPanel_Histogram : public CInstrumentPanel_Base
{
// Construction
public:
	CInstrumentPanel_Histogram();

// Attributes
public:

// Operations
public:
	void DisplayTheWholeSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp);
	void DisplayTheNewSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp);
	void AppendSample(int ID, int sampleValue, SAMPLE_ATTRIBUTE_t* attr = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentPanel_Histogram)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstrumentPanel_Histogram();

// Implementation
protected:
	//{{AFX_MSG(CGraph_Oscilloscope)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIME_HISTOGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
