#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_DDM.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/xml/Mpeg2_DSMCC_section_XML.h"

////////////////////////////////////////////

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
