#ifndef __HAL_TINYXML2_H__
#define __HAL_TINYXML2_H__

#include <stdint.h>

#include "../Compile.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"
#include "thirdparty_libs/tinyxml2/include/tinyxml2.h"

#define MAX_TREE_LEVEL			10

typedef struct
{
	int offset;
	int i_left;
} BITS_TRACER_t;

using namespace tinyxml2;

#define XMLDOC_NewElementForString(pxmlDoc,pxmlParentNode,pszField,comment)							 pxmlDoc->NewElementForString(pxmlParentNode, pszField,comment)
#define XMLDOC_NewElementForBits(pxmlDoc,pxmlParentNode,pszField,value,bits,mnemonic,comment)		 pxmlDoc->NewElementForBits(pxmlParentNode, pszField, value, bits, mnemonic, comment)
#define XMLDOC_NewElementForX64Bits(pxmlDoc,pxmlParentNode,pszField,value,bits,mnemonic,comment)	 pxmlDoc->NewElementForX64Bits(pxmlParentNode, pszField, value, bits, mnemonic, comment)
#define XMLDOC_NewElementForByteBuf(pxmlDoc,pxmlParentNode,pszField,buf,length,comment)				 pxmlDoc->NewElementForByteBuf(pxmlParentNode, pszField, buf, length, comment)

#define XMLDOC_NewElementForByteMode(pxmlDoc,pxmlParentNode,pszField,value,bytes,comment)			 pxmlDoc->NewElementForByteMode(pxmlParentNode, pszField, value, bytes, comment)

class _CDL_EXPORT HALForXMLDoc : public tinyxml2::XMLDocument
{
	friend class XMLElement;

public:
	HALForXMLDoc(void);
	~HALForXMLDoc(void);

protected:
	struct
	{
		XMLElement * pxmlBranchNode;
	} m_stBranchStack[MAX_TREE_LEVEL];
	int			 m_nTreeLevel;

	BITS_TRACER_t m_bs_tracer;				//added by chendelin 2018.9.29

protected:
	void TrackTheBranchNode(XMLElement* pxmlBranchNode);
public:

	XMLElement * QueryNodeByKeyName(XMLElement* pxmlParentNode, const char* pszNodeName);

	//bitstream position control function
	void SetSyncOffset(int offset);
	//int GetSyncOffset(void);
	void Align(void);

	void SetAnchor(XMLElement* pxmlBranchNode, int offset = -1);
	void ClearAnchor(XMLElement* pxmlBranchNode);

	//New branch node
	XMLElement* NewRootElement(const char* key_name, const char* pszComment = NULL);
	//XMLElement* CloseRootElement(void);

	XMLElement* NewBranchElement(XMLElement* pxmlParent, const char* key_name, const char* pszComment = NULL);
	//void CloseBranchElement(XMLElement* pxmlBranchNode);

	//New leaf node
	XMLElement * NewElementForString(XMLElement* pxmlParent, const char* key_name, const char* pszComment = NULL);
	XMLElement * NewElementForBits(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL);
	XMLElement * NewElementForByteBuf(XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL);
	XMLElement * NewElementForX64Bits(XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL);

	XMLElement * NewElementForByteMode(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int byte_length = -1, const char* pszComment = NULL);
};


#endif	//__HAL_TINYXML2_H__

