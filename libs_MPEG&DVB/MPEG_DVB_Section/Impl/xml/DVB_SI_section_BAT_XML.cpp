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
int DVB_SI_BAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, bouquet_association_section_t* pBATSection)
{
	S32		 rtcode = SECTION_PARSE_NO_ERROR;
	S32		 N = 0;
	S32		 stream_loop_length;
	S32		 descriptor_loop_length;
	U8*		 pl1temp;
	U8*		 pl2temp;

	uint16_t descriptor_tag;
	int		 descriptor_length;

	S32		 move_length;
	S32		 reserved_count;
	char	 pszTemp[64];
	BITS_t   bs;

	STREAM_DESCRIPTION_t*	pStream;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("bouquet_association_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= DVB_SI_BAT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_BAT_SECTION_MAX_SIZE))
		{
			//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
			//这是内部处理的策略问题，不是必须这么做的
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				int table_id = section_buf[0];

				if (table_id == TABLE_ID_BAT)
				{
					bouquet_association_section_t* pbat_section = (pBATSection != NULL) ? pBATSection : new bouquet_association_section_t;
					memset(pbat_section, 0x00, sizeof(bouquet_association_section_t));

					BITS_map(&bs, section_buf, section_size);

					pbat_section->table_id = BITS_get(&bs, 8);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pbat_section->table_id, 8, "uimsbf", NULL, &bs);

					pbat_section->section_syntax_indicator = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pbat_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

					if (pbat_section->section_syntax_indicator == 1)
					{
						pbat_section->reserved_future_use0 = BITS_get(&bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use0, 1, "bslbf", NULL, &bs);
						pbat_section->reserved0 = BITS_get(&bs, 2);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pbat_section->reserved0, 2, "bslbf", NULL, &bs);

						pbat_section->section_length = BITS_get(&bs, 12);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", pbat_section->section_length, 12, "uimsbf", NULL, &bs);

						if ((pbat_section->section_length >= DVB_SI_BAT_SECTION_MIN_SIZE - 3) && (pbat_section->section_length <= DVB_SI_BAT_SECTION_MAX_SIZE - 3))
						{
							pbat_section->bouquet_id = BITS_get(&bs, 16);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "bouquet_id", pbat_section->bouquet_id, 16, "uimsbf", NULL, &bs);

							pbat_section->reserved1 = BITS_get(&bs, 2);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pbat_section->reserved1, 2, "bslbf", NULL, &bs);
							pbat_section->version_number = BITS_get(&bs, 5);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", pbat_section->version_number, 5, "uimsbf", NULL, &bs);
							pbat_section->current_next_indicator = BITS_get(&bs, 1);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", pbat_section->current_next_indicator, 1, "bslbf", NULL, &bs);

							pbat_section->section_number = BITS_get(&bs, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", pbat_section->section_number, 8, "uimsbf", NULL, &bs);

							pbat_section->last_section_number = BITS_get(&bs, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", pbat_section->last_section_number, 8, "uimsbf", NULL, &bs);

							if (pbat_section->section_number <= pbat_section->last_section_number)			//特征点
							{
								pbat_section->reserved_future_use1 = BITS_get(&bs, 4);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use1, 4, "bslbf", NULL, &bs);

								pbat_section->bouquet_descriptors_length = BITS_get(&bs, 12);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "bouquet_descriptors_length", pbat_section->bouquet_descriptors_length, 12, "uimsbf", NULL, &bs);

								reserved_count = 0;
								if (pbat_section->bouquet_descriptors_length > 0)
								{
									pl1temp = bs.p_cur;
									BITS_byteSkip(&bs, pbat_section->bouquet_descriptors_length);
									tinyxml2::XMLElement* pxmlBouquetDescriptorsNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "业务群描述符循环()", -1, -1, NULL, NULL, &bs);

									descriptor_loop_length = pbat_section->bouquet_descriptors_length;
									while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
									{
										descriptor_tag = pl1temp[0];
										descriptor_length = pl1temp[1];
										move_length = descriptor_length + 2;

										pbat_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
										pbat_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
										pbat_section->reserved_descriptor[reserved_count].descriptor_buf = pl1temp;
										pbat_section->reserved_descriptor[reserved_count].descriptor_size = move_length;

										switch (descriptor_tag)
										{
										case DVB_SI_BOUQUET_NAME_DESCRIPTOR:
											DVB_SI_decode_bouquet_name_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlBouquetDescriptorsNode);
											break;
										case DVB_SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR:
											DVB_SI_decode_multilingual_bouquet_name_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlBouquetDescriptorsNode);
											break;
										default:
											decode_reserved_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlBouquetDescriptorsNode);
											break;
										}

										reserved_count++;
										pl1temp += move_length;
										descriptor_loop_length -= move_length;
									}
								}
								pbat_section->reserved_count = reserved_count;

								pbat_section->reserved_future_use2 = BITS_get(&bs, 4);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", pbat_section->reserved_future_use2, 4, "bslbf", NULL, &bs);

								pbat_section->transport_stream_loop_length = BITS_get(&bs, 12);
								pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "transport_stream_loop_length", pbat_section->transport_stream_loop_length, 12, "uimsbf", NULL, &bs);

								N = 0;
								if (pbat_section->transport_stream_loop_length > 0)
								{
									pl1temp = bs.p_cur;
									BITS_byteSkip(&bs, pbat_section->transport_stream_loop_length);
									tinyxml2::XMLElement* pxmlStreamLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "流循环()", -1, -1, NULL, NULL, &bs);

									BITS_t stream_loop_bs;
									BITS_map(&stream_loop_bs, pl1temp, pbat_section->transport_stream_loop_length);

									stream_loop_length = pbat_section->transport_stream_loop_length;
									while ((stream_loop_length >= 6) && (N < MAX_STREAMS_PER_BAT_SECTION))
									{
										unsigned char* old_stream_ptr = stream_loop_bs.p_cur;
										tinyxml2::XMLElement* pxmlStreamNode = pxmlDoc->NewKeyValuePairElement(pxmlStreamLoopNode, "流");

										pStream = pbat_section->astStream + N;

										pStream->transport_stream_id = BITS_get(&stream_loop_bs, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "transport_stream_id", pStream->transport_stream_id, 16, "uimsbf", NULL, &stream_loop_bs);

										pStream->original_network_id = BITS_get(&stream_loop_bs, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "original_network_id", pStream->original_network_id, 16, "uimsbf", NULL, &stream_loop_bs);

										pStream->reserved = BITS_get(&stream_loop_bs, 4);
										pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "reserved", pStream->reserved, 4, "bslbf", NULL, &stream_loop_bs);

										pStream->transport_descriptors_length = BITS_get(&stream_loop_bs, 12);
										pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "transport_descriptors_length", pStream->transport_descriptors_length, 12, "uimsbf", NULL, &stream_loop_bs);

										reserved_count = 0;
										if (pStream->transport_descriptors_length > 0)
										{
											pl2temp = stream_loop_bs.p_cur;
											BITS_byteSkip(&stream_loop_bs, pStream->transport_descriptors_length);

											tinyxml2::XMLElement* pxmlDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlStreamNode, "流描述符循环()", -1, -1, NULL, NULL, &stream_loop_bs);

											descriptor_loop_length = pStream->transport_descriptors_length;
											while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
											{
												descriptor_tag = pl2temp[0];
												descriptor_length = pl2temp[1];
												move_length = descriptor_length + 2;

												pStream->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
												pStream->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
												pStream->reserved_descriptor[reserved_count].descriptor_buf = pl2temp;
												pStream->reserved_descriptor[reserved_count].descriptor_size = move_length;

												switch (descriptor_tag)
												{
												case DVB_SI_SERVICE_LIST_DESCRIPTOR:
													DVB_SI_decode_service_list_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
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
								pbat_section->N = N;

								unsigned int CRC_32 = BITS_get(&bs, 32);
								assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
								pbat_section->CRC_32 = CRC_32_code;
								pbat_section->CRC_32_verify = CRC_32_verify;
								tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", pbat_section->CRC_32, 32, "rpchof", NULL, &bs);

								if (CRC_32_verify != CRC_32_code)
								{
									sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", pbat_section->CRC_32_verify);
									pxmlCrcNode->SetAttribute("error", pszTemp);
									pxmlRootNode->SetAttribute("error", pszTemp);

									rtcode = SECTION_PARSE_CRC_ERROR;
								}
							}
							else
							{
								sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_number = 0x%02X, last_section_number = 0x%02X", pbat_section->section_number, pbat_section->last_section_number);
								pxmlRootNode->SetAttribute("error", pszTemp);

								rtcode = SECTION_PARSE_SYNTAX_ERROR;
							}
						}
						else
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_length = %d", pbat_section->section_length);
							pxmlRootNode->SetAttribute("error", pszTemp);

							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_syntax_indicator = %d", pbat_section->section_syntax_indicator);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}

					if (pBATSection == NULL)
					{
						//说明pbat_section指针临时分配，函数返回前需要释放
						delete pbat_section;
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
			pxmlRootNode->SetAttribute("error", "section buffer parameters error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

