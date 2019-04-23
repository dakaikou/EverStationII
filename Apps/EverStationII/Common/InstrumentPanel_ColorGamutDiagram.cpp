//

#include "stdafx.h"
#include <afxwin.h>
#include <assert.h>
#include "InstrumentPanel_ColorGamutDiagram.h"

#include "Utilities/Graphics/Include/Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define clip3(x, y, z)  ((y < x) ? x : ((y > z) ? z : y))

#define COLORFUL_CIE_XY_COORDINATES				1
#define RGB_TO_CIE								1

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Histogram
CInstrumentPanel_ColorGamutDiagram::CInstrumentPanel_ColorGamutDiagram()
{
}

CInstrumentPanel_ColorGamutDiagram::~CInstrumentPanel_ColorGamutDiagram()
{
}


BEGIN_MESSAGE_MAP(CInstrumentPanel_ColorGamutDiagram, CInstrumentPanel_Kernel)
	//{{AFX_MSG_MAP(CInstrumentPanel_ColorGamutDiagram)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstrumentPanel_ColorGamutDiagram::AppendOneFrame(uint8_t* pucY, int luma_width, int luma_height,
	uint8_t* pucCb, uint8_t* pucCr, int chroma_width, int chroma_height, int nColorSpace)
{
	uint8_t R = 255, G = 255, B = 255;

	double cie1931_x = 0;
	double cie1931_y = 0;
	double cieX = 0;
	double cieY = 0;
	double cieZ = 0;

	POINT pixel;
	CRect rectPicture(0, 0, m_rectWaveform.Width(), m_rectWaveform.Height());

	m_pMemDC->SelectObject(m_pWaveformBmp);

	for (int row = 0; row < chroma_height; row++)
	{
		for (int col = 0; col < chroma_width; col++)
		{
			uint8_t srcY = pucY[(col << 1)];
			uint8_t srcCb = pucCb[col];
			uint8_t srcCr = pucCr[col];

			if (srcY > 0)
			{
				if ((srcY > 235) || (srcY < 16))
				{
					srcY = clip3(16, srcY, 235);
				}

				if ((srcCb > 240) || (srcCb < 16))
				{
					assert(0);
				}

				if ((srcCr > 240) || (srcCr < 16))
				{
					assert(0);
				}

				if (nColorSpace == 6010)				//NTSC 1953
				{
					//double y = (double)(srcY - 16) / 219;
					double y = (double)srcY / 255;
					double cb = (double)(srcCb - 128) / 224;
					double cr = (double)(srcCr - 128) / 224;

					//GRAPHICS_yuv2rgb(srcColorSpace, srcY, srcCb, srcCr, &R, &G, &B);
					double r = y + 1.402 * cr;
					double g = y - 0.344 * cb - 0.714 * cr;
					double b = y + 1.772 * cb;

					r = clip3(0, r, 1.0);
					g = clip3(0, g, 1.0);
					b = clip3(0, b, 1.0);

#if COLORFUL_CIE_XY_COORDINATES
					R = (uint8_t)(255 * r);
					G = (uint8_t)(255 * g);
					B = (uint8_t)(255 * b);
#endif

#if RGB_TO_CIE
					cieX = 0.6069 * r + 0.1735 * g + 0.2003 * b;
					cieY = 0.2989 * r + 0.5866 * g + 0.1144 * b;
					cieZ = 0.0000 * r + 0.0661 * g + 1.1157 * b;
#else
					//NTSC
					cieX = 0.9807 * y + 0.2952 * cb + 0.7270 * cr;
					cieY = y + 0.0010 * cb + 0.0003 * cr;
					cieZ = 1.1818 * y + 1.9544 * cb - 0.0472 * cr;
#endif
				}
				else if (nColorSpace == 6012)				//PAL
				{
					//double y = (double)(srcY - 16) / 219;
					double y = (double)srcY / 255;
					double cb = (double)(srcCb - 128) / 224;
					double cr = (double)(srcCr - 128) / 224;

					//GRAPHICS_yuv2rgb(srcColorSpace, srcY, srcCb, srcCr, &R, &G, &B);
					double r = y + 1.402 * cr;
					double g = y - 0.344 * cb - 0.714 * cr;
					double b = y + 1.772 * cb;

					r = clip3(0, r, 1.0);
					g = clip3(0, g, 1.0);
					b = clip3(0, b, 1.0);

#if COLORFUL_CIE_XY_COORDINATES
					R = (uint8_t)(255 * r);
					G = (uint8_t)(255 * g);
					B = (uint8_t)(255 * b);
#endif

#if RGB_TO_CIE
					cieX = 0.4306 * r + 0.3415 * g + 0.1784 * b;
					cieY = 0.2220 * r + 0.7067 * g + 0.0713 * b;
					cieZ = 0.0202 * r + 0.1296 * g + 0.9393 * b;
#else
					//PAL
					cieX = 0.9505 * y + 0.1985 * cb + 0.3598 * cr;
					cieY = y - 0.1167 * cb - 0.1933 * cr;
					cieZ = 1.0891 * y + 1.6199 * cb - 0.0642 * cr;
#endif
				}
				else if (nColorSpace == 709)				//ITU-R BT.709
				{
					double y = (double)(srcY - 16) / 219;
					//double y = (double)srcY / 255;
					double cb = (double)(srcCb - 128) / 224;
					double cr = (double)(srcCr - 128) / 224;

					//GRAPHICS_yuv2rgb(srcColorSpace, srcY, srcCb, srcCr, &R, &G, &B);
					double r = y + 1.5748 * cr;
					double g = y - 0.1874 * cb - 0.4681 * cr;
					double b = y + 1.8556 * cb;

					r = clip3(0, r, 1.0);
					g = clip3(0, g, 1.0);
					b = clip3(0, b, 1.0);

#if COLORFUL_CIE_XY_COORDINATES
					R = (uint8_t)(255 * r);
					G = (uint8_t)(255 * g);
					B = (uint8_t)(255 * b);
#endif

#if RGB_TO_CIE
					cieX = 0.4124 * r + 0.3576 * g + 0.1805 * b;
					cieY = 0.2126 * r + 0.7152 * g + 0.0722 * b;
					cieZ = 0.0193 * r + 0.1192 * g + 0.9505 * b;
#else
					//y = 1.0;
					cieX = 0.9505 * y + 0.2679 * cb + 0.4820 * cr;
					cieY = y - 0.0001 * cb - 0.0001 * cr;
					cieZ = 1.0891 * y + 1.7415 * cb - 0.0254 * cr;
#endif
				}
				else if (nColorSpace == 2020)				//ITU-R BT.2020
				{
					//double y = (double)(srcY - 16) / 219;
					double y = (double)srcY / 255;
					double cb = (double)(srcCb - 128) / 224;
					double cr = (double)(srcCr - 128) / 224;

					//GRAPHICS_yuv2rgb(srcColorSpace, srcY, srcCb, srcCr, &R, &G, &B);
					double r = y + 1.4746 * cr;
					double g = y - 0.1646 * cb - 0.5714 * cr;
					double b = y + 1.8814 * cb;

					r = clip3(0, r, 1.0);
					g = clip3(0, g, 1.0);
					b = clip3(0, b, 1.0);

#if COLORFUL_CIE_XY_COORDINATES
					R = (uint8_t)(255 * r);
					G = (uint8_t)(255 * g);
					B = (uint8_t)(255 * b);
#endif

#if RGB_TO_CIE
					cieX = 0.6370 * r + 0.1446 * g + 0.1689 * b;
					cieY = 0.2627 * r + 0.6780 * g + 0.0593 * b;
					cieZ = 0.0000 * r + 0.0281 * g + 1.0610 * b;
#else
					cieX = 0.9505 * y + 0.2939 * cb + 0.8566 * cr;
					cieY = y;
					cieZ = 1.0891 * y + 1.9915 * cb - 0.0160 * cr;
#endif
				}

				if ((cieX > 0) && (cieY > 0) && (cieZ > 0))
				{
					double X_Y_Z = cieX + cieY + cieZ;
					cie1931_x = cieX / X_Y_Z;
					cie1931_y = cieY / X_Y_Z;

					assert((cie1931_x > 0) && (cie1931_x < 1.0));
					assert((cie1931_y > 0) && (cie1931_y < 1.0));

					pixel.x = XMAP_Value2Pos((int)(1000 * cie1931_x), rectPicture);
					pixel.y = YMAP_Value2Pos((int)(1000 * cie1931_y), rectPicture);

					m_pMemDC->SetPixel(pixel, RGB(R, G, B));
				}
			}
		}

		pucCb += chroma_width;
		pucCr += chroma_width;
		pucY += (luma_width << 1);
	}
}

void CInstrumentPanel_ColorGamutDiagram::DisplayLegendInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectGridArea)
{
	if ((pMemDC != NULL) && (pBkBmp != NULL))
	{
		POINT ptRed, ptBlue, ptGreen, ptWhite;
		
		CPen* pColorSpace2020Pen = new CPen;
		pColorSpace2020Pen->CreatePen(PS_SOLID, 1, RGB(235, 235, 235));
		CPen* pColorSpace709Pen = new CPen;
		pColorSpace709Pen->CreatePen(PS_SOLID, 1, RGB(235, 235, 16));
		CPen* pColorSpace6010Pen = new CPen;
		pColorSpace6010Pen->CreatePen(PS_SOLID, 1, RGB(235, 16, 235));
		CPen* pColorSpace6012Pen = new CPen;
		pColorSpace6012Pen->CreatePen(PS_SOLID, 1, RGB(16, 235, 235));

		//4K UHD color space ITU-T BT.2020
		ptWhite.x = XMAP_Value2Pos(313, rectGridArea);
		ptWhite.y = YMAP_Value2Pos(329, rectGridArea);

		ptRed.x = XMAP_Value2Pos(708, rectGridArea);
		ptRed.y = YMAP_Value2Pos(292, rectGridArea);

		ptGreen.x = XMAP_Value2Pos(170, rectGridArea);
		ptGreen.y = YMAP_Value2Pos(797, rectGridArea);

		ptBlue.x = XMAP_Value2Pos(131, rectGridArea);
		ptBlue.y = YMAP_Value2Pos(46, rectGridArea);

		pMemDC->SelectObject(pColorSpace2020Pen);
		pMemDC->MoveTo(ptRed);
		pMemDC->LineTo(ptGreen);
		pMemDC->LineTo(ptBlue);
		pMemDC->LineTo(ptRed);
		pMemDC->SetPixel(ptWhite, RGB(255, 255, 255));
		pMemDC->SetPixel(ptRed, RGB(255, 0, 0));
		pMemDC->SetPixel(ptGreen, RGB(0, 255, 0));
		pMemDC->SetPixel(ptBlue, RGB(0, 0, 255));

		pMemDC->MoveTo(500, 100);
		pMemDC->LineTo(550, 100);
		pMemDC->TextOutA(560, 92, "UHD 2020");

		//HD color space ITU-T BT.709
		ptRed.x = XMAP_Value2Pos(640, rectGridArea);
		ptRed.y = YMAP_Value2Pos(330, rectGridArea);

		ptGreen.x = XMAP_Value2Pos(300, rectGridArea);
		ptGreen.y = YMAP_Value2Pos(600, rectGridArea);

		ptBlue.x = XMAP_Value2Pos(150, rectGridArea);
		ptBlue.y = YMAP_Value2Pos(60, rectGridArea);

		pMemDC->SelectObject(pColorSpace709Pen);
		pMemDC->MoveTo(ptRed);
		pMemDC->LineTo(ptGreen);
		pMemDC->LineTo(ptBlue);
		pMemDC->LineTo(ptRed);

		pMemDC->SetPixel(ptWhite, RGB(255, 255, 255));
		pMemDC->SetPixel(ptRed, RGB(255, 0, 0));
		pMemDC->SetPixel(ptGreen, RGB(0, 255, 0));
		pMemDC->SetPixel(ptBlue, RGB(0, 0, 255));

		pMemDC->MoveTo(500, 116);
		pMemDC->LineTo(550, 116);
		pMemDC->TextOutA(560, 108, "HD 709");

		//SD color space ITU-T BT.601 PAL
		ptRed.x = XMAP_Value2Pos(640, rectGridArea);
		ptRed.y = YMAP_Value2Pos(330, rectGridArea);

		ptGreen.x = XMAP_Value2Pos(290, rectGridArea);
		ptGreen.y = YMAP_Value2Pos(600, rectGridArea);

		ptBlue.x = XMAP_Value2Pos(150, rectGridArea);
		ptBlue.y = YMAP_Value2Pos(60, rectGridArea);

		pMemDC->SelectObject(pColorSpace6012Pen);
		pMemDC->MoveTo(ptRed);
		pMemDC->LineTo(ptGreen);
		pMemDC->LineTo(ptBlue);
		pMemDC->LineTo(ptRed);

		pMemDC->SetPixel(ptWhite, RGB(255, 255, 255));
		pMemDC->SetPixel(ptRed, RGB(255, 0, 0));
		pMemDC->SetPixel(ptGreen, RGB(0, 255, 0));
		pMemDC->SetPixel(ptBlue, RGB(0, 0, 255));

		pMemDC->MoveTo(500, 132);
		pMemDC->LineTo(550, 132);
		pMemDC->TextOutA(560, 124, "SD PAL");

		//SD color space ITU-T BT.601 NTSC
		ptRed.x = XMAP_Value2Pos(670, rectGridArea);
		ptRed.y = YMAP_Value2Pos(330, rectGridArea);

		ptGreen.x = XMAP_Value2Pos(210, rectGridArea);
		ptGreen.y = YMAP_Value2Pos(710, rectGridArea);

		ptBlue.x = XMAP_Value2Pos(140, rectGridArea);
		ptBlue.y = YMAP_Value2Pos(80, rectGridArea);

		pMemDC->SelectObject(pColorSpace6010Pen);
		pMemDC->MoveTo(ptRed);
		pMemDC->LineTo(ptGreen);
		pMemDC->LineTo(ptBlue);
		pMemDC->LineTo(ptRed);

		pMemDC->SetPixel(ptWhite, RGB(255, 255, 255));
		pMemDC->SetPixel(ptRed, RGB(255, 0, 0));
		pMemDC->SetPixel(ptGreen, RGB(0, 255, 0));
		pMemDC->SetPixel(ptBlue, RGB(0, 0, 255));

		pMemDC->MoveTo(500, 148);
		pMemDC->LineTo(550, 148);
		pMemDC->TextOutA(560, 140, "SD NTSC");

		delete pColorSpace2020Pen;
		delete pColorSpace709Pen;
		delete pColorSpace6010Pen;
		delete pColorSpace6012Pen;
	}
}
