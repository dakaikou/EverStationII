#ifndef __HAL_TINYXML2_H__
#define __HAL_TINYXML2_H__

#include <stdint.h>

#include "../Compile.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"
#include "thirdparty_libs/tinyxml2/include/tinyxml2.h"

using namespace tinyxml2;

//#define XMLDOC_NewDeclaration(pxmlDoc,pszDeclaration) ((pxmlDoc != NULL)? pxmlDoc->NewDeclaration(pszDeclaration) : NULL)
//#define XMLDOC_InsertFirstChild(pxmlDoc,pxmlDeclaration) ((pxmlDoc != NULL)? pxmlDoc->InsertFirstChild(pxmlDeclaration) : NULL)
//#define XMLDOC_InsertEndChild(pxmlDoc,pxmlNode) ((pxmlDoc != NULL)? pxmlDoc->InsertEndChild(pxmlNode) : NULL)
//#define XMLDOC_NewRootElement(pxmlDoc,pszTitle) ((pxmlDoc != NULL)? ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement(pszTitle) : NULL)
//
//#define XMLDOC_NewElementForString(pxmlDoc,pxmlParentNode,pszField) ((pxmlDoc != NULL)? pxmlDoc->NewElementForString(pxmlParentNode, pszField) : NULL);
//#define XMLDOC_NewElementForBits(pxmlDoc,pxmlParentNode,pszField,value,bits,mnemonic,comment,pbs) ((pxmlDoc != NULL)? pxmlDoc->NewElementForBits(pxmlParentNode, pszField, value, bits, mnemonic, comment, pbs) : NULL);
//#define XMLDOC_NewElementForX64Bits(pxmlDoc,pxmlParentNode,pszField,value,bits,mnemonic,comment,pbs) ((pxmlDoc != NULL)? pxmlDoc->NewElementForX64Bits(pxmlParentNode, pszField, value, bits, mnemonic, comment, pbs) : NULL);
//#define XMLDOC_NewElementForBytes(pxmlDoc,pxmlParentNode,pszField,buf,length,comment,pbs) ((pxmlDoc != NULL)? pxmlDoc->NewElementForBytes(pxmlParentNode, pszField, buf, length, comment, pbs) : NULL);
//
//#define XMLNODE_SetAttribute(pxmlNode,pszItem,pszAttr) {if (pxmlNode != NULL) pxmlNode->SetAttribute(pszItem, pszAttr);}
//#define XMLNODE_SetStartEndPtr(pxmlNode,start_ptr,end_ptr) {if (pxmlNode != NULL) pxmlNode->SetStartEndPtr(start_ptr, end_ptr);}

#define XMLDOC_NewDeclaration(pxmlDoc,pszDeclaration)			pxmlDoc->NewDeclaration(pszDeclaration)
#define XMLDOC_InsertFirstChild(pxmlDoc,pxmlDeclaration)		pxmlDoc->InsertFirstChild(pxmlDeclaration)
#define XMLDOC_InsertEndChild(pxmlDoc,pxmlNode)					pxmlDoc->InsertEndChild(pxmlNode)
#define XMLDOC_NewRootElement(pxmlDoc,pszTitle)					((tinyxml2::XMLDocument*)pxmlDoc)->NewElement(pszTitle)

#define XMLDOC_NewElementForString(pxmlDoc,pxmlParentNode,pszField)										 pxmlDoc->NewElementForString(pxmlParentNode, pszField)
#define XMLDOC_NewElementForBits(pxmlDoc,pxmlParentNode,pszField,value,bits,mnemonic,comment,pbs)		 pxmlDoc->NewElementForBits(pxmlParentNode, pszField, value, bits, mnemonic, comment, pbs)
#define XMLDOC_NewElementForX64Bits(pxmlDoc,pxmlParentNode,pszField,value,bits,mnemonic,comment,pbs)	 pxmlDoc->NewElementForX64Bits(pxmlParentNode, pszField, value, bits, mnemonic, comment, pbs)
#define XMLDOC_NewElementForBytes(pxmlDoc,pxmlParentNode,pszField,buf,length,comment,pbs)				 pxmlDoc->NewElementForBytes(pxmlParentNode, pszField, buf, length, comment, pbs)

#define XMLNODE_SetAttribute(pxmlNode,pszItem,pszAttr)			pxmlNode->SetAttribute(pszItem, pszAttr)
#define XMLNODE_SetStartEndPtr(pxmlNode,start_ptr,end_ptr)		pxmlNode->SetStartEndPtr(start_ptr, end_ptr)

typedef struct
{
	int offset;
	int i_left;
} BITS_TRACER_t;

class _CDL_EXPORT HALForXMLDoc : public tinyxml2::XMLDocument
{
	friend class XMLElement;

public:
	HALForXMLDoc(void);
	~HALForXMLDoc(void);

public:
	XMLElement * NewElementForBits(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL, BITS_TRACER_t* pbs_tracer = NULL);
	XMLElement * NewElementForBytes(XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL, BITS_TRACER_t* pbs_tracer = NULL);
	XMLElement * NewElementForString(XMLElement* pxmlParent, const char* key_name, const char* string = NULL);

	XMLElement * NewElementForX64Bits(XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);

	//XMLElement * NewKeyValuePairElementByteMode(XMLElement* pxmlParent, const char* field_name, unsigned int value = -1, int byte_length = -1, const char* pszComment = NULL, const BYTES_t* pbytes_map = NULL);
	//XMLElement * NewKeyValuePairElementByteMode(XMLElement* pxmlParent, const char* field_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL, const BYTES_t* pbytes_map = NULL);

	XMLElement* QueryNodeByKeyName(XMLElement* pxmlParentNode, const char* pszNodeName);
};


#endif	//__HAL_TINYXML2_H__

