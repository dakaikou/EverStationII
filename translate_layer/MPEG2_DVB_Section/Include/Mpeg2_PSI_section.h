#ifndef _MPEG2_PSI_SECTION_H_
#define _MPEG2_PSI_SECTION_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define MPEG2_PSI_SECTION_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define MPEG2_PSI_SECTION_LIB __declspec(dllimport)
#   else
#       define MPEG2_PSI_SECTION_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_PSI_SECTION_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_PSI_SECTION_LIB
#endif

#include <stdint.h>

#include "MPEG2_DVB_Common.h"
#include "MPEG2_DVB_SysCapability.h"

#include "DVB_SI_Descriptor.h"

#define MIN(x, y)						(x < y? x : y)
//#define min(x, y)						(x < y? x : y)
/*------------------------------------------------------------
Section size definition
-------------------------------------------------------------*/
#define MPEG2_PSI_PAT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_PMT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_CAT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_CMT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_TSDT_SECTION_MAX_SIZE			1024

#define MPEG2_PSI_PAT_SECTION_MIN_SIZE			12
#define MPEG2_PSI_PMT_SECTION_MIN_SIZE			16
#define MPEG2_PSI_CAT_SECTION_MIN_SIZE			12
#define MPEG2_PSI_CMT_SECTION_MIN_SIZE			12
#define MPEG2_PSI_TSDT_SECTION_MIN_SIZE			12

/*------------------------------------------------------------
		PAT definition 
-------------------------------------------------------------*/
typedef struct _PROGRAM_MAP_DESCRIPTION_s
{
	uint16_t			program_number;
	uint16_t			reserved : 3;
	uint16_t			program_map_PID : 13;

} PROGRAM_MAP_DESCRIPTION_t, *PPROGRAM_MAP_DESCRIPTION_t;

typedef struct program_association_section_s
{
	uint8_t			table_id;									//8

	uint8_t			section_syntax_indicator;					//1
	uint8_t			reserved_future_use;						//1
	uint8_t			reserved0;									//2
	uint16_t			section_length;								//12

	uint16_t			transport_stream_id;						//16

	uint8_t			reserved1;									//2
	uint8_t			version_number;								//5
	uint8_t			current_next_indicator;						//1

	uint8_t			section_number;								//8
	uint8_t			last_section_number;						//8

	int							 program_map_count;
	PROGRAM_MAP_DESCRIPTION_t	 astProgramMaps[MAX_PROGRAMS_PER_PAT_SECTION];			//N x 32					每个section最多只能携带253个节目映射信息

	uint32_t			CRC_32;										//32
	uint32_t			CRC_32_recalculated;						//32

} program_association_section_t, *pprogram_association_section_t;

MPEG2_PSI_SECTION_LIB int MPEG2_PSI_PAT_DecodeSection(uint8_t *buf, int length, program_association_section_t* pPATSection);

/*------------------------------------------------------------
		PMT definition 
-------------------------------------------------------------*/
typedef struct _ES_DESCRIPTION_s
{
	uint8_t		stream_type;
	uint8_t		stream_subtype;				//added by chendelin, descriptor_tag

	uint16_t		reserved0 : 3;
	uint16_t		elementary_PID : 13;

	uint16_t	    reserved1 : 4;
	uint16_t		ES_info_length : 12;

	int								ES_descriptor_count;
	reserved_descriptor_t			ES_descriptors[MAX_RESERVED_DESCRIPTORS];

} ES_DESCRIPTION_t, *PES_DESCRIPTION_t;

typedef struct TS_program_map_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		reserved_future_use;						//1
	uint8_t		reserved0;									//2
	uint16_t		section_length;								//12

	uint16_t		program_number;								//16

	uint8_t		reserved1;									//2
	uint8_t		version_number;								//5
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	uint8_t		reserved2;									// 3
	uint16_t		PCR_PID;									//13

	uint8_t		reserved3;									// 4
	uint16_t		program_info_length;						//12

	int							program_descriptor_count;
	reserved_descriptor_t		program_descriptors[MAX_RESERVED_DESCRIPTORS];

	int							ES_map_count;
	ES_DESCRIPTION_t			astESMaps[MAX_ESS_PER_PMT_SECTION];

	uint32_t			CRC_32;										//32
	uint32_t			CRC_32_recalculated;						//32

} TS_program_map_section_t, *pTS_program_map_section_t;

MPEG2_PSI_SECTION_LIB	int MPEG2_PSI_PMT_DecodeSection(uint8_t *section_buf, const int section_length, TS_program_map_section_t* pPMTSection);

/*------------------------------------------------------------
		CAT definition 
-------------------------------------------------------------*/
typedef struct CA_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		reserved_future_use;						//1
	uint8_t		reserved0;									//2
	uint16_t		section_length;								//12

	uint32_t		reserved1;									//18
	uint8_t		version_number;								//5
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	int							CA_descriptor_count;
	reserved_descriptor_t		CA_descriptors[MAX_RESERVED_DESCRIPTORS];

	uint32_t			CRC_32;										//32
	uint32_t			CRC_32_recalculated;						//32

} CA_section_t, *pCA_section_t;

MPEG2_PSI_SECTION_LIB	int	MPEG2_PSI_CAT_DecodeSection(uint8_t *buf, int length, CA_section_t* pCATSection);


/*------------------------------------------------------------
	TSDT 
-------------------------------------------------------------*/
typedef struct TS_description_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		reserved_future_use;						//1
	uint8_t		reserved0;									//2
	uint16_t	section_length;								//12

	uint32_t	reserved1;									//18
	uint8_t		version_number;								//5
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	int							TS_descriptor_count;
	reserved_descriptor_t		TS_descriptors[MAX_RESERVED_DESCRIPTORS];

	uint32_t			CRC_32;										//32
	uint32_t			CRC_32_recalculated;						//32

} TS_description_section_t, *pTS_description_section_t;

MPEG2_PSI_SECTION_LIB int MPEG2_PSI_TSDT_DecodeSection(uint8_t *buf, int length, TS_description_section_t* pTSDTSection);



#endif
