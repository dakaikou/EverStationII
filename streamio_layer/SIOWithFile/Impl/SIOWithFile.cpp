#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#include "../Include/SIOWithFile.h"

static int	g_hFile = -1;
static int64_t g_llTotalFileLength = -1;

int file_receive_init(char* pszFile)
{
	int rtcode = TSFILE_UNKNOWN_ERROR;

	errno_t errcode = _sopen_s(&g_hFile, pszFile, _O_BINARY | _O_RDONLY, _SH_DENYWR, 0);

	if (g_hFile != -1)
	{
		_lseeki64(g_hFile, 0, SEEK_END);
		g_llTotalFileLength = _telli64(g_hFile);

		_lseeki64(g_hFile, 0, SEEK_SET);

		rtcode = TSFILE_NO_ERROR;
	}
	else
	{
		rtcode = TSFILE_OPEN_ERROR;						//打不开文件
	}

	return rtcode;
}

int file_start_receive(int64_t offset)
{
	int rtcode = TSFILE_UNKNOWN_ERROR;

	if (g_hFile != -1)
	{
		if (offset >= 0)
		{
			_lseeki64(g_hFile, offset, SEEK_SET);
		}

		rtcode = TSFILE_NO_ERROR;
	}

	return rtcode;
}

int file_receive(unsigned char *buffer, int count)
{
	int rdsize = _read(g_hFile, buffer, count);

	return rdsize;
}

int file_stop_receive(void)
{
	int rtcode = -1;

	return rtcode;
}

int file_receive_fini(void)
{
	int rtcode = TSFILE_UNKNOWN_ERROR;

	if (g_hFile != -1)
	{
		if (_close(g_hFile) == 0)
		{
			rtcode = TSFILE_NO_ERROR;
		}
		else
		{
			rtcode = TSFILE_CLOSE_ERROR;								//无法关闭文件
		}
		g_hFile = -1;
	}

	g_llTotalFileLength = -1;

	return rtcode;
}

int64_t file_attr_get_total_length(void)
{
	return g_llTotalFileLength;
}

int64_t file_attr_tell(void)
{
	int64_t llCurFileLength = -1;

	if (g_hFile != -1)
	{
		llCurFileLength = _telli64(g_hFile);
	}

	return llCurFileLength;
}

int file_seek(int64_t offset)
{
	int rtcode = TSFILE_UNKNOWN_ERROR;

	if (g_hFile != -1)
	{
		_lseeki64(g_hFile, offset, SEEK_SET);

		rtcode = TSFILE_NO_ERROR;
	}

	return rtcode;
}
