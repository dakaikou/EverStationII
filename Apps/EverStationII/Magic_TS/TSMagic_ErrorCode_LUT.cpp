#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "TSMagic_ErrorCode.h"

#include "MiddleWare\MiddleWare_TS_PayloadSplicer\Include\MiddleWare_SectionSplicer_ErrorCode.h"
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "syntax_translate_layer\MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_TSPacket\Include\Mpeg2_TS_ErrorCode.h"

int	TSMagic_ErrorCodeLookup(uint32_t error_code, char* pszText, int size)
{
	int		rtcode = 0;
	
	if (pszText != NULL)
	{
		switch (error_code)
		{
			//section�﷨����
		case SECTION_PARSE_PARAMETER_ERROR:
			sprintf_s(pszText, size, "section������������(����:0x%08X)", error_code);
			break;
		case SECTION_PARSE_CRC_ERROR:
			sprintf_s(pszText, size, "section����CRC����(����:0x%08X)", error_code);
			break;
		case SECTION_PARSE_SYNTAX_ERROR:
			sprintf_s(pszText, size, "section�����﷨����(����:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_SECTION_NUMBER_ERROR:
			sprintf_s(pszText, size, "section��������Ŵ���(����:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_MEMORY_ERROR:
			sprintf_s(pszText, size, "section�����ڴ�������(����:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_DUPLICATED_SECTION:
			sprintf_s(pszText, size, "section���������ظ��Ķ�(����:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_VERSION_CHANGE:
			sprintf_s(pszText, size, "section�汾�ű��(����:0x%08X)", error_code);
			break;
		case SECTION_PARSE_LENGTH_ERROR:
			sprintf_s(pszText, size, "section�γ��ȴ���(����:0x%08X)", error_code);
			break;
//		case SECTION_PARSE_SECTION_NOT_COMPLETED:
//			sprintf_s(pszText, size, "sectionδ�Ѽ������(����:0x%08X)", error_code);
//			break;
			//sectionƴ��
		case SECTION_SPLICE_TRANSPORT_ERROR:
			sprintf_s(pszText, size, "sectionƴ��TS���������(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_CONTINUITY_ERROR:
			sprintf_s(pszText, size, "sectionƴ��TS����Ŵ���(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_PID_ERROR:
			sprintf_s(pszText, size, "sectionƴ��PID��ͻ����(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_SECTION_LENGTH_ERROR:
			sprintf_s(pszText, size, "sectionƴ�ӳ��ȴ���(����:0x%08X)", error_code);
			break;
		//case SECTION_SPLICE_PARAMETER_ERROR:
		//	sprintf_s(pszText, size, "sectionƴ�Ӳ�������(����:0x%08X)", error_code);
		//	break;
		case SECTION_SPLICE_SCRAMBLED_PACKET:
			sprintf_s(pszText, size, "sectionƴ��TS������(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_FOLLOW_PACKET:
			sprintf_s(pszText, size, "sectionƴ��δ�ȵ��㹻����(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_INVALID_PACKET:
			sprintf_s(pszText, size, "sectionƴ����ЧTS��(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_MEMORY_ERROR:
			sprintf_s(pszText, size, "sectionƴ���ڴ����(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_SYNTAX_ERROR:
			sprintf_s(pszText, size, "sectionƴ���﷨����(����:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_DO_NOT_SYNC:
			sprintf_s(pszText, size, "sectionƴ��TS��δͬ��(����:0x%08X)", error_code);
			break;
			//TS_packet�����������
		case TSPACKET_PARSE_PARAMETER_ERROR:
			sprintf_s(pszText, size, "TS������������(����:0x%08X)", error_code);
			break;
		case TSPACKET_PARSE_SYNC_ERROR:
			sprintf_s(pszText, size, "TS����ͬ������(����:0x%08X)", error_code);
			break;
		case TSPACKET_PARSE_SYNTAX_ERROR:
			sprintf_s(pszText, size, "TS�����﷨����(����:0x%08X)", error_code);
			break;
		default:
			sprintf_s(pszText, size, "δ֪����(����:0x%08X)", error_code);
			break;
		}
	}		
	return rtcode;
}

