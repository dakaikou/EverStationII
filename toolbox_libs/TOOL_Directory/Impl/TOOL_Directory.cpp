#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h> //_mkdir函数的头文件
#include <io.h>     //_access函数的头文件

#include "../Include/TOOL_Directory.h"

int	DIR_GetModulePathLength(char* pszFile)
{
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
	}

	return idx;
}

int DIR_BuildDirectory(char* pszFilePath)
{
	//char	pszExeFile[MAX_PATH];
	char	pszDrive[3];
	char	pszDir[256];
	char	pszFile[256];
	char	pszExt[8];
	int		idx = 0;
	char	pszTemp[256];
	char	pszLeftDir[256];

	_splitpath_s(pszFilePath, pszDrive, pszDir, pszFile, pszExt);
	idx = strlen(pszDrive);

	//查找字符"\\"的个数
	if (strlen(pszDir) > 0)
	{
		char* pszTempDir = pszDir + 1;
		idx += 1;

		char* pszSubDir = NULL;
		while (strlen(pszTempDir) > 0)
		{
			pszSubDir = strchr(pszTempDir, '\\');
			idx += (int)(pszSubDir - pszTempDir);
			pszTempDir = pszSubDir + 1;
			idx += 1;

			memcpy(pszLeftDir, pszFilePath, idx);
			pszLeftDir[idx] = '\0';
			//sprintf_s(pszTemp, sizeof(pszTemp), "%s%s", pszLeftDir, pszFile);
			_mkdir(pszLeftDir);
		}

		if (strlen(pszFile) > 0)
		{
			if (strlen(pszExt) == 0)
			{
				memcpy(pszLeftDir, pszFilePath, idx);
				pszLeftDir[idx] = '\0';
				sprintf_s(pszTemp, sizeof(pszTemp), "%s%s", pszLeftDir, pszFile);
				_mkdir(pszTemp);
			}
		}
	}

	return 0;
}


//void CreateDir(const char *dir)
//{
//	int m = 0, n;
//	string str1, str2;
//
//	str1 = dir;
//	str2 = str1.substr(0, 2);
//	str1 = str1.substr(3, str1.size());
//
//	while (m >= 0)
//	{
//		m = str1.find('\\');
//
//		str2 += '\\' + str1.substr(0, m);
//		n = _access(str2.c_str(), 0); //判断该目录是否存在
//		if (n == -1)
//		{
//			_mkdir(str2.c_str());     //创建目录
//		}
//
//		str1 = str1.substr(m + 1, str1.size());
//	}
//}


