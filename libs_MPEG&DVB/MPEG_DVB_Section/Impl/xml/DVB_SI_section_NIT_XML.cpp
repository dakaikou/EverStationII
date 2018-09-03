#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/DVB_SI_section_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_NIT_DecodeSection_to_XML(uint8_t* section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, network_information_section_t* pNITSection)
{
	S32		 rtcode = SECTION_PARSE_NO_ERROR;
	U32		 N = 0;
	S32		 descriptor_loop_length;
	S32		 stream_loop_length;
	U8*		 pl1temp;
	U8*		 pl2temp;

	U16		 descriptor_tag;
	U8		 descriptor_length;
	U16		 move_length;
	S32		 reserved_count;
	char	 pszTemp[64];

	STREAM_DESCRIPTION_t*	pStream;
	BITS_t	bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("network_information_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= DVB_SI_NIT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_NIT_SECTION_MAX_SIZE))
		{
			//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
			//这是内部处理的策略问题，不是必须这么做的
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				network_information_section_t* pnit_section = (pNITSection != NULL) ? pNITSection : new network_information_section_t;
				memset(pnit_section, 0x00, sizeof(network_information_section_t));

				BITS_map(&bs, section_buf, section_size);

				pnit_section->table_id = BITS_get(&bs, 8);
				assert((pnit_section->table_id == TABLE_ID_NIT_ACTUAL) || (pnit_section->table_id == TABLE_ID_NIT_OTHER));
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pnit_section->table_id, 8, "uimsbf", NULL, &bs);

				pnit_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pnit_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

				if (pnit_section->section_syntax_indicator == 1)		//check point
				{
					pnit_section->reserved_future_use0 = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", pnit_section->reserved_future_use0, 1, "bslbf", NULL, &bs);
					pnit_section->reserved0 = BITS_get(&bs, 2);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pnit_section->reserved0, 2, "bslbf", NULL, &bs);

					pnit_section->section_length = BITS_get(&bs, 12);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", pnit_section->section_length, 12, "uimsbf", NULL, &bs);

					if ((pnit_section->section_length >= (DVB_SI_NIT_SECTION_MIN_SIZE - 3)) && (pnit_section->section_length <= (DVB_SI_NIT_SECTION_MAX_SIZE - 3)))		//chenck point
					{
						pnit_section->network_id = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "network_id", pnit_section->network_id, 16, "uimsbf", NULL, &bs);

						pnit_section->reserved1 = BITS_get(&bs, 2);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pnit_section->reserved1, 2, "bslbf", NULL, &bs);
						pnit_section->version_number = BITS_get(&bs, 5);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", pnit_section->version_number, 5, "uimsbf", NULL, &bs);
						pnit_section->current_next_indicator = BITS_get(&bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", pnit_section->current_next_indicator, 1, "bslbf", NULL, &bs);

						pnit_section->section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", pnit_section->section_number, 8, "uimsbf", NULL, &bs);

						pnit_section->last_section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", pnit_section->last_section_number, 8, "uimsbf", NULL, &bs);

						if (pnit_section->section_number <= pnit_section->last_section_number)			//check point
						{
							pnit_section->reserved_future_use1 = BITS_get(&bs, 4);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", pnit_section->reserved_future_use1, 4, "bslbf", NULL, &bs);

							pnit_section->network_descriptors_length = BITS_get(&bs, 12);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "network_descriptors_length", pnit_section->network_descriptors_length, 12, "uimsbf", NULL, &bs);

							if ((pnit_section->network_descriptors_length >= 0) && (pnit_section->network_descriptors_length <= (pnit_section->section_length - 13)))
							{
								reserved_count = 0;
								if (pnit_section->network_descriptors_length > 0)
								{
									pl1temp = bs.p_cur;							//save the network descriptor buf pointer
									BITS_byteSkip(&bs, pnit_section->network_descriptors_length);
									tinyxml2::XMLElement* pxmlNetworkDescriptorsNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "network_descriptors()", -1, -1, NULL, NULL, &bs);

									descriptor_loop_length = pnit_section->network_descriptors_length;
									while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
									{
										descriptor_tag = pl1temp[0];
										descriptor_length = pl1temp[1];
										move_length = descriptor_length + 2;

										if (move_length <= descriptor_loop_length)
										{
											pnit_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
											pnit_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
											pnit_section->reserved_descriptor[reserved_count].descriptor_buf = pl1temp;
											pnit_section->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

											switch (descriptor_tag)
											{
											case DVB_SI_NETWORK_NAME_DESCRIPTOR:
												DVB_SI_decode_network_name_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlNetworkDescriptorsNode);
												break;
											case DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:
												DVB_SI_decode_multilingual_network_name_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlNetworkDescriptorsNode);
												break;
											default:
												decode_reserved_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlNetworkDescriptorsNode);

												break;
											}

											reserved_count++;

											pl1temp += move_length;
											descriptor_loop_length -= move_length;
										}
										else
										{
											report_descriptor_length_error_to_xml(pl1temp, move_length, descriptor_loop_length, pxmlDoc, pxmlNetworkDescriptorsNode);
											break;
										}
									}
								}
								pnit_section->reserved_count = reserved_count;

								pnit_section->reserved_future_use2 = BITS_get(&bs, 4);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", pnit_section->reserved_future_use2, 4, "bslbf", NULL, &bs);

								pnit_section->transport_stream_loop_length = BITS_get(&bs, 12);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "transport_stream_loop_length", pnit_section->transport_stream_loop_length, 12, "uimsbf", NULL, &bs);

								if ((pnit_section->transport_stream_loop_length >= 0) &&
									(pnit_section->transport_stream_loop_length <= (pnit_section->section_length - 13 - pnit_section->network_descriptors_length)))
								{
									N = 0;
									if (pnit_section->transport_stream_loop_length > 0)
									{
										pl1temp = bs.p_cur;
										BITS_byteSkip(&bs, pnit_section->transport_stream_loop_length);
										tinyxml2::XMLElement* pxmlStreamLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "transport_stream_loop()", -1, -1, NULL, NULL, &bs);

										BITS_t stream_loop_bs;
										BITS_map(&stream_loop_bs, pl1temp, pnit_section->transport_stream_loop_length);

										stream_loop_length = pnit_section->transport_stream_loop_length;
										while ((stream_loop_length >= 6) && (N < MAX_STREAMS_PER_NIT_SECTION))
										{
											unsigned char* old_stream_ptr = stream_loop_bs.p_cur;
											tinyxml2::XMLElement* pxmlStreamNode = pxmlDoc->NewKeyValuePairElement(pxmlStreamLoopNode, "transport_stream()");

											pStream = &(pnit_section->astStream[N]);

											pStream->transport_stream_id = BITS_get(&stream_loop_bs, 16);
											pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "transport_stream_id", pStream->transport_stream_id, 16, "uimsbf", NULL, &stream_loop_bs);

											pStream->original_network_id = BITS_get(&stream_loop_bs, 16);
											pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "original_network_id", pStream->original_network_id, 16, "uimsbf", NULL, &stream_loop_bs);

											pStream->reserved = BITS_get(&stream_loop_bs, 4);
											pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "reserved", pStream->reserved, 4, "bslbf", NULL, &stream_loop_bs);

											pStream->transport_descriptors_length = BITS_get(&stream_loop_bs, 12);
											pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "transport_descriptors_length", pStream->transport_descriptors_length, 12, "uimsbf", NULL, &stream_loop_bs);

											reserved_count = 0;
											if ((pStream->transport_descriptors_length >= 0) && (pStream->transport_descriptors_length <= (stream_loop_length - 6)))
											{
												if (pStream->transport_descriptors_length > 0)
												{
													pl2temp = stream_loop_bs.p_cur;
													BITS_byteSkip(&stream_loop_bs, pStream->transport_descriptors_length);

													tinyxml2::XMLElement* pxmlDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "transport_descriptors()", -1, -1, NULL, NULL, &stream_loop_bs);

													descriptor_loop_length = pStream->transport_descriptors_length;
													while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
													{
														descriptor_tag = pl2temp[0];
														descriptor_length = pl2temp[1];
														move_length = descriptor_length + 2;

														pStream->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
														pStream->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
														pStream->reserved_descriptor[reserved_count].descriptor_buf = pl2temp;
														pStream->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

														switch (descriptor_tag)
														{
														case DVB_SI_SERVICE_LIST_DESCRIPTOR:
															DVB_SI_decode_service_list_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR:
															DVB_SI_decode_satellite_delivery_system_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR:
															DVB_SI_decode_cable_delivery_system_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														case DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
															DVB_SI_decode_private_data_specifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
															break;
														default:
															decode_reserved_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);

															break;
														}

														reserved_count++;

														pl2temp += move_length;
														descriptor_loop_length -= move_length;
													}
												}
											}
											else
											{
												sprintf_s(pszTemp, sizeof(pszTemp), "Error! incorrect transport_descriptors_length = 0x%03X", pStream->transport_descriptors_length);
												pxmlStreamNode->SetAttribute("error", pszTemp);
												break;
											}
											pStream->reserved_count = reserved_count;

											pxmlDoc->UpdateBufMark(pxmlStreamNode, old_stream_ptr, stream_loop_bs.p_cur);
											sprintf_s(pszTemp, sizeof(pszTemp), "TSID=0x%04X, ONetID=0x%04X", pStream->transport_stream_id, pStream->original_network_id);
											pxmlStreamNode->SetAttribute("comment", pszTemp);

											stream_loop_length -= (6 + pStream->transport_descriptors_length);
											N++;
										}

										sprintf_s(pszTemp, sizeof(pszTemp), "共 %d 个流", N);
										pxmlStreamLoopNode->SetAttribute("comment", pszTemp);
									}
									pnit_section->N = N;

									unsigned int CRC_32 = BITS_get(&bs, 32);
									assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
									pnit_section->CRC_32 = CRC_32_code;
									pnit_section->CRC_32_verify = CRC_32_verify;
									tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", pnit_section->CRC_32, 32, "rpchof", NULL, &bs);

									if (CRC_32_verify != CRC_32_code)
									{
										sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", pnit_section->CRC_32_verify);
										pxmlCrcNode->SetAttribute("error", pszTemp);
										pxmlRootNode->SetAttribute("error", pszTemp);
									}
								}
								else
								{
									sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect transport_stream_loop_length = 0x%03X(%d)", pnit_section->transport_stream_loop_length, pnit_section->transport_stream_loop_length);
									pxmlRootNode->SetAttribute("error", pszTemp);

									rtcode = SECTION_PARSE_SYNTAX_ERROR;
								}
							}
							else
							{
								sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect network_descriptors_length = 0x%03X(%d)", pnit_section->network_descriptors_length, pnit_section->network_descriptors_length);
								pxmlRootNode->SetAttribute("error", pszTemp);

								rtcode = SECTION_PARSE_SYNTAX_ERROR;
							}
						}
						else
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_number = 0x%02X, last_section_number = 0x%02X", pnit_section->section_number, pnit_section->last_section_number);
							pxmlRootNode->SetAttribute("error", pszTemp);

							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_length = %d", pnit_section->section_length);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_syntax_indicator = %d", pnit_section->section_syntax_indicator);
					pxmlRootNode->SetAttribute("error", pszTemp);

					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}

				if (pNITSection == NULL)
				{
					//说明pnit_section指针临时分配，函数返回前需要释放
					delete pnit_section;
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
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section buffer parameters error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
