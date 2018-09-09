#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_DSMCC_section_UNM.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/xml/Mpeg2_DSMCC_section_XML.h"
#include "../../Include/xml/Mpeg2_DSMCC_descriptor_XML.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
int MPEG2_DSMCC_UNM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, dsmcc_unm_section_t* pDSMCCSection)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	char					pszTemp[64];

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	DownloadServerInitiate_t*			pDownloadServerInitiate;
	DownloadInfoIndication_t*			pDownloadInfoIndication;

	BITS_t   bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("DSMCC_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (section_size <= MPEG2_DSMCC_SECTION_MAX_SIZE))
		{
			uint8_t table_id = section_buf[0];

			if ((table_id >= 0x38) && (table_id <= 0x3F))
			{
				dsmcc_unm_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_unm_section_t;
				memset(pdsmcc_section, 0x00, sizeof(dsmcc_unm_section_t));

				unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
				unsigned int CRC_32_code = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

				//section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

				//stream_error = 0;
				//if (section_syntax_indicator == 1)
				//{
				//	if (pdsmcc_section->CRC_32_verify != pdsmcc_section->CRC_32)
				//	{
				//		stream_error = 1;
				//	}
				//}
				//else
				//{
				//	//没有校验checksum, 以后增加

				//	//			assert((length % 4) == 0);
				//}

				BITS_map(&bs, section_buf, section_size);

				pdsmcc_section->table_id = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL, &bs);

				pdsmcc_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

				pdsmcc_section->private_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL, &bs);

				pdsmcc_section->reserved0 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL, &bs);

				pdsmcc_section->dsmcc_section_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL, &bs);

				if ((pdsmcc_section->dsmcc_section_length + 3) == section_size)
				{
					pdsmcc_section->table_id_extension = BITS_get(&bs, 16);
					tinyxml2::XMLElement* pxmlTableIDExtNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", "transaction_id<15:0>", &bs);

					pdsmcc_section->reserved1 = BITS_get(&bs, 2);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL, &bs);

					pdsmcc_section->version_number = BITS_get(&bs, 5);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL, &bs);

					pdsmcc_section->current_next_indicator = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL, &bs);

					pdsmcc_section->section_number = BITS_get(&bs, 8);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL, &bs);

					pdsmcc_section->last_section_number = BITS_get(&bs, 8);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL, &bs);

					if (pdsmcc_section->section_number <= pdsmcc_section->last_section_number)			//DSMCC语法特征点
					{
						int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
						uint8_t* section_payload_ptr = bs.p_cur;
						BITS_byteSkip(&bs, section_payload_length);
						tinyxml2::XMLElement* pxmlMessageNode = NULL;

						BYTES_t bytes;
						BYTES_map(&bytes, section_payload_ptr, section_payload_length);

						if (pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM)
						{
							pxmlMessageNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "userNetworkMessage()", -1, -1, NULL, NULL, &bs);

							//包含DownloadServerInitiate、DownloadInfoIndication、DownloadCancel

							uint8_t* old_msg_header_ptr = bytes.p_cur;
							tinyxml2::XMLElement* pxmlHeaderNode = pxmlDoc->NewKeyValuePairElementByteMode(pxmlMessageNode, "dsmccMessageHeader()");

							pdsmccMessageHeader = &(pdsmcc_section->dsmccMessageHeader);

							pdsmccMessageHeader->protocolDiscriminator = BYTES_get(&bytes, 1);
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "protocolDiscriminator", pdsmccMessageHeader->protocolDiscriminator, 1, NULL, &bytes);

							pdsmccMessageHeader->dsmccType = BYTES_get(&bytes, 1);
							MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccMessageHeader->dsmccType, pszTemp, sizeof(pszTemp));
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "dsmccType", pdsmccMessageHeader->dsmccType, 1, pszTemp, &bytes);

							pdsmccMessageHeader->messageId = BYTES_get(&bytes, 2);
							MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccMessageHeader->messageId, pszTemp, sizeof(pszTemp));
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "messageId", pdsmccMessageHeader->messageId, 2, pszTemp, &bytes);

							pdsmccMessageHeader->transactionId = BYTES_get(&bytes, 4);
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "transactionId", pdsmccMessageHeader->transactionId, 4, NULL, &bytes);

							pdsmccMessageHeader->reserved = BYTES_get(&bytes, 1);
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "reserved", pdsmccMessageHeader->reserved, 1, NULL, &bytes);

							pdsmccMessageHeader->adaptationLength = BYTES_get(&bytes, 1);
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "adaptationLength", pdsmccMessageHeader->adaptationLength, 1, NULL, &bytes);

							pdsmccMessageHeader->messageLength = BYTES_get(&bytes, 2);
							pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "messageLength", pdsmccMessageHeader->messageLength, 2, NULL, &bytes);

							assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
							//messageLength解析错误，将是灾难性的

							if (pdsmccMessageHeader->adaptationLength > 0)
							{
								uint8_t* old_adaption_header_ptr = bytes.p_cur;
								tinyxml2::XMLElement* pxmlAdaptationHeaderNode = pxmlDoc->NewKeyValuePairElementByteMode(pxmlHeaderNode, "dsmccAdaptationHeader()");

								//解析adaptation
								pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType = BYTES_get(&bytes, 1);
								pxmlDoc->NewKeyValuePairElementByteMode(pxmlAdaptationHeaderNode, "adaptationType", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType, 1, NULL, &bytes);

								pdsmccMessageHeader->dsmccAdaptationHeader.N = pdsmccMessageHeader->adaptationLength - 1;

								if (pdsmccMessageHeader->dsmccAdaptationHeader.N > 0)
								{
									int copy_length = min(64, pdsmccMessageHeader->dsmccAdaptationHeader.N);
									memcpy(pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, bytes.p_cur, copy_length);
									BYTES_skip(&bytes, pdsmccMessageHeader->dsmccAdaptationHeader.N);

									pxmlDoc->NewKeyValuePairElementByteMode(pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, copy_length, NULL, &bytes);
								}

								pxmlDoc->UpdateBufMark(pxmlAdaptationHeaderNode, old_adaption_header_ptr, bytes.p_cur);
							}

							pxmlDoc->UpdateBufMark(pxmlHeaderNode, old_msg_header_ptr, bytes.p_cur);

							uint8_t* msg_payload_ptr = bytes.p_cur;

							int msg_payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
							if (msg_payload_length > 0)
							{
								BYTES_skip(&bytes, msg_payload_length);

								if (pdsmccMessageHeader->messageId == 0x1002)			//DII
								{
									pDownloadInfoIndication = &(pdsmcc_section->u.DownloadInfoIndication);

									rtcode = MPEG2_DSMCC_DecodeDownloadInfoIndication_to_xml(msg_payload_ptr, msg_payload_length, pxmlDoc, pxmlMessageNode, pDownloadInfoIndication);
								}
								else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
								{
									pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);

									rtcode = MPEG2_DSMCC_DecodeDownloadServerInitiate_to_xml(msg_payload_ptr, msg_payload_length, pxmlDoc, pxmlMessageNode, pDownloadServerInitiate);
								}
								else
								{
									pxmlDoc->NewKeyValuePairElementByteMode(pxmlMessageNode, "messagePayload[ ]", msg_payload_ptr, msg_payload_length, NULL, &bytes);

									sprintf_s(pszTemp, sizeof(pszTemp), "incorrect messageId = 0x%04X", pdsmccMessageHeader->messageId);
									pxmlMessageNode->SetAttribute("error", pszTemp);

									rtcode = SECTION_PARSE_SYNTAX_ERROR;
								}
							}
						}
						else
						{
							assert(0);
						}

						unsigned int CRC_32 = BITS_get(&bs, 32);
						assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
						pdsmcc_section->CRC_32 = CRC_32_code;
						pdsmcc_section->CRC_32_verify = CRC_32_verify;
						tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", pdsmcc_section->CRC_32, 32, "rpchof", NULL, &bs);

						if (CRC_32_verify != CRC_32_code)
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", pdsmcc_section->CRC_32_verify);
							pxmlCrcNode->SetAttribute("error", pszTemp);
							pxmlRootNode->SetAttribute("error", pszTemp);
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! section_number = 0x%02X, last_section_number = 0x%02X", pdsmcc_section->section_number, pdsmcc_section->last_section_number);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "syntax error! section_size = %d, dsmcc_section_length =%d", section_size, pdsmcc_section->dsmcc_section_length);
					pxmlRootNode->SetAttribute("error", pszTemp);
					rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
				}

				if (pDSMCCSection == NULL)
				{
					//说明pdsmcc_section指针临时分配，函数返回前需要释放
					delete pdsmcc_section;
				}
			}
			else
			{
				sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect table_id = 0x%02X", table_id);
				pxmlRootNode->SetAttribute("error", pszTemp);
				rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
			}
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
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeGroupInfoIndication_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, GroupInfoIndication_t* pGII)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*	ptemp;
	//uint8_t*	pend;
	//S32		remain_length;
	S32		n;
	S32		reserved_count;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		move_length;
	S32		copy_length;
	//S32		GroupInfoLength;
	//S32		compatibilityDescriptorLength;
	char	pszTemp[96];
	BYTES_t	bytes;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlGroupsNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "privateDataByte[ ]", buf, length, "GroupInfoIndication()");
		pxmlDoc->UpdateBufMark(pxmlGroupsNode, buf, buf + length);

		if ((buf != NULL) && (length > 0))
		{
			GroupInfoIndication_t* pGroupInfoIndication = (pGII != NULL) ? pGII : new GroupInfoIndication_t;
			memset(pGroupInfoIndication, 0x00, sizeof(GroupInfoIndication_t));

			BYTES_map(&bytes, buf, length);

			pGroupInfoIndication->NumberOfGroups = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupsNode, "NumberOfGroups", pGroupInfoIndication->NumberOfGroups, 2, NULL, &bytes);

			n = 0;
			for (int group_index = 0; group_index < pGroupInfoIndication->NumberOfGroups; group_index++)
			{
				sprintf_s(pszTemp, sizeof(pszTemp), "GROUP[%d]", group_index);

				uint8_t* old_group_ptr = bytes.p_cur;
				tinyxml2::XMLElement* pxmlGroupNode = pxmlDoc->NewKeyValuePairElement(pxmlGroupsNode, pszTemp);

				if (n < MAX_GROUPS)
				{
					pGroupInfoIndication->GroupInfo[n].GroupId = BYTES_get(&bytes, 4);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupNode, "GroupId", pGroupInfoIndication->GroupInfo[n].GroupId, 4, NULL, &bytes);

					pGroupInfoIndication->GroupInfo[n].GroupSize = BYTES_get(&bytes, 4);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupNode, "GroupSize", pGroupInfoIndication->GroupInfo[n].GroupSize, 4, NULL, &bytes);

					uint8_t* old_descriptor_ptr = bytes.p_cur;
					tinyxml2::XMLElement* pxmlGroupCompatibilityNode = pxmlDoc->NewKeyValuePairElement(pxmlGroupNode, "GroupCompatibility");

					pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength = BYTES_get(&bytes, 2);
					assert(pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength == 0);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupCompatibilityNode, "compatibilityDescriptorLength", pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength, 2, NULL, &bytes);

					if (pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength > 0)
					{
						copy_length = min(pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength, sizeof(pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorBuf));
						memcpy(pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorBuf, bytes.p_cur, copy_length);
						BYTES_skip(&bytes, pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength);

						pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupCompatibilityNode, "compatibilityDescriptorBuf", pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorBuf, copy_length, NULL, &bytes);
					}
					pxmlDoc->UpdateBufMark(pxmlGroupCompatibilityNode, old_descriptor_ptr, bytes.p_cur);

					pGroupInfoIndication->GroupInfo[n].GroupInfoLength = BYTES_get(&bytes, 2);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupNode, "GroupInfoLength", pGroupInfoIndication->GroupInfo[n].GroupInfoLength, 2, NULL, &bytes);

					if (pGroupInfoIndication->GroupInfo[n].GroupInfoLength > 0)
					{
						ptemp = bytes.p_cur;
						BYTES_skip(&bytes, pGroupInfoIndication->GroupInfo[n].GroupInfoLength);

						tinyxml2::XMLElement* pxmlDescriptionNode = pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupNode, "GroupInfo()", -1, -1, NULL, &bytes);

						memset(&(pGroupInfoIndication->GroupInfo[n].name_descriptor), 0, sizeof(name_descriptor_t));
						memset(&(pGroupInfoIndication->GroupInfo[n].location_descriptor), 0, sizeof(location_descriptor_t));

						reserved_count = 0;
						int GroupInfoLength = pGroupInfoIndication->GroupInfo[n].GroupInfoLength;
						while ((GroupInfoLength >= 2) && (reserved_count < 32))
						{
							descriptor_tag = (ptemp[0] | 0x3000);
							descriptor_length = ptemp[1];

							move_length = descriptor_length + 2;

							switch (descriptor_tag)
							{
							//case MPEG2_DSMCC_NAME_DESCRIPTOR:
							//	MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptionNode, &(pGroupInfoIndication->GroupInfo[n].name_descriptor));
							//	break;
							//case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
							//	MPEG2_DSMCC_decode_location_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptionNode, &(pGroupInfoIndication->GroupInfo[n].location_descriptor));
							//	break;
							default:
								//if (descriptor_tag == 0x3081)
								//{
								//	if (pGroupInfoIndication->GroupInfo[n].name_descriptor.descriptor_tag == 0x00)
								//	{
								//		MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptionNode, &(pGroupInfoIndication->GroupInfo[n].name_descriptor));
								//	}
								//}
								//else
								{
									if (reserved_count < MAX_RESERVED_DESCRIPTORS)
									{
										pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
										pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
										pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_buf = ptemp;
										pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_size = move_length;

										decode_reserved_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptionNode);

										reserved_count++;
									}
								}

								break;
							}

							ptemp += move_length;
							GroupInfoLength -= move_length;
						}

						pGroupInfoIndication->GroupInfo[n].reserved_count = reserved_count;
					}

					n++;
				}
				else
				{
					assert(0);
				}

				pxmlDoc->UpdateBufMark(pxmlGroupNode, old_group_ptr, bytes.p_cur);
			}
			pGroupInfoIndication->N = n;

			pGroupInfoIndication->PrivateDataLength = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupsNode, "PrivateDataLength", pGroupInfoIndication->PrivateDataLength, 2, NULL, &bytes);

			if (pGroupInfoIndication->PrivateDataLength > 0)
			{
				copy_length = min(sizeof(pGroupInfoIndication->privateDataByte), pGroupInfoIndication->PrivateDataLength);
				memcpy(pGroupInfoIndication->privateDataByte, bytes.p_cur, copy_length);
				BYTES_skip(&bytes, pGroupInfoIndication->PrivateDataLength);

				pxmlDoc->NewKeyValuePairElementByteMode(pxmlGroupsNode, "privateDataByte", pGroupInfoIndication->privateDataByte, copy_length, NULL, &bytes);
			}

			if (pGII == NULL)
			{
				//说明pGroupInfoIndication指针临时分配，函数返回前需要释放
				delete pGroupInfoIndication;
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "parameters error!");
			pxmlGroupsNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_DecodeIOR_to_xml(BITS_t* pbs, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, IOP::IOR_t* pior)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	char	pszTemp[96];
	int		i;
	BIOPProfileBody_t*			pBIOPProfileBody;
	LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
	BIOP::ObjectLocation_t*		pObjectLocation;
	DSM::ConnBinder_t*			pConnBinder;
	//int							component_index;

	if (pxmlDoc != NULL)
	{
		if (pbs != NULL)
		{
			if ((pbs->p_end - pbs->p_cur) > 4)
			{
				IOP::IOR_t* pIOR = (pior != NULL) ? pior : new IOP::IOR_t;
				memset(pIOR, 0x00, sizeof(IOP::IOR_t));

				//解析IOP::IOR()
				uint8_t* old_ptr = pbs->p_cur;

				tinyxml2::XMLElement* pxmlIORNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "IOP::IOR()");

				pIOR->type_id_length = BITS_get(pbs, 32);
				pxmlDoc->NewKeyValuePairElement(pxmlIORNode, "type_id_length", pIOR->type_id_length, 32, "uimsbf", "N1", pbs);

				int N1 = pIOR->type_id_length;
				if (N1 > 0)
				{
					uint8_t* ptemp = pbs->p_cur;
					BITS_byteSkip(pbs, N1);

					int copy_length = min(sizeof(pIOR->type_id_byte), N1);
					memcpy(pIOR->type_id_byte, ptemp, copy_length);
					pxmlDoc->NewKeyValuePairElement(pxmlIORNode, "type_id_byte[ ]", (uint8_t*)pIOR->type_id_byte, N1, pIOR->type_id_byte, pbs);
				}

				if ((N1 % 4) != 0)
				{
					for (i = 0; i < (4 - (N1 % 4)); i++)
					{
						pIOR->alignment_gap[i] = BITS_get(pbs, 8);
					}
				}

				pIOR->taggedProfiles_count = BITS_get(pbs, 32);
				pxmlDoc->NewKeyValuePairElement(pxmlIORNode, "taggedProfiles_count", pIOR->taggedProfiles_count, 32, "uimsbf", "N2", pbs);

				if (pIOR->taggedProfiles_count > 0)
				{
					uint8_t* old_profiles_ptr = pbs->p_cur;
					tinyxml2::XMLElement* pxmlProfilesNode = pxmlDoc->NewKeyValuePairElement(pxmlIORNode, "taggedProfiles()");

					for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
					{
						uint8_t* old_profile_ptr = pbs->p_cur;

						sprintf_s(pszTemp, sizeof(pszTemp), "IOP::taggedProfile( %d )", profile_index);
						tinyxml2::XMLElement* pxmlProfileNode = pxmlDoc->NewKeyValuePairElement(pxmlProfilesNode, pszTemp);

						pIOR->taggedProfile[profile_index].profileId_tag = BITS_get(pbs, 32);
						pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "profileId_tag", pIOR->taggedProfile[profile_index].profileId_tag, 32, "uimsbf", NULL, pbs);

						pIOR->taggedProfile[profile_index].profile_data_length = BITS_get(pbs, 32);
						pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "profile_data_length", pIOR->taggedProfile[profile_index].profile_data_length, 32, "uimsbf", "N3", pbs);

						uint8_t* ptemp = pbs->p_cur;

						BITS_t bs_profile;
						BITS_map(&bs_profile, ptemp, pIOR->taggedProfile[profile_index].profile_data_length);

						BITS_byteSkip(pbs, pIOR->taggedProfile[profile_index].profile_data_length);

						if (pIOR->taggedProfile[profile_index].profileId_tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
						{
							pxmlProfileNode->SetAttribute("comment", "BIOP Profile Body");

							pBIOPProfileBody = &(pIOR->taggedProfile[profile_index].u.BIOPProfileBody);

							pBIOPProfileBody->profileId_tag = pIOR->taggedProfile[profile_index].profileId_tag;
							pBIOPProfileBody->profile_data_length = pIOR->taggedProfile[profile_index].profile_data_length;

							assert(pBIOPProfileBody->profile_data_length > 0);

							pBIOPProfileBody->profile_data_byte_order = BITS_get(&bs_profile, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "profile_data_byte_order", pBIOPProfileBody->profile_data_byte_order, 8, "uimsbf", NULL, &bs_profile);

							pBIOPProfileBody->liteComponents_count = BITS_get(&bs_profile, 8);
							pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "liteComponents_count", pBIOPProfileBody->liteComponents_count, 8, "uimsbf", NULL, &bs_profile);

							if (pBIOPProfileBody->liteComponents_count > 0)
							{
								uint8_t* old_components_ptr = bs_profile.p_cur;
								tinyxml2::XMLElement* pxmlComponentsNode = pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "liteComponents()");

								for (int component_index = 0; component_index < pBIOPProfileBody->liteComponents_count; component_index++)
								{
									uint8_t* old_component_ptr = bs_profile.p_cur;
									sprintf_s(pszTemp, sizeof(pszTemp), "liteComponent( %d )", component_index);
									tinyxml2::XMLElement* pxmlComponentNode = pxmlDoc->NewKeyValuePairElement(pxmlComponentsNode, pszTemp);

									uint32_t componentId_tag = BITS_get(&bs_profile, 32);
									pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "componentId_tag", componentId_tag, 32, "uimsbf", NULL, &bs_profile);

									int component_data_length = BITS_get(&bs_profile, 8);
									pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "component_data_length", component_data_length, 8, "uimsbf", NULL, &bs_profile);

									if (componentId_tag == 0x49534F50)
									{
										pxmlComponentNode->SetAttribute("comment", "BIOP::ObjectLocation");

										pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

										pObjectLocation->componentId_tag = componentId_tag;
										pObjectLocation->component_data_length = component_data_length;

										pObjectLocation->carouselId = BITS_get(&bs_profile, 32);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "carouselId", pObjectLocation->carouselId, 32, "uimsbf", NULL, &bs_profile);

										pObjectLocation->moduleId = BITS_get(&bs_profile, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "moduleId", pObjectLocation->moduleId, 16, "uimsbf", NULL, &bs_profile);

										pObjectLocation->version.major = BITS_get(&bs_profile, 8);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "version.major", pObjectLocation->version.major, 8, "uimsbf", NULL, &bs_profile);

										pObjectLocation->version.minor = BITS_get(&bs_profile, 8);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "version.minor", pObjectLocation->version.minor, 8, "uimsbf", NULL, &bs_profile);

										pObjectLocation->objectKey_length = BITS_get(&bs_profile, 8);
										assert(pObjectLocation->objectKey_length == 4);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "objectKey_length", pObjectLocation->objectKey_length, 8, "uimsbf", NULL, &bs_profile);

										pObjectLocation->objectKey_data = BITS_get(&bs_profile, 32);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "objectKey_data", pObjectLocation->objectKey_data, 32, "uimsbf", NULL, &bs_profile);
									}
									else if (componentId_tag == 0x49534F40)
									{
										pxmlComponentNode->SetAttribute("comment", "DSM::ConnBinder");
										pConnBinder = &(pBIOPProfileBody->ConnBinder);

										pConnBinder->componentId_tag = componentId_tag;
										pConnBinder->component_data_length = component_data_length;

										pConnBinder->taps_count = BITS_get(&bs_profile, 8);
										assert(pConnBinder->taps_count >= 1);
										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "taps_count", pConnBinder->taps_count, 8, "uimsbf", NULL, &bs_profile);

										uint8_t* old_tap_ptr = bs_profile.p_cur;
										tinyxml2::XMLElement* pxmlTapNode = pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "BIOP::Tap( 0 )");

										pConnBinder->Tap.id = BITS_get(&bs_profile, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "id", pConnBinder->Tap.id, 16, "uimsbf", NULL, &bs_profile);

										pConnBinder->Tap.use = BITS_get(&bs_profile, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "use", pConnBinder->Tap.use, 16, "uimsbf", NULL, &bs_profile);

										pConnBinder->Tap.association_tag = BITS_get(&bs_profile, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "association_tag", pConnBinder->Tap.association_tag, 16, "uimsbf", NULL, &bs_profile);

										pConnBinder->Tap.selector_length = BITS_get(&bs_profile, 8);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "selector_length", pConnBinder->Tap.selector_length, 8, "uimsbf", NULL, &bs_profile);

										pConnBinder->Tap.selector_type = BITS_get(&bs_profile, 16);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "selector_type", pConnBinder->Tap.selector_type, 16, "uimsbf", NULL, &bs_profile);

										pConnBinder->Tap.transactionId = BITS_get(&bs_profile, 32);

										uint16_t dii_table_id_extension = pConnBinder->Tap.transactionId & 0x0000ffff;
										sprintf_s(pszTemp, sizeof(pszTemp), "* dii_table_id_extension=0x%04x", dii_table_id_extension);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "transactionId", pConnBinder->Tap.transactionId, 32, "uimsbf", pszTemp, &bs_profile);

										pConnBinder->Tap.timeout = BITS_get(&bs_profile, 32);
										pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "timeout", pConnBinder->Tap.timeout, 32, "uimsbf", NULL, &bs_profile);

										pxmlDoc->UpdateBufMark(pxmlTapNode, old_tap_ptr, bs_profile.p_cur);

										if (pConnBinder->taps_count > 1)
										{
											for (int tap_index = 1; tap_index < pConnBinder->taps_count; tap_index++)
											{
												uint8_t* old_tap_ptr = bs_profile.p_cur;
												sprintf_s(pszTemp, sizeof(pszTemp), "BIOP::Tap( %d )", tap_index);
												tinyxml2::XMLElement* pxmlTapNode = pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, pszTemp);

												uint16_t id = BITS_get(&bs_profile, 16);
												pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "id", id, 16, "uimsbf", NULL, &bs_profile);

												uint16_t use = BITS_get(&bs_profile, 16);
												pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "use", use, 16, "uimsbf", NULL, &bs_profile);

												uint16_t association_tag = BITS_get(&bs_profile, 16);
												pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "association_tag", association_tag, 16, "uimsbf", NULL, &bs_profile);

												int selector_length = BITS_get(&bs_profile, 8);
												pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "selector_length", selector_length, 8, "uimsbf", NULL, &bs_profile);

												ptemp = bs_profile.p_cur;
												BITS_byteSkip(&bs_profile, selector_length);

												pxmlDoc->NewKeyValuePairElement(pxmlTapNode, "selector_data_byte()", ptemp, selector_length, pszTemp, &bs_profile);

												pxmlDoc->UpdateBufMark(pxmlTapNode, old_tap_ptr, bs_profile.p_cur);
											}
										}
									}
									else
									{
										pxmlComponentNode->SetAttribute("comment", "BIOP::LiteComponent");

										ptemp = bs_profile.p_cur;
										BITS_byteSkip(&bs_profile, component_data_length);

										pxmlDoc->NewKeyValuePairElement(pxmlComponentNode, "component_data_byte[ ]", ptemp, component_data_length, NULL, &bs_profile);
									}

									pxmlDoc->UpdateBufMark(pxmlComponentNode, old_component_ptr, bs_profile.p_cur);
								}
							
								pxmlDoc->UpdateBufMark(pxmlComponentsNode, old_components_ptr, bs_profile.p_cur);
							}
						}
					//	else if (pIOR->taggedProfile[profile_index].profileId_tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
					//	{
					//		pxmlProfileNode->SetAttribute("comment", "LiteOptions Profile Body");

					//		pLiteOptionsProfileBody = &(pIOR->taggedProfile[profile_index].u.LiteOptionsProfileBody);

					//		pLiteOptionsProfileBody->profileId_tag = pIOR->taggedProfile[profile_index].profileId_tag;
					//		pLiteOptionsProfileBody->profile_data_length = pIOR->taggedProfile[profile_index].profile_data_length;

					//		assert(pLiteOptionsProfileBody->profile_data_length > 0);

					//		//		pLiteOptionsProfileBody->profileId_tag = tag;

					////		pLiteOptionsProfileBody->profile_data_length = *ptemp++;
					////		pLiteOptionsProfileBody->profile_data_length <<= 8;
					////		pLiteOptionsProfileBody->profile_data_length |= *ptemp++;
					////		pLiteOptionsProfileBody->profile_data_length <<= 8;
					////		pLiteOptionsProfileBody->profile_data_length |= *ptemp++;
					////		pLiteOptionsProfileBody->profile_data_length <<= 8;
					////		pLiteOptionsProfileBody->profile_data_length |= *ptemp++;

					////		pLiteOptionsProfileBody->profile_data_byte_order = *ptemp++;
					//	}
						else
						{
							pxmlProfileNode->SetAttribute("comment", "Unknown Profile Body");

							pxmlDoc->NewKeyValuePairElement(pxmlProfileNode, "profile_data_byte[ ]", ptemp, pIOR->taggedProfile[profile_index].profile_data_length, NULL, pbs);
						}

						pxmlDoc->UpdateBufMark(pxmlProfileNode, old_profile_ptr, pbs->p_cur);
					}

					pxmlDoc->UpdateBufMark(pxmlProfilesNode, old_profiles_ptr, pbs->p_cur);
				}

				pxmlDoc->UpdateBufMark(pxmlIORNode, old_ptr, pbs->p_cur);

				if (pior == NULL)
				{
					//说明pIOR指针临时分配，函数返回前需要释放
					delete pIOR;
				}
			}
			else
			{
				sprintf_s(pszTemp, sizeof(pszTemp), "bitstream parameters error!");
				pxmlParentNode->SetAttribute("error", pszTemp);
				rtcode = SECTION_PARSE_PARAMETER_ERROR;
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "parameters error!");
			pxmlParentNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeServiceGatewayInfo_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pSGI)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszTemp[96];

	BITS_t		bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlSGINode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "privateDataByte[]", buf, length, "ServiceGatewayInfo()");
		pxmlDoc->UpdateBufMark(pxmlSGINode, buf, buf + length);

		if ((buf != NULL) && (length > 4))
		{
			ServiceGatewayInfo_t* pServiceGatewayInfo = (pSGI != NULL) ? pSGI : new ServiceGatewayInfo_t;
			memset(pServiceGatewayInfo, 0x00, sizeof(ServiceGatewayInfo_t));

			BITS_map(&bs, buf, length);

			//解析IOP::IOR()
			MPEG2_DSMCC_DecodeIOR_to_xml(&bs, pxmlDoc, pxmlSGINode, &(pServiceGatewayInfo->IOR));

			pServiceGatewayInfo->downloadTaps_count = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "downloadTaps_count", pServiceGatewayInfo->downloadTaps_count, 8, "uimsbf", "N1", &bs);
			if (pServiceGatewayInfo->downloadTaps_count > 0)
			{
			//	assert(0);
			}

			pServiceGatewayInfo->serviceContextList_count = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "serviceContextList_count", pServiceGatewayInfo->serviceContextList_count, 8, "uimsbf", "N2", &bs);
			if (pServiceGatewayInfo->serviceContextList_count > 0)
			{
				uint8_t* ptemp = bs.p_cur;
				BITS_skip(&bs, pServiceGatewayInfo->serviceContextList_count);

				//int copy_length = min(sizeof(pServiceGatewayInfo->serviceContextList_data_byte), pServiceGatewayInfo->serviceContextList_count);
				//memcpy(pServiceGatewayInfo->serviceContextList_data_byte, ptemp, copy_length);

				pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "serviceContextList_data_byte()", ptemp, pServiceGatewayInfo->serviceContextList_count, NULL, &bs);
				assert(0);
			}

			pServiceGatewayInfo->userInfoLength = BITS_get(&bs, 16);
			//assert(pServiceGatewayInfo->userInfoLength == (bs.p_end - bs.p_cur));
			pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "userInfoLength", pServiceGatewayInfo->userInfoLength, 16, "uimsbf", "N3", &bs);
			if (pServiceGatewayInfo->userInfoLength > 0)
			{
				uint8_t* ptemp = bs.p_cur;
				BITS_skip(&bs, pServiceGatewayInfo->userInfoLength);

				int copy_length = min(sizeof(pServiceGatewayInfo->userInfo_data_byte), pServiceGatewayInfo->userInfoLength);
				memcpy(pServiceGatewayInfo->userInfo_data_byte, ptemp, copy_length);
				pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "userInfo_data_byte[ ]", pServiceGatewayInfo->userInfo_data_byte, copy_length, NULL, &bs);
			}

			if (pSGI == NULL)
			{
				//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
				delete pServiceGatewayInfo;
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "parameters error!");
			pxmlSGINode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeDownloadServerInitiate_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDSI)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	//uint8_t*	ptemp;
	int			remain_length;
	int			copy_length;
	char		pszTemp[96];
	char		pszStyle[64];
	BYTES_t		bytes;

	GroupInfoIndication_t*			pGroupInfoIndication;
	ServiceGatewayInfo_t*			pServiceGatewayInfo;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlSessionNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "DownloadServerInitiate()");
		pxmlDoc->UpdateBufMark(pxmlSessionNode, buf, buf + length);

		if ((buf != NULL) && (length >= 22))
		{
			DownloadServerInitiate_t* pDownloadServerInitiate = (pDSI != NULL) ? pDSI : new DownloadServerInitiate_t;
			memset(pDownloadServerInitiate, 0x00, sizeof(DownloadServerInitiate_t));

			BYTES_map(&bytes, buf, length);

			BYTES_copy(pDownloadServerInitiate->serverId, &bytes, 20);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "serverId", pDownloadServerInitiate->serverId, 20, NULL, &bytes);

			uint8_t* old_ptr = bytes.p_cur;
			tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "compatibilityDescriptor()");

			pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlDescriptorNode, "compatibilityDescriptorLength", pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength, 2, NULL, &bytes);

			if (pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength > 0)
			{
				copy_length = min(pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength, sizeof(pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorBuf));
				memcpy(pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorBuf, bytes.p_cur, copy_length);
				BYTES_skip(&bytes, pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength);

				pxmlDoc->NewKeyValuePairElementByteMode(pxmlDescriptorNode, "compatibilityDescriptorBuf", 
					pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorBuf, copy_length, NULL, &bytes);
			}

			pxmlDoc->UpdateBufMark(pxmlDescriptorNode, old_ptr, bytes.p_cur);

			pDownloadServerInitiate->privateDataLength = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "privateDataLength", pDownloadServerInitiate->privateDataLength, 2, NULL, &bytes);

			//defined in EN 301 192
			//若privateDataLength解析错误，将导致灾难性错误
			if (pDownloadServerInitiate->privateDataLength > 0)
			{
				remain_length = length - (int)(bytes.p_cur - bytes.p_start);
				assert(pDownloadServerInitiate->privateDataLength <= remain_length);

				uint8_t* privateDataByte = bytes.p_cur;
				BYTES_skip(&bytes, pDownloadServerInitiate->privateDataLength);

				//检查privateDataByte的类型
				strcpy_s(pszStyle, sizeof(pszStyle), "unknown");

				int value_hi = (privateDataByte[0] << 8) | privateDataByte[1];
				if (value_hi == 0)
				{
					int value_lo = (privateDataByte[2] << 8) | privateDataByte[3];
					if (value_lo == 4)
					{
						//判断的依据为 IOP::IOR()的type_id_length 一般为4
						strcpy_s(pszStyle, sizeof(pszStyle), "ServiceGatewayInfo");
					}
				}
				else
				{
					strcpy_s(pszStyle, sizeof(pszStyle), "GroupInfoIndication");
				}

				//old_ptr = bytes.p_cur;
				if (strcmp(pszStyle, "GroupInfoIndication") == 0)
				{
					pDownloadServerInitiate->data_broadcast_type = 0x0006;

					pGroupInfoIndication = &(pDownloadServerInitiate->u.GroupInfoIndication);
					rtcode = MPEG2_DSMCC_DecodeGroupInfoIndication_to_xml(privateDataByte, pDownloadServerInitiate->privateDataLength, pxmlDoc, pxmlSessionNode, pGroupInfoIndication);
				}
				else if (strcmp(pszStyle, "ServiceGatewayInfo") == 0)
				{
					pDownloadServerInitiate->data_broadcast_type = 0x0007;

					pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);
					rtcode = MPEG2_DSMCC_DecodeServiceGatewayInfo_to_xml(privateDataByte, pDownloadServerInitiate->privateDataLength, pxmlDoc, pxmlSessionNode, pServiceGatewayInfo);
				}
				else
				{
					copy_length = min(pDownloadServerInitiate->privateDataLength, sizeof(pDownloadServerInitiate->u.privateDataByte));
					memcpy(pDownloadServerInitiate->u.privateDataByte, privateDataByte, copy_length);

					//如何判断privateDataByte载荷的类型？
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "privateDataByte[ ]", pDownloadServerInitiate->u.privateDataByte, 
						copy_length , pszStyle, &bytes);
				}
			}

			if (pDSI == NULL)
			{
				//说明pDownloadServerInitiate指针临时分配，函数返回前需要释放
				delete pDownloadServerInitiate;
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "parameters error!");
			pxmlSessionNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeDownloadInfoIndication_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDII)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	int		n;
	int			reserved_count;
	uint16_t	descriptor_tag;
	int			descriptor_length;
	int			move_length;
	int		    copy_length;

	char		pszTemp[96];
	BYTES_t		bytes;

	DC_moduleInfo_t*	pDC_moduleInfo;
	BIOP_moduleInfo_t*	pBIOP_moduleInfo;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlSessionNode = NULL;
		pxmlSessionNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "DownloadInfoIndication()");
		pxmlDoc->UpdateBufMark(pxmlSessionNode, buf, buf + length);

		if ((buf != NULL) && (length >= 20))
		{
			DownloadInfoIndication_t* pDownloadInfoIndication = (pDII != NULL) ? pDII : new DownloadInfoIndication_t;
			memset(pDownloadInfoIndication, 0x00, sizeof(DownloadInfoIndication_t));

			BYTES_map(&bytes, buf, length);

			pDownloadInfoIndication->downloadId = BYTES_get(&bytes, 4);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "downloadId", pDownloadInfoIndication->downloadId, 4, NULL, &bytes);

			pDownloadInfoIndication->blockSize = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "blockSize", pDownloadInfoIndication->blockSize, 2, NULL, &bytes);

			pDownloadInfoIndication->windowSize = BYTES_get(&bytes, 1);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "windowSize", pDownloadInfoIndication->windowSize, 1, NULL, &bytes);

			pDownloadInfoIndication->ackPeriod = BYTES_get(&bytes, 1);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "ackPeriod", pDownloadInfoIndication->ackPeriod, 1, NULL, &bytes);

			pDownloadInfoIndication->tCDownloadWindow = BYTES_get(&bytes, 4);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "tCDownloadWindow", pDownloadInfoIndication->tCDownloadWindow, 4, NULL, &bytes);

			pDownloadInfoIndication->tCDownloadScenario = BYTES_get(&bytes, 4);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "tCDownloadScenario", pDownloadInfoIndication->tCDownloadScenario, 4, NULL, &bytes);

			uint8_t* old_ptr = bytes.p_cur;
			tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "compatibilityDescriptor()");

			pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlDescriptorNode, "compatibilityDescriptorLength", pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength, 2, NULL, &bytes);
			
			if (pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength > 0)
			{
				copy_length = min(pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength, sizeof(pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorBuf));
				memcpy(pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorBuf, bytes.p_cur, copy_length);
				BYTES_skip(&bytes, pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength);

				pxmlDoc->NewKeyValuePairElementByteMode(pxmlDescriptorNode, "compatibilityDescriptorBuf", pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorBuf, pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength, NULL, &bytes);
			}

			pxmlDoc->UpdateBufMark(pxmlDescriptorNode, old_ptr, bytes.p_cur);

			//defined in EN 301192
			pDownloadInfoIndication->numberOfModules = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "numberOfModules", pDownloadInfoIndication->numberOfModules, 2, NULL, &bytes);

			if (pDownloadInfoIndication->numberOfModules > 0)
			{
				n = 0;
				for (int moduleIndex = 0; moduleIndex < pDownloadInfoIndication->numberOfModules; moduleIndex++)
				{
					uint8_t* old_module_ptr = bytes.p_cur;
					sprintf_s(pszTemp, sizeof(pszTemp), "MODULE[%d]", moduleIndex);
					tinyxml2::XMLElement* pxmlModuleNode = pxmlDoc->NewKeyValuePairElement(pxmlSessionNode, pszTemp);

					uint16_t moduleId = BYTES_get(&bytes, 2);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "moduleId", moduleId, 2, NULL, &bytes);

					uint32_t moduleSize = BYTES_get(&bytes, 4);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "moduleSize", moduleSize, 4, NULL, &bytes);

					uint8_t moduleVersion = BYTES_get(&bytes, 1);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "moduleVersion", moduleVersion, 1, NULL, &bytes);

					uint8_t moduleInfoLength = BYTES_get(&bytes, 1);
					pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "moduleInfoLength", moduleInfoLength, 1, NULL, &bytes);

					sprintf_s(pszTemp, "ID=0x%04X, size=%d, ver=%d", moduleId, moduleSize, moduleVersion);
					pxmlModuleNode->SetAttribute("comment", pszTemp);

					uint8_t* moduleInfoByte = NULL;
					if (moduleInfoLength > 0)
					{
						moduleInfoByte = bytes.p_cur;
						BYTES_skip(&bytes, moduleInfoLength);
					}

					if (n < MAX_MODULES_PER_GROUP)
					{
						pDownloadInfoIndication->moduleInfo[n].moduleId = moduleId;
						pDownloadInfoIndication->moduleInfo[n].moduleSize = moduleSize;
						pDownloadInfoIndication->moduleInfo[n].moduleVersion = moduleVersion;
						pDownloadInfoIndication->moduleInfo[n].moduleInfoLength = moduleInfoLength;

						if (moduleInfoByte != NULL)
						{
							pDownloadInfoIndication->moduleInfo[n].data_broadcast_type = 0x0006;	//默认假设为DC
							if (pDownloadInfoIndication->moduleInfo[n].moduleInfoLength >= 21)
							{
								int taps_count = moduleInfoByte[12];
								if (taps_count >= 1)
								{
									uint16_t ID = (moduleInfoByte[13] << 8) | moduleInfoByte[14];
									uint16_t use = (moduleInfoByte[15] << 8) | moduleInfoByte[16];
									uint8_t selector_length = moduleInfoByte[19];
									if ((ID == 0x0000) && (use == 0x0017) && (selector_length == 0x00))
									{
										pDownloadInfoIndication->moduleInfo[n].data_broadcast_type = 0x0007;	//假设为OC
									}
								}
							}

							if (pDownloadInfoIndication->moduleInfo[n].data_broadcast_type == 0x0006)			//DC
							{
								pDC_moduleInfo = &(pDownloadInfoIndication->moduleInfo[n].u.DC_moduleInfo);

								reserved_count = 0;
								uint8_t* ptemp = moduleInfoByte;
								int remain_length = moduleInfoLength;
								int descriptor_count = 0;
								while ((remain_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
								{
									descriptor_tag = (ptemp[0] | 0x3000);
									descriptor_length = ptemp[1];
									move_length = descriptor_length + 2;

									//截成一段一段的描述符
									pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
									pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
									pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_buf = ptemp;
									pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

									switch (descriptor_tag)
									{
									case MPEG2_DSMCC_NAME_DESCRIPTOR:
										rtcode = MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->name_descriptor));
										break;
									case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
										rtcode = MPEG2_DSMCC_decode_location_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->location_descriptor));
										break;
									default:

										//if (descriptor_tag == 0x3081)
										//{
										//	if (pDC_moduleInfo->name_descriptor.descriptor_tag == 0x00)
										//	{
										//		rtcode = MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->name_descriptor));
										//	}
										//}
										//else
										{
											decode_reserved_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode);
										}

										break;
									}

									descriptor_count++;

									ptemp += move_length;
									remain_length -= move_length;
								}

								pDC_moduleInfo->reserved_count = reserved_count;
							}
							else if (pDownloadInfoIndication->moduleInfo[n].data_broadcast_type == 0x0007)
							{
								pBIOP_moduleInfo = &(pDownloadInfoIndication->moduleInfo[n].u.BIOP_moduleInfo);

								BYTES_t module_info_byte;
								BYTES_map(&module_info_byte, moduleInfoByte, pDownloadInfoIndication->moduleInfo[n].moduleInfoLength);

								pBIOP_moduleInfo->moduleTimeOut = BYTES_get(&module_info_byte, 4);
								pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "moduleTimeOut", pBIOP_moduleInfo->moduleTimeOut, 4, NULL, &module_info_byte);

								pBIOP_moduleInfo->blockTimeOut = BYTES_get(&module_info_byte, 4);
								pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "blockTimeOut", pBIOP_moduleInfo->blockTimeOut, 4, NULL, &module_info_byte);

								pBIOP_moduleInfo->minBlockTime = BYTES_get(&module_info_byte, 4);
								pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "minBlockTime", pBIOP_moduleInfo->minBlockTime, 4, NULL, &module_info_byte);

								pBIOP_moduleInfo->taps_count = BYTES_get(&module_info_byte, 1);
								pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "taps_count", pBIOP_moduleInfo->taps_count, 1, NULL, &module_info_byte);

								assert(pBIOP_moduleInfo->taps_count <= 4);
								for (int tap_index = 0; tap_index < pBIOP_moduleInfo->taps_count; tap_index++)
								{
									uint8_t* old_tap_ptr = module_info_byte.p_cur;
									sprintf_s(pszTemp, sizeof(pszTemp), "TAP[%d]", tap_index);
									tinyxml2::XMLElement* pxmlTapNode = pxmlDoc->NewKeyValuePairElement(pxmlModuleNode, pszTemp);

									pBIOP_moduleInfo->TAP[tap_index].id = BYTES_get(&module_info_byte, 2);
									assert(pBIOP_moduleInfo->TAP[tap_index].id == 0x0000);
									pxmlDoc->NewKeyValuePairElementByteMode(pxmlTapNode, "ID", pBIOP_moduleInfo->TAP[tap_index].id, 2, NULL, &module_info_byte);

									pBIOP_moduleInfo->TAP[tap_index].use = BYTES_get(&module_info_byte, 2);
									assert(pBIOP_moduleInfo->TAP[tap_index].use == 0x0017);
									MPEG2_DSMCC_NumericCoding2Text_tapUse(pBIOP_moduleInfo->TAP[tap_index].use, pszTemp, sizeof(pszTemp));
									pxmlDoc->NewKeyValuePairElementByteMode(pxmlTapNode, "use", pBIOP_moduleInfo->TAP[tap_index].use, 2, pszTemp, &module_info_byte);

									pBIOP_moduleInfo->TAP[tap_index].association_tag = BYTES_get(&module_info_byte, 2);
									pxmlDoc->NewKeyValuePairElementByteMode(pxmlTapNode, "association_tag", pBIOP_moduleInfo->TAP[tap_index].association_tag, 2, NULL, &module_info_byte);

									pBIOP_moduleInfo->TAP[tap_index].selector_length = BYTES_get(&module_info_byte, 1);
									assert(pBIOP_moduleInfo->TAP[tap_index].selector_length == 0x00);
									pxmlDoc->NewKeyValuePairElementByteMode(pxmlTapNode, "selector_length", pBIOP_moduleInfo->TAP[tap_index].selector_length, 1, NULL, &module_info_byte);

									pxmlDoc->UpdateBufMark(pxmlTapNode, old_tap_ptr, module_info_byte.p_cur);
								}

								pBIOP_moduleInfo->userInfoLength = BYTES_get(&module_info_byte, 1);
								pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "userInfoLength", pBIOP_moduleInfo->userInfoLength, 1, NULL, &module_info_byte);

								if (pBIOP_moduleInfo->userInfoLength > 0)
								{
									copy_length = min(64, pBIOP_moduleInfo->userInfoLength);
									memcpy(pBIOP_moduleInfo->userInfo_data_byte, module_info_byte.p_cur, copy_length);
									BYTES_skip(&module_info_byte, pBIOP_moduleInfo->userInfoLength);

									pxmlDoc->NewKeyValuePairElementByteMode(pxmlModuleNode, "module_info_byte()", -1, -1, NULL, &module_info_byte);
								}
							}
						}

						n++;
					}

					pxmlDoc->UpdateBufMark(pxmlModuleNode, old_module_ptr, bytes.p_cur);
				}
				pDownloadInfoIndication->N = n;
			}

			pDownloadInfoIndication->privateDataLength = BYTES_get(&bytes, 2);
			pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "privateDataLength", pDownloadInfoIndication->privateDataLength, 2, NULL, &bytes);

			if (pDownloadInfoIndication->privateDataLength > 0)
			{
				copy_length = min(64, pDownloadInfoIndication->privateDataLength);
				memcpy(pDownloadInfoIndication->privateDataByte, bytes.p_cur, copy_length);

				BYTES_skip(&bytes, pDownloadInfoIndication->privateDataLength);
				pxmlDoc->NewKeyValuePairElementByteMode(pxmlSessionNode, "privateDataByte[ ]", pDownloadInfoIndication->privateDataByte, copy_length, NULL, &bytes);
			}

			if (pDII == NULL)
			{
				//说明pDownloadInfoIndication指针临时分配，函数返回前需要释放
				delete pDownloadInfoIndication;
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "parameters error!");
			pxmlSessionNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
