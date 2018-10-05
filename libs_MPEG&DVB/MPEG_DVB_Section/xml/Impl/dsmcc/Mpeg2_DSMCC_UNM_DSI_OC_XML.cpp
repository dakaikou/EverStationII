#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_OC.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_OC_XML.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP_XML.h"

/////////////////////////////////////////////

int	MPEG2_DSMCC_DSI_OC_PresentServiceGatewayInfo_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	//char	pszTemp[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pServiceGatewayInfo != NULL))
	{
		XMLElement* pxmlSGINode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "ServiceGatewayInfo()", NULL);

		//½âÎöIOP::IOR()
		MPEG2_DSMCC_PresentIOR_to_xml(pxmlDoc, pxmlSGINode, &(pServiceGatewayInfo->IOR));

		XMLDOC_NewElementForBits(pxmlDoc, pxmlSGINode, "downloadTaps_count", pServiceGatewayInfo->downloadTaps_count, 8, "uimsbf", "N1");
		if (pServiceGatewayInfo->downloadTaps_count > 0)
		{
			assert(0);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlSGINode, "serviceContextList_count", pServiceGatewayInfo->serviceContextList_count, 8, "uimsbf", "N2");
		if (pServiceGatewayInfo->serviceContextList_count > 0)
		{
			//pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "serviceContextList_data_byte()", ptemp, pServiceGatewayInfo->serviceContextList_count, NULL, &bs);
			assert(0);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlSGINode, "userInfoLength", pServiceGatewayInfo->userInfoLength, 16, "uimsbf", "N3");
		if (pServiceGatewayInfo->userInfoLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSGINode, "userInfo_data_byte[ ]", pServiceGatewayInfo->userInfo_data_byte, pServiceGatewayInfo->userInfoLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
