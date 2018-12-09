#if !defined(AFX_TIME_SCATTER_DIAGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
#define AFX_TIME_SCATTER_DIAGRAM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include "InstrumentPanel_Base.h"

#define SCATTER_DIAGRAM_SAMPLE_DEPTH							1024 * 16
//#define SCATTER_DIAGRAM_SAMPLE_DEPTH							1024 * 20

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
	void DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);
	void DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);

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
