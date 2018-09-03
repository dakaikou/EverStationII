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

	m_pucBlockBuf = NULL;
	m_pnBlockLength = NULL;

	m_nDirMessageCount = 0;
	for (i = 0; i < 128; i++)
	{
		m_pDirectoryMessage[i] = NULL;
	}

	m_nFileMessageCount = 0;
	for (i = 0; i < 128; i++)
	{
		m_pFileMessage[i] = NULL;
	}

	Reset();
}

void CDSMCC_DDM::Reset(void)
{
	int		i;

	if (m_pnBlockLength != NULL)
	{
		free(m_pnBlockLength);
		m_pnBlockLength = NULL;
	}

	if (m_pucBlockBuf != NULL)
	{
		for (i = 0; i < m_nSectionCount; i++)
		{
			if (m_pucBlockBuf[i] != NULL)
			{
				free(m_pucBlockBuf[i]);
				m_pucBlockBuf[i] = NULL;
			}
		}

		free(m_pucBlockBuf);
		m_pucBlockBuf = NULL;
	}

	m_nMemoryForBlockBuf = 0;

	for (i = 0; i < m_nDirMessageCount; i++)
	{
		if (m_pDirectoryMessage[i] != NULL)
		{
			free(m_pDirectoryMessage[i]);
			m_pDirectoryMessage[i] = NULL;
		}
	}
	m_nMemoryForDirMessages = 0;
	m_nDirMessageCount = 0;

	for (i = 0; i < m_nFileMessageCount; i++)
	{
		if (m_pFileMessage[i] != NULL)
		{
			free(m_pFileMessage[i]);
			m_pFileMessage[i] = NULL;
		}
	}
	m_nMemoryForFileMessages = 0;
	m_nFileMessageCount = 0;

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
			m_usMessageId = DSMCC_section.dsmccDownloadDataHeader.messageId;

			if (m_usMessageId == 0x1006)			//DSI
			{
				//assert(0);
			}
			else if (m_usMessageId == 0x1002)			//DII
			{
				//assert(0);
			}
			else if (m_usMessageId == 0x1003)			//DDB
			{
				if (m_pnBlockLength == NULL)
				{
					m_pnBlockLength = (S32*)calloc(m_nSectionCount, sizeof(S32));

					if (m_pnBlockLength != NULL)
					{
						m_nMemoryForBlockBuf = m_nSectionCount * sizeof(S32);
						for (i = 0; i < m_nSectionCount; i++)
						{
							m_pnBlockLength[i] = 0;
						}
					}
					m_nModuleSize = 0;
				}

				if (m_pucBlockBuf == NULL)
				{
					m_pucBlockBuf = (U8**)calloc(m_nSectionCount, sizeof(U8*));
					if (m_pucBlockBuf != NULL)
					{
						m_nMemoryForBlockBuf += m_nSectionCount * sizeof(U8*);
						for (i = 0; i < m_nSectionCount; i++)
						{
							m_pucBlockBuf[i] = NULL;
						}
					}
				}

				m_pnBlockLength[pprivate_section->section_number] = DSMCC_section.DownloadDataBlock.N;
				m_nModuleSize += m_pnBlockLength[pprivate_section->section_number];

				m_pucBlockBuf[pprivate_section->section_number] = (U8*)malloc(m_pnBlockLength[pprivate_section->section_number] * sizeof(U8));
				if (m_pucBlockBuf[pprivate_section->section_number] != NULL)
				{
					m_nMemoryForBlockBuf += m_pnBlockLength[pprivate_section->section_number] * sizeof(U8);

					memcpy(m_pucBlockBuf[pprivate_section->section_number], DSMCC_section.DownloadDataBlock.blockDataByte, m_pnBlockLength[pprivate_section->section_number]);
				}

				if (m_bCollectOver)
				{
					//如果搜集齐了

					module_buf = (U8*)malloc(m_nModuleSize * sizeof(U8));
					if (module_buf != NULL)
					{
						module_length = 0;
						for (i = 0; i < m_nSectionCount; i++)
						{
							if (m_pucBlockBuf[i] != NULL)
							{
								memcpy(module_buf + module_length, m_pucBlockBuf[i], m_pnBlockLength[i]);
								module_length += m_pnBlockLength[i];
							}
						}

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

								if ((strcmp(objectKind_data, "dir") == 0) ||
									(strcmp(objectKind_data, "srg") == 0))
								{
									m_pDirectoryMessage[m_nDirMessageCount] = (DirectoryMessage_t*)malloc(sizeof(DirectoryMessage_t));
									if (m_pDirectoryMessage[m_nDirMessageCount] != NULL)
									{
										m_nMemoryForDirMessages += sizeof(DirectoryMessage_t);

										memset(m_pDirectoryMessage[m_nDirMessageCount], 0x00, sizeof(DirectoryMessage_t));
										MPEG2_DSMCC_DecodeDirectoryMessage(message_buf, message_length, m_pDirectoryMessage[m_nDirMessageCount]);

										m_nDirMessageCount++;
									}
								}
								else if (strcmp(objectKind_data, "fil") == 0)
								{
									m_pFileMessage[m_nFileMessageCount] = (BIOP_FileMessage_t*)malloc(sizeof(BIOP_FileMessage_t));
									if (m_pFileMessage[m_nFileMessageCount] != NULL)
									{
										m_nMemoryForFileMessages += sizeof(BIOP_FileMessage_t);

										memset(m_pFileMessage[m_nFileMessageCount], 0x00, sizeof(BIOP_FileMessage_t));
										MPEG2_DSMCC_DecodeBIOPFileMessage(message_buf, message_length, m_pFileMessage[m_nFileMessageCount]);

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

					if (m_pnBlockLength != NULL)
					{
						free(m_pnBlockLength);
						m_pnBlockLength = NULL;
					}

					if (m_pucBlockBuf != NULL)
					{
						for (i = 0; i < m_nSectionCount; i++)
						{
							if (m_pucBlockBuf[i] != NULL)
							{
								free(m_pucBlockBuf[i]);
								m_pucBlockBuf[i] = NULL;
							}
						}

						free(m_pucBlockBuf);
						m_pucBlockBuf = NULL;
					}
				}
			}
		}
	}

	return rtcode;
}

