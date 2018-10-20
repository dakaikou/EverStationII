#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_XML.h"
#include "../../../Include/MPEG_DVB_ErrorCode.h"
#include "../../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDSI)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	DownloadServerInitiate_t* pDownloadServerInitiate = (pDSI != NULL) ? pDSI : new DownloadServerInitiate_t;
	rtcode = MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(buf, size, pDownloadServerInitiate);
	rtcode = MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(pxmlDoc, pxmlParentNode, pDownloadServerInitiate);

	if (pDSI == NULL)
	{
		//说明pDownloadServerInitiate指针临时分配，函数返回前需要释放
		delete pDownloadServerInitiate;
	}

	return rtcode;
}

int	MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	//char		pszTemp[96];
	//char		pszStyle[64];
	char		pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDownloadServerInitiate != NULL))
	{
		//XMLElement* pxmlSessionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "DownloadServerInitiate()", NULL);
		XMLElement* pxmlSessionNode = pxmlParentNode;

		XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSessionNode, "serverId", pDownloadServerInitiate->serverId, 20, NULL);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadServerInitiate->compatibilityDescriptor);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlSessionNode, "compatibilityDescriptor()", NULL);
		pxmlDoc->SetAnchor(pxmlDescriptorNode);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlDescriptorNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);

		if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlDescriptorNode, "compatibilityDescriptorBuf[ ]",
				pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
		}

		pxmlDoc->ClearAnchor(pxmlDescriptorNode);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "privateDataLength", pDownloadServerInitiate->privateDataLength, 2, NULL);

		//defined in EN 301 192
		//若privateDataLength解析错误，将导致灾难性错误
		if (pDownloadServerInitiate->privateDataLength > 0)
		{
			if (pDownloadServerInitiate->data_broadcast_type == 0x0006)		//DC
			{
				sprintf_s(pszComment, sizeof(pszComment), "GroupInfoIndication");
			}
			else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)		//OC
			{
				sprintf_s(pszComment, sizeof(pszComment), "ServiceGatewayInfo");
			}
			else
			{
				sprintf_s(pszComment, sizeof(pszComment), "Unknown");
			}
			XMLElement* pxmlPayloadNode = pxmlDoc->NewBranchElement(pxmlSessionNode, "privateDataByte[ ]", pszComment);
			pxmlDoc->SetAnchor(pxmlPayloadNode);

			if (pDownloadServerInitiate->data_broadcast_type == 0x0006)		//DC
			{
				rtcode = MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication_to_xml(pDownloadServerInitiate->privateDataByte, pDownloadServerInitiate->privateDataLength, pxmlDoc, pxmlPayloadNode);
			}
			else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)		//OC
			{
				rtcode = MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo_to_xml(pDownloadServerInitiate->privateDataByte, pDownloadServerInitiate->privateDataLength, pxmlDoc, pxmlPayloadNode);
			}
			else
			{
				//如何判断privateDataByte载荷的类型？
				XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlPayloadNode, "privateDataByte[ ]", pDownloadServerInitiate->privateDataByte, pDownloadServerInitiate->privateDataLength, NULL);
			}

			pxmlDoc->ClearAnchor(pxmlPayloadNode);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

