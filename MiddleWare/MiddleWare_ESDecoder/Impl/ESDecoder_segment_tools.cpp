#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../Include/ESDecoder_segment_tools.h"
#include "../Include/ESDecoder_ErrorCode.h"

//返回第一个同步字的偏移量
int	ES_get_next_segment(uint8_t* es_buf, int es_length, ES_segment_t* psegment)
{
	int offset = -1;

	if ((es_buf != NULL) && (psegment != NULL))
	{
		unsigned int	sync_word;
		int		next_start_code = -1;

		unsigned int	next_bits;

		unsigned char*	start_rdptr;
		unsigned char*	next_rdptr;
		int				remain_length;

		psegment->nal_length = -1;
		psegment->nal_buf = NULL;

		start_rdptr = es_buf;
		remain_length = es_length;

		//while (remain_length >= psegment->min_length)
		while (remain_length >= psegment->match_bytes)
		{
			sync_word = start_rdptr[0];
			for (int i = 1; i < psegment->match_bytes; i++)
			{
				sync_word <<= 8;
				sync_word |= start_rdptr[i];
			}

			if ((sync_word & psegment->match_mask) == psegment->sync_word)
			{
				//DRA音频帧的长度在头部有指示
				if (strcmp(psegment->pszFourCC, "DRA") == 0)
				{
					int nFrmHeaderType = (start_rdptr[2] & 0x80) >> 7;
					int nNumWord = 0;
					if (nFrmHeaderType == 0)
					{
						nNumWord = start_rdptr[2] & 0x7f;
						nNumWord <<= 3;
						nNumWord |= ((start_rdptr[3] & 0xe0) >> 5);
					}
					else
					{
						nNumWord = start_rdptr[2] & 0x7f;
						nNumWord <<= 6;
						nNumWord |= ((start_rdptr[3] & 0xFC) >> 2);
					}

					psegment->min_length = nNumWord * 4;
				}

				next_rdptr = start_rdptr + psegment->min_length;

				int check_length = remain_length - psegment->min_length;

				while (check_length >= psegment->match_bytes)
				{
					next_bits = next_rdptr[0];
					for (int i = 1; i < psegment->match_bytes; i++)
					{
						next_bits <<= 8;
						next_bits |= next_rdptr[i];
					}

					if ((next_bits & psegment->match_mask) == psegment->sync_word)			//结束
					{
						psegment->nal_length = (int)(next_rdptr - start_rdptr);
						break;
					}
					else
					{
						check_length--;
						next_rdptr++;
					}
				}

				psegment->nal_buf = start_rdptr;
				if (psegment->nal_length == -1)
				{
					psegment->nal_length = remain_length;
				}

				psegment->segment_start_ptr = start_rdptr;
				psegment->segment_length = psegment->nal_length;

				//H264 prefix maybe 00 00 00 01 xx, or 00 00 01 xx
				if (strcmp(psegment->pszFourCC, "H264") == 0)
				{
					if (start_rdptr > es_buf)
					{
						uint8_t* prefix_ptr = start_rdptr - 1;
						if (*prefix_ptr == 0)
						{
							psegment->segment_start_ptr = prefix_ptr;
							psegment->segment_length++;
						}
					}

					psegment->nal_buf += 3;
					psegment->nal_length -= 3;

					psegment->prefix_length = (psegment->segment_length - psegment->nal_length);
				}

				offset = (int)(psegment->segment_start_ptr - es_buf);				//offset

				break;
			}
			else
			{
				remain_length--;
				start_rdptr++;
			}
		}
	}

	return offset;
}
