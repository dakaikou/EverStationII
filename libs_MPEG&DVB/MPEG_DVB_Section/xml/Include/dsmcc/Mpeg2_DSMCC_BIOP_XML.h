#ifndef _MPEG_DSMCC_COMMON_XML_H_
#define _MPEG_DSMCC_COMMON_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_PresentIOR_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IOP::IOR_t* pIOR);


#endif
