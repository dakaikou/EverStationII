#ifndef _TSMAGIC_DBASE_OCDCS_H_
#define _TSMAGIC_DBASE_OCDCS_H_

#include <stdint.h>
#include <stdio.h>

#include "../compile.h"

#define OPEN_DSMCC_DOWNLOAD			1

#define MAX_DOWNLOAD_SESSION		64

//typedef struct _DC_DOWNLOAD_INFO_s
//{
//	int					oc_flag;			
//
//	uint16_t			PID;
//	uint32_t			downloadId;
//	int					blockSize;
//
//	uint16_t			moduleId;
//	uint32_t			moduleSize;
//	uint8_t				moduleVersion;
//
//	char				moduleName[256];
//	FILE*				fp;
//
//	//通过计算得到的值
//	int					blockCount;
//	int*				blockFlags;
//
//	uint32_t			downloadSize;
//
//	int					collect_over;
//
//} DC_DOWNLOAD_INFO_t;

//typedef struct _OC_DOWNLOAD_INFO_s
//{
//	uint16_t			PID;
//	uint32_t			downloadId;
//
//	uint16_t			moduleId;
//	uint32_t			objectKey_data;
//
//	char				pszFileName[256];
//	FILE*				fp;
//
//} OC_DOWNLOAD_INFO_t;

typedef struct _DOWNLOAD_INFO_s
{
	uint16_t usCandidatePID;

} DOWNLOAD_INFO_t;

class _CDL_EXPORT CDB_OCDCs
{
public:
	CDB_OCDCs(void);
	~CDB_OCDCs(void);

public:
	//int					m_dsmcc_download_enable;
	//int					m_dc_download_over;
	//int					m_oc_download_over;

protected:

	//int					m_memory_used;								//单位：字节

	//int					m_dc_download_count;
	//DC_DOWNLOAD_INFO_t*	m_astDCDownloadInfos;
	//int					m_memory_for_dsmcc_dc_download;

	//int					m_oc_download_count;
	//OC_DOWNLOAD_INFO_t*	m_astOCDownloadInfos;
	//int					m_memory_for_dsmcc_oc_download;

	DOWNLOAD_INFO_t m_astDownloadInfos[MAX_DOWNLOAD_SESSION];
	int m_nDownloadSessionCount;

public:

protected:

public:
	void Init(void);
	void Reset(void);

	int AppendDownloadInfo(DOWNLOAD_INFO_t* pstDownloadInfo);
	int GetDownloadInfo(int index, DOWNLOAD_INFO_t* pstDownloadInfo);
	int GetDownloadCount(void);
	//int AddOCDownloadInfo(uint16_t usPID, uint32_t downloadID, uint32_t objectKey_data, uint16_t moduleID, char* pszFileName);

	//int AddDCDownloadInfo(int oc_flag, uint16_t usPID, uint32_t downloadID, int blockSize, uint16_t moduleID, int moduleSize, int moduleVersion, char* pszModuleName);

	//int GetDCDownloadCount(void);
	//DC_DOWNLOAD_INFO_t* GetDCDownloadInfo(int index);

	//int GetOCDownloadCount(void);
	//OC_DOWNLOAD_INFO_t* GetOCDownloadInfo(int index);
};


#endif		//_TSMAGIC_DBASE_PSISITABLES_H_

