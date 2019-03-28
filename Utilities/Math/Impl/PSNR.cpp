#include "..\Include\PSNR.h"
#include <math.h>

double psnr(uint8_t* reference, uint8_t* working, int size)
{
	unsigned char*  pOrg = reference;
	unsigned char*  pRec = working;
	double          ssd = 0;
	int             diff;
	double			psnr = 99.99;

	for (int i = 0; i < size; i++)
	{
		diff = pRec[i] - pOrg[i];
		ssd += (double)(diff * diff);
	}

	if (ssd > 0)
	{
		psnr = (10.0 * log10(size * 65025.0 / ssd));			//255 * 255 = 65025
	}

	return psnr;
}

