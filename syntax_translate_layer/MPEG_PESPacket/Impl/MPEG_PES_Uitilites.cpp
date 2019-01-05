#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "../Include/MPEG_stream_id.h"
#include "../Include/MPEG_PES_ErrorCode.h"
#include "../Include/MPEG_PES_Utilities.h"

/////////////////////////////////////////////
int MPEG_PES_NumericCoding2Text_StreamID(uint8_t stream_id, char* pszText, int size)
{
	int		rtcode = PES_PACKET_NO_ERROR;

	if (pszText != NULL)
	{
		if (stream_id == PROGRAM_STREAM_MAP)		//0xBC
		{
			sprintf_s(pszText, size, "program_stream_map (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == PRIVATE_STREAM_1)			//0xBD
		{
			sprintf_s(pszText, size, "private_stream_1 (stream_id=0x%02X)", stream_id);				//AC-3, DRA
		}
		else if (stream_id == PADDING_STREAM)				//0xBE
		{
			sprintf_s(pszText, size, "padding_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == PRIVATE_STREAM_2)				//0xBF
		{
			sprintf_s(pszText, size, "private_stream_2 (stream_id=0x%02X)", stream_id);
		}
		else if ((stream_id >= AUDIO_STREAM_MIN) && (stream_id <= AUDIO_STREAM_MAX))
		{
			sprintf_s(pszText, size, "13818-3/11172-3/13818-7/14496-3 audio stream (stream_id=0x%02X)", stream_id);
		}
		else if ((stream_id >= VIDEO_STREAM_MIN) && (stream_id <= VIDEO_STREAM_MAX))
		{
			sprintf_s(pszText, size, "13818-2/11172-2/14496-2/14496-10 video stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == ECM_STREAM)
		{
			sprintf_s(pszText, size, "ECM_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == EMM_STREAM)
		{
			sprintf_s(pszText, size, "EMM_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == DSMCC_STREAM)
		{
			sprintf_s(pszText, size, "13818-1 Annex A or 13818-6_DSMCC_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == ISO_13522_STREAM)
		{
			sprintf_s(pszText, size, "13522_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == TYPE_A_STREAM)
		{
			sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type A (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == TYPE_B_STREAM)
		{
			sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type B (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == TYPE_C_STREAM)
		{
			sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type C (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == TYPE_D_STREAM)
		{
			sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type D (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == TYPE_E_STREAM)
		{
			sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type E (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == ANCILLARY_STREAM)
		{
			sprintf_s(pszText, size, "ancillary_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == RESERVED_DATA_STREAM_A)
		{
			sprintf_s(pszText, size, "14496-1_SL-packetized_stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == RESERVED_DATA_STREAM_B)
		{
			sprintf_s(pszText, size, "14496-1_FlexMux_stream (stream_id=0x%02X)", stream_id);
		}
		else if ((stream_id >= 0xFC) && (stream_id <= 0xFE))
		{
			sprintf_s(pszText, size, "reserved data stream (stream_id=0x%02X)", stream_id);
		}
		else if (stream_id == PROGRAM_STREAM_DIRECTORY)
		{
			sprintf_s(pszText, size, "program_stream_directory (stream_id=0x%02X)", stream_id);
		}
		else
		{
			sprintf_s(pszText, size, "unknown stream (stream_id=0x%02X)", stream_id);
			rtcode = PES_PACKET_UNKNOWN_ERROR;
		}

		rtcode = PES_PACKET_NO_ERROR;
	}
	else
	{
		rtcode = PES_PACKET_PARAMETER_ERROR;
	}

	return rtcode;
}


