#ifndef _CDL_DSMCC_TABLE_H_
#define _CDL_DSMCC_TABLE_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_PSISI_EXPORT
#       define MW_PSISI_LIB __declspec(dllexport)
#   elif defined(_MW_PSISI_IMPORT)
#       define MW_PSISI_LIB __declspec(dllimport)
#   else
#       define MW_PSISI_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_PSISI_LIB __attribute__((visibility("default")))
#else
#   define MW_PSISI_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_Common.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Descriptor.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DII.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DSI.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_DDM_DDB.h"

#include "MiddleWare_PSISI_Common.h"

#define MAX_DOWNLOAD_OBJS			256
/*------------------------------------------------------------
		AIT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CAIT : public CPVT
{
public:
	CAIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CAIT(void);
public:

	//------------------semantic part------------------------
	uint16_t						m_usApplicationType;

	//------------------syntax part---------------------------
public:

	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

typedef struct DSMCC_DSI_s
{
	int				data_broadcast_type;						//0x0006 -- DC, 0x0007 -- OC

																//DC
	int				NumberOfGroups;
	GroupInfo_t*	astGroupInfo;

	//OC
	//业务网关信息
	uint32_t				carouselId;
	uint16_t				moduleId_for_srg;
	uint32_t				objectKey_data_for_srg;

	//	uint16_t				moduleId_for_dii;
	uint16_t				table_id_extension_for_dii;

} DSMCC_DSI_t;

typedef struct DSMCC_DII_s
{
	uint32_t			downloadId;								//32
	uint16_t			blockSize;								//16
	uint8_t			windowSize;								//8
	uint8_t			ackPeriod;								//8
	uint32_t			tCDownloadWindow;						//32
	uint32_t			tCDownloadScenario;						//32

	uint16_t				numberOfModules;					//16
	ModuleInfo_t*	astModuleInfo;

} DSMCC_DII_t;

class MW_PSISI_LIB CDSMCC_UNM : public CPVT
{
public:
	CDSMCC_UNM(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CDSMCC_UNM(void);

protected:
	uint16_t					m_usMessageId;

public:
	union
	{
		DSMCC_DSI_t					m_DSI;			//两者只能有其一，由messageId决定
		DSMCC_DII_t					m_DII;
	} u;

	int							m_nMemoryForModuleInfos;
	int							m_nMemoryForGroupInfos;

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetMessageID(void);
};

class MW_PSISI_LIB CDSMCC_DDM : public CPVT
{
public:
	CDSMCC_DDM(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CDSMCC_DDM(void);

protected:
	struct
	{
		uint8_t* buf;
		int		 length;
	} m_astBlockInfo[256];		//block_count 等同于section_count

	int						m_nMemAllocatedForModule;

	uint8_t*				m_ucModuleBuf;
	int						m_nModuleSize;
	uint16_t				m_usMessageId;

public:

	int							m_nDirMessageCount;
	BIOP::DirectoryMessage_t*	m_pDirectoryMessage[MAX_DOWNLOAD_OBJS];					//OC
	int							m_nMemAllocatedForDirMessages;

	int							m_nFileMessageCount;
	BIOP::FileMessage_t*		m_pFileMessage[MAX_DOWNLOAD_OBJS];						//OC
	int							m_nMemAllocatedForFileMessages;

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	void SaveModuleBufToDisk(char* pszFatherDirectory, char* pszModuleName);
};

//MPE datagram
class MW_PSISI_LIB CMPE : public CPVT
{
public:
	CMPE(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CMPE(void);
public:

/*------------------semantic part---------------------------*/
/*------------------syntax part---------------------------*/

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

/*------------------------------------------------------------
		INT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CINT : public CPVT
{
public:
	CINT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CINT(void);
public:

/*------------------semantic part---------------------------*/
	uint32_t		m_uiPlatformID;

/*------------------syntax part---------------------------*/

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

#endif
