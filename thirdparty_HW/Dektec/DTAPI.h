//*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#* DTAPI.h *#*#*#*#*#*#*#*#*#* (C) 2000-2007 DekTec
//
// DTAPI: C++ API for DekTec's Digital-Video PCI Cards and USB-2 Adapters
//
// Main header file

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Change History -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// For a revision history of the DTAPI, please refer to the DTAPI documentation

//#pragma once
#ifndef __DTAPI_H
#define __DTAPI_H

#define _USE_DTAPIDLL

#ifdef _USE_DTAPIDLL
	#ifdef DTAPIDLL_EXPORTS
	#define DTAPI_API __declspec(dllexport)
	#else
	#define DTAPI_API __declspec(dllimport)
	#endif
#else
	// Not using the DLL so define DTAPI_API as empty
	#define DTAPI_API
#endif

//-.-.-.-.-.-.-.-.-.-.-.-.- Additional Libraries to be Linked In -.-.-.-.-.-.-.-.-.-.-.-.-
#ifndef _USE_DTAPIDLL
	#pragma comment(lib, "iphlpapi.lib")
	#pragma comment(lib, "ws2_32.lib")

	#ifndef _DTAPI_DISABLE_AUTO_LINK
		// Are we using multi-threaded-DLL or static runtime libraries?
		#ifdef _DLL
			#pragma comment(lib, "DTAPIMD.lib")	// Link to DLL runtime version (/MD)
		#else
			#pragma comment(lib, "DTAPI.lib")	// Link to static runtime version (/MT)
		#endif
	#endif
#else
	#ifndef DTAPIDLL_EXPORTS
#ifdef _WIN64
#else
		#pragma comment(lib, "DTAPIDLL.lib")	// DLL import lib
#endif
	#else
		#pragma comment(lib, "iphlpapi.lib")
		#pragma comment(lib, "ws2_32.lib")
	#endif
#endif


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DTAPI Version -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_VERSION_MAJOR		3
#define  DTAPI_VERSION_MINOR		6
#define  DTAPI_VERSION_BUGFIX		0
#define  DTAPI_VERSION_BUILD		88


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Includes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#include <windows.h>			// For windows type HANDLE
#include <map>


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DTAPI Support Types +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

//if namespace usage is required
#ifdef _USE_NAMESPACE
namespace DtApi {
#endif

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Elementary Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
typedef unsigned long  DTAPI_RESULT;


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtDeviceDesc -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure describing a device.
//
struct DtDeviceDesc {	
	int  m_Category;			// Device category
	__int64  m_Serial;			// Unique serial number of the device
	int  m_PciBusNumber;		// PCI-bus number
	int  m_SlotNumber;			// PCI-slot number
	int  m_UsbAddress;			// USB address
	int  m_TypeNumber;			// Device type number
	int  m_DeviceId;			// Device ID
	int  m_VendorId;			// Vendor ID
	int  m_SubsystemId;			// Subsystem
	int  m_SubVendorId;			// Subsystem Vendor ID
	int  m_NumHwFuncs;			// #Hardware funtions hosted by device
	int  m_FirmwareVersion;		// Firmware version
	int  m_FirmwareVariant;		// Firmware variant
	int  m_NumDtInpChan;		// Number of input channels 
	int  m_NumDtOutpChan;		// Number of output channels 
	int  m_NumPorts;			// Number of physical ports
};

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Device Categories -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
// NOTE: Category values 32-63 are reserved for use in DTAPIplus
#define  DTAPI_CAT_PCI			0
#define  DTAPI_CAT_USB			1
#define  DTAPI_CAT_NDIS			2			// Not a real device category (intended for
											// getting  driver version use)

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtHwFuncDesc -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
//
// Structure describing a hardware function.
//
struct DtHwFuncDesc {
	DtDeviceDesc  m_DvcDesc;	// Device descriptor
	int  m_ChanType;			// Channel type (OR-able)
	int  m_StreamType;			// Transport-Stream type
	int  m_Flags;				// Capability flags (OR-able)
	int  m_IndexOnDvc;			// Index of hardware function
	int  m_Port;				// Physical port number
	unsigned char m_Ip[4];		// IP address (only valid for IP functions)
	unsigned char m_MacAddr[6];	// MAC address (only valid for IP functions)
};

//-.-.-.-.-.-.-.-.-.-.-.-.-.- Hardware-Function Channel Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_CHAN_OUTPUT		1
#define  DTAPI_TS_OUTPUT		1			// Obsolete, for backward compatibility
#define  DTAPI_CHAN_INPUT		2
#define  DTAPI_TS_INPUT			2			// Obsolete, for backward compatibility
#define  DTAPI_CHAN_DBLBUF		4

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Hardware-Function Stream Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_ASI_SDI			1
#define  DTAPI_TS_MOD			2
#define  DTAPI_TS_OVER_IP		3
#define  DTAPI_TS_SPI			4

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Hardware-Function Capabilities -.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_CAP_ASI			0x00000001
#define  DTAPI_CAP_BIDIR		0x00000002
#define  DTAPI_CAP_DBLBUF		0x00000004
#define  DTAPI_CAP_SDI			0x00000008
#define  DTAPI_CAP_FAILSAFE		0x00000010
#define  DTAPI_CAP_ADJLVL		0x10000000		// Adjustable output level

// Modulation-related
#define  DTAPI_CAP_DTMB			0x00004000
#define  DTAPI_CAP_DVBS			0x00001000
#define  DTAPI_CAP_DVBS2		0x00002000
#define  DTAPI_CAP_DVBT			0x00000010
#define  DTAPI_CAP_ISDBT		0x00000020
#define  DTAPI_CAP_QAM_A		0x00000100
#define  DTAPI_CAP_QAM_B		0x00000200
#define  DTAPI_CAP_QAM_C		0x00000400
#define  DTAPI_CAP_VSB			0x00010000

#define  DTAPI_CAP_8VSB			0x00010000		// LEGACY, replaced by DTAPI_CAP_VSB
#define  DTAPI_CAP_OFDM			0x00000010		// LEGACY, replaced by DTAPI_CAP_DVBT
#define  DTAPI_CAP_QPSK			0x00001000		// LEGACY, replaced by DTAPI_CAP_DVBS

#define  DTAPI_CAP_VHF			0x00100000		// VHF-band 47-470MHz
#define  DTAPI_CAP_UHF			0x00200000		// UHF-band 400-862MHz
#define  DTAPI_CAP_LBAND		0x00400000		// L-band 950-2150MHz
#define  DTAPI_CAP_IF			0x00800000		// IF output
#define  DTAPI_CAP_DIGIF		0x01000000		// Digital IF output

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- String Conversions Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

// Device
// Device type number (e.g. "DTA-100", "DTA-102")
#define DTAPI_DVC2STR_TYPE_NMB			0	
// Device type number + location (e.g. "DTA-100 in slot 5");
#define DTAPI_DVC2STR_TYPE_AND_LOC		1

// Hardware Function
// Device type number (e.g. "DTA-100", "DTA-102")
#define DTAPI_HWF2STR_TYPE_NMB			0	
// Device type number + port (e.g. "DTA-124 port 1")
#define DTAPI_HWF2STR_TYPE_AND_PORT		1
// Device type number + location (e.g. "DTA-100 in slot 5");
#define DTAPI_HWF2STR_TYPE_AND_LOC		2
// Interface type (e.g. "DVB-ASI" or "DVB-C")
#define DTAPI_HWF2STR_ITF_TYPE			3
// Short version of interface type (e.g. "ASI" instead "DVB-ASI")
#define DTAPI_HWF2STR_ITF_TYPE_SHORT	4


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtTsIpPars -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure for storing parameters related to the transmission of Transport Streams
// over IP
//
struct DtTsIpPars {
	unsigned char  m_Ip[4];			// IP Address
	unsigned short  m_Port;			// IP port
	unsigned char  m_SrcFltIp[4];	// Source filter: IP address
	unsigned short  m_SrcFltPort;	// Source filter: IP port 
	int  m_MulticastTtl;			// TTL for multicast Tx
	int  m_NumTpPerIp;				// #TPs per IP packet
	int  m_Protocol;				// Protocol: UDP/RTP
	int  m_DiffServ;				// Differentiated Services
	int  m_FecMode;					// Error correction mode
	int  m_FecNumRows;				// ‘D?= #rows in FEC matrix
	int  m_FecNumCols;				// ‘L?= #columns in FEC matrix
	int  m_Flags;					// Optional controls/status flags
};


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtTsIpStat -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure for retrieving Ip statistics from drivers
// Statistics will be reset after read.
//
struct DtTsIpStat {
	unsigned int  m_TotNumIpPackets;
	unsigned int  m_LostIpPacketsBeforeFec;		// #Lost Packets before FEC 
	unsigned int  m_LostIpPacketsAfterFec;		// #Lost Packets after FEC
	unsigned int  m_LostIpPacketsBeforeSort;	// #Lost Packets before RTP sorting
	unsigned int  m_LostIpPacketsAfterSort;		// #Lost Packets after RTP sorting
};

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtRawIpHeader -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
//
// Header placed infront of all Ip Packets when DTAPI_RXMODE_IPRAW mode is used
//
struct DtRawIpHeader {
	unsigned short  m_Tag;			// 0x44A0h = ‘D?60
	unsigned short  m_Length;		// IP Packet Length;
	unsigned long  m_TimeStamp;		// Timestamp
};

//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtDevice +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Class to represent a DEKTEC Device.
//
class DTAPI_API  DtDevice {

	// Public attributes
public:
	DtDeviceDesc  m_DvcDesc;		// Device Descriptor
	DtHwFuncDesc*  m_pHwf;			// Hardware functions, filled after Attach

	// Public access functions
public:
	int  Category(void)			{ return m_DvcDesc.m_Category; }
	int  FirmwareVersion(void)	{ return m_DvcDesc.m_FirmwareVersion; }
	int  TypeNumber(void)		{ return m_DvcDesc.m_TypeNumber; }
	bool IsAttached(void)		{ return m_Attached; }

	// Public member functions
public:
	DTAPI_RESULT  AttachToSerial(__int64 SerialNumber);
	DTAPI_RESULT  AttachToSlot(int PciBusNumber, int SlotNumber);
	DTAPI_RESULT  AttachToType(int TypeNumber, int DeviceNo=0);
	DTAPI_RESULT  Detach(void);
	DTAPI_RESULT  GetDescriptor(DtDeviceDesc& DvcDesc);
	DTAPI_RESULT  GetDeviceDriverVersion(int& DriverVersionMajor,
										 int& DriverVersionMinor);
	DTAPI_RESULT  GetDeviceDriverVersion(
								int& DriverVersionMajor, int& DriverVersionMinor,
								int& DriverVersionBugFix, int& DriverVersionBuild);
	DTAPI_RESULT  GetFirmwareVersion(int& FirmwareVersion);
	DTAPI_RESULT  GetIoConfig(int Port, int& IoConfig);
	DTAPI_RESULT  GetIoConfig(int Port, int& IoConfig, int& ParXtra);
	DTAPI_RESULT  GetNwSpeed(int Port, bool& Enable, int& Speed);
	DTAPI_RESULT  GetRefClkCnt(int& RefClkCnt);
	DTAPI_RESULT  GetRefClkCnt(int& RefClkCnt, int& RefClkFreqHz);
	DTAPI_RESULT  GetRefClkFreq(int&  RefClkFreqHz);
	DTAPI_RESULT  GetUsbSpeed(int& UsbSpeed);
	DTAPI_RESULT  HwFuncScan(int NumEntries, int& NumEntriesResult,
							 DtHwFuncDesc* pHwFuncs);
	DTAPI_RESULT  I2CRead(int DvcAddr, char* pBuffer, int NumBytesToRead);
	DTAPI_RESULT  I2CWrite(int DvcAddr, char* pBuffer, int NumBytesToWrite);
	DTAPI_RESULT  LedControl(int LedControl);
	DTAPI_RESULT  SetIoConfig(int Port, int IoConfig, int ParXtra=-1);
	DTAPI_RESULT  SetNwSpeed(int Port, bool Enable, int Speed);
	DTAPI_RESULT  VpdDelete(char* pTag);
	DTAPI_RESULT  VpdDelete(wchar_t* pTag);
	DTAPI_RESULT  VpdRead(char* pTag, char* pVpdItem);
	DTAPI_RESULT  VpdRead(wchar_t* pTag, wchar_t* pVpdItem);
	DTAPI_RESULT  VpdRead(char*  pTag, char*  pVpdItem, int&  ItemSize);
	DTAPI_RESULT  VpdRead(wchar_t*  pTag, char*  pVpdItem, int&   ItemSize);
	DTAPI_RESULT  VpdWrite(char* pTag, char* pVpdItem);
	DTAPI_RESULT  VpdWrite(wchar_t* pTag, wchar_t* pVpdItem);
	DTAPI_RESULT  VpdWrite(char* pTag, char* pVpdItem, int ItemSize);
	DTAPI_RESULT  VpdWrite(wchar_t* pTag, char* pVpdItem, int ItemSize);

	// Encapsulated data
public:
	bool  m_Attached;				// Attached-to-hardware flag
	HANDLE  m_hDriver;				// Handle to PCI-card's device driver
	int*  m_pGenRegs;				// General registers

	// Constructor, destructor
public:
	DtDevice();
	virtual ~DtDevice();

	// Friends
	friend class  DtInpChannel;
	friend class  DtOutpChannel;
	friend class  TsInpChannel;
	friend class  TsOutpChannel;
};


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtDevice Constants -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- I/O Configuration -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_IOCONFIG_NOTSUP		0
#define  DTAPI_IOCONFIG_INPUT		1
#define  DTAPI_IOCONFIG_OUTPUT		2
#define  DTAPI_IOCONFIG_DBLBUF		3
#define  DTAPI_IOCONFIG_IP			4
#define  DTAPI_IOCONFIG_FAILSAFE	5

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Support Flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
// Used in DtInpChannel and DtOutpChannel
#define  DTAPI_SUPPORTS_TS			1
#define  DTAPI_SUPPORTS_SDI			2


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtInpChannel +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Class to represent an input channel.
//
class DTAPI_API  DtInpChannel {

	friend class DtLoopWorker;		// Allow access to protected members

	// Public attributes
public:
	DtHwFuncDesc  m_HwFuncDesc;		// Hardware function descriptor

	// Public access functions
public:
	int  Category(void)			{ return m_HwFuncDesc.m_DvcDesc.m_Category; }
	int  FirmwareVersion(void)	{ return m_HwFuncDesc.m_DvcDesc.m_FirmwareVersion; }
	int  GetChannelType(void)	{ return m_HwFuncDesc.m_ChanType; }
	int  GetIndexOnDevice(void)	{ return m_HwFuncDesc.m_IndexOnDvc; }
	bool IsAttached(void)		{ return (m_pOpaq != NULL); }
	int  TypeNumber(void)		{ return m_HwFuncDesc.m_DvcDesc.m_TypeNumber; }

public:
	DTAPI_RESULT  Attach(DtDevice* pDtDvc, int InpIndex=0, bool ProbeOnly=false);
	DTAPI_RESULT  AttachToPort(DtDevice* pDtDvc, int Port, bool ProbeOnly=false);
	DTAPI_RESULT  ClearFifo(void);
	DTAPI_RESULT  ClearFlags(int Latched);
	DTAPI_RESULT  Detach(int DetachMode);
	DTAPI_RESULT  Equalise(int EqualiserSetting);
	DTAPI_RESULT  GetDescriptor(DtHwFuncDesc& HwFunDesc);
	DTAPI_RESULT  GetDemodControl(int& ModType, int& ParXtra0, int& ParXtra1,
								  int& ParXtra2);
	DTAPI_RESULT  GetDemodStatus(int& FLock, int& RLock, int& SNR);
	DTAPI_RESULT  GetFifoLoad(int& FifoLoad);
	DTAPI_RESULT  GetFlags(int& Flags, int& Latched);
	DTAPI_RESULT  GetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  GetIpStat(DtTsIpStat* pTsIpStat);
	DTAPI_RESULT  GetMaxFifoSize(int& MaxFifoSize);
	DTAPI_RESULT  GetReceiveByteCount(int& ByteCount);
	DTAPI_RESULT  GetRxControl(int& RxControl);
	DTAPI_RESULT  GetRxMode(int& RxMode);
	DTAPI_RESULT  GetStatistics(int& ViolCount);
	DTAPI_RESULT  GetStatus(int& PacketSize, int& NumInv, int& ClkDet,
							int& AsiLock, int& RateOk, int& AsiInv);
	DTAPI_RESULT  GetTargetId(int& Present, int& TargetId);
	DTAPI_RESULT  GetTsRateBps(int& TsRate);
	DTAPI_RESULT  GetTunerFrequency(__int64& FreqHz);
	DTAPI_RESULT  LedControl(int LedControl);
	DTAPI_RESULT  PolarityControl(int Polarity);
	DTAPI_RESULT  Read(char* pBuffer, int NumBytesToRead);
	DTAPI_RESULT  Read(char* pBuffer, int NumBytesToRead, int Timeout);
	DTAPI_RESULT  ReadDirect(char* pBuffer,	int NumBytesToRead, int& NumBytesRead);
	DTAPI_RESULT  ReadFrame(unsigned long* pFrame, int& FrameSize, int TimeOut=-1);
	DTAPI_RESULT  ReadUsingDma(char* pBuffer, int NumBytesToRead);
	DTAPI_RESULT  Reset(int ResetMode);
	DTAPI_RESULT  SetDemodControl(int ModType, int ParXtra0, int ParXtra1,
								  int ParXtra2);
	DTAPI_RESULT  SetLoopBackMode(int Mode);
	DTAPI_RESULT  SetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  SetPower(int Power);
	DTAPI_RESULT  SetRxControl(int RxControl);
	DTAPI_RESULT  SetRxMode(int RxMode);
	DTAPI_RESULT  SetTunerFrequency(__int64 FreqHz);
	DTAPI_RESULT  TuneChannel(int Band, int Channel, __int64& FreqHz);
	DTAPI_RESULT  WriteLoopBackData(char* pBuffer, int NumBytesToWrite);

protected:
	bool  IsTsSupported()  { return ((m_Support & DTAPI_SUPPORTS_TS)!=0); }
	bool  IsSdiSupported() { return ((m_Support & DTAPI_SUPPORTS_SDI)!=0); }

	// Encapsulated data
protected:
	void*  m_pOpaq;					// Opaque data; doubles as 'Attached' flag
	int  m_Support;					// Channel capabilities supported by this object

	// Constructor, destructor
public:
	DtInpChannel();
	virtual ~DtInpChannel();
};
typedef DtInpChannel Dtu234;		// For backwards compatibility

// Transport-Stream channel is a specialised version of DtInpChannel
class DTAPI_API TsInpChannel : public DtInpChannel {
	// Constructor, destructor
public:
	TsInpChannel();
	virtual ~TsInpChannel();
};

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtInpChannel Constants -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

// Feature not supported
#define  DTAPI_NOT_SUPPORTED		-1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.- ASI Polarity-Control Status -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_ASIINV_NORMAL		0
#define  DTAPI_ASIINV_INVERT		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- ASI Input-Clock Lock -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_ASI_NOLOCK			0
#define  DTAPI_ASI_INLOCK			1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Clock Detector -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_CLKDET_FAIL			0
#define  DTAPI_CLKDET_OK			1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Input Rate Ok -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_INPRATE_LOW			0
#define  DTAPI_INPRATE_OK			1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- #Invalid Bytes per Packet -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_NUMINV_NONE			0
#define  DTAPI_NUMINV_16			1
#define  DTAPI_NUMINV_OTHER			2

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Packet Size -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_PCKSIZE_INV			0
#define  DTAPI_PCKSIZE_188			2
#define  DTAPI_PCKSIZE_204			3

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- SDI Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_SDIMODE_INV			0
#define  DTAPI_SDIMODE_525			1
#define  DTAPI_SDIMODE_625			2

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Receive Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_RXCTRL_IDLE			0
#define  DTAPI_RXCTRL_RCV			1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Receive Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_RXMODE_ST188			0
#define  DTAPI_RXMODE_ST204			1
#define  DTAPI_RXMODE_STMP2			2
#define  DTAPI_RXMODE_STRAW			3
#define  DTAPI_RXMODE_STTRP			4
#define  DTAPI_RXMODE_IPRAW			5
#define  DTAPI_RX_TIMESTAMP			0x008	// Flag: can be OR-ed with other modes
#define  DTAPI_RXMODE_SDI			0x010	// Flag: can be OR-ed with other modes
#define  DTAPI_RXMODE_SDI_FULL		0x010	
#define  DTAPI_RXMODE_SDI_ACTVID	0x011	
#define  DTAPI_RXMODE_SDI_10B		0x020	// Flag: can be OR-ed with other modes
#define  DTAPI_RXMODE_SDI_HUFFMAN	0x080	// Flag: SDI Huffman compression on


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Demodulation status flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

// FEC-lock
#define  DTAPI_DEMOD_FECLOCK_FAIL	0
#define  DTAPI_DEMOD_FECLOCK_OK		1
// Receiver-lock
#define  DTAPI_DEMOD_RCVLOCK_FAIL	0
#define  DTAPI_DEMOD_RCVLOCK_OK		1	

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Channel bands -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

#define  DTAPI_BAND_BROADCAST_ONAIR	1
#define  DTAPI_BAND_FCC_CABLE		2
#define  DTAPI_BAND_IRC				3
#define  DTAPI_BAND_HRC				4


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtIsdbtPars +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

// Parameters per hierarchical ISDB-T layer
struct DtIsdbtLayerPars {
    int  m_NumSegments;			// Number of segments
    int  m_Modulation;			// Modulation type
    int  m_CodeRate;			// Code rate
    int  m_TimeInterleave;		// Time interleaving
	// Derived:
	int  m_BitRate;				// Bits per second assuming 6 MHz channel
};

// ISDB-T parameters including per-layer parameters
struct DtIsdbtPars {
	bool  m_DoMux;				// Hierarchical multiplexing yes/no
	int  m_BType;				// Broadcast type
	int  m_Mode;				// Transmission mode
	int  m_Guard;				// Guard interval
	int  m_PartialRx;			// Partal reception
	int  m_Emergency;			// Switch-on control for emergency broadcast
	int	 m_IipPid;				// PID used for multiplexing IIP packet
	DtIsdbtLayerPars  m_LayerPars[3];	// Layer-A/B/C parameters
	std::map<int, int>  m_Pid2Layer;	// PID-to-layer mapping
	int  m_LayerOther;			// Other PIDs are mapped to this layer
	// Derived:
	bool  m_Valid;				// The parameter set is valid
	int	 m_TotalBitrate;		// Bitrate of entire stream
	int	 m_SampleRate;			// 0=512/63MHz, 1=64/63MHz, 2=128/63MHz

	// Member function
	DTAPI_RESULT  CheckValidity(int& ResultCode);
	DTAPI_RESULT  ComputeRates(void);
	DTAPI_RESULT  RetrieveParsFromTs(char* pBuffer, int NumBytes);

	static int  BType2NumSegm(int BType);

	// Operators
	bool  operator == (DtIsdbtPars& Rhs);

	// Constructor
	DtIsdbtPars() : m_SampleRate(0) {}
};

// PID-to-layer mapping
#define  DTAPI_ISDBT_LAYER_A		1
#define  DTAPI_ISDBT_LAYER_B		2
#define  DTAPI_ISDBT_LAYER_C		4

#define  DTAPI_ISDBT_BTYPE_TV		0		// 13-segment TV broadcast
#define  DTAPI_ISDBT_BTYPE_RAD1		1		// 1-segment radio broadcast
#define  DTAPI_ISDBT_BTYPE_RAD3		2		// 3-segment radio broadcast

#define  DTAPI_ISDBT_GUARD_1_32		0
#define  DTAPI_ISDBT_GUARD_1_16		1
#define  DTAPI_ISDBT_GUARD_1_8		2
#define  DTAPI_ISDBT_GUARD_1_4		3

#define  DTAPI_ISDBT_MOD_DQPSK		0
#define  DTAPI_ISDBT_MOD_QPSK		1
#define  DTAPI_ISDBT_MOD_QAM16		2
#define  DTAPI_ISDBT_MOD_QAM64		3

#define  DTAPI_ISDBT_RATE_1_2		0
#define  DTAPI_ISDBT_RATE_2_3		1
#define  DTAPI_ISDBT_RATE_3_4		2
#define  DTAPI_ISDBT_RATE_5_6		3
#define  DTAPI_ISDBT_RATE_7_8		4

// Result codes for DtIsdbtPars::CheckValidity
#define  DTAPI_ISDBT_OK				0
#define  DTAPI_ISDBT_E_NSEGM		1
#define  DTAPI_ISDBT_E_PARTIAL		2


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtOutpChannel +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Class to represent a transport-stream output channel.
//
class DTAPI_API DtOutpChannel {

	friend class DtLoopWorker;	// Allow access to protected members

	// Public attributes
public:
	DtHwFuncDesc  m_HwFuncDesc;	// Hardware function descriptor

	// Public access functions
public:
	int  Category(void)			{ return m_HwFuncDesc.m_DvcDesc.m_Category; }
	int  FirmwareVersion(void)	{ return m_HwFuncDesc.m_DvcDesc.m_FirmwareVersion; }
	int  GetChannelType(void)	{ return m_HwFuncDesc.m_ChanType; }
	int  GetIndexOnDevice(void)	{ return m_HwFuncDesc.m_IndexOnDvc; }
	bool IsAttached(void)		{ return (m_pOpaq != NULL); }
	int  TypeNumber(void)		{ return m_HwFuncDesc.m_DvcDesc.m_TypeNumber; }

public:
	DTAPI_RESULT  Attach(DtDevice* pDtDvc, int OutpIndex=0, bool ProbeOnly=false);
	DTAPI_RESULT  AttachToPort(DtDevice* pDtDvc, int Port, bool ProbeOnly=false);
	DTAPI_RESULT  ClearFifo(void);
	DTAPI_RESULT  ClearFlags(int Latched);
	DTAPI_RESULT  Detach(int DetachMode);
	DTAPI_RESULT  GetDescriptor(DtHwFuncDesc& HwFunDesc);
	DTAPI_RESULT  GetExtClkFreq(int& ExtClkFreq);
	DTAPI_RESULT  GetFailsafeAlive(bool& Alive);
	DTAPI_RESULT  GetFailsafeConfig(bool& Enable, int& Timeout);
	DTAPI_RESULT  GetFifoLoad(int& FifoLoad);
	DTAPI_RESULT  GetFifoSize(int& FifoSize);
	DTAPI_RESULT  GetFlags(int& Status, int& Latched);
	DTAPI_RESULT  GetMaxFifoSize(int& MaxFifoSize);
	DTAPI_RESULT  GetModControl(int& ModType, int& CodeRate, int& ParXtra1, int& ParXtra2);
	DTAPI_RESULT  GetOutputLevel(int& LeveldBm);
	DTAPI_RESULT  GetRfControl(__int64& RfFreq, int& LockStatus);
	DTAPI_RESULT  GetTargetId(int& Present, int& TargetId);
	DTAPI_RESULT  GetTransmitByteCount(int& ByteCount);
	DTAPI_RESULT  GetTsRateBps(int& ClockGenMode, int& TsRate);
	DTAPI_RESULT  GetTsRateBps(int& TsRate);
	DTAPI_RESULT  GetTxControl(int& TxControl);
	DTAPI_RESULT  GetTxMode(int& TxPacketMode, int& TxStuffMode);
	DTAPI_RESULT  ReadLoopBackData(char* pBuffer, int BytesToRead);
	DTAPI_RESULT  Reset(int ResetMode);
	DTAPI_RESULT  SetFailsafeConfig(bool Enable, int Timeout = 0);
	DTAPI_RESULT  SetFailsafeAlive();
	DTAPI_RESULT  SetFifoSize(int FifoSize);
	DTAPI_RESULT  SetFifoSizeMax(void);
	DTAPI_RESULT  SetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  SetLoopBackMode(int Mode);
	DTAPI_RESULT  SetModControl(DtIsdbtPars&);
	DTAPI_RESULT  SetModControl(int ModType, int CodeRate, int ParXtra1, int ParXtra2);
	DTAPI_RESULT  SetOutputLevel(int LeveldBm);
	DTAPI_RESULT  SetPower(int Power);
	DTAPI_RESULT  SetRfControl(__int64 RfFreq);
	DTAPI_RESULT  SetRfMode(int RfMode);
    DTAPI_RESULT  SetTsRateBps(int ClockGenMode, int TsRate);
	DTAPI_RESULT  SetTsRateBps(int TsRate);
	DTAPI_RESULT  SetTxControl(int TxControl);
	DTAPI_RESULT  SetTxMode(int TxPacketMode, int TxStuffMode);
	DTAPI_RESULT  SetTxPolarity(int TxPolarity);
	DTAPI_RESULT  Write(char* pBuffer, int NumBytesToWrite);
	DTAPI_RESULT  WriteDirect(char* pBuffer, int NumBytesToWrite);
	DTAPI_RESULT  WriteUsingDma(char* pBuffer, int NumBytesToWrite);

protected:
	bool  IsTsSupported()  { return ((m_Support & DTAPI_SUPPORTS_TS)!=0); }
	bool  IsSdiSupported() { return ((m_Support & DTAPI_SUPPORTS_SDI)!=0); }

	// Encapsulated data
protected:
	void*  m_pOpaq;				// Opaque data; doubles as 'Attached' flag
	int  m_Support;				// Channel capabilities supported by this object:
								// DTAPI_SUPPORTS_TS and/or DTAPI_SUPPORTS_SDI

	// Constructor, destructor
public:
	DtOutpChannel();
	virtual ~DtOutpChannel();
};

// Transport-Stream output channel is a specialised version of DtOutpChannel
class DTAPI_API TsOutpChannel : public DtOutpChannel {
	// Constructor, destructor
public:
	TsOutpChannel();
	virtual ~TsOutpChannel();
};

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtOutpChannel Constants -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Clock-Generator Modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXCLOCK_INTERNAL		0
#define  DTAPI_TXCLOCK_EXTERNAL		1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Detach Mode flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_INSTANT_DETACH		1
#define  DTAPI_ZERO_OUTPUT			2
#define  DTAPI_TRISTATE_OUTPUT		4
#define  DTAPI_WAIT_UNTIL_SENT		8

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Equaliser Settings -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_EQUALISER_OFF		0
#define  DTAPI_EQUALISER_ON			1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- LED Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_LED_OFF				0
#define  DTAPI_LED_GREEN			1
#define  DTAPI_LED_RED				2
#define  DTAPI_LED_YELLOW			3
#define  DTAPI_LED_HARDWARE			4

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Loop-Back Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NO_LOOPBACK			0
#define  DTAPI_LOOPBACK_MODE		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Upconverter RF modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define DTAPI_UPCONV_NORMAL			0
#define DTAPI_UPCONV_MUTE			1
#define DTAPI_UPCONV_CW				2
#define DTAPI_UPCONV_CWI			3
#define DTAPI_UPCONV_CWQ			4

#define DTAPI_UPCONV_SPECINV		0x100	// Can be OR-ed with other RF modes


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_MOD_DVBS_QPSK		0		// Native DVB-S on DTA-107
#define  DTAPI_MOD_DVBS_BPSK		1
#define  DTAPI_MOD_QAM4				3
#define  DTAPI_MOD_QAM16			4
#define  DTAPI_MOD_QAM32			5
#define  DTAPI_MOD_QAM64			6
#define  DTAPI_MOD_QAM128			7
#define  DTAPI_MOD_QAM256			8
#define  DTAPI_MOD_DVBT				9	
#define  DTAPI_MOD_ATSC				10
#define  DTAPI_MOD_ISDBT			12
#define  DTAPI_MOD_IQDIRECT			15
#define  DTAPI_MOD_DVBS_QPSK_SOFT	16		// Soft DVB-S for DTA-107S2
#define  DTAPI_MOD_DVBS2_QPSK		32
#define  DTAPI_MOD_DVBS2_8PSK		33
#define  DTAPI_MOD_DTMB				48

#define  DTAPI_MOD_1_2				0		// Code rate 1/2
#define  DTAPI_MOD_2_3				1		// Code rate 2/3
#define  DTAPI_MOD_3_4				2		// Code rate 3/4
#define  DTAPI_MOD_4_5				3		// Code rate 4/5
#define  DTAPI_MOD_5_6				4		// Code rate 5/6
#define  DTAPI_MOD_6_7				5		// Code rate 6/7
#define  DTAPI_MOD_7_8				6		// Code rate 7/8
#define  DTAPI_MOD_1_4				7		// Code rate 1/4
#define  DTAPI_MOD_1_3				8		// Code rate 1/3
#define  DTAPI_MOD_2_5				9		// Code rate 2/5
#define  DTAPI_MOD_3_5				10		// Code rate 3/5
#define  DTAPI_MOD_8_9				11		// Code rate 8/9
#define  DTAPI_MOD_9_10				12		// Code rate 9/10

#define  DTAPI_MOD_J83_MSK			0x0003
#define  DTAPI_MOD_J83_A			2		// J.83 annex A (DVB-C)
#define  DTAPI_MOD_J83_B			3		// J.83 annex B (“American QAM?
#define  DTAPI_MOD_J83_C			1		// J.83 annex C (“Japanese QAM?
#define  DTAPI_MOD_DVBC_SOFT		0x100	// Soft DVB-C flag

// LEGACY defintions
#define  DTAPI_MOD_QPSK				0
#define  DTAPI_MOD_BPSK				1
#define  DTAPI_MOD_OFDM				9
#define  DTAPI_MOD_ROLLOFF_13		1		// Roll-off factor 13% (LEGACY)
#define  DTAPI_MOD_ROLLOFF_15		2		// Roll-off factor 15% (LEGACY)

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DTMB) -.-.-.-.-.-.-.-.-.-.-.-.-.-.

// ParXtra1
#define  DTAPI_MOD_DTMB_QAM4NR		0x00000000	// 4-QAM-NR
#define  DTAPI_MOD_DTMB_QAM4		0x00000001	// 4-QAM
#define  DTAPI_MOD_DTMB_QAM16		0x00000002	// 16-QAM
#define  DTAPI_MOD_DTMB_QAM32		0x00000003	// 32-QAM
#define  DTAPI_MOD_DTMB_QAM64		0x00000004	// 64-QAM
#define  DTAPI_MOD_DTMB_CO_MSK		0x00000007	// Constellation Mask

#define  DTAPI_MOD_DTMB_0_4			0x00000000	// FEC Code Rate 0.4
#define  DTAPI_MOD_DTMB_0_6			0x00000010	// FEC Code Rate 0.6
#define  DTAPI_MOD_DTMB_0_8			0x00000020	// FEC Code Rate 0.8
#define  DTAPI_MOD_DTMB_RATE_MSK	0x00000030	// FEC Code Rate Mask

#define  DTAPI_MOD_DTMB_PN420		0x00000000	// PN420
#define  DTAPI_MOD_DTMB_PN595		0x00000100	// PN595
#define  DTAPI_MOD_DTMB_PN945		0x00000200	// PN945
#define  DTAPI_MOD_DTMB_PN_MSK		0x00000300	// Frame-Header-Mode Mask

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DVB-S2) -.-.-.-.-.-.-.-.-.-.-.-.-.-.

// ParXtra1
#define  DTAPI_MOD_S2_NOPILOTS		0			// Pilots disabled
#define  DTAPI_MOD_S2_PILOTS		1			// Pilots enabled
#define  DTAPI_MOD_S2_PILOTS_MSK	1			// Mask for pilots field

#define  DTAPI_MOD_S2_SHORTFRM		2			// Short FECFRAME
#define  DTAPI_MOD_S2_LONGFRM		0			// Long FECFRAME
#define  DTAPI_MOD_S2_FRM_MSK		2			// Mask for FECFRAME field

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DVB-T) -.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_MOD_DVBT_5MHZ		0x00000001
#define  DTAPI_MOD_DVBT_6MHZ		0x00000002
#define  DTAPI_MOD_DVBT_7MHZ		0x00000003
#define  DTAPI_MOD_DVBT_8MHZ		0x00000004
#define  DTAPI_MOD_DVBT_BW_MSK		0x0000000F

#define  DTAPI_MOD_DVBT_QPSK		0x00000010
#define  DTAPI_MOD_DVBT_QAM16		0x00000020
#define  DTAPI_MOD_DVBT_QAM64		0x00000030
#define  DTAPI_MOD_DVBT_CO_MSK		0x000000F0

#define  DTAPI_MOD_DVBT_G_1_32		0x00000100
#define  DTAPI_MOD_DVBT_G_1_16		0x00000200
#define  DTAPI_MOD_DVBT_G_1_8		0x00000300
#define  DTAPI_MOD_DVBT_G_1_4		0x00000400
#define  DTAPI_MOD_DVBT_GU_MSK		0x00000F00

#define  DTAPI_MOD_DVBT_INDEPTH		0x00001000
#define  DTAPI_MOD_DVBT_NATIVE		0x00002000
#define  DTAPI_MOD_DVBT_IL_MSK		0x0000F000

#define  DTAPI_MOD_DVBT_2K			0x00010000
#define  DTAPI_MOD_DVBT_4K			0x00020000
#define  DTAPI_MOD_DVBT_8K			0x00030000
#define  DTAPI_MOD_DVBT_MD_MSK		0x000F0000

#define  DTAPI_MOD_DVBT_S48_OFF		0x00000000
#define  DTAPI_MOD_DVBT_S48			0x00100000
#define  DTAPI_MOD_DVBT_S48_MSK		0x00100000

#define  DTAPI_MOD_DVBT_S49_OFF		0x00000000
#define  DTAPI_MOD_DVBT_S49			0x00200000
#define  DTAPI_MOD_DVBT_S49_MSK		0x00200000

#define  DTAPI_MOD_DVBT_ENA4849		0x00000000
#define  DTAPI_MOD_DVBT_DIS4849		0x00400000
#define  DTAPI_MOD_DVBT_4849_MSK	0x00400000

// DVB-T LEGACY
#define  DTAPI_MOD_OFDM_5MHZ		0x00000001
#define  DTAPI_MOD_OFDM_6MHZ		0x00000002
#define  DTAPI_MOD_OFDM_7MHZ		0x00000003
#define  DTAPI_MOD_OFDM_8MHZ		0x00000004
#define  DTAPI_MOD_OFDM_BW_MSK		0x0000000F
#define  DTAPI_MOD_OFDM_QPSK		0x00000010
#define  DTAPI_MOD_OFDM_QAM16		0x00000020
#define  DTAPI_MOD_OFDM_QAM64		0x00000030
#define  DTAPI_MOD_OFDM_CO_MSK		0x000000F0
#define  DTAPI_MOD_OFDM_G_1_32		0x00000100
#define  DTAPI_MOD_OFDM_G_1_16		0x00000200
#define  DTAPI_MOD_OFDM_G_1_8		0x00000300
#define  DTAPI_MOD_OFDM_G_1_4		0x00000400
#define  DTAPI_MOD_OFDM_GU_MSK		0x00000F00
#define  DTAPI_MOD_OFDM_INDEPTH		0x00001000
#define  DTAPI_MOD_OFDM_NATIVE		0x00002000
#define  DTAPI_MOD_OFDM_IL_MSK		0x0000F000
#define  DTAPI_MOD_OFDM_2K			0x00010000
#define  DTAPI_MOD_OFDM_4K			0x00020000
#define  DTAPI_MOD_OFDM_8K			0x00030000
#define  DTAPI_MOD_OFDM_MD_MSK		0x000F0000
#define  DTAPI_MOD_OFDM_S48_OFF		0x00000000
#define  DTAPI_MOD_OFDM_S48			0x00100000
#define  DTAPI_MOD_OFDM_S48_MSK		0x00100000
#define  DTAPI_MOD_OFDM_S49_OFF		0x00000000
#define  DTAPI_MOD_OFDM_S49			0x00200000
#define  DTAPI_MOD_OFDM_S49_MSK		0x00200000
#define  DTAPI_MOD_OFDM_ENA4849		0x00000000
#define  DTAPI_MOD_OFDM_DIS4849		0x00400000
#define  DTAPI_MOD_OFDM_4849_MSK	0x00400000

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Packet Transmit Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXMODE_188			0
#define  DTAPI_TXMODE_204			1
#define  DTAPI_TXMODE_ADD16			2
#define  DTAPI_TXMODE_RAW			3
#define  DTAPI_TXMODE_192			4		// Supported on DTA-102 Firmware Rev >= 2
#define  DTAPI_TXMODE_130			5		// Supported on DTA-102 Firmware Rev >= 7
#define  DTAPI_TXMODE_MIN16			6		// Supported on DTA-100 Firmware Rev >= 5
											// and          DTA-102 Firmware Rev >= 8
#define  DTAPI_TXMODE_IPRAW			7
#define  DTAPI_TXMODE_MASK			0x00F	// Mask for TxMode without burst flag
#define  DTAPI_TXMODE_BURST			0x010	// Can be OR-ed with one of 188/192/204/ADD16/RAW
#define	 DTAPI_TXMODE_SDI			0x040
#define  DTAPI_TXMODE_SDI_FULL		0x040
#define  DTAPI_TXMODE_SDI_ACTVID	0x041
#define	 DTAPI_TXMODE_SDI_10B		0x080	// Can be OR-ed with DTAPI_TXMODE_SDI
#define  DTAPI_TXMODE_SDI_525		0x000	// Can be OR-ed with DTAPI_TXMODE_SDI/10BIT
#define  DTAPI_TXMODE_SDI_625		0x100	// Can be OR-ed with DTAPI_TXMODE_SDI/10BIT
#define  DTAPI_TXMODE_SDI_HUFFMAN	0x200	// SDI huffman compression on

#define  DTAPI_TXSTUFF_MODE_OFF		0		// Stuff-mode is disabled
#define  DTAPI_TXSTUFF_MODE_ON		1		// Stuff-mode is enabled

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Transmit Polarity -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXPOL_NORMAL			0
#define  DTAPI_TXPOL_INVERTED		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Polarity Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_POLARITY_AUTO		0
#define  DTAPI_POLARITY_NORMAL		2
#define  DTAPI_POLARITY_INVERT		3

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Power Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_POWER_OFF			0
#define  DTAPI_POWER_ON				1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Reset Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_FIFO_RESET			0
#define  DTAPI_FULL_RESET			1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Status Flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_RX_DMA_PENDING		1
#define  DTAPI_RX_FIFO_OVF			2
#define  DTAPI_RX_SYNC_ERR			4
#define  DTAPI_RX_RATE_OVF			8
#define  DTAPI_RX_TARGET_ERR		16

#define  DTAPI_TX_DMA_PENDING		1
#define  DTAPI_TX_FIFO_UFL			2
#define  DTAPI_TX_SYNC_ERR			4
#define  DTAPI_TX_READBACK_ERR		8
#define  DTAPI_TX_TARGET_ERR		16
#define  DTAPI_TX_MUX_OVF			32

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Target Adapter Present -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NO_CONNECTION		0
#define  DTAPI_DVB_SPI_SINK			1		// For output channels
#define  DTAPI_DVB_SPI_SOURCE		1		// For input channels
#define  DTAPI_TARGET_PRESENT		2
#define  DTAPI_TARGET_UNKNOWN		3

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Transmit Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXCTRL_IDLE			0
#define  DTAPI_TXCTRL_HOLD			1
#define  DTAPI_TXCTRL_SEND			3

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- RF PLL lock status -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_RFPLL_NO_LOCK		0
#define  DTAPI_RFPLL_LOCK			1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- USB-Speed Modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_USB_FULL_SPEED		0
#define  DTAPI_USB_HIGH_SPEED		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- SDRAM sizes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_SDRAM_SIZE_8MB		0
#define  DTAPI_SDRAM_SIZE_16MB		1
#define  DTAPI_SDRAM_SIZE_32MB		2


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtLoop +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//
// DtLoop class represents an object that provides functionality to loop MPEG-2
// transport-streams from a DtInpChannel to a DtOutpChannel object
//
class DTAPI_API DtLoop {

	friend class DtLoopWorker;

	// Public access functions
public:
	bool  IsStarted();

public:
	DTAPI_RESULT  AttachToInput(DtInpChannel* pDtInp);
	DTAPI_RESULT  AttachToOutput(DtOutpChannel* pDtOutp);
	DTAPI_RESULT  Detach();
	DTAPI_RESULT  DetachFromInput();
	DTAPI_RESULT  DetachFromOutput();
	DTAPI_RESULT  SetStuffingMode(int Mode, int TsRate);
	DTAPI_RESULT  Start(bool Start=true);

	// Internal methods
protected:

	// Encapsulated data
protected:
	DtLoopWorker*  m_pWorker;	// Internal worker class
	
	// Constructor, destructor
public:
	DtLoop();
	virtual ~DtLoop();
};


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtSdi +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// The DtSdi class provides helper functions for processing SDI data
// 
class DTAPI_API DtSdi
{
	friend class DtSdiUtility;

	// Public access functions
public:
	DTAPI_RESULT ConvertFrame(unsigned long* pInFrame, int& InFrameSize, int InFrameFormat,
							  unsigned long* pOutFrame, int& OutFrameSize,
							  int OutFrameFormat);

	// Internal methods
protected:

	// Encapsulated data
protected:
	DtSdiUtility*  m_pSdiUtil;	// Internal utility class
	
	// Constructor, destructor
public:
	DtSdi();
	virtual ~DtSdi();
};

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Conversions format -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define DTAPI_SDI_FULL		0x01
#define DTAPI_SDI_ACTVID	0x02
#define DTAPI_SDI_HUFFMAN	0x04
#define DTAPI_SDI_625		0x08
#define DTAPI_SDI_525		0x10
#define DTAPI_SDI_8B		0x20
#define DTAPI_SDI_10B		0x40
#define DTAPI_SDI_16B		0x80

#define DTAPI_SDI_BIT_MASK	0xE0


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ Global DTAPI Functions +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

DTAPI_API DTAPI_RESULT  DtapiCheckDeviceDriverVersion(void);
DTAPI_API DTAPI_RESULT  DtapiCheckDeviceDriverVersion(int DeviceCategory);
DTAPI_API DTAPI_RESULT  DtapiDeviceScan(int NumEntries, int& NumEntriesResult,
										DtDeviceDesc* DvcDescArr);
DTAPI_API DTAPI_RESULT  DtDeviceDesc2String(DtDeviceDesc* pDvcDesc, int StringType, 
											char* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtDeviceDesc2String(DtDeviceDesc* pDvcDesc, int StringType, 
											wchar_t* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtapiDvbS2GetBitRate(int& RateNum, int& RateDen, int ModType,
											 int XtraPar0, int XtraPar1);
DTAPI_API DTAPI_RESULT  DtapiDtHwFuncDesc2String(DtHwFuncDesc* pHwFunc, int StringType, 
												 char* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtapiDtHwFuncDesc2String(DtHwFuncDesc* pHwFunc, int StringType, 
												 wchar_t* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtapiGetDeviceDriverVersion(int&, int&, int&, int&);
DTAPI_API DTAPI_RESULT  DtapiGetDeviceDriverVersion(int, int&, int&, int&, int&);
DTAPI_API DTAPI_RESULT  DtapiGetVersion(int& LibVersion, int& LibVersionMinor);
DTAPI_API DTAPI_RESULT  DtapiGetVersion(int& LibVersion, int& LibVersionMinor,
										int& LibVersionBugFix, int& LibVersionBuild);
DTAPI_API DTAPI_RESULT  DtapiHwFuncScan(int NumEntries, int& NumEntriesResult,
										DtHwFuncDesc* pHwFuncs);
DTAPI_API DTAPI_RESULT  DtapiInitDtTsIpParsFromIpString(DtTsIpPars& TsIpPars,
														const char* pDstIp,
														const char* pSrcIp);
DTAPI_API DTAPI_RESULT  DtapiInitDtTsIpParsFromIpString(DtTsIpPars& TsIpPars,
														const wchar_t* pDstIp,
														const wchar_t* pSrcIp);
DTAPI_API DTAPI_RESULT  DtapiIpAddr2Str(char* pStr, int StrLen, unsigned char* pIpAddr);
DTAPI_API DTAPI_RESULT  DtapiIpAddr2Str(wchar_t* pStr, int StrLen,
										unsigned char* pIpAddr);
DTAPI_API DTAPI_RESULT  DtapiStr2IpAddr(unsigned char* pIpAddr, const char* pStr);
DTAPI_API DTAPI_RESULT  DtapiStr2IpAddr(unsigned char* pIpAddr, const wchar_t* pStr);

DTAPI_API const char*  DtapiResult2Str(DTAPI_RESULT DtapiResult);


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ Return Codes +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// NOTE: ERROR CODES 0x1100-0x12FF ARE RESERVED FOR USE IN THE DTAPIplus

#define  DTAPI_OK					0
#define  DTAPI_OK_FAILSAFE			1
#define  DTAPI_E					0x1000
#define  DTAPI_E_ATTACHED			(DTAPI_E + 0)
#define  DTAPI_E_BUF_TOO_SMALL		(DTAPI_E + 1)
#define  DTAPI_E_DEV_DRIVER			(DTAPI_E + 2)
#define  DTAPI_E_EEPROM_FULL		(DTAPI_E + 3)
#define  DTAPI_E_EEPROM_READ		(DTAPI_E + 4)
#define  DTAPI_E_EEPROM_WRITE		(DTAPI_E + 5)
#define  DTAPI_E_EEPROM_FORMAT		(DTAPI_E + 6)
#define  DTAPI_E_FIFO_FULL			(DTAPI_E + 7)
#define  DTAPI_E_IN_USE				(DTAPI_E + 8)
#define  DTAPI_E_INVALID_BUF		(DTAPI_E + 9)
#define  DTAPI_E_INVALID_CGMODE		(DTAPI_E + 10)
#define  DTAPI_E_INVALID_FLAGS		(DTAPI_E + 11)
#define  DTAPI_E_INVALID_MODE		(DTAPI_E + 12)
#define  DTAPI_E_INVALID_RATE		(DTAPI_E + 13)
#define  DTAPI_E_INVALID_SIZE		(DTAPI_E + 14)
#define  DTAPI_E_KEYWORD			(DTAPI_E + 15)
#define  DTAPI_E_NO_DEVICE			(DTAPI_E + 16)
#define  DTAPI_E_NO_DTA_CARD		(DTAPI_E + 16)	// BACKWARD COMPATIBILITY
#define  DTAPI_E_NO_LOOPBACK		(DTAPI_E + 17)
#define  DTAPI_E_NO_SUCH_DEVICE		(DTAPI_E + 18)
#define  DTAPI_E_NO_SUCH_OUTPUT		(DTAPI_E + 19)
#define  DTAPI_E_NO_DT_OUTPUT		(DTAPI_E + 20)
#define  DTAPI_E_NO_TS_OUTPUT		(DTAPI_E + 20)
#define  DTAPI_E_NOT_ATTACHED		(DTAPI_E + 21)
#define  DTAPI_E_NOT_FOUND			(DTAPI_E + 22)
#define  DTAPI_E_NOT_SUPPORTED		(DTAPI_E + 23)
#define  DTAPI_E_PCICARD			(DTAPI_E + 24)	// BACKWARD COMPATIBILITY
#define  DTAPI_E_DEVICE				(DTAPI_E + 24)
#define  DTAPI_E_TOO_LONG			(DTAPI_E + 25)
#define  DTAPI_E_UNDERFLOW			(DTAPI_E + 26)
#define  DTAPI_E_NO_SUCH_INPUT		(DTAPI_E + 27)
#define  DTAPI_E_NO_DT_INPUT		(DTAPI_E + 28)
#define  DTAPI_E_NO_TS_INPUT		(DTAPI_E + 28)
#define  DTAPI_E_DRIVER_INCOMP		(DTAPI_E + 29)
#define  DTAPI_E_INTERNAL			(DTAPI_E + 30)
#define  DTAPI_E_OUT_OF_MEM			(DTAPI_E + 31)
#define  DTAPI_E_INVALID_J83ANNEX	(DTAPI_E + 32)
#define  DTAPI_E_INVALID_ROLLOFF	(DTAPI_E + 32)
#define  DTAPI_E_IDLE				(DTAPI_E + 33)
#define  DTAPI_E_INSUF_LOAD			(DTAPI_E + 34)
#define  DTAPI_E_INVALID_BANDWIDTH	(DTAPI_E + 35)
#define  DTAPI_E_INVALID_CONSTEL	(DTAPI_E + 36)
#define  DTAPI_E_INVALID_GUARD		(DTAPI_E + 37)
#define  DTAPI_E_INVALID_INTERLVNG	(DTAPI_E + 38)
#define  DTAPI_E_INVALID_TRANSMODE	(DTAPI_E + 39)
#define  DTAPI_E_INVALID_TSTYPE		(DTAPI_E + 40)
#define  DTAPI_E_NO_IPPARS			(DTAPI_E + 41)
#define  DTAPI_E_NO_TSRATE			(DTAPI_E + 42)
#define  DTAPI_E_NOT_IDLE			(DTAPI_E + 43)
#define  DTAPI_E_INVALID_ARG		(DTAPI_E + 44)
#define  DTAPI_E_NDIS_DRIVER		(DTAPI_E + 45)
#define  DTAPI_E_DST_MAC_ADDR		(DTAPI_E + 46)
#define  DTAPI_E_NO_SUCH_PORT		(DTAPI_E + 47)
#define  DTAPI_E_WINSOCK			(DTAPI_E + 48)
#define  DTAPI_E_MULTICASTJOIN		(DTAPI_E + 49)
#define  DTAPI_E_EMBEDDED			(DTAPI_E + 50)
#define  DTAPI_E_LOCKED				(DTAPI_E + 51)
#define  DTAPI_E_NO_VALID_CALDATA	(DTAPI_E + 52)
#define  DTAPI_E_NO_LINK			(DTAPI_E + 53)
#define  DTAPI_E_INVALID_HEADER		(DTAPI_E + 54)
#define  DTAPI_E_INVALID_PARS		(DTAPI_E + 55)
#define  DTAPI_E_NOT_SDI_MODE		(DTAPI_E + 56)
#define  DTAPI_E_INCOMP_FRAME		(DTAPI_E + 57)
#define  DTAPI_E_UNSUP_CONV			(DTAPI_E + 58)
#define  DTAPI_E_OUTBUF_TOO_SMALL   (DTAPI_E + 59)
#define  DTAPI_E_CONFIG				(DTAPI_E + 60)
#define  DTAPI_E_TIMEOUT			(DTAPI_E + 61)
#define  DTAPI_E_INVALID_TIMEOUT	(DTAPI_E + 62)


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ IP Tx / Rx Constants +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- IP Tx / Rx Protocol -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_PROTO_UDP			0
#define  DTAPI_PROTO_RTP			1
#define  DTAPI_PROTO_AUTO			2
#define  DTAPI_PROTO_UNKN			2

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- IP Tx / Rx Fec mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_FEC_DISABLE			0
#define  DTAPI_FEC_2D				1		// FEC reconstruction
#define  DTAPI_FEC_2D_M1			1		// Mode1: FECdT = DVBdT + .5 * DVBdT
#define  DTAPI_FEC_2D_M2			2		// Mode2: FECdT = DVBdT


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ General Network Constants +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Ethernet Speed -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NWSPEED_AUTO			0		// Set
#define  DTAPI_NWSPEED_NOLINK		0		// Get
#define  DTAPI_NWSPEED_10MB_HALF	1
#define  DTAPI_NWSPEED_10MB_FULL	2
#define  DTAPI_NWSPEED_100MB_HALF	3
#define  DTAPI_NWSPEED_100MB_FULL	4
#define  DTAPI_NWSPEED_1GB_MASTER	5
#define  DTAPI_NWSPEED_1GB_SLAVE	6


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+ BACKWARD COMPATIBILITY SECTION +=+=+=+=+=+=+=+=+=+=+=+=+=+=

// Map old PciCard class on DtDevice
typedef class DtDevice PciCard;

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtapiHwFunc -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure that describes a hardware function on a PCI card.
//
struct DtapiHwFunc {		
	int  m_nPciBusNumber;		// PCI-bus number
	int  m_nSlotNumber;			// PCI-slot number
	int  m_nTypeNumber;			// PCI-card type number
	int  m_nDeviceId;			// Device ID of PCI card
	int  m_nVendorId;			// Vendor ID of PCI card
	int  m_nSubsystemId;		// Subsystem ID of PCI card
	int  m_nSubVendorId;		// Subsystem Vendor ID of PCI card
	int  m_nHwFuncType;			// Hardware-function type
	int  m_nHwFuncFlags;		// Hardware-function flags
	int  m_nIndexOnCard;		// Index of hardware function
};

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Global functions -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
DTAPI_API DTAPI_RESULT DtapiPciScan(int NumFuncEntries, int& NumFuncEntriesResult,
									 DtapiHwFunc* pHwFuncs);

//=+=+=+=+=+=+=+=+=+=+=+=+ END OF BACKWARD COMPATIBILITY SECTION +=+=+=+=+=+=+=+=+=+=+=+=+

//if namespace usage is required
#ifdef _USE_NAMESPACE
}       //end of namespace
#endif

#endif //#ifndef __DTAPI_H
