#ifndef _MPEG_DSMCC_COMMON_XML_H_
#define _MPEG_DSMCC_COMMON_XML_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_MPEG_DVB_EXPORT
#       define XML_MPEG_DVB_LIB __declspec(dllexport)
#   elif defined(_XML_MPEG_DVB_IMPORT)
#       define XML_MPEG_DVB_LIB __declspec(dllimport)
#   else
#       define XML_MPEG_DVB_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_MPEG_DVB_LIB __attribute__((visibility("default")))
#else
#   define XML_MPEG_DVB_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_BIOP.h"

#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_BIOP_PresentIOR_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::IOR_t* pIOR);

int	MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::BIOPProfileBody_t* pBIOPProfileBody = NULL);
int	MPEG2_DSMCC_BIOP_PresentBIOPProfileBody_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::BIOPProfileBody_t* pBIOPProfileBody);

int	MPEG2_DSMCC_BIOP_DecodeLiteOptionsProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::LiteOptionsProfileBody_t* pLiteOptionsProfileBody = NULL);
int	MPEG2_DSMCC_BIOP_PresentLiteOptionsProfileBody_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::LiteOptionsProfileBody_t* pLiteOptionsProfileBody);

XML_MPEG_DVB_LIB int	MPEG2_DSMCC_BIOP_PresentDirectMessage_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::DirectoryMessage_t* pDirectoryMessage);
XML_MPEG_DVB_LIB int	MPEG2_DSMCC_BIOP_PresentFileMessage_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::FileMessage_t* pFileMessage);

#endif
