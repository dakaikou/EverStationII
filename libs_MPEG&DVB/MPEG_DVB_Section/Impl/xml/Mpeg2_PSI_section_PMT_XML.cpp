#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/Mpeg2_PSI_section.h"
#include "../../Include/Mpeg2_PSI_Descriptor.h"
#include "../../Include/Mpeg2_PSI_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/Mpeg2_StreamType.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/Mpeg2_PSI_section_XML.h"
#include "../../Include/xml/Mpeg2_PSI_Descriptor_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

////////////////////////////////////////////
int MPEG2_PSI_PMT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, TS_program_map_section_t* pPMTSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;
	U8*	 pl1temp;
	U8*	 pl2temp;
	S32	 N;
	S32  descriptor_loop_length;
	S32  es_loop_length;
	U16	 descriptor_tag;
	U8	 descriptor_length;
	U16	 move_length;
	S32	 CA_count = 0;
	S32  language_count = 0;
	S32	 descriptor_count = 0;
	char pszTemp[64];
	char pszComment[128];

	BITS_t bs;

	ES_DESCRIPTION_t*	pESMap;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", section_size);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("TS_program_map_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		pxmlRootNode->SetAttribute("comment", pszTemp);

		if ((section_buf != NULL) && (section_size >= MPEG2_PSI_PMT_SECTION_MIN_SIZE) && (section_size <= MPEG2_PSI_PMT_SECTION_MAX_SIZE))
		{
			//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
			//这是内部处理的策略问题，不是必须这么做的
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				TS_program_map_section_t* ppmt_section = (pPMTSection != NULL) ? pPMTSection : new TS_program_map_section_t;
				memset(ppmt_section, 0x00, sizeof(TS_program_map_section_t));

				BITS_map(&bs, section_buf, section_size);

				ppmt_section->table_id = BITS_get(&bs, 8);
				assert(ppmt_section->table_id == TABLE_ID_PMT);			//故意抓虫子
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", ppmt_section->table_id, 8, "uimsbf", NULL, &bs);

				ppmt_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", ppmt_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

				if (ppmt_section->section_syntax_indicator == 1)
				{
					ppmt_section->reserved_future_use = BITS_get(&bs, 1);
					assert(ppmt_section->reserved_future_use == 0);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "'0'", ppmt_section->reserved_future_use, 1, "bslbf", NULL, &bs);
					ppmt_section->reserved0 = BITS_get(&bs, 2);
					assert(ppmt_section->reserved0 == 0b11);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ppmt_section->reserved0, 2, "bslbf", NULL, &bs);

					ppmt_section->section_length = BITS_get(&bs, 12);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", ppmt_section->section_length, 12, "uimsbf", NULL, &bs);

					if ((ppmt_section->section_length >= (MPEG2_PSI_PMT_SECTION_MIN_SIZE - 3)) && (ppmt_section->section_length <= (MPEG2_PSI_PMT_SECTION_MAX_SIZE - 3)))
					{
						ppmt_section->program_number = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "program_number", ppmt_section->program_number, 16, "uimsbf", NULL, &bs);

						ppmt_section->reserved1 = BITS_get(&bs, 2);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ppmt_section->reserved1, 2, "bslbf", NULL, &bs);
						ppmt_section->version_number = BITS_get(&bs, 5);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", ppmt_section->version_number, 5, "uimsbf", NULL, &bs);
						ppmt_section->current_next_indicator = BITS_get(&bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", ppmt_section->current_next_indicator, 1, "bslbf", NULL, &bs);

						ppmt_section->section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", ppmt_section->section_number, 8, "uimsbf", NULL, &bs);

						ppmt_section->last_section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", ppmt_section->last_section_number, 8, "uimsbf", NULL, &bs);

						if (ppmt_section->section_number <= ppmt_section->last_section_number)			//PMT语法特征点
						{
							ppmt_section->reserved2 = BITS_get(&bs, 3);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ppmt_section->reserved2, 3, "bslbf", NULL, &bs);

							ppmt_section->PCR_PID = BITS_get(&bs, 13);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "PCR_PID", ppmt_section->PCR_PID, 13, "uimsbf", NULL, &bs);

							ppmt_section->reserved3 = BITS_get(&bs, 4);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ppmt_section->reserved3, 4, "bslbf", NULL, &bs);

							ppmt_section->program_info_length = BITS_get(&bs, 12);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "program_info_length", ppmt_section->program_info_length, 12, "uimsbf", NULL, &bs);

							if (ppmt_section->program_info_length <= (ppmt_section->section_length - 13))	//PMT syntax check point
							{
								descriptor_count = 0;
								if (ppmt_section->program_info_length > 0)
								{
									pl1temp = bs.p_cur;
									BITS_byteSkip(&bs, ppmt_section->program_info_length);
									tinyxml2::XMLElement* pxmlProgramDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "节目描述符循环()", -1, -1, NULL, NULL, &bs);

									descriptor_loop_length = ppmt_section->program_info_length;
									while ((descriptor_loop_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
									{
										descriptor_tag = pl1temp[0];
										descriptor_length = pl1temp[1];
										move_length = descriptor_length + 2;

										//截成一段一段的描述符
										ppmt_section->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
										ppmt_section->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
										ppmt_section->reserved_descriptor[descriptor_count].descriptor_buf = pl1temp;
										ppmt_section->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

										switch (descriptor_tag)
										{
										case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
											MPEG2_PSI_decode_registration_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);
											break;
										case MPEG2_PSI_CA_DESCRIPTOR:
											MPEG2_PSI_decode_CA_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);
											break;
										case MPEG2_PSI_SYSTEM_CLOCK_DESCRIPTOR:
											MPEG2_PSI_decode_system_clock_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);
											break;
										case MPEG2_PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR:
											MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);
											break;
										case MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR:
											MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);
											break;
										case MPEG2_PSI_SMOOTHING_BUFFER_DESCRIPTOR:
											MPEG2_PSI_decode_smoothing_buffer_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);
											break;
										default:
											decode_reserved_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlProgramDescriptorLoopNode);

											break;
										}

										descriptor_count++;

										pl1temp += move_length;
										descriptor_loop_length -= move_length;
									}
								}
								ppmt_section->reserved_count = descriptor_count;

								es_loop_length = ppmt_section->section_length - ppmt_section->program_info_length - 13;
								N = 0;
								if (es_loop_length > 0)
								{
									uint8_t* es_loop_start_ptr = bs.p_cur;
									BITS_byteSkip(&bs, es_loop_length);
									tinyxml2::XMLElement* pxmlESLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "ES流循环()", -1, -1, NULL, NULL, &bs);

									BITS_t es_loop_bs;
									BITS_map(&es_loop_bs, es_loop_start_ptr, es_loop_length);

									while ((es_loop_length >= 5) && (N < MAX_ESS_PER_PMT_SECTION))			//为什么是16？ chendelin  2018/5/19
									{
										pESMap = ppmt_section->astESMap + N;

										pESMap->stream_subtype = STREAM_SUBTYPE_UNKNOWN;		//额外增加的类型识别  chendelin  20110423

										uint8_t* es_start_ptr = es_loop_bs.p_cur;
										tinyxml2::XMLElement* pxmlESNode = pxmlDoc->NewKeyValuePairElement(pxmlESLoopNode, "ES流()");

										pESMap->stream_type = BITS_get(&es_loop_bs, 8);
										MPEG2_PSI_NumericCoding2Text_StreamType(pESMap->stream_type, STREAM_SUBTYPE_UNKNOWN, pszTemp, sizeof(pszTemp));
										pxmlDoc->NewKeyValuePairElement(pxmlESNode, "stream_type", pESMap->stream_type, 8, "uimsbf", pszTemp, &es_loop_bs);

										pESMap->reserved0 = BITS_get(&es_loop_bs, 3);
										pxmlDoc->NewKeyValuePairElement(pxmlESNode, "reserved", pESMap->reserved0, 3, "bslbf", NULL, &es_loop_bs);

										pESMap->elementary_PID = BITS_get(&es_loop_bs, 13);
										pxmlDoc->NewKeyValuePairElement(pxmlESNode, "elementary_PID", pESMap->elementary_PID, 13, "uimsbf", NULL, &es_loop_bs);

										pESMap->reserved1 = BITS_get(&es_loop_bs, 4);
										pxmlDoc->NewKeyValuePairElement(pxmlESNode, "reserved", pESMap->reserved1, 4, "bslbf", NULL, &es_loop_bs);

										pESMap->ES_info_length = BITS_get(&es_loop_bs, 12);
										pxmlDoc->NewKeyValuePairElement(pxmlESNode, "ES_info_length", pESMap->ES_info_length, 12, "uimsbf", NULL, &es_loop_bs);

										if ((pESMap->ES_info_length >= 0) && (pESMap->ES_info_length <= (es_loop_length - 5)))		//syntax check point
										{
											descriptor_count = 0;
											if (pESMap->ES_info_length > 0)
											{
												pl2temp = es_loop_bs.p_cur;
												BITS_byteSkip(&es_loop_bs, pESMap->ES_info_length);

												tinyxml2::XMLElement* pxmlESDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlESNode, "ES描述符循环()", -1, -1, NULL, NULL, &es_loop_bs);

												descriptor_loop_length = pESMap->ES_info_length;
												while ((descriptor_loop_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
												{
													descriptor_tag = pl2temp[0];
													descriptor_length = pl2temp[1];
													move_length = descriptor_length + 2;

													if (move_length <= descriptor_loop_length)
													{
														pESMap->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
														pESMap->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
														pESMap->reserved_descriptor[descriptor_count].descriptor_buf = pl2temp;
														pESMap->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

														descriptor_count++;

														registration_descriptor_t registration_descriptor;
														switch (descriptor_tag)
														{
														case MPEG2_PSI_VIDEO_STREAM_DESCRIPTOR:
															MPEG2_PSI_decode_video_stream_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case MPEG2_PSI_AUDIO_STREAM_DESCRIPTOR:
															MPEG2_PSI_decode_audio_stream_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
															MPEG2_PSI_decode_registration_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode, &registration_descriptor);
															break;
														case MPEG2_PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR:
															MPEG2_PSI_decode_data_stream_alignment_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case MPEG2_PSI_ISO_639_LANGUAGE_DESCRIPTOR:
															MPEG2_PSI_decode_ISO_639_language_descriptor(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR:
															MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case MPEG2_DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR:
															MPEG2_DSMCC_decode_carousel_identifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case MPEG2_DSMCC_ASSOCIATION_TAG_DESCRIPTOR: 
															MPEG2_DSMCC_decode_association_tag_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case DVB_SI_STREAM_IDENTIFIER_DESCRIPTOR:
															DVB_SI_decode_stream_identifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case DVB_SI_TELETEXT_DESCRIPTOR:
															DVB_SI_decode_teletext_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case DVB_SI_SUBTITLING_DESCRIPTOR:
															DVB_SI_decode_subtitling_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case DVB_SI_DATA_BROADCAST_ID_DESCRIPTOR:
															DVB_SI_decode_data_broadcast_id_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														case DVB_SI_AC3_DESCRIPTOR:
															DVB_SI_decode_ac3_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														default:
															decode_reserved_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESDescriptorLoopNode);
															break;
														}

														if (pESMap->stream_type == 0x05)
														{
															//根据descriptor_tag进一步识别ES流的类型
															if (pESMap->stream_subtype == STREAM_SUBTYPE_UNKNOWN)
															{
																switch (descriptor_tag)
																{
																case DVB_SI_APPLICATION_SIGNALLING_DESCRIPTOR:
																	pESMap->stream_subtype = STREAM_SUBTYPE_AIT;
																	break;
																default:
																	break;
																}
															}
														}
														else if (pESMap->stream_type == 0x06)
														{
															//根据descriptor_tag进一步识别ES流的类型
															if (pESMap->stream_subtype == STREAM_SUBTYPE_UNKNOWN)
															{
																switch (descriptor_tag)
																{
																case MPEG2_PSI_REGISTRATION_DESCRIPTOR:

																	if (strcmp(registration_descriptor.format_identifier_char, "DRA1") == 0)
																	{
																		pESMap->stream_subtype = STREAM_SUBTYPE_DRA;
																	}

																	break;
																case DVB_SI_TELETEXT_DESCRIPTOR:
																	pESMap->stream_subtype = STREAM_SUBTYPE_TELETEXT;
																	break;
																case DVB_SI_SUBTITLING_DESCRIPTOR:
																	pESMap->stream_subtype = STREAM_SUBTYPE_SUBTITLE;
																	break;
																case DVB_SI_AC3_DESCRIPTOR:
																	pESMap->stream_subtype = STREAM_SUBTYPE_AC3;
																	break;
																default:
																	break;
																}
															}
														}
														else if (pESMap->stream_type == 0x81)				//added by chendelin		2018.4.28
														{
															//流类型标识为“用户私有”
															//根据descriptor_tag进一步识别ES流的类型
															if (pESMap->stream_subtype == STREAM_SUBTYPE_UNKNOWN)
															{
																switch (descriptor_tag)
																{
																case MPEG2_PSI_REGISTRATION_DESCRIPTOR:

																	if (strcmp(registration_descriptor.format_identifier_char, "AC-3") == 0)
																	{
																		pESMap->stream_subtype = STREAM_SUBTYPE_AC3;
																	}

																	break;
																case DVB_SI_AC3_DESCRIPTOR:
																case DVB_SI_AC3_AUDIO_DESCRIPTOR:
																	pESMap->stream_subtype = STREAM_SUBTYPE_AC3;
																	break;
																default:
																	break;
																}
															}
														}
													}
													else
													{
														report_descriptor_length_error_to_xml(pl2temp, move_length, descriptor_loop_length, pxmlDoc, pxmlESDescriptorLoopNode);
														pxmlESDescriptorLoopNode->SetAttribute("error", "Error !");
														pxmlESNode->SetAttribute("error", "Error !");
														pxmlESLoopNode->SetAttribute("error", "Error !");
														pxmlRootNode->SetAttribute("error", "Error !");
														break;
													}

													pl2temp += move_length;
													descriptor_loop_length -= move_length;
												}
											}
											pESMap->reserved_count = descriptor_count;
										}
										else
										{
											BITS_byteSkip(&es_loop_bs, (es_loop_length - 5));
											sprintf_s(pszTemp, sizeof(pszTemp), "Error! incorrect descriptors_loop_length = %d, could not exceed remain service_loop_length %d", pESMap->ES_info_length, (es_loop_length - 5));
											pxmlRootNode->SetAttribute("error", pszTemp);
											pxmlESNode->SetAttribute("error", pszTemp);

											rtcode = SECTION_PARSE_SYNTAX_ERROR;
										}

										MPEG2_PSI_NumericCoding2Text_StreamType(pESMap->stream_type, pESMap->stream_subtype, pszTemp, sizeof(pszTemp));
										sprintf_s(pszComment, sizeof(pszComment), "ES_PID=0x%04X, stream_type=0x%02X-- %s", pESMap->elementary_PID, pESMap->stream_type, pszTemp);
										pxmlESNode->SetAttribute("comment", pszComment);
										pxmlDoc->UpdateBufMark(pxmlESNode, es_start_ptr, es_loop_bs.p_cur);
										es_loop_length -= (pESMap->ES_info_length + 5);

										N++;
									}
								}
								ppmt_section->N = N;

								unsigned int CRC_32 = BITS_get(&bs, 32);
								assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
								ppmt_section->CRC_32 = CRC_32_code;
								ppmt_section->CRC_32_verify = CRC_32_verify;
								tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", ppmt_section->CRC_32, 32, "rpchof", NULL, &bs);

								if (CRC_32_verify != CRC_32_code)
								{
									sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", ppmt_section->CRC_32_verify);
									pxmlCrcNode->SetAttribute("error", pszTemp);
									pxmlRootNode->SetAttribute("error", pszTemp);
								}
							}
							else
							{
								sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect program_info_length = 0x%03X", ppmt_section->program_info_length);
								pxmlRootNode->SetAttribute("error", pszTemp);

								rtcode = SECTION_PARSE_SYNTAX_ERROR;
							}
						}
						else
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! section_number = 0x%02X, last_section_number = 0x%02X", ppmt_section->section_number, ppmt_section->last_section_number);
							pxmlRootNode->SetAttribute("error", pszTemp);

							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_length = %d", ppmt_section->section_length);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_syntax_indicator = %d", ppmt_section->section_syntax_indicator);
					pxmlRootNode->SetAttribute("error", pszTemp);

					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}

				if (pPMTSection == NULL)
				{
					//说明ppmt_section指针临时分配，函数返回前需要释放
					delete ppmt_section;
				}
			}
			//else
			//{
			//	pxmlDoc->NewTitleElement(pxmlRoot, "section buffer CRC error!");
			//	rtcode = SECTION_PARSE_CRC_ERROR;
			//}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "section buffer parameters error! section_length = %d", section_size);
			pxmlRootNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

