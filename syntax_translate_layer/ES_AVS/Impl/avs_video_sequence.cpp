#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../common/BitStream.h"

#include "../Include/avs_video_sequence.h"
#include "avs_video_vlc.h"
#include "avs_video_table.h"

//static AVS_VLC_Table_t					 AVS_LUT_VLC_Table[19][64];
//
//static AVS_video_sequence_header_t		 avs_video_sequence_header;
//static AVS_i_picture_header_t			 avs_i_picture_header;
//static AVS_pb_picture_header_t			 avs_pb_picture_header;
//static AVS_sequence_display_extension_t	 avs_sequence_display_extension;
//static AVS_picture_display_extension_t	 avs_picture_display_extension;
//static AVS_copyright_extension_t		 avs_copyright_extension;
//static AVS_camera_parameters_extension_t avs_camera_parameters_extension;
//static AVS_user_data_t					 avs_user_data;
//static AVS_slice_t						 avs_slice;
//static AVS_macroblock_t					 avs_macroblock;
//
//static Video_decode_info_t				 avs_decode_info;
//
//static uint8_t*			pucForwardFrame[3] = {NULL, NULL, NULL};							//0 -- Y, 1 -- Cb, 2 -- Cr
//static uint8_t*			pucBackwardFrame[3] = {NULL, NULL, NULL};
//static uint8_t*			pucAuxFrame[3] = {NULL, NULL, NULL};
//
//static uint8_t*			pucDisplayFrame[3] = {NULL, NULL, NULL};					//当前显示帧
//static uint8_t*			pucDecodeFrame[3] = {NULL, NULL, NULL};						//当前解码帧
//
//static uint8_t*			render_avs_video_buf_rgb888;
//static int			render_avs_video_buf_size;
//
//static int			MbWidth;								//avs_video_sequence_header & sequence_extension	
//static int			MbHeight;
//static int			MbCount;
//static int			HalfMbCount;
//
//static uint8_t			PictureType;
//static uint8_t			PictureStructure;
//static uint8_t			PictureReferenceFlag;
//
//static int			PreviousQP;
//static int			CurrentQP;
//static uint8_t			FixedQP;
//static int			SkipMbCount;
//
//static int			MbIndex;
//static int			SkipModeFlag;
//
//static int			MbType;
//static int			MvNum;
//static int			MbPredMode;
//static int			MbWeightingFlag;
//
//static uint8_t			PatternCode[12];
//static S16			QuantCoeffArray[64];
//static int			alternate_scan = 0;
//
//static int			NumberOfReference;				//max value = 4
//
//static int			gbInitDecoder = 0;
//static HANDLE		ghAccessEvent = NULL;
//
//#if AVS_VID_DECODER_DEBUG
//
//static FILE*	m_hDebugFile = NULL;
//static FILE*	m_hDebugYUV = NULL;
//static int		m_nDebugFrameCount;
//
//#endif
//
//void avs_video_init(void)
//{
//	memset(&avs_video_sequence_header, 0, sizeof(AVS_video_sequence_header_t));
//	memset(&avs_i_picture_header, 0, sizeof(AVS_i_picture_header_t));
//
//	avs_video_make_vlc_table();
//
////#if VID_YUV2RGB_FastAlgorithm
////	Initialize_yuv2rgb_LUT();
////#endif
//
//	ghAccessEvent = ::CreateEvent(NULL, 0, 1, NULL);
//}
//
//void avs_video_term(void)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	::CloseHandle(ghAccessEvent);
//	ghAccessEvent = NULL;
//}
//
//void avs_video_open(int* pwidth, int* pheight, S8* pszFourCC, int size)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	if (!gbInitDecoder)
//	{
//		MbWidth = (avs_video_sequence_header.horizontal_size + 15) / 16;
//
//		if (avs_video_sequence_header.progressive_sequence == 1)
//		{
//			MbHeight = (avs_video_sequence_header.vertical_size + 15) / 16;
//		}
//		else
//		{
//			MbHeight = ((avs_video_sequence_header.vertical_size + 31) / 32) * 2;
//		}
//
//		MbCount = MbWidth * MbHeight;
//		HalfMbCount = (MbCount >> 1);
//
//		avs_decode_info.chroma_format = avs_video_sequence_header.chroma_format;
//
//		avs_decode_info.luma_width = (MbWidth << 4);
//		avs_decode_info.luma_height = (MbHeight << 4);					//??????
//		avs_decode_info.luma_pix_count = avs_decode_info.luma_width * avs_decode_info.luma_height;
//		avs_decode_info.luma_buf_size = avs_decode_info.luma_pix_count;
//
////		if (avs_decode_info.luma_width >= 1000)
////		{
////			avs_decode_info.decimate = 2;
////		}
////		else
////		{
////			avs_decode_info.decimate = 1;
////		}
//
//		avs_decode_info.display_width = avs_decode_info.luma_width;
//		avs_decode_info.display_height = avs_decode_info.luma_height;
////		avs_decode_info.graph_width = avs_decode_info.display_width / avs_decode_info.decimate;
////		avs_decode_info.graph_height = avs_decode_info.display_height / avs_decode_info.decimate;
//
//		avs_decode_info.chroma_width = (avs_video_sequence_header.chroma_format == CHROMA_FORMAT_4_4_4) ? avs_decode_info.luma_width : (avs_decode_info.luma_width >> 1);
//		avs_decode_info.chroma_height = (avs_video_sequence_header.chroma_format != CHROMA_FORMAT_4_2_0) ? avs_decode_info.luma_height : (avs_decode_info.luma_height >> 1);
//		avs_decode_info.chroma_pix_count = avs_decode_info.chroma_width * avs_decode_info.chroma_height;
//		avs_decode_info.chroma_buf_size = avs_decode_info.chroma_pix_count;
//
//		if (pucForwardFrame[CC_Y] != NULL)
//		{
//			free(pucForwardFrame[CC_Y]);
//			pucForwardFrame[CC_Y] = NULL;
//		}
//
//		pucForwardFrame[CC_Y] = (uint8_t*)malloc(avs_decode_info.luma_buf_size);
//		memset(pucForwardFrame[CC_Y], 0x00, avs_decode_info.luma_buf_size);
//
//		if (pucForwardFrame[CC_Cb] != NULL)
//		{
//			free(pucForwardFrame[CC_Cb]);
//			pucForwardFrame[CC_Cb] = NULL;
//		}
//
//		pucForwardFrame[CC_Cb] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucForwardFrame[CC_Cb], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucForwardFrame[CC_Cr] != NULL)
//		{
//			free(pucForwardFrame[CC_Cr]);
//			pucForwardFrame[CC_Cr] = NULL;
//		}
//
//		pucForwardFrame[CC_Cr] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucForwardFrame[CC_Cr], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucBackwardFrame[CC_Y] != NULL)
//		{
//			free(pucBackwardFrame[CC_Y]);
//			pucBackwardFrame[CC_Y] = NULL;
//		}
//
//		pucBackwardFrame[CC_Y] = (uint8_t*)malloc(avs_decode_info.luma_buf_size);
//		memset(pucBackwardFrame[CC_Y], 0x00, avs_decode_info.luma_buf_size);
//
//		if (pucBackwardFrame[CC_Cb] != NULL)
//		{
//			free(pucBackwardFrame[CC_Cb]);
//			pucBackwardFrame[CC_Cb] = NULL;
//		}
//
//		pucBackwardFrame[CC_Cb] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucBackwardFrame[CC_Cb], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucBackwardFrame[CC_Cr] != NULL)
//		{
//			free(pucBackwardFrame[CC_Cr]);
//			pucBackwardFrame[CC_Cr] = NULL;
//		}
//
//		pucBackwardFrame[CC_Cr] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucBackwardFrame[CC_Cr], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucAuxFrame[CC_Y] != NULL)
//		{
//			free(pucAuxFrame[CC_Y]);
//			pucAuxFrame[CC_Y] = NULL;
//		}
//
//		pucAuxFrame[CC_Y] = (uint8_t*)malloc(avs_decode_info.luma_buf_size);
//		memset(pucAuxFrame[CC_Y], 0x00, avs_decode_info.luma_buf_size);
//
//		if (pucAuxFrame[CC_Cb] != NULL)
//		{
//			free(pucAuxFrame[CC_Cb]);
//			pucAuxFrame[CC_Cb] = NULL;
//		}
//
//		pucAuxFrame[CC_Cb] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucAuxFrame[CC_Cb], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucAuxFrame[CC_Cr] != NULL)
//		{
//			free(pucAuxFrame[CC_Cr]);
//			pucAuxFrame[CC_Cr] = NULL;
//		}
//
//		pucAuxFrame[CC_Cr] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucAuxFrame[CC_Cr], 0x00, avs_decode_info.chroma_buf_size);
//
//		pucDisplayFrame[CC_Y] = NULL;
//		pucDisplayFrame[CC_Cb] = NULL;
//		pucDisplayFrame[CC_Cr] = NULL;
//
//		pucDecodeFrame[CC_Y] = NULL;
//		pucDecodeFrame[CC_Cb] = NULL;
//		pucDecodeFrame[CC_Cr] = NULL;
//
////		render_avs_video_buf_size = avs_decode_info.graph_width * avs_decode_info.graph_height * 3;
//
////		if (render_avs_video_buf_rgb888 != NULL)
////		{
////			free(render_avs_video_buf_rgb888);
////			render_avs_video_buf_rgb888 = NULL;
////		}
//
////		render_avs_video_buf_rgb888 = (uint8_t*)malloc(render_avs_video_buf_size);
////		memset(render_avs_video_buf_rgb888, 0x00, render_avs_video_buf_size);
//
//#if AVS_VID_DECODER_DEBUG
//		m_nDebugFrameCount = 0;
//#endif
//
//		gbInitDecoder = 1;
//	}
//
//	if (pwidth != NULL)
//	{
//		*pwidth = avs_decode_info.display_width;
//	}
//	if (pheight != NULL)
//	{
//		*pheight = avs_decode_info.display_height;
//	}
//	if (pszFourCC != NULL)
//	{
//		strcpy_s(pszFourCC, size, "YV12");
//	}
//
//	avs_decode_info.graph_with_grid = 0;
//
//	::SetEvent(ghAccessEvent);
//}
//
//void avs_video_reset(int* pwidth, int* pheight)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	if (gbInitDecoder)
//	{
//		MbCount = MbWidth * MbHeight;
//		HalfMbCount = (MbCount >> 1);
//
//		avs_decode_info.chroma_format = avs_video_sequence_header.chroma_format;
//
//		avs_decode_info.luma_width = (MbWidth << 4);
//		avs_decode_info.luma_height = (MbHeight << 4);					//??????
//		avs_decode_info.luma_pix_count = avs_decode_info.luma_width * avs_decode_info.luma_height;
//		avs_decode_info.luma_buf_size = avs_decode_info.luma_pix_count;
//
////		if (avs_decode_info.luma_width >= 1000)
////		{
////			avs_decode_info.decimate = 2;
////		}
////		else
////		{
////			avs_decode_info.decimate = 1;
////		}
//
//		avs_decode_info.display_width = avs_decode_info.luma_width;
//		avs_decode_info.display_height = avs_decode_info.luma_height;
////		avs_decode_info.graph_width = avs_decode_info.display_width / avs_decode_info.decimate;
////		avs_decode_info.graph_height = avs_decode_info.display_height / avs_decode_info.decimate;
//
//		avs_decode_info.chroma_width = (avs_video_sequence_header.chroma_format == CHROMA_FORMAT_4_4_4) ? avs_decode_info.luma_width : (avs_decode_info.luma_width >> 1);
//		avs_decode_info.chroma_height = (avs_video_sequence_header.chroma_format != CHROMA_FORMAT_4_2_0) ? avs_decode_info.luma_height : (avs_decode_info.luma_height >> 1);
//		avs_decode_info.chroma_pix_count = avs_decode_info.chroma_width * avs_decode_info.chroma_height;
//		avs_decode_info.chroma_buf_size = avs_decode_info.chroma_pix_count;
//
//		if (pucForwardFrame[CC_Y] != NULL)
//		{
//			free(pucForwardFrame[CC_Y]);
//			pucForwardFrame[CC_Y] = NULL;
//		}
//
//		pucForwardFrame[CC_Y] = (uint8_t*)malloc(avs_decode_info.luma_buf_size);
//		memset(pucForwardFrame[CC_Y], 0x00, avs_decode_info.luma_buf_size);
//
//		if (pucForwardFrame[CC_Cb] != NULL)
//		{
//			free(pucForwardFrame[CC_Cb]);
//			pucForwardFrame[CC_Cb] = NULL;
//		}
//
//		pucForwardFrame[CC_Cb] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucForwardFrame[CC_Cb], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucForwardFrame[CC_Cr] != NULL)
//		{
//			free(pucForwardFrame[CC_Cr]);
//			pucForwardFrame[CC_Cr] = NULL;
//		}
//
//		pucForwardFrame[CC_Cr] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucForwardFrame[CC_Cr], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucBackwardFrame[CC_Y] != NULL)
//		{
//			free(pucBackwardFrame[CC_Y]);
//			pucBackwardFrame[CC_Y] = NULL;
//		}
//
//		pucBackwardFrame[CC_Y] = (uint8_t*)malloc(avs_decode_info.luma_buf_size);
//		memset(pucBackwardFrame[CC_Y], 0x00, avs_decode_info.luma_buf_size);
//
//		if (pucBackwardFrame[CC_Cb] != NULL)
//		{
//			free(pucBackwardFrame[CC_Cb]);
//			pucBackwardFrame[CC_Cb] = NULL;
//		}
//
//		pucBackwardFrame[CC_Cb] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucBackwardFrame[CC_Cb], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucBackwardFrame[CC_Cr] != NULL)
//		{
//			free(pucBackwardFrame[CC_Cr]);
//			pucBackwardFrame[CC_Cr] = NULL;
//		}
//
//		pucBackwardFrame[CC_Cr] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucBackwardFrame[CC_Cr], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucAuxFrame[CC_Y] != NULL)
//		{
//			free(pucAuxFrame[CC_Y]);
//			pucAuxFrame[CC_Y] = NULL;
//		}
//
//		pucAuxFrame[CC_Y] = (uint8_t*)malloc(avs_decode_info.luma_buf_size);
//		memset(pucAuxFrame[CC_Y], 0x00, avs_decode_info.luma_buf_size);
//
//		if (pucAuxFrame[CC_Cb] != NULL)
//		{
//			free(pucAuxFrame[CC_Cb]);
//			pucAuxFrame[CC_Cb] = NULL;
//		}
//
//		pucAuxFrame[CC_Cb] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucAuxFrame[CC_Cb], 0x00, avs_decode_info.chroma_buf_size);
//
//		if (pucAuxFrame[CC_Cr] != NULL)
//		{
//			free(pucAuxFrame[CC_Cr]);
//			pucAuxFrame[CC_Cr] = NULL;
//		}
//
//		pucAuxFrame[CC_Cr] = (uint8_t*)malloc(avs_decode_info.chroma_buf_size);
//		memset(pucAuxFrame[CC_Cr], 0x00, avs_decode_info.chroma_buf_size);
//
//		pucDisplayFrame[CC_Y] = NULL;
//		pucDisplayFrame[CC_Cb] = NULL;
//		pucDisplayFrame[CC_Cr] = NULL;
//
//		pucDecodeFrame[CC_Y] = NULL;
//		pucDecodeFrame[CC_Cb] = NULL;
//		pucDecodeFrame[CC_Cr] = NULL;
//
////		render_avs_video_buf_size = avs_decode_info.graph_width * avs_decode_info.graph_height * 3;
//
////		if (render_avs_video_buf_rgb888 != NULL)
////		{
////			free(render_avs_video_buf_rgb888);
////			render_avs_video_buf_rgb888 = NULL;
////		}
//
////		render_avs_video_buf_rgb888 = (uint8_t*)malloc(render_avs_video_buf_size);
////		memset(render_avs_video_buf_rgb888, 0x00, render_avs_video_buf_size);
//	}
//
////	if (pwidth != NULL)
////	{
////		*pwidth = avs_decode_info.graph_width;
////	}
////	if (pheight != NULL)
////	{
////		*pheight = avs_decode_info.graph_height;
////	}
//
//	avs_decode_info.graph_with_grid = 0;
//
//	::SetEvent(ghAccessEvent);
//}
//
//void avs_video_close(void)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	if (gbInitDecoder)
//	{
//		gbInitDecoder = 0;
//
//		if (pucForwardFrame[CC_Y])
//		{
//			free(pucForwardFrame[CC_Y]);
//			pucForwardFrame[CC_Y] = NULL;
//		}
//
//		if (pucForwardFrame[CC_Cb])
//		{
//			free(pucForwardFrame[CC_Cb]);
//			pucForwardFrame[CC_Cb] = NULL;
//		}
//
//		if (pucForwardFrame[CC_Cr])
//		{
//			free(pucForwardFrame[CC_Cr]);
//			pucForwardFrame[CC_Cr] = NULL;
//		}
//
//		if (pucBackwardFrame[CC_Y])
//		{
//			free(pucBackwardFrame[CC_Y]);
//			pucBackwardFrame[CC_Y] = NULL;
//		}
//
//		if (pucBackwardFrame[CC_Cb])
//		{
//			free(pucBackwardFrame[CC_Cb]);
//			pucBackwardFrame[CC_Cb] = NULL;
//		}
//	
//		if (pucBackwardFrame[CC_Cr])
//		{
//			free(pucBackwardFrame[CC_Cr]);
//			pucBackwardFrame[CC_Cr] = NULL;
//		}
//
//		if (pucAuxFrame[CC_Y])
//		{
//			free(pucAuxFrame[CC_Y]);
//			pucAuxFrame[CC_Y] = NULL;
//		}
//
//		if (pucAuxFrame[CC_Cb])
//		{
//			free(pucAuxFrame[CC_Cb]);
//			pucAuxFrame[CC_Cb] = NULL;
//		}
//		
//		if (pucAuxFrame[CC_Cr])
//		{
//			free(pucAuxFrame[CC_Cr]);
//			pucAuxFrame[CC_Cr] = NULL;
//		}
//
//		if (render_avs_video_buf_rgb888 != NULL)
//		{
//			free(render_avs_video_buf_rgb888);
//			render_avs_video_buf_rgb888 = NULL;
//		}
//
//		pucDisplayFrame[CC_Y] = NULL;
//		pucDisplayFrame[CC_Cb] = NULL;
//		pucDisplayFrame[CC_Cr] = NULL;
//
//		pucDecodeFrame[CC_Y] = NULL;
//		pucDecodeFrame[CC_Cb] = NULL;
//		pucDecodeFrame[CC_Cr] = NULL;
//
//#if AVS_VID_DECODER_DEBUG
//
//		if (m_hDebugFile)
//		{
//			fclose(m_hDebugFile);
//			m_hDebugFile = NULL;
//		}
//
//		if (m_hDebugYUV)
//		{
//			fclose(m_hDebugYUV);
//			m_hDebugYUV = NULL;
//		}
//		
//		m_nDebugFrameCount = 0;
//#endif
//	}
//
//	avs_decode_info.luma_width = 0;
//	avs_decode_info.luma_height = 0;
//	avs_decode_info.luma_buf_size = 0;
//	avs_decode_info.luma_pix_count = 0;
//
//	avs_decode_info.chroma_width = 0;
//	avs_decode_info.chroma_height = 0;
//	avs_decode_info.chroma_buf_size = 0;
//	avs_decode_info.chroma_pix_count = 0;
//
//	MbWidth = 0;
//	MbHeight = 0;
//	MbCount = 0;
//	HalfMbCount = 0;
//
////	avs_decode_info.decimate = 1;
////	avs_decode_info.graph_width = 0;
////	avs_decode_info.graph_height = 0;
//
//	avs_decode_info.graph_with_grid = 0;
//
//	::SetEvent(ghAccessEvent);
//}
//
////int avs_video_check_padding_bits(FIFO_t* pbs)
////{
////	int rtcode = 0;
////	uint32_t nextcode = 0;
////
////	uint8_t*	rdptr = pbs->rdptr + 1;
////
////	nextcode = (rdptr[0] << 16) | (rdptr[1] << 8) | rdptr[2];
////	if (nextcode == 0x000001)
////	{
////		rtcode = 1;					//found padding bits
////	}
////
////	return rtcode;
////}
//
////int avs_video_next_start_code(FIFO_t* pbs)
////{
////	int		start_code = -1;
////	int		next_start_code = -1;
////	int		rtcode = -1;
////
////	if (pbs->bitleft > 0)
////	{
////		if (pbs->rdbit_offset > 0)
////		{
////			FIFO_Flush(pbs, (8 - pbs->rdbit_offset));
////		}
////	}
////
////	do
////	{
////		next_start_code = FIFO_Peek(pbs, 32);
////		if ((next_start_code & 0xffffff00) == 0x00000100)
////		{
////			start_code = next_start_code & 0x000000ff;
////			rtcode = start_code;
////			break;
////		}
////		else
////		{
////			FIFO_Flush(pbs, 8);
////
////			if (pbs->bitleft == 0)
////			{
//////				next_start_code = -1;
////				break;
////			}
////		}
////
////	} while (1);
////
////	return rtcode;
////}
//
////int	avs_video_scaling_list(FIFO_t* pbs, uint8_t* scalingList, int sizeOfScalingList, uint8_t* useDefaultScalingMatrixFlag)
////{
////	int lastScale = 8;
////	int nextScale = 8;
////	int delta_scale;
////	int	j;
////
////	for (j = 0; j < sizeOfScalingList; j++)
////	{
////		if (nextScale != 0)
////		{
////			delta_scale = GetBits_se(pbs);
////			nextScale = (lastScale + delta_scale + 256) % 256;
////			useDefaultScalingMatrixFlag[j] = (j == 0 && nextScale == 0);
////		}
////
////		scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
////		lastScale = scalingList[j];
////	}
////
////	return 0;
////}
//
//AVS_video_sequence_header_t* avs_video_get_video_sequence_header(void)
//{
//	return &avs_video_sequence_header;
//}
//
////int avs_video_decode_video_sequence_header(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	/*syntax part parse*/
////	avs_video_sequence_header.video_sequence_start_code = FIFO_Get(pbs, 32);
////	avs_video_sequence_header.profile_id = FIFO_Get(pbs, 8);
////	avs_video_sequence_header.level_id = FIFO_Get(pbs, 8);
////	avs_video_sequence_header.progressive_sequence = FIFO_Get(pbs, 1);
////	avs_video_sequence_header.horizontal_size = FIFO_Get(pbs, 14);
////	avs_video_sequence_header.vertical_size = FIFO_Get(pbs, 14);
////	avs_video_sequence_header.chroma_format = FIFO_Get(pbs, 2);
////	avs_video_sequence_header.sample_precision = FIFO_Get(pbs, 3);
////	avs_video_sequence_header.aspect_ratio = FIFO_Get(pbs, 4);
////	avs_video_sequence_header.frame_rate_code = FIFO_Get(pbs, 4);
////	avs_video_sequence_header.bit_rate_lower = FIFO_Get(pbs, 18);
////	avs_video_sequence_header.marker_bit0 = FIFO_Get(pbs, 1);
////	avs_video_sequence_header.bit_rate_upper = FIFO_Get(pbs, 12);
////	avs_video_sequence_header.low_delay = FIFO_Get(pbs, 1);
////	avs_video_sequence_header.marker_bit1 = FIFO_Get(pbs, 1);
////	avs_video_sequence_header.bbv_buffer_size = FIFO_Get(pbs, 18);
////	avs_video_sequence_header.reserved_bits = FIFO_Get(pbs, 3);
////
////	avs_video_sequence_header.bit_rate = ((avs_video_sequence_header.bit_rate_upper << 18) + avs_video_sequence_header.bit_rate_lower) * 400;
////	avs_video_sequence_header.BBS = 16 * 1024 * avs_video_sequence_header.bbv_buffer_size; 
////	avs_video_sequence_header.frame_rate = AVS_LUT_FrameRate[avs_video_sequence_header.frame_rate_code];
////
////	avs_video_next_start_code(pbs);
////
////#if AVS_VID_DECODER_DEBUG
////
////	if (m_hDebugFile == NULL)
////	{
////		fopen_s(&m_hDebugFile, "e:\\avs_debug\\avs_vid_decoder_debug.txt", "wt");
////		fopen_s(&m_hDebugYUV, "e:\\avs_debug\\avs_vid_decoder_debug.yuv", "wb");
////	}
////
////	if (m_hDebugFile)
////	{
////		fprintf(m_hDebugFile, "\nVIDEO_SEQUENCE_HEADER\n");
////		fprintf(m_hDebugFile, "profile_id = %d\n", avs_video_sequence_header.profile_id);
////		fprintf(m_hDebugFile, "level_id = %d\n", avs_video_sequence_header.level_id);
////		fprintf(m_hDebugFile, "progressive_sequence = %d\n", avs_video_sequence_header.progressive_sequence);
////		fprintf(m_hDebugFile, "horizontal_size = %d\n", avs_video_sequence_header.horizontal_size);
////		fprintf(m_hDebugFile, "vertical_size = %d\n", avs_video_sequence_header.vertical_size);
////		fprintf(m_hDebugFile, "chroma_format = %d\n", avs_video_sequence_header.chroma_format);
////		fprintf(m_hDebugFile, "sample_precision = %d\n", avs_video_sequence_header.sample_precision);
////		fprintf(m_hDebugFile, "aspect_ratio = %d\n", avs_video_sequence_header.aspect_ratio);
////		fprintf(m_hDebugFile, "frame_rate_code = %d\n", avs_video_sequence_header.frame_rate_code);
////		fprintf(m_hDebugFile, "bit_rate_lower = %d\n", avs_video_sequence_header.bit_rate_lower);
////		fprintf(m_hDebugFile, "low_delay = %d\n", avs_video_sequence_header.low_delay);
////		fprintf(m_hDebugFile, "bbv_buffer_size = %d\n", avs_video_sequence_header.bbv_buffer_size);
////
////		fprintf(m_hDebugFile, "\n");
////	}
////#endif
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//int avs_video_decode_buffer_clear(void)
//{
//	int		pixindex;
//	int		row, col;
//
//	if ((pucDecodeFrame[CC_Y] != NULL) &&
//		(pucDecodeFrame[CC_Cb] != NULL) &&
//		(pucDecodeFrame[CC_Cr] != NULL))
//	{
//
//		pixindex = 0;
//		for (row = 0; row < avs_decode_info.luma_height; row++)
//		{
//			for (col = 0; col < avs_decode_info.luma_width; col++)
//			{
//				pucDecodeFrame[CC_Y][pixindex] = 0;
//
//				pixindex++;
//			}
//		}
//
//		pixindex = 0;
//		for (row = 0; row < avs_decode_info.chroma_height; row++)
//		{
//			for (col = 0; col < avs_decode_info.chroma_width; col++)
//			{
//				pucDecodeFrame[CC_Cb][pixindex] = 128;
//				pucDecodeFrame[CC_Cr][pixindex] = 128;
//
//				pixindex++;
//			}
//		}
//	}
//
//	return 0;
//}
//
//AVS_i_picture_header_t* avs_video_get_i_picture_header(void)
//{
//	return &avs_i_picture_header;
//}
//
////int	avs_video_decode_i_picture_header(FIFO_t* pbs)
////{
////#if !AVS_VID_DECODER_I
////	S16*	psTemp;
////#endif
////
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	avs_i_picture_header.i_picture_start_code = FIFO_Get(pbs, 32);
////	avs_i_picture_header.bbv_delay = FIFO_Get(pbs, 16);
////	avs_i_picture_header.time_code_flag = FIFO_Get(pbs, 1);
////
////	if (avs_i_picture_header.time_code_flag == 1)
////	{
//////		avs_i_picture_header.time_code = FIFO_Get(pbs, 24);
////		avs_i_picture_header.drop_time_flag = FIFO_Get(pbs, 1);
////		avs_i_picture_header.time_code_hours = FIFO_Get(pbs, 5);
////		avs_i_picture_header.time_code_minutes = FIFO_Get(pbs, 6);
////		avs_i_picture_header.time_code_seconds = FIFO_Get(pbs, 6);
////		avs_i_picture_header.time_code_pictures = FIFO_Get(pbs, 6);
////	}
////
////	avs_i_picture_header.picture_distance = FIFO_Get(pbs, 8);
////
////	if (avs_video_sequence_header.low_delay == 1)
////	{
////		avs_i_picture_header.bbv_check_times = GetBits_ue(pbs);
////	}
////
////	//当progressive_sequence == 0时，
////	//如果progressive_frame == 1,则解码器以帧周期的整数倍为时间间隔输出重建帧
////	//如果progressive_frame == 0,则解码器将重建帧拆成两场，以场周期为时间间隔输出
////	avs_i_picture_header.progressive_frame = FIFO_Get(pbs, 1);
////	if (avs_i_picture_header.progressive_frame == 0)
////	{
////		avs_i_picture_header.picture_structure = FIFO_Get(pbs, 1);
////	}
////	else
////	{
////		avs_i_picture_header.picture_structure = 1;
////	}
////
////	avs_i_picture_header.top_field_first = FIFO_Get(pbs, 1);
////	avs_i_picture_header.repeat_first_field = FIFO_Get(pbs, 1);
////	avs_i_picture_header.fixed_picture_qp = FIFO_Get(pbs, 1);
////	avs_i_picture_header.picture_qp = FIFO_Get(pbs, 6);
////	avs_i_picture_header.skip_mode_flag = 0;
////	
////	if (avs_i_picture_header.progressive_frame == 0)
////	{
////		if (avs_i_picture_header.picture_structure == 0)
////		{
////			avs_i_picture_header.skip_mode_flag = FIFO_Get(pbs, 1);
////		}
////	}
////
////	avs_i_picture_header.reserved_bits = FIFO_Get(pbs, 4);
////	avs_i_picture_header.loop_filter_disable = FIFO_Get(pbs, 1);
////
////	avs_i_picture_header.alpha_c_offset = 0;
////	avs_i_picture_header.beta_offset = 0;
////	
////	if (!avs_i_picture_header.loop_filter_disable)
////	{
////		avs_i_picture_header.loop_filter_parameter_flag = FIFO_Get(pbs, 1);
////		if (avs_i_picture_header.loop_filter_parameter_flag)
////		{
////			avs_i_picture_header.alpha_c_offset = GetBits_se(pbs);
////			avs_i_picture_header.beta_offset = GetBits_se(pbs);
////		}
////	}
////
////	avs_video_next_start_code(pbs);
////
////	//semantic parse
////	PictureType = AVS_I_FRAME;
////
////	if (avs_video_sequence_header.progressive_sequence)
////	{
////		PictureStructure = 1;
////	}
////	else
////	{
////		if (avs_i_picture_header.progressive_frame)
////		{
////			PictureStructure = 1;
////		}
////		else
////		{
////			PictureStructure = avs_i_picture_header.picture_structure;
////		}
////	}
////
////	PreviousQP = avs_i_picture_header.picture_qp;
////	FixedQP = avs_i_picture_header.fixed_picture_qp;
////	MbIndex = 0;
////	SkipModeFlag = avs_i_picture_header.skip_mode_flag;
////
////	alternate_scan = (PictureStructure == 1) ? 0 : 1;
////
////	//如果当前图像是I帧，并且PictureStructure == 0
////	NumberOfReference = 1;
////
////	if (gbInitDecoder)
////	{
////
////#if AVS_VID_DECODER_I
////		//setup the decode frame buffer
////		pucDecodeFrame[CC_Y] = pucForwardFrame[CC_Y];
////		pucDecodeFrame[CC_Cb] = pucForwardFrame[CC_Cb];
////		pucDecodeFrame[CC_Cr] = pucForwardFrame[CC_Cr];
////#else
////		//interchange the buffer pointer
////		psTemp = pucForwardFrame[CC_Y];
////		pucForwardFrame[CC_Y] = pucBackwardFrame[CC_Y];
////		pucBackwardFrame[CC_Y] = psTemp;
////
////		psTemp = pucForwardFrame[CC_Cb];
////		pucForwardFrame[CC_Cb] = pucBackwardFrame[CC_Cb];
////		pucBackwardFrame[CC_Cb] = psTemp;
////
////		psTemp = pucForwardFrame[CC_Cr];
////		pucForwardFrame[CC_Cr] = pucBackwardFrame[CC_Cr];
////		pucBackwardFrame[CC_Cr] = psTemp;
////
////		//setup the decode frame buffer
////		pucDecodeFrame[CC_Y] = pucBackwardFrame[CC_Y];
////		pucDecodeFrame[CC_Cb] = pucBackwardFrame[CC_Cb];
////		pucDecodeFrame[CC_Cr] = pucBackwardFrame[CC_Cr];
////
////#endif
////		avs_video_decode_buffer_clear();
////
////#if AVS_VID_DECODER_DEBUG
////		if (m_hDebugFile)
////		{
////			fprintf(m_hDebugFile, "\nI_PICTURE_HEADER\n");
////			fprintf(m_hDebugFile, "\n");
////			fprintf(m_hDebugFile, "bbv_delay = %d\n", avs_i_picture_header.bbv_delay);
////			fprintf(m_hDebugFile, "time_code_flag = %d\n", avs_i_picture_header.time_code_flag);
////			fprintf(m_hDebugFile, "picture_distance = %d\n", avs_i_picture_header.picture_distance);
////			fprintf(m_hDebugFile, "progressive_frame = %d\n", avs_i_picture_header.progressive_frame);
////			fprintf(m_hDebugFile, "top_field_first = %d\n", avs_i_picture_header.top_field_first);
////			fprintf(m_hDebugFile, "repeat_first_field = %d\n", avs_i_picture_header.repeat_first_field);
////			fprintf(m_hDebugFile, "fixed_picture_qp = %d\n", avs_i_picture_header.fixed_picture_qp);
////			fprintf(m_hDebugFile, "picture_qp = %d\n", avs_i_picture_header.picture_qp);
////			fprintf(m_hDebugFile, "loop_filter_disable = %d\n", avs_i_picture_header.loop_filter_disable);
////			fprintf(m_hDebugFile, "loop_filter_parameter_flag = %d\n", avs_i_picture_header.loop_filter_parameter_flag);
////		}
////#endif
////	}
////
////	/*syntax part parse*/
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
////int	avs_video_decode_video_sequence_end(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	FIFO_Flush(pbs, 32);
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//AVS_user_data_t* avs_video_get_user_data(void)
//{
//	return &avs_user_data;
//}
//
////int	avs_video_decode_user_data(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	avs_user_data.user_data_start_code = FIFO_Get(pbs, 32);
////
////	avs_user_data.user_data_length = 0;
////
////	while (FIFO_Peek(pbs, 24) != 0x000001)
////	{
////		avs_user_data.user_data[avs_user_data.user_data_length] = FIFO_Get(pbs, 8);
////		avs_user_data.user_data_length ++;
////	}
////
////	avs_video_next_start_code(pbs);
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//AVS_sequence_display_extension_t* avs_video_get_sequence_display_extension(void)
//{
//	return &avs_sequence_display_extension;
//}
//
////此部分只跟显示有关,跟解码过程没有关系
////int	avs_video_decode_sequence_display_extension(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	avs_sequence_display_extension.extension_start_code = FIFO_Get(pbs, 32);
////	avs_sequence_display_extension.extension_id = FIFO_Get(pbs, 4);
////	avs_sequence_display_extension.video_format = FIFO_Get(pbs, 3);
////	avs_sequence_display_extension.sample_range = FIFO_Get(pbs, 1);
////	avs_sequence_display_extension.colour_description = FIFO_Get(pbs, 1);
////
////	if (avs_sequence_display_extension.colour_description)
////	{
////		avs_sequence_display_extension.colour_primaries = FIFO_Get(pbs, 8);
////		avs_sequence_display_extension.transfer_characteristics = FIFO_Get(pbs, 8);
////		avs_sequence_display_extension.matrix_coefficients = FIFO_Get(pbs, 8);
////	}
////
////	avs_sequence_display_extension.display_horizontal_size = FIFO_Get(pbs, 14);
////	avs_sequence_display_extension.marker_bit = FIFO_Get(pbs, 1);
////	avs_sequence_display_extension.display_vertical_size = FIFO_Get(pbs, 14);
////	avs_sequence_display_extension.reserved_bits = FIFO_Get(pbs, 2);
////
////	avs_video_next_start_code(pbs);
////
////#if AVS_VID_DECODER_DEBUG
////	if (m_hDebugFile)
////	{
////		fprintf(m_hDebugFile, "\nSEQUENCE_DISPLAY_EXTENSION\n");
////		fprintf(m_hDebugFile, "\n");
////		fprintf(m_hDebugFile, "extension_id = %d\n", avs_sequence_display_extension.extension_id);
////		fprintf(m_hDebugFile, "video_format = %d\n", avs_sequence_display_extension.video_format);
////		fprintf(m_hDebugFile, "sample_range = %d\n", avs_sequence_display_extension.sample_range);
////		fprintf(m_hDebugFile, "colour_description = %d\n", avs_sequence_display_extension.colour_description);
////		fprintf(m_hDebugFile, "display_horizontal_size = %d\n", avs_sequence_display_extension.display_horizontal_size);
////		fprintf(m_hDebugFile, "display_vertical_size = %d\n", avs_sequence_display_extension.display_vertical_size);
////		fprintf(m_hDebugFile, "\n");
////	}
////#endif
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//AVS_copyright_extension_t* avs_video_get_copyright_extension(void)
//{
//	return &avs_copyright_extension;
//}
//
////int	avs_video_decode_copyright_extension(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	FIFO_Flush(pbs, 32);
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//AVS_camera_parameters_extension_t* avs_video_get_camera_parameters_extension(void)
//{
//	return &avs_camera_parameters_extension;
//}
//
////int	avs_video_decode_camera_parameters_extension(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	FIFO_Flush(pbs, 32);
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//AVS_picture_display_extension_t* avs_video_get_picture_display_extension(void)
//{
//	return &avs_picture_display_extension;
//}
//
////int	avs_video_decode_picture_display_extension(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	FIFO_Flush(pbs, 32);
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//AVS_pb_picture_header_t* avs_video_get_pb_picture_header(void)
//{
//	return &avs_pb_picture_header;
//}
//
////int	avs_video_decode_pb_picture_header(FIFO_t* pbs)
////{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	FIFO_Flush(pbs, 32);
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//int avs_video_reorder_display_frame(void)
//{
//	switch (PictureType)
//	{
//	case AVS_I_FRAME:
//	case AVS_P_FRAME:
//		pucDisplayFrame[CC_Y] = pucForwardFrame[CC_Y];
//		pucDisplayFrame[CC_Cb] = pucForwardFrame[CC_Cb];
//		pucDisplayFrame[CC_Cr] = pucForwardFrame[CC_Cr];
//
//		break;
//
//	case AVS_B_FRAME:
//
//		pucDisplayFrame[CC_Y] = pucAuxFrame[CC_Y];
//		pucDisplayFrame[CC_Cb] = pucAuxFrame[CC_Cb];
//		pucDisplayFrame[CC_Cr] = pucAuxFrame[CC_Cr];
//		
//		break;
//
//	default:
//
//		pucDisplayFrame[CC_Y] = NULL;
//		pucDisplayFrame[CC_Cb] = NULL;
//		pucDisplayFrame[CC_Cr] = NULL;
//
//		break;
//	}
//
//	return 0;
//}
//
//int avs_video_vlc_lookup(int table, int trans_coefficient, AVS_VLC_Table_t* pvlc_tab)
//{
//	if (pvlc_tab)
//	{
//		switch (table)
//		{
//		case AVS_VLC0_Intra:
//			
//			break;
//		default:
//			break;
//		}
//	}
//
//	return 0;
//}
//
////int avs_video_block(FIFO_t* pbs, int i)
////{
////	int rtcode = 0;
////	int trans_coefficient;
////	int	escape_level_diff;
////	int CurrentVLCTable;
////	int maxAbsLevel = 0;
////	int absLevel;
////	int	RefAbsLevel;
////	int k;
////	AVS_VLC_Table_t	vlc_tab[64];
////	int	j;
////	int coeffNum = -1;
////	int	count = 0;
////	int	run;
////
////	if (PatternCode[i])
////	{
////		if (i < 4)
////		{
////			CurrentVLCTable = (MbType == AVS_I_8x8) ? AVS_VLC0_Intra : AVS_VLC0_Inter;
////		}
////		else
////		{
////			CurrentVLCTable = AVS_VLC0_Chroma;
////		}
////	
////		for (j = 0; j < 65; j++)
////		{
////			k = AVS_LUT_VLC_k[CurrentVLCTable];
////			trans_coefficient = GetBits_ue(pbs, k);
////
////#if AVS_VID_DECODER_DEBUG
////			if (m_hDebugFile)
////			{
////				fprintf(m_hDebugFile, "trans_coefficient = %d\n", trans_coefficient);
////			}
////#endif
////
////			if (trans_coefficient == AVS_LUT_VLC_EOB[CurrentVLCTable])
////			{
////				count = j;
////				break;
////			}
////			else if (trans_coefficient >= 59)
////			{
////				if (i < 4)
////				{
////					escape_level_diff = (MbType == AVS_I_8x8) ? GetBits_ue(pbs, 1) : GetBits_ue(pbs, 0);
////				}
////				else
////				{
////					escape_level_diff = GetBits_ue(pbs, 0);
////				}
////
////#if AVS_VID_DECODER_DEBUG
////				if (m_hDebugFile)
////				{
////					fprintf(m_hDebugFile, "escape_level_diff = %d\n", escape_level_diff);
////				}
////#endif
////				run = (trans_coefficient - 59) >> 1;
////				if (run < 64)
////				{
////					if (run > AVS_LUT_VLC_MaxRun[CurrentVLCTable])
////					{
////						RefAbsLevel = 1;
////					}
////					else
////					{
////						RefAbsLevel = AVS_LUT_VLC_RefAbsLevel[CurrentVLCTable][run];
////					}
////
////					vlc_tab[j].run = run;
////					if (trans_coefficient & 1)
////					{
////						vlc_tab[j].level = -(RefAbsLevel + escape_level_diff);
////					}
////					else
////					{
////						vlc_tab[j].level = (RefAbsLevel + escape_level_diff);
////					}
////				}
////				else
////				{
////					rtcode = -1;
////					break;
////				}
////			}
////			else if (trans_coefficient >= 0)
////			{
////				vlc_tab[j] = AVS_LUT_VLC_Table[CurrentVLCTable][trans_coefficient];
////			}
////			else		//wrong condition
////			{
////				rtcode = -1;
////				break;
////			}
////
////			absLevel = abs(vlc_tab[j].level);
////
////			if (absLevel > maxAbsLevel)
////			{
////				if (i < 4)		//luminance
////				{
////					if (MbType == AVS_I_8x8)		//intra
////					{
////						switch (absLevel)
////						{
////						case 1:
////							CurrentVLCTable = AVS_VLC1_Intra;
////							break;
////						case 2:
////							CurrentVLCTable = AVS_VLC2_Intra;
////							break;
////						case 3:
////						case 4:
////							CurrentVLCTable = AVS_VLC3_Intra;
////							break;
////						case 5:
////						case 6:
////						case 7:
////							CurrentVLCTable = AVS_VLC4_Intra;
////							break;
////						case 8:
////						case 9:
////						case 10:
////							CurrentVLCTable = AVS_VLC5_Intra;
////							break;
////						default:
////							if (absLevel > 10)
////							{
////								CurrentVLCTable = AVS_VLC6_Intra;
////							}
////
////							break;
////						}
////					}
////					else							//inter
////					{
////						switch (absLevel)
////						{
////						case 1:
////							CurrentVLCTable = AVS_VLC1_Inter;
////							break;
////						case 2:
////							CurrentVLCTable = AVS_VLC2_Inter;
////							break;
////						case 3:
////							CurrentVLCTable = AVS_VLC3_Inter;
////							break;
////						case 4:
////						case 5:
////						case 6:
////							CurrentVLCTable = AVS_VLC4_Inter;
////							break;
////						case 7:
////						case 8:
////						case 9:
////							CurrentVLCTable = AVS_VLC5_Inter;
////							break;
////						default:
////							if (absLevel > 9)
////							{
////								CurrentVLCTable = AVS_VLC6_Inter;
////							}
////
////							break;
////						}
////					}
////				}
////				else			//chrominance
////				{
////					switch (absLevel)
////					{
////					case 1:
////						CurrentVLCTable = AVS_VLC1_Chroma;
////						break;
////					case 2:
////						CurrentVLCTable = AVS_VLC2_Chroma;
////						break;
////					case 3:
////					case 4:
////						CurrentVLCTable = AVS_VLC3_Chroma;
////						break;
////					default:
////						if (absLevel > 4)
////						{
////							CurrentVLCTable = AVS_VLC4_Chroma;
////						}
////						break;
////					}
////				}
////
////				maxAbsLevel = absLevel;
////			}
////		}
////
////		memset(QuantCoeffArray, 0x00, sizeof(QuantCoeffArray));
////		for (j = count - 1; j >= 0; j--)
////		{
////			coeffNum += vlc_tab[j].run + 1;
////
////			if (coeffNum < 64)
////			{
////				QuantCoeffArray[coeffNum] = vlc_tab[j].level;
////			}
////			else		//error condition
////			{
////				rtcode = -1;
////				break;
////			}
////		}
////	}
////
////	return rtcode;
////}
//
//AVS_MB_INFO_t* avs_video_get_macroblock(int macroblockindex)
//{
//	return NULL;
//}
//
//int avs_video_lut_p_macroblock_type(int MbTypeIndex, int* pMbType, int* pMvNum, int* pMbPredMode)
//{
//	switch (MbTypeIndex)
//	{
//	case 0:
//		*pMbType = AVS_P_Skip;
//		*pMvNum = 0;
//		break;
//	case 1:
//		*pMbType = AVS_P_16x16;
//		*pMvNum = 1;
//		break;
//	case 2:
//		*pMbType = AVS_P_16x8;
//		*pMvNum = 2;
//		break;
//	case 3:
//		*pMbType = AVS_P_8x16;
//		*pMvNum = 2;
//		break;
//	case 4:
//		*pMbType = AVS_P_8x8;
//		*pMvNum = 4;
//		break;
//	case 5:
//		*pMbType = AVS_I_8x8;
//		*pMvNum = 0;
//		break;
//	}
//
//	return 0;
//}
//
//int avs_video_lut_b_macroblock_type(int MbTypeIndex, int* pMbType, int* pMvNum, int* pMbPredMode)
//{
//	switch (MbTypeIndex)
//	{
//	case 0:
//		*pMbType = AVS_B_Skip;
//		*pMvNum = 0;
//		break;
//	case 1:
//		*pMbType = AVS_B_Direct_16x16;
//		*pMvNum = 0;
//		break;
//	case 2:
//		*pMbType = AVS_B_Fwd_16x16;
//		*pMvNum = 1;
//		break;
//	case 3:
//		*pMbType = AVS_B_Bck_16x16;
//		*pMvNum = 1;
//		break;
//	case 4:
//		*pMbType = AVS_B_Sym_16x16;
//		*pMvNum = 1;
//		break;
//	case 24:
//		*pMbType = AVS_I_8x8;
//		*pMvNum = 0;
//		break;
//	}
//
//	return 0;
//}
//
//int	avs_video_reconstruct_block(int mb_row, int mb_col, int i)
//{
//	int		 rtcode = 0;
//	int		 v, u;
//	int		 v_2;
//	int		 w = 0;
//	int		 pix_index = 0;
//	int		 pix_offset;
//	S16		 pix_value;	
//	int		 sum = 0;
//	int		 bx, by;
//	int		 width;
//	int		 pix_count;
////	int		 k;
//	S16		 QF[8][8];
//	int		 cc;
//	int		 QP;
//
//	cc = AVS_LUT_cc[i];
//
////	if (i >= 4) return 0;
//
//	if (PatternCode[i])
//	{
//		//反扫描
//		for (v = 0; v < 8; v++)				//行序号
//		{
//			for (u = 0; u < 8; u++)			//列序号
//			{
//				QF[v][u] = QuantCoeffArray[scan[alternate_scan][v][u]];
//			}
//		}
///*
//#if AVS_VID_DECODER_DEBUG
//		if (m_hDebugFile)
//		{
//			fprintf(m_hDebugFile, "Inverse Scan: [mb_row = %d, mb_col = %d, block = %d]\n", mb_row, mb_col, i);
//			for (v = 0; v < 8; v++)				//行序号
//			{
//				for (u = 0; u < 8; u++)			//列序号
//				{
//					fprintf(m_hDebugFile, " %3d", QF[v][u]);
//				}
//				fprintf(m_hDebugFile, "\n");
//			}
//		}
//#endif
//*/
//		QP = CurrentQP;
//		if (i >= 4)
//		{
//			if ( QP < 43)
//			{
//				QP = CurrentQP;
//			}
//			else
//			{
//				QP = AVS_LUT_Chroma_QP[CurrentQP - 43];
//			}
//		}
//
//		//反量化
//		for (v = 0; v < 8; v++)				//行序号
//		{
//			for (u = 0; u < 8; u++)			//列序号
//			{
//				QF[v][u] = (QF[v][u] * AVS_LUT_DequantTable[QP] + (1 << (AVS_LUT_ShiftTable[QP] - 1))) >> AVS_LUT_ShiftTable[QP];
////				QF[v][u] = 255;
//			}
//		}
//
//#if AVS_VID_DECODER_DEBUG
//		if (m_hDebugFile)
//		{
//			fprintf(m_hDebugFile, "Inverse Quantisation:\n");
//			for (v = 0; v < 8; v++)				//行序号
//			{
//				for (u = 0; u < 8; u++)			//列序号
//				{
//					fprintf(m_hDebugFile, " %3d", QF[v][u]);
//				}
//				fprintf(m_hDebugFile, "\n");
//			}
//		}
//#endif
//
//		//IDCT
//	//	MMX_IDCT((S16*)QF);
//	//	Fast_IDCT((S16*)QuantCoeffMatrix);
//	//	Reference_IDCT((S16*)QF);
///*
//#if MPEG_VID_DECODER_DEBUG
//		if (m_hDebugFile)
//		{
//			fprintf(m_hDebugFile, "Inverse DCT: [mb_row = %d, mb_col = %d, block = %d]\n", mb_row, mb_column, block);
//			for (v = 0; v < 8; v++)				//行序号
//			{
//				for (u = 0; u < 8; u++)			//列序号
//				{
//					fprintf(m_hDebugFile, " %3d", QF[v][u]);
//				}
//				fprintf(m_hDebugFile, "\n");
//			}
//		}
//#endif
//*/
//		//存储到显存
//		if (cc == CC_Y)
//		{
//			by = ((mb_row << 1) + (i >> 1)) << 3;
//			bx = ((mb_col << 1) + (i & 1)) << 3;
//
//			width = avs_decode_info.luma_width;
//			pix_count = avs_decode_info.luma_pix_count;
//		}
//		else
//		{
//			switch (avs_video_sequence_header.chroma_format)
//			{
//			case CHROMA_FORMAT_4_2_0:
//				
//				by = mb_row << 3;
//				bx = mb_col << 3;
//				break;
//
//			case CHROMA_FORMAT_4_2_2:
//			
//				by = ((mb_row << 1) + ((i - 4) >> 1)) << 3;
//				bx = mb_col << 3;
//				break;
//
//			case CHROMA_FORMAT_4_4_4:
//
//				if (i < 8)
//				{
//					by = ((mb_row << 1) + ((i - 4) >> 1)) << 3;
//				}
//				else
//				{
//					by = ((mb_row << 1) + ((i - 4) >> 1)) << 3;
//				}
//
//				bx = ((mb_col << 1) + ((i - 4) >> 2)) << 3;
//				
//				break;
//
//			default:
//				break;
//			}
//
//			width = avs_decode_info.chroma_width;
//			pix_count = avs_decode_info.chroma_pix_count;
//	
//		}
//
//	//	if (mpeg_macroblock_modes.dct_type == 1)
//		if (0)
//		{
//			if (cc == CC_Y)
//			{
//	//			for (v = (block / 2); v < 16; v += 2)
//				for (v = (i >> 1); v < 16; v += 2)
//				{
//					v_2 = (v >> 1);
//
//					pix_index = ((mb_row << 4) + v) * width + bx;
//
//					for (u = 0; u < 8; u++)
//					{
//						if ((pix_index >= 0) && (pix_index < pix_count))
//						{
//							//f[y][x]
//	//						pix_value = (QF[v/2][u] > 255) ? 255 : ((QF[v/2][u] < 0) ? 0 : QF[v/2][u]);
//							pix_value = (QF[v_2][u] > 255) ? 255 : ((QF[v_2][u] < 0) ? 0 : QF[v_2][u]);
//
//							if (pucDecodeFrame[cc])
//							{
//								pucDecodeFrame[cc][pix_index] = (uint8_t)pix_value;
//							}
//
//							pix_index ++;
//						}
//						else
//						{
//							rtcode = -1;
//							break;
//						}
//					}
//				}
//			}
//			else
//			{
//				pix_offset = by * width + bx;
//				for (v = 0; v < 8; v ++)
//				{
//					pix_index = pix_offset;
//					for (u = 0; u < 8; u++)
//					{
//						if (pix_index < pix_count)
//						{
//							//f[y][x]
//							pix_value = (QF[v][u] > 255) ? 255 : ((QF[v][u] < 0) ? 0 : QF[v][u]);
//							
//							if (pucDecodeFrame[cc])
//							{
//								pucDecodeFrame[cc][pix_index] = (uint8_t)pix_value;
//							}
//
//							pix_index ++;
//						}
//						else
//						{
//							rtcode = -1;
//							break;
//						}
//					}
//
//					pix_offset += width;
//				}
//			}
//		}
//		else
//		{
//			pix_offset = by * width + bx;
//
//			for (v = 0; v < 8; v ++)
//			{
//				pix_index = pix_offset;
//				for (u = 0; u < 8; u++)
//				{
//					if ((pix_index >= 0) && (pix_index < pix_count))
//					{
//						if (MbType == AVS_I_8x8)//f[y][x]
//						{
//							pix_value = QF[v][u];
//						}
//						else				//f[y][x] + p[y][x]
//						{
//	//						pix_value = QF[v][u] + pel_pred[v][u];
//						}
//
//						pix_value = (pix_value > 255) ? 255 : (pix_value < 0) ? 0 : pix_value;
//
//						if (pucDecodeFrame[cc])
//						{
//							pucDecodeFrame[cc][pix_index] = (uint8_t)pix_value;
//						}
//
//						pix_index ++;
//					}
//					else
//					{
//						rtcode = -1;
//						break;
//					}
//				}
//
//				pix_offset += width;
//			}
//		}
//	}
//
//	return rtcode;
//}
//
//int avs_video_skipped_macroblock(int mb_index)
//{
//	switch (PictureType)
//	{
//	case AVS_I_FRAME:
//		break;
//	case AVS_P_FRAME:
//		avs_macroblock.mb_type = AVS_P_Skip;
//		break;
//	case AVS_B_FRAME:
//		avs_macroblock.mb_type = AVS_B_Skip;
//		break;
//	default:
//		break;
//	}
//
//	return 0;
//}
//
////int avs_video_macroblock(FIFO_t* pbs)
////{
////	int		rtcode = -1;
////	int		i;
////	int		MbTypeIndex;
////	int		CBPCodeNum;
////	int		MbCBP;
////	int		MbCBP422;
////	int		MbRow;
////	int		MbCol;
////
////	if ((PictureType != AVS_I_FRAME) || ((PictureStructure == 0) && (MbIndex >= HalfMbCount)))
////	{
////		avs_macroblock.mb_type = GetBits_ue(pbs);
////	}
////
////	//设置宏块类型
////	switch (PictureType)
////	{
////	case AVS_I_FRAME:
////
////		switch (PictureStructure)
////		{
////		case 0:
////			
////			if (MbIndex < HalfMbCount)		//第一场
////			{
////				MbType = AVS_I_8x8;
////				MvNum = 0;
////			}
////			else
////			{
////				if (SkipModeFlag == 1)
////				{
////					MbTypeIndex = avs_macroblock.mb_type + 1;
////				}
////				else
////				{
////					MbTypeIndex = avs_macroblock.mb_type;
////				}
////
////				if (MbTypeIndex >= 5)
////				{
////					CBPCodeNum = MbTypeIndex - 5;
////					MbTypeIndex = 5;
////				}
////
////				avs_video_lut_p_macroblock_type(MbTypeIndex, &MbType, &MvNum, &MbPredMode);
////				//
////			}
////			break;
////
////		case 1:
////
////			MbType = AVS_I_8x8;
////			MvNum = 0;
////
////			break;
////		}
////
////		break;
////
////	case AVS_P_FRAME:
////		
////		if (SkipModeFlag == 1)
////		{
////			MbTypeIndex = avs_macroblock.mb_type + 1;
////		}
////		else
////		{
////			MbTypeIndex = avs_macroblock.mb_type;
////		}
////
////		if (MbTypeIndex >= 5)
////		{
////			CBPCodeNum = MbTypeIndex - 5;
////			MbTypeIndex = 5;
////		}
////
////		avs_video_lut_p_macroblock_type(MbTypeIndex, &MbType, &MvNum, &MbPredMode);
////
////		break;
////
////	case AVS_B_FRAME:
////		
////		if (SkipModeFlag == 1)
////		{
////			MbTypeIndex = avs_macroblock.mb_type + 1;
////		}
////		else
////		{
////			MbTypeIndex = avs_macroblock.mb_type;
////		}
////
////		if (MbTypeIndex >= 24)
////		{
////			CBPCodeNum = MbTypeIndex - 24;
////			MbTypeIndex = 24;
////		}
////
////		avs_video_lut_b_macroblock_type(MbTypeIndex, &MbType, &MvNum, &MbPredMode);
////
////		break;
////
////	default:
////		break;
////	}
////
////	if ((MbType != AVS_P_Skip) && (MbType != AVS_B_Skip))
////	{
////		if (MbType == AVS_B_8x8)
////		{
////			for (i = 0; i < 4; i++)
////			{
////				avs_macroblock.mb_part_type[i] = FIFO_Get(pbs, 2);
////			}
////		}
////
////		if (MbType == AVS_I_8x8)
////		{
////			for (i = 0; i < 4; i++)
////			{
////				avs_macroblock.pred_mode_flag[i] = FIFO_Get(pbs, 1);
////
////				if (avs_macroblock.pred_mode_flag[i] == 0)
////				{
////					avs_macroblock.intra_luma_pred_mode[i] = FIFO_Get(pbs, 2);
////				}
////
////#if AVS_VID_DECODER_DEBUG
////				if (m_hDebugFile)
////				{
////					fprintf(m_hDebugFile, "pred_mode_flag[%d] = %d\n", i, avs_macroblock.pred_mode_flag[i]);
////				}
////#endif
////			}
////
////			avs_macroblock.intra_chroma_pred_mode = GetBits_ue(pbs);
////
////			if (avs_video_sequence_header.chroma_format == CHROMA_FORMAT_4_2_2)
////			{
////				avs_macroblock.intra_chroma_pred_mode_422 = GetBits_ue(pbs);
////			}
////
////#if AVS_VID_DECODER_DEBUG
////			if (m_hDebugFile)
////			{
////				fprintf(m_hDebugFile, "intra_chroma_pred_mode = %d\n", avs_macroblock.intra_chroma_pred_mode);
////			}
////#endif
////		}
////
////		if (((PictureType == AVS_P_FRAME) || ((PictureType == AVS_B_FRAME) && (PictureStructure == 0))) && (PictureReferenceFlag == 0))
////		{ 
////			for (i = 0; i < MvNum; i++)
////			{
////				avs_macroblock.mb_reference_index[i] = FIFO_Get(pbs, 1);
////			}
////		}
////
////		for (i = 0; i < MvNum; i++)
////		{
////			avs_macroblock.mv_diff_x[i] = GetBits_se(pbs);
////			avs_macroblock.mv_diff_y[i] = GetBits_se(pbs);
////		}
////
////		if (MbWeightingFlag == 1)
////		{
////			avs_macroblock.weighting_prediction = FIFO_Get(pbs, 1);
////		}
////
////		if (!(((MbTypeIndex >= 24) && (PictureType == AVS_B_FRAME)) || ((MbTypeIndex >= 5) && (PictureType != AVS_B_FRAME))))
////		{
////			CBPCodeNum = GetBits_ue(pbs);
////		}
////
////		avs_macroblock.cbp = (MbType == AVS_I_8x8) ? AVS_LUT_MbCBP[0][CBPCodeNum] : AVS_LUT_MbCBP[1][CBPCodeNum];
////		MbCBP = avs_macroblock.cbp;
////
////		for (i = 0; i < 6; i++)
////		{
////			PatternCode[i] = (MbCBP & (1 << i)) ? 1 : 0;
////		}
////
////		if (avs_video_sequence_header.chroma_format == CHROMA_FORMAT_4_2_2)
////		{
////			avs_macroblock.cbp_422 = (MbType == AVS_I_8x8) ? AVS_LUT_MbCBP422[0][CBPCodeNum] : AVS_LUT_MbCBP422[1][CBPCodeNum];
////			MbCBP422 = avs_macroblock.cbp_422;
////			for (i = 6; i < 8; i++)
////			{
////				PatternCode[i] = (MbCBP422 & (1 << (i - 6))) ? 1 : 0;
////			}
////		}
////
////#if AVS_VID_DECODER_DEBUG
////		if (m_hDebugFile)
////		{
////			fprintf(m_hDebugFile, "MbCBP = %d(0x%02x)\n", MbCBP, MbCBP);
////		}
////#endif
////
////		CurrentQP = PreviousQP;
////
////		if (((MbCBP > 0) || ((MbCBP422 > 0) && (avs_video_sequence_header.chroma_format == CHROMA_FORMAT_4_2_2))) && !FixedQP)
////		{
////			avs_macroblock.mb_qp_delta = GetBits_se(pbs);
////			CurrentQP += avs_macroblock.mb_qp_delta;
////		}
////
////		MbRow = MbIndex / MbWidth;
////		MbCol = MbIndex % MbWidth;
////
////		for (i = 0; i < 6; i++)
////		{
////			rtcode = avs_video_block(pbs, i);
////			if (rtcode < 0)
////			{
////				break;
////			}
////
////			rtcode = avs_video_reconstruct_block(MbRow, MbCol, i);
////			if (rtcode < 0)
////			{
////				break;
////			}
////		}
////
////		if ((rtcode >= 0) && (avs_video_sequence_header.chroma_format == CHROMA_FORMAT_4_2_2))
////		{
////			for (i = 6; i < 8; i++)
////			{
////				rtcode = avs_video_block(pbs, i);
////
////				if (rtcode < 0)
////				{
////					break;
////				}
////
////				rtcode = avs_video_reconstruct_block(MbRow, MbCol, i);
////				if (rtcode < 0)
////				{
////					break;
////				}
////			}
////		}
////	}
////
////	return rtcode;
////}
//
////int avs_video_decode_slice(FIFO_t* pbs)
////{
////	int		 rtcode = -1;
////	int		 i;
////
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
////
////	avs_slice.slice_start_code = FIFO_Get(pbs, 32);
////	avs_slice.slice_vertical_position = (avs_slice.slice_start_code & 0x000000ff);
////
//////#if AVS_VID_DECODER_DEBUG
//////	if (m_hDebugFile)
//////	{
//////		fprintf(m_hDebugFile, "\nSLICE\n");
//////		fprintf(m_hDebugFile, "\n");
//////		fprintf(m_hDebugFile, "slice_vertical_position = %d\n", avs_slice.slice_vertical_position);
//////		fprintf(m_hDebugFile, "\n");
//////	}
//////#endif
////
////	if (gbInitDecoder
////#if AVS_VID_DECODER_I
////		&& (PictureType == AVS_I_FRAME))
////#elif AVS_VID_DECODER_P
////		&& (PictureType == AVS_P_FRAME))
////#elif AVS_VID_DECODER_B 
////		&& (PictureType == AVS_B_FRAME))
////#elif AVS_VID_DECODER_IP 
////		&& ((PictureType == AVS_I_FRAME) || (PictureType == AVS_P_FRAME)))
////#else
////	)
////#endif
////	{
//////		MbIndex = avs_slice.slice_vertical_position * MbWidth;
////
////		if (avs_video_sequence_header.vertical_size > 2800)			//vertical_size is a member variable, derived from avs_video_sequence_header and sequence_extension
////		{
////			avs_slice.slice_vertical_position_extension = (FIFO_Get(pbs, 3) << 7) + avs_slice.slice_vertical_position;
//////			MbIndex = avs_slice.slice_vertical_position_extension * MbWidth;
////		}
////
////		if (avs_i_picture_header.fixed_picture_qp == 0)
////		{
////			FixedQP = FIFO_Get(pbs, 1);
////			PreviousQP = FIFO_Get(pbs, 6);
////		}
////
////		MbWeightingFlag = 0;	//如果mb_weighting_flag不存在，则等于0
////		//非I帧
////		//或I帧隔行场时
////		if ((PictureType != AVS_I_FRAME) || ((PictureStructure == 0) && (MbIndex >= HalfMbCount)))
////		{
////			avs_slice.slice_weighting_flag = FIFO_Get(pbs, 1);
////
////			if (avs_slice.slice_weighting_flag == 1)
////			{
////				for (i = 0; i < NumberOfReference; i++)
////				{
////					avs_slice.luma_scale[i] = FIFO_Get(pbs, 8);
////					avs_slice.luma_shift[i] = FIFO_Get(pbs, 8);
////					FIFO_Flush(pbs, 1);
////
////					avs_slice.chroma_scale[i] = FIFO_Get(pbs, 8);
////					avs_slice.chroma_shift[i] = FIFO_Get(pbs, 8);
////					FIFO_Flush(pbs, 1);
////				}
////
////				avs_slice.mb_weighting_flag = FIFO_Get(pbs, 1);
//////				MbWeightingFlag = mb_weighting_flag;	//如果mb_weighting_flag存在，则等于此
////			}
////		}
////
////		SkipMbCount = 0;
////
////		do
////		{
////			if ((PictureType != AVS_I_FRAME) || ((PictureStructure == 0) && (MbIndex >= HalfMbCount)))
////			{
////				if (SkipModeFlag == 1)
////				{
////					avs_slice.mb_skip_run = GetBits_ue(pbs);
////					SkipMbCount = avs_slice.mb_skip_run;
////				}
////			}
////
////			if (SkipMbCount > 0)
////			{
////				for (i = 0; i < SkipMbCount; i++)
////				{
////					avs_video_skipped_macroblock(MbIndex);
////
////					MbIndex ++;
////				}
////			}
////
////			if (MbIndex < MbCount)
////			{
////#if AVS_VID_DECODER_DEBUG
////				if (m_hDebugFile)
////				{
////					fprintf(m_hDebugFile, "\n");
////					fprintf(m_hDebugFile, "SLICE = %d, MbIndex = %d\n", avs_slice.slice_vertical_position, MbIndex);
////					fprintf(m_hDebugFile, "\n");
////				}
////#endif
////				rtcode = avs_video_macroblock(pbs);
////				MbIndex ++;
////
////				if ((MbIndex % MbWidth) == 0)		//AVS 标准设计的码流或编码器应该存在问题，本部分就是修补该BUG。chendelin 20061127
////				{
////					if (avs_video_check_padding_bits(pbs))
////					{
////						break;
////					}
////				}
////			}
////			else
////			{
////				break;
////			}
////
////		} while ((rtcode >= 0) && (FIFO_Peek(pbs, 24) != 0x000001));
////
////		avs_video_next_start_code(pbs);
////
////		if ((pbs->rdptr[3] < AVS_SLICE_START_CODE_MIN) || (pbs->rdptr[3] > AVS_SLICE_START_CODE_MAX))
////		{
////			avs_video_reorder_display_frame();
////
////#if AVS_VID_DECODER_DEBUG
////
////			if (m_hDebugFile)					//last row
////			{
////			}
////#endif
////		}
////	}
////	else
////	{
////		avs_video_next_start_code(pbs);
////	}
////
////	::SetEvent(ghAccessEvent);
////
////	return 0;
////}
//
//uint8_t* avs_video_output(int* pwidth, int* pheight)
//{
//	if (gbInitDecoder == 0) return NULL;
//
//#if AVS_VID_DECODER_I
//	if (PictureType == AVS_P_FRAME) return NULL;
//	if (PictureType == AVS_B_FRAME) return NULL;
//#elif AVS_VID_DECODER_P
//	if (PictureType == AVS_I_FRAME) return NULL;
//	if (PictureType == AVS_B_FRAME) return NULL;
//#elif AVS_VID_DECODER_B
//	if (PictureType == AVS_I_FRAME) return NULL;
//	if (PictureType == AVS_P_FRAME) return NULL;
//#elif AVS_VID_DECODER_IP
//	if (PictureType == AVS_B_FRAME) return NULL;
//#endif
//
//	if ((pucDisplayFrame[CC_Y] != NULL) &&
//		(pucDisplayFrame[CC_Cb] != NULL) &&
//		(pucDisplayFrame[CC_Cr] != NULL))
//	{
//		::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//		if (render_avs_video_buf_rgb888 != NULL)
//		{
////			Video_yuv2rgb(	pucDisplayFrame[CC_Y],
////							pucDisplayFrame[CC_Cb],
////							pucDisplayFrame[CC_Cr],
////							render_avs_video_buf_rgb888,
////							&avs_decode_info);
//
//#if MPEG_VID_DECODER_DEBUG
//			if (m_pucY && m_pucCb && m_pucCr)
//			{
//				if (m_hDebugYUV)
//				{
//					fwrite(m_pucY, luma_pix_count, sizeof(char), m_hDebugYUV);
//					fwrite(m_pucCb, chroma_pix_count, sizeof(char), m_hDebugYUV);
//					fwrite(m_pucCr, chroma_pix_count, sizeof(char), m_hDebugYUV);
//				}
//			}
//#endif
//		}
//
////		*pwidth = avs_decode_info.graph_width;
////		*pheight = avs_decode_info.graph_height;
//
//		::SetEvent(ghAccessEvent);
//	}
//
//	return render_avs_video_buf_rgb888;
//}
//
//void avs_video_setgrid(void)
//{
//	avs_decode_info.graph_with_grid = !avs_decode_info.graph_with_grid;
//}
//
//Video_decode_info_t* avs_video_get_decode_info(void)
//{
//	return &avs_decode_info;
//}
//
///************************************************************************/
///* 函数功能：初始化VLC表                                                */
///************************************************************************/
//void avs_video_make_vlc_table(void)
//{
//	int level, run, ipos;
//	int i;
//
//	for (i = 0; i < 7; i++)
//	{
//		for (run = 0; run < 26; run++)
//		{
//			for (level = 0; level < 27; level++)
//			{
//				ipos = AVS_2DVLC_INTRA[i][run][level];
//				if (ipos >= 0)
//				{
//					AVS_LUT_VLC_Table[i][ipos].level = level;
//					AVS_LUT_VLC_Table[i][ipos].run = run;
//					
//					AVS_LUT_VLC_Table[i][ipos + 1].level = -level;
//					AVS_LUT_VLC_Table[i][ipos + 1].run = run;
//				}
//			}
//		}
//	}
//	
//	for (i = 0; i < 7; i++)
//	{
//		for (run = 0; run < 26; run++)
//		{
//			for (level = 0; level < 27; level++)
//			{
//				ipos = AVS_2DVLC_INTER[i][run][level];
//
//				if (ipos >= 0)
//				{
//					AVS_LUT_VLC_Table[7 + i][ipos].level = level;
//					AVS_LUT_VLC_Table[7 + i][ipos].run = run;
//					
//					AVS_LUT_VLC_Table[7 + i][ipos + 1].level = -level;
//					AVS_LUT_VLC_Table[7 + i][ipos + 1].run = run;
//				}
//			}
//		}
//	}
//	
//	for (i = 0; i < 5; i++)
//	{
//		for (run = 0; run < 26; run++)
//		{
//			for (level = 0; level < 27; level++)
//			{
//				ipos = AVS_2DVLC_CHROMA[i][run][level];
//
//				if (ipos >= 0)
//				{
//					AVS_LUT_VLC_Table[14 + i][ipos].level = level;
//					AVS_LUT_VLC_Table[14 + i][ipos].run = run;
//					
//					AVS_LUT_VLC_Table[14 + i][ipos + 1].level = -level;
//					AVS_LUT_VLC_Table[14 + i][ipos + 1].run = run;
//				}
//			}
//		}
//	}
//}
//

