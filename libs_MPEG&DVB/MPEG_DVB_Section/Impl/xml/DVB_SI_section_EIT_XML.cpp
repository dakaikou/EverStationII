#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/DVB_SI_section_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_EIT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, event_information_section_t* pEITSection)
{
	S32		 rtcode = SECTION_PARSE_NO_ERROR;
	S32		 N = 0;
	uint8_t* pl1temp;
	uint8_t* pl2temp;
	char	pszTemp[64];
	char	pszStartTime[64];
	char	pszDuration[64];
	char	pszComment[128];
	BITS_t  bs;

	U16		 descriptor_tag;
	U8		 descriptor_length;
	S32		 move_length;
	S32		 reserved_count;

	EVENT_DESCRIPTION_t*	pEvent;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("event_information_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= DVB_SI_EIT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_EIT_SECTION_MAX_SIZE))
		{
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				BITS_map(&bs, section_buf, section_size);

				event_information_section_t* peit_section = (pEITSection != NULL) ? pEITSection : new event_information_section_t;
				memset(peit_section, 0x00, sizeof(event_information_section_t));

				peit_section->table_id = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", peit_section->table_id, 8, "uimsbf", NULL, &bs);

				peit_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", peit_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

				if (peit_section->section_syntax_indicator == 1)
				{
					peit_section->reserved_future_use = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", peit_section->reserved_future_use, 1, "bslbf", NULL, &bs);
					peit_section->reserved0 = BITS_get(&bs, 2);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", peit_section->reserved0, 2, "bslbf", NULL, &bs);

					peit_section->section_length = BITS_get(&bs, 12);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", peit_section->section_length, 12, "uimsbf", NULL, &bs);

					if ((peit_section->section_length >= DVB_SI_EIT_SECTION_MIN_SIZE - 3) && (peit_section->section_length <= DVB_SI_EIT_SECTION_MAX_SIZE - 3))
					{
						peit_section->service_id = BITS_get(&bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "service_id", peit_section->service_id, 16, "uimsbf", NULL, &bs);

						peit_section->reserved1 = BITS_get(&bs, 2);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved1", peit_section->reserved1, 2, "bslbf", NULL, &bs);
						peit_section->version_number = BITS_get(&bs, 5);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", peit_section->version_number, 5, "uimsbf", NULL, &bs);
						peit_section->current_next_indicator = BITS_get(&bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", peit_section->current_next_indicator, 1, "bslbf", NULL, &bs);

						peit_section->section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", peit_section->section_number, 8, "uimsbf", NULL, &bs);

						peit_section->last_section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", peit_section->last_section_number, 8, "uimsbf", NULL, &bs);

						if (peit_section->section_number <= peit_section->last_section_number)
						{
							peit_section->transport_stream_id = BITS_get(&bs, 16);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "transport_stream_id", peit_section->transport_stream_id, 16, "uimsbf", NULL, &bs);

							peit_section->original_network_id = BITS_get(&bs, 16);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "original_network_id", peit_section->original_network_id, 16, "uimsbf", NULL, &bs);

							peit_section->segment_last_section_number = BITS_get(&bs, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "segment_last_section_number", peit_section->segment_last_section_number, 8, "uimsbf", NULL, &bs);

							peit_section->last_table_id = BITS_get(&bs, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_table_id", peit_section->last_table_id, 8, "uimsbf", NULL, &bs);

							N = 0;
							int event_loop_length = peit_section->section_length - 15;
							if (event_loop_length > 0)
							{
								pl1temp = bs.p_cur;
								BITS_byteSkip(&bs, event_loop_length);
								tinyxml2::XMLElement* pxmlEventLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "事件循环()", -1, -1, NULL, NULL, &bs);

								BITS_t event_loop_bs;
								BITS_map(&event_loop_bs, pl1temp, event_loop_length);

								while ((event_loop_length >= 12) && (N < MAX_EVENTS_IN_EIT_SECTION))
								{
									unsigned char* old_event_ptr = event_loop_bs.p_cur;
									tinyxml2::XMLElement* pxmlEventNode = pxmlDoc->NewKeyValuePairElement(pxmlEventLoopNode, "事件");

									pEvent = &(peit_section->astEvent[N]);

									pEvent->event_id = BITS_get(&event_loop_bs, 16);
									pxmlDoc->NewKeyValuePairElement(pxmlEventNode, "event_id", pEvent->event_id, 16, "uimsbf", NULL, &event_loop_bs);

									pEvent->start_time = BITS_x64_get(&event_loop_bs, 40);
									DVB_SI_NumericCoding2Text_UTCTime(pEvent->start_time, pszStartTime, sizeof(pszStartTime));
									pxmlDoc->NewKeyValuePairElementX64Mode(pxmlEventNode, "start_time", pEvent->start_time, 40, "bslbf", pszStartTime, &event_loop_bs);

									pEvent->duration = BITS_get(&event_loop_bs, 24);
									DVB_SI_NumericCoding2Text_BCDTime(pEvent->duration, pszDuration, sizeof(pszDuration));
									pxmlDoc->NewKeyValuePairElement(pxmlEventNode, "duration", pEvent->duration, 24, "uimsbf", pszDuration, &event_loop_bs);

									pEvent->running_status = BITS_get(&event_loop_bs, 3);
									DVB_SI_NumericCoding2Text_RunningStatus(pEvent->running_status, pszTemp, sizeof(pszTemp));
									pxmlDoc->NewKeyValuePairElement(pxmlEventNode, "running_status", pEvent->running_status, 3, "uimsbf", pszTemp, &event_loop_bs);

									pEvent->free_CA_mode = BITS_get(&event_loop_bs, 1);
									pxmlDoc->NewKeyValuePairElement(pxmlEventNode, "free_CA_mode", pEvent->free_CA_mode, 1, "bslbf", NULL, &event_loop_bs);

									pEvent->descriptors_loop_length = BITS_get(&event_loop_bs, 12);
									pxmlDoc->NewKeyValuePairElement(pxmlEventNode, "descriptors_loop_length", pEvent->descriptors_loop_length, 12, "uimsbf", NULL, &event_loop_bs);

									reserved_count = 0;
									if (pEvent->descriptors_loop_length)
									{
										pl2temp = event_loop_bs.p_cur;
										BITS_byteSkip(&event_loop_bs, pEvent->descriptors_loop_length);

										tinyxml2::XMLElement* pxmlDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlEventNode, "事件描述符循环()", -1, -1, NULL, NULL, &event_loop_bs);

										int descriptor_loop_length = pEvent->descriptors_loop_length;
										while ((descriptor_loop_length >= 2) && (descriptor_loop_length <= (event_loop_length - 12)) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
										{
											descriptor_tag = pl2temp[0];
											descriptor_length = pl2temp[1];
											move_length = descriptor_length + 2;

											pEvent->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
											pEvent->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
											pEvent->reserved_descriptor[reserved_count].descriptor_buf = pl2temp;
											pEvent->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

											switch (descriptor_tag)
											{
											case DVB_SI_SHORT_EVENT_DESCRIPTOR:
												DVB_SI_decode_short_event_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
												break;
											case DVB_SI_CONTENT_DESCRIPTOR:
												DVB_SI_decode_content_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
												break;
											case DVB_SI_PARENTAL_RATING_DESCRIPTOR:
												DVB_SI_decode_parental_rating_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
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
									pEvent->reserved_count = reserved_count;

									pxmlDoc->UpdateBufMark(pxmlEventNode, old_event_ptr, event_loop_bs.p_cur);
									sprintf_s(pszComment, sizeof(pszComment), "event_ID=0x%04X, start_time=%s, duration=%s", pEvent->event_id, pszStartTime, pszDuration);
									pxmlEventNode->SetAttribute("comment", pszComment);

									event_loop_length -= (12 + pEvent->descriptors_loop_length);
									N++;
								}
							}
							peit_section->N = N;

							unsigned int CRC_32 = BITS_get(&bs, 32);
							assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
							peit_section->CRC_32 = CRC_32_code;
							peit_section->CRC_32_verify = CRC_32_verify;
							tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", peit_section->CRC_32, 32, "rpchof", NULL, &bs);

							if (CRC_32_verify != CRC_32_code)
							{
								sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", peit_section->CRC_32_verify);
								pxmlCrcNode->SetAttribute("error", pszTemp);
								pxmlRootNode->SetAttribute("error", pszTemp);
							}
						}
						else
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! section_number = 0x%02X, last_section_number = 0x%02X", peit_section->section_number, peit_section->last_section_number);
							pxmlRootNode->SetAttribute("error", pszTemp);

							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_length = %d", peit_section->section_length);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_syntax_indicator = %d", peit_section->section_syntax_indicator);
					pxmlRootNode->SetAttribute("error", pszTemp);

					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}

				if (pEITSection == NULL)
				{
					//说明peit_section指针临时分配，函数返回前需要释放
					delete peit_section;
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
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
