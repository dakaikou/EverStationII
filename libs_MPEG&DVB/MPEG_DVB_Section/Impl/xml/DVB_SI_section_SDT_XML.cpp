#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/DVB_SI_section_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_SDT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, service_description_section_t* pSDTSection)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	S32		 N = 0;
	S32		 service_loop_length;
	S32		 descriptor_loop_length;
	U8*		 pl1temp;
	U8*		 pl2temp;
	U16		 descriptor_tag;
	U8		 descriptor_length;
	S32		 move_length;
	S32		 reserved_count;
	char	 pszTemp[256];
	service_descriptor_t service_descriptor;

	BITS_t   bs;

	SERVICE_DESCRIPTION_t*	pServiceDescription;

	if (pxmlDoc != NULL)
	{ 
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("service_description_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		//SDT表的最小长度为15字节
		if ((section_buf != NULL) && (section_size >= DVB_SI_SDT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_SDT_SECTION_MAX_SIZE))
		{
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				uint8_t table_id = section_buf[0];

				if ((table_id == TABLE_ID_SDT_ACTUAL) ||
					(table_id == TABLE_ID_SDT_OTHER))
				{
					service_description_section_t* psdt_section = (pSDTSection != NULL) ? pSDTSection : new service_description_section_t;
					memset(psdt_section, 0x00, sizeof(service_description_section_t));

					BITS_map(&bs, section_buf, section_size);

					psdt_section->table_id = BITS_get(&bs, 8);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", psdt_section->table_id, 8, "uimsbf", NULL, &bs);

					psdt_section->section_syntax_indicator = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", psdt_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

					if (psdt_section->section_syntax_indicator == 1)
					{
						psdt_section->reserved_future_use0 = BITS_get(&bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", psdt_section->reserved_future_use0, 1, "bslbf", NULL, &bs);
						psdt_section->reserved0 = BITS_get(&bs, 2);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", psdt_section->reserved0, 2, "bslbf", NULL, &bs);

						psdt_section->section_length = BITS_get(&bs, 12);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", psdt_section->section_length, 12, "uimsbf", NULL, &bs);

						if ((psdt_section->section_length >= (DVB_SI_SDT_SECTION_MIN_SIZE - 3)) && (psdt_section->section_length <= (DVB_SI_SDT_SECTION_MAX_SIZE - 3)))
						{
							//actual_length = psdt_section->section_length + 3;

							psdt_section->transport_stream_id = BITS_get(&bs, 16);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "transport_stream_id", psdt_section->transport_stream_id, 16, "uimsbf", NULL, &bs);

							psdt_section->reserved1 = BITS_get(&bs, 2);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", psdt_section->reserved1, 2, "bslbf", NULL, &bs);
							psdt_section->version_number = BITS_get(&bs, 5);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", psdt_section->version_number, 5, "uimsbf", NULL, &bs);
							psdt_section->current_next_indicator = BITS_get(&bs, 1);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", psdt_section->current_next_indicator, 1, "bslbf", NULL, &bs);

							psdt_section->section_number = BITS_get(&bs, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", psdt_section->section_number, 8, "uimsbf", NULL, &bs);

							psdt_section->last_section_number = BITS_get(&bs, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", psdt_section->last_section_number, 8, "uimsbf", NULL, &bs);

							if (psdt_section->section_number <= psdt_section->last_section_number)			//特征点
							{
								psdt_section->original_network_id = BITS_get(&bs, 16);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "original_network_id", psdt_section->original_network_id, 16, "uimsbf", NULL, &bs);

								psdt_section->reserved_future_use1 = BITS_get(&bs, 8);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", psdt_section->reserved_future_use1, 8, "bslbf", NULL, &bs);

								service_loop_length = psdt_section->section_length - 12;

								N = 0;
								if (service_loop_length > 0)
								{
									pl1temp = bs.p_cur;
									BITS_byteSkip(&bs, service_loop_length);
									tinyxml2::XMLElement* pxmlServiceLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "业务循环()", -1, -1, NULL, NULL, &bs);

									BITS_t service_loop_bs;
									BITS_map(&service_loop_bs, pl1temp, service_loop_length);
									while ((service_loop_length >= 5) && (N < MAX_SERVICES_PER_STREAM))
									{
										unsigned char* old_service_ptr = service_loop_bs.p_cur;
										tinyxml2::XMLElement* pxmlServiceNode = pxmlDoc->NewKeyValuePairElement(pxmlServiceLoopNode, "业务");

										pServiceDescription = psdt_section->astServiceDescription + N;

										pServiceDescription->service_id = BITS_get(&service_loop_bs, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "service_id", pServiceDescription->service_id, 16, "uimsbf", NULL, &service_loop_bs);

										pServiceDescription->reserved_future_use = BITS_get(&service_loop_bs, 6);
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "reserved_future_use", pServiceDescription->reserved_future_use, 6, "bslbf", NULL, &service_loop_bs);

										pServiceDescription->EIT_schedule_flag = BITS_get(&service_loop_bs, 1);
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "EIT_schedule_flag", pServiceDescription->EIT_schedule_flag, 1, "bslbf", NULL, &service_loop_bs);
										pServiceDescription->EIT_present_following_flag = BITS_get(&service_loop_bs, 1);
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "EIT_present_following_flag", pServiceDescription->EIT_present_following_flag, 1, "bslbf", NULL, &service_loop_bs);

										pServiceDescription->running_status = BITS_get(&service_loop_bs, 3);
										DVB_SI_NumericCoding2Text_RunningStatus(pServiceDescription->running_status, pszTemp, sizeof(pszTemp));
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "running_status", pServiceDescription->running_status, 3, "uimsbf", pszTemp, &service_loop_bs);
										pServiceDescription->free_CA_mode = BITS_get(&service_loop_bs, 1);
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "free_CA_mode", pServiceDescription->free_CA_mode, 1, "bslbf", NULL, &service_loop_bs);

										pServiceDescription->descriptors_loop_length = BITS_get(&service_loop_bs, 12);
										pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "descriptors_loop_length", pServiceDescription->descriptors_loop_length, 12, "uimsbf", NULL, &service_loop_bs);

										if ((pServiceDescription->descriptors_loop_length >= 0) && (pServiceDescription->descriptors_loop_length <= (service_loop_length - 5)))
										{
											reserved_count = 0;
											if (pServiceDescription->descriptors_loop_length > 0)
											{
												pl2temp = service_loop_bs.p_cur;
												BITS_byteSkip(&service_loop_bs, pServiceDescription->descriptors_loop_length);

												uint8_t* pend = service_loop_bs.p_cur;

												tinyxml2::XMLElement* pxmlDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlServiceNode, "业务描述符循环()", -1, -1, NULL, NULL, &service_loop_bs);

												descriptor_loop_length = pServiceDescription->descriptors_loop_length;
												while ((pl2temp <= (pend - 2)) && (descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
												{
													descriptor_tag = pl2temp[0];
													descriptor_length = pl2temp[1];
													move_length = descriptor_length + 2;

													if (move_length <= descriptor_loop_length)
													{
														pServiceDescription->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
														pServiceDescription->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
														pServiceDescription->reserved_descriptor[reserved_count].descriptor_buf = pl2temp;
														pServiceDescription->reserved_descriptor[reserved_count].descriptor_size = move_length;

														reserved_count++;

														switch (descriptor_tag)
														{
														case DVB_SI_BOUQUET_NAME_DESCRIPTOR:
															DVB_SI_decode_bouquet_name_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_SERVICE_DESCRIPTOR:
															DVB_SI_decode_service_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode, &service_descriptor);
															break;
														case DVB_SI_CA_IDENTIFIER_DESCRIPTOR:
															DVB_SI_decode_CA_identifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR:
															DVB_SI_decode_multilingual_service_name_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
															DVB_SI_decode_private_data_specifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_DATA_BROADCAST_DESCRIPTOR:
															DVB_SI_decode_data_broadcast_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														default:
															decode_reserved_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);

															break;
														}
													}
													else
													{
														report_descriptor_length_error_to_xml(pl2temp, move_length, descriptor_loop_length, pxmlDoc, pxmlDescriptorLoopNode);
														pxmlDescriptorLoopNode->SetAttribute("error", "Error !");
														pxmlRootNode->SetAttribute("error", "Error !");

														break;
													}

													pl2temp += move_length;
													descriptor_loop_length -= move_length;
												}
											}
											pServiceDescription->reserved_count = reserved_count;
										}
										else
										{
											BITS_byteSkip(&service_loop_bs, (service_loop_length - 5));
											sprintf_s(pszTemp, sizeof(pszTemp), "Error! incorrect descriptors_loop_length = %d, could not exceed remain service_loop_length %d", pServiceDescription->descriptors_loop_length, (service_loop_length - 5));
											pxmlServiceNode->SetAttribute("error", pszTemp);
											pxmlServiceLoopNode->SetAttribute("error", "Error !");
											pxmlRootNode->SetAttribute("error", "Error !");

											rtcode = SECTION_PARSE_SYNTAX_ERROR;
										}

										pxmlDoc->UpdateBufMark(pxmlServiceNode, old_service_ptr, service_loop_bs.p_cur);
										sprintf_s(pszTemp, sizeof(pszTemp), "ID=0x%04X, %s", pServiceDescription->service_id, service_descriptor.trimmed_service_name);
										pxmlServiceNode->SetAttribute("comment", pszTemp);

										//service_loop_length -= (pServiceDescription->descriptors_loop_length + 5);
										service_loop_length -= (int)(service_loop_bs.p_cur - old_service_ptr);
										N++;
									}
								}
								psdt_section->N = N;

								unsigned int CRC_32 = BITS_get(&bs, 32);
								assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
								psdt_section->CRC_32 = CRC_32_code;
								psdt_section->CRC_32_verify = CRC_32_verify;
								tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", psdt_section->CRC_32, 32, "rpchof", NULL, &bs);

								if (CRC_32_verify != CRC_32_code)
								{
									sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", psdt_section->CRC_32_verify);
									pxmlCrcNode->SetAttribute("error", pszTemp);
									pxmlRootNode->SetAttribute("error", pszTemp);

									rtcode = SECTION_PARSE_CRC_ERROR;
								}
							}
							else
							{
								sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_number = 0x%02X, last_section_number = 0x%02X", psdt_section->section_number, psdt_section->last_section_number);
								pxmlRootNode->SetAttribute("error", pszTemp);

								rtcode = SECTION_PARSE_SYNTAX_ERROR;
							}
						}
						else
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_length = %d", psdt_section->section_length);
							pxmlRootNode->SetAttribute("error", pszTemp);

							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_syntax_indicator = %d", psdt_section->section_syntax_indicator);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}

					if (pSDTSection == NULL)
					{
						//说明psdt_section指针临时分配，函数返回前需要释放
						delete psdt_section;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect table_id = 0x%02X", table_id);
					pxmlRootNode->SetAttribute("error", pszTemp);
					rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
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
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section buffer parameter error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}

