#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"

#include "libs_Mpeg&DVB/Mpeg_TSPacket/Include/Mpeg2_TS_packet.h"
#include "libs_Mpeg&DVB/Mpeg_TSPacket/Include/Mpeg2_TS_Utilities.h"
#include "libs_Mpeg&DVB/Mpeg_TSPacket\Include\Mpeg2_TS_PID.h"
#include "libs_Mpeg&DVB/Mpeg_PESPacket/Include/MPEG_stream_id.h"
#include "libs_Mpeg&DVB/Mpeg_PESPacket/Include/MPEG_PES_Utilities.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\Mpeg2_table_id.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_StreamType.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_PSI_Utilities.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/MPEG_DVB_Common.h"

#include "../Include/MiddleWare_DB_TSPackets.h"
#include "../Include/MiddleWare_DB_ErrorCode.h"

#include "thirdparty_libs/SQLite3/Include/sqlite3.h"  

int user_callback(void* param, int nColCount, char** pColValue, char** pColName)
{
	char pszMsg[256];
	//CString s;
	for (int i = 0; i<nColCount; i++)
	{
		sprintf_s(pszMsg, sizeof(pszMsg), "<col=%d>%s:%s\n", i, pColName[i], pColValue[i]);
	//	s += pColName[i];
	//	s += ":";
	//	s += pColValue[i];
	//	s += "\t";
	}
	//s += "\n";
	//TRACE("user_callback: %s", s.GetBuffer());
	//cout << s << endl;
	return 0;
}

int sqlite3_test()
{
	//char	pszExeFile[MAX_PATH];
	//char	pszDbPath[MAX_PATH];
	char	pszDbFile[MAX_PATH];
	//int		len;

	//GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	//len = GetModulePathLength(pszExeFile);
	//assert((len > 0) && (len < MAX_PATH));

	//memcpy(pszDbPath, pszExeFile, len);
	//pszDbPath[len] = '\0';
	//	sprintf_s(pszDbFile, sizeof(pszDbFile), "%s\\test.db", pszDbPath);
	sprintf_s(pszDbFile, sizeof(pszDbFile), "e:\\EverStationII.db");

	char strSql[256];

	sqlite3* pdb;
	int nResult = sqlite3_open(pszDbFile, &pdb);
	if (nResult != SQLITE_OK)
	{
		//TRACE("打开数据库失败：%s\n", sqlite3_errmsg(pdb));
		//cout << "打开数据库失败：" << sqlite3_errmsg(db) << endl;

		return 0;
	}
	else
	{
		//TRACE("数据库打开成功!\n");
		//cout << "数据库打开成功" << endl;
	}

	char* errmsg;
	nResult = sqlite3_exec(pdb, "create table TSPACKETS(id integer primary key autoincrement,name varchar(100))", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("创建表失败(errmsg)：%s\n", errmsg);
		//TRACE("创建表失败(sqlite3_errmsg)：%s\n", sqlite3_errmsg(pdb));
		sqlite3_free(errmsg);
	}

	//CString strSql;
	//for (int i = 0; i<100; i++)
	//{
	//	strSql += "insert into fuck values(null,'caonima');";
	//}
	//TRACE(strSql);

	//cout << strSql << endl;
	//nResult = sqlite3_exec(pdb, strSql.GetBuffer(), NULL, NULL, &errmsg);
	nResult = sqlite3_exec(pdb, "insert into TSPACKETS values(null,'packets');", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		//TRACE("插入数据失败(errmsg)：%s", errmsg);
		//TRACE("插入数据失败(sqlite3_errmsg)：%s", sqlite3_errmsg(pdb));
	}

	strcpy_s(strSql, sizeof(strSql), "select * from TSPACKETS");
	nResult = sqlite3_exec(pdb, strSql, user_callback, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("读数据表(errmsg)：%s\n", errmsg);
		//cout << errmsg << endl;
		sqlite3_free(errmsg);
		return 0;
	}

	sqlite3_close(pdb);
	return 0;
}


CDB_TSPackets::CDB_TSPackets()
{
	int	i;

	for (i = 0; i < MAX_PID_COUNT; i++)
	{
		m_pRecords[i] = NULL;
	}

	Reset();

	//sqlite3_test();
}

CDB_TSPackets::~CDB_TSPackets()
{
	Reset();
}

void CDB_TSPackets::Reset(void)
{
	int			i;

	m_nPIDCount = 0;
	m_total_packet_count = 0;
//	strcpy(m_filename, "");
	m_packet_length = 0;
	m_total_bitrate_from_software = 0;
	m_total_bitrate_from_hardware = 1000000000;

	for (i = 0; i < MAX_PID_COUNT; i++)
	{
		if (m_pRecords[i] != NULL)
		{
			delete m_pRecords[i];
			m_pRecords[i] = NULL;
		}
	}
}

void CDB_TSPackets::PartialReset(void)
{
	int			i;

	m_total_packet_count = 0;

	for (i = 0; i < MAX_PID_COUNT; i++)
	{
		if (m_pRecords[i] != NULL)
		{
			m_pRecords[i]->count = 0;
		}
	}
}

int CDB_TSPackets::AddDummyRecord(uint16_t usPID, uint8_t payload_class, uint8_t stream_type, uint8_t stream_subtype, char* pszMsg, int size)
{
	int rtcode = MIDDLEWARE_DB_NO_ERROR;

	RECORD_TSPacket_t*	pInfo;

	pInfo = m_pRecords[usPID];

	if (pInfo == NULL)
	{
		//说明是新发现的PID
		pInfo = new RECORD_TSPacket_t;
		if (pInfo != NULL)
		{
			m_nPIDCount++;

			m_pRecords[usPID] = pInfo;

			pInfo->usPID = usPID;
			if (payload_class == TSPAYLOAD_CLASS_SECTION)
			{
				pInfo->ucClass = payload_class;
			}
			else
			{
				pInfo->ucClass = GetTSPayloadClassByStreamType(stream_type, stream_subtype);
			}
			pInfo->ucMainType = 0x00;
			pInfo->ucStreamType = stream_type;

			strcpy_s(pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle), pszMsg);
			pInfo->packet_style_need_update = 1;

			pInfo->count = 0;

			memset(pInfo->uiTempCheckedType, 0x00, sizeof(pInfo->uiTempCheckedType));
			pInfo->nCheckedCount = 0;

			pInfo->datarate_min = 1000000000;
			pInfo->datarate_max = -1000000000;

			pInfo->continuity_counter = 0;
			pInfo->continuity_counter_error = 0;
			pInfo->continuity_counter_need_update = 0;

			pInfo->ts_scramble = -1;
			strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "");

			pInfo->ts_scramble_need_update = 0;

			rtcode = MIDDLEWARE_DB_NO_ERROR;
		}
		else
		{
			rtcode = MIDDLEWARE_DB_NO_ENOUGH_MEMORY;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

int CDB_TSPackets::AddPacket(transport_packet_t* pTS_packet)
{
	int					rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;

	int					PID;
	RECORD_TSPacket_t*	pInfo;
	int					next_counter;
	uint8_t				pointer_field;
	int					iTableStartPos;
	uint8_t				table_id;
	uint8_t				stream_id;
	int					section_length;
	int					start_code;
	uint32_t			pid_type;
	uint8_t				class_type;
	uint8_t				main_type;
	int					bSameType;
	int					i;

	uint8_t*			payload_buf;
	uint8_t*			section_buf;

	assert(pTS_packet != NULL);

	m_total_packet_count++;
	
	PID = pTS_packet->PID;
	pInfo = m_pRecords[PID];

	if (pInfo == NULL)
	{
		//说明是新发现的PID，做一些简单的类型检查
		pInfo = new RECORD_TSPacket_t;
		if (pInfo != NULL)
		{
			m_nPIDCount ++;

			m_pRecords[PID] = pInfo;

			class_type = MPEG2_TS_GetPayloadClassByPID(PID);		//只能查出PID已知的PSI/SI section

			pInfo->usPID = PID;
			pInfo->ucClass = class_type;
			pInfo->ucMainType = 0x00;
			pInfo->ucStreamType = 0x00;

			if (pInfo->ucClass != TSPAYLOAD_CLASS_UNKNOWN)
			{
				MPEG2_TS_NumericCoding2Text_PID(PID, pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle));
				pInfo->packet_style_need_update = 1;
			}
			else
			{
				if (PID == 0x1FFF)
				{
					strcpy_s(pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle), "NULL packet");
					pInfo->packet_style_need_update = 0;
				}
				else
				{
					strcpy_s(pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle), "");
					pInfo->packet_style_need_update = 0;
				}
			}

			pInfo->count = 1;

			memset(pInfo->uiTempCheckedType, 0x00, sizeof(pInfo->uiTempCheckedType));		
			pInfo->nCheckedCount = 0;

			pInfo->datarate_min = 1000000000;
			pInfo->datarate_max = -1000000000;

			pInfo->continuity_counter = pTS_packet->continuity_counter;
			pInfo->continuity_counter_error = 0;
			pInfo->continuity_counter_need_update = 1;

			pInfo->ts_scramble = -1;
			pInfo->ts_scramble_need_update = 0;

			rtcode = MIDDLEWARE_DB_NO_ERROR;
		}
		else
		{
			rtcode = MIDDLEWARE_DB_NO_ENOUGH_MEMORY;
		}
	}
	else
	{
		//说明已经发现过该PID
		pInfo->count ++;

		if (pTS_packet->PID != 0x1fff)
		{
			//若不是空包，检查一下包序号连续性
			next_counter = pInfo->continuity_counter + 1;
			next_counter %= 0x10;

			if (pTS_packet->continuity_counter != next_counter)
			{
				pInfo->continuity_counter_error ++;
				pInfo->continuity_counter_need_update = 1;

				rtcode = ETR290_CONTINUITY_COUNT_ERROR;
			}

			pInfo->continuity_counter = pTS_packet->continuity_counter;
		}
		else
		{
			//空包其实也有连续号，但基本上都有错误，检查没有太大意义
		}

		rtcode = MIDDLEWARE_DB_NO_ERROR;
	}

	if (rtcode == MIDDLEWARE_DB_NO_ERROR)
	{
		if (pTS_packet->transport_scrambling_control != pInfo->ts_scramble)
		{
			pInfo->ts_scramble = pTS_packet->transport_scrambling_control;
			pInfo->ts_scramble_need_update = 1;

			switch (pInfo->ts_scramble)
			{
	/*
			case 0:
	#if CHINESE_VERSION 
				strcpy(pInfo->ts_scramble_style, "未加扰");
	#else
				strcpy(pInfo->ts_scramble_style, "not scrambled");
	#endif
				break;
			case 1:
	#if CHINESE_VERSION 
				strcpy(pInfo->ts_scramble_style, "保留");
	#else
				strcpy(pInfo->ts_scramble_style, "reserved");
	#endif
				break;
	*/
			case 2:
#if CHINESE_VERSION 
	//			strcpy(pInfo->ts_scramble_style, "偶密钥加扰");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(偶) *");
#else
	//			strcpy(pInfo->ts_scramble_style, "even key scrambled");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(even) *");
#endif
				break;

			case 3:
#if CHINESE_VERSION 
	//			strcpy(pInfo->ts_scramble_style, "奇密钥加扰");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(奇) *");
#else
	//			strcpy(pInfo->ts_scramble_style, "odd key scrambled");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(odd) *");
#endif
				break;

			default:
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "");
				break;
			}
		}

		//判断TS包的类型
		//if (pInfo->ucClass == PID_CLASS_UNKNOWN)
		if (pInfo->nCheckedCount < PID_STYLE_VERIFY_COUNTDOWNS)
		{
			if (pTS_packet->transport_scrambling_control == 0)		//在未加扰的情况下判断TS净荷的类型
			{
				if (pTS_packet->payload_unit_start_indicator)		//净荷的起始
				{
					if (pTS_packet->adaptation_field_control & 0x01)	//若该TS包有净荷
					{
						payload_buf = pTS_packet->payload_buf;
						start_code = (payload_buf[0]<<16) + (payload_buf[1]<<8) + payload_buf[2];

						class_type = TSPAYLOAD_CLASS_UNKNOWN;
						if (start_code == 0x000001)		//净荷为PES流
						{
							stream_id = payload_buf[3];

							class_type = GetTSPayloadClassByStreamID(stream_id);
							main_type = stream_id;		//stream_id
						}
						else
						{
							//若不是PES流，假设按照section进行验证
							//按照table_id进行查找
							pointer_field = pTS_packet->payload_buf[0];

							//sample-SCTV_GSA_AIT_431_13Dec-pcr抖动大.ts，pointer_field域不为0
							//if (pointer_field == 0x00)		//section的pointer_field通常为0，这么写比较靠谱一点
							{
								iTableStartPos = pointer_field + 1;

								if (iTableStartPos < (pTS_packet->payload_length - 3))
								{
									section_buf = pTS_packet->payload_buf + iTableStartPos;
									table_id = section_buf[0];

									//if ((pTS_packet->payload_buf[iTableStartPos + 1] & 0xC0) == 0x80)			//判断条件更严格一点，降低识别错误
									{
										S32 min_length, max_length;
										GetSectionMinMaxLength(table_id, &min_length, &max_length);

										section_length = (section_buf[1] & 0x0f);
										section_length <<= 8;
										section_length |= section_buf[2];

										if ((section_length >= (min_length - 3)) && (section_length <= (max_length - 3)))		//根据长度，进一步删除误识别的section
										{
											class_type = TSPAYLOAD_CLASS_SECTION;
											//注：对于DSMCC section而言，同一个PID上可能会有0x3B\0x3C table_id出现，判断3次是否为同一个table_id，会有问题
											if (table_id == 0x3C)
											{
												//如果是DSMCC_DDM，强制转换成DSMCC_UNM。added by chendelin 20180819
												table_id = 0x3B;
											}
											main_type = table_id;
										}
									}
								}
							}
						}

						if (class_type != TSPAYLOAD_CLASS_UNKNOWN)
						{
							pid_type = class_type;
							pid_type <<= 13;
							pid_type |= pInfo->usPID;
							pid_type <<= 8;
							pid_type |= main_type;
							pid_type <<= 8;
							//pid_type |= sub_type;

							pInfo->uiTempCheckedType[pInfo->nCheckedCount] = pid_type;				//临时寄存一下类型，不作判断
							pInfo->nCheckedCount++;

							if (pInfo->nCheckedCount >= PID_STYLE_VERIFY_COUNTDOWNS)	//为啥是3次？根据实际的码流试出来的
							{
								bSameType = true;
								for (i = 1; i < pInfo->nCheckedCount; i++)
								{
									if (pInfo->uiTempCheckedType[i] != pInfo->uiTempCheckedType[0])
									{
										bSameType = false;
										break;
									}
								}

								if (bSameType)
								{
									//若多次检测出来的类型一致，则结果应该是可信的，降低误判概率
									//pInfo->uiPIDStyleCode = pInfo->uiTempCheckedType[0];
									pid_type = pInfo->uiTempCheckedType[0];

									//pInfo->ucClass = (pid_type & 0xE0000000) >> 29;
									class_type = ((pid_type & 0xE0000000) >> 29);
									//if (pInfo->ucClass != TSPAYLOAD_CLASS_UNKNOWN)
									//{
									//	if (pInfo->ucClass == TSPAYLOAD_CLASS_SECTION)		//通过payload 3次识别出的类型更可信一点
									//	{
									//		pInfo->ucClass = class_type;
									//	}
									//	//assert(class_type == pInfo->ucClass);
									//}
									//else
									//{
										pInfo->ucClass = class_type;
									//}
									pInfo->ucMainType = (pid_type & 0x0000ff00) >> 8;

									if ((pInfo->ucClass == TSPAYLOAD_CLASS_PES_AUDIO) ||
										(pInfo->ucClass == TSPAYLOAD_CLASS_PES_VIDEO) ||
										(pInfo->ucClass == TSPAYLOAD_CLASS_PES_DATA))
									{
										if (pInfo->ucStreamType != 0x00)
										{
											MPEG2_PSI_NumericCoding2Text_StreamType(pInfo->ucStreamType, 0x00, pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle));
										}
										else
										{
											MPEG_PES_NumericCoding2Text_StreamID(pInfo->ucMainType, pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle));
										}
									}
									else if (pInfo->ucClass == TSPAYLOAD_CLASS_SECTION)
									{
										MPEG_DVB_NumericCoding2Text_TableID(pInfo->usPID, pInfo->ucMainType, pInfo->pszPacketStyle, sizeof(pInfo->pszPacketStyle));
									}

									pInfo->packet_style_need_update = 1;
								}
								else
								{
									//若多次检测出来的类型不一致，则结果不可信。
									pInfo->nCheckedCount = 0;							//重新检测
									memset(pInfo->uiTempCheckedType, 0x00, sizeof(pInfo->uiTempCheckedType));
								}
							}
						}
					}
				}
			}//在未加扰的情况下判断TS净荷的类型
		}
	}

	return rtcode;
}

int CDB_TSPackets::GetRecord(uint16_t usPID, RECORD_TSPacket_t *pRecord)
{
	int rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;

	if (pRecord != NULL)
	{
		if (m_pRecords[usPID] != NULL)
		{
			memcpy(pRecord, m_pRecords[usPID], sizeof(RECORD_TSPacket_t));
			rtcode = NO_ERROR;
		}
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

int CDB_TSPackets::UpdateStreamType(uint16_t usPID, uint8_t stream_type)
{
	int rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;
	RECORD_TSPacket_t* pTSPacketInfo = m_pRecords[usPID];

	if (pTSPacketInfo != NULL)
	{
		pTSPacketInfo->ucStreamType = stream_type;
		if (stream_type != 0x00)
		{
			MPEG2_PSI_NumericCoding2Text_StreamType(stream_type, 0x00, pTSPacketInfo->pszPacketStyle, sizeof(pTSPacketInfo->pszPacketStyle));
			pTSPacketInfo->packet_style_need_update = 1;
		}
		rtcode = MIDDLEWARE_DB_NO_ERROR;
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}

int CDB_TSPackets::UpdateDescription(uint16_t usPID, char* pszMsg, int size)
{
	int rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;
	RECORD_TSPacket_t* pTSPacketInfo = m_pRecords[usPID];

	if (pTSPacketInfo != NULL)
	{
		if (pszMsg != NULL)
		{
			if (strcmp(pTSPacketInfo->pszPacketStyle, "") == 0)
			{
				strcpy_s(pTSPacketInfo->pszPacketStyle, sizeof(pTSPacketInfo->pszPacketStyle), pszMsg);
				pTSPacketInfo->packet_style_need_update = 1;
			}
			rtcode = MIDDLEWARE_DB_NO_ERROR;
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

int CDB_TSPackets::SetDecimateFlag(uint16_t usPID, int bDecimate)
{
	int rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;

	if (m_pRecords[usPID] != NULL)
	{
		m_pRecords[usPID]->bDecimate = bDecimate;
		rtcode = MIDDLEWARE_DB_NO_ERROR;
	}
	else
	{
		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
	}

	return rtcode;
}
