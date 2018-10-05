#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>

#include "../Include/MiddleWare_SectionSplicer_ErrorCode.h"
#include "../Include/Mpeg2_SectionSplicer.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_table_id.h"

CSectionSplicer::CSectionSplicer(void)
{
	Init();
	Reset();
}

CSectionSplicer::~CSectionSplicer()
{
	Reset();
}

void CSectionSplicer::Init(void)
{
	m_pucSectionBuf = NULL;
}

void CSectionSplicer::Reset(void)
{
	if (m_pucSectionBuf != NULL)
	{
		free(m_pucSectionBuf);
		m_pucSectionBuf = NULL;
	}

	m_usPID = 0xffff;
	m_ucTableID = 0xff;
	m_nPkIndex = -1;
	m_nSectionLength = 0;
	m_nWriteSize = 0;
}

S32 CSectionSplicer::WriteTSPacket(transport_packet_t* ptransport_packet)
{
	S32		rtcode = SECTION_SPLICE_UNKNOWN_ERROR;

	//section syntax
	U8*		section_buf;
	S32		section_length;
	S32		pointer_field;

	U8*		payload_buf;
	S32		payload_length;

	//control part
	S32		bAligned = 0;
	S32		copy_length;

//	U8		ucOldTableID = 0xFF;

	if (ptransport_packet != NULL)
	{
		//if (ptransport_packet->PID == 0x0600)
		//{
		//	int debug = 1;		//仅调试用
		//}

		if (ptransport_packet->transport_error_indicator == 0)				//packet with no error
		{
			if (ptransport_packet->transport_scrambling_control == 0)		//not scrambled
			{
				payload_buf = ptransport_packet->payload_buf;
				payload_length = ptransport_packet->payload_length;

				assert(payload_length <= 184);

				if (ptransport_packet->payload_unit_start_indicator)
				{
					//payload_unit_start_indicator标识着新的section起始
					pointer_field = payload_buf[0];

					section_buf = payload_buf + 1 + pointer_field;
					payload_length -= (1 + pointer_field);

					//section 起始部分
					if (m_pucSectionBuf != NULL)
					{
						//当前TS包携带着上一个Section的尾巴，下一个Section的开始，示例码流：SCTV_GSA_AIT_431_13Dec.ts

						memcpy(m_pucSectionBuf + m_nWriteSize, payload_buf + 1, pointer_field);
						m_nWriteSize += pointer_field;

						if (m_nWriteSize >= m_nSectionLength)
						{
							rtcode = SECTION_SPLICE_ONE_MORE_SECTIONS;
						}
						else
						{
							//上次不完整，需要丢弃
							Reset();

							rtcode = NO_ERROR;
						}
					}
					else
					{
						rtcode = NO_ERROR;
					}

					if (rtcode == NO_ERROR)
					{
						if (payload_length >= 3)
						{
							S32 maybe_padding = 1;

							//填充字节检查
							for (int i = 0; i < payload_length; i++)
							{
								if (section_buf[i] != 0xFF)
								{
									maybe_padding = 0;
								}
							}

							if (maybe_padding == 0)
							{
								//净荷不是填充数据，需要认真分析它的内容

								section_length = (section_buf[1] & 0x0f) << 8;
								section_length |= section_buf[2];

								if ((section_length > 0) && (section_length <= 4093))
								{
									m_nSectionLength = section_length + 3;
									m_usPID = ptransport_packet->PID;
									m_ucTableID = section_buf[0];

									m_nPkIndex = ptransport_packet->continuity_counter;

									m_nWriteSize = 0;
									assert(m_pucSectionBuf == NULL);
									m_pucSectionBuf = (U8*)malloc(m_nSectionLength);
									if (m_pucSectionBuf != NULL)
									{
										copy_length = (payload_length > m_nSectionLength) ? m_nSectionLength : payload_length;

										if (copy_length > 0)
										{
											memcpy(m_pucSectionBuf, section_buf, copy_length);
										}
										m_nWriteSize = copy_length;

										if (m_nWriteSize >= m_nSectionLength)
										{
											rtcode = NO_ERROR;
										}
										else
										{
											rtcode = SECTION_SPLICE_NOT_COMPLETE;
										}
									}
									else
									{
										rtcode = SECTION_SPLICE_MEMORY_ERROR;
									}
								}
								else
								{
									rtcode = SECTION_SPLICE_SYNTAX_ERROR;
								}
							}
							else
							{
								//净荷剩下的部分全是填充字节，这属于正常现象
								rtcode = SECTION_SPLICE_PADDING_BYTES;
							}
						}
						else
						{
							//若payload_unit_start_indicator指示一个section的起始，但是TS的payload连最基本的section头部信息都包含不全，则这个section值得怀疑

							rtcode = SECTION_SPLICE_SYNTAX_ERROR;
						}
					}
				}
				else
				{
					//首先要判断一下是否已经开始拼接section
					if (m_usPID != 0xffff)
					{
						if (ptransport_packet->PID == m_usPID)		//PID必须保持一致
						{
							//检查包序号
							if (ptransport_packet->continuity_counter == 0)
							{
								bAligned = (m_nPkIndex == 15) ? 1 : 0;
							}
							else
							{
								bAligned = ((m_nPkIndex + 1) == ptransport_packet->continuity_counter) ? 1 : 0;
							}

							if (bAligned)
							{
								if ((m_nWriteSize + payload_length) > m_nSectionLength)
								{
									copy_length = m_nSectionLength - m_nWriteSize;
								}
								else
								{
									copy_length = payload_length;
								}

								memcpy(m_pucSectionBuf + m_nWriteSize, payload_buf, copy_length);
								m_nWriteSize += copy_length;
				
								if (m_nWriteSize >= m_nSectionLength)
								{
									rtcode = NO_ERROR;
								}
								else
								{
									//继续等下一个TS包
									m_nPkIndex = ptransport_packet->continuity_counter;

									rtcode = SECTION_SPLICE_NOT_COMPLETE;
								}
							}
							else
							{
								//如果包序号不连续，说明有丢包的现象发生，此时section数据也是不能用的
								Reset();
								rtcode = SECTION_SPLICE_CONTINUITY_ERROR;						//包不连续
							}
						}
						else
						{
							//在拼接的过程中发生了PID变化，此时section数据也是不能用的
							Reset();
							rtcode = SECTION_SPLICE_PID_ERROR;					//上一个包还没有拼完，又来下一个包
						}
					}
					else
					{
						//不处理，丢掉TS包
						rtcode = SECTION_SPLICE_NOT_SYNC;
					}
				}
			}
			else
			{
				//遇到TS包加扰的情况下，不管前面已经收集多少数据，都要扔掉
				Reset();
				rtcode = SECTION_SPLICE_SCRAMBLED_PACKET;				//TS包被加扰
			}
		}
		else
		{
			//遇到TS包错误的情况下，不管前面已经收集多少数据，都要扔掉
			Reset();
			rtcode = SECTION_SPLICE_TRANSPORT_ERROR;			//TS包传输错误
		}
	}
	else
	{
		Reset();
		rtcode = SECTION_SPLICE_PARAMETER_ERROR;				//输入参数错误
	}

	return rtcode;
}

