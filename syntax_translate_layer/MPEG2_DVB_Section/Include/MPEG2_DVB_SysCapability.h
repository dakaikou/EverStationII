#ifndef __MPEG_DVB_SYS_CAPABILITY_H__
#define __MPEG_DVB_SYS_CAPABILITY_H__

/*------------------------------------------------------------
Software limits definition
-------------------------------------------------------------*/
/*limits definition for SI information*/
#define MAX_LANGUAGES							16		//in teletext_descriptor this value maybe 50
#define MAX_LOCAL_TIME_AREA						32

#define	MAX_CAS									5
#define MAX_STREAMS_PER_NETWORK					40
#define MAX_SERVICES_PER_STREAM					64

#define MAX_APPLICATIONS						16

#define MAX_APPLICATION_TYPES					4
#define	MAX_APPLICATION_PROFILES				2
#define MAX_APPLICATION_TRANSPORT_PROTOCOLS		16

#define MAX_GROUPS								16

#define MAX_MODULES_PER_GROUP					1024
#define MAX_OBJECTS_PER_SERVICE					16

#define MAX_SUPPORTED_RUNSTATUS					10

#define	MAX_RESERVED_DESCRIPTORS				32

#define MAX_NIBBLES								2
#define MAX_COUNTRIES							2
#define MAX_EXTENDED_EVENTS						2

#define MAX_IP_PLATFORMS						32
#define MAX_IP_PLATFORM_NAME_LENGTH				16
#define MAX_IP_ADDRESSES						10

#define MAX_INT_TARGETS							10

#define MAX_CELLS								26
#define MAX_SUB_CELLS							26

#define MAX_VBI_DATA_SERVICES					5

#define MAX_COMPONENT_TEXT_LENGTH				16

#define MAX_PRIVATE_DATA_LENGTH					16
#define MAX_COPYRIGHT_INFO_LENGTH				32

// the capacity in every section
#define MAX_STREAMS_PER_NIT_SECTION				50			//changed by chendelin 20150718
#define MAX_STREAMS_PER_BAT_SECTION				50          //changed by chendelin 20150718
#define MAX_ESS_PER_PMT_SECTION					32          //changed by chendelin 20180719
#define MAX_PROGRAMS_PER_PAT_SECTION			32
#define MAX_EVENTS_IN_EIT_SECTION				200


#endif