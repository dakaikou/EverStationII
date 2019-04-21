#if !defined(AFX_LUMA_GAMUT_SCATTER_DIAGRAM_GRAPH_H__1BB7E10F_5558_4C78_82B2_86C7653E8081__INCLUDED_)
#define AFX_LUMA_GAMUT_SCATTER_DIAGRAM_GRAPH_H__1BB7E10F_5558_4C78_82B2_86C7653E8081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include <windows.h>
#include <stdint.h>
#include "InstrumentPanel_Base.h"

/////////////////////////////////////////////////////////////////////////////
class CInstrumentPanel_LumaGamutDiagram : public CInstrumentPanel_Base
{
// Construction
public:
	CInstrumentPanel_LumaGamutDiagram();

// Attributes
public:

// Operations
public:

	void AppendOneFrame(uint8_t* pucY, int luma_width, int luma_height);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentPanel_LumaGamutDiagram)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstrumentPanel_LumaGamutDiagram();

// Implementation
protected:
	//{{AFX_MSG(CInstrumentPanel_LumaGamutDiagram)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUMA_GAMUT_SCATTER_DIAGRAM_GRAPH_H__1BB7E10F_5558_4C78_82B2_86C7653E8081__INCLUDED_)
