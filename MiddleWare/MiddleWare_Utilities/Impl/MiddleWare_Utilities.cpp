#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "../Include/MiddleWare_Utilities.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_PSI_section.h"
#include "libs_Mpeg&DVB/MPEG_PESPacket/Include/MPEG_stream_id.h"



/****************************************************************************************
	函数：FILE_GetStreamType()
	描述：获取流媒体文件的类型

	输入：流媒体文件名
	输出：空
	返回：流媒体文件类型
	历史：
			20051224	chendelin		修改无法识别VCD流的BUG
****************************************************************************************/
_CDL_EXPORT int FILE_GetStreamType(char* pszFileName)
{
	int		stream_type = STREAM_UNKNOWN;
	U8		data_buf[131072];
	U8*		rd_ptr;
	U8*		end_ptr;
	int		rdcount;
	U16		sync_word;
	U16		read_word;
	U32		sync_dword;
	U32		read_dword;
//	U8		start_code;

	FILE*	fp = NULL;
	fopen_s(&fp, pszFileName, "rb");
	if (fp != NULL)
	{
//		fseek(fp, 65536, SEEK_SET);

		rdcount = fread(data_buf, sizeof(U8), sizeof(data_buf), fp);
		end_ptr = data_buf + rdcount;

		//步骤1: 判断是否是TS流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 612))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x47)
				{
					if ((*(rd_ptr + 188) == 0x47) &&
						(*(rd_ptr + 376) == 0x47))
					{
						stream_type = STREAM_TS;
						break;
					}
					else if ((*(rd_ptr + 204) == 0x47) &&
							 (*(rd_ptr + 408) == 0x47))
					{
						stream_type = STREAM_TS;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		//步骤2: 判断是否是PS流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x01) &&
						(*(rd_ptr + 3) == 0xba))		//pack_start_code
					{
						stream_type = STREAM_PS;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		//步骤3: 判断是否是MPEG VES流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x01) &&
						(*(rd_ptr + 3) == 0xb3))		//sequence_header
					{
						stream_type = STREAM_MPEGVES;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		//步骤4: 判断是否是H264 VES流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 5))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x00) &&
						(*(rd_ptr + 3) == 0x01) &&
						((*(rd_ptr + 4) & 0x9f) == 0x07))		//sequence_header
					{
						stream_type = STREAM_H264VES;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		//步骤5: 判断是否是MPEG AES流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0xff)
				{
					sync_dword = *rd_ptr++;

					if ((*rd_ptr & 0xf0) == 0xf0)
					{
						sync_dword <<= 8;
						sync_dword |= *rd_ptr++;

						sync_dword <<= 8;
						sync_dword |= *rd_ptr++;

						sync_dword <<= 8;
						sync_dword |= *rd_ptr++;

						while (rd_ptr < end_ptr - 4)
						{
							read_dword = *rd_ptr ++;
							read_dword <<= 8;
							read_dword |= *rd_ptr ++;
							read_dword <<= 8;
							read_dword |= *rd_ptr ++;
							read_dword <<= 8;
							read_dword |= *rd_ptr ++;

							if ((read_dword & 0xfffffff0) == (sync_dword & 0xfffffff0))
							{
								if ((sync_dword & 0x00060000) == 0x00000000)
								{
									stream_type = STREAM_AACAES;
								}
								else
								{
									stream_type = STREAM_MPEGAES;
								}
								break;
							}
						}

						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		//步骤6: 判断是否是AC3 AES流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 2))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x0B)
				{
					sync_word = *rd_ptr++;
					sync_word <<= 8;

					if (*rd_ptr == 0x77)
					{
						sync_word |= *rd_ptr;
						rd_ptr ++;

						while (rd_ptr < end_ptr - 2)
						{
							read_word = *rd_ptr ++;
							read_word <<= 8;
							read_word |= *rd_ptr ++;

							if (read_word == sync_word)
							{
								stream_type = STREAM_AC3AES;
								break;
							}
						}

						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		//步骤7: 判断是否是AVS流
		if (stream_type == STREAM_UNKNOWN)
		{
			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x01) &&
						(*(rd_ptr + 3) == 0xb0))		//pack_start_code
					{
						stream_type = STREAM_AVSVES;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
		}

		fclose(fp);
	}

	return stream_type;
}

/****************************************************************************************
	函数：FILE_CheckStreamType()
	描述：检验流媒体文件的类型

	输入：类型
		  流媒体文件名
		  
	输出：空

	返回：1 -- 吻合   0 -- 不一致
	历史：
			20060922	chendelin		
****************************************************************************************/
_CDL_EXPORT int FILE_CheckStreamType(int stream_type, char* pszFileName)
{
	int		rtcode = FALSE;
	U8		data_buf[131072];
	U8*		rd_ptr;
	U8*		end_ptr;
	U8*		next_ptr;
	int		rdcount;
	U16		sync_word;
	U16		read_word;
	U32		sync_dword;
	U32		read_dword;
	S8		strbuf[16];
	S32		bsynced = 0;

	FILE*	fp = NULL;
	fopen_s(&fp, pszFileName, "rb");
	if (fp != NULL)
	{
		rdcount = fread(data_buf, sizeof(U8), sizeof(data_buf), fp);
		end_ptr = data_buf + rdcount;

		switch (stream_type)
		{
		case STREAM_TS:

			rd_ptr = data_buf;
			
			while (rd_ptr < (end_ptr - 612))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x47)
				{
					if ((*(rd_ptr + 188) == 0x47) &&
						(*(rd_ptr + 376) == 0x47))
					{
						rtcode = TRUE;
						break;
					}
					else if ((*(rd_ptr + 204) == 0x47) &&
							 (*(rd_ptr + 408) == 0x47))
					{
						rtcode = TRUE;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
			break;

		case STREAM_PS:

			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x01) &&
						((*(rd_ptr + 3) == 0xba) || (*(rd_ptr + 3) == 0xb3)))		//pack_start_code
					{
						rtcode = TRUE;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}

			break;

		case STREAM_MPEGVES:

			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x01) &&
						(*(rd_ptr + 3) == 0xb3))		//sequence_header
					{
						rtcode = TRUE;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}

			break;

		case STREAM_H264VES:

			rd_ptr = data_buf;
			while (rd_ptr <= (end_ptr - 4))		
			{
				if (rd_ptr[0] == 0x00)
				{
//					if ((rd_ptr[1] == 0x00) &&
//						(rd_ptr[2] == 0x01) &&
//						(((rd_ptr[3] & 0x9f) == 0x07) || ((rd_ptr[3] & 0x9f) == 0x06)))		//sequence_header
					if ((rd_ptr[1] == 0x00) &&
						(rd_ptr[2] == 0x01) &&
						((rd_ptr[3] & 0x9f) == 0x07))					//sequence_header
					{
						rtcode = TRUE;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
			break;

		case STREAM_AACAES:
		case STREAM_MPEGAES:

			bsynced = 0;
			rd_ptr = data_buf;
			while (rd_ptr <= (end_ptr - 4))
			{
				if (rd_ptr[0] == 0xff)
				{
					if ((rd_ptr[1] & 0xf0) == 0xf0)
					{
						sync_dword = rd_ptr[0];
						sync_dword <<= 8;
						sync_dword |= rd_ptr[1];
						sync_dword <<= 8;
						sync_dword |= rd_ptr[2];
						sync_dword <<= 8;
						sync_dword |= rd_ptr[3];

						sync_dword &= 0xfffffdff;

						next_ptr = rd_ptr + 4;

						while (next_ptr <= (end_ptr - 4))
						{
							read_dword = next_ptr[0];
							read_dword <<= 8;
							read_dword |= next_ptr[1];
							read_dword <<= 8;
							read_dword |= next_ptr[2];
							read_dword <<= 8;
							read_dword |= next_ptr[3];

							read_dword &= 0xfffffdff;

							if ((read_dword & 0xfffffff0) == (sync_dword & 0xfffffff0))		//兼顾到AAC
							{
								if ((sync_dword & 0x00060000) == 0x00000000)
								{
									rtcode = TRUE;
								}
								else
								{
									rtcode = TRUE;
								}

								bsynced = 1;
								break;
							}
							else
							{
								next_ptr ++;
							}
						}

						if (bsynced)
						{
							break;
						}
						else
						{
							rd_ptr ++;
						}
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
			break;

		case STREAM_AC3AES:

			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 2))		//根据MPEG2系统层标准留3个204包长的预留
			{
				if (*rd_ptr == 0x0B)
				{
					sync_word = *rd_ptr++;
					sync_word <<= 8;

					if (*rd_ptr == 0x77)
					{
						sync_word |= *rd_ptr;
						rd_ptr ++;

						while (rd_ptr < end_ptr - 2)
						{
							read_word = *rd_ptr ++;
							read_word <<= 8;
							read_word |= *rd_ptr ++;

							if (read_word == sync_word)
							{
								rtcode = TRUE;
								break;
							}
						}

						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}
			break;

		case STREAM_AVSVES:

			rd_ptr = data_buf;
			while (rd_ptr < (end_ptr - 4))
			{
				if (*rd_ptr == 0x00)
				{
					if ((*(rd_ptr + 1) == 0x00) &&
						(*(rd_ptr + 2) == 0x01) &&
						(*(rd_ptr + 3) == 0xb0))		//pack_start_code
					{
						rtcode = TRUE;
						break;
					}
					else
					{
						rd_ptr ++;
					}
				}
				else
				{
					rd_ptr ++;
				}
			}

			break;

		case STREAM_WAVEAES:

			rd_ptr = data_buf;
			memset(strbuf, 0x00, sizeof(strbuf));
			memcpy(strbuf, rd_ptr, 4);
			if (strcmp(strbuf, "RIFF") == 0)
			{
				rd_ptr += 8;
				memset(strbuf, 0x00, sizeof(strbuf));
				memcpy(strbuf, rd_ptr, 4);

				if (strcmp(strbuf, "WAVE") == 0)
				{
					rtcode = TRUE;
				}
			}

			break;

		default:
			break;
		}

		fclose(fp);
	}

	return rtcode;
}





