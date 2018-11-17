#ifndef __DVB_MHP_TABLE_ID_H__
#define __DVB_MHP_TABLE_ID_H__

#define TABLE_ID_NIT_ACTUAL					0x40
#define TABLE_ID_NIT_OTHER					0x41

#define TABLE_ID_SDT_ACTUAL					0x42
//reserved for future use					0x43~0x45
#define TABLE_ID_SDT_OTHER					0x46
//reserved for future use					0x47~0x49

#define TABLE_ID_BAT						0x4A
//reserved for future use					0x4B
#define TABLE_ID_INT						0x4C
//reserved for future use					0x4D
#define TABLE_ID_EIT_PF_ACTUAL				0x4E
#define TABLE_ID_EIT_PF_OTHER				0x4F

#define TABLE_ID_EIT_SCH_ACTUAL_0			0x50
#define TABLE_ID_EIT_SCH_ACTUAL_1			0x51
#define TABLE_ID_EIT_SCH_ACTUAL_2			0x52
#define TABLE_ID_EIT_SCH_ACTUAL_3			0x53
#define TABLE_ID_EIT_SCH_ACTUAL_4			0x54
#define TABLE_ID_EIT_SCH_ACTUAL_5			0x55
#define TABLE_ID_EIT_SCH_ACTUAL_6			0x56
#define TABLE_ID_EIT_SCH_ACTUAL_7			0x57
#define TABLE_ID_EIT_SCH_ACTUAL_8			0x58
#define TABLE_ID_EIT_SCH_ACTUAL_9			0x59
#define TABLE_ID_EIT_SCH_ACTUAL_A			0x5A
#define TABLE_ID_EIT_SCH_ACTUAL_B			0x5B
#define TABLE_ID_EIT_SCH_ACTUAL_C			0x5C
#define TABLE_ID_EIT_SCH_ACTUAL_D			0x5D
#define TABLE_ID_EIT_SCH_ACTUAL_E			0x5E
#define TABLE_ID_EIT_SCH_ACTUAL_F			0x5F
#define TABLE_ID_EIT_SCH_OTHER_0			0x60
#define TABLE_ID_EIT_SCH_OTHER_1			0x61
#define TABLE_ID_EIT_SCH_OTHER_2			0x62
#define TABLE_ID_EIT_SCH_OTHER_3			0x63
#define TABLE_ID_EIT_SCH_OTHER_4			0x64
#define TABLE_ID_EIT_SCH_OTHER_5			0x65
#define TABLE_ID_EIT_SCH_OTHER_6			0x66
#define TABLE_ID_EIT_SCH_OTHER_7			0x67
#define TABLE_ID_EIT_SCH_OTHER_8			0x68
#define TABLE_ID_EIT_SCH_OTHER_9			0x69
#define TABLE_ID_EIT_SCH_OTHER_A			0x6A
#define TABLE_ID_EIT_SCH_OTHER_B			0x6B
#define TABLE_ID_EIT_SCH_OTHER_C			0x6C
#define TABLE_ID_EIT_SCH_OTHER_D			0x6D
#define TABLE_ID_EIT_SCH_OTHER_E			0x6E
#define TABLE_ID_EIT_SCH_OTHER_F			0x6F

#define TABLE_ID_TDT						0x70
#define TABLE_ID_RST						0x71
#define TABLE_ID_ST							0x72
#define TABLE_ID_TOT						0x73
#define TABLE_ID_AIT						0x74
//reserved for future use					0x75~0x7D
#define TABLE_ID_DIT						0x7E
#define TABLE_ID_SIT						0x7F
#define TABLE_ID_ECM_MIN					0x80
#define TABLE_ID_ECM_MAX					0x81
#define TABLE_ID_EMM_MIN					0x82
#define TABLE_ID_EMM_MAX					0x8F
//user defined								0x80~0xFE
#define TABLE_ID_UNKNOWN					0xFF

#define TABLE_ID_MPE						TABLE_ID_DSM_CC_PVT

#endif

