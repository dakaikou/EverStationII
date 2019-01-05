#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <Windows.h>
#include "../../Include/common/video_common.h"
#include "../../Include/kernel/kernel_h264_video_decoder.h"
#include "kernel_h264_video_table.h"
#include "XStream_BitStream/Include/bitstream.h"

static H264_seq_parameter_set_t				h264_seq_parameter_set[MAXSPS];
static H264_pic_parameter_set_t				h264_pic_parameter_set[MAXPPS];
static H264_slice_header_t					h264_slice_header;
static H264_slice_data_t					h264_slice_data;
static H264_macroblock_layer_t*				ph264_macroblock_layer = NULL;

static Video_decode_info_t					h264_decode_info;

static H264_seq_parameter_set_t*			pCurSPS = NULL;
static H264_pic_parameter_set_t*			pCurPPS = NULL;

static S16			ScalingList4x4[6][16];
static S16			ScalingList8x8[2][64];
static S16			UseDefaultScalingMatrix4x4Flag[6][16];
static S16			UseDefaultScalingMatrix8x8Flag[2][64];

static S16			Intra16x16DCLevel[16];				//一个宏块有4个8x8块，每个8x8块有4个4x4子块
static S16			Intra16x16ACLevel[16][15];
static S16			LumaLevel[16][16];
static S16			LumaLevel8x8[4][64];

static S16			ChromaDCLevel[2][16];				//FIX ME,目前只能支持4:2:0格式
static S16			ChromaACLevel[2][16][15];

static uint8_t*			pucForwardFrame[3] = {NULL, NULL, NULL};							//0 -- Y, 1 -- Cb, 2 -- Cr
static uint8_t*			pucBackwardFrame[3] = {NULL, NULL, NULL};
static uint8_t*			pucAuxFrame[3] = {NULL, NULL, NULL};

static uint8_t*			pucDisplayFrame[3] = {NULL, NULL, NULL};					//当前显示帧
static uint8_t*			pucDecodeFrame[3] = {NULL, NULL, NULL};						//当前解码帧

//static uint8_t*			render_h264_video_buf_rgb888;
//static int			render_h264_video_buf_size;

static int			gbInitDecoder = 0;
static HANDLE		ghAccessEvent = NULL;

#if H264_VID_DECODER_DEBUG

static FILE*	m_hDebugFile = NULL;
static FILE*	m_hDebugYUV = NULL;
static int		m_nDebugMBCount;

#endif

static int		m_nDebugFrameCount;

void h264_video_init(void)
{
	int		i;

	memset(&h264_seq_parameter_set, 0x00, MAXSPS * sizeof(H264_seq_parameter_set_t));
	memset(&h264_pic_parameter_set, 0x00, MAXPPS * sizeof(H264_pic_parameter_set_t));
	memset(&h264_slice_header, 0x00, sizeof(H264_slice_header_t));
	memset(&h264_slice_data, 0x00, sizeof(H264_slice_data_t));

	memset(&h264_decode_info, 0x00, sizeof(Video_decode_info_t));

	for (i = 0; i < MAXSPS; i++)
	{
		h264_seq_parameter_set[i].seq_parameter_set_id = -1;
	}

	for (i = 0; i < MAXPPS; i++)
	{
		h264_pic_parameter_set[i].pic_parameter_set_id = -1;
	}

	ph264_macroblock_layer = NULL;
	pCurSPS = NULL;
	pCurPPS = NULL;

//#if VID_YUV2RGB_FastAlgorithm
//	Initialize_yuv2rgb_LUT();
//#endif

	h264_decode_info.graph_with_grid = 0;

	ghAccessEvent = ::CreateEvent(NULL, 0, 1, NULL);
}

void h264_video_term(void)
{
	::WaitForSingleObject(ghAccessEvent, INFINITE);

	::CloseHandle(ghAccessEvent);
	ghAccessEvent = NULL;

	h264_decode_info.graph_with_grid = 0;
}

static int h264_video_decode_buffer_clear(void)
{
	if ((pucDecodeFrame[CC_Y] != NULL) &&
		(pucDecodeFrame[CC_Cb] != NULL) &&
		(pucDecodeFrame[CC_Cr] != NULL))
	{
		memset(pucDecodeFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);
		memset(pucDecodeFrame[CC_Cb], 0x80, h264_decode_info.chroma_buf_size);
		memset(pucDecodeFrame[CC_Cr], 0x80, h264_decode_info.chroma_buf_size);
	}

	return 0;
}

static int h264_video_reorder_decode_frame(const int slice_type)
{
#if H264_VID_DECODER_IBP
	uint8_t*	pucTemp;
#endif

	if (gbInitDecoder)
	{
		switch (slice_type)
		{
		case H264_SLICE_I0:
		case H264_SLICE_I1:
		case H264_SLICE_P0:
		case H264_SLICE_P1:

			m_nDebugFrameCount ++;

#if H264_VID_DECODER_IBP
			//interchange the buffer pointer
			pucTemp = pucForwardFrame[CC_Y];
			pucForwardFrame[CC_Y] = pucBackwardFrame[CC_Y];
			pucBackwardFrame[CC_Y] = pucTemp;

			pucTemp = pucForwardFrame[CC_Cb];
			pucForwardFrame[CC_Cb] = pucBackwardFrame[CC_Cb];
			pucBackwardFrame[CC_Cb] = pucTemp;

			pucTemp = pucForwardFrame[CC_Cr];
			pucForwardFrame[CC_Cr] = pucBackwardFrame[CC_Cr];
			pucBackwardFrame[CC_Cr] = pucTemp;
#endif
			//setup the decode frame buffer
			pucDecodeFrame[CC_Y] = pucBackwardFrame[CC_Y];
			pucDecodeFrame[CC_Cb] = pucBackwardFrame[CC_Cb];
			pucDecodeFrame[CC_Cr] = pucBackwardFrame[CC_Cr];

#if H264_VID_DECODER_CLEAR				
			h264_video_decode_buffer_clear();
#endif

			break;

		case H264_SLICE_B0:
		case H264_SLICE_B1:

			m_nDebugFrameCount ++;

			pucDecodeFrame[CC_Y] = pucAuxFrame[CC_Y];
			pucDecodeFrame[CC_Cb] = pucAuxFrame[CC_Cb];
			pucDecodeFrame[CC_Cr] = pucAuxFrame[CC_Cr];
		
#if H264_VID_DECODER_CLEAR				
			h264_video_decode_buffer_clear();
#endif
			break;

		default:

			pucDecodeFrame[CC_Y] = NULL;
			pucDecodeFrame[CC_Cb] = NULL;
			pucDecodeFrame[CC_Cr] = NULL;

			break;
		}
	}

	return 0;
}

static int h264_video_reorder_display_frame(const int slice_type)
{
	switch (slice_type)
	{
	case H264_SLICE_I0:
	case H264_SLICE_I1:
	case H264_SLICE_P0:
	case H264_SLICE_P1:
		
#if H264_VID_DECODER_IBP
		pucDisplayFrame[CC_Y] = pucForwardFrame[CC_Y];
		pucDisplayFrame[CC_Cb] = pucForwardFrame[CC_Cb];
		pucDisplayFrame[CC_Cr] = pucForwardFrame[CC_Cr];
#else
		pucDisplayFrame[CC_Y] = pucBackwardFrame[CC_Y];
		pucDisplayFrame[CC_Cb] = pucBackwardFrame[CC_Cb];
		pucDisplayFrame[CC_Cr] = pucBackwardFrame[CC_Cr];
#endif
		break;

//	case H264_B_FRAME:

//		pucDisplayFrame[CC_Y] = pucAuxFrame[CC_Y];
//		pucDisplayFrame[CC_Cb] = pucAuxFrame[CC_Cb];
//		pucDisplayFrame[CC_Cr] = pucAuxFrame[CC_Cr];

//		break;

	default:

		pucDisplayFrame[CC_Y] = NULL;
		pucDisplayFrame[CC_Cb] = NULL;
		pucDisplayFrame[CC_Cr] = NULL;

		break;
	}

	return 0;
}

void h264_video_open(int* pwidth, int* pheight, S8* pszFourCC, int strSize)
{
	::WaitForSingleObject(ghAccessEvent, INFINITE);

	if (!gbInitDecoder)
	{
		if (pCurSPS != NULL)
		{
			h264_decode_info.chroma_format = pCurSPS->chroma_format_idc;

			h264_decode_info.luma_width = pCurSPS->PicWidthInSamplesL;
			h264_decode_info.luma_height = pCurSPS->PicHeightInSamplesL;
			h264_decode_info.luma_pix_count = pCurSPS->PicWidthInSamplesL * pCurSPS->PicHeightInSamplesL;
			h264_decode_info.luma_buf_size = h264_decode_info.luma_pix_count;

			h264_decode_info.chroma_width = pCurSPS->PicWidthInSamplesC;
			h264_decode_info.chroma_height = pCurSPS->PicHeightInSamplesC;
			h264_decode_info.chroma_pix_count = pCurSPS->PicWidthInSamplesC * pCurSPS->PicWidthInSamplesC;
			h264_decode_info.chroma_buf_size = h264_decode_info.chroma_pix_count;

			h264_decode_info.mb_width = pCurSPS->PicWidthInMbs;
			h264_decode_info.mb_height_frame = pCurSPS->FrameHeightInMbs;
			h264_decode_info.mb_count = h264_decode_info.mb_width * h264_decode_info.mb_height_frame;

//			if (pCurSPS->PicWidthInSamplesL >= 1000)
//			{
//				h264_decode_info.decimate = 2;
//			}
//			else
//			{
//				h264_decode_info.decimate = 1;
//			}

//			h264_decode_info.graph_width = pCurSPS->PicWidthInSamplesL / h264_decode_info.decimate;
//			h264_decode_info.graph_height = pCurSPS->PicHeightInSamplesL / h264_decode_info.decimate;
//			h264_decode_info.display_width = h264_decode_info.graph_width;
//			h264_decode_info.display_height = h264_decode_info.graph_height;

			if (pucForwardFrame[CC_Y] != NULL)
			{
				free(pucForwardFrame[CC_Y]);
				pucForwardFrame[CC_Y] = NULL;
			}

			pucForwardFrame[CC_Y] = (uint8_t*)malloc(h264_decode_info.luma_buf_size);
			memset(pucForwardFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);

			if (pucForwardFrame[CC_Cb] != NULL)
			{
				free(pucForwardFrame[CC_Cb]);
				pucForwardFrame[CC_Cb] = NULL;
			}

			pucForwardFrame[CC_Cb] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucForwardFrame[CC_Cb], 0x00, h264_decode_info.chroma_buf_size);

			if (pucForwardFrame[CC_Cr] != NULL)
			{
				free(pucForwardFrame[CC_Cr]);
				pucForwardFrame[CC_Cr] = NULL;
			}

			pucForwardFrame[CC_Cr] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucForwardFrame[CC_Cr], 0x00, h264_decode_info.chroma_buf_size);

			if (pucBackwardFrame[CC_Y] != NULL)
			{
				free(pucBackwardFrame[CC_Y]);
				pucBackwardFrame[CC_Y] = NULL;
			}

			pucBackwardFrame[CC_Y] = (uint8_t*)malloc(h264_decode_info.luma_buf_size);
			memset(pucBackwardFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);

			if (pucBackwardFrame[CC_Cb] != NULL)
			{
				free(pucBackwardFrame[CC_Cb]);
				pucBackwardFrame[CC_Cb] = NULL;
			}

			pucBackwardFrame[CC_Cb] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucBackwardFrame[CC_Cb], 0x00, h264_decode_info.chroma_buf_size);

			if (pucBackwardFrame[CC_Cr] != NULL)
			{
				free(pucBackwardFrame[CC_Cr]);
				pucBackwardFrame[CC_Cr] = NULL;
			}

			pucBackwardFrame[CC_Cr] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucBackwardFrame[CC_Cr], 0x00, h264_decode_info.chroma_buf_size);

			if (pucAuxFrame[CC_Y] != NULL)
			{
				free(pucAuxFrame[CC_Y]);
				pucAuxFrame[CC_Y] = NULL;
			}

			pucAuxFrame[CC_Y] = (uint8_t*)malloc(h264_decode_info.luma_buf_size);
			memset(pucAuxFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);

			if (pucAuxFrame[CC_Cb] != NULL)
			{
				free(pucAuxFrame[CC_Cb]);
				pucAuxFrame[CC_Cb] = NULL;
			}

			pucAuxFrame[CC_Cb] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucAuxFrame[CC_Cb], 0x00, h264_decode_info.chroma_buf_size);

			if (pucAuxFrame[CC_Cr] != NULL)
			{
				free(pucAuxFrame[CC_Cr]);
				pucAuxFrame[CC_Cr] = NULL;
			}

			pucAuxFrame[CC_Cr] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucAuxFrame[CC_Cr], 0x00, h264_decode_info.chroma_buf_size);

			pucDisplayFrame[CC_Y] = NULL;
			pucDisplayFrame[CC_Cb] = NULL;
			pucDisplayFrame[CC_Cr] = NULL;

			pucDecodeFrame[CC_Y] = NULL;
			pucDecodeFrame[CC_Cb] = NULL;
			pucDecodeFrame[CC_Cr] = NULL;

//			render_h264_video_buf_size = h264_decode_info.graph_width * h264_decode_info.graph_height * 3;
//
//			if (render_h264_video_buf_rgb888 != NULL)
//			{
//				free(render_h264_video_buf_rgb888);
//				render_h264_video_buf_rgb888 = NULL;
//			}

//			render_h264_video_buf_rgb888 = (uint8_t*)malloc(render_h264_video_buf_size);
//			memset(render_h264_video_buf_rgb888, 0x00, render_h264_video_buf_size);

			ph264_macroblock_layer = (H264_macroblock_layer_t*)malloc(h264_decode_info.mb_count * sizeof(H264_macroblock_layer_t));

			m_nDebugFrameCount = 0;

			gbInitDecoder = 1;
		}
	}

//	if (pwidth != NULL)
//	{
//		*pwidth = h264_decode_info.graph_width;
//	}
//	if (pheight != NULL)
//	{
//		*pheight = h264_decode_info.graph_height;
//	}

	h264_decode_info.graph_with_grid = 0;
	
	::SetEvent(ghAccessEvent);
}

void h264_video_reset(int* pwidth, int* pheight)
{
	int			luma_buf_size;
	int			luma_pix_count;

	int			chroma_buf_size;
	int			chroma_pix_count;

	::WaitForSingleObject(ghAccessEvent, INFINITE);

	if (gbInitDecoder)
	{
		if (pCurSPS != NULL)
		{
			luma_pix_count = pCurSPS->PicWidthInSamplesL * pCurSPS->PicHeightInSamplesL;
			luma_buf_size = luma_pix_count;

			chroma_pix_count = pCurSPS->PicWidthInSamplesC * pCurSPS->PicWidthInSamplesC;
			chroma_buf_size = chroma_pix_count;

			h264_decode_info.chroma_format = pCurSPS->chroma_format_idc;

			h264_decode_info.mb_width = pCurSPS->PicWidthInMbs;
			h264_decode_info.mb_height_frame = pCurSPS->FrameHeightInMbs;
			h264_decode_info.mb_count = h264_decode_info.mb_width * h264_decode_info.mb_height_frame;

//			if (pCurSPS->PicWidthInSamplesL >= 1000)
//			{
//				h264_decode_info.decimate = 2;
//			}
//			else
//			{
//				h264_decode_info.decimate = 1;
//			}

//			h264_decode_info.graph_width = pCurSPS->PicWidthInSamplesL / h264_decode_info.decimate;
//			h264_decode_info.graph_height = pCurSPS->PicHeightInSamplesL / h264_decode_info.decimate;

			if (pucForwardFrame[CC_Y] != NULL)
			{
				free(pucForwardFrame[CC_Y]);
				pucForwardFrame[CC_Y] = NULL;
			}

			pucForwardFrame[CC_Y] = (uint8_t*)malloc(h264_decode_info.luma_buf_size);
			memset(pucForwardFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);

			if (pucForwardFrame[CC_Cb] != NULL)
			{
				free(pucForwardFrame[CC_Cb]);
				pucForwardFrame[CC_Cb] = NULL;
			}

			pucForwardFrame[CC_Cb] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucForwardFrame[CC_Cb], 0x00, h264_decode_info.chroma_buf_size);

			if (pucForwardFrame[CC_Cr] != NULL)
			{
				free(pucForwardFrame[CC_Cr]);
				pucForwardFrame[CC_Cr] = NULL;
			}

			pucForwardFrame[CC_Cr] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucForwardFrame[CC_Cr], 0x00, h264_decode_info.chroma_buf_size);

			if (pucBackwardFrame[CC_Y] != NULL)
			{
				free(pucBackwardFrame[CC_Y]);
				pucBackwardFrame[CC_Y] = NULL;
			}

			pucBackwardFrame[CC_Y] = (uint8_t*)malloc(h264_decode_info.luma_buf_size);
			memset(pucBackwardFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);

			if (pucBackwardFrame[CC_Cb] != NULL)
			{
				free(pucBackwardFrame[CC_Cb]);
				pucBackwardFrame[CC_Cb] = NULL;
			}

			pucBackwardFrame[CC_Cb] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucBackwardFrame[CC_Cb], 0x00, h264_decode_info.chroma_buf_size);

			if (pucBackwardFrame[CC_Cr] != NULL)
			{
				free(pucBackwardFrame[CC_Cr]);
				pucBackwardFrame[CC_Cr] = NULL;
			}

			pucBackwardFrame[CC_Cr] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucBackwardFrame[CC_Cr], 0x00, h264_decode_info.chroma_buf_size);

			if (pucAuxFrame[CC_Y] != NULL)
			{
				free(pucAuxFrame[CC_Y]);
				pucAuxFrame[CC_Y] = NULL;
			}

			pucAuxFrame[CC_Y] = (uint8_t*)malloc(h264_decode_info.luma_buf_size);
			memset(pucAuxFrame[CC_Y], 0x00, h264_decode_info.luma_buf_size);

			if (pucAuxFrame[CC_Cb] != NULL)
			{
				free(pucAuxFrame[CC_Cb]);
				pucAuxFrame[CC_Cb] = NULL;
			}

			pucAuxFrame[CC_Cb] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucAuxFrame[CC_Cb], 0x00, h264_decode_info.chroma_buf_size);

			if (pucAuxFrame[CC_Cr] != NULL)
			{
				free(pucAuxFrame[CC_Cr]);
				pucAuxFrame[CC_Cr] = NULL;
			}

			pucAuxFrame[CC_Cr] = (uint8_t*)malloc(h264_decode_info.chroma_buf_size);
			memset(pucAuxFrame[CC_Cr], 0x00, h264_decode_info.chroma_buf_size);

			pucDisplayFrame[CC_Y] = NULL;
			pucDisplayFrame[CC_Cb] = NULL;
			pucDisplayFrame[CC_Cr] = NULL;

			pucDecodeFrame[CC_Y] = NULL;
			pucDecodeFrame[CC_Cb] = NULL;
			pucDecodeFrame[CC_Cr] = NULL;

			if (ph264_macroblock_layer != NULL)
			{
				free(ph264_macroblock_layer);
				ph264_macroblock_layer = NULL;
			}
			ph264_macroblock_layer = (H264_macroblock_layer_t*)malloc(h264_decode_info.mb_count * sizeof(H264_macroblock_layer_t));

//			render_h264_video_buf_size = h264_decode_info.graph_width * h264_decode_info.graph_height * 3;

//			if (render_h264_video_buf_rgb888 != NULL)
//			{
//				free(render_h264_video_buf_rgb888);
//				render_h264_video_buf_rgb888 = NULL;
//			}

//			render_h264_video_buf_rgb888 = (uint8_t*)malloc(render_h264_video_buf_size);
//			memset(render_h264_video_buf_rgb888, 0x00, render_h264_video_buf_size);
		}
	}

//	if (pwidth != NULL)
//	{
//		*pwidth = h264_decode_info.graph_width;
//	}
//	if (pheight != NULL)
//	{
//		*pheight = h264_decode_info.graph_height;
//	}

	h264_decode_info.graph_with_grid = 0;

	::SetEvent(ghAccessEvent);
}

void h264_video_close(void)
{
	::WaitForSingleObject(ghAccessEvent, INFINITE);

	if (gbInitDecoder)
	{
		gbInitDecoder = 0;

		if (pucForwardFrame[CC_Y])
		{
			free(pucForwardFrame[CC_Y]);
			pucForwardFrame[CC_Y] = NULL;
		}

		if (pucForwardFrame[CC_Cb])
		{
			free(pucForwardFrame[CC_Cb]);
			pucForwardFrame[CC_Cb] = NULL;
		}

		if (pucForwardFrame[CC_Cr])
		{
			free(pucForwardFrame[CC_Cr]);
			pucForwardFrame[CC_Cr] = NULL;
		}

		if (pucBackwardFrame[CC_Y])
		{
			free(pucBackwardFrame[CC_Y]);
			pucBackwardFrame[CC_Y] = NULL;
		}

		if (pucBackwardFrame[CC_Cb])
		{
			free(pucBackwardFrame[CC_Cb]);
			pucBackwardFrame[CC_Cb] = NULL;
		}
	
		if (pucBackwardFrame[CC_Cr])
		{
			free(pucBackwardFrame[CC_Cr]);
			pucBackwardFrame[CC_Cr] = NULL;
		}

		if (pucAuxFrame[CC_Y])
		{
			free(pucAuxFrame[CC_Y]);
			pucAuxFrame[CC_Y] = NULL;
		}

		if (pucAuxFrame[CC_Cb])
		{
			free(pucAuxFrame[CC_Cb]);
			pucAuxFrame[CC_Cb] = NULL;
		}
		
		if (pucAuxFrame[CC_Cr])
		{
			free(pucAuxFrame[CC_Cr]);
			pucAuxFrame[CC_Cr] = NULL;
		}

		if (ph264_macroblock_layer != NULL)
		{
			free(ph264_macroblock_layer);
			ph264_macroblock_layer = NULL;
		}

//		if (render_h264_video_buf_rgb888 != NULL)
//		{
//			free(render_h264_video_buf_rgb888);
//			render_h264_video_buf_rgb888 = NULL;
//		}

		pucDisplayFrame[CC_Y] = NULL;
		pucDisplayFrame[CC_Cb] = NULL;
		pucDisplayFrame[CC_Cr] = NULL;

		pucDecodeFrame[CC_Y] = NULL;
		pucDecodeFrame[CC_Cb] = NULL;
		pucDecodeFrame[CC_Cr] = NULL;


#if AVS_VID_DECODER_DEBUG
		if (m_pucY != NULL)
		{
			free(m_pucY);
			m_pucY = NULL;
		}
		
		if (m_pucCb != NULL)
		{
			free(m_pucCb);
			m_pucCb = NULL;
		}

		if (m_pucCr != NULL)
		{
			free(m_pucCr);
			m_pucCr = NULL;
		}

		if (pMBInfo != NULL)
		{
			free(pMBInfo);
			pMBInfo = NULL;
		}

		if (m_hDebugFile)
		{
			fclose(m_hDebugFile);
			m_hDebugFile = NULL;
		}

		if (m_hDebugYUV)
		{
			fclose(m_hDebugYUV);
			m_hDebugYUV = NULL;
		}
		
#endif
		m_nDebugFrameCount = 0;
	}

//	h264_decode_info.decimate = 1;
//	h264_decode_info.graph_width = 0;
//	h264_decode_info.graph_height = 0;

	h264_decode_info.graph_with_grid = 0;

	::SetEvent(ghAccessEvent);
}

void h264_video_setgrid(void)
{
	h264_decode_info.graph_with_grid = !h264_decode_info.graph_with_grid;
}

H264_MB_INFO_t* h264_video_get_macroblock(int macroblockindex)
{
	return NULL;
}

//返回值：下一个单元的类型。当前类型可由pnal_unit带回
//int h264_video_get_annexb_nal_unit(FIFO_t* pfifo, H264_nal_unit_t* pnal_unit)
//{
//	int		next_start_code = -1;
//
//	uint8_t		nal_byte;
//	uint32_t		next_bits;
//	int		NumBytesInRBSP;
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
//		next_start_code = FIFO_Peek(pfifo, 24);
//		if (next_start_code == 0x000001)
//		{
//			NumBytesInRBSP = 0;
//
//			pnal_unit->rbsp_byte[NumBytesInRBSP++] = FIFO_Get(pfifo, 8);
//			pnal_unit->rbsp_byte[NumBytesInRBSP++] = FIFO_Get(pfifo, 8);
//			pnal_unit->rbsp_byte[NumBytesInRBSP++] = FIFO_Get(pfifo, 8);
//
//			nal_byte = FIFO_Get(pfifo, 8);
//			pnal_unit->rbsp_byte[NumBytesInRBSP++] = nal_byte;
//
//			pnal_unit->start_code_prefix = next_start_code;
//
//			pnal_unit->forbidden_zero_bit = (nal_byte & 0x80) >> 7;
//			pnal_unit->nal_ref_idc = (nal_byte & 0x60) >> 5;
//			pnal_unit->nal_unit_type = (nal_byte & 0x1f);
//
//			while (1)
//			{
//				next_bits = FIFO_Peek(pfifo, 24);
//
//				if (next_bits == 0x000001)			//结束
//				{
//					pnal_unit->NumBytesInRBSP = NumBytesInRBSP;
//
//					next_start_code = (FIFO_Peek(pfifo, 32) & 0x0000001f);			//next_nal_unit_type
//					break;
//				}
//				else if (next_bits == 0x000003)
//				{
//					pnal_unit->rbsp_byte[NumBytesInRBSP++] = FIFO_Get(pfifo, 8);
//					pnal_unit->rbsp_byte[NumBytesInRBSP++] = FIFO_Get(pfifo, 8);
//					FIFO_Flush(pfifo, 8);
//				}
//				else
//				{
//					pnal_unit->rbsp_byte[NumBytesInRBSP++] = FIFO_Get(pfifo, 8);
//				}
//
//				if (pfifo->bitleft >= 24)
//				{
//					continue;
//				}
//				else
//				{
//					next_start_code = -1;
//					break;
//				}
//			} 
//
//			break;
//		}
//		else
//		{
//			FIFO_Flush(pfifo, 8);
//
//			if (pfifo->bitleft <= 0)
//			{
//				next_start_code = -1;
//				break;
//			}
//		}
//
//	} while (1);
//
//#if H264_VID_DECODER_DEBUG
//	if (m_hDebugFile)
//	{
//		fprintf(m_hDebugFile, "\nstartcode: NumBytesInRBSP = %d, nal_reference_idc = %d, nal_unit_type = %d\n", 
//								pnal_unit->NumBytesInRBSP,
//								pnal_unit->nal_ref_idc,
//								pnal_unit->nal_unit_type);
//	}
//#endif
//
//	return next_start_code;
//}

static int h264_video_more_rbsp_data(BITS_t* pbs)
{
	int	rtcode = 0;
	int	ctr_bit;
	int leftbyte;
	int	cnt;
	int	i;

	assert(pbs->p < pbs->p_end);

	//there is more until we're in the last byte
//	if (pbs->p < (pbs->p_end - 1))
	if (pbs->p < (pbs->p_end - 2))
	{
		rtcode = 1;
	}
	else
	{
		leftbyte = *pbs->p;

		ctr_bit = leftbyte & (1 << (pbs->i_left - 1));
		if (ctr_bit == 0)		//not meeting the stop bit
		{
			rtcode = 1;
		}
		else
		{
			cnt = 0;
			for (i = 0; i < (pbs->i_left - 1); i++)
			{
				ctr_bit = leftbyte & (1 << (pbs->i_left - 2 - i));
				if (ctr_bit > 0)
				{
					cnt ++;
				}
			}

			if (pbs->p == (pbs->p_end - 1))				//just one byte left
			{
				rtcode = (cnt != 0);
			}
			else										//two bytes left
			{
				rtcode = (cnt != 0) || (pbs->p[1] != 0);
			}
		}
	}

	return rtcode;
}

static int h264_video_scaling_list(BITS_t* pbs, S16* scalingList, int sizeOfScalingList, S16* useDefaultScalingMatrixFlag)
{
	int lastScale = 8;
	int nextScale = 8;
	int delta_scale;
	int	j;

	for (j = 0; j < sizeOfScalingList; j++)
	{
		if (nextScale != 0)
		{
			delta_scale = BITS_se(pbs);
			nextScale = (lastScale + delta_scale + 256) % 256;
			useDefaultScalingMatrixFlag[j] = (j == 0 && nextScale == 0);
		}

		scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
		lastScale = scalingList[j];
	}

	return 0;
}

static int h264_video_vui_parameters(BITS_t* pbs, pH264_vui_parameters_t pvui_parameters)
{
	pvui_parameters->aspect_ratio_info_present_flag = BITS_get(pbs, 1);

	if (pvui_parameters->aspect_ratio_info_present_flag)
	{
		pvui_parameters->aspect_ratio_idc = BITS_get(pbs, 8);

		if (pvui_parameters->aspect_ratio_idc == H264_EXTENDED_SAR)
		{
			pvui_parameters->sar_width = BITS_get(pbs, 16);
			pvui_parameters->sar_height = BITS_get(pbs, 16);
		}
	}

	pvui_parameters->overscan_info_present_flag = BITS_get(pbs, 1);
	if (pvui_parameters->overscan_info_present_flag)
	{
		pvui_parameters->overscan_appropriate_flag = BITS_get(pbs, 1);
	}

	//default values
	pvui_parameters->video_format = 5;
	pvui_parameters->video_full_range_flag = 0;
	pvui_parameters->colour_primaries = 2;
	pvui_parameters->transfer_characteristics = 2;

	pvui_parameters->video_signal_type_present_flag = BITS_get(pbs, 1);
	if (pvui_parameters->video_signal_type_present_flag)
	{
		pvui_parameters->video_format = BITS_get(pbs, 3);
		pvui_parameters->video_full_range_flag = BITS_get(pbs, 1);
		pvui_parameters->colour_description_present_flag = BITS_get(pbs, 1);
		if (pvui_parameters->colour_description_present_flag)
		{
			pvui_parameters->colour_primaries = BITS_get(pbs, 8);
			pvui_parameters->transfer_characteristics = BITS_get(pbs, 8);
			pvui_parameters->matrix_coefficients = BITS_get(pbs, 8);
		}
	}

	pvui_parameters->chroma_loc_info_present_flag = BITS_get(pbs, 1);
	if (pvui_parameters->chroma_loc_info_present_flag)
	{
		pvui_parameters->chroma_sample_loc_type_top_filed = BITS_ue(pbs);
		pvui_parameters->chroma_sample_loc_type_bottom_field = BITS_ue(pbs);
	}

	pvui_parameters->timing_info_present_flag = BITS_get(pbs, 1);
	if (pvui_parameters->timing_info_present_flag)
	{
		pvui_parameters->num_units_in_tick = BITS_get(pbs, 32);
		pvui_parameters->time_scale = BITS_get(pbs, 32);
		pvui_parameters->fixed_frame_rate_flag = BITS_get(pbs, 1);
	}

	pvui_parameters->nal_hrd_parameters_present_flag = BITS_get(pbs, 1);
	if (pvui_parameters->nal_hrd_parameters_present_flag)
	{
		//hrd_parameters()
	}

	pvui_parameters->vcl_hrd_parameters_present_flag = BITS_get(pbs, 1);
	if (pvui_parameters->vcl_hrd_parameters_present_flag)
	{
		//hrd_parameters()
	}

	if (pvui_parameters->nal_hrd_parameters_present_flag || pvui_parameters->vcl_hrd_parameters_present_flag)
	{
		pvui_parameters->low_delay_hrd_flag = BITS_get(pbs, 1);
	}

	pvui_parameters->pic_struct_present_flag = BITS_get(pbs, 1);
	pvui_parameters->bitstream_restriction_flag = BITS_get(pbs, 1);
	if (pvui_parameters->bitstream_restriction_flag)
	{
		pvui_parameters->motion_vectors_over_pic_boundaries_flag = BITS_get(pbs, 1);
		pvui_parameters->max_bytes_per_pic_denom = BITS_ue(pbs);
		pvui_parameters->max_bits_per_mb_denom = BITS_ue(pbs);
		pvui_parameters->log2_max_mv_length_horizontal = BITS_ue(pbs);
		pvui_parameters->log2_max_mv_length_vertical = BITS_ue(pbs);
		pvui_parameters->num_reorder_frames = BITS_ue(pbs);
		pvui_parameters->max_dec_frame_buffering = BITS_ue(pbs);
	}

	return 0;
}

H264_seq_parameter_set_t* h264_video_get_seq_parameter_set(void)
{
	return pCurSPS;
}

int h264_video_decode_seq_parameter_set(H264_nal_unit_t* pnal_unit)
{
	int		rtcode = 0;
	int		i;
	BITS_t	bs;

	int		profile_idc;
	int		constraint_set0_flag;
	int		constraint_set1_flag;
	int		constraint_set2_flag;
	int		constraint_set3_flag;
	int		reserved_zero_4bits;
	int		level_idc;
	int		seq_parameter_set_id;

	H264_seq_parameter_set_t*	pSPS;

	::WaitForSingleObject(ghAccessEvent, INFINITE);

	BITS_map(&bs, pnal_unit->rbsp_byte, pnal_unit->NumBytesInRBSP);

	BITS_skip(&bs, 32);		//flush start prefix code

	profile_idc = BITS_get(&bs, 8);
	
	constraint_set0_flag = BITS_get(&bs, 1);
	constraint_set1_flag = BITS_get(&bs, 1);
	constraint_set2_flag = BITS_get(&bs, 1);
	constraint_set3_flag = BITS_get(&bs, 1);
	reserved_zero_4bits = BITS_get(&bs, 4);

	level_idc = BITS_get(&bs, 8);

	seq_parameter_set_id = BITS_ue(&bs);

	if ((seq_parameter_set_id >= 0) && (seq_parameter_set_id < MAXSPS))
	{
		pSPS = &h264_seq_parameter_set[seq_parameter_set_id];

		pSPS->profile_idc = profile_idc;
		pSPS->constraint_set0_flag = constraint_set0_flag;
		pSPS->constraint_set1_flag = constraint_set1_flag;
		pSPS->constraint_set2_flag = constraint_set2_flag;
		pSPS->constraint_set3_flag = constraint_set3_flag;
		pSPS->reserved_zero_4bits = reserved_zero_4bits;

		pSPS->level_idc = level_idc;
		pSPS->seq_parameter_set_id = seq_parameter_set_id;

		pSPS->chroma_format_idc = 1;
		pSPS->bit_depth_luma_minus8   = 0;
		pSPS->bit_depth_chroma_minus8 = 0;
		pSPS->qpprime_y_zero_transform_bypass_flag = 0;

		// Residue Color Transform
		pSPS->residual_colour_transform_flag = 0;

		if ((pSPS->profile_idc == H264_PROFILE_FREXT_HIGH)	  ||
			(pSPS->profile_idc == H264_PROFILE_FREXT_HIGH_110) ||
			(pSPS->profile_idc == H264_PROFILE_FREXT_HIGH_122) ||
			(pSPS->profile_idc == H264_PROFILE_FREXT_HIGH_144))
		{
			pSPS->chroma_format_idc = BITS_ue(&bs);

			if (pSPS->chroma_format_idc == 3)
			{
				pSPS->residual_colour_transform_flag = BITS_get(&bs, 1);
			}

			pSPS->bit_depth_luma_minus8 = BITS_ue(&bs);
			pSPS->bit_depth_chroma_minus8 = BITS_ue(&bs);
			pSPS->qpprime_y_zero_transform_bypass_flag = BITS_get(&bs, 1);
			pSPS->seq_scaling_matrix_present_flag = BITS_get(&bs, 1);

			if (pSPS->seq_scaling_matrix_present_flag)
			{
				for (i = 0; i < 8; i++)
				{
					pSPS->seq_scaling_list_present_flag[i] = BITS_get(&bs, 1);

					if (pSPS->seq_scaling_list_present_flag[i])
					{
						if (i < 6)
						{
							h264_video_scaling_list(&bs, ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i]);
						}
						else
						{
							h264_video_scaling_list(&bs, ScalingList8x8[i], 64, UseDefaultScalingMatrix8x8Flag[i - 6]);
						}
					}
				}
			}
		}

		pSPS->log2_max_frame_num_minus4 = BITS_ue(&bs);
		pSPS->pic_order_cnt_type = BITS_ue(&bs);

		if (pSPS->pic_order_cnt_type == 0)
		{
			pSPS->log2_max_pic_order_cnt_lsb_minus4 = BITS_ue(&bs);
		}
		else if (pSPS->pic_order_cnt_type == 1)
		{
			pSPS->delta_pic_order_always_zero_flag = BITS_get(&bs, 1);
			pSPS->offset_for_non_ref_pic = BITS_se(&bs);
			pSPS->offset_for_top_to_bottom_field = BITS_se(&bs);
			pSPS->num_ref_frames_in_pic_order_cnt_cycle = BITS_ue(&bs);

			for (i = 0; i < (int)pSPS->num_ref_frames_in_pic_order_cnt_cycle; i++)
			{
				pSPS->offset_for_ref_frame[i] = BITS_se(&bs);
			}
		}

		pSPS->num_ref_frames = BITS_ue(&bs);
		pSPS->gaps_in_frame_num_value_allowed_flag = BITS_get(&bs, 1);
		pSPS->pic_width_in_mbs_minus1 = BITS_ue(&bs);
		pSPS->pic_height_in_map_units_minus1 = BITS_ue(&bs);
		pSPS->frame_mbs_only_flag = BITS_get(&bs, 1);

		pSPS->mb_adaptive_frame_field_flag = 0;				//默认值
		if (!pSPS->frame_mbs_only_flag)
		{
			pSPS->mb_adaptive_frame_field_flag = BITS_get(&bs, 1);
		}

		pSPS->direct_8x8_inference_flag = BITS_get(&bs, 1);
		pSPS->frame_cropping_flag = BITS_get(&bs, 1);

		if (pSPS->frame_cropping_flag)
		{
			pSPS->frame_crop_left_offset = BITS_ue(&bs);
			pSPS->frame_crop_right_offset = BITS_ue(&bs);
			pSPS->frame_crop_top_offset = BITS_ue(&bs);
			pSPS->frame_crop_bottom_offset = BITS_ue(&bs);
		}

		pSPS->vui_parameters_present_flag = BITS_get(&bs, 1);

		if (pSPS->vui_parameters_present_flag)
		{
			h264_video_vui_parameters(&bs, &(pSPS->vui_parameters));
		}

		//semantic part
		switch (pSPS->chroma_format_idc)
		{
		case CHROMA_FORMAT_MONO:
			pSPS->MbWidthC = 0;
			pSPS->MbHeightC = 0;
			pSPS->SubWidthC = -1;
			pSPS->SubHeightC = -1;
			break;
		case CHROMA_FORMAT_4_2_0:
			pSPS->MbWidthC = 8;
			pSPS->MbHeightC = 8;
			pSPS->SubWidthC = 2;
			pSPS->SubHeightC = 2;
			break;
		case CHROMA_FORMAT_4_2_2:
			pSPS->MbWidthC = 8;			
			pSPS->MbHeightC = 16;
			pSPS->SubWidthC = 2;
			pSPS->SubHeightC = 1;
			break;
		case CHROMA_FORMAT_4_4_4:
			pSPS->MbWidthC = 16;
			pSPS->MbHeightC = 16;
			pSPS->SubWidthC = 1;
			pSPS->SubHeightC = 1;
			break;
		}

		pSPS->BitDepthY = pSPS->bit_depth_luma_minus8 + 8;
		pSPS->BitDepthC = pSPS->bit_depth_chroma_minus8 + 8;

		pSPS->QpBdOffsetY = 6 * pSPS->bit_depth_luma_minus8;
		pSPS->QpBdOffsetC = 6 * (pSPS->bit_depth_chroma_minus8 + pSPS->residual_colour_transform_flag);

		pSPS->MaxFrameNum = (1 << (pSPS->log2_max_frame_num_minus4 + 4));
		pSPS->MaxPicOrderCntLsb = (1 << (pSPS->log2_max_pic_order_cnt_lsb_minus4 + 4));
		pSPS->PicWidthInMbs = pSPS->pic_width_in_mbs_minus1 + 1;
		pSPS->PicHeightInMapUnits = pSPS->pic_height_in_map_units_minus1 + 1;
		pSPS->PicSizeInMapUnits = pSPS->PicWidthInMbs * pSPS->PicHeightInMapUnits;
		pSPS->FrameHeightInMbs = (2 - pSPS->frame_mbs_only_flag) * pSPS->PicHeightInMapUnits;

		pSPS->PicWidthInSamplesL = pSPS->PicWidthInMbs * 16;
		pSPS->PicHeightInSamplesL = pSPS->FrameHeightInMbs * 16;
		pSPS->PicWidthInSamplesC = pSPS->PicWidthInMbs * pSPS->MbWidthC;
		pSPS->PicHeightInSamplesC = pSPS->FrameHeightInMbs * pSPS->MbHeightC;

		//defualt seq_parameter_set

		pCurSPS = pSPS;

#if H264_VID_DECODER_DEBUG

		if (m_hDebugFile == NULL)
		{
			m_hDebugFile = fopen("e:\\h264_debug\\h264_vid_decoder_debug.txt", "wt");
			m_hDebugYUV = fopen("e:\\h264_debug\\h264_vid_decoder_debug.yuv", "wb");
		}

		if (m_hDebugFile)
		{
			fprintf(m_hDebugFile, "\nseq_parameter_set_rbsp\n");
			fprintf(m_hDebugFile, " profile_idc = %d\n", pSPS->profile_idc);
			fprintf(m_hDebugFile, " level_idc = %d\n", pSPS->level_idc);

			if ((pSPS->profile_idc == H264_PROFILE_FREXT_HIGH)	  ||
				(pSPS->profile_idc == H264_PROFILE_FREXT_HIGH_110) ||
				(pSPS->profile_idc == H264_PROFILE_FREXT_HIGH_122) ||
				(pSPS->profile_idc == H264_PROFILE_FREXT_HIGH_144))
			{
				fprintf(m_hDebugFile, " chroma_format_idc = %d\n", pSPS->chroma_format_idc);
		//		fprintf(m_hDebugFile, "\n");
			}

			fprintf(m_hDebugFile, " seq_parameter_set_id = %d\n", pSPS->seq_parameter_set_id);
			fprintf(m_hDebugFile, " log2_max_frame_num_minus4 = %d\n", pSPS->log2_max_frame_num_minus4);
			fprintf(m_hDebugFile, " pic_order_cnt_type = %d\n", pSPS->pic_order_cnt_type);
			fprintf(m_hDebugFile, " num_ref_frames = %d\n", pSPS->num_ref_frames);
			fprintf(m_hDebugFile, " pic_width_in_mbs_minus1 = %d\n", pSPS->pic_width_in_mbs_minus1);
			fprintf(m_hDebugFile, " pic_height_in_map_units_minus1 = %d\n", pSPS->pic_height_in_map_units_minus1);
			fprintf(m_hDebugFile, " frame_mbs_only_flag = %d\n", pSPS->frame_mbs_only_flag);
		}
#endif
	}
	else
	{
		rtcode = -1;
	}

	::SetEvent(ghAccessEvent);

	return rtcode;
}

H264_pic_parameter_set_t* h264_video_get_pic_parameter_set(void)
{
	return pCurPPS;
}

int h264_video_decode_pic_parameter_set(H264_nal_unit_t* pnal_unit)
{
	int		rtcode = 0;
	int		iGroup;
	int		i;
	BITS_t	bs;
	int		pic_parameter_set_id;
	
	H264_pic_parameter_set_t*	pPPS;

	::WaitForSingleObject(ghAccessEvent, INFINITE);

	BITS_map(&bs, pnal_unit->rbsp_byte, pnal_unit->NumBytesInRBSP);

	BITS_skip(&bs, 32);		//flush start prefix code

	pic_parameter_set_id = BITS_ue(&bs);

	if ((pic_parameter_set_id >= 0) && (pic_parameter_set_id < MAXPPS))
	{
		pPPS = &h264_pic_parameter_set[pic_parameter_set_id];

		pPPS->pic_parameter_set_id = pic_parameter_set_id;

		pPPS->seq_parameter_set_id = BITS_ue(&bs);
		pPPS->entropy_coding_mode_flag = BITS_get(&bs, 1);
		pPPS->pic_order_present_flag = BITS_get(&bs, 1);
		pPPS->num_slice_groups_minus1 = BITS_ue(&bs);

		if (pPPS->num_slice_groups_minus1 > 0)
		{
			pPPS->slice_group_map_type = BITS_ue(&bs);

			if (pPPS->slice_group_map_type == 0)
			{
				for (iGroup = 0; iGroup <= (int)pPPS->num_slice_groups_minus1; iGroup++)
				{
					pPPS->run_length_minus1[iGroup] = BITS_ue(&bs);
				}
			}
			else if (pPPS->slice_group_map_type == 2)
			{
				for (iGroup = 0; iGroup <= (int)pPPS->num_slice_groups_minus1; iGroup++)
				{
					pPPS->top_left[iGroup] = BITS_ue(&bs);
					pPPS->bottom_right[iGroup] = BITS_ue(&bs);
				}
			}
			else if ((pPPS->slice_group_map_type == 3) ||
					 (pPPS->slice_group_map_type == 4) ||
					 (pPPS->slice_group_map_type == 5))
			{
				pPPS->slice_group_change_direction_flag = BITS_get(&bs, 1);
				pPPS->slice_group_change_rate_minus1 = BITS_ue(&bs);
			}
			else if (pPPS->slice_group_map_type == 6)
			{
				pPPS->pic_size_in_map_units_minus1 = BITS_ue(&bs);

				int bits = CeilLog2(pPPS->num_slice_groups_minus1 + 1);
				for (i = 0; i <= (int)pPPS->pic_size_in_map_units_minus1; i++)
				{
					pPPS->slice_group_id[i] = BITS_get(&bs, bits);
				}
			}
		}

		pPPS->num_ref_idx_l0_active_minus1 = BITS_ue(&bs);
		pPPS->num_ref_idx_l1_active_minus1 = BITS_ue(&bs);
		pPPS->weighted_pred_flag = BITS_get(&bs, 1);
		pPPS->weighted_bipred_idc = BITS_get(&bs, 2);
		pPPS->pic_init_qp_minus26 = BITS_se(&bs);
		pPPS->pic_init_qs_minus26 = BITS_se(&bs);
		pPPS->chroma_qp_index_offset = BITS_se(&bs);
		pPPS->deblocking_filter_control_present_flag = BITS_get(&bs, 1);
		pPPS->constrained_intra_pred_flag = BITS_get(&bs, 1);
		pPPS->redundant_pic_cnt_present_flag = BITS_get(&bs, 1);

		if (h264_video_more_rbsp_data(&bs))
		{
			pPPS->transform_8x8_mode_flag = BITS_get(&bs, 1);
			pPPS->pic_scaling_matrix_present_flag = BITS_get(&bs, 1);
			if (pPPS->pic_scaling_matrix_present_flag)
			{
				for (i = 0; i < (6 + 2 * pPPS->transform_8x8_mode_flag); i++)
				{
					pPPS->pic_scaling_list_present_flag[i] = BITS_get(&bs, 1);

					if (pPPS->pic_scaling_list_present_flag[i])
					{
						if (i < 6)
						{
							h264_video_scaling_list(&bs, ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i]);
						}
						else
						{
							h264_video_scaling_list(&bs, ScalingList8x8[i], 64, UseDefaultScalingMatrix8x8Flag[i - 6]);
						}
					}
				}
			}

			pPPS->second_chroma_qp_index_offset = BITS_se(&bs);
		}
		else
		{
			pPPS->transform_8x8_mode_flag = 0;
			pPPS->pic_scaling_matrix_present_flag = 0;
			pPPS->second_chroma_qp_index_offset = 0;
		}

		//semantic part

		//set default pic_parameter_set
		pCurPPS = pPPS;
	
		memset(&h264_slice_header, 0x00, sizeof(H264_slice_header_t));

#if H264_VID_DECODER_DEBUG
		if (m_hDebugFile)
		{
			fprintf(m_hDebugFile, "\npic_parameter_set_rbsp\n");
			fprintf(m_hDebugFile, "pic_parameter_set_id = %d\n", pPPS->pic_parameter_set_id);
			fprintf(m_hDebugFile, "seq_parameter_set_id = %d\n", pPPS->seq_parameter_set_id);
			fprintf(m_hDebugFile, "entropy_coding_mode_flag = %d\n", pPPS->entropy_coding_mode_flag);
			fprintf(m_hDebugFile, "pic_order_present_flag = %d\n", pPPS->pic_order_present_flag);
			fprintf(m_hDebugFile, "num_slice_groups_minus1 = %d\n", pPPS->num_slice_groups_minus1);

			if (pPPS->num_slice_groups_minus1)
			{
				fprintf(m_hDebugFile, "slice_group_map_type = %d\n", pPPS->slice_group_map_type); 
			}

			fprintf(m_hDebugFile, "num_ref_idx_l0_active_minus1 = %d\n", pPPS->num_ref_idx_l0_active_minus1);
			fprintf(m_hDebugFile, "num_ref_idx_l1_active_minus1 = %d\n", pPPS->num_ref_idx_l1_active_minus1);

			fprintf(m_hDebugFile, "weighted_pred_flag = %d\n", pPPS->weighted_pred_flag);
			fprintf(m_hDebugFile, "weighted_bipred_idc = %d\n", pPPS->weighted_bipred_idc);
			fprintf(m_hDebugFile, "pic_init_qp_minus26 = %d\n", pPPS->pic_init_qp_minus26);
			fprintf(m_hDebugFile, "pic_init_qs_minus26 = %d\n", pPPS->pic_init_qs_minus26);
			fprintf(m_hDebugFile, "chroma_qp_index_offset = %d\n", pPPS->chroma_qp_index_offset);
			fprintf(m_hDebugFile, "deblocking_filter_control_present_flag = %d\n", pPPS->deblocking_filter_control_present_flag);
			fprintf(m_hDebugFile, "constrained_intra_pred_flag = %d\n", pPPS->constrained_intra_pred_flag);
			fprintf(m_hDebugFile, "redundant_pic_cnt_present_flag = %d\n", pPPS->redundant_pic_cnt_present_flag);

			fprintf(m_hDebugFile, "\n");
		}
#endif
	}
	else
	{
		rtcode = -1;
	}

	::SetEvent(ghAccessEvent);

	return rtcode;
}

H264_slice_header_t* h264_video_get_slice_header(void)
{
	return &h264_slice_header;
}

static int h264_video_decode_pred_weight_table(BITS_t* pbs, H264_pred_weight_table_t* ppred_weight_table)
{
	int i;
	int j;

	ppred_weight_table->luma_log2_weight_denom = BITS_ue(pbs);
	ppred_weight_table->chroma_log2_weight_denom = BITS_ue(pbs);

	for (i = 0; i <= h264_slice_header.num_ref_idx_l0_active_minus1; i++)
	{
		ppred_weight_table->luma_weight_l0_flag[i] = BITS_get(pbs, 1);
		if (ppred_weight_table->luma_weight_l0_flag[i])
		{
			ppred_weight_table->luma_weight_l0[i] = BITS_se(pbs);
			ppred_weight_table->luma_offset_l0[i] = BITS_se(pbs);
		}

		ppred_weight_table->chroma_weight_l0_flag[i] = BITS_get(pbs, 1);
		if (ppred_weight_table->chroma_weight_l0_flag[i])
		{
			for (j = 0; j < 2; j++)
			{
				ppred_weight_table->chroma_weight_l0[i][j] = BITS_se(pbs);
				ppred_weight_table->chroma_offset_l0[i][j] = BITS_se(pbs);
			}
		}
	}

	if ((h264_slice_header.slice_type == H264_SLICE_B0) ||
		(h264_slice_header.slice_type == H264_SLICE_B1))
	{
		for (i = 0; i <= h264_slice_header.num_ref_idx_l1_active_minus1; i++)
		{
			ppred_weight_table->luma_weight_l1_flag[i] = BITS_get(pbs, 1);
			if (ppred_weight_table->luma_weight_l1_flag[i])
			{
				ppred_weight_table->luma_weight_l1[i] = BITS_se(pbs);
				ppred_weight_table->luma_offset_l1[i] = BITS_se(pbs);
			}

			ppred_weight_table->chroma_weight_l1_flag[i] = BITS_get(pbs, 1);
			if (ppred_weight_table->chroma_weight_l1_flag[i])
			{
				for (j = 0; j < 2; j++)
				{
					ppred_weight_table->chroma_weight_l1[i][j] = BITS_se(pbs);
					ppred_weight_table->chroma_offset_l1[i][j] = BITS_se(pbs);
				}
			}
		}
	}

	return 0;
}

static int h264_video_decode_ref_pic_list_reordering(BITS_t* pbs, int slice_type, H264_ref_pic_list_reordering_t* pref_pic_list_reordering)
{
	int		i;

	if ((slice_type != H264_SLICE_I0) &&
		(slice_type != H264_SLICE_SI0) &&
		(slice_type != H264_SLICE_I1) &&
		(slice_type != H264_SLICE_SI1))
	{
		pref_pic_list_reordering->ref_pic_list_reordering_flag_l0 = BITS_get(pbs, 1);

		if (pref_pic_list_reordering->ref_pic_list_reordering_flag_l0)
		{
			i = 0;

			do
			{
				pref_pic_list_reordering->reordering_of_pic_nums_idc[0] = BITS_ue(pbs);

				if ((pref_pic_list_reordering->reordering_of_pic_nums_idc[0] == 0) ||
					(pref_pic_list_reordering->reordering_of_pic_nums_idc[0] == 1))
				{
					pref_pic_list_reordering->abs_diff_pic_num_minus1[0][i] = BITS_ue(pbs);
				}
				else if (pref_pic_list_reordering->reordering_of_pic_nums_idc[0] == 2)
				{
					pref_pic_list_reordering->long_term_pic_num[0][i] = BITS_ue(pbs);
				}

				i ++;

			} while (pref_pic_list_reordering->reordering_of_pic_nums_idc[0] != 3);
		}
	}

	if ((slice_type == H264_SLICE_B0) ||
		(slice_type == H264_SLICE_B1))
	{
		pref_pic_list_reordering->ref_pic_list_reordering_flag_l1 = BITS_get(pbs, 1);

		if (pref_pic_list_reordering->ref_pic_list_reordering_flag_l1)
		{
			i = 0;

			do
			{
				pref_pic_list_reordering->reordering_of_pic_nums_idc[1] = BITS_ue(pbs);

				if ((pref_pic_list_reordering->reordering_of_pic_nums_idc[1] == 0) ||
					(pref_pic_list_reordering->reordering_of_pic_nums_idc[1] == 1))
				{
					pref_pic_list_reordering->abs_diff_pic_num_minus1[1][i] = BITS_ue(pbs);
				}
				else if (pref_pic_list_reordering->reordering_of_pic_nums_idc[1] == 2)
				{
					pref_pic_list_reordering->long_term_pic_num[1][i] = BITS_ue(pbs);
				}

				i++;

			} while (pref_pic_list_reordering->reordering_of_pic_nums_idc[1] != 3);
		}
	}

	return 0;
}

static int h264_video_decode_dec_ref_pic_marking(BITS_t* pbs, int nal_unit_type, H264_dec_ref_pic_marking_t* pdec_ref_pic_marking)
{
	int		i;

	if (nal_unit_type == 5)
	{
		pdec_ref_pic_marking->no_output_of_prior_pics_flag = BITS_get(pbs, 1);
		pdec_ref_pic_marking->long_term_reference_flag = BITS_get(pbs, 1);
	}
	else
	{
		pdec_ref_pic_marking->adaptive_ref_pic_marking_mode_flag = BITS_get(pbs, 1);
		if (pdec_ref_pic_marking->adaptive_ref_pic_marking_mode_flag)
		{
			i = 0;
			do
			{
				pdec_ref_pic_marking->memory_management_control_operation = BITS_ue(pbs);

				if ((pdec_ref_pic_marking->memory_management_control_operation == 1) ||
					(pdec_ref_pic_marking->memory_management_control_operation == 3))
				{
					pdec_ref_pic_marking->difference_of_pic_nums_minus1[i] = BITS_ue(pbs);
				}

				if (pdec_ref_pic_marking->memory_management_control_operation == 2)
				{
					pdec_ref_pic_marking->long_term_pic_num[i] = BITS_ue(pbs);
				}

				if ((pdec_ref_pic_marking->memory_management_control_operation == 3) ||
					(pdec_ref_pic_marking->memory_management_control_operation == 6))
				{
					pdec_ref_pic_marking->long_term_frame_idx[i] = BITS_ue(pbs);
				}

				if (pdec_ref_pic_marking->memory_management_control_operation == 4)
				{
					pdec_ref_pic_marking->max_long_term_frame_idx_plus1[i] = BITS_ue(pbs);
				}

				i++;

			} while (pdec_ref_pic_marking->memory_management_control_operation != 0);
		}
	}

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "\n");
		fprintf(m_hDebugFile, "no_output_of_prior_pics_flag = %d\n", pdec_ref_pic_marking->no_output_of_prior_pics_flag);
		fprintf(m_hDebugFile, "long_term_reference_flag = %d\n", pdec_ref_pic_marking->long_term_reference_flag);
		fprintf(m_hDebugFile, "\n");
	}
#endif

	return 0;
}

int h264_video_decode_slice_header(BITS_t* pbs, int nal_ref_idc, int nal_unit_type)
{
	int		rtcode = 0;
	int		bits;
	int		slice_type; 

	h264_slice_header.first_mb_in_slice = BITS_ue(pbs);
	h264_slice_header.slice_type = slice_type = BITS_ue(pbs);
	h264_slice_header.pic_parameter_set_id = BITS_ue(pbs);
	
	pCurPPS = &h264_pic_parameter_set[h264_slice_header.pic_parameter_set_id];
	pCurSPS = &h264_seq_parameter_set[pCurPPS->seq_parameter_set_id];

	bits = pCurSPS->log2_max_frame_num_minus4 + 4;
	h264_slice_header.frame_num = BITS_get(pbs, bits);

	h264_slice_header.field_pic_flag = 0;				//默认值设为0
	if (!pCurSPS->frame_mbs_only_flag)
	{
		h264_slice_header.field_pic_flag = BITS_get(pbs, 1);
		if (h264_slice_header.field_pic_flag)
		{
			h264_slice_header.bottom_field_flag = BITS_get(pbs, 1);
		}
	}

	if (nal_unit_type == 5)
	{
		h264_slice_header.idr_pic_id = BITS_ue(pbs);
	}

	if (pCurSPS->pic_order_cnt_type == 0)
	{
		bits = pCurSPS->log2_max_pic_order_cnt_lsb_minus4 + 4;
		h264_slice_header.pic_order_cnt_lsb = BITS_get(pbs, bits);

		if (pCurPPS->pic_order_present_flag && !h264_slice_header.field_pic_flag)
		{
			h264_slice_header.delta_pic_order_cnt_bottom = BITS_se(pbs);
		}
	}

	if ((pCurSPS->pic_order_cnt_type == 1) && (!pCurSPS->delta_pic_order_always_zero_flag))
	{
		h264_slice_header.delta_pic_order_cnt[0] = BITS_se(pbs);
		if (pCurPPS->pic_order_present_flag && !h264_slice_header.field_pic_flag)
		{
			h264_slice_header.delta_pic_order_cnt[1] = BITS_se(pbs);
		}
	}

	if (pCurPPS->redundant_pic_cnt_present_flag)
	{
		h264_slice_header.redundant_pic_cnt = BITS_ue(pbs);
	}

	if ((slice_type == H264_SLICE_B0) ||
		(slice_type == H264_SLICE_B1))
	{
		h264_slice_header.direct_spatial_mv_pred_flag = BITS_get(pbs, 1);
	}

	if ((slice_type == H264_SLICE_P0) ||
		(slice_type == H264_SLICE_SP0) ||
		(slice_type == H264_SLICE_B0) ||
		(slice_type == H264_SLICE_P1) ||
		(slice_type == H264_SLICE_SP1) ||
		(slice_type == H264_SLICE_B1))
	{
		h264_slice_header.num_ref_idx_active_override_flag = BITS_get(pbs, 1);
		if (h264_slice_header.num_ref_idx_active_override_flag)
		{
			h264_slice_header.num_ref_idx_l0_active_minus1 = BITS_ue(pbs);

			if ((slice_type == H264_SLICE_B0) ||
				(slice_type == H264_SLICE_B1))
			{
				h264_slice_header.num_ref_idx_l1_active_minus1 = BITS_ue(pbs);
			}
		}
	}

	h264_video_decode_ref_pic_list_reordering(pbs, slice_type, &(h264_slice_header.ref_pic_list_reordering));

	if ((pCurPPS->weighted_pred_flag && 
			((slice_type == H264_SLICE_P0) || (slice_type == H264_SLICE_P1) || (slice_type == H264_SLICE_SP0) || (slice_type == H264_SLICE_P1))) ||
		((pCurPPS->weighted_bipred_idc == 1) && ((slice_type == H264_SLICE_B0) || (slice_type == H264_SLICE_B1))))
	{
		h264_video_decode_pred_weight_table(pbs, &(h264_slice_header.pred_weight_table));
	}

	if (nal_ref_idc != 0)
	{
		h264_video_decode_dec_ref_pic_marking(pbs, nal_unit_type, &(h264_slice_header.dec_ref_pic_marking));
	}

	if (pCurPPS->entropy_coding_mode_flag &&
		(h264_slice_header.slice_type != H264_SLICE_I0) &&
		(h264_slice_header.slice_type != H264_SLICE_I1) &&
		(h264_slice_header.slice_type != H264_SLICE_SI0) &&
		(h264_slice_header.slice_type != H264_SLICE_SI1))
	{
		h264_slice_header.cabac_init_idc = BITS_ue(pbs);
	}

	h264_slice_header.slice_qp_delta = BITS_se(pbs);

	if ((slice_type == H264_SLICE_SP0) ||
		(slice_type == H264_SLICE_SI0) ||
		(slice_type == H264_SLICE_SP1) ||
		(slice_type == H264_SLICE_SI1))
	{
		if ((slice_type == H264_SLICE_SP0) ||
			(slice_type == H264_SLICE_SP1))
		{
			h264_slice_header.sp_for_switch_flag = BITS_get(pbs, 1);
		}

		h264_slice_header.slice_qs_delta = BITS_se(pbs);
	}

	if (pCurPPS->deblocking_filter_control_present_flag)
	{
		h264_slice_header.disable_deblocking_filter_idc = BITS_ue(pbs);

		if (h264_slice_header.disable_deblocking_filter_idc != 1)
		{
			h264_slice_header.slice_alpha_c0_offset_div2 = BITS_se(pbs);
			h264_slice_header.slice_beta_offset_div2 = BITS_se(pbs);
		}
	}

	if ((pCurPPS->num_slice_groups_minus1 > 0) &&
		(pCurPPS->slice_group_map_type >= 3) && 
		(pCurPPS->slice_group_map_type <= 5))
	{
		h264_slice_header.slice_group_change_cycle = BITS_get(pbs, bits);
	}

	//semantic part
	h264_slice_header.MbaffFrameFlag = (pCurSPS->mb_adaptive_frame_field_flag && !h264_slice_header.field_pic_flag);
	h264_slice_header.SliceQPy = 26 + pCurPPS->pic_init_qp_minus26 + h264_slice_header.slice_qp_delta;
	h264_slice_header.SliceQSy = 26 + pCurPPS->pic_init_qs_minus26 + h264_slice_header.slice_qs_delta;

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "first_mb_in_slice = %d\n", h264_slice_header.first_mb_in_slice);
		fprintf(m_hDebugFile, "slice_type = %d\n", h264_slice_header.slice_type);
		fprintf(m_hDebugFile, "pic_parameter_set_id = %d\n", h264_slice_header.pic_parameter_set_id);
		fprintf(m_hDebugFile, "frame_num = %d\n", h264_slice_header.frame_num);

		if (nal_unit_type == 5)
		{
			fprintf(m_hDebugFile, "idr_pic_id = %d\n", h264_slice_header.idr_pic_id);
		}

		fprintf(m_hDebugFile, "pic_order_cnt_lsb = %d\n", h264_slice_header.pic_order_cnt_lsb);
		fprintf(m_hDebugFile, "delta_pic_order_cnt_bottom = %d\n", h264_slice_header.delta_pic_order_cnt_bottom);

		if ((h264_slice_header.slice_type == H264_SLICE_P0) ||
			(h264_slice_header.slice_type == H264_SLICE_SP0) ||
			(h264_slice_header.slice_type == H264_SLICE_B0) ||
			(h264_slice_header.slice_type == H264_SLICE_P1) ||
			(h264_slice_header.slice_type == H264_SLICE_SP1) ||
			(h264_slice_header.slice_type == H264_SLICE_B1))
		{
			fprintf(m_hDebugFile, "num_ref_idx_active_override_flag = %d\n", h264_slice_header.num_ref_idx_active_override_flag);

			if (h264_slice_header.num_ref_idx_active_override_flag)
			{
				fprintf(m_hDebugFile, "num_ref_idx_l0_active_minus1 = %d\n", h264_slice_header.num_ref_idx_l0_active_minus1);
			}
		}

		if (pCurPPS->entropy_coding_mode_flag &&
			(h264_slice_header.slice_type != H264_SLICE_I0) &&
			(h264_slice_header.slice_type != H264_SLICE_I1) &&
			(h264_slice_header.slice_type != H264_SLICE_SI0) &&
			(h264_slice_header.slice_type != H264_SLICE_SI1))
		{
			h264_slice_header.cabac_init_idc = BITS_ue(pbs);
			fprintf(m_hDebugFile, "cabac_init_idc = %d\n", h264_slice_header.cabac_init_idc);
		}

		fprintf(m_hDebugFile, "slice_qp_delta = %d\n", h264_slice_header.slice_qp_delta);
		fprintf(m_hDebugFile, "slice_beta_offset_div2 = %d\n", h264_slice_header.slice_beta_offset_div2);
	}
#endif

	return rtcode;
}

_inline int h264_video_decode_NextMbAddress(int n)
{
	int		i;

	i = n + 1;

//	while ((i < h264_seq_parameter_set.PicSizeInMbs) && (MbToSliceGroupMap[i] != MbToSliceGroupMap[n]))
//	{
//		i ++;
//	}

	return i;
}

#define BLOCK_INDEX_CHROMA_DC		(-2)
#define BLOCK_INDEX_LUMA_DC			(-1)

static int h264_video_decode_residual_block_cabac(BITS_t* pbs, int mbindex, int i4x4Blkidx, S16* coeffLevel, int maxNumCoeff)
{
	int rtcode = 0;

	return rtcode;
}

static int h264_video_lookup_TotalCoeff_TrailingOnes(BITS_t* pbs, int vlcindex, int* pTotalCoeff, int* pTrailingOnes)
{
	int rtcode = 0;
	int coeff_token;
	int	len;
	int code;
	int	TrailingOnes;
	int TotalCoeff;
	int	bExit = 0;
	int MaxCoeff;

	if (vlcindex == 4)
	{
		MaxCoeff = 5;
	}
	else if (vlcindex == 5)
	{
		MaxCoeff = 9;
	}
	else
	{
		MaxCoeff = 17;
	}

	for (TrailingOnes = 0; TrailingOnes < 4; TrailingOnes ++)
	{
		for (TotalCoeff = TrailingOnes; TotalCoeff < MaxCoeff; TotalCoeff ++)
		{
			len = NUMVLC_lentab[vlcindex][TrailingOnes][TotalCoeff];
			code = BITS_show(pbs, len);

			coeff_token = NUMVLC_codtab[vlcindex][TrailingOnes][TotalCoeff];

			if (coeff_token == code)
			{
				BITS_skip(pbs, len);

#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, "\n vlcindex = %d, len = %d, coeff_token = 0x%x, TotalCoeff = %d, TrailingOnes = %d\n", vlcindex, len, coeff_token, TotalCoeff, TrailingOnes);
				}
#endif
				bExit = 1;
				break;
			}
		}

		if (bExit)
		{
			break;
		}
	}

	*pTotalCoeff = TotalCoeff;
	*pTrailingOnes = TrailingOnes;

	return rtcode;
}

static int h264_video_lookup_TotalZeros(BITS_t* pbs, int TotalCoeff, int maxNumCoeff)
{
	int total_zeros = 0;
	int i;
	int cycle;
	int len;
	int cod;
	int vlc;
	int index;
	int	vlcindex;

	if (maxNumCoeff == 4)
	{
		vlcindex = 0;
		cycle = 5 - TotalCoeff;
	}
	else if (maxNumCoeff == 8)
	{
		vlcindex = 1;
		cycle = 9 - TotalCoeff;
	}
	else
	{
		vlcindex = 2;
		cycle = 17 - TotalCoeff;
	}

	index = TotalCoeff - 1;

	for (i = 0; i < cycle; i++)
	{
		len = TOTALZEROS_lentab[vlcindex][index][i];
		cod = TOTALZEROS_codtab[vlcindex][index][i];

		vlc = BITS_show(pbs, len);
		if (cod == vlc)
		{
			BITS_skip(pbs, len);

			total_zeros = i;
			break;
		}
	}

	return total_zeros;
}

static int h264_video_lookup_RunBefore(BITS_t* pbs, int zerosLeft)
{
	int run_before = 0;
	int i;
	int cycle;
	int len;
	int cod;
	int vlc;
	int index;

	if (zerosLeft > 6)
	{
		index = 6;
		cycle = 15;
	}
	else
	{
		index = zerosLeft - 1;
		cycle = zerosLeft + 1;
	}

	for (i = 0; i < cycle; i++)
	{
		len = RUNBEFORE_lentab[index][i];
		cod = RUNBEFORE_codtab[index][i];

		vlc = BITS_show(pbs, len);
		if (cod == vlc)
		{
			BITS_skip(pbs, len);

			run_before = i;
			break;
		}
	}

	return run_before;
}

static int h264_video_mb_predict_non_zero_code(int mbindex, int i4x4BlkIdx)
{
	int		nC = 0;
	int		nA;
	int		nB;
	int		idxA;
	int		idxB;
	int		mbAddrA;
	int		mbAddrB;
	int		chromaIndex;

	H264_macroblock_layer_t*	pCurMB = ph264_macroblock_layer + mbindex;
	H264_macroblock_layer_t*	pMBA;
	H264_macroblock_layer_t*	pMBB;

	chromaIndex = H264_LUT_chromaindex[pCurSPS->chroma_format_idc];

	if (i4x4BlkIdx >= 16)		//chroma sub 4x4 block
	{
		nC = 0;
	}

	idxA = NEIGHBOUR_leftA[chromaIndex][i4x4BlkIdx];
	idxB = NEIGHBOUR_upperB[chromaIndex][i4x4BlkIdx];

	if (idxA >= 0)
	{
		nA = pCurMB->TotalCoeff[idxA];
	}
	else
	{
		if ((mbindex % pCurSPS->PicWidthInMbs) == 0)
		{
			mbAddrA = -1;
		}
		else
		{
			mbAddrA = mbindex - 1;
		}

		if (mbAddrA >= 0)
		{
			pMBA = ph264_macroblock_layer + mbAddrA;

			idxA += 48;
			nA = pMBA->TotalCoeff[idxA];
		}
		else
		{
			nA = 0;
		}
	}

	if (idxB >= 0)
	{
		nB = pCurMB->TotalCoeff[idxB];
	}
	else
	{
		mbAddrB = mbindex - pCurSPS->PicWidthInMbs;
		if (mbAddrB >= 0)
		{
			pMBB = ph264_macroblock_layer + mbAddrB;

			idxB += 48;
			nB = pMBB->TotalCoeff[idxB];
		}
		else
		{
			nB = 0;
		}
	}

	if ((idxA >= 0) && (idxB >= 0))
	{
		nC = (nA + nB + 1) >> 1;
	}
	else
	{
		nC = nA + nB;
	}

	return nC;
}

static int	h264_video_reconstruct_block(int mb_row, int mb_col, int i8x8, int i4x4, S16 QF[4][4])
{
	int		 rtcode = 0;
	int		 v, u;
	int		 w = 0;
	int		 pix_index = 0;
	int		 pix_offset;
	S16		 pix_value;	
	int		 sum = 0;
	int		 bx, by;
	int		 width;
	int		 pix_count;
	int		 cc;

	cc = H264_LUT_cc[i8x8];

	if (i8x8 >= 4) return 0;

	//存储到显存
	if (cc == CC_Y)
	{
		by = (((mb_row << 1) + (i8x8 >> 1)) << 3) + H264_LUT_4x4_YOFFSET[i4x4];
		bx = (((mb_col << 1) + (i8x8 & 1)) << 3) + H264_LUT_4x4_XOFFSET[i4x4];

		width = h264_decode_info.luma_width;
		pix_count = h264_decode_info.luma_pix_count;
	}
/*	else
	{
		switch (pCurSPS->chroma_format_idc)
		{
		case CHROMA_FORMAT_4_2_0:
			
			by = mb_row << 3;
			bx = mb_col << 3;
			break;

		case CHROMA_FORMAT_4_2_2:
		
			by = ((mb_row << 1) + ((i8x8 - 4) >> 1)) << 3;
			bx = mb_col << 3;
			break;

		case CHROMA_FORMAT_4_4_4:

			if (i8x8 < 8)
			{
				by = ((mb_row << 1) + ((i8x8 - 4) >> 1)) << 3;
			}
			else
			{
				by = ((mb_row << 1) + ((i8x8 - 4) >> 1)) << 3;
			}

			bx = ((mb_col << 1) + ((i8x8 - 4) >> 2)) << 3;
			
			break;

		default:
			break;
		}

		width = h264_decode_info.chroma_width;
		pix_count = h264_decode_info.chroma_pix_count;

	}
*/
//	if (mpeg_macroblock_modes.dct_type == 1)
	if (0)
	{
/*		if (cc == CC_Y)
		{
//			for (v = (block / 2); v < 16; v += 2)
			for (v = (i8x8 >> 1); v < 16; v += 2)
			{
				v_2 = (v >> 1);

				pix_index = ((mb_row << 4) + v) * width + bx;

				for (u = 0; u < 8; u++)
				{
					if ((pix_index >= 0) && (pix_index < pix_count))
					{
						//f[y][x]
//						pix_value = (QF[v/2][u] > 255) ? 255 : ((QF[v/2][u] < 0) ? 0 : QF[v/2][u]);
						pix_value = (QF[v_2][u] > 255) ? 255 : ((QF[v_2][u] < 0) ? 0 : QF[v_2][u]);

						if (pucDecodeFrame[cc])
						{
							pucDecodeFrame[cc][pix_index] = (uint8_t)pix_value;
						}

						pix_index ++;
					}
					else
					{
						rtcode = -1;
						break;
					}
				}
			}
		}
		else
		{
			pix_offset = by * width + bx;
			for (v = 0; v < 4; v ++)
			{
				pix_index = pix_offset;
				for (u = 0; u < 4; u++)
				{
					if (pix_index < pix_count)
					{
						//f[y][x]
						pix_value = (QF[v][u] > 255) ? 255 : ((QF[v][u] < 0) ? 0 : QF[v][u]);
						
						if (pucDecodeFrame[cc])
						{
							pucDecodeFrame[cc][pix_index] = (uint8_t)pix_value;
						}

						pix_index ++;
					}
					else
					{
						rtcode = -1;
						break;
					}
				}

				pix_offset += width;
			}
		}
*/	}
	else
	{
		pix_offset = by * width + bx;

		for (v = 0; v < 4; v ++)
		{
			pix_index = pix_offset;
			for (u = 0; u < 4; u++)
			{
				if ((pix_index >= 0) && (pix_index < pix_count))
				{
//					pix_value = QF[v][u] + 128;
//						if (mb_type == H264_I_8x8)//f[y][x]
//						{
//					pix_value = (QF[v][u] >= 0) ? QF[v][u] * 50 : -QF[v][u] * 50;
//					pix_value = (QF[v][u] >= 0) ? QF[v][u] : -QF[v][u];
					pix_value = (cc == CC_Y) ? H264_Clip1Y(QF[v][u]) : H264_Clip1C(QF[v][u]);
//					pix_value *= 10;
//						}
//						else				//f[y][x] + p[y][x]
//						{
//							pix_value = QF[v][u] + pel_pred[v][u];
//						}

					if (pucDecodeFrame[cc])
					{
						pucDecodeFrame[cc][pix_index] = (uint8_t)pix_value;
					}

					pix_index ++;
				}
				else
				{
					rtcode = -1;
					break;
				}
			}

			pix_offset += width;
		}
	}

	return rtcode;
}

static int h264_video_4x4_LevelScale(int m, int i, int j, int mbIsInterFlag, int iYCbCr)
{
	int	rtcode = 1;
	int weightScale;
	int	normAdjust;
	int	scalIndex = iYCbCr + ((mbIsInterFlag == 1) ? 3 : 0);
//	S16* ScalingList4x4 = &(ScalingList4x4[iYCbCr + ((mbIsInterFlag == 1) ? 3 : 0)][0]);

	weightScale = (mbIsInterFlag == 1) ? H264_Default_4x4_Inter[H264_4x4_SCAN[0][i][j]] : H264_Default_4x4_Intra[H264_4x4_SCAN[0][i][j]];

	if (((i % 2) == 0) && ((j % 2) == 0))
	{
		normAdjust = H264_LUT_v[m][0];
	}
	else if (((i % 2) == 1) && ((j % 2) == 1))
	{
		normAdjust = H264_LUT_v[m][1];
	}
	else
	{
		normAdjust = H264_LUT_v[m][2];
	}

	rtcode = weightScale * normAdjust;

	return rtcode;
}

static int h264_video_scaling_and_transform(int MbPartPredMode, int i4x4BlkIdx, int sMbFlag, H264_macroblock_layer_t* pMB, S16 c[4][4], S16 r[4][4])
{
	int	qP;
	int	rtcode = 0;
	int	i, j;
	S16	d[4][4];
	S16	e[4][4];
	S16	f[4][4];
	S16	g[4][4];
	S16	h[4][4];

	int bQP = 1;
	int mbIsInterFlag;
	int iYCbCr;

	//Intra macroblock
	if ((MbPartPredMode == Intra_4x4) ||
		(MbPartPredMode == Intra_8x8) ||
		(MbPartPredMode == Intra_16x16))
	{
		mbIsInterFlag = 0;
	}
	else
	{
		mbIsInterFlag = 1;
	}

	if (pCurSPS->qpprime_y_zero_transform_bypass_flag && (pMB->QPy_ == 0))		//bypass mode
	{
		for (i = 0; i < 4; i ++)
		{
			for (j = 0; j < 4; j ++)
			{
				r[i][j] = c[i][j];
			}
		}
	}
	else
	{
		if (((i4x4BlkIdx >= 0) && (i4x4BlkIdx < 16)) || (i4x4BlkIdx == BLOCK_INDEX_LUMA_DC))		//luma residual
		{
			if (sMbFlag == 0)
			{
				qP = pMB->QPy_;
			}
			else
			{
			}

			iYCbCr = 0;
		}
		else if ((i4x4BlkIdx >= 16) && (i4x4BlkIdx < 48))	//chroma residual
		{
			if (sMbFlag == 0)
			{
			}
			else
			{
			}

			if ((i4x4BlkIdx >= 16) && (i4x4BlkIdx < 32))
			{
				iYCbCr = 1;
			}
			else if ((i4x4BlkIdx >= 32) && (i4x4BlkIdx < 48))
			{
				iYCbCr = 2;
			}
		}

		//cij -> dij
		for (i = 0; i < 4; i ++)
		{
			for (j = 0; j < 4; j ++)
			{
				if ((i == 0) && (j == 0))			//DC coeffient
				{
					if ((i4x4BlkIdx == BLOCK_INDEX_LUMA_DC) || (i4x4BlkIdx == BLOCK_INDEX_CHROMA_DC))
					{
						d[0][0] = c[0][0];
						bQP = 0;
					}
				}
				else
				{
					bQP = 1;
				}

				if (bQP)
				{
					if (qP >= 24)
					{
						d[i][j] = (c[i][j] * h264_video_4x4_LevelScale(qP % 6, i, j, mbIsInterFlag, iYCbCr)) << (qP / 6 - 4);
					}
					else
					{
						d[i][j] = (c[i][j] * h264_video_4x4_LevelScale(qP % 6, i, j, mbIsInterFlag, iYCbCr) + (1 << (3 - qP / 6))) >> (4 - qP / 6);
					}
				}
			}
		}

		//dij -> eij
		for (i = 0; i < 4; i ++)
		{
			e[i][0] = d[i][0] + d[i][2];
			e[i][1] = d[i][0] - d[i][2];
			e[i][2] = (d[i][1] >> 1) - d[i][3];
			e[i][3] = d[i][1] + (d[i][3] >> 1);
		}

		//eij -> fij
		for (i = 0; i < 4; i ++)
		{
			f[i][0] = e[i][0] + e[i][3];
			f[i][1] = e[i][1] + e[i][2];
			f[i][2] = e[i][1] - e[i][2];
			f[i][3] = e[i][0] - e[i][3];
		}

		//fij -> gij
		for (j = 0; j < 4; j ++)
		{
			g[0][j] = f[0][j] + f[2][j];
			g[1][j] = f[0][j] - f[2][j];
			g[2][j] = (f[1][j] >> 1) - f[3][j];
			g[3][j] = f[1][j] + (f[3][j] >> 1);
		}

		//gij -> hij
		for (j = 0; j < 4; j ++)
		{
			h[0][j] = g[0][j] + g[3][j];
			h[1][j] = g[1][j] + g[2][j];
			h[2][j] = g[1][j] - g[2][j];
			h[3][j] = g[0][j] - g[3][j];
		}

		for (i = 0; i < 4; i ++)
		{
			for (j = 0; j < 4; j ++)
			{
				r[i][j] = (h[i][j] + 32) >> 6;
			}
		}
	}

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "\n scaling & transform: \n");
		for (i = 0; i < 4; i++)				//行序号
		{
			for (j = 0; j < 4; j++)			//列序号
			{
				fprintf(m_hDebugFile, " %3d", r[i][j]);
			}
			fprintf(m_hDebugFile, "\n");
		}
	}
#endif

	return 0;
}

static int h264_video_inverse_scan(S16 coeffLevel[16], S16 c[4][4])
{
	int		rtcode = 0;
	int		u, v;

	for (v = 0; v < 4; v++)				//行序号
	{
		for (u = 0; u < 4; u++)			//列序号
		{
			c[v][u] = coeffLevel[H264_4x4_SCAN[0][v][u]];		//frame macroblocks
		}
	}

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "\n inverse_scan: \n");
		for (v = 0; v < 4; v++)				//行序号
		{
			for (u = 0; u < 4; u++)			//列序号
			{
				fprintf(m_hDebugFile, " %3d", c[v][u]);
			}
			fprintf(m_hDebugFile, "\n");
		}
	}
#endif

	return rtcode;
}

static int h264_video_decode_residual_block_cavlc(BITS_t* pbs, int mbindex, int i4x4BlkIdx, S16* coeffLevel, int maxNumCoeff)
{
	int rtcode = 0;
	int	i;
	int	nC;
	int vlcindex;
	int TotalCoeff;
	int TrailingOnes;
	int	suffixLength;
	int	trailing_ones_sign_flag;
	int	level_prefix;
	int	level_suffix;
	int	levelCode;
	int	level[16];
	int run[16];
	int total_zeros;
	int zerosLeft;
	int coeffNum;
	int	len;
	int	levelSuffixSize;
	int run_before;

	H264_macroblock_layer_t* pCurMB = ph264_macroblock_layer + mbindex;

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "\n mbindex = %d, i4x4BlkIdx = %d, maxNumCoeff = %d\n", mbindex, i4x4BlkIdx, maxNumCoeff);
	}
#endif

	for (i = 0; i < maxNumCoeff; i++)
	{
		coeffLevel[i] = 0;
	}

	if (i4x4BlkIdx == BLOCK_INDEX_CHROMA_DC)
	{
		switch (pCurSPS->chroma_format_idc)
		{
		case CHROMA_FORMAT_4_2_0:
			nC = -1;
			vlcindex = 4;
			break;
		case CHROMA_FORMAT_4_2_2:
			nC = -2;
			vlcindex = 5;
			break;
		case CHROMA_FORMAT_4_4_4:
			nC = 0;
			vlcindex = 0;
			break;
		}

		rtcode = h264_video_lookup_TotalCoeff_TrailingOnes(pbs, vlcindex, &TotalCoeff, &TrailingOnes);
	}
	else
	{
		//derive nC
		if (i4x4BlkIdx == BLOCK_INDEX_LUMA_DC)
		{
			nC = h264_video_mb_predict_non_zero_code(mbindex, 0);
		}
		else
		{
			nC = h264_video_mb_predict_non_zero_code(mbindex, i4x4BlkIdx);
		}

		vlcindex = NVMVLC_nC_table[nC];

		rtcode = h264_video_lookup_TotalCoeff_TrailingOnes(pbs, vlcindex, &TotalCoeff, &TrailingOnes);

		//FIX ME
		if (i4x4BlkIdx != BLOCK_INDEX_LUMA_DC)
		{
			pCurMB->TotalCoeff[i4x4BlkIdx] = TotalCoeff;
		}
		else
		{
			pCurMB->TotalCoeff[0] = TotalCoeff;
		}
	}

	if (TotalCoeff > 0)
	{
		if ((TotalCoeff > 10) && (TrailingOnes < 3))
		{
			suffixLength = 1;
		}
		else
		{
			suffixLength = 0;
		}

		for (i = 0; i < TotalCoeff; i++)
		{
			if (i < TrailingOnes)
			{
				trailing_ones_sign_flag = BITS_get(pbs, 1);
#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, " trailing_ones_sign_flag = %d\n", trailing_ones_sign_flag);
				}
#endif
				level[i] = 1 - (trailing_ones_sign_flag << 1);
			}
			else
			{
				len = 1;
				while (!BITS_show(pbs, len))
				{
					len ++;
				}

				BITS_skip(pbs, len);

				level_prefix = len - 1;
				levelCode = (level_prefix << suffixLength);

				if ((suffixLength > 0) || (level_prefix >= 14))
				{
					levelSuffixSize = suffixLength;
					if ((level_prefix == 14) && (suffixLength == 0))
					{
						levelSuffixSize = 4;
					}
					if (level_prefix == 15)
					{
						levelSuffixSize = 12;
					}

					if (levelSuffixSize > 0)
					{
						level_suffix = BITS_get(pbs, levelSuffixSize);
						levelCode += level_suffix;
					}
				}

				if ((level_prefix == 15) && (suffixLength == 0))
				{
					levelCode += 15;
				}

				if ((i == TrailingOnes) && (TrailingOnes < 3))
				{
					levelCode += 2;
				}

				if (levelCode % 2 == 0)
				{
					level[i] = (levelCode + 2) >> 1;
				}
				else
				{
					level[i] = (-levelCode - 1) >> 1;
				}

				if (suffixLength == 0)
				{
					suffixLength = 1;
				}

				if ((abs(level[i]) > (3 << (suffixLength - 1))) && (suffixLength < 6))
				{
					suffixLength ++;
				}

#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, " level_prefix = %d, level[%d] = %d\n", level_prefix, i, level[i]);
				}
#endif
			}
		}
		
		if (TotalCoeff < maxNumCoeff)
		{
			total_zeros = h264_video_lookup_TotalZeros(pbs, TotalCoeff, maxNumCoeff);
#if H264_VID_DECODER_DEBUG
			if (m_hDebugFile)
			{
				fprintf(m_hDebugFile, " total_zeros = %d\n", total_zeros);
			}
#endif
			zerosLeft = total_zeros;
		}
		else
		{
			zerosLeft = 0;
		}

		for (i = 0; i < (TotalCoeff - 1); i++)
		{
			if (zerosLeft > 0)
			{
				run_before = h264_video_lookup_RunBefore(pbs, zerosLeft);
				run[i] = run_before;
#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, " run_before = %d\n", run_before);
				}
#endif
			}
			else
			{
				run[i] = 0;
			}

			zerosLeft = zerosLeft - run[i];
		}

		run[TotalCoeff - 1] = zerosLeft;
		coeffNum = -1;

		for (i = TotalCoeff - 1; i >= 0; i--)
		{
			coeffNum += run[i] + 1;
			coeffLevel[coeffNum] = level[i];
		}
	}

//	if (i4x4BlkIdx >= 0)
//	{
//	}

	return rtcode;
}

static int h264_video_decode_residual(BITS_t* pbs, int slice_type, int mbindex)
{
	int rtcode = 0;
	int (*residual_block)(BITS_t* pbs, int mbindex, int i4x4Blkidx, S16* coeffLevel, int maxNumCoeff);		//定义函数指针
	int	MbPartPredMode;
	int	iCbCr;
	int i8x8;
	int	i4x4;
	int i;
	int	i4x4BlkIdx;
	int cbp_flag;
	int mb_type;
	int	mb_row;
	int mb_col;
	int	NumC8x8;
	S16	c[4][4];
	S16	r[4][4];

	H264_macroblock_layer_t* pCurMB = ph264_macroblock_layer + mbindex;

	mb_type = pCurMB->mb_type;
	MbPartPredMode = LUT_MbPartPredMode[slice_type][0][mb_type];

	mb_row = mbindex / h264_decode_info.mb_width;
	mb_col = mbindex % h264_decode_info.mb_width;

	if (!pCurPPS->entropy_coding_mode_flag)
	{
		residual_block = h264_video_decode_residual_block_cavlc;
	}
	else
	{
		residual_block = h264_video_decode_residual_block_cabac;
	}

	if (MbPartPredMode == Intra_16x16)
	{
		residual_block(pbs, mbindex, BLOCK_INDEX_LUMA_DC, Intra16x16DCLevel, 16);
	}
	else
	{
		for (i = 0; i < 16; i++)
		{
			Intra16x16DCLevel[i] = 0;
		}
	}

	if (pCurMB->CodedBlockPatternLuma != 0)
	{
		i4x4BlkIdx = 0;
		for (i8x8 = 0; i8x8 < 4; i8x8 ++)			//each luma 8x8 block
		{
			cbp_flag = pCurMB->CodedBlockPatternLuma & (1 << i8x8);

			if (!pCurMB->transform_size_8x8_flag || !pCurPPS->entropy_coding_mode_flag)
			{
				for (i4x4 = 0; i4x4 < 4; i4x4 ++)
				{
					if (cbp_flag)
					{
						if (MbPartPredMode == Intra_16x16)
						{
							residual_block(pbs, mbindex, i4x4BlkIdx, Intra16x16ACLevel[i4x4BlkIdx], 15);
						}
						else
						{
							residual_block(pbs, mbindex, i4x4BlkIdx, LumaLevel[i4x4BlkIdx], 16);

							h264_video_inverse_scan(LumaLevel[i4x4BlkIdx], c);
							h264_video_scaling_and_transform(MbPartPredMode, i4x4BlkIdx, 0, pCurMB, c, r);
							h264_video_reconstruct_block(mb_row, mb_col, i8x8, i4x4, r);
						}
					}
					else 
					{
						if (MbPartPredMode == Intra_16x16)
						{
							for (i = 0; i < 15; i++)
							{
								Intra16x16ACLevel[i4x4BlkIdx][i] = 0;
							}
						}
						else
						{
							for (i = 0; i < 16; i++)
							{
								LumaLevel[i4x4BlkIdx][i] = 0;
							}
						}
					}

					if (!pCurPPS->entropy_coding_mode_flag && pCurMB->transform_size_8x8_flag)
					{
						for (i = 0; i < 16; i++)
						{
							LumaLevel8x8[i8x8][4 * i + i4x4] = LumaLevel[i4x4BlkIdx][i];
						}
					}

					i4x4BlkIdx ++;
				}
			}
			else if (cbp_flag)		//CABAC && transform_size_8x8_flag
			{
				residual_block(pbs, mbindex, i4x4BlkIdx, LumaLevel8x8[i4x4BlkIdx], 64);
			}
			else
			{
				for (i = 0; i < 64; i ++)
				{
					LumaLevel8x8[i8x8][i] = 0;
				}
			}

		}
	}

	if (pCurSPS->chroma_format_idc != 0)
	{
		NumC8x8 = 4 / (pCurSPS->SubWidthC * pCurSPS->SubHeightC);

		cbp_flag = pCurMB->CodedBlockPatternChroma & 0x03;
		for (iCbCr = 0; iCbCr < 2; iCbCr++)		//Cb -- Cr
		{
			if (cbp_flag)
			{
				residual_block(pbs, mbindex, BLOCK_INDEX_CHROMA_DC, ChromaDCLevel[iCbCr], 4 * NumC8x8);
			}
			else
			{
				for (i = 0; i < 4 * NumC8x8; i++)
				{
					ChromaDCLevel[iCbCr][i] = 0;
				}
			}
		}

		cbp_flag = pCurMB->CodedBlockPatternChroma & 0x02;
		for (iCbCr = 0; iCbCr < 2; iCbCr++)
		{
			i4x4BlkIdx = 16 * (iCbCr + 1);
			for (i8x8 = 0; i8x8 < NumC8x8; i8x8 ++)
			{
				for (i4x4 = 0; i4x4 < 4; i4x4 ++)
				{
					if (cbp_flag)		//chroma AC residual present
					{
						residual_block(pbs, mbindex, i4x4BlkIdx, ChromaACLevel[iCbCr][i8x8 * 4 + i4x4], 15);
					}
					else
					{
						for (i = 0; i < 15; i++)
						{
							ChromaACLevel[i8x8 * 4 + iCbCr][i4x4][i] = 0;
						}
					}

					i4x4BlkIdx ++;
				}
			}
		}
	}

	return rtcode;
}

static int h264_video_decode_sub_mb_pred_cavlc(BITS_t* pbs, int slice_type, int mb_type)
{
	int rtcode = 0;

	return rtcode;
}

static int h264_video_decode_sub_mb_pred_cabac(BITS_t* pbs, int slice_type, int mb_type)
{
	int rtcode = 0;

	return rtcode;
}

static int h264_vide_decode_mb_pred_cavlc(BITS_t* pbs, int slice_type, int mbindex)
{
	int		rtcode = 0;
	int		MbPartPredMode;
	int		luma4x4BlkIdx;
	int		luma8x8BlkIdx;
	int		mbPartIdx;
	int		compIdx;
	int		NumMbPart;
	int		mb_type;

	H264_macroblock_layer_t* pCurMB = ph264_macroblock_layer + mbindex;
	H264_macroblock_layer_t* pMBA = NULL;
	H264_macroblock_layer_t* pMBB = NULL;
	int		idxA;
	int		idxB;
	int		mbAddrA;
	int		mbAddrB;
	int		chromaIndex = H264_LUT_chromaindex[pCurSPS->chroma_format_idc];
	int		dcPredModePredictedFlag = 0;
	int		intraMxMPredModeA;
	int		intraMxMPredModeB;
	int		MbPartPredModeA;
	int		MbPartPredModeB;
	int		predIntra4x4PredMode;

	mb_type = pCurMB->mb_type;
	MbPartPredMode = LUT_MbPartPredMode[slice_type][0][mb_type];

	if ((MbPartPredMode == Intra_4x4) ||
		(MbPartPredMode == Intra_8x8) ||
		(MbPartPredMode == Intra_16x16))
	{
		if ((mbindex % pCurSPS->PicWidthInMbs) == 0)
		{
			mbAddrA = -1;
		}
		else
		{
			mbAddrA = mbindex - 1;
		}

		if (mbAddrA < 0)
		{
			dcPredModePredictedFlag = 1;
			MbPartPredModeA = -1;
		}
		else
		{
			pMBA = ph264_macroblock_layer + mbAddrA;
			MbPartPredModeA = LUT_MbPartPredMode[slice_type][0][pMBA->mb_type];
		}

		mbAddrB = mbindex - pCurSPS->PicWidthInMbs;

		if (mbAddrB < 0)
		{
			dcPredModePredictedFlag = 1;
			MbPartPredModeB = -1;
		}
		else
		{
			pMBB = ph264_macroblock_layer + mbAddrB;
			MbPartPredModeB = LUT_MbPartPredMode[slice_type][0][pMBB->mb_type];
		}

		if (MbPartPredMode == Intra_4x4)
		{
			for (luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{
				//根据luma4x4BlkIdx计算mbAddrA & luma4x4BlkIdxIdxA 和 mbAddrB & luma4x4BlkIdxIdxB
				if (mbAddrA >= 0)
				{
					idxA = NEIGHBOUR_leftA[chromaIndex][luma4x4BlkIdx];
					if (idxA < 0)
					{
						idxA += 48;
					}
				}

				if (mbAddrB >= 0)
				{
					idxB = NEIGHBOUR_upperB[chromaIndex][luma4x4BlkIdx];
					if (idxB < 0)
					{
						idxB += 48;
					}
				}

				if ((dcPredModePredictedFlag == 1) || ((MbPartPredModeA != Intra_4x4) && (MbPartPredModeA != Intra_8x8)))
				{
					intraMxMPredModeA = 2;
				}
				else
				{
					intraMxMPredModeA = pMBA->Intra4x4PredMode[idxA];
				}

				if ((dcPredModePredictedFlag == 1) || ((MbPartPredModeB != Intra_4x4) && (MbPartPredModeB != Intra_8x8)))
				{
					intraMxMPredModeB = 2;
				}
				else
				{
					intraMxMPredModeB = pMBB->Intra4x4PredMode[idxB];
				}

				predIntra4x4PredMode = min(intraMxMPredModeA, intraMxMPredModeB);

				pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx] = BITS_get(pbs, 1);

				if (!pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx])
				{
					pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx] = BITS_get(pbs, 3);

					if (pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx] < predIntra4x4PredMode)
					{
						pCurMB->Intra4x4PredMode[luma4x4BlkIdx] = pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx];
					}
					else
					{
						pCurMB->Intra4x4PredMode[luma4x4BlkIdx] = pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx] + 1;
					}
				}
				else
				{
					pCurMB->Intra4x4PredMode[luma4x4BlkIdx] = predIntra4x4PredMode;
				}

#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, "\n prev_intra4x4_pred_mode_flag[%d] = %d\n", luma4x4BlkIdx, pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx]);
					if (!pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx])
					{
						fprintf(m_hDebugFile, " rem_intra4x4_pred_mode[%d] = %d\n", luma4x4BlkIdx, pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx]);
					}

					fprintf(m_hDebugFile, " Intra4x4PredMode[%d] = %d\n", luma4x4BlkIdx, pCurMB->Intra4x4PredMode[luma4x4BlkIdx]);
				}
#endif
			}
		}

		if (MbPartPredMode == Intra_8x8)
		{
			for (luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx ++)
			{
				pCurMB->prev_intra8x8_pred_mode_flag[luma8x8BlkIdx] = BITS_get(pbs, 1);

				if (!pCurMB->prev_intra8x8_pred_mode_flag[luma8x8BlkIdx])
				{
					pCurMB->rem_intra8x8_pred_mode[luma8x8BlkIdx] = BITS_get(pbs, 3);
				}
#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, "\n prev_intra4x4_pred_mode_flag[%d] = %d\n", luma8x8BlkIdx, pCurMB->prev_intra8x8_pred_mode_flag[luma8x8BlkIdx]);
					if (!pCurMB->prev_intra8x8_pred_mode_flag[luma8x8BlkIdx])
					{
						fprintf(m_hDebugFile, " rem_intra8x8_pred_mode[%d] = %d\n", luma8x8BlkIdx, pCurMB->rem_intra8x8_pred_mode[luma8x8BlkIdx]);
					}
				}
#endif
			}
		}

		if (pCurSPS->chroma_format_idc != 0)
		{
			pCurMB->intra_chroma_pred_mode = BITS_ue(pbs);
#if H264_VID_DECODER_DEBUG
			if (m_hDebugFile)
			{
				fprintf(m_hDebugFile, "\n intra_chroma_pred_mode = %d ", pCurMB->intra_chroma_pred_mode);

				switch (pCurMB->intra_chroma_pred_mode)
				{
				case 0:
					fprintf(m_hDebugFile, "[DC]\n");
					break;
				case 1:
					fprintf(m_hDebugFile, "[Horizontal]\n");
					break;
				case 2:
					fprintf(m_hDebugFile, "[Vertical]\n");
					break;
				case 3:
					fprintf(m_hDebugFile, "[Plane]\n");
					break;
				default:
					fprintf(m_hDebugFile, "\n");
					break;
				}
			}
#endif
		}
	}
	else if (MbPartPredMode != Direct)
	{
		NumMbPart = LUT_NumMbPart[slice_type][mb_type];

		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (((pCurPPS->num_ref_idx_l0_active_minus1 > 0) || (h264_slice_data.mb_field_decoding_flag)) &&
				(MbPartPredMode != Pred_L1))
			{
//				h264_macroblock_layer.ref_idx_l0[mbPartIdx] = entropy_coding_mode_flag ? BITS_ae(pbs) : GetBits_te(pbs);
			}
		}

		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (((pCurPPS->num_ref_idx_l1_active_minus1 > 0) || (h264_slice_data.mb_field_decoding_flag)) &&
				(MbPartPredMode != Pred_L0))
			{
//				h264_macroblock_layer.ref_idx_l1[mbPartIdx] = entropy_coding_mode_flag ? BITS_ae(pbs) : GetBits_te(pbs);
			}
		}

		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (MbPartPredMode != Pred_L1)
			{
				for (compIdx = 0; compIdx < 2; compIdx ++)
				{
					pCurMB->mvd_l0[mbPartIdx][0][compIdx] = BITS_se(pbs);
				}
			}
		}
		
		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (MbPartPredMode != Pred_L0)
			{
				for (compIdx = 0; compIdx < 2; compIdx ++)
				{
					pCurMB->mvd_l1[mbPartIdx][0][compIdx] = BITS_se(pbs);
				}
			}
		}
	}

	return rtcode;
}

static int h264_vide_decode_mb_pred_cabac(BITS_t* pbs, int slice_type, int mbindex)
{
	int		rtcode = 0;
	int		MbPartPredMode;
	int		luma4x4BlkIdx;
	int		mbPartIdx;
	int		compIdx;
	int		NumMbPart;
	int		mb_type;

	H264_macroblock_layer_t*	pCurMB = ph264_macroblock_layer + mbindex;

	mb_type = pCurMB->mb_type;
	MbPartPredMode = LUT_MbPartPredMode[slice_type][0][mb_type];

	if ((MbPartPredMode == Intra_4x4) ||
		(MbPartPredMode == Intra_16x16))
	{
		if (MbPartPredMode == Intra_4x4)
		{
			for (luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{
				pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx] = BITS_ae(pbs);

				if (!pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx])
				{
					pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx] = BITS_ae(pbs);
				}
#if H264_VID_DECODER_DEBUG
				if (m_hDebugFile)
				{
					fprintf(m_hDebugFile, "\n prev_intra4x4_pred_mode_flag[%d] = %d\n", luma4x4BlkIdx, pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx]);
					if (!pCurMB->prev_intra4x4_pred_mode_flag[luma4x4BlkIdx])
					{
						fprintf(m_hDebugFile, " rem_intra4x4_pred_mode[%d] = %d\n", luma4x4BlkIdx, pCurMB->rem_intra4x4_pred_mode[luma4x4BlkIdx]);
					}
				}
#endif
			}
		}

		pCurMB->intra_chroma_pred_mode = BITS_ae(pbs);
#if H264_VID_DECODER_DEBUG
		if (m_hDebugFile)
		{
			fprintf(m_hDebugFile, "\n intra_chroma_pred_mode = %d\n", pCurMB->intra_chroma_pred_mode);
		}
#endif
	}
	else if (MbPartPredMode != Direct)
	{
		NumMbPart = LUT_NumMbPart[slice_type][mb_type];

		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (((pCurPPS->num_ref_idx_l0_active_minus1 > 0) || (h264_slice_data.mb_field_decoding_flag)) &&
				(MbPartPredMode != Pred_L1))
			{
//				h264_macroblock_layer.ref_idx_l0[mbPartIdx] = entropy_coding_mode_flag ? BITS_ae(pbs) : GetBits_te(pbs);
			}
		}

		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (((pCurPPS->num_ref_idx_l1_active_minus1 > 0) || (h264_slice_data.mb_field_decoding_flag)) &&
				(MbPartPredMode != Pred_L0))
			{
//				h264_macroblock_layer.ref_idx_l1[mbPartIdx] = entropy_coding_mode_flag ? BITS_ae(pbs) : GetBits_te(pbs);
			}
		}

		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (MbPartPredMode != Pred_L1)
			{
				for (compIdx = 0; compIdx < 2; compIdx ++)
				{
					pCurMB->mvd_l0[mbPartIdx][0][compIdx] = BITS_ae(pbs);
				}
			}
		}
		
		for (mbPartIdx = 0; mbPartIdx < NumMbPart; mbPartIdx++)
		{
			MbPartPredMode = LUT_MbPartPredMode[slice_type][mbPartIdx][mb_type];

			if (MbPartPredMode != Pred_L0)
			{
				for (compIdx = 0; compIdx < 2; compIdx ++)
				{
					pCurMB->mvd_l1[mbPartIdx][0][compIdx] = BITS_ae(pbs);
				}
			}
		}
	}

	return rtcode;
}

static int h264_video_decode_macroblock_layer_cavlc(BITS_t* pbs, int slice_type, int CurrMbAddress)
{
	int		rtcode = 0;
	int		MbPartPredMode;
	int		mb_type;
	int		codeNum;
	int		index;
	int		mbPartIdx;
	int		noSubMbPartSizeLessThan8x8Flag;
	H264_macroblock_layer_t*	pCurMB;

	pCurMB = ph264_macroblock_layer + CurrMbAddress;
	assert(pCurMB != NULL);

	pCurMB->slice_number = h264_slice_header.slice_number;

	pCurMB->mb_type = mb_type = BITS_ue(pbs);

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "\nCurrMbAddress = %d [slice_number = %d]\n", CurrMbAddress, pCurMB->slice_number);
		fprintf(m_hDebugFile, " mb_type = %d\n", pCurMB->mb_type);
	}
#endif

	if (pCurMB->mb_type == H264_I_PCM)
	{
		BITS_align(pbs);

		//FIX ME
//		for (i = 0; i < 256 * ChromaFormatFactor; i++)
//		{
//			h264_macroblock_layer.
//		}
	}
	else
	{
		noSubMbPartSizeLessThan8x8Flag = 1;

		MbPartPredMode = LUT_MbPartPredMode[slice_type][0][mb_type];

		if ((mb_type != H264_I_NxN) &&
			(MbPartPredMode != Intra_16x16) &&
			(LUT_NumMbPart[slice_type][mb_type] == 4))
		{
			h264_video_decode_sub_mb_pred_cavlc(pbs, slice_type, mb_type);

			for (mbPartIdx = 0; mbPartIdx < 4; mbPartIdx ++)
			{
			}
		}
		else
		{
			if (pCurPPS->transform_8x8_mode_flag && (mb_type == H264_I_NxN))
			{
				pCurMB->transform_size_8x8_flag = BITS_get(pbs, 1);
			}
			h264_vide_decode_mb_pred_cavlc(pbs, slice_type, CurrMbAddress);
		}

		if (MbPartPredMode != Intra_16x16)
		{
			codeNum = BITS_ue(pbs);
			index = (MbPartPredMode == Intra_4x4) ? 0 : 1;

			pCurMB->coded_block_pattern = LUT_CBP[index][codeNum];

			pCurMB->CodedBlockPatternLuma = pCurMB->coded_block_pattern & 0x0f;		//equal to % 16
			pCurMB->CodedBlockPatternChroma = pCurMB->coded_block_pattern >> 4;

#if H264_VID_DECODER_DEBUG
			if (m_hDebugFile)
			{
				fprintf(m_hDebugFile, " coded_block_pattern = %d [codeNum = %d]\n", pCurMB->coded_block_pattern, codeNum);
			}
#endif

			if ((pCurMB->CodedBlockPatternLuma > 0) &&
				(pCurPPS->transform_8x8_mode_flag == 1) &&
				(mb_type != H264_I_NxN) &&
				(noSubMbPartSizeLessThan8x8Flag == 1) &&
				((mb_type != H264_B_Direct_16x16) || (pCurSPS->direct_8x8_inference_flag)))
			{
				pCurMB->transform_size_8x8_flag = BITS_get(pbs, 1);
			}
		}
		else
		{
			pCurMB->CodedBlockPatternLuma = LUT_ICBP_luma[mb_type];
			pCurMB->CodedBlockPatternChroma = LUT_ICBP_chroma[mb_type];
		}

#if H264_VID_DECODER_DEBUG
		if (m_hDebugFile)
		{
			fprintf(m_hDebugFile, " CodedBlockPatternLuma = %d, CodedBlockPatternChroma = %d\n", pCurMB->CodedBlockPatternLuma, pCurMB->CodedBlockPatternChroma);
		}
#endif

		pCurMB->mb_qp_delta = 0;
		if ((pCurMB->CodedBlockPatternLuma > 0) || (pCurMB->CodedBlockPatternChroma > 0) || (MbPartPredMode == Intra_16x16))
		{
			pCurMB->mb_qp_delta = BITS_se(pbs);

			if (pCurMB->QPy_prev == -1000000)
			{
				pCurMB->QPy_prev = h264_slice_header.SliceQPy;
			}
			else
			{
				int prevMb = CurrMbAddress - 1;

				assert(prevMb >= 0);
				H264_macroblock_layer_t* pPreMB = ph264_macroblock_layer + prevMb;

				pCurMB->QPy_prev = pPreMB->QPy;
			}

			pCurMB->QPy = ((pCurMB->QPy_prev + pCurMB->mb_qp_delta + 52 + 2 * pCurSPS->QpBdOffsetY) % (52 + pCurSPS->QpBdOffsetY)) - pCurSPS->QpBdOffsetY;
			pCurMB->QPy_ = pCurMB->QPy + pCurSPS->QpBdOffsetY;

#if H264_VID_DECODER_DEBUG
			if (m_hDebugFile)
			{
				fprintf(m_hDebugFile, " mb_qp_delta = %d, QPy = %d, QPy_ = %d\n", pCurMB->mb_qp_delta, pCurMB->QPy, pCurMB->QPy_);
			}
#endif

			h264_video_decode_residual(pbs, slice_type, CurrMbAddress);
		}
	}

	return rtcode;
}

static int h264_video_decode_macroblock_layer_cabac(BITS_t* pbs, int slice_type, int CurrMbAddress)
{
	int		rtcode = -1;
/*
	int		entropy_coding_mode_flag = pCurPPS->entropy_coding_mode_flag;
	int		MbPartPredMode;
	int		mb_type;
	int		codeNum;
	int		index;
	H264_macroblock_layer_t* pCurMB = ph264_macroblock_layer + CurrMbAddress;

	pCurMB->mb_type = mb_type = entropy_coding_mode_flag ? BITS_ae(pbs) : BITS_ue(pbs);

#if H264_VID_DECODER_DEBUG
	if (m_hDebugFile)
	{
		fprintf(m_hDebugFile, "\nCurrMbAddress = %d\n", CurrMbAddress);
		fprintf(m_hDebugFile, " mb_type = %d\n", pCurMB->mb_type);
	}
#endif

	if (pCurMB->mb_type == H264_I_PCM)
	{
		BITS_align(pbs);

//		for (i = 0; i < 256 * ChromaFormatFactor; i++)
//		{
//			h264_macroblock_layer.
//		}
	}
	else
	{
		MbPartPredMode = LUT_MbPartPredMode[slice_type][0][mb_type];

		if ((MbPartPredMode != Intra_4x4) &&
			(MbPartPredMode != Intra_16x16) &&
			(LUT_NumMbPart[slice_type][mb_type] == 4))
		{
			h264_video_decode_sub_mb_pred_cabac(pbs, slice_type, mb_type);
		}
		else
		{
			h264_vide_decode_mb_pred_cabac(pbs, slice_type, CurrMbAddress);
		}

		if (MbPartPredMode != Intra_16x16)
		{
			codeNum = entropy_coding_mode_flag ? BITS_ae(pbs) : BITS_ue(pbs);
			index = (MbPartPredMode == Intra_4x4) ? 0 : 1;

			pCurMB->coded_block_pattern = LUT_CBP[index][codeNum];
		}
		else
		{

		}
#if H264_VID_DECODER_DEBUG
		if (m_hDebugFile)
		{
			fprintf(m_hDebugFile, " coded_block_pattern = %d [codeNum = %d]\n", pCurMB->coded_block_pattern, codeNum);
		}
#endif

		pCurMB->CodedBlockPatternLuma = pCurMB->coded_block_pattern % 16;
		pCurMB->CodedBlockPatternChroma = pCurMB->coded_block_pattern >> 4;
		if ((pCurMB->CodedBlockPatternLuma > 0) || (pCurMB->CodedBlockPatternChroma > 0) || (MbPartPredMode == Intra_16x16))
		{
			pCurMB->mb_qp_delta = entropy_coding_mode_flag ? BITS_ae(pbs) : BITS_ue(pbs);

#if H264_VID_DECODER_DEBUG
			if (m_hDebugFile)
			{
				fprintf(m_hDebugFile, " mb_qp_delta = %d\n", pCurMB->mb_qp_delta);
			}
#endif
			h264_video_decode_residual(pbs, slice_type, CurrMbAddress);
		}
	}
*/
	return rtcode;
}

int h264_video_decode_slice_data(BITS_t* pbs, int nal_unit_type)
{
	int rtcode = 0;
	int CurrMbAddr;
	int moreDataFlag;
	int prevMbSkipped;
	int slice_type;
	int i;
	H264_macroblock_layer_t*	pCurMB;

	slice_type = h264_slice_header.slice_type;

	if (pCurPPS->entropy_coding_mode_flag)			//CABAC
	{
		BITS_align(pbs);
	}

	CurrMbAddr = h264_slice_header.first_mb_in_slice * (1 + h264_slice_header.MbaffFrameFlag);

	if (CurrMbAddr == 0)		//图片的第一个宏块
	{
		h264_video_reorder_decode_frame(slice_type);

		//初始化所有宏块
		memset(ph264_macroblock_layer, 0x00, sizeof(H264_macroblock_layer_t) * h264_decode_info.mb_count);
		for (i = 0; i < h264_decode_info.mb_count; i++)
		{
			ph264_macroblock_layer[i].slice_number = -1;
		}
	}
	
	pCurMB = ph264_macroblock_layer + CurrMbAddr;
	pCurMB->QPy_prev = -1000000;										//first in slice
	
#if H264_VID_DECODER_I
	if ((slice_type != H264_SLICE_I0) &&
		(slice_type != H264_SLICE_I1))
	{
		return -1;
	}
#endif

	moreDataFlag = 1;
	prevMbSkipped = 0;

	do
	{
//		if (CurrMbAddr == 17)
//		{
//			CurrMbAddr = 17;
//		}

		if (pCurPPS->entropy_coding_mode_flag)		//CABAC
		{
			//预测帧
			if ((slice_type != H264_SLICE_I0) &&
				(slice_type != H264_SLICE_I1) &&
				(slice_type != H264_SLICE_SI0) &&
				(slice_type != H264_SLICE_SI1))
			{
				h264_slice_data.mb_skip_flag = BITS_ae(pbs);

				moreDataFlag = !h264_slice_data.mb_skip_flag;
			}

			if (moreDataFlag)
			{
				if (h264_slice_header.MbaffFrameFlag && 
					((CurrMbAddr % 2 == 0) || ((CurrMbAddr % 2 == 1) && prevMbSkipped)))
				{
					h264_slice_data.mb_field_decoding_flag = BITS_ae(pbs);
				}

				rtcode = h264_video_decode_macroblock_layer_cabac(pbs, slice_type, CurrMbAddr);

				if ((slice_type != H264_SLICE_I0) &&
					(slice_type != H264_SLICE_I1) &&
					(slice_type != H264_SLICE_SI0) &&
					(slice_type != H264_SLICE_SI1))
				{
					prevMbSkipped = h264_slice_data.mb_skip_flag;
				}

				if (h264_slice_header.MbaffFrameFlag && (CurrMbAddr % 2 == 0))
				{
					moreDataFlag = 1;
				}
				else
				{
					h264_slice_data.end_of_slice_flag = BITS_ae(pbs);
					moreDataFlag = !h264_slice_data.end_of_slice_flag;
				}
			}
		}
		else										//CAVLC
		{
			//预测帧
			if ((slice_type != H264_SLICE_I0) &&
				(slice_type != H264_SLICE_I1) &&
				(slice_type != H264_SLICE_SI0) &&
				(slice_type != H264_SLICE_SI1))
			{
				h264_slice_data.mb_skip_run = BITS_ue(pbs);
				
				prevMbSkipped = (h264_slice_data.mb_skip_run > 0);
				for (i = 0; i < h264_slice_data.mb_skip_run; i++)
				{
					CurrMbAddr = h264_video_decode_NextMbAddress(CurrMbAddr);
				}

				moreDataFlag = h264_video_more_rbsp_data(pbs);
			}
			
			if (moreDataFlag)
			{
				if (h264_slice_header.MbaffFrameFlag && 
					((CurrMbAddr % 2 == 0) || ((CurrMbAddr % 2 == 1) && prevMbSkipped)))
				{
					h264_slice_data.mb_field_decoding_flag = BITS_get(pbs, 1);
				}

				rtcode = h264_video_decode_macroblock_layer_cavlc(pbs, slice_type, CurrMbAddr);

				moreDataFlag = h264_video_more_rbsp_data(pbs);
			}
		}

		CurrMbAddr = h264_video_decode_NextMbAddress(CurrMbAddr);

//	} while (moreDataFlag && (rtcode >= 0) && (CurrMbAddr < 22));
	} while (moreDataFlag && (rtcode >= 0));

	if (CurrMbAddr >= h264_decode_info.mb_count)		//last macroblock
	{
		h264_video_reorder_display_frame(slice_type);

		//初始化slice_header信息		
		memset(&h264_slice_header, 0x00, sizeof(H264_slice_header_t));
	}

	return rtcode;
}

int	h264_video_decode_slice_layer_without_partitioning(H264_nal_unit_t* pnal_unit)
{
	int				rtcode = 0;
	BITS_t			bs;

	::WaitForSingleObject(ghAccessEvent, INFINITE);

	BITS_map(&bs, pnal_unit->rbsp_byte, pnal_unit->NumBytesInRBSP);

	BITS_skip(&bs, 32);		//flush start prefix code

	rtcode = h264_video_decode_slice_header(&bs, pnal_unit->nal_ref_idc, pnal_unit->nal_unit_type);
	
	if (rtcode >= 0)
	{
		if (gbInitDecoder)
		{
			h264_video_decode_slice_data(&bs, pnal_unit->nal_unit_type);
			//rbsp_slice_trailing_bits()
		}
	}

	h264_slice_header.slice_number ++;

	::SetEvent(ghAccessEvent);

	return rtcode;
}

//int	h264_video_decode_slice_data_partition_a_layer_rbsp(FIFO_t* pbs)
//{
////	uint32_t	slice_id;
//
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
////	h264_video_decode_slice_header(pbs, 0, 2);
////	slice_id = BITS_ue(pbs);
////	h264_video_decode_slice_data(pbs, 2);
////	h264_slice_header.slice_number ++;
//	//rbsp_slice_trailing_bits()
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

int	h264_video_decode_slice_data_partition_b_layer_rbsp(BITS_t* pbs)
{
	uint32_t slice_id;

	::WaitForSingleObject(ghAccessEvent, INFINITE);
	
	slice_id = BITS_ue(pbs);

//	if (redundant_pic_cnt_present_flag)
//	{
//		redundant_pic_cnt = BITS_ue(pbs);
//	}

//	h264_video_decode_slice_data(pbs, 3);
	//rbsp_slice_trailing_bits()

	::SetEvent(ghAccessEvent);

	return 0;
}

int	h264_video_decode_slice_data_partition_c_layer_rbsp(BITS_t* pbs)
{
	uint32_t slice_id;

	::WaitForSingleObject(ghAccessEvent, INFINITE);
	
	slice_id = BITS_ue(pbs);

//	if (redundant_pic_cnt_present_flag)
//	{
//		redundant_pic_cnt = BITS_ue(pbs);
//	}

//	h264_video_decode_slice_data(pbs, 3);
	//rbsp_slice_trailing_bits()

	::SetEvent(ghAccessEvent);

	return 0;
}

//int	h264_video_decode_sei_rbsp(FIFO_t* pbs)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	FIFO_Flush(pbs, 8);
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//int	h264_video_decode_access_unit_delimiter_rbsp(FIFO_t* pbs)
//{
//	uint8_t	primary_pic_type;
//
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	primary_pic_type = FIFO_Get(pbs, 3);
//	//rbsp_trailing_bits()
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//int	h264_video_decode_end_of_seq_rbsp(FIFO_t* pbs)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	FIFO_Flush(pbs, 8);
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//int	h264_video_decode_end_of_stream_rbsp(FIFO_t* pbs)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	FIFO_Flush(pbs, 8);
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//int	h264_video_decode_filler_data_rbsp(FIFO_t* pbs)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	while (FIFO_Peek(pbs, 8) == 0xFF)
//	{
//		FIFO_Flush(pbs, 8);
//	}
//
//	//rbsp_trailing_bits()
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//int	h264_video_decode_seq_parameter_set_extension_rbsp(FIFO_t* pbs)
//{
//	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	FIFO_Flush(pbs, 8);
//
//	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

#if 0

uint8_t* h264_video_output(int* pwidth, int* pheight)
{
	if (gbInitDecoder == 0) return NULL;

	if (pucDisplayFrame[CC_Y] == NULL) return NULL;
	if (pucDisplayFrame[CC_Cb] == NULL) return NULL;
	if (pucDisplayFrame[CC_Cr] == NULL) return NULL;
/*
#if MPEG_VID_DECODER_I
	if (mpeg_picture_header.picture_coding_type == MPEG_P_FRAME) return NULL;
	if (mpeg_picture_header.picture_coding_type == MPEG_B_FRAME) return NULL;
#elif MPEG_VID_DECODER_P
	if (mpeg_picture_header.picture_coding_type == MPEG_I_FRAME) return NULL;
	if (mpeg_picture_header.picture_coding_type == MPEG_B_FRAME) return NULL;
#elif MPEG_VID_DECODER_B
	if (mpeg_picture_header.picture_coding_type == MPEG_I_FRAME) return NULL;
	if (mpeg_picture_header.picture_coding_type == MPEG_P_FRAME) return NULL;
#elif MPEG_VID_DECODER_IP
	if (mpeg_picture_header.picture_coding_type == MPEG_B_FRAME) return NULL;
#endif
*/
	::WaitForSingleObject(ghAccessEvent, INFINITE);

//	if (render_h264_video_buf_rgb888 != NULL)
//	{
//		Video_yuv2rgb(	pucDisplayFrame[CC_Y],
//						pucDisplayFrame[CC_Cb],
//						pucDisplayFrame[CC_Cr],
//						render_h264_video_buf_rgb888,
//						&h264_decode_info);
			
#if H264_VID_DECODER_DEBUG
		if (m_hDebugYUV)
		{
			fwrite(pucDisplayFrame[CC_Y], h264_decode_info.luma_pix_count, sizeof(char), m_hDebugYUV);
			fwrite(pucDisplayFrame[CC_Cb], h264_decode_info.chroma_pix_count, sizeof(char), m_hDebugYUV);
			fwrite(pucDisplayFrame[CC_Cr], h264_decode_info.chroma_pix_count, sizeof(char), m_hDebugYUV);
		}
#endif

//		*pwidth = h264_decode_info.graph_width;
//		*pheight = h264_decode_info.graph_height;
//	}

	::SetEvent(ghAccessEvent);

//	return render_h264_video_buf_rgb888;
	return NULL;
}

#endif

Video_decode_info_t* h264_video_get_decode_info(void)
{
	return &h264_decode_info;
}

