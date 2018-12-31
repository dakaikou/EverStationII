#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "../Include/ESDecoder.h"
#include "../Include/ESDecoder_ErrorCode.h"

CESDecoder::CESDecoder(void)
{
	m_hFile = -1;
	memset(m_pszFileName, sizeof(m_pszFileName), 0x00);
	m_dwStreamType = 0;

	m_callback_av_es_parse = NULL;

	m_nPrePcrPos = -1;
	m_nPrePtsPos = -1;
	m_nPreDtsPos = -1;

	m_usES_PID = 0xffff;
	m_usPCR_PID = 0xffff;
}

CESDecoder::~CESDecoder()
{
	//Reset();
}

int CESDecoder::Open(uint32_t dwStreamType, char* pszFileName)
{
	m_dwStreamType = dwStreamType;

	if (m_dwStreamType & STREAM_FILE)
	{
		strcpy_s(m_pszFileName, pszFileName);

		_sopen_s(&m_hFile, m_pszFileName, _O_BINARY | _O_RDONLY, _SH_DENYWR, 0);

		_lseeki64(m_hFile, 0, SEEK_END);
		m_nFileTotalSize = _telli64(m_hFile);

		_lseeki64(m_hFile, 0, SEEK_SET);
		m_nFileStartPos = 0;
		m_nCurReadPos = 0;
	}

	return 0;
}

//int CESDecoder::IsOpened(void)
//{
//	return (m_hMsgWnd != NULL);
//}

int CESDecoder::Close()
{
	if (m_dwStreamType & STREAM_FILE)
	{
		if (m_hFile != -1)
		{
			_close(m_hFile);
			m_hFile = -1;
		}
		memset(m_pszFileName, sizeof(m_pszFileName), 0x00);
	}

	return 0;
}

int	CESDecoder::GetTitle(char* pszTitle, int strLen)
{
	int rtcode = ESDECODER_NO_ERROR;

	if (strlen(m_pszFileName) > 0)
	{
		strcpy_s(pszTitle, strLen, m_pszFileName);
	}
	else
	{
		rtcode = ESDECODER_UNKNOWN_ERROR;
	}

	return rtcode;
}

// pos -- first ts packet byte position
//int CESDecoder::WriteTSPacket(U8* buf, int length, S64 pos)
//{
//	int		rtcode = NO_ERROR;
//	U8*		packet_buf = buf;
//	U8*		payload_buf;
//	int		payload_length;
//	S8		transport_error_indicator;
//	S8		transport_scrambling_control;
//	S8		payload_unit_start_indicator;
//	S8		adaptation_field_control;	
//	U8		adaptation_field_length;
//	U8		pes_header_length;
//	U16		usNewPID;
//
////	m_nPos = pos;
//
//	if (IsOpened() && (m_nStreamType == STREAM_TS))
//	{
//		usNewPID = ((packet_buf[1] & 0x1f) << 8) | packet_buf[2];
//
//		if (usNewPID == m_usES_PID)
//		{
//			payload_buf = NULL;
//			payload_length = 0;
//		
//			transport_error_indicator = (packet_buf[1] & 0x80) >> 7;
//
//			if (transport_error_indicator == 0)				//packet with no error
//			{
//				transport_scrambling_control = (packet_buf[3] & 0xC0) >> 6;
//
//				if (transport_scrambling_control == 0)		//not scrambled
//				{
//					payload_unit_start_indicator = (packet_buf[1] & 0x40) >> 6;
//
//					adaptation_field_control = (packet_buf[3] & 0x30) >> 4;
//					if ((adaptation_field_control & 0x02) != 0x00)					/*has adaptation field*/
//					{
//						adaptation_field_length = packet_buf[4];
//						payload_buf = packet_buf + 5 + adaptation_field_length;
//					}
//					else
//					{
//						adaptation_field_length = 0;
//						payload_buf = packet_buf + 4;
//					}
//
//					payload_length = 188 - (payload_buf - packet_buf);
//
//					//get pes stream
//
//					if (payload_unit_start_indicator)
//					{
//						//discard the pes header
//						pes_header_length = payload_buf[8] + 9;
//			
//	//					if (av_pes_parse != NULL)
//	//					{
//	//						av_pes_parse((PVOID*)this, payload_buf, pes_header_length, m_nPos);
//	//					}
//						PES_decode_header(payload_buf, pes_header_length, &m_PES_packet);
//						if (m_hMsgWnd != NULL)
//						{
//							::SendMessage(m_hMsgWnd, WM_UPDATE_PES_HEADER, (WPARAM)&m_PES_packet, m_PES_packet.stream_id);
//						}
//		
//						payload_buf += pes_header_length;
//						payload_length -= pes_header_length;
//					}
//
//					//copy data
//					WriteESData(payload_buf, payload_length);
//				}
//			}
//		}
//	}
//	else
//	{
//	}
//
//	return rtcode;
//}

int CESDecoder::WriteTSPacket(transport_packet_t* pts_packet, int64_t pos)
{
	int				rtcode = NO_ERROR;
	//uint8_t*		payload_buf;
	//int			payload_length;
	//uint8_t		pes_header_length;

	//	m_nPos = pos;

	//if (IsOpened() && (m_nStreamType == STREAM_TS))
	//{

	//	if (pts_packet->PID == m_usES_PID)
	//	{

	//		if (pts_packet->transport_error_indicator == 0)				//packet with no error
	//		{
	//			if (pts_packet->transport_scrambling_control == 0)		//not scrambled
	//			{
	//				payload_buf = pts_packet->payload_buf;
	//				payload_length = pts_packet->payload_length;

	//				//get pes stream

	//				if (pts_packet->payload_unit_start_indicator)
	//				{
	//					//discard the pes header
	//					pes_header_length = payload_buf[8] + 9;

	//					MPEG_decode_PES_packet(payload_buf, pes_header_length, &m_PES_packet);
	//					//if (m_hMsgWnd != NULL)
	//					//{
	//					//	::SendMessage(m_hMsgWnd, WM_UPDATE_PES_HEADER, (WPARAM)&m_PES_packet, m_PES_packet.stream_id);
	//					//}

	//					payload_buf += pes_header_length;
	//					payload_length -= pes_header_length;
	//				}

	//				//copy data
	//				WriteESData(payload_buf, payload_length);
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//}

	return rtcode;
}

int CESDecoder::WriteESData(uint8_t* buf, int length)
{
	int		rtcode = 0;
	//int		move_length;
	//int		copy_length;

	//if ((m_fifo.wrptr + length) > m_fifo.endptr)
	//{
	//	//move old data
	//	move_length = m_fifo.rdptr - m_fifo.buf;
	//	
	//	if (move_length > 0)
	//	{
	//		copy_length = m_fifo.wrptr - m_fifo.rdptr;
	//		memcpy(m_fifo.buf, m_fifo.rdptr, copy_length);
	//		m_fifo.rdptr = m_fifo.buf;
	//		m_fifo.wrptr = m_fifo.buf + copy_length;
	//	}

	//	//搬移后仍然溢出，则丢弃尾部数据
	//	if ((m_fifo.wrptr + length) > m_fifo.endptr)
	//	{
	//		m_fifo.wrptr -= length;
	//		m_fifo.bitleft -= (length << 3);
	//	}
	//}

	//memcpy(m_fifo.wrptr, buf, length);
	//m_fifo.wrptr += length;
	//m_fifo.bitleft += (length << 3);

	//if (av_es_parse != NULL)
	//{
	//	av_es_parse((PVOID*)this);
	//}

	//m_nWriteCount ++;

	return rtcode;
}

int CESDecoder::SetParams(uint16_t ES_PID, uint16_t PCR_PID)
{
//	usNewPID = ((packet_buf[1] & 0x1f) << 8) | packet_buf[2];
//	if (usNewPID != m_usPID)
//	{
//		m_usPID = usNewPID;

//		m_fifo.rdptr = m_fifo.wrptr = m_fifo.buf;
//		m_fifo.rdbit_offset = 0;
//		m_fifo.bitleft = 0;
//	}

	m_usES_PID = ES_PID;
	m_usPCR_PID = PCR_PID;

	return 0;
}

int CESDecoder::FillData(void)
{
	int		rtcode = -1;
	//int		move_length;
	//int		copy_length;
	//int		read_length;

	//if (m_fifo.hFile != -1)
	//{
	//	move_length = m_fifo.rdptr - m_fifo.buf;
	//	if (move_length > 0)
	//	{
	//		//move old data
	//		copy_length = m_fifo.wrptr - m_fifo.rdptr;
	//		memcpy(m_fifo.buf, m_fifo.rdptr, copy_length);

	//		m_fifo.rdptr = m_fifo.buf;
	//		m_fifo.wrptr = m_fifo.buf + copy_length;
	//	}

	//	copy_length = m_fifo.endptr - m_fifo.wrptr;

	//	if (copy_length > 0)
	//	{
	//		//read new data
	//		read_length = _read(m_fifo.hFile, m_fifo.wrptr, copy_length);
	//		if (read_length == 0)		//eof
	//		{
	//			_lseeki64(m_fifo.hFile, 0, 0);
	//			read_length = _read(m_fifo.hFile, m_fifo.wrptr, copy_length);
	//		}

	//		m_fifo.wrptr += read_length;
	//		m_fifo.bitleft += (read_length << 3);

	//		rtcode = copy_length;
	//	}
	//}

	return rtcode;
}

//void CESDecoder::Reset(void)
//{
//	m_nTriggerType = ES_TRIGGER_ONESHOT;		
//	m_bTriggering = 0;
//
//	m_nPrePcrPos = -1;
//	m_nPrePtsPos = -1;
//	m_nPreDtsPos = -1;
//
//	m_usES_PID = 0xffff;
//	m_usPCR_PID = 0xffff;
//}

//返回旧的触发类型
//int CESDecoder::SetTrigger(int nTriggerType)
//{
//	int rtcode = 0;
//	
//	//rtcode = m_nTriggerType;
//
//	//m_nTriggerType = nTriggerType;
//
//	return rtcode;
//}

//int	CESDecoder::Preview_SetFileRatio(int nPercent)
//{
//	int64_t	    offset;
//	float		dRatio = nPercent / 100.0f;
//
//	offset = m_nFileStartPos + (int64_t)((m_nFileTotalSize - m_nFileStartPos) * dRatio);
//
//	//if (m_hFile != -1)
//	//{
//	//	_lseek(m_hFile, offset, SEEK_SET);
//	//}
//
//	//m_fifo.rdptr = m_fifo.wrptr = m_fifo.buf;
//	//m_fifo.bitleft = 0;
//	//m_fifo.rdbit_offset = 0;
//	//m_fifo.endptr = m_fifo.buf + FIFO_BUF_SIZE;
//
//	return 0;
//}

//int	CESDecoder::Preview_EOF(void)
//{
//	int bEOF = 0;
//
//	bEOF = (Preview_GetFilePos() >= m_nFileTotalSize) ? 1 : 0;
//
//	return bEOF;
//}
//
//int	CESDecoder::Preview_GetFilePos(void)
//{
//	int filepos = 0;
//	
//	//if (m_hFile != -1)
//	//{
//	//	filepos = _tell(m_hFile);
//	//}
//
//	return filepos;
//}
//
//int CESDecoder::Preview_GetFileRatio(void)
//{
//	int64_t filepos = 0;
//	int percent = 0;
//
//	//filepos = Preview_GetFilePos() - (m_fifo.endptr - m_fifo.rdptr);
//
//	//if (m_nFileTotalSize > 0)
//	//{
//	//	percent = (int)((filepos - m_nFileStartPos) * 100.0f / (m_nFileTotalSize - m_nFileStartPos));
//	//}
//
//	return percent;
//}

int CESDecoder::Preview_FirstPicture(void)
{
	return -1;
}

int CESDecoder::Preview_LastPicture(void)
{
	return -1;
}

int CESDecoder::Preview_Forward1Picture(void)
{
	return -1;
}

int CESDecoder::Preview_Backward1Picture(void)
{
	return -1;
}

int CESDecoder::Preview_ForwardNPicture(int n)
{
	return -1;
}

int CESDecoder::Preview_BackwardNPicture(int n)
{
	return -1;
}

int CESDecoder::Preview_SeekAtPercent(int nPercent)
{
	return -1;
}

int CESDecoder::Preview_beEOF(void)
{
	return -1;
}