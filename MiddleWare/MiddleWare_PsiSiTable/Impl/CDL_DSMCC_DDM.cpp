#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_section.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_section_DDM.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section\Include\MPEG_DVB_ErrorCode.h"

#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "../Include/MiddleWare_DSMCC_Table.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
CDSMCC_DDM::CDSMCC_DDM(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CDSMCC_DDM::~CDSMCC_DDM(void)
{
	Reset();
}

void CDSMCC_DDM::Init(void)
{
	int		i;

	CPVT::Init();

	for (i = 0; i < 256; i++)
	{
		m_astBlockInfo[i].length = 0;
		m_astBlockInfo[i].buf = NULL;
	}

	m_nDirMessageCount = 0;
	for (i = 0; i < MAX_DOWNLOAD_OBJS; i++)
	{
		m_pDirectoryMessage[i] = NULL;
	}

	m_nFileMessageCount = 0;
	for (i = 0; i < MAX_DOWNLOAD_OBJS; i++)
	{
		m_pFileMessage[i] = NULL;
	}

	Reset();
}

void CDSMCC_DDM::Reset(void)
{
	int		i;

	for (i = 0; i < m_nSectionCount; i++)
	{
		if (m_astBlockInfo[i].buf != NULL)
		{
			free(m_astBlockInfo[i].buf);
			m_astBlockInfo[i].buf = NULL;

			m_astBlockInfo[i].length = 0;
		}
	}

	m_nModuleSize = 0;

	m_nMemAllocatedForModule = 0;

	for (i = 0; i < m_nDirMessageCount; i++)
	{
		if (m_pDirectoryMessage[i] != NULL)
		{
			free(m_pDirectoryMessage[i]);
			m_pDirectoryMessage[i] = NULL;
		}
	}
	m_nMemAllocatedForDirMessages = 0;
	m_nDirMessageCount = 0;

	for (i = 0; i < m_nFileMessageCount; i++)
	{
		if (m_pFileMessage[i] != NULL)
		{
			if (m_pFileMessage[i]->content_data_byte != NULL)
			{
				free(m_pFileMessage[i]->content_data_byte);
			}

			free(m_pFileMessage[i]);
			m_pFileMessage[i] = NULL;
		}
	}
	m_nMemAllocatedForFileMessages = 0;
	m_nFileMessageCount = 0;

	m_usMessageId = 0xFFFF;

	CPVT::Reset();
}

int CDSMCC_DDM::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;					//0 -- fail
	int i;
	
	dsmcc_ddm_section_t			DSMCC_section;

	U8*							message_buf;
	S32							message_length;

	U8*							module_buf;
	S32							module_length;

	S8							magic[5];
	U8							objectKey_length;
	S8							objectKind_data[4];

	rtcode = CPVT::AddSection(usPID, pprivate_section);
	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = MPEG2_DSMCC_DDM_DecodeSection(buf, length, &DSMCC_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			uint16_t usMessageId = DSMCC_section.dsmccDownloadDataHeader.messageId;
			if (m_usMessageId == 0xFFFF)
			{
				m_usMessageId = usMessageId;
			}
			else
			{
				assert(m_usMessageId == usMessageId);		//从messageId是否相等来检验语法，实际上没有多大必要
			}

			if (usMessageId == 0x1003)			//DDB
			{
				if (m_astBlockInfo[pprivate_section->section_number].buf == NULL)
				{
					m_astBlockInfo[pprivate_section->section_number].buf = (uint8_t*)malloc(DSMCC_section.DownloadDataBlock.N);
					if (m_astBlockInfo[pprivate_section->section_number].buf != NULL)
					{
						m_astBlockInfo[pprivate_section->section_number].length = DSMCC_section.DownloadDataBlock.N;
						memcpy(m_astBlockInfo[pprivate_section->section_number].buf, DSMCC_section.DownloadDataBlock.blockDataByte, DSMCC_section.DownloadDataBlock.N);

						m_nModuleSize += DSMCC_section.DownloadDataBlock.N;
						m_nMemAllocatedForModule += DSMCC_section.DownloadDataBlock.N;
					}
				}

				if (m_bCollectOver)		//m_bCollectOver是基类CPVT的成员变量
				{
					//如果搜集齐了
					module_buf = (uint8_t*)malloc(m_nModuleSize);
					if (module_buf != NULL)
					{
						//把block里面存储的数据按序倒腾到module缓存里面
						module_length = 0;
						for (i = 0; i < m_nSectionCount; i++)
						{
							if (m_astBlockInfo[i].buf != NULL)
							{
								memcpy(module_buf + module_length, m_astBlockInfo[i].buf, m_astBlockInfo[i].length);
								module_length += m_astBlockInfo[i].length;
							}
						}
						assert(module_length == m_nModuleSize);		//此处检测没啥必要，实际上是相等的

						//解析DDB
						memcpy(magic, module_buf, 4);
						magic[4] = '\0';

						if (strcmp(magic, "BIOP") == 0)		//说明是OC
						{
							message_buf = module_buf;
							while (module_length > 0)
							{
								message_length = 12 + ((message_buf[8] << 24) | (message_buf[9] << 16) | (message_buf[10] << 8) | message_buf[11]);

								//究竟是哪个消息，还需要进一步判断
								objectKey_length = message_buf[12];
								memcpy(objectKind_data, message_buf + 17 + objectKey_length, 4);

								if ((strcmp(objectKind_data, "dir") == 0) ||			//directory
									(strcmp(objectKind_data, "srg") == 0))				//service gatway message
								{
									m_pDirectoryMessage[m_nDirMessageCount] = (DirectoryMessage_t*)malloc(sizeof(DirectoryMessage_t));
									if (m_pDirectoryMessage[m_nDirMessageCount] != NULL)
									{
										m_nMemAllocatedForDirMessages += sizeof(DirectoryMessage_t);

										memset(m_pDirectoryMessage[m_nDirMessageCount], 0x00, sizeof(DirectoryMessage_t));
										MPEG2_DSMCC_DecodeDirectoryMessage(message_buf, message_length, m_pDirectoryMessage[m_nDirMessageCount]);

										m_nDirMessageCount++;
									}
								}
								else if (strcmp(objectKind_data, "fil") == 0)
								{
									m_pFileMessage[m_nFileMessageCount] = (FileMessage_t*)malloc(sizeof(FileMessage_t));
									if (m_pFileMessage[m_nFileMessageCount] != NULL)
									{
										memset(m_pFileMessage[m_nFileMessageCount], 0x00, sizeof(FileMessage_t));
										MPEG2_DSMCC_DecodeFileMessage(message_buf, message_length, m_pFileMessage[m_nFileMessageCount]);

										m_nMemAllocatedForFileMessages += (sizeof(FileMessage_t) + m_pFileMessage[m_nFileMessageCount]->content_length);

										m_nFileMessageCount++;
									}
								}

								message_buf += message_length;
								module_length -= message_length;
							}
						}

						//释放module_buf
						free(module_buf);
					}
				}
			}
			else
			{
				assert(0);
			}
		}
	}

	return rtcode;
}

