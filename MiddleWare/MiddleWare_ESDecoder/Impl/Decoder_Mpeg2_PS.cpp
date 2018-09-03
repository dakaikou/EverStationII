#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "Mpeg_PESPacket/Include/Mpeg2_stream_id.h"
#include "../Include/Decoder_Mpeg2_PS.h"
//#include "common/CDL_Error.h"

CMPEG_PS_Decoder::CMPEG_PS_Decoder()
{
	m_hMsgWnd = NULL;
	m_hFile = -1;
	m_bSynced = 0;
	m_nPackLength = 0;

	m_nStreamType = 0;

	m_hDispatchMsgWnd = NULL;
//	ps_parse = NULL;

	m_ps_stream_type = 0;

	memset(&m_pack_header, 0x00, sizeof(MPEG_pack_header_t));
	memset(&m_system_header, 0x00, sizeof(MPEG_system_header_t));

	m_hDebugFile = NULL;
}

CMPEG_PS_Decoder::~CMPEG_PS_Decoder(void)
{
	if (m_hDebugFile != NULL)
	{
		fclose(m_hDebugFile);
		m_hDebugFile = NULL;
	}
}

S32 CMPEG_PS_Decoder::GetSize(void)
{
	return (S32)m_nFileSize;
}

S32 CMPEG_PS_Decoder::Seek(S32 filepos)
{
	S32		rtcode = 0;

	if (m_hFile != -1)
	{
		_lseeki64(m_hFile, filepos, SEEK_SET);
	}

	return rtcode;
}

S32 CMPEG_PS_Decoder::FillData(void)
{
	S32		rtcode = -1;
//	S32		move_length;
//	S32		copy_length;
//	S32		read_length;
//	S32		filepos;
//
//	if (m_fifo.hFile != -1)
//	{
//		move_length = m_fifo.rdptr - m_fifo.buf;
//		if (move_length > 0)
//		{
//			//move old data
//			copy_length = m_fifo.wrptr - m_fifo.rdptr;
//			memcpy(m_fifo.buf, m_fifo.rdptr, copy_length);
//
//			m_fifo.rdptr = m_fifo.buf;
//			m_fifo.wrptr = m_fifo.buf + copy_length;
//		}
//
//		copy_length = m_fifo.endptr - m_fifo.wrptr;
//
//		if (copy_length > 0)
//		{
//			//read new data
//			filepos = _tell(m_hFile);
//			read_length = _read(m_fifo.hFile, m_fifo.wrptr, copy_length);
////			if (read_length == 0)		//eof
////			{
////				_lseeki64(m_fifo.hFile, 0, 0);
////				read_length = _read(m_fifo.hFile, m_fifo.wrptr, copy_length);
////			}
//
////			if (_eof(m_hFile))
////			{
////				rtcode = -1;
////			}
////			else if (read_length > 0)
//			if (filepos >= m_nFileSize)
//			{
//				rtcode = -1;
//			}
//
//			if (read_length == copy_length)
//			{
//				m_fifo.wrptr += read_length;
//				m_fifo.bitleft += (read_length << 3);
//
//				rtcode = copy_length;
//			}
//			else
//			{
//				rtcode = -1;
//			}
//		}
//	}

	return rtcode;
}

//S32 CMPEG_PS_Decoder::get_pack_unit(FIFO_t* pfifo, AV_nal_unit_t* pnal_unit)
//{
//	S32		next_start_code = -1;
//	S32		offset;
//
//	U32		next_bits;
//	S32		NumBytesInRBSP;
//
//	U8*		rdptr;
//
//	pnal_unit->NumBytesInRBSP = 0;
//	pnal_unit->rbsp_byte = NULL;
//	pnal_unit->start_code = -1;
//
//	//step1: byte alligned
//	if (pfifo->bitleft > 0)
//	{
//		if (pfifo->rdbit_offset > 0)
//		{
//			FIFO_Flush(pfifo, (8 - pfifo->rdbit_offset));
//		}
//	}
//
//	//step2: nal unit aligned
//	pnal_unit->NumBytesInRBSP = 0;
//	do
//	{
//		if (!m_bSynced)
//		{
//			next_start_code = FIFO_Peek(pfifo, 32);
//			if (next_start_code == 0x000001BA)
//			{
//				NumBytesInRBSP = 4;
//				rdptr = pfifo->rdptr + 4;
//
//				//搜寻下一个pack的起点，以便确定一个pack的长度
//				next_bits = rdptr[0];
//
//				next_bits <<= 8;
//				next_bits |= rdptr[1];
//
//				next_bits <<= 8;
//				next_bits |= rdptr[2];
//
//				while (1)
//				{
//					next_bits <<= 8;
//					next_bits |= rdptr[3];
//
//					if (next_bits == 0x000001BA)			//结束
//					{
//						pnal_unit->rbsp_byte = pfifo->rdptr;
//						pnal_unit->start_code = pnal_unit->rbsp_byte[3];
//						pnal_unit->NumBytesInRBSP = NumBytesInRBSP;
//
//						next_start_code = rdptr[3];			//next_nal_unit_type
//
//						pfifo->rdptr += NumBytesInRBSP;
//						pfifo->bitleft -= (NumBytesInRBSP << 3);
//
//						m_bSynced = 1;
//						m_nPackLength = NumBytesInRBSP;
//						
//						break;
//					}
//					else
//					{
//						NumBytesInRBSP ++;
//						rdptr ++;
//					}
//
//					if ((pfifo->endptr - rdptr) >= 4)
//					{
//						continue;
//					}
//					else
//					{
//						offset = rdptr - pfifo->rdptr;
//
//						if (FillData() == -1)
//						{
//							next_start_code = -1;
//							break;
//						}
//						else
//						{
//							rdptr = pfifo->rdptr + offset;
//						}
//					}
//				} 
//
//				break;
//			}
//			else
//			{
//				FIFO_Flush(pfifo, 8);
//
//				if (pfifo->bitleft <= 0)
//				{
//					next_start_code = -1;
//					break;
//				}
//			}
//		}
//		else
//		{
//			if ((pfifo->endptr - pfifo->rdptr) < (m_nPackLength + 4))
//			{
//				if (FillData() == -1)
//				{
//					next_start_code = -1;
//					break;
//				}
//			}
//
//			rdptr = pfifo->rdptr;
//			next_bits = rdptr[0];
//			next_bits <<= 8;
//			next_bits |= rdptr[1];
//			next_bits <<= 8;
//			next_bits |= rdptr[2];
//			next_bits <<= 8;
//			next_bits |= rdptr[3];
//
//			if (next_bits == 0x000001BA)
//			{
//				pnal_unit->rbsp_byte = pfifo->rdptr;
//				pnal_unit->start_code = pnal_unit->rbsp_byte[3];
//				pnal_unit->NumBytesInRBSP = m_nPackLength;
//
//				next_start_code = rdptr[3];			//next_nal_unit_type
//
//				pfifo->rdptr += m_nPackLength;
//				pfifo->bitleft -= (m_nPackLength << 3);
//
//				break;
//			}
//			else
//			{
//				m_bSynced = 0;
//				m_nPackLength = 0;
//
//				//FIFO_Flush(&m_fifo, 32);
//			}
//		}
//
//	} while (1);
//
//	return next_start_code;
//}

S32 CMPEG_PS_Decoder::Open(HWND hMsgWnd, S32 nStreamType, S8* pszFileName)
{
	S32		rtcode = 0;

//	M2VDecoder.Open(hMsgWnd, STREAM_MPEGVES, NULL);
//	M2ADecoder.Open(hMsgWnd, STREAM_MPEGAES, NULL);
//	AC3Decoder.Open(hMsgWnd, STREAM_AC3AES, NULL);
//
//	m_fifo.rdptr = m_fifo.wrptr = m_fifo.buf;
//	m_fifo.endptr = m_fifo.buf + FIFO_BUF_SIZE;
//	m_fifo.bitleft = 0;
//	m_fifo.rdbit_offset = 0;
//
//	if (nStreamType & STREAM_FILE)
//	{
//		m_hFile = _open(pszFileName, _O_BINARY);
//
//		if (m_hFile != NULL)
//		{
//			m_nStreamType = nStreamType;
//
//			m_hMsgWnd = hMsgWnd;
//			strcpy_s(m_pszFileName, pszFileName);
//
//			_lseek(m_hFile, 0, SEEK_END);
//			m_nFileSize = _tell(m_hFile);
//
//			_lseek(m_hFile, 0, SEEK_SET);
//			m_nStartPos = 0;
//
//			m_fifo.hFile = m_hFile;
//
//			m_nTriggerType = ES_TRIGGER_ONESHOT;		
//			m_bTriggering = 0;
//
//			m_bSynced = 0;
//			m_nPackLength = 0;
//			m_nStartPos = 0;
//
//			//suppose
//			m_ucVidID = 0xe0;
//			m_ucAudID = 0xc0;
////			m_nSelVid = 0xe0;
////			m_nSelAud = 0xc0;
//
////			ps_parse = Mpeg_PS_parse;
////			state = SearchSync(&offset);
//
//			Preview_Init();
//			Preview_AtPercent(0);
//		}
//		else
//		{
//			m_fifo.hFile = -1;
//
//			rtcode = 2;
//		}
//	}
//	else
//	{
//		rtcode = 1;
//	}

	return rtcode;
}

S32 CMPEG_PS_Decoder::Close(void)
{
	m_bSynced = 0;
	m_nPackLength = 0;

	m_nStartPos = 0;

	if (m_hFile != -1)
	{
		_close(m_hFile);
		m_hFile = -1;
	}

	m_nTriggerType = ES_TRIGGER_ONESHOT;
	m_bTriggering = 0;

	M2VDecoder.Close();
	M2ADecoder.Close();
	AC3Decoder.Close();

	return 0;
}

void CMPEG_PS_Decoder::Reset(void)
{
	m_nTriggerType = ES_TRIGGER_ONESHOT;
	m_bTriggering = 0;

	m_bSynced = 0;

	m_nPackLength = 0;
}

//返回旧的触发类型
S32 CMPEG_PS_Decoder::SetTrigger(S32 nTriggerType)
{
	S32 rtcode;
	
	rtcode = m_nTriggerType;

	m_nTriggerType = nTriggerType;

	return rtcode;
}

/*
U32 Mpeg_PS_parse(PVOID* pvoid)
{
	S32						next_start_code;
	AV_nal_unit_t			nal_unit;
	S32						prefix;
	S32						start_code;
	U8*						rdptr;
	S32						pes_header_length;
	S32						next_bits;
	S32						pack_move;

	CMPEG_PS_Decoder* pMpegPSDecoder = (CMPEG_PS_Decoder*)pvoid;

	if (pMpegPSDecoder != NULL)
	{
		while (1)
		{
			next_start_code = pMpegPSDecoder->get_nal_unit(&(pMpegPSDecoder->m_fifo), &nal_unit);
			if (next_start_code >= 0)
			{
				do
				{
					rdptr = nal_unit.rbsp_byte;

					assert(rdptr < pMpegPSDecoder->m_fifo.endptr);

					prefix = (rdptr[0] << 16) | (rdptr[1] << 8) | rdptr[2];

					if (prefix == 0x000001)
					{
						start_code = rdptr[3];

						switch (start_code)
						{
						case PACK_START_CODE:

							if (((rdptr[4] & 0xf0) >> 4) == 2)		//'0010' -- MPEG1 PS stream
							{
								pMpegPSDecoder->m_ps_stream_type = PS_MPEG_11172;		
								pack_move = 12;
							}
							else if (((rdptr[4] & 0xc0) >> 6) == 1)		//'01' -- MPEG2 PS stream
							{
								pMpegPSDecoder->m_ps_stream_type = PS_MPEG_13818;
								pack_move = 14 + (rdptr[13] & 0x07);
							}
							else
							{
								pMpegPSDecoder->m_ps_stream_type = PS_UNKNOWN;
								pack_move = 12;
							}

							//BUGS: FIX ME later!		chendelin		20070124
							if (pMpegPSDecoder->m_nTriggerType == AV_TRIGGER_ONESHOT)
							{
								if (pMpegPSDecoder->m_bTriggering == 0)		//需要触发
								{
									pMpegPSDecoder->m_bTriggering = 1;
								}
								else
								{
									//触发已经完成，清除触发标记
									pMpegPSDecoder->m_bTriggering = 0;
									pMpegPSDecoder->m_nTriggerType = AV_TRIGGER_NONE;
								}
							}
							else if (pMpegPSDecoder->m_nTriggerType == AV_TRIGGER_CONTINUE)
							{
								pMpegPSDecoder->m_bTriggering = 1;
							}

//							if (pMpegPSDecoder->m_bTriggering && pMpegPSDecoder->report_m2ps_pack_header)
//							{
//								pMpegPSDecoder->report_m2ps_pack_header(pMpegPSDecoder->m_ps_stream_type, rdptr, pack_move);
//							}

							nal_unit.rbsp_byte += pack_move;
							nal_unit.NumBytesInRBSP -= pack_move;

							break;

						case SYSTEM_HEADER_START_CODE:
							
							pack_move = 6 + ((rdptr[4] << 8) | rdptr[5]);
							//BUGS: FIX ME later!		chendelin		20070124
//							if (pMpegPSDecoder->m_bTriggering && pMpegPSDecoder->report_m2ps_system_header)
//							{
//								pMpegPSDecoder->report_m2ps_system_header(pMpegPSDecoder->m_ps_stream_type, rdptr, pack_move);
//							}

							nal_unit.rbsp_byte += pack_move;
							nal_unit.NumBytesInRBSP -= pack_move;

							break;

						case PADDING_STREAM:
							
							if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_11172)
							{
								pack_move = 2325;
							}
							else
							{
								pack_move = 6 + ((rdptr[4] << 8) | rdptr[5]);
							}

							nal_unit.rbsp_byte += pack_move;
							nal_unit.NumBytesInRBSP -= pack_move;

							break;

						case PRIVATE_STREAM_1:

							pack_move = 6 + ((rdptr[4] << 8) | rdptr[5]);

//							pes_header_length = rdptr[8] + 9;

//							if (pMpegPSDecoder->report_m2a_pes_header != NULL)
//							{
//								pMpegPSDecoder->report_m2a_pes_header(pMpegPSDecoder->m_ps_stream_type, rdptr, pes_header_length);
//							}

							nal_unit.rbsp_byte += pack_move;
							nal_unit.NumBytesInRBSP -= pack_move;

							break;

						case PRIVATE_STREAM_2:

							pack_move = 6 + ((rdptr[4] << 8) | rdptr[5]);

//							if (pMpegPSDecoder->m_bTriggering && pMpegPSDecoder->report_m2ps_private_stream_2)
//							{
//								pMpegPSDecoder->report_m2ps_private_stream_2(rdptr, pack_move);
//							}

							nal_unit.rbsp_byte += pack_move;
							nal_unit.NumBytesInRBSP -= pack_move;
							
							break;

						default:

							if ((start_code >= VIDEO_STREAM_MIN) && (start_code <= VIDEO_STREAM_MAX))
							{
								if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_11172)				//mpeg1
								{
									pes_header_length = 6;
									next_bits = rdptr[pes_header_length];

									while (next_bits == 0xff)
									{
										pes_header_length ++;
										next_bits = rdptr[pes_header_length];
									}

									if ((next_bits & 0xc0) == 0x40)
									{
										pes_header_length += 2;
										next_bits = rdptr[pes_header_length];
									}

									if ((next_bits & 0xf0) == 0x20)
									{
										pes_header_length += 5;
									}
									else if ((next_bits & 0xf0) == 0x30)
									{
										pes_header_length += 10;
									}
									else
									{
										pes_header_length += 1;
									}
								}
								else if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_13818)			//mpeg2
								{
									pes_header_length = rdptr[8] + 9;
								}

								pack_move = 6 + ((rdptr[4] << 8) | rdptr[5]);

			//					if (pMpegPSSplicer->report_m2v_pes_header != NULL)
			//					{
			//						pMpegPSSplicer->report_m2v_pes_header(m_ps_stream_type, rdptr, pes_header_length, 0);
			//					}

								nal_unit.rbsp_byte += pes_header_length;
								nal_unit.NumBytesInRBSP -= pes_header_length;
								
///								pack_move -= pes_header_length;
								pack_move = nal_unit.NumBytesInRBSP;
								rdptr = nal_unit.rbsp_byte;

								assert(pack_move >= 0);

								pMpegPSDecoder->M2VDecoder.WriteESData(rdptr, pack_move);

								nal_unit.rbsp_byte += pack_move;
								nal_unit.NumBytesInRBSP -= pack_move;
							}
							else if ((start_code >= AUDIO_STREAM_MIN) && (start_code <= AUDIO_STREAM_MAX))
							{
								if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_11172)				//mpeg1
								{
									pes_header_length = 6;
									next_bits = rdptr[pes_header_length];

									while (next_bits == 0xff)
									{
										pes_header_length ++;
										next_bits = rdptr[pes_header_length];
									}

									if ((next_bits & 0xc0) == 0x40)
									{
										pes_header_length += 2;
										next_bits = rdptr[pes_header_length];
									}

									if ((next_bits & 0xf0) == 0x20)
									{
										pes_header_length += 5;
									}
									else if ((next_bits & 0xf0) == 0x30)
									{
										pes_header_length += 10;
									}
									else
									{
										pes_header_length += 1;
									}
								}
								else if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_13818)			//mpeg2
								{
									pes_header_length = rdptr[8] + 9;
								}

								pack_move = 6 + ((rdptr[4] << 8) | rdptr[5]);

								assert(pack_move < nal_unit.NumBytesInRBSP);

			//					if (pMpegPSSplicer->report_m2v_pes_header != NULL)
			//					{
			//						pMpegPSSplicer->report_m2v_pes_header(m_ps_stream_type, rdptr, pes_header_length, 0);
			//					}

								nal_unit.rbsp_byte += pes_header_length;
								nal_unit.NumBytesInRBSP -= pes_header_length;
								
//								pack_move -= pes_header_length;
								pack_move = nal_unit.NumBytesInRBSP;
								rdptr = nal_unit.rbsp_byte;

								assert(pack_move >= 0);

//								pMpegPSDecoder->M2VDecoder.WriteESData(rdptr, pack_move);

								nal_unit.rbsp_byte += pack_move;
								nal_unit.NumBytesInRBSP -= pack_move;
							}
							else
							{
								pack_move = nal_unit.NumBytesInRBSP;

								nal_unit.rbsp_byte += pack_move;
								nal_unit.NumBytesInRBSP -= pack_move;			//丢弃所有数据
							}

							break;
						}
					}
					else
					{
						break;
					}

				} while (nal_unit.NumBytesInRBSP > 0);
			}
			else
			{
				break;
			}
		}
	}
	
	return 0;
}
*/
S32	CMPEG_PS_Decoder::get_next_start_code(AV_nal_unit_t* pnal_unit)
{
	S32		next_start_code = -1;
	S32		temp;
	U8*		buf;
	U8*		start_ptr;
	S32		length;

	start_ptr = pnal_unit->rbsp_byte;
	buf = pnal_unit->rbsp_byte;
	length = pnal_unit->NumBytesInRBSP;

	while (length >= 4)
	{
		temp = buf[0];
		temp <<= 8;
		temp |= buf[1];
		temp <<= 8;
		temp |= buf[2];

		if (temp == 0x000001)
		{
			next_start_code = buf[3];

			pnal_unit->start_code = next_start_code;
			pnal_unit->rbsp_byte = buf;
			pnal_unit->NumBytesInRBSP -= (buf - start_ptr);

			break;
		}
		else
		{
			buf ++;
			length --;
		}
	}

	return next_start_code;
}

//预览初始化
//首先需要找到sequence_header
//然后找到第一幅图像头，其间所有的字段都需要解释

S32	CMPEG_PS_Decoder::Preview_Init(void)
{
	S32		rtcode = 0;

	S32				pack_start_code = -1;
	S32				next_start_code = -1;
	AV_nal_unit_t	nal_unit;

	S32				length;
	U8*				buf;
	S32				pack_move;
	S32				pes_header_length;
	S32				next_bits;

	S32				bExit = 0;
	S32				bFindVideo = 0;
	S32				bFindAudio = 0;
	S32				bFindPackHeader = 0;
//	S32				bFindSystemHeader = 0;

//	do
//	{
//		pack_start_code = get_pack_unit(&m_fifo, &nal_unit);
//
//		if (pack_start_code >= 0)
//		{
//			do
//			{
//				next_start_code = get_next_start_code(&nal_unit);
//				if (next_start_code >= 0)
//				{
//					buf = nal_unit.rbsp_byte;
//					length = nal_unit.NumBytesInRBSP;
//					
//					switch (next_start_code)
//					{
//					case PACK_START_CODE:
//
//						if (((buf[4] & 0xf0) >> 4) == 2)				//'0010'	-- MPEG1 PS stream
//						{
//							m_ps_stream_type = PS_MPEG_11172;
//							pack_move = 12;
//						}
//						else if (((buf[4] & 0xc0) >> 6) == 1)			//'01'		-- MPEG2 PS stream
//						{
//							m_ps_stream_type = PS_MPEG_13818;
//							pack_move = 14 + (buf[13] & 0x07);
//						}
//						else
//						{
//							m_ps_stream_type = PS_UNKNOWN;
//							pack_move = 12;
//						}
//
//						mpeg_ps_decode_pack_header(buf, &length, &m_pack_header);
//						assert(length == pack_move);
//
//						nal_unit.rbsp_byte += length;
//						nal_unit.NumBytesInRBSP -= length;
//
//						if (m_nTriggerType == ES_TRIGGER_ONESHOT)
//						{
//							if (bFindPackHeader == 0)
//							{
//								m_bTriggering = 1;
//								bFindPackHeader = 1;
//							}
//							else
//							{
//								m_bTriggering = 0;
//							}
//
////							if (m_bTriggering == 0)		//需要触发
////							{
////								m_bTriggering = 1;
////							}
////							else
////							{
//								//触发已经完成，清除触发标记
////								m_bTriggering = 0;
////								m_nTriggerType = AV_TRIGGER_NONE;
////							}
//						}
//						else if (m_nTriggerType == ES_TRIGGER_CONTINUE)
//						{
//							m_bTriggering = 1;
//						}
//
//						//if (m_bTriggering && (m_hMsgWnd != NULL))
//						//{
//						//	::SendMessage(m_hMsgWnd, WM_UPDATE_MPEG_PACK_HEADER, (WPARAM)(&m_pack_header), NULL);
//						//}
//
//						break;
//					
//					case SYSTEM_HEADER_START_CODE:
//
//						mpeg_ps_decode_system_header(buf, &length, &m_system_header);
//
//						nal_unit.rbsp_byte += length;
//						nal_unit.NumBytesInRBSP -= length;
//
////						if (m_bTriggering && (m_hMsgWnd != NULL))
//						//if (m_hMsgWnd != NULL)
//						//{
//						//	::SendMessage(m_hMsgWnd, WM_UPDATE_MPEG_SYSTEM_HEADER, (WPARAM)(&m_system_header), NULL);
//						//}
//
////						bExit = 1;
//
//						break;
//
//					default:
//
//						if ((next_start_code >= VIDEO_STREAM_MIN) && (next_start_code <= VIDEO_STREAM_MAX))
//						{
//							m_ucVidID = next_start_code;
//
//							pack_move = 6 + ((buf[4] << 8) | buf[5]);
//							if (pack_move > nal_unit.NumBytesInRBSP)
//							{
//								pack_move = nal_unit.NumBytesInRBSP;
//							}
//
//							if (m_ps_stream_type == PS_MPEG_11172)
//							{
//								pes_header_length = 6;
//								next_bits = buf[pes_header_length];
//
//								while (next_bits == 0xff)
//								{
//									pes_header_length ++;
//									next_bits = buf[pes_header_length];
//								}
//
//								if ((next_bits & 0xc0) == 0x40)
//								{
//									pes_header_length += 2;
//									next_bits = buf[pes_header_length];
//								}
//
//								if ((next_bits & 0xf0) == 0x20)
//								{
//									pes_header_length += 5;
//								}
//								else if ((next_bits & 0xf0) == 0x30)
//								{
//									pes_header_length += 10;
//								}
//								else
//								{
//									pes_header_length += 1;
//								}
//							}
//							else
//							{
//								pes_header_length = buf[8] + 9;
//							}
//
//							nal_unit.rbsp_byte += pes_header_length;
//							nal_unit.NumBytesInRBSP -= pes_header_length;
//							pack_move -= pes_header_length;
//
//							M2VDecoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//
//							nal_unit.rbsp_byte += pack_move;
//							nal_unit.NumBytesInRBSP -= pack_move;
//
//							bFindVideo = 1;
//							bExit = bFindVideo && bFindAudio;
//
//						}
//						else if ((next_start_code >= AUDIO_STREAM_MIN) && (next_start_code <= AUDIO_STREAM_MAX))
//						{
//							m_ucAudID = next_start_code;
//
//							pack_move = 6 + ((buf[4] << 8) | buf[5]);
//							if (pack_move > nal_unit.NumBytesInRBSP)
//							{
//								pack_move = nal_unit.NumBytesInRBSP;
//							}
//
//							if (m_ps_stream_type == PS_MPEG_11172)
//							{
//								pes_header_length = 6;
//								next_bits = buf[pes_header_length];
//
//								while (next_bits == 0xff)
//								{
//									pes_header_length ++;
//									next_bits = buf[pes_header_length];
//								}
//
//								if ((next_bits & 0xc0) == 0x40)
//								{
//									pes_header_length += 2;
//									next_bits = buf[pes_header_length];
//								}
//
//								if ((next_bits & 0xf0) == 0x20)
//								{
//									pes_header_length += 5;
//								}
//								else if ((next_bits & 0xf0) == 0x30)
//								{
//									pes_header_length += 10;
//								}
//								else
//								{
//									pes_header_length += 1;
//								}
//							}
//							else
//							{
//								pes_header_length = buf[8] + 9;
//							}
//
//							nal_unit.rbsp_byte += pes_header_length;
//							nal_unit.NumBytesInRBSP -= pes_header_length;
//							pack_move -= pes_header_length;
//
//							M2ADecoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//
//							nal_unit.rbsp_byte += pack_move;
//							nal_unit.NumBytesInRBSP -= pack_move;
//
//							bFindAudio = 1;
//							bExit = bFindVideo && bFindAudio;
//						}
//						else if (next_start_code == PRIVATE_STREAM_1)
//						{
//							if (m_ps_stream_type == PS_MPEG_11172)
//							{
//								pack_move = nal_unit.NumBytesInRBSP;
//
//								nal_unit.rbsp_byte += pack_move;
//								nal_unit.NumBytesInRBSP -= pack_move;
//							}
//							else
//							{
//								pack_move = 6 + ((buf[4] << 8) | buf[5]);
//								if ((buf[3] == 0xBD) && (pack_move >= 13))
//								{
//									m_ucAudID = buf[3];
//
//									if (pack_move > nal_unit.NumBytesInRBSP)
//									{
//										pack_move = nal_unit.NumBytesInRBSP;
//									}
//
//									pes_header_length = buf[8] + 9 + 4;			//+4 fix AC3 BUG.但是还没有找到相关说明文档
//
//									nal_unit.rbsp_byte += pes_header_length;
//									nal_unit.NumBytesInRBSP -= pes_header_length;
//									pack_move -= pes_header_length;
//
//									assert(pack_move >= 0);
//									AC3Decoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//
//									bFindAudio = 1;
//									bExit = bFindVideo && bFindAudio;
//								}
//								else
//								{
//									pack_move = nal_unit.NumBytesInRBSP;
//								}
//								
//								nal_unit.rbsp_byte += pack_move;
//								nal_unit.NumBytesInRBSP -= pack_move;
//							}
//
//							break;
//						}
//						else
//						{
//							pack_move = 4;
//							nal_unit.rbsp_byte += pack_move;
//							nal_unit.NumBytesInRBSP -= pack_move;
//						}
//
//						break;
//					}
//				}
//
//			} while ((next_start_code >= 0) && (!bExit));
//		}
//
//	} while ((pack_start_code >= 0) && (!bExit));

	return rtcode;
}

void CMPEG_PS_Decoder::OpenVideo(HWND hWnd, S8* pszFourCC, S32 strSize)
{
	M2VDecoder.OpenVideo(hWnd, pszFourCC, strSize);

#if PS_DEBUG_PACK_UNIT
	m_hDebugFile = fopen("e:\\测试流\\ps_pack_unit.ps", "wb");
#elif PS_DEBUG_VIDEO_UNIT
	m_hDebugFile = fopen("e:\\测试流\\ps_video_unit.vid", "wb");
#elif PS_DEBUG_AUDIO_UNIT
	m_hDebugFile = fopen("e:\\测试流\\ps_audio_unit.aud", "wb");
#endif
}

void CMPEG_PS_Decoder::CloseVideo(void)
{
	M2VDecoder.CloseVideo();

#if PS_DEBUG_PACK_UNIT
	if (m_hDebugFile != NULL)
	{
		fclose(m_hDebugFile);
		m_hDebugFile = NULL;
	}
#endif
}

void CMPEG_PS_Decoder::OpenAudio(HWND hWnd)
{
	if (m_ps_stream_type == PS_MPEG_11172)
	{
		M2ADecoder.OpenAudio(hWnd);
	}
	else
	{
		AC3Decoder.OpenAudio(hWnd);
	}
}

void CMPEG_PS_Decoder::CloseAudio(void)
{
	if (m_ps_stream_type == PS_MPEG_11172)
	{
		M2ADecoder.CloseAudio();
	}
	else
	{
		AC3Decoder.CloseAudio();
	}
}

S32	CMPEG_PS_Decoder::Preview_EOF(void)
{
	S32 bEOF = 0;

	bEOF = (_tell(m_hFile) >= m_nFileSize) ? 1 : 0;

	return bEOF;
}

S32 CMPEG_PS_Decoder::Preview_AtPercent(S32 percent)
{
	S32	   offset;
	float  dRatio = percent / 100.0f;

	offset = m_nStartPos + (S32)((m_nFileSize - m_nStartPos) * dRatio);

	//if (m_hFile != -1)
	//{
	//	_lseek(m_hFile, offset, SEEK_SET);
	//}

	//m_fifo.rdptr = m_fifo.wrptr = m_fifo.buf;
	//m_fifo.endptr = m_fifo.buf + FIFO_BUF_SIZE;
	//m_fifo.bitleft = 0;
	//m_fifo.rdbit_offset = 0;

	m_bSynced = 0;
	m_nPackLength = 0;

	return percent;
}

S32 CMPEG_PS_Decoder::ExtDebug(void)
{
	S32		rtcode = 0;

	S32		pack_start_code = -1;
	S32		next_start_code = -1;

	AV_nal_unit_t	nal_unit;
	U8*				buf;
	S32				pack_move;
	S32				pes_header_length;
	S32				next_bits;

	S32				total_length = 0;

	S32		bExit = 0;

//	do
//	{
//		pack_start_code = get_pack_unit(&m_fifo, &nal_unit);
//		if (pack_start_code >= 0)
//		{
//			total_length += nal_unit.NumBytesInRBSP;
//
//	#if	PS_DEBUG_PACK_UNIT
//			if (m_hDebugFile != NULL)
//			{
//				fwrite(m_pucPackBuf, sizeof(U8), pack_length, m_hDebugFile);
//			}
//	#endif
//
//			do
//			{
//				next_start_code = get_next_start_code(&nal_unit);
//				if (next_start_code >= 0)
//				{
//					buf = nal_unit.rbsp_byte;
//
//					if (next_start_code == PACK_START_CODE)
//					{
//						if (((buf[4] & 0xf0) >> 4) == 2)				//'0010'	-- MPEG1 PS stream
//						{
//							m_ps_stream_type = PS_MPEG_11172;
//							pack_move = 12;
//						}
//						else if (((buf[4] & 0xc0) >> 6) == 1)			//'01'		-- MPEG2 PS stream
//						{
//							m_ps_stream_type = PS_MPEG_13818;
//							pack_move = 14 + (buf[13] & 0x07);
//						}
//						else
//						{
//							m_ps_stream_type = PS_UNKNOWN;
//							pack_move = 12;
//						}
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else if (next_start_code == SYSTEM_HEADER_START_CODE)
//					{
//						pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else if (next_start_code == PADDING_STREAM)
//					{
//						if (m_ps_stream_type == PS_MPEG_11172)
//						{
//							pack_move = nal_unit.NumBytesInRBSP;
//						}
//						else
//						{
//							pack_move = 6 + ((buf[4] << 8) | buf[5]);
//						}
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else if (next_start_code == PRIVATE_STREAM_1)
//					{
//						pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//						if (m_ps_stream_type == PS_MPEG_11172)
//						{
//						}
//						else
//						{
//							if ((buf[3] == 0xBD) && (pack_move >= 13))								//AC3 stream
//							{
//								pes_header_length = buf[8] + 9 + 4;			//+4 fix AC3 BUG.但是还没有找到相关说明文档
//
//								if (pack_move > nal_unit.NumBytesInRBSP)
//								{
//									pack_move = nal_unit.NumBytesInRBSP;
//								}
//
//								nal_unit.rbsp_byte += pes_header_length;
//								nal_unit.NumBytesInRBSP -= pes_header_length;
//								pack_move -= pes_header_length;
//
//	#if PS_DEBUG_AUDIO_UNIT
//								fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, m_hDebugFile);
//	#else
//		#if PS_OPEN_AUDIO_DECODER
//								AC3Decoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//		#endif
//	#endif
//							}
//						}
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else if (next_start_code == PRIVATE_STREAM_2)
//					{
//						pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else if ((next_start_code >= VIDEO_STREAM_MIN) && (next_start_code <= VIDEO_STREAM_MAX))
//					{
//						pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//						if (m_ps_stream_type == PS_MPEG_11172)
//						{
//							pes_header_length = 6;
//							next_bits = buf[pes_header_length];
//
//							while (next_bits == 0xff)
//							{
//								pes_header_length ++;
//								next_bits = buf[pes_header_length];
//							}
//
//							if ((next_bits & 0xc0) == 0x40)
//							{
//								pes_header_length += 2;
//								next_bits = buf[pes_header_length];
//							}
//
//							if ((next_bits & 0xf0) == 0x20)
//							{
//								pes_header_length += 5;
//							}
//							else if ((next_bits & 0xf0) == 0x30)
//							{
//								pes_header_length += 10;
//							}
//							else
//							{
//								pes_header_length += 1;
//							}
//						}
//						else
//						{
//							pes_header_length = buf[8] + 9;
//						}
//
//						if (pack_move > nal_unit.NumBytesInRBSP)
//						{
//							pack_move = nal_unit.NumBytesInRBSP;
//						}
//
//						nal_unit.rbsp_byte += pes_header_length;
//						nal_unit.NumBytesInRBSP -= pes_header_length;
//						pack_move -= pes_header_length;
//
//#if PS_DEBUG_VIDEO_UNIT
//						fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, m_hDebugFile);
//#else
//	#if PS_OPEN_VIDEO_DECODER
//						M2VDecoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//	#endif
//#endif
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else if ((next_start_code >= AUDIO_STREAM_MIN) && (next_start_code <= AUDIO_STREAM_MAX))
//					{
//						pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//						if (m_ps_stream_type == PS_MPEG_11172)
//						{
//							pes_header_length = 6;
//							next_bits = buf[pes_header_length];
//
//							while (next_bits == 0xff)
//							{
//								pes_header_length ++;
//								next_bits = buf[pes_header_length];
//							}
//
//							if ((next_bits & 0xc0) == 0x40)
//							{
//								pes_header_length += 2;
//								next_bits = buf[pes_header_length];
//							}
//
//							if ((next_bits & 0xf0) == 0x20)
//							{
//								pes_header_length += 5;
//							}
//							else if ((next_bits & 0xf0) == 0x30)
//							{
//								pes_header_length += 10;
//							}
//							else
//							{
//								pes_header_length += 1;
//							}
//						}
//						else
//						{
//							pes_header_length = buf[8] + 9;
//						}
//
//						if (pack_move > nal_unit.NumBytesInRBSP)
//						{
//							pack_move = nal_unit.NumBytesInRBSP;
//						}
//
//						nal_unit.rbsp_byte += pes_header_length;
//						nal_unit.NumBytesInRBSP -= pes_header_length;
//						pack_move -= pes_header_length;
//
//	#if PS_DEBUG_AUDIO_UNIT
//						fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, m_hDebugFile);
//	#else
//		#if PS_OPEN_AUDIO_DECODER
//						M2ADecoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//		#endif
//	#endif
//
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//					else
//					{
//						pack_move = 4;
//						nal_unit.rbsp_byte += pack_move;
//						nal_unit.NumBytesInRBSP -= pack_move;
//					}
//				}
//
//			} while (next_start_code >= 0);
//		}
//
//	} while ((pack_start_code >= 0) && (total_length < m_nFileSize));

	return 0;
}

S32 CMPEG_PS_Decoder::Preview_NextPack(void)
{
	S32		rtcode = 0;

	S32		pack_start_code = -1;
	S32		next_start_code = -1;

	AV_nal_unit_t	nal_unit;
	U8*				buf;
	S32				pack_move;
	S32				pes_header_length;
	S32				next_bits;

	S32			filepos;
	S32			percent;

	S32		bExit = 0;

//	pack_start_code = get_pack_unit(&m_fifo, &nal_unit);
//	if (pack_start_code >= 0)
//	{
//#if	PS_DEBUG_PACK_UNIT
//		if (m_hDebugFile != NULL)
//		{
//			fwrite(m_pucPackBuf, sizeof(U8), pack_length, m_hDebugFile);
//		}
//#endif
//
//		do
//		{
//			next_start_code = get_next_start_code(&nal_unit);
//			if (next_start_code >= 0)
//			{
//				buf = nal_unit.rbsp_byte;
//
//				if (next_start_code == PACK_START_CODE)
//				{
//					if (((buf[4] & 0xf0) >> 4) == 2)				//'0010'	-- MPEG1 PS stream
//					{
//						m_ps_stream_type = PS_MPEG_11172;
//						pack_move = 12;
//					}
//					else if (((buf[4] & 0xc0) >> 6) == 1)			//'01'		-- MPEG2 PS stream
//					{
//						m_ps_stream_type = PS_MPEG_13818;
//						pack_move = 14 + (buf[13] & 0x07);
//					}
//					else
//					{
//						m_ps_stream_type = PS_UNKNOWN;
//						pack_move = 12;
//					}
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else if (next_start_code == SYSTEM_HEADER_START_CODE)
//				{
//					pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else if (next_start_code == PADDING_STREAM)
//				{
//					if (m_ps_stream_type == PS_MPEG_11172)
//					{
//						pack_move = nal_unit.NumBytesInRBSP;
//					}
//					else
//					{
//						pack_move = 6 + ((buf[4] << 8) | buf[5]);
//					}
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else if (next_start_code == PRIVATE_STREAM_1)
//				{
//					pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//					if (m_ps_stream_type == PS_MPEG_11172)
//					{
//					}
//					else
//					{
//						if ((buf[3] == 0xBD) && (pack_move >= 13))								//AC3 stream
//						{
//							pes_header_length = buf[8] + 9 + 4;			//+4 fix AC3 BUG.但是还没有找到相关说明文档
//
//							if (pack_move > nal_unit.NumBytesInRBSP)
//							{
//								pack_move = nal_unit.NumBytesInRBSP;
//							}
//
//							nal_unit.rbsp_byte += pes_header_length;
//							nal_unit.NumBytesInRBSP -= pes_header_length;
//							pack_move -= pes_header_length;
//
//#if PS_DEBUG_AUDIO_UNIT
//							fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, m_hDebugFile);
//#else
//	#if PS_OPEN_AUDIO_DECODER
//							AC3Decoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//	#endif
//#endif
//						}
//					}
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else if (next_start_code == PRIVATE_STREAM_2)
//				{
//					pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else if ((next_start_code >= VIDEO_STREAM_MIN) && (next_start_code <= VIDEO_STREAM_MAX))
//				{
//					pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//					if (m_ps_stream_type == PS_MPEG_11172)
//					{
//						pes_header_length = 6;
//						next_bits = buf[pes_header_length];
//
//						while (next_bits == 0xff)
//						{
//							pes_header_length ++;
//							next_bits = buf[pes_header_length];
//						}
//
//						if ((next_bits & 0xc0) == 0x40)
//						{
//							pes_header_length += 2;
//							next_bits = buf[pes_header_length];
//						}
//
//						if ((next_bits & 0xf0) == 0x20)
//						{
//							pes_header_length += 5;
//						}
//						else if ((next_bits & 0xf0) == 0x30)
//						{
//							pes_header_length += 10;
//						}
//						else
//						{
//							pes_header_length += 1;
//						}
//					}
//					else
//					{
//						pes_header_length = buf[8] + 9;
//					}
//
//					if (pack_move > nal_unit.NumBytesInRBSP)
//					{
//						pack_move = nal_unit.NumBytesInRBSP;
//					}
//
//					nal_unit.rbsp_byte += pes_header_length;
//					nal_unit.NumBytesInRBSP -= pes_header_length;
//					pack_move -= pes_header_length;
//
//#if PS_DEBUG_VIDEO_UNIT
//					fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, m_hDebugFile);
//#else
//	#if PS_OPEN_VIDEO_DECODER
//					M2VDecoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//	#endif
//#endif
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else if ((next_start_code >= AUDIO_STREAM_MIN) && (next_start_code <= AUDIO_STREAM_MAX))
//				{
//					pack_move = 6 + ((buf[4] << 8) | buf[5]);
//
//					if (m_ps_stream_type == PS_MPEG_11172)
//					{
//						pes_header_length = 6;
//						next_bits = buf[pes_header_length];
//
//						while (next_bits == 0xff)
//						{
//							pes_header_length ++;
//							next_bits = buf[pes_header_length];
//						}
//
//						if ((next_bits & 0xc0) == 0x40)
//						{
//							pes_header_length += 2;
//							next_bits = buf[pes_header_length];
//						}
//
//						if ((next_bits & 0xf0) == 0x20)
//						{
//							pes_header_length += 5;
//						}
//						else if ((next_bits & 0xf0) == 0x30)
//						{
//							pes_header_length += 10;
//						}
//						else
//						{
//							pes_header_length += 1;
//						}
//					}
//					else
//					{
//						pes_header_length = buf[8] + 9;
//					}
//
//					if (pack_move > nal_unit.NumBytesInRBSP)
//					{
//						pack_move = nal_unit.NumBytesInRBSP;
//					}
//
//					nal_unit.rbsp_byte += pes_header_length;
//					nal_unit.NumBytesInRBSP -= pes_header_length;
//					pack_move -= pes_header_length;
//
//#if PS_DEBUG_AUDIO_UNIT
//					fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, m_hDebugFile);
//#else
//	#if PS_OPEN_AUDIO_DECODER
//					M2ADecoder.WriteESData(nal_unit.rbsp_byte, pack_move);
//	#endif
//#endif
//
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//				else
//				{
//					pack_move = 4;
//					nal_unit.rbsp_byte += pack_move;
//					nal_unit.NumBytesInRBSP -= pack_move;
//				}
//			}
//
//		} while (next_start_code >= 0);
//	}

	filepos = _tell(m_hFile);
	percent = (S32)(filepos * 100.0 / m_nFileSize);

	return percent;
}

void CMPEG_PS_Decoder::SetGrid(void)
{
/*
	U8*		buf = NULL;
	S32		width;
	S32		height;

	mpeg_video_setgrid();

	if (m_hWnd != NULL)
	{
		buf = mpeg_video_output(&width, &height);

		if (buf != NULL)
		{
			if (callback_videooutput_Render_bmp != NULL)
			{
				callback_videooutput_Render_bmp(m_hDC, buf, width, height);
			}
		}
	}
*/
}

S32 CMPEG_PS_Decoder::PackDecimate(HWND hMsgWnd, char* pszVFile, char* pszAFile)
{
	if ((pszVFile != NULL) || (pszAFile != NULL))
	{
		m_hDispatchMsgWnd = hMsgWnd;

		if (pszVFile != NULL)
		{
			strcpy_s(m_pszVFile, pszVFile);
		}
		else
		{
			strcpy_s(m_pszVFile, "NULL");
		}

		if (pszAFile != NULL)
		{
			strcpy_s(m_pszAFile, pszAFile);
		}
		else
		{
			strcpy_s(m_pszAFile, "NULL");
		}

		::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MPEG_PS_dispatch, this, 0, 0);
	}
/*
	S32					pack_start_code = -1;
	S32					next_start_code = -1;
	AV_nal_unit_t		nal_unit;
	S32					wr_count = 0;
	S32					pack_move;
	S32					pes_header_length;
	S32					next_bits;

	U8*					buf;
//	S32					length;
//	S32					i;
//	S32					total_length;
	S32					old_fpos;
//	S32					end_fpos = start_pos + decimate_length;

	S32					cur_fpos;
	S32					last_fpos;

	FILE*				fp_vdst = NULL;
	FILE*				fp_adst = NULL;

	if (pszVFile != NULL)
	{
		fp_vdst = fopen(pszVFile, "wb");
	}

	if (pszAFile != NULL)
	{
		fp_adst = fopen(pszAFile, "wb");
	}

	if (m_hFile != -1)
	{
		old_fpos = _tell(m_hFile);
		_lseek(m_hFile, 0, SEEK_SET);

		cur_fpos = 0;
		last_fpos = 0;

		m_fifo.rdptr = m_fifo.wrptr = m_fifo.buf;
		m_fifo.endptr = m_fifo.buf + FIFO_BUF_SIZE;
		m_fifo.bitleft = 0;
		m_fifo.rdbit_offset = 0;

//		total_length = 0;

		do
		{
			pack_start_code = get_pack_unit(&m_fifo, &nal_unit);
			cur_fpos = _tell(m_hFile);
			if (cur_fpos < last_fpos)				//文件循环读，对于文件分解，不需要循环读
			{
				break;
			}
			last_fpos = cur_fpos;

			if (pack_start_code >= 0)
			{
				do
				{
					next_start_code = get_next_start_code(&nal_unit);
					if (next_start_code >= 0)
					{
						buf = nal_unit.rbsp_byte;
						pack_move = 6 + ((buf[4] << 8) | buf[5]);

//						if ((next_start_code == 0xE0) || 
//							(next_start_code == 0xC0) ||
//							(next_start_code == 0xBD))
						if ((next_start_code == m_ucVidID) ||
							(next_start_code == m_ucAudID))
						{
							if (m_ps_stream_type == PS_MPEG_11172)
							{
								pes_header_length = 6;
								next_bits = buf[pes_header_length];

								while (next_bits == 0xff)
								{
									pes_header_length ++;
									next_bits = buf[pes_header_length];
								}

								if ((next_bits & 0xc0) == 0x40)
								{
									pes_header_length += 2;
									next_bits = buf[pes_header_length];
								}

								if ((next_bits & 0xf0) == 0x20)
								{
									pes_header_length += 5;
								}
								else if ((next_bits & 0xf0) == 0x30)
								{
									pes_header_length += 10;
								}
								else
								{
									pes_header_length += 1;
								}
							}
							else
							{
								if ((next_start_code == 0xBD) && (pack_move >= 13))
								{
									pes_header_length = buf[8] + 9 + 4;			//+4 fix AC3 BUG.但是还没有找到相关说明文档
								}
								else
								{
//									pes_header_length = buf[8] + 9;
									pes_header_length = pack_move; 
								}
							}

							if (pack_move > nal_unit.NumBytesInRBSP)
							{
								pack_move = nal_unit.NumBytesInRBSP;
							}

							nal_unit.rbsp_byte += pes_header_length;
							nal_unit.NumBytesInRBSP -= pes_header_length;
							pack_move -= pes_header_length;

//							if (next_start_code == 0xE0)
							if (next_start_code == m_ucVidID)
							{
								if (fp_vdst != NULL)
								{
									fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, fp_vdst);
								}
							}
//							else if ((next_start_code == 0xC0) ||
//									 (next_start_code == 0xBD))
							else if (next_start_code == m_ucAudID)
							{
								if (fp_adst != NULL)
								{
									fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, fp_adst);
								}
							}
						}
						else
						{
							if (next_start_code == PACK_START_CODE)
							{
								if (((buf[4] & 0xf0) >> 4) == 2)				//'0010'	-- MPEG1 PS stream
								{
									m_ps_stream_type = PS_MPEG_11172;
									pack_move = 12;
								}
								else if (((buf[4] & 0xc0) >> 6) == 1)			//'01'		-- MPEG2 PS stream
								{
									m_ps_stream_type = PS_MPEG_13818;
									pack_move = 14 + (buf[13] & 0x07);
								}
								else
								{
									m_ps_stream_type = PS_UNKNOWN;
									pack_move = 12;
								}
							}
							else if (next_start_code == SYSTEM_HEADER_START_CODE)
							{
								pack_move = 6 + ((buf[4] << 8) | buf[5]);
							}
							else if (next_start_code == PADDING_STREAM)
							{
								if (m_ps_stream_type == PS_MPEG_11172)
								{
//									pack_move = 6 + ((buf[4] << 8) | buf[5]);
									pack_move = nal_unit.NumBytesInRBSP;
								}
								else
								{
									pack_move = 6 + ((buf[4] << 8) | buf[5]);
								}
							}
							else if (next_start_code == PRIVATE_STREAM_1)
							{
								pack_move = 6 + ((buf[4] << 8) | buf[5]);
							}
							else if (next_start_code == PRIVATE_STREAM_2)
							{
								pack_move = 6 + ((buf[4] << 8) | buf[5]);
							}
							else if ((next_start_code >= VIDEO_STREAM_MIN) && (next_start_code <= VIDEO_STREAM_MAX))
							{
//								pack_move = 6 + ((buf[4] << 8) | buf[5]);
								pack_move = nal_unit.NumBytesInRBSP;
							}
							else if ((next_start_code >= AUDIO_STREAM_MIN) && (next_start_code <= AUDIO_STREAM_MAX))
							{
//								pack_move = 6 + ((buf[4] << 8) | buf[5]);
								pack_move = nal_unit.NumBytesInRBSP;
							}
							else
							{
								pack_move = 4;
							}
						}

						nal_unit.rbsp_byte += pack_move;
						nal_unit.NumBytesInRBSP -= pack_move;
					}

				} while (next_start_code >= 0);
			}

		} while ((pack_start_code >= 0));

		_lseek(m_hFile, old_fpos, SEEK_SET);
	}

	if (fp_vdst != NULL)
	{
		fclose(fp_vdst);
		fp_vdst = NULL;
	}
	if (fp_adst != NULL)
	{
		fclose(fp_adst);
		fp_adst = NULL;
	}
*/
	return 0;
}

/*
S32	CMPEG_PS_Decoder::SearchSync(S32* poffset)
{
	S32		rtcode = 0;
	U8		buf[1024];
	U8*		endptr = buf + sizeof(buf);
	U8*		rdptr = endptr;
	S32		offset;
	S32		rdsize;
	S32		movelength;
	S32		readlength;
	S32		start_code;
	S32		bFind = 0;

	rdsize = 0;
	offset = 0;

	do
	{
		movelength = endptr - rdptr;
		if (movelength > 0)
		{
			memcpy(buf, rdptr, movelength);
		}
		rdptr = buf;
		endptr = buf + movelength;

		readlength = sizeof(buf) - movelength;
		rdsize = _read(m_hFile, buf + movelength, readlength);

		if (rdsize > 0)
		{
			endptr += rdsize;

			while (rdptr <= (endptr - 4))
			{
				start_code = rdptr[0];
				start_code <<= 8;
				start_code |= rdptr[1];
				start_code <<= 8;
				start_code |= rdptr[2];
				start_code <<= 8;
				start_code |= rdptr[3];

				if (start_code == 0x000001BA)
				{
//					_lseeki64(m_hFile, rdptr - endptr, SEEK_CUR);

					bFind = 1;
					break;
				}
				else
				{
					rdptr ++;
					offset ++;
				}
			}
		}
		else
		{
			break;
		}

	} while (!bFind);

	if (bFind)
	{
		S32 filepos = _tell(m_hFile);
		_lseek(m_hFile, -(endptr - rdptr), SEEK_CUR);
		filepos = _tell(m_hFile);
	}
	else
	{
		offset += (endptr - rdptr);
		rtcode = 1;
	}

	if (poffset != NULL)
	{
		*poffset = offset;
	}

	return rtcode;
}
*/

U32 MPEG_PS_dispatch(PVOID* pvoid)
{
	CMPEG_PS_Decoder* pMpegPSDecoder = (CMPEG_PS_Decoder*)pvoid;

	if (pMpegPSDecoder != NULL)
	{
		char				pszDebugInfo[256];
		S32					pack_start_code = -1;
		S32					next_start_code = -1;
		AV_nal_unit_t		nal_unit;
		S32					wr_count = 0;
		S32					pack_move;
		S32					pes_header_length;
		S32					next_bits;

		U8*					buf;
	//	S32					length;
	//	S32					i;
		S32					total_length;
		S32					old_fpos;
	//	S32					end_fpos = start_pos + decimate_length;

		S32					cur_fpos;
		S32					last_fpos;

		FILE*				fp_vdst = NULL;
		FILE*				fp_adst = NULL;

		if (strcmp(pMpegPSDecoder->m_pszVFile, "NULL") != 0)
		{
			fopen_s(&fp_vdst, pMpegPSDecoder->m_pszVFile, "wb");
		}

		if (strcmp(pMpegPSDecoder->m_pszAFile, "NULL") != 0)
		{
			fopen_s(&fp_adst, pMpegPSDecoder->m_pszAFile, "wb");
		}

		if (pMpegPSDecoder->m_hFile != -1)
		{
			old_fpos = _tell(pMpegPSDecoder->m_hFile);

			_lseek(pMpegPSDecoder->m_hFile, 0, SEEK_END);
			total_length =_tell(pMpegPSDecoder->m_hFile);

			_lseek(pMpegPSDecoder->m_hFile, 0, SEEK_SET);

			cur_fpos = 0;
			last_fpos = 0;

			//pMpegPSDecoder->m_fifo.rdptr = pMpegPSDecoder->m_fifo.wrptr = pMpegPSDecoder->m_fifo.buf;
			//pMpegPSDecoder->m_fifo.endptr = pMpegPSDecoder->m_fifo.buf + FIFO_BUF_SIZE;
			//pMpegPSDecoder->m_fifo.bitleft = 0;
			//pMpegPSDecoder->m_fifo.rdbit_offset = 0;

	//		do
	//		{
	//			pack_start_code = pMpegPSDecoder->get_pack_unit(&pMpegPSDecoder->m_fifo, &nal_unit);
	//			cur_fpos = _tell(pMpegPSDecoder->m_hFile);
	//			if (cur_fpos < last_fpos)				//文件循环读，对于文件分解，不需要循环读
	//			{
	//				break;
	//			}
	//			last_fpos = cur_fpos;

	//			if (pMpegPSDecoder->m_hDispatchMsgWnd != NULL)
	//			{
	//				::SendMessage(pMpegPSDecoder->m_hDispatchMsgWnd, MSG_PS_DISPATCH, (int)((cur_fpos / (double)total_length) * 100), 1);
	//			}

	//			if (pack_start_code >= 0)
	//			{
	//				do
	//				{
	//					next_start_code = pMpegPSDecoder->get_next_start_code(&nal_unit);
	//					if (next_start_code >= 0)
	//					{
	//						buf = nal_unit.rbsp_byte;
	//						pack_move = 6 + ((buf[4] << 8) | buf[5]);

	//						if ((next_start_code == pMpegPSDecoder->m_ucVidID) ||
	//							(next_start_code == pMpegPSDecoder->m_ucAudID))
	//						{
	//							if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_11172)
	//							{
	//								pes_header_length = 6;
	//								next_bits = buf[pes_header_length];

	//								while (next_bits == 0xff)
	//								{
	//									pes_header_length ++;
	//									next_bits = buf[pes_header_length];
	//								}

	//								if ((next_bits & 0xc0) == 0x40)
	//								{
	//									pes_header_length += 2;
	//									next_bits = buf[pes_header_length];
	//								}

	//								if ((next_bits & 0xf0) == 0x20)
	//								{
	//									pes_header_length += 5;
	//								}
	//								else if ((next_bits & 0xf0) == 0x30)
	//								{
	//									pes_header_length += 10;
	//								}
	//								else
	//								{
	//									pes_header_length += 1;
	//								}
	//							}
	//							else
	//							{
	//								if ((next_start_code == 0xBD) && (pack_move >= 13))
	//								{
	//									pes_header_length = buf[8] + 9 + 4;			//+4 fix AC3 BUG.但是还没有找到相关说明文档
	//								}
	//								else
	//								{
	////									pes_header_length = buf[8] + 9;
	//									pes_header_length = pack_move; 
	//								}
	//							}

	//							if (pack_move > nal_unit.NumBytesInRBSP)
	//							{
	//								pack_move = nal_unit.NumBytesInRBSP;
	//							}

	//							nal_unit.rbsp_byte += pes_header_length;
	//							nal_unit.NumBytesInRBSP -= pes_header_length;
	//							pack_move -= pes_header_length;

	//							if (next_start_code == pMpegPSDecoder->m_ucVidID)
	//							{
	//								if (fp_vdst != NULL)
	//								{
	//									fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, fp_vdst);
	//								}
	//							}
	//							else if (next_start_code == pMpegPSDecoder->m_ucAudID)
	//							{
	//								if (fp_adst != NULL)
	//								{
	//									fwrite(nal_unit.rbsp_byte, sizeof(char), pack_move, fp_adst);
	//								}
	//							}
	//						}
	//						else
	//						{
	//							if (next_start_code == PACK_START_CODE)
	//							{
	//								if (((buf[4] & 0xf0) >> 4) == 2)				//'0010'	-- MPEG1 PS stream
	//								{
	//									pMpegPSDecoder->m_ps_stream_type = PS_MPEG_11172;
	//									pack_move = 12;
	//								}
	//								else if (((buf[4] & 0xc0) >> 6) == 1)			//'01'		-- MPEG2 PS stream
	//								{
	//									pMpegPSDecoder->m_ps_stream_type = PS_MPEG_13818;
	//									pack_move = 14 + (buf[13] & 0x07);
	//								}
	//								else
	//								{
	//									pMpegPSDecoder->m_ps_stream_type = PS_UNKNOWN;
	//									pack_move = 12;
	//								}
	//							}
	//							else if (next_start_code == SYSTEM_HEADER_START_CODE)
	//							{
	//								pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//							}
	//							else if (next_start_code == PADDING_STREAM)
	//							{
	//								if (pMpegPSDecoder->m_ps_stream_type == PS_MPEG_11172)
	//								{
	////									pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//									pack_move = nal_unit.NumBytesInRBSP;
	//								}
	//								else
	//								{
	//									pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//								}
	//							}
	//							else if (next_start_code == PRIVATE_STREAM_1)
	//							{
	//								pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//							}
	//							else if (next_start_code == PRIVATE_STREAM_2)
	//							{
	//								pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//							}
	//							else if ((next_start_code >= VIDEO_STREAM_MIN) && (next_start_code <= VIDEO_STREAM_MAX))
	//							{
	////								pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//								pack_move = nal_unit.NumBytesInRBSP;
	//							}
	//							else if ((next_start_code >= AUDIO_STREAM_MIN) && (next_start_code <= AUDIO_STREAM_MAX))
	//							{
	////								pack_move = 6 + ((buf[4] << 8) | buf[5]);
	//								pack_move = nal_unit.NumBytesInRBSP;
	//							}
	//							else
	//							{
	//								pack_move = 4;

	//								if (pMpegPSDecoder->m_hDispatchMsgWnd != NULL)
	//								{
	//									sprintf_s(pszDebugInfo, "流分解错误: %02x %02x %02x %02x %02x %02x %02x %02x....\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	//									::SendMessage(pMpegPSDecoder->m_hDispatchMsgWnd, MSG_PS_DISPATCH,(WPARAM)pszDebugInfo, 2);
	//								}
	//							}
	//						}

	//						nal_unit.rbsp_byte += pack_move;
	//						nal_unit.NumBytesInRBSP -= pack_move;
	//					}

	//				} while (next_start_code >= 0);
	//			}

	//		} while ((pack_start_code >= 0));

			_lseek(pMpegPSDecoder->m_hFile, old_fpos, SEEK_SET);

			if (pMpegPSDecoder->m_hDispatchMsgWnd != NULL)
			{
				//::SendMessage(pMpegPSDecoder->m_hDispatchMsgWnd, MSG_PS_DISPATCH, 0, 0);
			}
		}

		if (fp_vdst != NULL)
		{
			fclose(fp_vdst);
			fp_vdst = NULL;
		}
		if (fp_adst != NULL)
		{
			fclose(fp_adst);
			fp_adst = NULL;
		}
	}

	return 0;
}
