#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_DSMCC_section.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_DSMCC_Utilities.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_table_id.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/MPEG_DVB_ErrorCode.h"

#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/dsmcc/Mpeg2_DSMCC_UNM.h"

#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "../Include/MiddleWare_DSMCC_Table.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
CDSMCC_UNM::CDSMCC_UNM(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CDSMCC_UNM::~CDSMCC_UNM(void)
{
	Reset();
}

void CDSMCC_UNM::Init(void)
{
	CPVT::Init();

	u.m_DSI.astGroupInfo = NULL;
	u.m_DII.astModuleInfo = NULL;
	m_nMemoryForGroupInfos = 0;
	m_nMemoryForModuleInfos = 0;

	Reset();
}

void CDSMCC_UNM::Reset(void)
{
	if (u.m_DSI.astGroupInfo != NULL)
	{
		free(u.m_DSI.astGroupInfo);
		u.m_DSI.astGroupInfo = NULL;
	}
	m_nMemoryForGroupInfos = 0;

	if (u.m_DII.astModuleInfo != NULL)
	{
		free(u.m_DII.astModuleInfo);
		u.m_DII.astModuleInfo = NULL;
	}
	m_nMemoryForModuleInfos = 0;

	memset(&u.m_DSI, 0x00, sizeof(DSMCC_DSI_t));
	memset(&u.m_DII, 0x00, sizeof(DSMCC_DII_t));

	CPVT::Reset();
}

int CDSMCC_UNM::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;					//0 -- fail
	
	dsmcc_unm_section_t			DSMCC_section;
	DownloadServerInitiate_t*	pDownloadServerInitiate;
	DownloadInfoIndication_t*	pDownloadInfoIndication;

	rtcode = CPVT::AddSection(usPID, pprivate_section);
	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		assert(pprivate_section->table_id == 0x3B);

		rtcode = MPEG2_DSMCC_UNM_DecodeSection(buf, length, &DSMCC_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_usMessageId = DSMCC_section.dsmccMessageHeader.messageId;

			if (m_usMessageId == 0x1006)			//DSI
			{
				pDownloadServerInitiate = &(DSMCC_section.u.DownloadServerInitiate);

				u.m_DSI.data_broadcast_type = pDownloadServerInitiate->data_broadcast_type;

				if (pDownloadServerInitiate->data_broadcast_type == 0x0006)		//DC
				{
					u.m_DSI.NumberOfGroups = pDownloadServerInitiate->u.GroupInfoIndication.NumberOfGroups;
					u.m_DSI.astGroupInfo = (GroupInfo_t*)realloc(u.m_DSI.astGroupInfo, u.m_DSI.NumberOfGroups * sizeof(GroupInfo_t));

					if (u.m_DSI.astGroupInfo != NULL)
					{
						m_nMemoryForGroupInfos = u.m_DSI.NumberOfGroups * sizeof(GroupInfo_t);
						memcpy(u.m_DSI.astGroupInfo, &(pDownloadServerInitiate->u.GroupInfoIndication.GroupInfo), u.m_DSI.NumberOfGroups * sizeof(GroupInfo_t));
					}
				}
				else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)		//OC
				{
					BIOPProfileBody_t* pBIOPProfileBody = pDownloadServerInitiate->u.ServiceGatewayInfo.IOR.pBIOPProfileBodyPortrait;

					BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);
					u.m_DSI.carouselId = pObjectLocation->carouselId;
					u.m_DSI.moduleId_for_srg = pObjectLocation->moduleId;
					u.m_DSI.objectKey_data_for_srg = pObjectLocation->objectKey_data;

					DSM::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);
					u.m_DSI.table_id_extension_for_dii = pConnBinder->Tap[0].transactionId & 0x0000ffff;

				}
			}
			else if (m_usMessageId == 0x1002)			//DII
			{
				pDownloadInfoIndication = &(DSMCC_section.u.DownloadInfoIndication);

				u.m_DII.downloadId = pDownloadInfoIndication->downloadId;
				u.m_DII.blockSize = pDownloadInfoIndication->blockSize;
				u.m_DII.windowSize = pDownloadInfoIndication->windowSize;
				u.m_DII.ackPeriod = pDownloadInfoIndication->ackPeriod;
				u.m_DII.tCDownloadWindow = pDownloadInfoIndication->tCDownloadWindow;
				u.m_DII.tCDownloadScenario = pDownloadInfoIndication->tCDownloadScenario;

				u.m_DII.numberOfModules = pDownloadInfoIndication->N;
				u.m_DII.astModuleInfo = (moduleInfo_t*)realloc(u.m_DII.astModuleInfo, u.m_DII.numberOfModules * sizeof(moduleInfo_t));
				if (u.m_DII.astModuleInfo != NULL)
				{
					m_nMemoryForModuleInfos = u.m_DII.numberOfModules * sizeof(moduleInfo_t);
					memcpy(u.m_DII.astModuleInfo, &(pDownloadInfoIndication->moduleInfo), u.m_DII.numberOfModules * sizeof(moduleInfo_t));
				}
			}

		}
	}

	return rtcode;
}

uint16_t CDSMCC_UNM::GetMessageID(void)
{
	return m_usMessageId;
}

