#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_section.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/MPEG_DVB_ErrorCode.h"

#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM.h"

#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "../Include/MiddleWare_DSMCC_Table.h"

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
	
	dsmcc_section_t				dsmcc_section;

	rtcode = CPVT::AddSection(usPID, pprivate_section);
	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		assert(pprivate_section->table_id == 0x3B);

		rtcode = MPEG2_DSMCC_DecodeSection(buf, length, &dsmcc_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_usMessageId = dsmcc_section.dsmccMessageHeader.messageId;

			if (m_usMessageId == 0x1006)			//DSI
			{
				DownloadServerInitiate_t downloadServerInitiate;
				MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(dsmcc_section.dsmccMessagePayloadBuf, dsmcc_section.dsmccMessagePayloadLength, &downloadServerInitiate);

				u.m_DSI.data_broadcast_type = downloadServerInitiate.data_broadcast_type;

				if (downloadServerInitiate.data_broadcast_type == 0x0006)		//DC
				{
					GroupInfoIndication_t groupInfoIndication;
					MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(downloadServerInitiate.privateDataByte, downloadServerInitiate.privateDataLength, &groupInfoIndication);
					u.m_DSI.NumberOfGroups = groupInfoIndication.NumberOfGroups;
					u.m_DSI.astGroupInfo = (GroupInfo_t*)realloc(u.m_DSI.astGroupInfo, u.m_DSI.NumberOfGroups * sizeof(GroupInfo_t));

					if (u.m_DSI.astGroupInfo != NULL)
					{
						m_nMemoryForGroupInfos = u.m_DSI.NumberOfGroups * sizeof(GroupInfo_t);
						memcpy(u.m_DSI.astGroupInfo, &(groupInfoIndication.GroupInfo), u.m_DSI.NumberOfGroups * sizeof(GroupInfo_t));
					}
				}
				else if (downloadServerInitiate.data_broadcast_type == 0x0007)		//OC
				{
					ServiceGatewayInfo_t serviceGatewayInfo;
					MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(downloadServerInitiate.privateDataByte, downloadServerInitiate.privateDataLength, &serviceGatewayInfo);
					
					IOP::IOR_t* pIOR = &(serviceGatewayInfo.IOR);

					for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
					{
						IOP::TaggedProfile_t* ptaggedProfile = pIOR->taggedProfiles + profile_index;

						if (ptaggedProfile->profileId_tag == 0x49534F06)		//BIOPProfileBody
						{
							BIOPProfileBody_t* pBIOPProfileBody = &(pIOR->BIOPProfileBody);
							MPEG2_DSMCC_IOP_DecodeBIOPProfileBody(ptaggedProfile->profileId_tag, ptaggedProfile->profile_data_length, ptaggedProfile->profile_data_byte, pBIOPProfileBody);

							BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);
							u.m_DSI.carouselId = pObjectLocation->carouselId;
							u.m_DSI.moduleId_for_srg = pObjectLocation->moduleId;
							u.m_DSI.objectKey_data_for_srg = pObjectLocation->objectKey_data;

							DSM::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);
							u.m_DSI.table_id_extension_for_dii = pConnBinder->Tap[0].transactionId & 0x0000ffff;

							break;
						}
					}
				}
			}
			else if (m_usMessageId == 0x1002)			//DII
			{
				DownloadInfoIndication_t downloadInfoIndication;
				MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(dsmcc_section.dsmccMessagePayloadBuf, dsmcc_section.dsmccMessagePayloadLength, &downloadInfoIndication);

				u.m_DII.downloadId = downloadInfoIndication.downloadId;
				u.m_DII.blockSize = downloadInfoIndication.blockSize;
				u.m_DII.windowSize = downloadInfoIndication.windowSize;
				u.m_DII.ackPeriod = downloadInfoIndication.ackPeriod;
				u.m_DII.tCDownloadWindow = downloadInfoIndication.tCDownloadWindow;
				u.m_DII.tCDownloadScenario = downloadInfoIndication.tCDownloadScenario;

				u.m_DII.numberOfModules = downloadInfoIndication.numberOfModules;
				u.m_DII.astModuleInfo = (ModuleInfo_t*)realloc(u.m_DII.astModuleInfo, u.m_DII.numberOfModules * sizeof(ModuleInfo_t));
				if (u.m_DII.astModuleInfo != NULL)
				{
					m_nMemoryForModuleInfos = u.m_DII.numberOfModules * sizeof(ModuleInfo_t);
					memcpy(u.m_DII.astModuleInfo, &(downloadInfoIndication.ModuleInfo), u.m_DII.numberOfModules * sizeof(ModuleInfo_t));
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

