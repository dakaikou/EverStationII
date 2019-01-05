#ifndef _MPEG2_PSISI_TABLE_H_
#define _MPEG2_PSISI_TABLE_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_PSISI_EXPORT
#       define MW_PSISI_LIB __declspec(dllexport)
#   elif defined(_MW_PSISI_IMPORT)
#       define MW_PSISI_LIB __declspec(dllimport)
#   else
#       define MW_PSISI_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_PSISI_LIB __attribute__((visibility("default")))
#else
#   define MW_PSISI_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_SI_Descriptor.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_SI_Section.h"

#include "MiddleWare_PSISI_Common.h"
#include "MiddleWare_PSISI_SysCapability.h"

/*------------------------------------------------------------
		PAT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CPAT : public CPVT
{
public:
	CPAT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CPAT(void);

protected:

/*------------------semantic part---------------------------*/
	uint16_t				  m_usTransportStreamID;
	uint16_t				  m_usNitPID;

	int						   m_nProgramCount;
	PROGRAM_MAP_DESCRIPTION_t* m_astProgram;
	int						   m_nMemoryForProgramInfos;

public:
	void	 Init(void);
	void	 Reset(void);
	int		 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetNetworkPID(void);
	uint16_t GetTransportStreamID(void);

	uint16_t GetProgramCount(void);
	int		 GetProgramByID(uint16_t program_number, PROGRAM_MAP_DESCRIPTION_t* pstProgramMap);
	int		 GetProgramByIndex(int index, PROGRAM_MAP_DESCRIPTION_t* pstProgramMap);
};

/*------------------------------------------------------------
		PMT definition 
-------------------------------------------------------------*/
typedef struct _ES_INFO_s
{
	uint8_t		stream_type;
	uint8_t		stream_subtype;				//added by chendelin, descriptor_tag
	uint16_t	elementary_PID;

	uint16_t	data_broadcast_id;
	uint32_t	carousel_id;				//added by chendelin, 20101129
	uint16_t	carousel_type_id;			//added by chendelin, 20101129
	uint8_t		component_tag;				//added by chendelin, 20101128
	uint16_t	association_tag;			//added by chendelin, 20101129
	uint32_t	transaction_id;				//added by chendelin, 20101129

	//uint16_t	ES_info_length;

} ES_INFO_t;


class MW_PSISI_LIB CPMT : public CPVT
{
public:
	CPMT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CPMT(void);

protected:
	uint16_t				  m_usProgramNumber;
	uint16_t				  m_usPCR_PID;

	int						  m_nEsCount;
	ES_INFO_t*				  m_astEsInfo;
	int						  m_nMemoryForEsInfos;

public:
	void	 Init(void);
	void	 Reset(void);
	int		 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetProgramNumber(void);
	uint16_t GetPcrPID(void);

	int GetESCount(void);
	int GetESInfo(int index, ES_INFO_t* pstESInfo);
};

/*------------------------------------------------------------
		CAT definition 
-------------------------------------------------------------*/
typedef struct _CA_INFO_s
{
	uint16_t	CA_system_PID;
	uint16_t	EMM_PID;

} CA_SYSTEM_INFO_t;

class MW_PSISI_LIB CCAT : public CPVT
{
public:
	CCAT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CCAT(void);
protected:
	int					m_nCASystemCount;
	CA_SYSTEM_INFO_t*	m_astCASystemInfo;
	int				    m_nMemoryForCASystemInfos;

public:
	void	 Init(void);
	void	 Reset(void);
	int		 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetCASystemCount(void);
	int		 GetCASystemInfoByID(uint16_t CA_system_ID, CA_SYSTEM_INFO_t* pstCASystemInfo);
	int		 GetCASystemInfoByIndex(int index, CA_SYSTEM_INFO_t* pstCASystemInfo);
};

/*------------------------------------------------------------
	TSDT 
-------------------------------------------------------------*/
class MW_PSISI_LIB CTSDT : public CPVT
{
public:
	CTSDT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CTSDT(void);
public:

public:
	void	 Init(void);
	void	 Reset(void);
	int		 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

/*------------------------------------------------------------
		NIT definition 
-------------------------------------------------------------*/

typedef struct _STREAM_INFO_s
{
	uint16_t									transport_stream_id;
	uint16_t									original_network_id;

	service_list_descriptor_t					service_list_descriptor;
	
	union
	{
		satellite_delivery_system_descriptor_t		satellite_delivery_system_descriptor;
		cable_delivery_system_descriptor_t			cable_delivery_system_descriptor;
		terrestrial_delivery_system_descriptor_t	terrestrial_delivery_system_descriptor;
	} uDelivery;

} STREAM_INFO_t;

class MW_PSISI_LIB CNIT : public CPVT
{
public:
	CNIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CNIT(void);

protected:
/*------------------semantic part---------------------------*/
	uint16_t					m_usNetworkID;
	//char						m_pszNetworkName[MAX_NETWORK_NAME_LENGTH + 1];
	char*						m_pszNetworkName;

/*------------------syntax part---------------------------*/
	int						m_nStreamCount;
	STREAM_INFO_t*			m_astStreamInfo;
	int						m_nMemoryForStreamInfos;

public:
	void	 Init(void);
	void	 Reset(void);
	int		 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetNetworkID(void);
	int		 GetNetworkName(char* pszName, int size);

	int		 GetStreamCount(void);
	int GetStreamByIndex(int stream_index, STREAM_INFO_t* pstStreamInfo);
	int GetStreamByID(uint16_t transport_stream_id, STREAM_INFO_t* pstStreamInfo);
};

/*------------------------------------------------------------
		BAT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CBAT : public CPVT
{
public:
	CBAT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CBAT(void);

protected:

/*------------------semantic part---------------------------*/
	uint16_t				m_usBouquetID;
	//char					m_pszBouquetName[MAX_BOUQUET_NAME_LENGTH + 1];
	char*					m_pszBouquetName;
	
/*------------------syntax part---------------------------*/
	int						m_nStreamCount;
	STREAM_INFO_t*			m_astStreamInfo;
	int						m_nMemoryForStreamInfos;

public:
	void	 Init(void);
	void	 Reset(void);
	int		 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetBouquetID(void);
	int		 GetBouquetName(char* pszName, int size);

	int		 GetStreamCount(void);
	int GetStreamByIndex(int stream_index, STREAM_INFO_t* pstStreamInfo);
	int GetStreamByID(uint16_t transport_stream_id, STREAM_INFO_t* pstStreamInfo);
};

/*------------------------------------------------------------
		SDT definition 
-------------------------------------------------------------*/
typedef struct _SERVICE_INFO_s
{
	uint16_t								service_id;						//16 bits

	uint8_t									EIT_schedule_flag;				//1
	uint8_t									EIT_present_following_flag;		//1

	uint8_t									running_status;					//3
	uint8_t									free_CA_mode;					//1

	uint8_t									service_type;
	
	char									pszServiceProviderName[MAX_SERVICE_PROVIDER_NAME_LENGTH + 1];	//修剪前导符之后
	char									pszServiceName[MAX_SERVICE_NAME_LENGTH + 1];						//修剪前导符之后

	int										data_broadcast_count;
	data_broadcast_descriptor_t				data_broadcast_descriptor[MAX_RESERVED_DESCRIPTORS];

} SERVICE_INFO_t;

class MW_PSISI_LIB CSDT : public CPVT
{
public:
	CSDT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CSDT(void);

protected:

/*------------------semantic part---------------------------*/
	uint16_t				m_usTransportStreamID;
	uint16_t				m_usOriginalNetworkID;

	int						m_nServiceCount;
	SERVICE_INFO_t*			m_astServiceInfo;
	int						m_nMemoryForServiceInfos;

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	int GetTransportStreamID(void);
	int GetOriginalNetworkID(void);

	int GetServiceCount(void);
	int	GetServiceInfoByID(uint16_t service_id, SERVICE_INFO_t* pstServiceInfo);
	int GetServiceInfoByIndex(int index, SERVICE_INFO_t* pstServiceInfo);
};

/*------------------------------------------------------------
		EIT definition 
-------------------------------------------------------------*/
typedef struct _EVENT_INFO_s
{
	uint16_t event_id;									//16

	int64_t	 start_time;								//40
	uint32_t duration;								//24

	char		pszEventName[MAX_EVENT_NAME_LENGTH + 1];
	char		pszEventText[MAX_EVENT_TEXT_LENGTH + 1];

} EVENT_INFO_t;

class MW_PSISI_LIB CEIT : public CPVT
{
public:
	CEIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CEIT(void);

protected:

/*------------------semantic part---------------------------*/
	uint16_t				m_usServiceID;
	uint16_t				m_usTransportStreamID;
	uint16_t				m_usOriginalNetworkID;

	int						m_nEventCount;
	EVENT_INFO_t*			m_astEvent;
	int						m_nMemoryForEvents;

	
/*------------------syntax part---------------------------*/
	//int						m_nSegmentCount;
	//eit_segment_t*			m_astSegment;
	//int						m_nMemoryForSegments;

public:
	void	Init(void);
	void	Reset(void);
	
	int		AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetServiceID(void);
	uint16_t GetTransportStreamID(void);
	uint16_t GetOriginalNetworkID(void);

	int GetEventCount(void);
	int GetEventInfoByIndex(int event_index, EVENT_INFO_t* pstEventInfo);
};

/*------------------------------------------------------------
		RST definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CRST : public CPVT
{
public:
	CRST(uint16_t Key, uint16_t PID, uint8_t table_id);
	~CRST(void);
public:

public:
	void Init(void);
	void Reset(void);
	int  AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
private:
};

/*------------------------------------------------------------
		TDT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CTDT : public CPVT
{
public:
	CTDT(uint16_t Key, uint16_t PID, uint8_t table_id);
	~CTDT(void);
public:
	uint64_t m_UTC_time;

public:
	void Init(void);
	void Reset(void);
	int AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

/*------------------------------------------------------------
		TOT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CTOT : public CPVT
{
public:
	CTOT(uint16_t Key, uint16_t PID, uint8_t table_id);
	~CTOT(void);
public:
/*------------------syntax part---------------------------*/
	uint64_t m_UTC_time;

public:
	void Init(void);
	void Reset(void);
	int  AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

/*------------------------------------------------------------
		CMT definition 
-------------------------------------------------------------*/
class MW_PSISI_LIB CCMT : public CPVT
{
public:
	CCMT(uint16_t Key, uint16_t PID, uint8_t table_id);
	~CCMT(void);
public:

public:
	void Init(void);
	void Reset(void);
	int  AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
private:
};

#endif
