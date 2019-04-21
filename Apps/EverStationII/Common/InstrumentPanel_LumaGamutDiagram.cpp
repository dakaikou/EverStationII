//

#include "stdafx.h"
#include <afxwin.h>
#include <assert.h>
#include "InstrumentPanel_LumaGamutDiagram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_LumaGamutDiagram
CInstrumentPanel_LumaGamutDiagram::CInstrumentPanel_LumaGamutDiagram()
{
}

CInstrumentPanel_LumaGamutDiagram::~CInstrumentPanel_LumaGamutDiagram()
{
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_LumaGamutDiagram, CInstrumentPanel_Base)
	//{{AFX_MSG_MAP(CInstrumentPanel_LumaGamutDiagram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstrumentPanel_LumaGamutDiagram::AppendSample(int colIndex, int colWidth, uint8_t srcY)
{
	POINT pixel;

	double x = (double)colIndex / colWidth;

	CRect rectPicture(0,0, m_rectWaveform.Width(), m_rectWaveform.Height());

	pixel.x = XMAP_Value2Pos((int)(1024 * x), rectPicture);
	pixel.y = YMAP_Value2Pos(srcY, rectPicture);

	m_pMemDC->SelectObject(m_pWaveformBmp);
	m_pMemDC->SetPixel(pixel, RGB(255, 255, 255));
}

