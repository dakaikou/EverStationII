#ifndef __MPEG2_PSI_TABLE_ID_H__
#define __MPEG2_PSI_TABLE_ID_H__

#define TABLE_ID_PAT						0x00			//program_association_section
#define TABLE_ID_CAT						0x01			//conditional_access_section(CA_section)
#define TABLE_ID_PMT						0x02			//TS_program_map_section
#define TABLE_ID_TSDT						0x03			//TS_description_section
#define TABLE_ID_SCENE						0x04			//ISO_IEC_14496_scene_description_section
#define TABLE_ID_OBJECT						0x05			//ISO_IEC_14496_object_descriptor_section
#define TABLE_ID_METADATA					0x06			//Metadata_section
#define TABLE_ID_IPMP						0x07			//IPMP control_information_section
#define TABLE_ID_14496						0x08			//ISO_IEC_14496_section
//reserved									0x09~0x37

//ISO/IEC 13818-6 reserved					0x38~0x39
//#define TABLE_ID_DSMCC_MPE					0x3a
#define TABLE_ID_DSMCC_UNM					0x3b
#define TABLE_ID_DSMCC_DDM					0x3c
#define TABLE_ID_DSMCC_SD					0x3d
//#define TABLE_ID_DSMCC_PVT					0x3e
#define TABLE_ID_DSMCC_MPE					0x3e
#define TABLE_ID_DSMCC_RSV					0x3f

#define TABLE_ID_UNKNOWN					0xFF

#endif

