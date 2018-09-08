#include "stdafx.h"

#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_OCDCs.h"

#include "libs_MPEG&DVB/MPEG_TSPacket/Include/Mpeg2_TS_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_TSPacket\Include\Mpeg2_TS_Utilities.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\Include\MPEG_DVB_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"

#include "..\Common\define.h"

#include "../Magic_TS/TSMagicView.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_Decimate_Dsmcc.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <direct.h>
#include <math.h>
#include <assert.h>
#include <Shlwapi.h>


/******************文件、目录操作********************/
//检查输入的目录是否存在，如不存在，则创建新目录   
//void ts_dsmcc_oc_download(char* rootPath, pthread_params_t pThreadParams)
//{
//#if OPEN_DSMCC_DOWNLOAD
//	int							rtcode;
//	DC_DOWNLOAD_INFO_t*			pDCDownloadInfo;
//	OC_DOWNLOAD_INFO_t*			pOCDownloadInfo;
//	int							dc_index;
//	int							download_index;
//	size_t						read_size;
//	size_t						write_size;
////	int							rewind_size;
//
//	unsigned char*				module_buf;
//	unsigned char				temp_buf[32];
////	int							module_length;
//	unsigned char*				message_buf;
//	int							message_length;
//	int							remain_length;
//	char						magic[5];
//	unsigned char				objectKey_length;
//	char						objectKind_data[4];
//	char						pszDCFileName[MAX_PATH];
//	char						pszOCFileName[MAX_PATH];
//	char						pszVirtualFile[MAX_PATH];
//	FILE*						fp_virtual = NULL;		
//	int							virtual_size;
//
//	S8							pszDebug[MAX_TXT_CHARS];
//
//	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
//	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();
//	CDB_OCDCs* pDB_OCDCs = pTSMagicView->GetOCDCsDBase();
//
//	FileMessage_t			FileMessage;
//
//	sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析开始");
//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//	sprintf_s(pszVirtualFile, sizeof(pszVirtualFile), "%s\\oc_virtual.dat", rootPath);
//
//	fopen_s(&fp_virtual, pszVirtualFile, "wb");
//	if (fp_virtual != NULL)
//	{
//		//创建一个更大的虚拟文件
//		virtual_size = 0;
//		int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
//		for (dc_index = 0; dc_index < dc_download_count; dc_index ++)
//		{
//			pDCDownloadInfo = pDB_OCDCs->GetDCDownloadInfo(dc_index);
//
//			//首先应该解析module，根据解析得到的objectKey_data进行匹配
//			if (pDCDownloadInfo != NULL)
//			{
//				if (pDCDownloadInfo->oc_flag == 1)
//				{
//					sprintf_s(pszDCFileName, sizeof(pszDCFileName), "%s\\%s", rootPath, pDCDownloadInfo->moduleName);
//					fopen_s(&(pDCDownloadInfo->fp), pszDCFileName, "rb");
//					if (pDCDownloadInfo->fp != NULL)
//					{
//						fseek(pDCDownloadInfo->fp, 0, SEEK_SET);
//
//						module_buf = (unsigned char*)malloc(pDCDownloadInfo->moduleSize);
//						if (module_buf != NULL)
//						{
//							read_size = fread(module_buf, sizeof(unsigned char), pDCDownloadInfo->moduleSize, pDCDownloadInfo->fp);
//							assert(read_size == pDCDownloadInfo->moduleSize);
//
//							write_size = fwrite(module_buf, sizeof(char), read_size, fp_virtual);
//							assert(write_size == read_size);
//							virtual_size += (uint32_t)write_size;
//
//							free(module_buf);
//							module_buf = NULL;
//						}
//
//						fclose(pDCDownloadInfo->fp);
//					}
//
//					pDCDownloadInfo->fp = NULL;
//				}
//			}
//		}
//
//		fclose(fp_virtual);
//	}
//	else
//	{
//		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析——创建虚拟文件失败-%s", pszVirtualFile);
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//	}
//
//	fopen_s(&fp_virtual, pszVirtualFile, "rb");
//	if (fp_virtual != NULL)
//	{
//		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析——正在处理%s", pszVirtualFile);
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
////		rewind_size = 0;
//		remain_length = virtual_size;
//		fseek(fp_virtual, 0, SEEK_SET);
//		while (remain_length >= 32)				//一个BIOP::Message至少有30个字节，此处严格判断，可以防止程序崩溃
//		{
////			fseek(fp_virtual, rewind_size, SEEK_SET);
//			read_size = fread(temp_buf, sizeof(unsigned char), 32, fp_virtual);
//
//			//解析DDB
//			memcpy(magic, temp_buf, 4);
//			magic[4] = '\0';
//
//			if (strcmp(magic, "BIOP") == 0)		//说明是OC且能同步上
//			{
//				message_length = 12 + ((temp_buf[8] << 24) | (temp_buf[9] << 16) | (temp_buf[10] << 8) | temp_buf[11]);
//				message_buf = (unsigned char*)malloc(message_length);
//				if (message_buf != NULL)
//				{
//					memcpy(message_buf, temp_buf, 32);
//
//					read_size = fread(message_buf + 32, sizeof(unsigned char), message_length - 32, fp_virtual);
//					assert(read_size == (message_length - 32));
//
//					//究竟是哪个消息，还需要进一步判断
//					objectKey_length = message_buf[12];
//					memcpy(objectKind_data, message_buf + 17 + objectKey_length, 4);
//
//					if (strcmp(objectKind_data, "fil") == 0)
//					{
//						memset(&FileMessage, 0x00, sizeof(FileMessage_t));
//						rtcode = MPEG2_DSMCC_DecodeFileMessage(message_buf, message_length, &FileMessage);
//						if (rtcode == NO_ERROR)
//						{
//							int oc_download_count = pDB_OCDCs->GetOCDownloadCount();
//							for (download_index = 0; download_index < oc_download_count; download_index ++)
//							{
//								pOCDownloadInfo = pDB_OCDCs->GetOCDownloadInfo(download_index);
//
//								if ((pOCDownloadInfo->downloadId == pOCDownloadInfo->downloadId) &&
//									(pOCDownloadInfo->moduleId == pOCDownloadInfo->moduleId) &&
//									(pOCDownloadInfo->objectKey_data == FileMessage.objectKey_data))
//								{
//									sprintf_s(pszOCFileName, sizeof(pszOCFileName), "%s\\%s", rootPath, pOCDownloadInfo->pszFileName);
//									
//									fopen_s(&(pOCDownloadInfo->fp), pszOCFileName, "wb");
//									if (pOCDownloadInfo->fp != NULL)
//									{
//										write_size = fwrite(FileMessage.content_data_byte, sizeof(unsigned char), FileMessage.content_length, pOCDownloadInfo->fp);
//										assert(write_size == FileMessage.content_length);
//
//										fclose(pOCDownloadInfo->fp);
//									}
//
//									break;
//								}
//							}
//						}
//						else
//						{
//							assert(0);
//						}
//					}
//
//					remain_length -= message_length;
//
//					free(message_buf);
//				}
//				else
//				{
//					sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析——内存分配不足-需要%d字节", message_length);
//					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//
//					break;
//				}
//			}
//			else
//			{
//				assert(0);
//
//				//说明丢失同步
//				sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析——消息头不等于 BIOP");
//				::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//				::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//
//				break;
//			}
//		}
//
//		fclose(fp_virtual);
//	}
//	else
//	{
//		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析——读虚拟文件失败-%s", pszVirtualFile);
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//	}
//
//	::DeleteFile(pszVirtualFile);
//
//	sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: OC解析结束。");
//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//#endif
//}

//S32						g_enable_dsmcc_download = 0;
//S32							g_splicer_count = 0;
//CSectionSplicer*			g_astSectionSplicer = NULL;

//S32 ts_dsmcc_download_init(void)
//{
//	g_splicer_count = 0;
//	g_astSectionSplicer = NULL;
////	g_enable_dsmcc_download = 0;
//
//	return 0;
//}

//S32 ts_dsmcc_download_fini(void)
//{
//	S32 filter_index = 0;
//
//	//释放sectionsplicer资源
//	for (filter_index = 0; filter_index < g_splicer_count; filter_index++)
//	{
//		g_astSectionSplicer[filter_index].Reset();
//	}
//	free(g_astSectionSplicer);
//
//	g_splicer_count = 0;
////	g_enable_dsmcc_download = 0;
//
//	return 0;
//}

//S32 ts_dsmcc_download_section_filter(CDB_OCDCs* pDB_OCDCs, U16 nPID, U8* section_buf, S32 section_length)
//{
//	S32							rtcode = -1;
//	S32							download_index;
//
//	DC_DOWNLOAD_INFO_t*			pDCDownloadInfo;
////	OC_DOWNLOAD_INFO_t*			pOCDownloadInfo;
//	U16							table_id_extension;
//	dsmcc_ddm_section_t			DSMCC_section;
//	S32							block_index;
//	S32							collect_over;
//	S32							write_offset;
//
////	if ((PSISI_REPORT.dc_download_count > 0) && (PSISI_REPORT.dc_download_over == 0))
//	if (pDB_OCDCs->m_dsmcc_download_enable == 1)
//	{
//		assert(section_buf != NULL);
//
//		table_id_extension = (section_buf[3] << 8) | section_buf[4];
//
//		pDCDownloadInfo = NULL;
//		int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
//		for (download_index = 0; download_index < dc_download_count; download_index ++)
//		{
//			DC_DOWNLOAD_INFO_t* pTemp = pDB_OCDCs->GetDCDownloadInfo(download_index);
//			if ((pTemp->PID == nPID) &&
//				(pTemp->moduleId == table_id_extension))
//			{
//				pDCDownloadInfo = pTemp;
//				break;
//			}
//		}
//
//		if (pDCDownloadInfo != NULL)
//		{
//			if (pDCDownloadInfo->fp != NULL)			//若文件创建成功了再提取数据
//			{
//				if (pDCDownloadInfo->collect_over == 0)
//				{
//					rtcode = MPEG2_DSMCC_DDM_DecodeSection(section_buf, section_length, &DSMCC_section);
//					if (rtcode == SECTION_PARSE_NO_ERROR)
//					{
//						if ((DSMCC_section.dsmccDownloadDataHeader.downloadId == pDCDownloadInfo->downloadId) &&
//							(DSMCC_section.DownloadDataBlock.moduleId == pDCDownloadInfo->moduleId))
//						{
//							block_index = DSMCC_section.DownloadDataBlock.blockNumber;
//
//							if (pDCDownloadInfo->blockFlags[block_index] == 0)
//							{
//								write_offset = block_index * pDCDownloadInfo->blockSize;
//								if (pDCDownloadInfo->fp != NULL)
//								{
//									fseek(pDCDownloadInfo->fp, write_offset, SEEK_SET);
//									fwrite(DSMCC_section.DownloadDataBlock.blockDataByte, 1, DSMCC_section.DownloadDataBlock.N, pDCDownloadInfo->fp);
//
//									pDCDownloadInfo->downloadSize += DSMCC_section.DownloadDataBlock.N;
//	//													download_file_size += DSMCC_section.u.DownloadDataBlock.N;
//	//													download_ratio = (S32)(100 * download_file_size / total_file_size);
//	//													if (download_ratio > old_ratio)
//	//													{
//	//														old_ratio = download_ratio;
//	//														::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_RATIO, (WPARAM)NULL, (LPARAM)download_ratio);
//	//													}
//
//									pDCDownloadInfo->blockFlags[block_index] = 1;
//
//									//检查一下是否收集齐了
//									collect_over = 1;
//									for (block_index = 0; block_index < pDCDownloadInfo->blockCount; block_index ++)
//									{
//										if (pDCDownloadInfo->blockFlags[block_index] == 0)
//										{
//											collect_over = 0;
//											break;
//										}
//									}
//
//									pDCDownloadInfo->collect_over = collect_over;
//
//									if (pDCDownloadInfo->collect_over == 1)
//									{
//										fclose(pDCDownloadInfo->fp);
//										pDCDownloadInfo->fp = NULL;
//
//	//														sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: PID=0x%04X %s 收集齐!", pDCDownloadInfo->PID, pDCDownloadInfo->moduleName);
//	//														::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
//
//										collect_over = 1;
//										for (download_index = 0; download_index < dc_download_count; download_index ++)
//										{
//											DC_DOWNLOAD_INFO_t* pTemp = pDB_OCDCs->GetDCDownloadInfo(download_index);
//											if (pTemp->collect_over == 0)
//											{
//												collect_over = 0;
//												break;
//											}
//										}
//
//										pDB_OCDCs->m_dc_download_over = collect_over;
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//
//	return rtcode;
//}

//S32 ts_dsmcc_download_packet_filter(CDB_OCDCs* pDB_OCDCs, transport_packet_t* ptransport_packet)
//{
//	S32							download_index;
////	S8							pszDebug[MAX_TXT_CHARS];
//	CSectionSplicer*			pSectionSplicer;
//	S32							filter_index;
//	S32							rtcode;
//	U8*							section_buf;
//	S32							section_length;
//
//	U8							table_id;
////	U16							table_id_extension;
//
////	DSMCC_section_t				DSMCC_section;
////	S32							block_index;
////	S32							write_left_count;
////	S32							write_size;
//
//	DC_DOWNLOAD_INFO_t*			pDCDownloadInfo;
//
//	if (ptransport_packet != NULL)
//	{
//		int dc_download_count = pDB_OCDCs->GetDCDownloadCount();
//		//确定本TS包是否需要过滤
//		for (download_index = 0; download_index < dc_download_count; download_index ++)
//		{
//			pDCDownloadInfo = pDB_OCDCs->GetDCDownloadInfo(download_index);
//			if (pDCDownloadInfo->fp != NULL)
//			{
//				if (pDCDownloadInfo->collect_over == 0)
//				{
//					if (ptransport_packet->PID == pDCDownloadInfo->PID)
//					{
//						break;
//					}
//				}
//			}
//		}
//
//		if (download_index < dc_download_count)
//		{
//			//本TS包携带需要下载的DSMCC数据
//
//			//首先寻找该PID曾分配的过滤器
//			pSectionSplicer = NULL;
//			for (filter_index = 0; filter_index < g_splicer_count; filter_index++)
//			{
//				if (g_astSectionSplicer[filter_index].m_usPID == ptransport_packet->PID)
//				{
//					pSectionSplicer = g_astSectionSplicer + filter_index;
//					break;
//				}
//			}
//
//			if (pSectionSplicer == NULL)
//			{
//				//说明没有找到
//
//				//从已有的FILTER中重新分配一个
//				for (filter_index = 0; filter_index < g_splicer_count; filter_index++)
//				{
//					if (g_astSectionSplicer[filter_index].m_usPID == 0xFFFF)
//					{
//						pSectionSplicer = g_astSectionSplicer + filter_index;
//						break;
//					}
//				}
//			}
//
//			if (pSectionSplicer == NULL)
//			{
//				//说明FILTER都已经占用满了
//				g_splicer_count ++;
//				g_astSectionSplicer = (CSectionSplicer*)realloc(g_astSectionSplicer, g_splicer_count * sizeof(CSectionSplicer));
//				if (g_astSectionSplicer != NULL)
//				{
//					pSectionSplicer = g_astSectionSplicer + (g_splicer_count - 1);
//
//					pSectionSplicer->Init();
//				}
//			}
//
//			if (pSectionSplicer != NULL)
//			{
//				rtcode = pSectionSplicer->WriteTSPacket(ptransport_packet);
//				if (rtcode == NO_ERROR)
//				{
//					section_buf = pSectionSplicer->m_pucSectionBuf;
//					section_length = pSectionSplicer->m_nSectionLength;
//
//					//判断是否为需要的section
//					table_id = section_buf[0];
////						table_id_extension = (section_buf[3] << 8) | section_buf[4];
//
//					if (table_id == TABLE_ID_DSMCC_DDM)
//					{
//						ts_dsmcc_download_section_filter(pDB_OCDCs, ptransport_packet->PID, section_buf, section_length);
//
//					}
//
//					pSectionSplicer->Reset();
//				}
//			}
//		}
//	}
//
//	return 0;
//}

//DSMCC OC/DC下载线程
void ts_dsmcc_download_loop(pthread_params_t pThreadParams)
{
#if OPEN_DSMCC_DOWNLOAD

	S8	  pszDebug[MAX_TXT_CHARS];
	S8	  exeDrive[3];

	S32	  old_ratio = 0;
	S32	  rtcode;

	CDB_PsiSiTables*			pDB_PsiSiTables = NULL;
	CDB_OCDCs*					pDB_OCDCs = NULL;

	DOWNLOAD_INFO_t				stDownloadInfo;
	S32							download_index;
	S32							i;
	S64							total_file_size;							//所有需要下载文件的大小，K字节
	S64							download_file_size;
	//S32							not_complete_file_count;
	S32							download_ratio;

	U32							old_tickcount;
	U32							new_tickcount;
	S32							diff_tickcount;

	//S32							write_left_count;
	//S32							write_size;
	//S32							block_index;

	if (pThreadParams != NULL)
	{
		pThreadParams->dsmcc_download_thread_running = 1;

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_THREAD, 1, NULL);

		old_tickcount = ::GetTickCount();

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 开始");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		pDB_PsiSiTables = pThreadParams->pDB_PsiSiTables;
		pDB_OCDCs = pThreadParams->pDB_OCDCs;

		//计算下载总的数据量
		total_file_size = 0;
		download_file_size = 0;
		//int download_file_count = pDB_OCDCs->GetDownloadCount();
		//for (download_index = 0; download_index < dc_download_count; download_index ++)
		//{
		//	pDCDownloadInfo = pDB_OCDCs->GetDCDownloadInfo(download_index);
		//	total_file_size += pDCDownloadInfo->moduleSize;
		//}

		//sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 需要下载%d K字节数据，共%d个文件", (int)(total_file_size / 1024), dc_download_count);
		//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
		//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

		char	pszExeFile[MAX_PATH];
		char	pszAppTempPath[MAX_PATH];
		char	pszDownloadRootPath[MAX_PATH];

		//获取应用程序所在盘符的根路径
		GetModuleFileName(NULL, pszExeFile, MAX_PATH);
		exeDrive[0] = pszExeFile[0];
		exeDrive[1] = pszExeFile[1];
		exeDrive[2] = '\0';
		sprintf_s(pszAppTempPath, sizeof(pszAppTempPath), "%s\\~EverStationII", exeDrive);
		sprintf_s(pszDownloadRootPath, sizeof(pszDownloadRootPath), "%s\\download_OCDC", pszAppTempPath);

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 创建下载根目录 %s", pszDownloadRootPath);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

		rtcode = ::CreateDirectory(pszAppTempPath, NULL);
		rtcode = ::CreateDirectory(pszDownloadRootPath, NULL);
		//if (rtcode == 0)
		//{
		//	sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 创建下载根目录%s失败", pszDownloadRootPath);
		//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
		//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
		//}

		int download_count = pDB_OCDCs->GetDownloadCount();
		for (download_index = 0; download_index < download_count; download_index++)
		{
			pDB_OCDCs->GetDownloadInfo(download_index, &stDownloadInfo);

			sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 下载TS_PID=0x%04X携带的OC/DC数据", stDownloadInfo.usCandidatePID);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

			pDB_PsiSiTables->DownloadDsmccTree(stDownloadInfo.usCandidatePID, pszDownloadRootPath);

			//判断应用程序是否中断了下载
			if (pThreadParams->dsmcc_download_thread_running == 0)
			{
				break;
			}
		}

		pDB_OCDCs->Reset();

		pThreadParams->dsmcc_download_thread_running = 0;
		pThreadParams->dsmcc_download_thread_stopped = 1;

		new_tickcount = ::GetTickCount();
		diff_tickcount = new_tickcount - old_tickcount;

		sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 结束(耗时%dms)", diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DOWNLOAD_THREAD, 0, NULL);
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "DSMCC下载: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
#endif
}

U32 TSMagic_dsmcc_download_thread(LPVOID lpParam)
{
	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;
	ts_dsmcc_download_loop(pThreadParams);

	return 0;
}


