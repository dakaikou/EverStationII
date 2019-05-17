#ifndef __API_ES_DECODER_H__
#define __API_ES_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_ES_EXPORT
#       define MW_ES_LIB __declspec(dllexport)
#   elif defined(_MW_ES_IMPORT)
#       define MW_ES_LIB __declspec(dllimport)
#   else
#       define MW_ES_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_ES_LIB __attribute__((visibility("default")))
#else
#   define MW_ES_LIB
#endif

#include <stdint.h>

#include <windows.h>


#define DS_NOTIFY_COUNT					2

class MW_ES_LIB CESDecoder
{
public:
	CESDecoder(void);

public:

protected:

	uint32_t		m_dwStreamType;

	char			m_pszFileName[256];

	int				m_hFile;
	int64_t			m_nFileTotalSize;
	int64_t			m_nFileStartPos;
	int64_t			m_nCurReadPos;

public:

	int			Open(uint32_t dwStreamType, const char* pszFileName);
	int			Close(void);
	int			GetTitle(char* pszTitle, int strLen);

public:
	virtual int	Preview_FirstPicture(void);
	virtual int	Preview_LastPicture(void);

	virtual int	Preview_Forward1Picture(void);
	virtual int	Preview_Backward1Picture(void);

	virtual int	Preview_ForwardNPicture(int n);
	virtual int	Preview_BackwardNPicture(int n);

	virtual int	Preview_CurPicture(void);
	virtual int	Preview_SeekAtPercent(int nPercent);
	virtual int	Preview_beEOF(void);

public:
	~CESDecoder();
};

#endif

