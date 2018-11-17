#include "../Include/SIOWithDektec.h"

#ifdef _WIN64
#else
#include "thirdparty_HW/Dektec/DTAPI.h"

DtDevice		gDtDvc;				// Our device
DtOutpChannel	gDtOutp;			// Our output channel
DtInpChannel	gDtInp;				// Our input channel
#endif

int dektec_transmit_fini(void)
{
	return 0;
}

int dektec_transmit_init(void)
{
#ifdef _WIN64
	return -1;
#else
	DTAPI_RESULT dr = -1;

	int n, NumHwFuncsFound(0);
	DtHwFuncDesc HwFuncs[20];

	// Scan for available DekTec devices
	dr = ::DtapiHwFuncScan(sizeof(HwFuncs)/sizeof(HwFuncs[0]), NumHwFuncsFound, HwFuncs);
	if ( dr != DTAPI_OK )
	{
		//AfxMessageBox("扫描设备失败");
		return -1;
	
	}

	// Loop through found hardware functions
	bool Found(false);
	int DeviceNum(0);
	DtHwFuncDesc* pHwf = &HwFuncs[0];
	for ( n=0; n<NumHwFuncsFound; n++, pHwf++ )
	{	
		// Skip non-outputs
		if ( 0==(pHwf->m_ChanType & DTAPI_CHAN_OUTPUT) )
			continue;

		// Looking for a specific type??
	    if ( pHwf->m_DvcDesc.m_TypeNumber== 245 )
		{
			Found = true;
			break;
		}
	}
	// Did we find our device?
	if ( !Found  )
	{
//		AfxMessageBox("找不到设备");
		return -2;
	}
	
	// Attach to the device
	dr = gDtDvc.AttachToSerial(pHwf->m_DvcDesc.m_Serial);
	if ( dr != DTAPI_OK )
	{
		// Special case: driver version is not compatible with this version of DtPlay,
		// get the driver version and throw an exception
//		AfxMessageBox("设备连接失败");
		return -3;
	}
		
	// Attach to the output channel
	dr = gDtOutp.AttachToPort(&gDtDvc, pHwf->m_Port);
	if ( dr != DTAPI_OK )
	{
//		AfxMessageBox("端口连接失败");
		return -4;
	}

	// Init channel to initial 'safe' state
	dr = gDtOutp.SetTxControl(DTAPI_TXCTRL_IDLE);	// Start in IDLE mode
	if ( dr != DTAPI_OK )
	{
//		AfxMessageBox("初始化失败");
		return -5;
	}
//	dr = gDtOutp.SetFifoSize(8*1024*1024);	// Set FIFO size to 8MB
	dr = gDtOutp.SetFifoSize(240*1024);
	dr = gDtOutp.ClearFifo();			// Clear FIFO (i.e. start with zero load)

	// Set the transmission mode. NOTE: ISDB-T only supports 204 byte mode
	dr = gDtOutp.SetTxMode( 0, DTAPI_TXSTUFF_MODE_OFF );

	return dr;
#endif
}

int dektec_start_transmit(int bitrate)
{
#ifdef _WIN64
	return -1;
#else
	DTAPI_RESULT dr = -1;

	dr = gDtOutp.SetTsRateBps(bitrate);
	if ( dr != DTAPI_OK )
	{
	//	AfxMessageBox("set bitrate failed!");
		return 1;
	}
	dr = gDtOutp.SetTxControl(DTAPI_TXCTRL_HOLD);
	if ( dr != DTAPI_OK )
	{
	//	AfxMessageBox("Set tx control failed!");
		return 2;
	}

	return dr;
#endif
}

int dektec_transmit(unsigned char *buffer, int count)
{
#ifdef _WIN64
#else
	DTAPI_RESULT dr = -1;
	dr = gDtOutp.Write((char*)buffer, count);
#endif

	return count;
	
}

int dektec_stop_transmit(void)
{
#ifdef _WIN64
#else
	gDtOutp.SetTxControl(DTAPI_TXCTRL_IDLE);
	gDtOutp.Detach(DTAPI_INSTANT_DETACH);
	gDtDvc.Detach();
#endif

	return 0;
}

int dektec_receive_init(void)
{
#ifdef _WIN64
	return -1;
#else
	DTAPI_RESULT	dr = -1;
	int n, NumHwFuncsFound(0);
	DtHwFuncDesc HwFuncs[20];

	// Scan for available DekTec devices
	dr = ::DtapiHwFuncScan(sizeof(HwFuncs)/sizeof(DtHwFuncDesc), NumHwFuncsFound, HwFuncs);
	if ( dr != DTAPI_OK )
	{
//		AfxMessageBox("硬件设备扫描失败");
		return -1;
	}
	// Loop through found hardware functions
	bool Found(false);
	int DeviceNum(0);

	DtHwFuncDesc* pHwf = HwFuncs;
	for (n = 0; n < NumHwFuncsFound; n++, pHwf++)
	{
		// Skip non-inputs
		if (0==(pHwf->m_ChanType & DTAPI_CHAN_INPUT))
		{
			continue;
		}

		// Looking for a specific type??
		if ( pHwf->m_DvcDesc.m_TypeNumber == 245 )
		{
			Found = true;
			break;
		}
	}
	// Did we find our device?
	if ( !Found)
	{
		// Could not find any matching input at all
//		AfxMessageBox("找不到设备");
		return -2;
	}
	
	// Attach to the device
	dr = gDtDvc.AttachToSerial(pHwf->m_DvcDesc.m_Serial);

	if ( dr != DTAPI_OK )
	{
//		AfxMessageBox("设备连接失败");
		return -3;
	}

	// Attach to the output channel
	dr = gDtInp.AttachToPort(&gDtDvc, pHwf->m_Port);
	if ( dr != DTAPI_OK )
	{
// 		AfxMessageBox("端口连接失败");
		return -4;
	}

	// Init channel to initial 'safe' state
	dr = gDtInp.SetRxControl(DTAPI_RXCTRL_IDLE);	// Start in IDLE mode
	if ( dr != DTAPI_OK )
	{
//		AfxMessageBox("初始化失败");
		return -5;
	}

	dr = gDtInp.ClearFifo();			// Clear FIFO (i.e. start with zero load)
	dr = gDtInp.ClearFlags(0xFFFFFFFF);// Clear all flags
	
	// Set the receive mode
	dr = gDtInp.SetRxMode(DTAPI_RXMODE_STRAW);//st188
	if (dr != DTAPI_OK)
	{
//		AfxMessageBox("采集模式设置失败");
		return -6;
	}

	return dr;
#endif
}

int dektec_start_receive(void)
{
#ifdef _WIN64
	return -1;
#else
	DTAPI_RESULT dr = -1;
	int SdiMode = -1;
	int i = 0;
	
	bool ValidSignal=false;

	//-.-.-.-.-.-.-.-.-.-.-.-.-.- First loop wait for a signal -.-.-.-.-.-.-.-.-.-.-.-.-.-
	while (!ValidSignal) 
	{ 
		int CarrierDetect, n;

		dr = gDtInp.GetStatus(SdiMode, n, CarrierDetect, n, n, n);

		// Do we have a carrier and for SDI recording we also check the SDI mode
		if ( CarrierDetect != DTAPI_CLKDET_OK )
		{
			// No carrier => wait shortly for signal

			Sleep(500);
            i++; 
			if(i == 1)
			{
//				AfxMessageBox("无信号");
				return -1;					//无信号
			}
			continue;
		}
		ValidSignal = true;
	}

	// Init allignment

	// Start reception
	dr = gDtInp.ClearFifo();			// Clear FIFO (i.e. start with zero load)
	dr = gDtInp.ClearFlags(0xFFFFFFFF);// Clear all flags

	dr = gDtInp.SetRxControl(DTAPI_RXCTRL_RCV);

	return dr;
#endif
}

int dektec_receive(unsigned char *buffer, int count)
{
	int	rtcode = 0;

#ifdef _WIN64
#else
	DTAPI_RESULT dr;
	int			 fifo_load;

	// Read alligned number of data from input channel
	dr = gDtInp.GetFifoLoad(fifo_load);
	if (dr == DTAPI_OK)
	{
		if (fifo_load > count)
		{
			dr = gDtInp.Read((char*)buffer, count);

			if (dr == DTAPI_OK)
			{
				rtcode = count;
			}
		}
	}
#endif

	return rtcode;
}

int dektec_stop_receive(void)
{
#ifdef _WIN64
#else
	DTAPI_RESULT dr = -1;
	// Detach from channel and device
	gDtInp.SetRxControl(DTAPI_RXCTRL_IDLE);

	dr = gDtInp.ClearFifo();			// Clear FIFO (i.e. start with zero load)
	dr = gDtInp.ClearFlags(0xFFFFFFFF);// Clear all flags
#endif

	return 0;
}

int dektec_receive_fini(void)
{
#ifdef _WIN64
#else
	gDtInp.Detach(DTAPI_INSTANT_DETACH);
	gDtDvc.Detach();
#endif

	return 0;
}

int dektec_get_asiin_bitrate(void)
{
	int	rtcode = -1;

#ifdef _WIN64
#else
	gDtInp.GetTsRateBps(rtcode);
#endif

	return rtcode;
}

