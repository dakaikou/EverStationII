#include <string.h>
#include <stdio.h>

#include "../Include/XStream_Utilities.h"

int	GetModulePathLength(char* pszFile)
{
//	char		 pszFile[MAX_PATH];
//	char		 pszPath[MAX_PATH];
	char*		 pdest;
	int			 idx = -1;

	//获取应用程序的路径
	if (pszFile != NULL)
	{
		pdest = strchr(pszFile, '\\');
		while (pdest != NULL)
		{
			idx = (int)(pdest - pszFile);
			pdest = strchr(pszFile + idx + 1, '\\');
		}

//		if (idx > 0)
//		{
//			memcpy(pszPath, pszFile, idx);
//			pszPath[idx] = '\0';
//		}
	}

	return idx;
}


