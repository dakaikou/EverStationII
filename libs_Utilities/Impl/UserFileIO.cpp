#include <string.h>
#include <stdio.h>

#include "../Include/XStream_Utilities.h"

int	GetModulePathLength(char* pszFile)
{
	char*		 pdest;
	int			 idx = -1;

	//��ȡӦ�ó����·��
	if (pszFile != NULL)
	{
		pdest = strchr(pszFile, '\\');
		while (pdest != NULL)
		{
			idx = (int)(pdest - pszFile);
			pdest = strchr(pszFile + idx + 1, '\\');
		}
	}

	return idx;
}

//int BuildDirectory(CString  strPath)
//{
//	CString   strSubPath;
//	//	CString   strInfo;   
//	int   nCount = 0;
//	int   nIndex = 0;
//
//	//�����ַ�"\\"�ĸ���   
//	do
//	{
//		nIndex = strPath.Find("\\", nIndex) + 1;
//		nCount++;
//	} while ((nIndex - 1) != -1);
//
//	nIndex = 0;
//	//��飬������Ŀ¼   
//	while ((nCount - 1) >= 0)
//	{
//		nIndex = strPath.Find("\\", nIndex) + 1;
//		if ((nIndex - 1) == -1)
//		{
//			strSubPath = strPath;
//		}
//		else
//		{
//			strSubPath = strPath.Left(nIndex);
//		}
//
//		if (!PathFileExists(strSubPath))
//		{
//			if (!::CreateDirectory(strSubPath, NULL))
//			{
//				//				strInfo   =   "Build   Directory";   
//				//				strInfo   +=   strSubPath;   
//				//				strInfo   +=   "   Fail!";   
//				//				AfxMessageBox(strInfo, MB_OK);   
//				return -1;
//			}
//		}
//
//		nCount--;
//	};
//
//	return 0;
//}



