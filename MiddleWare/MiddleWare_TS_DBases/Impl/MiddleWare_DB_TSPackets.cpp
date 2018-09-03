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
		//TRACE("�����ݿ�ʧ�ܣ�%s\n", sqlite3_errmsg(pdb));
		//cout << "�����ݿ�ʧ�ܣ�" << sqlite3_errmsg(db) << endl;

		return 0;
	}
	else
	{
		//TRACE("���ݿ�򿪳ɹ�!\n");
		//cout << "���ݿ�򿪳ɹ�" << endl;
	}

	char* errmsg;
	nResult = sqlite3_exec(pdb, "create table TSPACKETS(id integer primary key autoincrement,name varchar(100))", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("������ʧ��(errmsg)��%s\n", errmsg);
		//TRACE("������ʧ��(sqlite3_errmsg)��%s\n", sqlite3_errmsg(pdb));
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
		//TRACE("��������ʧ��(errmsg)��%s", errmsg);
		//TRACE("��������ʧ��(sqlite3_errmsg)��%s", sqlite3_errmsg(pdb));
	}

	strcpy_s(strSql, sizeof(strSql), "select * from TSPACKETS");
	nResult = sqlite3_exec(pdb, strSql, user_callback, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("�����ݱ�(errmsg)��%s\n", errmsg);
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
		//˵�����·��ֵ�PID
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
		//˵�����·��ֵ�PID����һЩ�򵥵����ͼ��
		pInfo = new RECORD_TSPacket_t;
		if (pInfo != NULL)
		{
			m_nPIDCount ++;

			m_pRecords[PID] = pInfo;

			class_type = MPEG2_TS_GetPayloadClassByPID(PID);		//ֻ�ܲ��PID��֪��PSI/SI section

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
		//˵���Ѿ����ֹ���PID
		pInfo->count ++;

		if (pTS_packet->PID != 0x1fff)
		{
			//�����ǿհ������һ�°����������
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
			//�հ���ʵҲ�������ţ��������϶��д��󣬼��û��̫������
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
				strcpy(pInfo->ts_scramble_style, "δ����");
	#else
				strcpy(pInfo->ts_scramble_style, "not scrambled");
	#endif
				break;
			case 1:
	#if CHINESE_VERSION 
				strcpy(pInfo->ts_scramble_style, "����");
	#else
				strcpy(pInfo->ts_scramble_style, "reserved");
	#endif
				break;
	*/
			case 2:
#if CHINESE_VERSION 
	//			strcpy(pInfo->ts_scramble_style, "ż��Կ����");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(ż) *");
#else
	//			strcpy(pInfo->ts_scramble_style, "even key scrambled");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(even) *");
#endif
				break;

			case 3:
#if CHINESE_VERSION 
	//			strcpy(pInfo->ts_scramble_style, "����Կ����");
				strcpy_s(pInfo->ts_scramble_style, sizeof(pInfo->ts_scramble_style), "(��) *");
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

		//�ж�TS��������
		//if (pInfo->ucClass == PID_CLASS_UNKNOWN)
		if (pInfo->nCheckedCount < PID_STYLE_VERIFY_COUNTDOWNS)
		{
			if (pTS_packet->transport_scrambling_control == 0)		//��δ���ŵ�������ж�TS���ɵ�����
			{
				if (pTS_packet->payload_unit_start_indicator)		//���ɵ���ʼ
				{
					if (pTS_packet->adaptation_field_control & 0x01)	//����TS���о���
					{
						payload_buf = pTS_packet->payload_buf;
						start_code = (payload_buf[0]<<16) + (payload_buf[1]<<8) + payload_buf[2];

						class_type = TSPAYLOAD_CLASS_UNKNOWN;
						if (start_code == 0x000001)		//����ΪPES��
						{
							stream_id = payload_buf[3];

							class_type = GetTSPayloadClassByStreamID(stream_id);
							main_type = stream_id;		//stream_id
						}
						else
						{
							//������PES�������谴��section������֤
							//����table_id���в���
							pointer_field = pTS_packet->payload_buf[0];

							//sample-SCTV_GSA_AIT_431_13Dec-pcr������.ts��pointer_field��Ϊ0
							//if (pointer_field == 0x00)		//section��pointer_fieldͨ��Ϊ0����ôд�ȽϿ���һ��
							{
								iTableStartPos = pointer_field + 1;

								if (iTableStartPos < (pTS_packet->payload_length - 3))
								{
									section_buf = pTS_packet->payload_buf + iTableStartPos;
									table_id = section_buf[0];

									//if ((pTS_packet->payload_buf[iTableStartPos + 1] & 0xC0) == 0x80)			//�ж��������ϸ�һ�㣬����ʶ�����
									{
										S32 min_length, max_length;
										GetSectionMinMaxLength(table_id, &min_length, &max_length);

										section_length = (section_buf[1] & 0x0f);
										section_length <<= 8;
										section_length |= section_buf[2];

										if ((section_length >= (min_length - 3)) && (section_length <= (max_length - 3)))		//���ݳ��ȣ���һ��ɾ����ʶ���section
										{
											class_type = TSPAYLOAD_CLASS_SECTION;
											//ע������DSMCC section���ԣ�ͬһ��PID�Ͽ��ܻ���0x3B\0x3C table_id���֣��ж�3���Ƿ�Ϊͬһ��table_id����������
											if (table_id == 0x3C)
											{
												//�����DSMCC_DDM��ǿ��ת����DSMCC_UNM��added by chendelin 20180819
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

							pInfo->uiTempCheckedType[pInfo->nCheckedCount] = pid_type;				//��ʱ�Ĵ�һ�����ͣ������ж�
							pInfo->nCheckedCount++;

							if (pInfo->nCheckedCount >= PID_STYLE_VERIFY_COUNTDOWNS)	//Ϊɶ��3�Σ�����ʵ�ʵ������Գ�����
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
									//����μ�����������һ�£�����Ӧ���ǿ��ŵģ��������и���
									//pInfo->uiPIDStyleCode = pInfo->uiTempCheckedType[0];
									pid_type = pInfo->uiTempCheckedType[0];

									//pInfo->ucClass = (pid_type & 0xE0000000) >> 29;
									class_type = ((pid_type & 0xE0000000) >> 29);
									//if (pInfo->ucClass != TSPAYLOAD_CLASS_UNKNOWN)
									//{
									//	if (pInfo->ucClass == TSPAYLOAD_CLASS_SECTION)		//ͨ��payload 3��ʶ��������͸�����һ��
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
									//����μ����������Ͳ�һ�£����������š�
									pInfo->nCheckedCount = 0;							//���¼��
									memset(pInfo->uiTempCheckedType, 0x00, sizeof(pInfo->uiTempCheckedType));
								}
							}
						}
					}
				}
			}//��δ���ŵ�������ж�TS���ɵ�����
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
