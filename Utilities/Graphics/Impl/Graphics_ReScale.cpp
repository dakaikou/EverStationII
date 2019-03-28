#include <algorithm>

#include "..\..\Include\Graphics.h"

using namespace std;

int GRAPHICS_PICTURE_Enlarge(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int decimate_coeff)
{
	int rtcode = 0;

	if ((src != NULL) && (dst != NULL))
	{
		uint8_t* pSrcRowStart = src;
		uint8_t* pDstRowStart = dst;
		for (int src_row = 0; src_row < src_h; src_row++)
		{
			for (int j = 0; j < decimate_coeff; j++)
			{
				int dst_col_base = 0;
				for (int src_col = 0; src_col < src_w; src_col++)
				{
					for (int i = 0; i < decimate_coeff; i++)
					{
						pDstRowStart[dst_col_base + i] = pSrcRowStart[src_col];
					}
					dst_col_base += decimate_coeff;
				}

				pDstRowStart += dst_w;
			}

			pSrcRowStart += src_w;
		}
	}

	return rtcode;
}

int GRAPHICS_PICTURE_Reduce(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int decimate_coeff)
{
	int rtcode = 0;

	if ((src != NULL) && (dst != NULL))
	{
		uint8_t* pSrcRowStart = src;
		uint8_t* pDstRowStart = dst;
		for (int src_row = 0; src_row < src_h; src_row += decimate_coeff)
		{
			int dst_col = 0;
			for (int src_col = 0; src_col < src_w; src_col += decimate_coeff)
			{
				pDstRowStart[dst_col] = pSrcRowStart[src_col];
				dst_col++;
			}

			pDstRowStart += dst_w;
			pSrcRowStart += (src_w * decimate_coeff);
		}
	}

	return rtcode;
}
