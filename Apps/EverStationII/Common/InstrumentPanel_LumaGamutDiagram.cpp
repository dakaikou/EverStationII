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


BEGIN_MESSAGE_MAP(CInstrumentPanel_LumaGamutDiagram, CInstrumentPanel_Kernel)
	//{{AFX_MSG_MAP(CInstrumentPanel_LumaGamutDiagram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstrumentPanel_LumaGamutDiagram::AppendOneFrame(uint8_t* pucY, int luma_width, int luma_height)
{
	POINT pixel;
	CRect rectPicture(0, 0, m_rectWaveform.Width(), m_rectWaveform.Height());

	m_pMemDC->SelectObject(m_pWaveformBmp);

	for (int row = 0; row < luma_height; row++)
	{
		for (int col = 0; col < luma_width; col++)
		{
			double x = (double)col / luma_width;

			pixel.x = XMAP_Value2Pos((int)(1024 * x), rectPicture);
			pixel.y = YMAP_Value2Pos(pucY[col], rectPicture);

			m_pMemDC->SetPixel(pixel, RGB(255, 255, 255));
		}

		pucY += luma_width;
	}
}
