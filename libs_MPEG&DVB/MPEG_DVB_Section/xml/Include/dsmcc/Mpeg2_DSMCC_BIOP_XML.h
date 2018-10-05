#ifndef _MPEG_DSMCC_COMMON_XML_H_
#define _MPEG_DSMCC_COMMON_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

//�޷�Ԥ��IOR()�ṹ��ĳ��ȣ����Ǽ���һ�飬��˵�һ������������ش�ָ��
int	MPEG2_DSMCC_PresentIOR_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IOP::IOR_t* pIOR);


#endif
