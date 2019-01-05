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
			//section语法分析
		case SECTION_PARSE_PARAMETER_ERROR:
			sprintf_s(pszText, size, "section解析参数错误(代码:0x%08X)", error_code);
			break;
		case SECTION_PARSE_CRC_ERROR:
			sprintf_s(pszText, size, "section解析CRC错误(代码:0x%08X)", error_code);
			break;
		case SECTION_PARSE_SYNTAX_ERROR:
			sprintf_s(pszText, size, "section解析语法错误(代码:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_SECTION_NUMBER_ERROR:
			sprintf_s(pszText, size, "section解析段序号错误(代码:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_MEMORY_ERROR:
			sprintf_s(pszText, size, "section解析内存分配错误(代码:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_DUPLICATED_SECTION:
			sprintf_s(pszText, size, "section解析发现重复的段(代码:0x%08X)", error_code);
			break;
		case MIDDLEWARE_PSISI_VERSION_CHANGE:
			sprintf_s(pszText, size, "section版本号变更(代码:0x%08X)", error_code);
			break;
		case SECTION_PARSE_LENGTH_ERROR:
			sprintf_s(pszText, size, "section段长度错误(代码:0x%08X)", error_code);
			break;
//		case SECTION_PARSE_SECTION_NOT_COMPLETED:
//			sprintf_s(pszText, size, "section未搜集齐错误(代码:0x%08X)", error_code);
//			break;
			//section拼接
		case SECTION_SPLICE_TRANSPORT_ERROR:
			sprintf_s(pszText, size, "section拼接TS包传输错误(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_CONTINUITY_ERROR:
			sprintf_s(pszText, size, "section拼接TS包序号错误(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_PID_ERROR:
			sprintf_s(pszText, size, "section拼接PID冲突错误(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_SECTION_LENGTH_ERROR:
			sprintf_s(pszText, size, "section拼接长度错误(代码:0x%08X)", error_code);
			break;
		//case SECTION_SPLICE_PARAMETER_ERROR:
		//	sprintf_s(pszText, size, "section拼接参数错误(代码:0x%08X)", error_code);
		//	break;
		case SECTION_SPLICE_SCRAMBLED_PACKET:
			sprintf_s(pszText, size, "section拼接TS包加扰(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_FOLLOW_PACKET:
			sprintf_s(pszText, size, "section拼接未等到足够数据(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_INVALID_PACKET:
			sprintf_s(pszText, size, "section拼接无效TS包(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_MEMORY_ERROR:
			sprintf_s(pszText, size, "section拼接内存错误(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_SYNTAX_ERROR:
			sprintf_s(pszText, size, "section拼接语法错误(代码:0x%08X)", error_code);
			break;
		case SECTION_SPLICE_DO_NOT_SYNC:
			sprintf_s(pszText, size, "section拼接TS包未同步(代码:0x%08X)", error_code);
			break;
			//TS_packet解析错误代码
		case TSPACKET_PARSE_PARAMETER_ERROR:
			sprintf_s(pszText, size, "TS解析参数错误(代码:0x%08X)", error_code);
			break;
		case TSPACKET_PARSE_SYNC_ERROR:
			sprintf_s(pszText, size, "TS解析同步错误(代码:0x%08X)", error_code);
			break;
		case TSPACKET_PARSE_SYNTAX_ERROR:
			sprintf_s(pszText, size, "TS解析语法错误(代码:0x%08X)", error_code);
			break;
		default:
			sprintf_s(pszText, size, "未知错误(代码:0x%08X)", error_code);
			break;
		}
	}		
	return rtcode;
}

