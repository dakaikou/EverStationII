#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_DDM.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_DDM_XML.h"

////////////////////////////////////////////
int	MPEG2_DSMCC_DDM_DecodeDownloadDataBlock_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDDB)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	DownloadDataBlock_t* pDownloadDataBlock = (pDDB != NULL) ? pDDB : new DownloadDataBlock_t;
	rtcode = MPEG2_DSMCC_DDM_DecodeDownloadDataBlock(buf, size, pDownloadDataBlock);
	rtcode = MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(pxmlDoc, pxmlParentNode, pDownloadDataBlock);

	if (pDDB == NULL)
	{
		//说明pDownloadDataBlock指针临时分配，函数返回前需要释放
		delete pDownloadDataBlock;
	}

	return rtcode;
}

int MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDownloadDataBlock != NULL))
	{
		XMLElement* pxmlSessionNode = pxmlParentNode;

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "moduleId", pDownloadDataBlock->moduleId, 2, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "moduleVersion", pDownloadDataBlock->moduleVersion, 1, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "reserved", pDownloadDataBlock->reserved, 1, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "blockNumber", pDownloadDataBlock->blockNumber, 2, NULL);

		if (pDownloadDataBlock->N > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSessionNode, "blockDataByte[ ]", pDownloadDataBlock->blockDataByte, pDownloadDataBlock->N, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
