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
int MPEG2_PSI_PMT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, TS_program_map_section_t* ppmt_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (ppmt_section != NULL))
	{
		uint16_t descriptor_tag;
		int		 descriptor_length;
		uint8_t* descriptor_buf;
		int		 descriptor_size;
		char pszField[128];
		char pszComment[128];

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		sprintf_s(pszField, sizeof(pszField), "program_map_section(table_id=0x%02X)", ppmt_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, ppmt_section->section_length + 3);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", ppmt_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", ppmt_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "'0'", ppmt_section->reserved_future_use, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppmt_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", ppmt_section->section_length, 12, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "program_number", ppmt_section->program_number, 16, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppmt_section->reserved1, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", ppmt_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", ppmt_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", ppmt_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", ppmt_section->last_section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppmt_section->reserved2, 3, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "PCR_PID", ppmt_section->PCR_PID, 13, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppmt_section->reserved3, 4, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "program_info_length", ppmt_section->program_info_length, 12, "uimsbf", NULL);

		if (ppmt_section->program_info_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "program_info()");
			XMLElement* pxmlProgramInfoNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlProgramInfoNode, ppmt_section->program_info_length);

			for (int descriptor_index = 0; descriptor_index < ppmt_section->program_descriptor_count; descriptor_index++)
			{
				descriptor_buf = ppmt_section->program_descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = ppmt_section->program_descriptors[descriptor_index].descriptor_tag;
				descriptor_length = ppmt_section->program_descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
					//case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
					//	MPEG2_PSI_decode_registration_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case MPEG2_PSI_CA_DESCRIPTOR:
					//	MPEG2_PSI_decode_CA_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case MPEG2_PSI_SYSTEM_CLOCK_DESCRIPTOR:
					//	MPEG2_PSI_decode_system_clock_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case MPEG2_PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR:
					//	MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR:
					//	MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
					//case MPEG2_PSI_SMOOTHING_BUFFER_DESCRIPTOR:
					//	MPEG2_PSI_decode_smoothing_buffer_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
					//	break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlProgramInfoNode, ppmt_section->program_descriptors + descriptor_index);
					break;
				}
			}
		}

		int es_loop_length = ppmt_section->section_length - 9 - ppmt_section->program_info_length - 4;
		if (es_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "ES流循环(共 %d 个流)", ppmt_section->ES_map_count);
			XMLElement* pxmlESInfoLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlESInfoLoopNode, es_loop_length);

			for (int es_index = 0; es_index < ppmt_section->ES_map_count; es_index++)
			{
				ES_DESCRIPTION_t*	pstESMap = ppmt_section->astESMaps + es_index;

				int es_description_length = 5 + pstESMap->ES_info_length;
				sprintf_s(pszField, sizeof(pszField), "ES流[%d](PID=0x%04X)", es_index, pstESMap->elementary_PID);
				MPEG2_PSI_NumericCoding2Text_StreamType(pstESMap->stream_type, STREAM_SUBTYPE_UNKNOWN, pszComment, sizeof(pszComment));
				XMLElement* pxmlESNode = XMLDOC_NewElementForString(pxmlDoc, pxmlESInfoLoopNode, pszField, pszComment);
				XMLNODE_SetFieldLength(pxmlESNode, es_description_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlESNode, "stream_type", pstESMap->stream_type, 8, "uimsbf", pszComment);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlESNode, "reserved", pstESMap->reserved0, 3, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlESNode, "elementary_PID", pstESMap->elementary_PID, 13, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlESNode, "reserved", pstESMap->reserved1, 4, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlESNode, "ES_info_length", pstESMap->ES_info_length, 12, "uimsbf", NULL);

				if (pstESMap->ES_info_length > 0)
				{
					sprintf_s(pszField, sizeof(pszField), "ES_info()");
					XMLElement* pxmlESInfoNode = XMLDOC_NewElementForString(pxmlDoc, pxmlESNode, pszField, NULL);
					XMLNODE_SetFieldLength(pxmlESInfoNode, pstESMap->ES_info_length);

					for (int descriptor_index = 0; descriptor_index < pstESMap->ES_descriptor_count; descriptor_index++)
					{
						descriptor_buf = pstESMap->ES_descriptors[descriptor_index].descriptor_buf;
						descriptor_tag = pstESMap->ES_descriptors[descriptor_index].descriptor_tag;
						descriptor_length = pstESMap->ES_descriptors[descriptor_index].descriptor_length;
						descriptor_size = descriptor_length + 2;

						switch (descriptor_tag)
						{
							//case MPEG2_PSI_VIDEO_STREAM_DESCRIPTOR:
							//	MPEG2_PSI_decode_video_stream_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case MPEG2_PSI_AUDIO_STREAM_DESCRIPTOR:
							//	MPEG2_PSI_decode_audio_stream_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
							//	MPEG2_PSI_decode_registration_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode, &registration_descriptor);
							//	break;
							//case MPEG2_PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR:
							//	MPEG2_PSI_decode_data_stream_alignment_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case MPEG2_PSI_ISO_639_LANGUAGE_DESCRIPTOR:
							//	MPEG2_PSI_decode_ISO_639_language_descriptor(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR:
							//	MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case MPEG2_DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR:
							//	MPEG2_DSMCC_decode_carousel_identifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case MPEG2_DSMCC_ASSOCIATION_TAG_DESCRIPTOR:
							//	MPEG2_DSMCC_decode_association_tag_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case DVB_SI_STREAM_IDENTIFIER_DESCRIPTOR:
							//	DVB_SI_decode_stream_identifier_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case DVB_SI_TELETEXT_DESCRIPTOR:
							//	DVB_SI_decode_teletext_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case DVB_SI_SUBTITLING_DESCRIPTOR:
							//	DVB_SI_decode_subtitling_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case DVB_SI_DATA_BROADCAST_ID_DESCRIPTOR:
							//	DVB_SI_decode_data_broadcast_id_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
							//case DVB_SI_AC3_DESCRIPTOR:
							//	DVB_SI_decode_ac3_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlESInfoNode);
							//	break;
						default:
							MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlESInfoNode, pstESMap->ES_descriptors + descriptor_index);
							break;
						}
					}
				}
			}
		}

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", ppmt_section->CRC_32, 32, "rpchof", NULL);

		if (ppmt_section->CRC_32_recalculated != ppmt_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", ppmt_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_PSI_PMT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, TS_program_map_section_t* pPMTSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	TS_program_map_section_t* ppmt_section = (pPMTSection != NULL) ? pPMTSection : new TS_program_map_section_t;
	rtcode = MPEG2_PSI_PMT_DecodeSection(section_buf, section_size, ppmt_section);
	rtcode = MPEG2_PSI_PMT_PresentSection_to_XML(pxmlDoc, ppmt_section);

	if (pPMTSection == NULL)
	{
		//说明ppmt_section指针临时分配，函数返回前需要释放
		delete ppmt_section;
	}

	return rtcode;
}

