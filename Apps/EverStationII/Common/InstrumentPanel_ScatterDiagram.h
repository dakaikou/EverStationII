#if !defined(AFX_TIME_SCATTER_DIAGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
#define AFX_TIME_SCATTER_DIAGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include "InstrumentPanel_Base.h"

#define HISTGRAM_SAMPLE_DEPTH							1024 * 20
#define HISTGRAM_HORZ_DIVISION							101								//根据被测对象的分辨率确定

/////////////////////////////////////////////////////////////////////////////
// CGraph_Oscilloscope window
class CInstrumentPanel_ScatterDiagram : public CInstrumentPanel_Base
{
// Construction
public:
	CInstrumentPanel_ScatterDiagram();

// Attributes
public:

// Operations
public:

	void AppendSample(int ID, int xsampleValue, int ysampleValue, SAMPLE_ATTRIBUTE_t* xattr=NULL, SAMPLE_ATTRIBUTE_t* yattr=NULL);
	void DisplayMeasureGraph(CDC* pDC, CBitmap* pGraphBmp);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentPanel_Histogram)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstrumentPanel_ScatterDiagram();

// Implementation
protected:
	//{{AFX_MSG(CInstrumentPanel_ScatterDiagram)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIME_SCATTER_DIAGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
