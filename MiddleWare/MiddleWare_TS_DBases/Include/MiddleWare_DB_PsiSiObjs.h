#ifndef _TSMAGIC_DBASE_PSISI_OBJS_H_
#define _TSMAGIC_DBASE_PSISI_OBJS_H_

#include <stdint.h>

#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

#define DEBUG_PAT			1
#define DEBUG_PMT			1
#define DEBUG_CAT			1
#define DEBUG_TSDT			1
#define DEBUG_NIT			1
#define DEBUG_SDT			1
#define DEBUG_EIT			1
#define DEBUG_BAT			1
#define DEBUG_TDT			1
#define DEBUG_TOT			1
#define DEBUG_RST			1
#define DEBUG_CMT			1
#define DEBUG_DSMCC			1
#define DEBUG_MPE			1
#define DEBUG_AIT			1
#define DEBUG_INT			1
#define DEBUG_ECM			1
#define DEBUG_EMM			1

class _CDL_EXPORT CDB_PsiSiObjs
{
public:
	CDB_PsiSiObjs(void);
	~CDB_PsiSiObjs(void);

protected:

	int			m_memory_used;								//单位：字节

	/*--------------------------------------------------
			MPEG2 DVB syntax part
	---------------------------------------------------*/
	int			m_obj_count;
	CPVT**		m_apPsiSiObjs;									//由PID、table_id、table_id_extension共同确定一个表
	int			m_memory_used_for_objs;

protected:
	void Init(void);

	void ResetRecords(void);

	int DSMCC_BuildOCTree(uint16_t PID, DSMCC_DSI_t* pDSI, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
	int OC_DownloadDirectoryAndFiles(uint16_t PID, uint16_t moduleId_for_srg, uint32_t objectKey_data, char* pszRootPath);

	int DSMCC_BuildDCTree(uint16_t PID, DSMCC_DSI_t* pDSI, uint8_t carousel_type_id, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
	int DSMCC_DownloadDCTree(uint16_t PID, DSMCC_DSI_t* pDSI, uint8_t carousel_type_id, char* pszRootPath);

	int OC_BuildDirectory(uint16_t PID, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, uint16_t moduleId_for_srg, uint32_t objectKey_data);

public:

	void Reset(void);

	CPVT* Append(uint16_t PID, uint8_t table_id, uint16_t table_id_extension);

	CPVT* QueryByKey(uint32_t Key);
	CPVT* QueryBy2ID(uint16_t PID, uint8_t table_id);
	CPVT* QueryBy3ID(uint16_t PID, uint8_t table_id, uint16_t table_id_extension);

	CEIT* QueryEITSchBy3ID(uint16_t usOriginalNetworkID, uint16_t usTransportStreamID, uint16_t usServiceID);

	CSDT* QuerySDTBy2ID(uint16_t usOriginalNetworkID, uint16_t usTransportStreamID);
	CSDT* QuerySDTByTSID(uint16_t usTransportStreamID);

	CNIT* QueryNITByID(uint16_t usNetworkID);
	CPAT* QueryPAT(void);

	CSDT* QueryActualSDT(void);
	CDSMCC_UNM* QueryDsmccUNM_DSI(uint16_t PID);

	int BuildDsmccTree(uint16_t usPID, HALForXMLDoc* pxmlDoc);
	int DownloadDsmccTree(uint16_t usPID, char* pszRootPath);

	int BuildBouquetTree(uint32_t uiCode, HALForXMLDoc* pxmlDoc);
};

#endif		//_TSMAGIC_DBASE_PSISITABLES_H_
