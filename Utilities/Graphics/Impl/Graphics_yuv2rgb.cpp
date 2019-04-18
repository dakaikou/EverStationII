#include <algorithm>

#include "..\..\Include\Graphics.h"

using namespace std;

#define clip3(x, y, z)  ((y < x) ? x : ((y > z) ? z : y))

int GRAPHICS_yuv2rgb(int colorSpace, uint8_t YD, uint8_t UD, uint8_t VD, uint8_t* pRD, uint8_t* pGD, uint8_t* pBD, int bFullScale)
{
	if (bFullScale)			//Y [16~235]  RGB [0~255]
	{
		if (colorSpace == 6010)				//NTSC
		{
			int Y = YD - 16;			//   0~219, 220 levels
			int Cb = UD - 128;			//-112~112, 225 levels
			int Cr = VD - 128;			//-112~112, 225 levels
#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(1.164*Y + 1.596 * Cr + 0.5);
			int G = (int)(1.164*Y - 0.392 * Cb - 0.813 * Cr + 0.5);
			int B = (int)(1.164*Y + 2.017 * Cb + 0.5);
#else
			int R = ((298 * Y + 409 * Cr + 128) >> 8);
			int G = ((298 * Y - 100 * Cb - 208 * Cr + 128) >> 8);
			int B = ((298 * Y + 516 * Cb + 128) >> 8);
#endif

			*pRD = (uint8_t)clip3(0, R, 255);
			*pGD = (uint8_t)clip3(0, G, 255);
			*pBD = (uint8_t)clip3(0, B, 255);
		}
		else if (colorSpace == 6012)				//PAL
		{
			int Y = YD - 16;			//   0~219, 220 levels
			int Cb = UD - 128;			//-112~112, 225 levels
			int Cr = VD - 128;			//-112~112, 225 levels
#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(1.164 * Y + 1.596 * Cr + 0.5);
			int G = (int)(1.164 * Y - 0.392 * Cb - 0.813 * Cr + 0.5);
			int B = (int)(1.164 * Y + 2.017 * Cb + 0.5);
#else
			int R = ((298 * Y + 409 * Cr + 128) >> 8);
			int G = ((298 * Y - 100 * Cb - 208 * Cr + 128) >> 8);
			int B = ((298 * Y + 516 * Cb + 128) >> 8);
#endif

			* pRD = (uint8_t)clip3(0, R, 255);
			*pGD = (uint8_t)clip3(0, G, 255);
			*pBD = (uint8_t)clip3(0, B, 255);
		}
		else if (colorSpace == 709)			//ITU-R BT.709
		{
			int Y = YD - 16;
			int Cb = UD - 128;
			int Cr = VD - 128;
#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(1.1644*Y + 1.7928 * Cr + 0.5);
			int G = (int)(1.1644*Y - 0.2133 * Cb - 0.5329 * Cr + 0.5);
			int B = (int)(1.1644*Y + 2.1124 * Cb + 0.5);
#else
			int R = ((298 * Y + 459 * Cr + 128) >> 8);
			int G = ((298 * Y - 55 * Cb - 136 * Cr + 128) >> 8);
			int B = ((298 * Y + 541 * Cb + 128) >> 8);
#endif

			*pRD = (uint8_t)clip3(0, R, 255);
			*pGD = (uint8_t)clip3(0, G, 255);
			*pBD = (uint8_t)clip3(0, B, 255);
		}
		else if (colorSpace == 2020)			//ITU-R BT.2020
		{
			int Y = YD - 16;
			int Cb = UD - 128;
			int Cr = VD - 128;

#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(1.1644*Y + 1.6787 * Cr + 0.5);
			int G = (int)(1.1644*Y - 0.1874 * Cb - 0.6505 * Cr + 0.5);
			int B = (int)(1.1644*Y + 2.1418 * Cb + 0.5);
#else
			int R = ((298 * Y + 430 * Cr + 128) >> 8);
			int G = ((298 * Y - 48 * Cb - 167 * Cr + 128) >> 8);
			int B = ((298 * Y + 548 * Cb + 128) >> 8);
#endif
			*pRD = (uint8_t)clip3(16, R, 235);
			*pGD = (uint8_t)clip3(16, G, 235);
			*pBD = (uint8_t)clip3(16, B, 235);
		}
	}
	else
	{
		if (colorSpace == 6010)					//NTSC
		{
			int Y = YD;
			int Cb = UD - 128;
			int Cr = VD - 128;
#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(Y + 1.371 * Cr + 0.5);
			int G = (int)(Y - 0.336 * Cb - 0.698 * Cr + 0.5);
			int B = (int)(Y + 1.732 * Cb + 0.5);
#else
			int R = Y + ((351 * Cr + 128) >> 8);
			int G = Y + ((-86 * Cb - 179 * Cr + 128) >> 8);
			int B = Y + ((443 * Cb + 128) >> 8);
#endif

			*pRD = (uint8_t)clip3(16, R, 235);
			*pGD = (uint8_t)clip3(16, G, 235);
			*pBD = (uint8_t)clip3(16, B, 235);
		}
		else if (colorSpace == 6012)			//PAL
		{
			int Y = YD;
			int Cb = UD - 128;
			int Cr = VD - 128;
#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(Y + 1.371 * Cr + 0.5);
			int G = (int)(Y - 0.336 * Cb - 0.698 * Cr + 0.5);
			int B = (int)(Y + 1.732 * Cb + 0.5);
#else
			int R = Y + ((351 * Cr + 128) >> 8);
			int G = Y + ((-86 * Cb - 179 * Cr + 128) >> 8);
			int B = Y + ((443 * Cb + 128) >> 8);
#endif

			* pRD = (uint8_t)clip3(16, R, 235);
			*pGD = (uint8_t)clip3(16, G, 235);
			*pBD = (uint8_t)clip3(16, B, 235);
		}
		else if (colorSpace == 709)				//ITU-R BT.709
		{
			int Y = YD;
			int Cb = UD - 128;
			int Cr = VD - 128;
#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(Y + 1.5397 * Cr + 0.5);
			int G = (int)(Y - 0.1832 * Cb - 0.4577 * Cr + 0.5);
			int B = (int)(Y + 1.8142 * Cb + 0.5);
#else
			int R = Y + ((394 * Cr + 128) >> 8);
			int G = Y + ((-47 * Cb - 117 * Cr + 128) >> 8);
			int B = Y + ((464 * Cb + 128) >> 8);
#endif

			*pRD = (uint8_t)clip3(16, R, 235);
			*pGD = (uint8_t)clip3(16, G, 235);
			*pBD = (uint8_t)clip3(16, B, 235);
		}
		else if (colorSpace == 2020)				//ITU-R BT.2020
		{
			int Y = YD;
			int Cb = UD - 128;
			int Cr = VD - 128;

#if YUV2RGB_FLOATING_POINT_CALCULATION
			int R = (int)(Y + 1.4417 * Cr + 0.5);
			int G = (int)(Y - 0.1609 * Cb - 0.5587 * Cr + 0.5);
			int B = (int)(Y + 1.8394 * Cb + 0.5);
#else
			int R = Y + ((369 * Cr + 128) >> 8);
			int G = Y + ((-41 * Cb - 143 * Cr + 128) >> 8);
			int B = Y + ((471 * Cb + 128) >> 8);
#endif
			*pRD = (uint8_t)clip3(16, R, 235);
			*pGD = (uint8_t)clip3(16, G, 235);
			*pBD = (uint8_t)clip3(16, B, 235);
		}
	}

	return 0;
}

