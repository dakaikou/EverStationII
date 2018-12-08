#if !defined(AFX_TIME_WAVEFORM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
#define AFX_TIME_WAVEFORM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include "InstrumentPanel_Base.h"

#define WAVEFORM_SAMPLE_DEPTH							1024
#define WAVEFORM_SAMPLE_COUNT							1024

/////////////////////////////////////////////////////////////////////////////
// CGraph_Oscilloscope window
class CInstrumentPanel_Waveform : public CInstrumentPanel_Base
{
// Construction
public:
	CInstrumentPanel_Waveform();

// Attributes
public:

// Operations
public:
	void AppendSample(int ID, int sampleValue, SAMPLE_ATTRIBUTE_t* attr = NULL);
	void DisplayTheNewSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp);
	void DisplayTheWholeSamplesInMemory(CDC* pDC, CBitmap* pGraphBmp);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraph_Histogram)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstrumentPanel_Waveform();

// Implementation
protected:
protected:
	//{{AFX_MSG(CGraph_Oscilloscope)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIME_WAVEFORM_GRAPH_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
