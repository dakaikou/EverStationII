#if !defined(AFX_COLOR_GAMUT_SCATTER_DIAGRAM_GRAPH_H__1BB7E10F_5558_4C78_82B2_86C7653E8081__INCLUDED_)
#define AFX_COLOR_GAMUT_SCATTER_DIAGRAM_GRAPH_H__1BB7E10F_5558_4C78_82B2_86C7653E8081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include <windows.h>
#include <stdint.h>
#include "InstrumentPanel_Base.h"

/////////////////////////////////////////////////////////////////////////////
class CInstrumentPanel_ColorGamutDiagram : public CInstrumentPanel_Base
{
// Construction
public:
	CInstrumentPanel_ColorGamutDiagram();

// Attributes
public:

// Operations
public:
	void AppendOneFrame(uint8_t* pucY, int luma_width, int luma_height,
		uint8_t* pucCb, uint8_t* pucCr, int chroma_width, int chroma_height, int nColorSpace);

	void DisplayLegendInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectWaveform);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentPanel_ColorSpaceDiagram)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstrumentPanel_ColorGamutDiagram();

// Implementation
protected:
	//{{AFX_MSG(CInstrumentPanel_ColorGamutDiagram)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOR_GAMUT_SCATTER_DIAGRAM_GRAPH_H__1BB7E10F_5558_4C78_82B2_86C7653E8081__INCLUDED_)
