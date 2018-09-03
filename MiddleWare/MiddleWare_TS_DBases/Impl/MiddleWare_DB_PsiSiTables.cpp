#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#include "../Include/MiddleWare_DB_PsiSiTables.h"
#include "../Include/MiddleWare_DB_ErrorCode.h"

#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_SI_Utilities.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_DSMCC_Utilities.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

CDB_PsiSiTables::CDB_PsiSiTables()
{
	Init();
}

CDB_PsiSiTables::~CDB_PsiSiTables()
{
	Reset();
}

void CDB_PsiSiTables::Init(void)
{
	m_memory_used = 0;

	m_apPVTs = NULL;

	ResetRecords();

#if DEBUG_OCDC
	m_dc_download_count = 0;
	m_astDCDownloadInfo = NULL;

	m_oc_download_count = 0;
	m_astOCDownloadInfo = NULL;

	ResetOCDCs();
#endif

	time_t tm;
	srand((unsigned int)time(&tm));
}


void CDB_PsiSiTables::Reset(void)
{
	ResetRecords();

#if DEBUG_OCDC
	ResetOCDCs();
#endif

	m_memory_used = 0;
}

void CDB_PsiSiTables::ResetRecords(void)
{
	int			index;
	CPVT*		pPVT;

	if (m_apPVTs != NULL)
	{
		for (index = 0; index < m_pvt_count; index++)
		{
			pPVT = m_apPVTs[index];

			if (pPVT != NULL)
			{
				pPVT->Reset();					//无法复位派生类
				delete pPVT;

				m_apPVTs[index] = NULL;
			}
		}

		free(m_apPVTs);
	}

	m_apPVTs = NULL;
	m_pvt_count = 0;
	m_memory_for_pvts = 0;
}

//int CDB_PsiSiTables::GetRecordCount(void)
//{
//	return m_pvt_count;
//}

CPVT* CDB_PsiSiTables::QueryByKey(uint32_t Key)
{
	CPVT* pPVT = NULL;

	for (int pvt_index = 0; pvt_index < m_pvt_count; pvt_index++)
	{
		CPVT* pTemp = m_apPVTs[pvt_index];
		if (pTemp != NULL)
		{
			if (Key == pTemp->GetKey()) {
				pPVT = pTemp;
				break;
			}
		}
	}

	return pPVT;
}

CPVT* CDB_PsiSiTables::QueryBy3ID(uint16_t PID, uint8_t table_id, uint16_t table_id_extension)
{
	CPVT* pPVT = NULL;

	for (int pvt_index = 0; pvt_index < m_pvt_count; pvt_index++)
	{
		CPVT* pTemp = m_apPVTs[pvt_index];
		if (pTemp != NULL)
		{
			if ((PID == pTemp->GetPID()) &&
				(table_id == pTemp->GetTableID()) &&
				(table_id_extension == pTemp->GetTableIDExtension()))
			{
				pPVT = pTemp;
				break;
			}
		}
	}

	return pPVT;
}

CPVT* CDB_PsiSiTables::QueryBy2ID(uint16_t PID, uint8_t table_id)
{
	CPVT* pPVT = NULL;

	for (int pvt_index = 0; pvt_index < m_pvt_count; pvt_index++)
	{
		CPVT* pTemp = m_apPVTs[pvt_index];
		if (pTemp != NULL)
		{
			if ((PID == pTemp->GetPID()) &&
				(table_id == pTemp->GetTableID()))
			{
				pPVT = pTemp;
				break;
			}
		}
	}

	return pPVT;
}

CPVT* CDB_PsiSiTables::AppendRecord(uint16_t PID, uint8_t table_id, uint16_t table_id_extension)
{
	CPVT* pPVT = NULL;

	uint32_t usKey = (uint32_t)rand();
	assert(usKey > 0);

	if (table_id == TABLE_ID_PAT)
	{
#if DEBUG_PAT
		CPAT* pPAT = new CPAT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pPAT;
#endif
	}
	else if (table_id == TABLE_ID_CAT)
	{
#if DEBUG_CAT
		CCAT* pCAT = new CCAT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pCAT;
#endif
	}
	else if (table_id == TABLE_ID_PMT)
	{
#if DEBUG_PMT
		CPMT* pPMT = new CPMT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pPMT;
#endif
	}
	else if (table_id == TABLE_ID_TSDT)
	{
#if DEBUG_TSDT
		CTSDT* pTSDT = new CTSDT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pTSDT;
#endif
	}
	else if ((table_id == TABLE_ID_NIT_ACTUAL) || (table_id == TABLE_ID_NIT_OTHER))
	{
#if DEBUG_NIT
		CNIT* pNIT = new CNIT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pNIT;
#endif
	}
	else if ((table_id == TABLE_ID_SDT_ACTUAL) || (table_id == TABLE_ID_SDT_OTHER))
	{
#if DEBUG_SDT
		CSDT* pSDT = new CSDT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pSDT;
#endif
	}
	else if (table_id == TABLE_ID_BAT)
	{
#if DEBUG_BAT
		CBAT* pBAT = new CBAT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pBAT;
#endif
	}
	else if (table_id == TABLE_ID_INT)
	{
#if DEBUG_INT
		CINT* pINT = new CINT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pINT;
#endif
	}
	else if ((table_id >= TABLE_ID_EIT_PF_ACTUAL) && (table_id <= TABLE_ID_EIT_SCH_OTHER_F))
	{
#if DEBUG_EIT
		CEIT* pEIT = new CEIT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pEIT;
#endif
	}
	else if (table_id == TABLE_ID_TDT)
	{
#if DEBUG_TDT
		CTDT* pTDT = new CTDT(usKey, PID, table_id);
		pPVT = (CPVT*)pTDT;
#endif
	}
	else if (table_id == TABLE_ID_RST)
	{
#if DEBUG_RST
		CRST* pRST = new CRST(usKey, PID, table_id);
		pPVT = (CPVT*)pRST;
#endif
	}
	else if (table_id == TABLE_ID_TOT)
	{
#if DEBUG_TOT
		CTOT* pTOT = new CTOT(usKey, PID, table_id);
		pPVT = (CPVT*)pTOT;
#endif
	}
	else if ((table_id >= TABLE_ID_ECM_MIN) && (table_id <= TABLE_ID_EMM_MAX))
	{
#if DEBUG_ECM || DEBUG_EMM
		CCMT* pCMT = new CCMT(usKey, PID, table_id);
		pPVT = (CPVT*)pCMT;
#endif
	}
	else if (table_id == TABLE_ID_AIT)
	{
#if DEBUG_AIT
		CAIT* pAIT = new CAIT(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pAIT;
#endif
	}
	else if (table_id == TABLE_ID_DSMCC_MPE)
	{
#if DEBUG_MPE
		CMPE* pMPE = new CMPE(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pMPE;
#endif
	}
	else if (table_id == TABLE_ID_DSMCC_UNM)
	{
#if DEBUG_DSMCC
		CDSMCC_UNM* pDSMCC = new CDSMCC_UNM(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pDSMCC;
#endif
	}
	else if (table_id == TABLE_ID_DSMCC_DDM)
	{
#if DEBUG_DSMCC
		CDSMCC_DDM* pDSMCC = new CDSMCC_DDM(usKey, PID, table_id, table_id_extension);
		pPVT = (CPVT*)pDSMCC;
#endif
	}
//		else if (table_id == TABLE_ID_DSMCC_PVT)
//		{
//#if DEBUG_DSMCC
//			CDSMCC_PVT* pDSMCC = new CDSMCC_PVT(usKey, PID, table_id, table_id_extension);
//			pPVT = (CPVT*)pDSMCC;
//#endif
//		}
	else
	{
		pPVT = new CPVT(usKey, PID, table_id, table_id_extension);				//无法new 派生类
	}

	if (pPVT != NULL)
	{
		m_pvt_count++;

		m_apPVTs = (CPVT**)realloc(m_apPVTs, m_pvt_count * sizeof(CPVT*));
		if (m_apPVTs != NULL)
		{
			m_apPVTs[m_pvt_count - 1] = pPVT;
			m_memory_for_pvts = m_pvt_count * sizeof(CPVT*);
		}
	}

	return pPVT;
}

CEIT* CDB_PsiSiTables::QueryEITSchBy3ID(uint16_t usOriginalNetworkID, uint16_t usTransportStreamID, uint16_t usServiceID)
{
	CEIT* pEIT = NULL;

	for (int i = 0; i < m_pvt_count; i++)
	{
		CPVT* pPVT = m_apPVTs[i];
		uint8_t ucTableID = pPVT->GetTableID();
		if ((ucTableID >= TABLE_ID_EIT_SCH_ACTUAL_0) && (ucTableID <= TABLE_ID_EIT_SCH_OTHER_F))
		{
			CEIT* pTemp = (CEIT*)pPVT;

			if ((usOriginalNetworkID == pTemp->GetOriginalNetworkID()) &&
				(usTransportStreamID == pTemp->GetTransportStreamID()) &&
				(usServiceID == pTemp->GetServiceID()))
			{
				pEIT = pTemp;
				break;
			}
		}
	}

	return pEIT;
}

CSDT* CDB_PsiSiTables::QuerySDTBy2ID(uint16_t usOriginalNetworkID, uint16_t usTransportStreamID)
{
	CSDT* pSDT = NULL;

	for (int i = 0; i < m_pvt_count; i++)
	{
		CPVT* pPVT = m_apPVTs[i];
		uint8_t ucTableID = pPVT->GetTableID();
		if ((ucTableID == TABLE_ID_SDT_ACTUAL) || (ucTableID == TABLE_ID_SDT_OTHER))
		{
			CSDT* pTemp = (CSDT*)pPVT;

			if ((usOriginalNetworkID == pTemp->GetOriginalNetworkID()) &&
				(usTransportStreamID == pTemp->GetTransportStreamID()))
			{
				pSDT = pTemp;
				break;
			}
		}
	}

	return pSDT;
}

CSDT* CDB_PsiSiTables::QuerySDTByTSID(uint16_t usTransportStreamID)
{
	CSDT* pSDT = NULL;

	for (int i = 0; i < m_pvt_count; i++)
	{
		CPVT* pPVT = m_apPVTs[i];
		uint8_t ucTableID = pPVT->GetTableID();
		if ((ucTableID == TABLE_ID_SDT_ACTUAL) || (ucTableID == TABLE_ID_SDT_OTHER))
		{
			CSDT* pTemp = (CSDT*)pPVT;

			if (usTransportStreamID == pTemp->GetTransportStreamID())
			{
				pSDT = pTemp;
				break;
			}
		}
	}

	return pSDT;
}

CNIT* CDB_PsiSiTables::QueryNITByID(uint16_t usNetworkID)
{
	CNIT* pNIT = NULL;

	for (int i = 0; i < m_pvt_count; i++)
	{
		CPVT* pPVT = m_apPVTs[i];
		uint8_t ucTableID = pPVT->GetTableID();
		if ((ucTableID == TABLE_ID_NIT_ACTUAL) || (ucTableID == TABLE_ID_NIT_OTHER))
		{
			CNIT* pTemp = (CNIT*)pPVT;

			if (usNetworkID == pTemp->GetNetworkID())
			{
				pNIT = pTemp;
				break;
			}
		}
	}

	return pNIT;
}

CPAT* CDB_PsiSiTables::QueryPAT(void)
{
	CPAT* pPAT = NULL;

	pPAT = (CPAT*)QueryBy2ID(0x0000, TABLE_ID_PAT);

	//for (int i = 0; i < m_pvt_count; i++)
	//{
	//	CPVT* pPVT = m_apPVTs[i];
	//	uint8_t ucTableID = pPVT->GetTableID();
	//	if (ucTableID == TABLE_ID_PAT)
	//	{
	//		CPAT* pTemp = (CPAT*)pPVT;
	//		break;
	//	}
	//}

	return pPAT;
}

CSDT* CDB_PsiSiTables::QueryActualSDT(void)
{
	CSDT* pSDT = NULL;

	pSDT = (CSDT*)QueryBy2ID(0x0011, TABLE_ID_SDT_ACTUAL);

	return pSDT;
}

CDSMCC_UNM* CDB_PsiSiTables::QueryDsmccUNM_DSI(uint16_t PID)
{
	CDSMCC_UNM* pDSMCC = NULL;

	for (int i = 0; i < m_pvt_count; i++)
	{
		CPVT* pPVT = m_apPVTs[i];
		if ((pPVT->GetPID() == PID) && (pPVT->GetTableID() == TABLE_ID_DSMCC_UNM))
		{
			CDSMCC_UNM* pTempTable = (CDSMCC_UNM*)pPVT;
			if (pTempTable->GetMessageID() == 0x1006)
			{
				pDSMCC = pTempTable;
				break;
			}
		}
	}

	return pDSMCC;
}

int CDB_PsiSiTables::DSMCC_BuildOCTree(uint16_t PID, DSMCC_DSI_t* pDSI, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pxmlDoc != NULL)
	{
#if DEBUG_DSMCC
		if (pDSI != NULL)			//DSI
		{
			CDSMCC_UNM*			pDSMCC_DII;
			CDSMCC_DDM*			pDSMCC_DDM;

			DSMCC_DII_t*				pDII;
			DirectoryMessage_t*			pDirectoryMessage;
			BIOP_FileMessage_t*			pBIOP_FileMessage;
			Bindings_t*					pBindings;
			//BIOP::Name_t*				pName;
			IOP::IOR_t*					pIOR;
			BIOPProfileBody_t*			pBIOPProfileBody;
			LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
			BIOP::ObjectLocation_t*		pObjectLocation;
			DSM::ConnBinder_t*			pConnBinder;

			char			pszText[256];
			char			pszTemp[64];
			int				module_index;
			int				binding_index;
			int				blockCount;
			int				component_index;
			int				profile_index;
			int				object_index;
			//int				i;

			tinyxml2::XMLElement*	hDsiItem;
			tinyxml2::XMLElement*	hDiiItem;
			tinyxml2::XMLElement*	hDdbItem;
			tinyxml2::XMLElement*	hSrgItem;
			tinyxml2::XMLElement*	hDirItem;
			tinyxml2::XMLElement*	hChildItem;
			tinyxml2::XMLElement*	hModuleItem;
			tinyxml2::XMLElement*	hBindingItem;
			tinyxml2::XMLElement*	hNameComponentItem;
			tinyxml2::XMLElement*	hIORItem;
			tinyxml2::XMLElement*	hIOP_taggedProfileItem;

			hDsiItem = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "DSI()");

			////DSI信息
			{
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDsiItem, "carouselId", pDSI->carouselId, 32);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDsiItem, "moduleId_for_srg", pDSI->moduleId_for_srg, 16);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDsiItem, "objectKey_data_for_srg", pDSI->objectKey_data_for_srg, 32);

				hChildItem = pxmlDoc->NewKeyValuePairElement(hDsiItem, "table_id_extension_for_dii", pDSI->table_id_extension_for_dii, 16);
			}

			////DII信息
			hDiiItem = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "DII()");

			pDSMCC_DII = (CDSMCC_UNM*)QueryBy3ID(PID, TABLE_ID_DSMCC_UNM, pDSI->table_id_extension_for_dii);

			if (pDSMCC_DII != NULL)
			{
				pDII = &(pDSMCC_DII->u.m_DII);

				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "downloadId", pDII->downloadId, 32);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "blockSize", pDII->blockSize, 16);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "windowSize", pDII->windowSize, 8);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "ackPeriod", pDII->ackPeriod, 8);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "tCDownloadWindow", pDII->tCDownloadWindow, 32);
				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "tCDownloadScenario", pDII->tCDownloadScenario, 32);

				hChildItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, "numberOfModules", pDII->numberOfModules, 16);

				for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
				{
					sprintf_s(pszText, sizeof(pszText), "MODULE (%d - 0x%04X)", module_index, pDII->astModuleInfo[module_index].moduleId);
					hModuleItem = pxmlDoc->NewKeyValuePairElement(hDiiItem, pszText);

					hChildItem = pxmlDoc->NewKeyValuePairElement(hModuleItem, "moduleId", pDII->astModuleInfo[module_index].moduleId, 16);
					hChildItem = pxmlDoc->NewKeyValuePairElement(hModuleItem, "moduleSize", pDII->astModuleInfo[module_index].moduleSize, 32);
					hChildItem = pxmlDoc->NewKeyValuePairElement(hModuleItem, "moduleVersion", pDII->astModuleInfo[module_index].moduleVersion, 8);

					//计算参数
					blockCount = (S32)(ceil((double)pDII->astModuleInfo[module_index].moduleSize / pDII->blockSize));
					//sprintf_s(pszText, sizeof(pszText), "%d", blockCount);
					hChildItem = pxmlDoc->NewKeyValuePairElement(hModuleItem, "blockCount", blockCount);

					////DDB信息
					pDSMCC_DDM = (CDSMCC_DDM*)QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, pDII->astModuleInfo[module_index].moduleId);
					if (pDSMCC_DDM != NULL)
					{
						hDdbItem = pxmlDoc->NewKeyValuePairElement(hModuleItem, "DDB()");

						for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index++)
						{
							pDirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];

							if (pDirectoryMessage != NULL)
							{
								if (strcmp(pDirectoryMessage->objectKind_data, "srg") == 0)
								{
									sprintf_s(pszText, sizeof(pszText), "BIOP::ServiceGatewayMessage (objectKey_data_byte=0x%08X)", pDirectoryMessage->objectKey_data);
								}
								else if (strcmp(pDirectoryMessage->objectKind_data, "dir") == 0)
								{
									sprintf_s(pszText, sizeof(pszText), "BIOP::DirectoryMessage (objectKey_data_byte=0x%08X)", pDirectoryMessage->objectKey_data);
								}
								else
								{
									//								assert(0);
								}
								hSrgItem = pxmlDoc->NewKeyValuePairElement(hDdbItem, pszText);

								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "magic", pDirectoryMessage->magic);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "biop_version.major", 8, pDirectoryMessage->biop_version.major);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "biop_version.minor", 8, pDirectoryMessage->biop_version.minor);

								MPEG2_DSMCC_NumericCoding2Text_byteOrder(pDirectoryMessage->byte_order, pszTemp, sizeof(pszTemp));
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "byte_order", pDirectoryMessage->byte_order, 8, pszTemp);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "message_type", pDirectoryMessage->message_type, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "message_size", pDirectoryMessage->message_size, 32);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKey_length", pDirectoryMessage->objectKey_length, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKey_data_byte", pDirectoryMessage->objectKey_data, 32);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKind_length", pDirectoryMessage->objectKind_length, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKind_data", pDirectoryMessage->objectKind_data);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectInfo_length", pDirectoryMessage->objectInfo_length, 16);

								if (pDirectoryMessage->objectInfo_length > 0)
								{
									//memset(pszText, 0x00, sizeof(pszText));
									//for (i = 0; i < min(8, pDirectoryMessage->objectInfo_length); i++)
									//{
									//	sprintf_s(pszText + i * 3, 4, "%02X ", pDirectoryMessage->objectInfo_data_byte[i]);
									//}
									//if (i < pDirectoryMessage->objectInfo_length)
									//{
									//	sprintf_s(pszText + i * 3, 4, "...");
									//}
									hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectInfo_data_byte", pDirectoryMessage->objectInfo_data_byte, pDirectoryMessage->objectInfo_length);
								}

								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "serviceContextList_count", pDirectoryMessage->serviceContextList_count, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "messageBody_length", pDirectoryMessage->messageBody_length, 32);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "bindings_count", pDirectoryMessage->bindings_count, 16);

								if (pDirectoryMessage->bindings_count > 0)
								{
									for (binding_index = 0; binding_index < pDirectoryMessage->bindings_count; binding_index++)
									{
										pBindings = pDirectoryMessage->bindings + binding_index;
										//pName = &(pBindings->Name);

										if (pBindings->Name.nameComponents_count > 0)
										{
											sprintf_s(pszText, sizeof(pszText), "binding (%d)----%s:%s", binding_index, pBindings->Name.kind_data_byte[0], pBindings->Name.id_data_byte[0]);
										}
										else
										{
											sprintf_s(pszText, sizeof(pszText), "binding (%d)", binding_index);
										}
										hBindingItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, pszText);

										sprintf_s(pszText, sizeof(pszText), "BIOP::Name()");
										hNameComponentItem = pxmlDoc->NewKeyValuePairElement(hBindingItem, pszText);

										hChildItem = pxmlDoc->NewKeyValuePairElement(hNameComponentItem, "nameComponents_count", pBindings->Name.nameComponents_count, 8);

										for (component_index = 0; component_index < pBindings->Name.nameComponents_count; component_index++)
										{
											sprintf_s(pszText, sizeof(pszText), "NameComponent (%d)", component_index);
											hChildItem = pxmlDoc->NewKeyValuePairElement(hNameComponentItem, pszText);

											pxmlDoc->NewKeyValuePairElement(hChildItem, "id_length", pBindings->Name.id_length[component_index], 8);
											if (pBindings->Name.id_length[component_index] > 0)
											{
												pxmlDoc->NewKeyValuePairElement(hChildItem, "id_data_byte", (uint8_t*)pBindings->Name.id_data_byte[component_index], pBindings->Name.id_length[component_index], pBindings->Name.id_data_byte[component_index]);
											}

											pxmlDoc->NewKeyValuePairElement(hChildItem, "kind_length", pBindings->Name.kind_length[component_index], 8);
											if (pBindings->Name.kind_length[component_index] > 0)
											{
												//pxmlDoc->NewKeyValueElement(hChildItem, "kind_data_byte", (uint8_t*)(pBindings->Name.kind_data_byte[component_index]), pBindings->Name.kind_length[component_index], pBindings->Name.kind_data_byte[component_index]);
											}
										}

										MPEG2_DSMCC_NumericCoding2Text_bindingType(pBindings->bindingType, pszTemp, sizeof(pszTemp));
										hChildItem = pxmlDoc->NewKeyValuePairElement(hBindingItem, "bindingType", pBindings->bindingType, 8, pszTemp);

										sprintf_s(pszText, sizeof(pszText), "IOP::IOR()");
										hIORItem = pxmlDoc->NewKeyValuePairElement(hBindingItem, pszText);

										pIOR = &(pBindings->IOR);

										hChildItem = pxmlDoc->NewKeyValuePairElement(hIORItem, "type_id_length", pIOR->type_id_length, 8);
										hChildItem = pxmlDoc->NewKeyValuePairElement(hIORItem, "type_id_byte", (uint8_t*)pIOR->type_id_byte, pIOR->type_id_length, pIOR->type_id_byte);
										hChildItem = pxmlDoc->NewKeyValuePairElement(hIORItem, "taggedProfiles_count", pIOR->taggedProfiles_count, 8);

										if (pIOR->taggedProfiles_count > 0)
										{
											for (profile_index = 0; profile_index < (S32)(pIOR->taggedProfiles_count); profile_index++)
											{
												if (pIOR->taggedProfile[profile_index].profileId_tag == 0x49534F06)
												{
													pBIOPProfileBody = &(pIOR->taggedProfile[profile_index].u.BIOPProfileBody);

													hIOP_taggedProfileItem = pxmlDoc->NewKeyValuePairElement(hIORItem, "BIOPProfileBody()");

													MPEG2_DSMCC_NumericCoding2Text_tagId(pBIOPProfileBody->profileId_tag, pszTemp, sizeof(pszTemp));
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "profileId_tag", pBIOPProfileBody->profileId_tag, 32, pszTemp);
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "profile_data_length", pBIOPProfileBody->profile_data_length, 32);

													MPEG2_DSMCC_NumericCoding2Text_byteOrder(pBIOPProfileBody->profile_data_byte_order, pszTemp, sizeof(pszTemp));
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "profile_data_byte_order", pBIOPProfileBody->profile_data_byte_order, 8, pszTemp);
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "liteComponents_count", pBIOPProfileBody->liteComponents_count, 8);

													if (pBIOPProfileBody->ObjectLocation.componentId_tag == 0x49534F50)
													{
														pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

														sprintf_s(pszText, sizeof(pszText), "BIOP::ObjectLocation()");
														hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, pszText);

														MPEG2_DSMCC_NumericCoding2Text_tagId(pObjectLocation->componentId_tag, pszTemp, sizeof(pszTemp));
														sprintf_s(pszText, sizeof(pszText), "componentId_tag = 0x%08X [%s]", pObjectLocation->componentId_tag, pszTemp);
														pxmlDoc->NewKeyValuePairElement(hChildItem, pszText);

														pxmlDoc->NewKeyValuePairElement(hChildItem, "component_data_length", pObjectLocation->component_data_length, 8);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "carouselId", pObjectLocation->carouselId, 32);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "moduleId", pObjectLocation->moduleId, 16);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "version.major", pObjectLocation->version.major, 8);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "version.minor", pObjectLocation->version.minor, 8);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "objectKey_length", pObjectLocation->objectKey_length, 8);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "objectKey_data_byte", pObjectLocation->objectKey_data, 32);
													}

													if (pBIOPProfileBody->ConnBinder.componentId_tag == 0x49534F40)
													{
														pConnBinder = &(pBIOPProfileBody->ConnBinder);

														sprintf_s(pszText, sizeof(pszText), "DSM::ConnBinder()");
														hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, pszText);

														MPEG2_DSMCC_NumericCoding2Text_tagId(pConnBinder->componentId_tag, pszTemp, sizeof(pszTemp));
														sprintf_s(pszText, sizeof(pszText), "componentId_tag = 0x%08X [%s]", pConnBinder->componentId_tag, pszTemp);
														pxmlDoc->NewKeyValuePairElement(hChildItem, pszText);

														pxmlDoc->NewKeyValuePairElement(hChildItem, "component_data_length", 8, pConnBinder->component_data_length);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "taps_count", 8, pConnBinder->taps_count);

														hChildItem = pxmlDoc->NewKeyValuePairElement(hChildItem, "DSM::Tap()");

														pxmlDoc->NewKeyValuePairElement(hChildItem, "id", pConnBinder->Tap.id, 16);
														MPEG2_DSMCC_NumericCoding2Text_tapUse(pConnBinder->Tap.use, pszTemp, sizeof(pszTemp));
														pxmlDoc->NewKeyValuePairElement(hChildItem, "use", pConnBinder->Tap.use, 16, pszTemp);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "association_tag", pConnBinder->Tap.association_tag, 16);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "selector_length", pConnBinder->Tap.selector_length, 8);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "selector_type", pConnBinder->Tap.selector_type, 16);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "transactionId", pConnBinder->Tap.transactionId, 32);
														pxmlDoc->NewKeyValuePairElement(hChildItem, "timeout", pConnBinder->Tap.timeout, 32);
													}
												}
												else if (pIOR->taggedProfile[profile_index].profileId_tag == 0x49534F05)
												{
													pLiteOptionsProfileBody = &(pIOR->taggedProfile[profile_index].u.LiteOptionsProfileBody);

													hIOP_taggedProfileItem = pxmlDoc->NewKeyValuePairElement(hIORItem, "LiteOptionsProfileBody()");

													MPEG2_DSMCC_NumericCoding2Text_tagId(pLiteOptionsProfileBody->profileId_tag, pszTemp, sizeof(pszTemp));
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "profileId_tag", pLiteOptionsProfileBody->profileId_tag, 32, pszTemp);
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "profile_data_length", pLiteOptionsProfileBody->profile_data_length, 32);

													MPEG2_DSMCC_NumericCoding2Text_byteOrder(pLiteOptionsProfileBody->profile_data_byte_order, pszTemp, sizeof(pszTemp));
													hChildItem = pxmlDoc->NewKeyValuePairElement(hIOP_taggedProfileItem, "profile_data_byte_order", pLiteOptionsProfileBody->profile_data_byte_order, 8, pszTemp);
												}
											}
										}

										hChildItem = pxmlDoc->NewKeyValuePairElement(hBindingItem, "objectInfo_length", 16, pBindings->objectInfo_length);
										if (pBindings->objectInfo_length > 0)
										{
											//if (strcmp(pBindings->Name.kind_data_byte[0], "fil") == 0)
											//{
											//	sprintf_s(pszText, sizeof(pszText), "0x");
											//	for (i = 0; i < 8; i++)
											//	{
											//		sprintf_s(pszText, sizeof(pszText), "%s%02X", pszText, pBindings->objectInfo_data_byte[i]);
											//	}
											//	hChildItem = pxmlDoc->NewKeyValueElement(hBindingItem, "DSM::File::ContentSize", pBindings->objectInfo_data_byte[i]);

											//	if (pBindings->objectInfo_length > 8)
											//	{
											//		hChildItem = pxmlDoc->NewKeyValueElement(hBindingItem, "descriptor()", pszText);
											//	}
											//}
											//else
											//{
											//	sprintf_s(pszText, sizeof(pszText), "");
											//	for (i = 0; i < min(16, pBindings->objectInfo_length); i++)
											//	{
											//		sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, pBindings->objectInfo_data_byte[i]);
											//	}
											//	hChildItem = pxmlDoc->NewKeyValueElement(hBindingItem, "objectInfo_data_byte", pszText);
											//}
										}
									}
								}
							}
						}

						for (object_index = 0; object_index < pDSMCC_DDM->m_nFileMessageCount; object_index++)
						{
							pBIOP_FileMessage = pDSMCC_DDM->m_pFileMessage[object_index];

							if (pBIOP_FileMessage != NULL)
							{
								sprintf_s(pszText, sizeof(pszText), "BIOP::FileMessage (objectKey_data_byte=0x%08X)", pBIOP_FileMessage->objectKey_data);
								hSrgItem = pxmlDoc->NewKeyValuePairElement(hDdbItem, pszText);

								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "magic", pBIOP_FileMessage->magic);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "biop_version.major", pBIOP_FileMessage->biop_version.major, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "biop_version.minor", pBIOP_FileMessage->biop_version.minor, 8);

								MPEG2_DSMCC_NumericCoding2Text_byteOrder(pBIOP_FileMessage->byte_order, pszTemp, sizeof(pszTemp));
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "byte_order", pBIOP_FileMessage->byte_order, 8, pszTemp);

								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "message_type", pBIOP_FileMessage->message_type, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "message_size", pBIOP_FileMessage->message_size, 32);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKey_length", pBIOP_FileMessage->objectKey_length, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKey_data_byte", pBIOP_FileMessage->objectKey_data, 32);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKind_length", pBIOP_FileMessage->objectKind_length, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectKind_data", (uint8_t*)pBIOP_FileMessage->objectKind_data, pBIOP_FileMessage->objectKind_length, pBIOP_FileMessage->objectKind_data);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "objectInfo_length", pBIOP_FileMessage->objectInfo_length, 16);

								//							memset(pszText, 0x00, MAX_TXT_CHARS);
								//sprintf_s(pszText, sizeof(pszText), "0x");
								//for (i = 0; i < 8; i++)
								//{
								//	sprintf_s(pszText, sizeof(pszText), "%s%02X", pszText, pBIOP_FileMessage->ContentSize[i]);
								//}
								//hChildItem = pxmlDoc->NewKeyValueElement(hSrgItem, "DSM::File::ContentSize", pszText);

								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "serviceContextList_count", pBIOP_FileMessage->serviceContextList_count, 8);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "messageBody_length", pBIOP_FileMessage->messageBody_length, 32);
								hChildItem = pxmlDoc->NewKeyValuePairElement(hSrgItem, "content_length", pBIOP_FileMessage->content_length, 32);
							}
						}

					}
				}
			}

			hDirItem = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "文件目录结构");

			//OC_BuildDirectory(PID, hDirItem, pDSI->moduleId_for_srg, pDSI->objectKey_data_for_srg);

		}

#endif
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

int CDB_PsiSiTables::DSMCC_BuildDCTree(uint16_t PID, DSMCC_DSI_t* pDSI, uint8_t carousel_type_id, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pxmlDoc != NULL)
	{
#if DEBUG_DSMCC
		if (pDSI != NULL)
		{
			CDSMCC_UNM*		pDSMCC_DII;
			DSMCC_DII_t*	pDII;

			DC_moduleInfo_t*	pDC_moduleInfo;

			char			pszText[256];
			int				group_index;
			int				module_index;
			int				blockCount;

			tinyxml2::XMLElement*	pxmlDsiItem;
			tinyxml2::XMLElement*	pxmlChildItem;
			tinyxml2::XMLElement*	pxmlGroupItem;
			tinyxml2::XMLElement*	pxmlModuleItem;

			if (carousel_type_id == 0x02)			//2层数据轮播
			{
				pxmlDsiItem = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "SuperGroup()");

				pxmlDoc->NewKeyValuePairElement(pxmlDsiItem, "NumberOfGroups", pDSI->NumberOfGroups, 16);

				for (group_index = 0; group_index < pDSI->NumberOfGroups; group_index++)
				{
					sprintf_s(pszText, sizeof(pszText), "GROUP(%d)", group_index);
					pxmlGroupItem = pxmlDoc->NewKeyValuePairElement(pxmlDsiItem, pszText);

					pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "GroupId", pDSI->astGroupInfo[group_index].GroupId, 32);
					pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "GroupSize", pDSI->astGroupInfo[group_index].GroupSize, 32);

					pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "GroupName", pDSI->astGroupInfo[group_index].name_descriptor.text_char);

					pDSMCC_DII = (CDSMCC_UNM*)QueryBy3ID(PID, TABLE_ID_DSMCC_UNM, (pDSI->astGroupInfo[group_index].GroupId & 0x0000ffff));

					if (pDSMCC_DII != NULL)
					{
						pDII = &(pDSMCC_DII->u.m_DII);

						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "downloadId", pDII->downloadId, 32);
						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "blockSize", pDII->blockSize, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "windowSize", pDII->windowSize, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "ackPeriod", pDII->ackPeriod, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "tCDownloadWindow", pDII->tCDownloadWindow, 32);
						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "tCDownloadScenario", pDII->tCDownloadScenario, 32);

						pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, "numberOfModules", pDII->numberOfModules, 16);

						for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
						{
							sprintf_s(pszText, sizeof(pszText), "MODULE(%d - 0x%04X)", module_index, pDII->astModuleInfo[module_index].moduleId);
							pxmlModuleItem = pxmlDoc->NewKeyValuePairElement(pxmlGroupItem, pszText);

							pxmlDoc->NewKeyValuePairElement(pxmlModuleItem, "moduleId", pDII->astModuleInfo[module_index].moduleId, 16);
							pxmlDoc->NewKeyValuePairElement(pxmlModuleItem, "moduleSize", pDII->astModuleInfo[module_index].moduleSize, 32);
							pxmlDoc->NewKeyValuePairElement(pxmlModuleItem, "moduleVersion", pDII->astModuleInfo[module_index].moduleVersion, 8);

							//计算参数
							blockCount = (S32)(ceil((double)pDII->astModuleInfo[module_index].moduleSize / pDII->blockSize));
							//sprintf_s(pszText, sizeof(pszText), "%d", blockCount);
							pxmlDoc->NewKeyValuePairElement(pxmlModuleItem, "blockCount", blockCount);

							pDC_moduleInfo = &(pDII->astModuleInfo[module_index].u.DC_moduleInfo);
							pxmlDoc->NewKeyValuePairElement(pxmlModuleItem, "moduleName", pDC_moduleInfo->name_descriptor.text_char);
						}
					}

				}

			}
		}
		else
		{ 
			pxmlParentNode->SetAttribute("error", "pDSI = null!");
			rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
		}
#endif
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

//usPID  -- input, DSMCC 的PID；
//pxmlDoc -- output，所生成的DSMCC树通过此参数反馈给调用者
//返回值，程序处理状态码。
int CDB_PsiSiTables::BuildDsmccTree(uint16_t usPID, XMLDocForMpegSyntax* pxmlDoc)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		CDSMCC_UNM*		pDSMCC_DSI;
		CDSMCC_UNM*		pDSMCC_DII;
		DSMCC_DSI_t*	pDSI;
		DSMCC_DII_t*	pDII;

		char pszTitle[128];
		char pszTemp[32];

		pDSMCC_DSI = (CDSMCC_UNM*)QueryDsmccUNM_DSI(usPID);
		if (pDSMCC_DSI != NULL)
		{
			assert(pDSMCC_DSI->GetMessageID() == 0x1006);

			pDSI = &(pDSMCC_DSI->u.m_DSI);

			DVB_SI_NumericCoding2Text_DataBroadcastID(pDSI->data_broadcast_type, pszTemp, sizeof(pszTemp));
			sprintf_s(pszTitle, sizeof(pszTitle), "DSMCC分析 <TS_PID=0x%04X(%d), %s>", usPID, usPID, pszTemp);
			//设置标题

			//根节点
			tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement(pszTitle);
			pxmlDoc->InsertEndChild(pxmlRootNode);

			if (pDSI->data_broadcast_type == 0x0006)		//DC
			{
				DSMCC_BuildDCTree(usPID, pDSI, 0x02, pxmlDoc, pxmlRootNode);
			}
			else if (pDSI->data_broadcast_type == 0x0007)	//OC
			{
				DSMCC_BuildOCTree(usPID, pDSI, pxmlDoc, pxmlRootNode);
			}
		}
		else
		{
			rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
		}

	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

//uiCode  -- input, 高16位表示BAT在数据库中的唯一Key，低16位表示bouquet_id；
//pxmlDoc -- output，所生成的bouquet树通过此参数反馈给调用者
//返回值，程序处理状态码。
int CDB_PsiSiTables::BuildBouquetTree(uint32_t uiCode, XMLDocForMpegSyntax* pxmlDoc)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;
	STREAM_INFO_t stStreamInfoInBAT;
	STREAM_INFO_t stStreamInfoInNIT;
	SERVICE_INFO_t stServiceInfo;

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		char pszStreamID[64];
		char pszFrequency[64];
		char pszSymbolRate[64];
		char pszModulation[64];
		char pszPolarization[64];

		char pszText[128];
		char pszTemp[64];
		char pszTSInfo[256];

		uint16_t usKey = (uiCode & 0xffff0000) >> 16;
		uint16_t usBouquetID = uiCode & 0x0000ffff;

		CBAT* pBAT = (CBAT*)QueryByKey(usKey);
		if (pBAT != NULL)
		{
			pBAT->GetBouquetName(pszTemp, sizeof(pszTemp));
			sprintf_s(pszText, sizeof(pszText), "Bouquet分析 <bouquet_id=0x%04X(%d), bouquet_name=%s>", usBouquetID, usBouquetID, pszTemp);
			//设置标题

			//根节点
			tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement(pszText);
			pxmlDoc->InsertEndChild(pxmlRootNode);

			int nStreamCount = pBAT->GetStreamCount();
			if (nStreamCount > 0)
			{
				for (int stream_index = 0; stream_index < nStreamCount; stream_index++)
				{
					pBAT->GetStreamByIndex(stream_index, &stStreamInfoInBAT);

					uint16_t transport_stream_id = stStreamInfoInBAT.transport_stream_id;
					uint16_t original_network_id = stStreamInfoInBAT.original_network_id;

					CNIT* pNIT = (CNIT*)QueryNITByID(original_network_id);

					pNIT->GetStreamByID(transport_stream_id, &stStreamInfoInNIT);

					sprintf_s(pszStreamID, sizeof(pszStreamID), "ONetID:0x%04X, TsID:0x%04x", stStreamInfoInNIT.original_network_id, stStreamInfoInNIT.transport_stream_id);

					memset(pszTSInfo, 0x00, sizeof(pszTSInfo));
					if (stStreamInfoInNIT.cable_delivery_system_descriptor.descriptor_tag == DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR)
					{
						sprintf_s(pszFrequency, sizeof(pszFrequency), "%x.%04xMHz", (stStreamInfoInNIT.cable_delivery_system_descriptor.frequency & 0xFFFF0000) >> 16, stStreamInfoInNIT.cable_delivery_system_descriptor.frequency & 0x0000FFFF);
						sprintf_s(pszSymbolRate, sizeof(pszSymbolRate), "%x.%04xMsps", (stStreamInfoInNIT.cable_delivery_system_descriptor.symbol_rate & 0xFFFF0000) >> 16, stStreamInfoInNIT.cable_delivery_system_descriptor.symbol_rate & 0x0000FFFF);
						DVB_SI_NumericCoding2Text_CableModulation(stStreamInfoInNIT.cable_delivery_system_descriptor.modulation, pszModulation, sizeof(pszModulation));
						sprintf_s(pszTSInfo, sizeof(pszTSInfo), "%s, 频率:%s, 符号率:%s, 调制:%s", pszStreamID, pszFrequency, pszSymbolRate, pszModulation);
					}
					else if (stStreamInfoInNIT.satellite_delivery_system_descriptor.descriptor_tag == DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR)
					{
						sprintf_s(pszFrequency, sizeof(pszFrequency), "%X.%05XGHz", (stStreamInfoInNIT.satellite_delivery_system_descriptor.frequency & 0xFFF00000) >> 20, stStreamInfoInNIT.satellite_delivery_system_descriptor.frequency & 0x000FFFFF);
						sprintf_s(pszSymbolRate, sizeof(pszSymbolRate), "%X.%04XMsps", (stStreamInfoInNIT.satellite_delivery_system_descriptor.symbol_rate & 0xFFFF0000) >> 16, stStreamInfoInNIT.satellite_delivery_system_descriptor.symbol_rate & 0x0000FFFF);
						DVB_SI_NumericCoding2Text_SatelliteModulationType(stStreamInfoInNIT.satellite_delivery_system_descriptor.modulation_type, pszModulation, sizeof(pszModulation));
						DVB_SI_NumericCoding2Text_SatellitePolarization(stStreamInfoInNIT.satellite_delivery_system_descriptor.polarization, pszPolarization, sizeof(pszPolarization));
						sprintf_s(pszTSInfo, sizeof(pszTSInfo), "%s, 频率:%s, 符号率:%s, 调制:%s, 极化方式:%s", pszStreamID, pszFrequency, pszSymbolRate, pszModulation, pszPolarization);
					}
					else
					{
						sprintf_s(pszTSInfo, sizeof(pszTSInfo), "%s", pszStreamID);
					}

					//sprintf_s(pszText, sizeof(pszText), "传送流 [ONetID=0x%04X(%d) - TsID=0x%04X(%d)]", original_network_id, original_network_id, transport_stream_id, transport_stream_id);
					//tinyxml2::XMLElement* pxmlStreamNode = pxmlDoc->NewTitleElement(pxmlRootNode, pszText);

					//sprintf_s(pszText, sizeof(pszText), "original_network_id = 0x%04X(%d)", original_network_id, original_network_id);
					//pxmlDoc->NewTitleElement(pxmlStreamNode, pszText);

					//sprintf_s(pszText, sizeof(pszText), "transport_stream_id = 0x%04X(%d)", transport_stream_id, transport_stream_id);
					//pxmlDoc->NewTitleElement(pxmlStreamNode, pszText);

					if (stStreamInfoInBAT.service_list_descriptor.descriptor_tag == DVB_SI_SERVICE_LIST_DESCRIPTOR)
					{
						pservice_list_descriptor_t pservice_list_descriptor = &(stStreamInfoInBAT.service_list_descriptor);

						if (pservice_list_descriptor->N > 0)
						{
							CSDT* pSDT = (CSDT*)QuerySDTBy2ID(original_network_id, transport_stream_id);

							//sprintf_s(pszText, sizeof(pszText), "SERVICES[N = %d]", pservice_list_descriptor->N);
							//tinyxml2::XMLElement* pxmlServiceListNode = pxmlDoc->NewTitleElement(pxmlStreamNode, pszText);

							for (int i = 0; i < pservice_list_descriptor->N; i++)
							{
								//uint8_t service_type = pservice_list_descriptor->service_type[i];
								uint16_t service_id = pservice_list_descriptor->service_id[i];

								if (pSDT != NULL)
								{
									pSDT->GetServiceInfoByID(service_id, &stServiceInfo);
								}

								//DVB_SI_NumericCoding2Text_ServiceType(service_type, pszTemp, sizeof(pszTemp));

								sprintf_s(pszText, sizeof(pszText), "%s  [service_id = 0x%04X(%d),  %s]",
									stServiceInfo.pszServiceName, service_id, service_id, pszTSInfo);

								tinyxml2::XMLElement* pxmlServiceNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, pszText);
							}
						}
					}
				}
			}
		}
		else
		{
			rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}
