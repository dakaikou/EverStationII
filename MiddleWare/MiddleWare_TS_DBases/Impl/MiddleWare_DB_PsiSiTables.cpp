#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <Shlwapi.h>

#include "../Include/MiddleWare_DB_PsiSiTables.h"
#include "../Include/MiddleWare_DB_ErrorCode.h"

#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_SI_Utilities.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_DSMCC_Utilities.h"

#include "libs_MPEG&DVB\MPEG_DVB_Section\xml\Include\dsmcc\Mpeg2_DSMCC_DDM_XML.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\xml\Include\dsmcc\Mpeg2_DSMCC_BIOP_XML.h"

#include "libs_Utilities\Include\XStream_Utilities.h"

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

int CDB_PsiSiTables::DSMCC_BuildOCTree(uint16_t PID, DSMCC_DSI_t* pDSI, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
#if DEBUG_DSMCC
		if (pDSI != NULL)			//DSI
		{
			CDSMCC_UNM*			pDSMCC_DII;
			CDSMCC_DDM*			pDSMCC_DDM;

			DSMCC_DII_t*				pDII;
			BIOP::DirectoryMessage_t*			pDirectoryMessage;
			BIOP::FileMessage_t*				pFileMessage;
			BIOP::Binding_t*					pBindings;
			BIOP::IOR_t*						pIOR;
			BIOP::BIOPProfileBody_t*			pBIOPProfileBody;
			//LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
			BIOP::ObjectLocation_t*		pObjectLocation;
			BIOP::ConnBinder_t*			pConnBinder;

			char			pszText[256];
			char			pszTemp[64];
			int				module_index;
			int				binding_index;
			int				blockCount;
			int				component_index;
			int				profile_index;
			int				object_index;
			//int				i;

			XMLElement*	hSessionItem;
			XMLElement*	hSrgItem;
			XMLElement*	hDirItem;
			XMLElement*	hChildItem;
			XMLElement*	hModuleItem;
			XMLElement*	hBindingItem;
			XMLElement*	hNameComponentItem;
			XMLElement*	hIORItem;
			XMLElement*	hIOP_taggedProfileItem;

			hSessionItem = pxmlDoc->NewBranchElement(pxmlParentNode, "下载会话（DSI/DII/DDB）", NULL);

			XMLElement* hDSIItem = pxmlDoc->NewBranchElement(hSessionItem, "DownloadServerInitiate(DSI) - 获知业务网关的IOR", NULL);

			//从DSI获得的信息，包括轮播ID、srg所在的模块、DII的table_id_extension
			hChildItem = pxmlDoc->NewElementForBits(hDSIItem, "carouselId", pDSI->carouselId, 32, NULL, NULL);
			hChildItem = pxmlDoc->NewElementForBits(hDSIItem, "moduleId_for_srg", pDSI->moduleId_for_srg, 16, NULL, NULL);
			hChildItem = pxmlDoc->NewElementForBits(hDSIItem, "objectKey_data_for_srg", pDSI->objectKey_data_for_srg, 32, NULL, NULL);

			hChildItem = pxmlDoc->NewElementForBits(hDSIItem, "table_id_extension_for_dii", pDSI->table_id_extension_for_dii, 16, NULL, NULL);

			XMLElement* hDIIItem = pxmlDoc->NewBranchElement(hSessionItem, "DownloadInfoIndication(DII) - 获知模块的传输参数", NULL);
			//从DII获得的信息，包括下载ID、模块数量、模块ID
			//模块ID同时也是DDB的table_id_extension
			pDSMCC_DII = (CDSMCC_UNM*)QueryBy3ID(PID, TABLE_ID_DSMCC_UNM, pDSI->table_id_extension_for_dii);
			if (pDSMCC_DII != NULL)
			{
				pDII = &(pDSMCC_DII->u.m_DII);

				hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "downloadId", pDII->downloadId, 32, NULL, NULL);
				hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "blockSize", pDII->blockSize, 16, NULL, NULL);
				//hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "windowSize", pDII->windowSize, 8, NULL, NULL);
				//hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "ackPeriod", pDII->ackPeriod, 8, NULL, NULL);
				//hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "tCDownloadWindow", pDII->tCDownloadWindow, 32, NULL, NULL);
				//hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "tCDownloadScenario", pDII->tCDownloadScenario, 32, NULL, NULL);

				hChildItem = pxmlDoc->NewElementForBits(hDIIItem, "numberOfModules", pDII->numberOfModules, -1, NULL, NULL);

				for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
				{
					sprintf_s(pszText, sizeof(pszText), "MODULE[%d] - ID=0x%04X)", module_index, pDII->astModuleInfo[module_index].moduleId);
					hModuleItem = pxmlDoc->NewBranchElement(hDIIItem, pszText, NULL);

					hChildItem = pxmlDoc->NewElementForBits(hModuleItem, "moduleId", pDII->astModuleInfo[module_index].moduleId, 16, NULL, NULL);
					hChildItem = pxmlDoc->NewElementForBits(hModuleItem, "moduleSize", pDII->astModuleInfo[module_index].moduleSize, 32, NULL, NULL);
					hChildItem = pxmlDoc->NewElementForBits(hModuleItem, "moduleVersion", pDII->astModuleInfo[module_index].moduleVersion, 8, NULL, NULL);

					//计算参数
					blockCount = (int)(ceil((double)pDII->astModuleInfo[module_index].moduleSize / pDII->blockSize));
					hChildItem = pxmlDoc->NewElementForBits(hModuleItem, "blockCount", blockCount, -1, NULL, NULL);

					XMLElement* hDDBItem = pxmlDoc->NewBranchElement(hModuleItem, "DownloadDataBlock(DDB)", NULL);

					////DDB信息
					pDSMCC_DDM = (CDSMCC_DDM*)QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, pDII->astModuleInfo[module_index].moduleId);
					if (pDSMCC_DDM != NULL)
					{
						//有些是srg或者directory消息，有些是fil消息，最好能区分一下，而不是每个模块都去遍历一下
						for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index++)
						{
							pDirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];

							if (pDirectoryMessage != NULL)
							{
								MPEG2_DSMCC_BIOP_PresentDirectMessage_to_xml(pxmlDoc, hDDBItem, pDirectoryMessage);
							}
						}

						for (object_index = 0; object_index < pDSMCC_DDM->m_nFileMessageCount; object_index++)
						{
							pFileMessage = pDSMCC_DDM->m_pFileMessage[object_index];
							if (pFileMessage != NULL)
							{
								MPEG2_DSMCC_BIOP_PresentFileMessage_to_xml(pxmlDoc, hDDBItem, pFileMessage);
							}
						}
					}
				}
			}

			hDirItem = pxmlDoc->NewBranchElement(pxmlParentNode, "下载文件目录结构", NULL);
			OC_BuildDirectory(PID, pxmlDoc, hDirItem, pDSI->moduleId_for_srg, pDSI->objectKey_data_for_srg);
		}

#endif
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

//递归函数，如果当前路径是个子目录，则递归调用本函数
int CDB_PsiSiTables::OC_DownloadDirectoryAndFiles(uint16_t PID, uint16_t moduleId_for_srg, uint32_t objectKey_data, char* pszRootPath)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	CDSMCC_DDM*					pDSMCC_DDM;
	BIOP::DirectoryMessage_t*	pDirectoryMessage;
	BIOP::Binding_t*			pBindings;
	BIOP::Name_t*				pName;
	BIOP::ObjectLocation_t*		pObjectLocation;

	unsigned short				sub_moduleId;
	unsigned int				sub_objectKey_data;

	char			pszPath[256];

	pDSMCC_DDM = (CDSMCC_DDM*)QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, moduleId_for_srg);
	if (pDSMCC_DDM != NULL)
	{
		for (int dir_object_index = 0; dir_object_index < pDSMCC_DDM->m_nDirMessageCount; dir_object_index++)
		{
			pDirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[dir_object_index];
			if (pDirectoryMessage != NULL)
			{
				if (pDirectoryMessage->objectKey_data == objectKey_data)
				{
					for (int dir_binding_index = 0; dir_binding_index < pDirectoryMessage->bindings_count; dir_binding_index++)
					{
						pBindings = pDirectoryMessage->bindings + dir_binding_index;
						pName = &(pBindings->Name);

						//sprintf_s(pszText, sizeof(pszText), "%s:%s", pName->kind_data_byte[0], pName->id_data_byte[0]);
						sprintf_s(pszPath, sizeof(pszPath), "%s\\%s", pszRootPath, pName->nameComponents[0].id_data_byte);

						if (strcmp(pBindings->IOR.type_id_byte, "dir") == 0)	//如果是目录
						{
							BuildDirectory(pszPath);

							BIOP::BIOPProfileBody_t* pBIOPProfileBody = &(pBindings->IOR.BIOPProfileBody);
							pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

							sub_moduleId = pObjectLocation->moduleId;
							sub_objectKey_data = pObjectLocation->objectKey_data;

							OC_DownloadDirectoryAndFiles(PID, sub_moduleId, sub_objectKey_data, pszPath);
						}
						else if (strcmp(pBindings->IOR.type_id_byte, "fil") == 0)	//如果是文件
						{
							FILE* fp = NULL;
							fopen_s(&fp, pszPath, "wb");
							if (fp != NULL)
							{
								BIOP::BIOPProfileBody_t* pBIOPProfileBody = &(pBindings->IOR.BIOPProfileBody);
								pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

								uint16_t moduleId_for_ddb = pObjectLocation->moduleId;
								uint32_t objectKey_data = pObjectLocation->objectKey_data;

								pDSMCC_DDM = (CDSMCC_DDM*)QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, moduleId_for_ddb);
								if (pDSMCC_DDM != NULL)
								{
									for (int fil_object_index = 0; fil_object_index < pDSMCC_DDM->m_nFileMessageCount; fil_object_index++)
									{
										BIOP::FileMessage_t* pFileMessage = pDSMCC_DDM->m_pFileMessage[fil_object_index];

										if (pFileMessage != NULL)
										{
											if (pFileMessage->objectKey_data == objectKey_data)
											{
												fwrite(pFileMessage->content_data_byte, sizeof(uint8_t), pFileMessage->content_length, fp);
												break;
											}
										}
									}
								}

								fclose(fp);
							}
						}
					}

					break;
				}
			}
		}
	}

	return rtcode;
}

//递归函数，如果当前路径是个子目录，则递归调用本函数
int CDB_PsiSiTables::OC_BuildDirectory(uint16_t PID, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, uint16_t moduleId_for_srg, uint32_t objectKey_data)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	CDSMCC_DDM*					pDSMCC_DDM;
	BIOP::DirectoryMessage_t*	pDirectoryMessage;
	BIOP::Binding_t*			pBindings;
	BIOP::Name_t*				pName;
	BIOP::ObjectLocation_t*		pObjectLocation;

	int							ddm_table_index;
	int							binding_index;
	int							object_index;
	unsigned short				sub_moduleId;
	unsigned int				sub_objectKey_data;

	char			pszText[256];

	pDSMCC_DDM = (CDSMCC_DDM*)QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, moduleId_for_srg);
	if (pDSMCC_DDM != NULL)
	{
		for (object_index = 0; object_index < pDSMCC_DDM->m_nDirMessageCount; object_index++)
		{
			pDirectoryMessage = pDSMCC_DDM->m_pDirectoryMessage[object_index];
			if (pDirectoryMessage != NULL)
			{
				if (pDirectoryMessage->objectKey_data == objectKey_data)
				{
					for (binding_index = 0; binding_index < pDirectoryMessage->bindings_count; binding_index++)
					{
						pBindings = pDirectoryMessage->bindings + binding_index;
						pName = &(pBindings->Name);

						sprintf_s(pszText, sizeof(pszText), "%s:%s", pName->nameComponents[0].kind_data_byte, pName->nameComponents[0].id_data_byte);
						XMLElement* pxmlChildNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszText, NULL);

						if (strcmp(pBindings->IOR.type_id_byte, "dir") == 0)
						{
							BIOP::BIOPProfileBody_t* pBIOPProfileBody = &(pBindings->IOR.BIOPProfileBody);
							pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

							sub_moduleId = pObjectLocation->moduleId;
							sub_objectKey_data = pObjectLocation->objectKey_data;

							OC_BuildDirectory(PID, pxmlDoc, pxmlChildNode, sub_moduleId, sub_objectKey_data);
						}
					}

					break;
				}
			}
		}
	}

	return rtcode;
}

int CDB_PsiSiTables::DSMCC_BuildDCTree(uint16_t PID, DSMCC_DSI_t* pDSI, uint8_t carousel_type_id, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pxmlDoc != NULL)
	{
#if DEBUG_DSMCC
		if (pDSI != NULL)
		{
			CDSMCC_UNM*		pDSMCC_DII;
			DSMCC_DII_t*	pDII;

			char			pszField[64];
			int				group_index;
			int				module_index;
			int				blockCount;

			XMLElement*	pxmlDsiItem;
			XMLElement*	pxmlChildItem;
			XMLElement*	pxmlGroupItem;
			XMLElement*	pxmlModuleItem;

			if (carousel_type_id == 0x02)			//2层数据轮播
			{
				pxmlDsiItem = pxmlDoc->NewBranchElement(pxmlParentNode, "SuperGroup()", NULL);

				pxmlDoc->NewElementForBits(pxmlDsiItem, "NumberOfGroups", pDSI->NumberOfGroups, 16, "uimsbf", NULL);

				for (group_index = 0; group_index < pDSI->NumberOfGroups; group_index++)
				{
					sprintf_s(pszField, sizeof(pszField), "GROUP[%d]()", group_index);
					pxmlGroupItem = pxmlDoc->NewBranchElement(pxmlDsiItem, pszField, NULL);

					pxmlDoc->NewElementForBits(pxmlGroupItem, "GroupId", pDSI->astGroupInfo[group_index].GroupId, 32, "uimsbf", NULL);
					pxmlDoc->NewElementForBits(pxmlGroupItem, "GroupSize", pDSI->astGroupInfo[group_index].GroupSize, 32, "uimsbf", NULL);

					pxmlDoc->NewBranchElement(pxmlGroupItem, "GroupName", pDSI->astGroupInfo[group_index].name_descriptor.text_char);

					pDSMCC_DII = (CDSMCC_UNM*)QueryBy3ID(PID, TABLE_ID_DSMCC_UNM, (pDSI->astGroupInfo[group_index].GroupId & 0x0000ffff));

					if (pDSMCC_DII != NULL)
					{
						pDII = &(pDSMCC_DII->u.m_DII);

						pxmlDoc->NewElementForBits(pxmlGroupItem, "downloadId", pDII->downloadId, 32, "uimsbf", NULL);
						pxmlDoc->NewElementForBits(pxmlGroupItem, "blockSize", pDII->blockSize, 16, "uimsbf", NULL);
						pxmlDoc->NewElementForBits(pxmlGroupItem, "windowSize", pDII->windowSize, 8, "uimsbf", NULL);
						pxmlDoc->NewElementForBits(pxmlGroupItem, "ackPeriod", pDII->ackPeriod, 8, "uimsbf", NULL);
						pxmlDoc->NewElementForBits(pxmlGroupItem, "tCDownloadWindow", pDII->tCDownloadWindow, 32, "uimsbf", NULL);
						pxmlDoc->NewElementForBits(pxmlGroupItem, "tCDownloadScenario", pDII->tCDownloadScenario, 32, "uimsbf", NULL);

						pxmlDoc->NewElementForBits(pxmlGroupItem, "numberOfModules", pDII->numberOfModules, 16, "uimsbf", NULL);

						for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
						{
							ModuleInfo_t* pmoduleInfo = pDII->astModuleInfo + module_index;
							sprintf_s(pszField, sizeof(pszField), "MODULE(%d - 0x%04X)", module_index, pmoduleInfo->moduleId);
							pxmlModuleItem = pxmlDoc->NewBranchElement(pxmlGroupItem, pszField, NULL);

							pxmlDoc->NewElementForBits(pxmlModuleItem, "moduleId", pmoduleInfo->moduleId, 16, "uimsbf", NULL);
							pxmlDoc->NewElementForBits(pxmlModuleItem, "moduleSize", pmoduleInfo->moduleSize, 32, "uimsbf", NULL);
							pxmlDoc->NewElementForBits(pxmlModuleItem, "moduleVersion", pmoduleInfo->moduleVersion, 8, "uimsbf", NULL);

							//计算参数
							blockCount = (S32)(ceil((double)pmoduleInfo->moduleSize / pDII->blockSize));
							pxmlDoc->NewElementForBits(pxmlModuleItem, "blockCount", blockCount, -1, "uimsbf", NULL);

							if (strlen(pmoduleInfo->moduleName) > 0)
							{
								pxmlDoc->NewElementForString(pxmlModuleItem, "moduleName", pmoduleInfo->moduleName);
							}
							else
							{
								pxmlDoc->NewElementForString(pxmlModuleItem, "moduleName", "Unknown");
							}
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

int CDB_PsiSiTables::DSMCC_DownloadDCTree(uint16_t PID, DSMCC_DSI_t* pDSI, uint8_t carousel_type_id, char* pszRootPath)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	char pszGroupDir[MAX_PATH];

	if ((pDSI != NULL) && (pszRootPath != NULL))
	{
#if DEBUG_DSMCC
		CDSMCC_UNM*		pDSMCC_DII;
		DSMCC_DII_t*	pDII;

		//DC_ModuleInfo_t*	pDC_moduleInfo;

		int				group_index;
		int				module_index;
		//int				blockCount;
		//char			pszField[64];

		if (carousel_type_id == 0x02)			//2层数据轮播
		{
			for (group_index = 0; group_index < pDSI->NumberOfGroups; group_index++)
			{
				sprintf_s(pszGroupDir, sizeof(pszGroupDir), "%s\\%s", pszRootPath, pDSI->astGroupInfo[group_index].name_descriptor.text_char);
				BuildDirectory(pszGroupDir);

				pDSMCC_DII = (CDSMCC_UNM*)QueryBy3ID(PID, TABLE_ID_DSMCC_UNM, (pDSI->astGroupInfo[group_index].GroupId & 0x0000ffff));
				if (pDSMCC_DII != NULL)
				{
					pDII = &(pDSMCC_DII->u.m_DII);

					for (module_index = 0; module_index < pDII->numberOfModules; module_index++)
					{
						ModuleInfo_t* pmoduleInfo = pDII->astModuleInfo + module_index;
						CDSMCC_DDM* pDSMCC_DDM = (CDSMCC_DDM*)QueryBy3ID(PID, TABLE_ID_DSMCC_DDM, pmoduleInfo->moduleId);
						pDSMCC_DDM->SaveModuleBufToDisk(pszGroupDir, pmoduleInfo->moduleName);
					}
				}
			}
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
int CDB_PsiSiTables::BuildDsmccTree(uint16_t usPID, HALForXMLDoc* pxmlDoc)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		CDSMCC_UNM*		pDSMCC_DSI;
		DSMCC_DSI_t*	pDSI;

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
			XMLElement* pxmlRootNode = pxmlDoc->NewElement(pszTitle);
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

int CDB_PsiSiTables::DownloadDsmccTree(uint16_t usPID, char* pszRootPath)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	if (pszRootPath != NULL)
	{
		CDSMCC_UNM*		pDSMCC_DSI;
		DSMCC_DSI_t*	pDSI;

		pDSMCC_DSI = (CDSMCC_UNM*)QueryDsmccUNM_DSI(usPID);
		if (pDSMCC_DSI != NULL)
		{
			assert(pDSMCC_DSI->GetMessageID() == 0x1006);

			pDSI = &(pDSMCC_DSI->u.m_DSI);

			if (pDSI->data_broadcast_type == 0x0006)		//DC
			{
				DSMCC_DownloadDCTree(usPID, pDSI, 0x02, pszRootPath);
			}
			else if (pDSI->data_broadcast_type == 0x0007)	//OC
			{
				OC_DownloadDirectoryAndFiles(usPID, pDSI->moduleId_for_srg, pDSI->objectKey_data_for_srg, pszRootPath);
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
int CDB_PsiSiTables::BuildBouquetTree(uint32_t uiCode, HALForXMLDoc* pxmlDoc)
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
			XMLElement* pxmlRootNode = pxmlDoc->NewElement(pszText);
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

								//XMLElement* pxmlServiceNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszText, NULL);
								pxmlDoc->NewElementForString(pxmlRootNode, pszText, NULL);
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
