#include "stdafx.h"

#include "..\Common\define.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_ErrorCode.h"
#include "TSMagic_Trigger_Section.h"
#include "TSMagic_Callbacks_To_Gui.h"

#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_ErrorCode.h"
#include "syntax_translate_layer/MPEG_PESPacket/Include/MPEG_stream_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer\Include\MiddleWare_SectionSplicer_ErrorCode.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
#include "thirdparty_libs\glog\glog\logging.h"

#include "TSMagicView.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

CTrigger_PsiSiSection::CTrigger_PsiSiSection(void)
{
	m_usPID = 0xFFFF;
}

CTrigger_PsiSiSection::~CTrigger_PsiSiSection(void)
{
}

void CTrigger_PsiSiSection::Reset(void)
{
	m_usPID = 0xFFFF;
	CTrigger::Reset();
}

uint16_t CTrigger_PsiSiSection::GetPID(void)
{
	return m_usPID;
}

int CTrigger_PsiSiSection::IsMatched(uint8_t* buf, int length)
{
	int		 equal = -1;
	int		 i;
	uint8_t		 mask;
	uint8_t		 data;
	uint8_t		 src;
	uint8_t		 dst;

	if (buf != NULL)
	{
		equal = 1;

		mask = m_ucMatchMasks[0];
		data = m_ucMatchDatas[0];

		src = buf[0] & mask;
		dst = data & mask;

		if (src == dst)
		{
			for (i = 1; i < m_nMatchLength; i++)
			{
				mask = m_ucMatchMasks[i];
				data = m_ucMatchDatas[i];

				src = buf[i + 2] & mask;					//section length field is not compared
				dst = data & mask;
				if (src != dst)
				{
					equal = 0;
					break;
				}
			}
		}
		else
		{
			equal = 0;
		}
	}

	return equal;
}

//void CTrigger_PsiSiSection::SetMatchParamsForEIT(uint16_t PID, uint16_t original_network_id, uint16_t transport_stream_id, uint16_t service_id, uint8_t table_id, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = table_id;
//
//	//service_id
//	ucReqMask[1] = 0xff;
//	ucReqMask[2] = 0xff;
//	ucReqData[1] = (service_id & 0xff00) >> 8;
//	ucReqData[2] = (service_id & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	//transport_stream_id
//	ucReqMask[6] = 0xff;
//	ucReqMask[7] = 0xff;
//	ucReqData[6] = (transport_stream_id & 0xff00) >> 8;
//	ucReqData[7] = (transport_stream_id & 0x00ff);
//
//	//original_network_id
//	ucReqMask[8] = 0xff;
//	ucReqMask[9] = 0xff;
//	ucReqData[8] = (original_network_id & 0xff00) >> 8;
//	ucReqData[9] = (original_network_id & 0x00ff);
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}


//void CTrigger_PsiSiSection::SetMatchParamsForINT(uint16_t PID, uint32_t platform_id, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_INT;
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	ucReqMask[6] = 0xff;
//	ucReqMask[7] = 0xff;
//	ucReqMask[8] = 0xff;
//	ucReqData[6] = (platform_id & 0xff0000) >> 16;
//	ucReqData[7] = (platform_id & 0x00ff00) >> 8;
//	ucReqData[8] = (platform_id & 0x0000ff);
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForAIT(uint16_t PID, uint16_t application_type, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_AIT;
//
//	//application_type
//	ucReqMask[1] = 0xff;
//	ucReqData[1] = (application_type & 0xff00) >> 8;
//	ucReqMask[2] = 0xff;
//	ucReqData[2] = (application_type & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

void CTrigger_PsiSiSection::SetMatchParamsForNormalSection(uint16_t PID, uint8_t table_id, uint16_t table_id_extension, uint8_t section_number)
{
	uint8_t		ucReqMask[16];
	uint8_t		ucReqData[16];

	memset(ucReqMask, 0x00, sizeof(ucReqMask));
	memset(ucReqData, 0x00, sizeof(ucReqData));

	//table_id
	ucReqMask[0] = 0xff;
	ucReqData[0] = table_id;

	//table_id_extension
	ucReqMask[1] = 0xff;
	ucReqData[1] = (table_id_extension & 0xff00) >> 8;
	ucReqMask[2] = 0xff;
	ucReqData[2] = (table_id_extension & 0x00ff);

	//section_number: note -- section_length is not contained in the filter mask and data
	ucReqMask[4] = 0xff;
	ucReqData[4] = section_number;

	m_usPID = PID;
	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
}

void CTrigger_PsiSiSection::SetMatchParamsForOtherSection(uint16_t PID, uint8_t table_id)
{
	uint8_t		ucReqMask[16];
	uint8_t		ucReqData[16];

	memset(ucReqMask, 0x00, sizeof(ucReqMask));
	memset(ucReqData, 0x00, sizeof(ucReqData));

	//table_id
	ucReqMask[0] = 0xff;
	ucReqData[0] = table_id;

	//table_id_extension
	//ucReqMask[1] = 0xff;
	//ucReqData[1] = (table_id_extension & 0xff00) >> 8;
	//ucReqMask[2] = 0xff;
	//ucReqData[2] = (table_id_extension & 0x00ff);

	//section_number: note -- section_length is not contained in the filter mask and data
	//ucReqMask[4] = 0xff;
	//ucReqData[4] = section_number;

	m_usPID = PID;
	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
}

//void CTrigger_PsiSiSection::SetMatchParamsForDSMCC(uint16_t PID, uint8_t table_id, uint16_t table_id_extension, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = table_id;
//
//	//table_id_extension
//	ucReqMask[1] = 0xff;
//	ucReqData[1] = (table_id_extension & 0xff00) >> 8;
//	ucReqMask[2] = 0xff;
//	ucReqData[2] = (table_id_extension & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

/*
void SECTION_TRIGGER_DSMCC_SD_Set(uint16_t PID, uint16_t table_id_extension, uint8_t section_number)
{
#if GUI_TS_ANALYZER_PSISI
char		pszText[MAX_TXT_CHARS];
uint8_t		ucReqMask[16];
uint8_t		ucReqData[16];
int		i;
CWnd*	pWnd;

CTSMagicView* pTSMagicView = CTSMagicView::GetView();
pWindow->m_dlgSectionTriggerWaiting.ShowWindow(SW_SHOW);

sprintf_s(pszText, sizeof(pszText), "DSM-CC Stream Descriptors TRIGGER: section_number = %d", section_number);
//	pWindow->m_dlgSectionTriggerWaiting.SetWindowText(pszText);
::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_INFO);

//	TSMagic_Debug(DEBUG_INFO, pszText);

memset(ucReqMask, 0x00, sizeof(ucReqMask));
memset(ucReqData, 0x00, sizeof(ucReqData));

//table_id
ucReqMask[0] = 0xff;
ucReqData[0] = TABLE_ID_DSM_CC_SD;

//table_id_extension
ucReqMask[1] = 0xff;
ucReqData[1] = (table_id_extension & 0xff00) >> 8;
ucReqMask[2] = 0xff;
ucReqData[2] = (table_id_extension & 0x00ff);

//section_number: note -- section_length is not contained in the filter mask and data
ucReqMask[4] = 0xff;
ucReqData[4] = section_number;

TRIGGER_PSISI_SECTION_setparams(PID, FILTER_PSISI_SYNTAX_DSMCC_SD, ucReqMask, ucReqData, sizeof(ucReqMask), 1);

memset(pszText, 0x00, sizeof(pszText));
for (i = 0; i < sizeof(ucReqMask); i++)
{
sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqMask[i]);
}

pWnd = pWindow->m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
pWnd->SetWindowText(pszText);

memset(pszText, 0x00, sizeof(pszText));
for (i = 0; i < sizeof(ucReqData); i++)
{
sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqData[i]);
}

pWnd = pWindow->m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
pWnd->SetWindowText(pszText);
#endif
}

void SECTION_TRIGGER_DSMCC_DDM_Set(uint16_t PID, uint16_t table_id_extension, uint8_t section_number)
{
#if GUI_TS_ANALYZER_PSISI
char		pszText[MAX_TXT_CHARS];
uint8_t		ucReqMask[16];
uint8_t		ucReqData[16];
int		i;
CWnd*	pWnd;

CTSMagicView* pTSMagicView = CTSMagicView::GetView();
pWindow->m_dlgSectionTriggerWaiting.ShowWindow(SW_SHOW);

sprintf_s(pszText, sizeof(pszText), "DSM-CC DDM TRIGGER: section_number = %d", section_number);
//	pWindow->m_dlgSectionTriggerWaiting.SetWindowText(pszText);
::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_INFO);

//	TSMagic_Debug(DEBUG_INFO, pszText);

memset(ucReqMask, 0x00, sizeof(ucReqMask));
memset(ucReqData, 0x00, sizeof(ucReqData));

//table_id
ucReqMask[0] = 0xff;
ucReqData[0] = TABLE_ID_DSM_CC_DDM;

//table_id_extension
ucReqMask[1] = 0xff;
ucReqData[1] = (table_id_extension & 0xff00) >> 8;
ucReqMask[2] = 0xff;
ucReqData[2] = (table_id_extension & 0x00ff);

//section_number: note -- section_length is not contained in the filter mask and data
ucReqMask[4] = 0xff;
ucReqData[4] = section_number;

TRIGGER_PSISI_SECTION_setparams(PID, FILTER_PSISI_SYNTAX_DSMCC_DDM, ucReqMask, ucReqData, sizeof(ucReqMask), 1);

memset(pszText, 0x00, sizeof(pszText));
for (i = 0; i < sizeof(ucReqMask); i++)
{
sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqMask[i]);
}

pWnd = pWindow->m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
pWnd->SetWindowText(pszText);

memset(pszText, 0x00, sizeof(pszText));
for (i = 0; i < sizeof(ucReqData); i++)
{
sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqData[i]);
}

pWnd = pWindow->m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
pWnd->SetWindowText(pszText);
#endif
}

void SECTION_TRIGGER_DSMCC_UNM_Set(uint16_t PID, uint16_t table_id_extension, uint8_t section_number)
{
#if GUI_TS_ANALYZER_PSISI
char		pszText[MAX_TXT_CHARS];
uint8_t		ucReqMask[16];
uint8_t		ucReqData[16];
int		i;
CWnd*	pWnd;

CTSMagicView* pTSMagicView = CTSMagicView::GetView();
pWindow->m_dlgSectionTriggerWaiting.ShowWindow(SW_SHOW);

sprintf_s(pszText, sizeof(pszText), "DSM-CC UNM TRIGGER: section_number = %d", section_number);
//	pWindow->m_dlgSectionTriggerWaiting.SetWindowText(pszText);
::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_INFO);

//	TSMagic_Debug(DEBUG_INFO, pszText);

memset(ucReqMask, 0x00, sizeof(ucReqMask));
memset(ucReqData, 0x00, sizeof(ucReqData));

//table_id
ucReqMask[0] = 0xff;
ucReqData[0] = TABLE_ID_DSM_CC_UNM;

//table_id_extension
ucReqMask[1] = 0xff;
ucReqData[1] = (table_id_extension & 0xff00) >> 8;
ucReqMask[2] = 0xff;
ucReqData[2] = (table_id_extension & 0x00ff);

//section_number: note -- section_length is not contained in the filter mask and data
ucReqMask[4] = 0xff;
ucReqData[4] = section_number;

TRIGGER_PSISI_SECTION_setparams(PID, FILTER_PSISI_SYNTAX_DSMCC_UNM, ucReqMask, ucReqData, sizeof(ucReqMask), 1);

memset(pszText, 0x00, sizeof(pszText));
for (i = 0; i < sizeof(ucReqMask); i++)
{
sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqMask[i]);
}

pWnd = pWindow->m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_MASK);
pWnd->SetWindowText(pszText);

memset(pszText, 0x00, sizeof(pszText));
for (i = 0; i < sizeof(ucReqData); i++)
{
sprintf_s(pszText, sizeof(pszText), "%s %02X", pszText, ucReqData[i]);
}

pWnd = pWindow->m_dlgSectionTriggerWaiting.GetDlgItem(IDC_STATIC_DATA);
pWnd->SetWindowText(pszText);
#endif
}
*/

//void CTrigger_PsiSiSection::SetMatchParamsForMPE(uint16_t PID, uint16_t usRsv, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_MPE;
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForPAT(uint16_t PID, uint16_t transport_stream_id, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_PAT;
//
//	ucReqMask[1] = 0xff;
//	ucReqMask[2] = 0xff;
//	ucReqData[1] = (transport_stream_id & 0xff00) >> 8;
//	ucReqData[2] = (transport_stream_id & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}



//void CTrigger_PsiSiSection::SetMatchParamsForPMT(uint16_t PID, uint16_t program_number, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_PMT;
//
//	//program_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[1] = 0xff;
//	ucReqData[1] = (program_number & 0xff00) >> 8;
//	ucReqMask[2] = 0xff;
//	ucReqData[2] = (program_number & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForTSDT(uint16_t PID, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_TSDT;
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForCAT(uint16_t PID, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_CAT;
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForNIT(uint16_t PID, uint8_t table_id, uint16_t network_id, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = table_id;
//
//	ucReqMask[1] = 0xff;
//	ucReqMask[2] = 0xff;
//	ucReqData[1] = (network_id & 0xff00) >> 8;
//	ucReqData[2] = (network_id & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForSDT(uint16_t PID, uint16_t original_network_id, uint16_t transport_stream_id, uint8_t table_id, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = table_id;
//
//	ucReqMask[1] = 0xff;
//	ucReqMask[2] = 0xff;
//	ucReqData[1] = (transport_stream_id & 0xff00) >> 8;
//	ucReqData[2] = (transport_stream_id & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	ucReqMask[6] = 0xff;
//	ucReqMask[7] = 0xff;
//	ucReqData[6] = (original_network_id & 0xff00) >> 8;
//	ucReqData[7] = (original_network_id & 0x00ff);
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForBAT(uint16_t PID, uint16_t bouquet_id, uint8_t table_id, uint8_t section_number)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = table_id;
//
//	ucReqMask[1] = 0xff;
//	ucReqMask[2] = 0xff;
//	ucReqData[1] = (bouquet_id & 0xff00) >> 8;
//	ucReqData[2] = (bouquet_id & 0x00ff);
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	ucReqMask[4] = 0xff;
//	ucReqData[4] = section_number;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForTDT(uint16_t PID)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_TDT;
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForTOT(uint16_t PID)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_TOT;
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}

//void CTrigger_PsiSiSection::SetMatchParamsForRST(uint16_t PID)
//{
//	uint8_t		ucReqMask[16];
//	uint8_t		ucReqData[16];
//
//	memset(ucReqMask, 0x00, sizeof(ucReqMask));
//	memset(ucReqData, 0x00, sizeof(ucReqData));
//
//	//table_id
//	ucReqMask[0] = 0xff;
//	ucReqData[0] = TABLE_ID_RST;
//
//	//section_number: note -- section_length is not contained in the filter mask and data
//	m_usPID = PID;
//	SetMatchParams(ucReqMask, ucReqData, sizeof(ucReqMask), 1);
//}


void ts_section_trigger_loop(ts_thread_params_t* pThreadParams)
{
#if OPEN_SECTION_TRIGGER
	uint8_t	  packet_buf[204];
	int		  packet_length;
	int		  stream_synced = 0;
	int		  catchedCount = 0;
	char	  pszDebug[256];

	int64_t	  read_byte_pos = 0;
	int64_t   section_start_pos = -1;
	int64_t   section_end_pos = -1;
	int		  rtcode;

	CSectionSplicer				SectionSplicer;

	CTransportStream*			ptransport_stream = NULL;
	transport_packet_t			transport_packet;

	CTrigger_PsiSiSection*		pSectionTrigger = NULL;

	if (pThreadParams != NULL)
	{
		uint32_t tickCount = ::GetTickCount();
		srand(tickCount);
		uint32_t randNum = rand();

		pThreadParams->section_trigger_thread_running = 1;

		sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉<ID:0x%08X>: 开始", randNum);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		LOG(INFO) << pszDebug;

		ptransport_stream = pThreadParams->pTStream;
		ptransport_stream->StartGetData();				//启动接收线程

		pSectionTrigger = pThreadParams->pTrigger_Section;

		while (pThreadParams->section_trigger_thread_running == 1)
		{
			if (stream_synced == 0)
			{
				do
				{
					rtcode = ptransport_stream->Synchronize(&packet_length);
					if (rtcode == MIDDLEWARE_TS_NO_ERROR)
					{
						stream_synced = 1;

						read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动
						sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉<ID:0x%08X>: 找到同步, TS包长 %d字节（文件位置：0x%llx）\n", randNum, packet_length, read_byte_pos);

						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
						LOG(INFO) << pszDebug;
						break;
					}
					else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
					{
						ptransport_stream->SeekToBegin();
					}
				} while (pThreadParams->section_trigger_thread_running == 1);
			}
			else if (stream_synced == 1)
			{
				packet_length = sizeof(packet_buf);
				rtcode = ptransport_stream->SyncReadOnePacket(packet_buf, &packet_length);
				read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动
				if (rtcode == MIDDLEWARE_TS_NO_ERROR)
				{
					//成功读出一个TS包
					rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);
					if (rtcode == TSPACKET_PARSE_NO_ERROR)
					{
						//PSISI section trigger
						if (transport_packet.PID == pSectionTrigger->GetPID())
						{
							if (pSectionTrigger->IsOpened() && (!pSectionTrigger->IsFull()))
							{
								do {
									rtcode = SectionSplicer.WriteTSPacket(&transport_packet);
									
									if (rtcode == SECTION_SPLICE_FIRST_PACKET)
									{
										catchedCount = 1;
										section_start_pos = read_byte_pos - packet_length;
									}
									else if (rtcode == SECTION_SPLICE_FOLLOW_PACKET)
									{
										catchedCount++;
									}
									else if ((rtcode == SECTION_SPLICE_LAST_PACKET) || (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START))
									{
										catchedCount++;

										if (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START)
										{
											section_end_pos = read_byte_pos - packet_length + 4 + 1 + transport_packet.payload_buf[0];		//pointer_field
										}
										else
										{
											section_end_pos = read_byte_pos;
										}

										if (pSectionTrigger->IsMatched(SectionSplicer.m_pucSectionBuf, SectionSplicer.m_nSectionLength))
										{
											int nOldCatchedCount = pSectionTrigger->GetCatchedCount();
											pSectionTrigger->SaveTheWholePacket(SectionSplicer.m_pucSectionBuf, SectionSplicer.m_nSectionLength);

											if (nOldCatchedCount == 0)		//捕捉到第一个匹配section时报告状态
											{
												::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_SECTION_TRIGGER_STATE, 0, 0);

												if (section_start_pos == -1)
												{
													section_start_pos = read_byte_pos - packet_length;
												}

												sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉<ID:0x%08X>: 找到匹配Section（文件位置：开始于0x%llx - 结束于0x%llx）\n", randNum, section_start_pos, section_end_pos);
												::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
												LOG(INFO) << pszDebug;
											}

											if (pSectionTrigger->IsFull())
											{
												if (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START)
												{
													ptransport_stream->Seek(read_byte_pos - packet_length);
												}
												else
												{
													ptransport_stream->Seek(read_byte_pos);
												}

												break;
											}
										}
										else
										{
											if (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START)
											{
												section_start_pos = read_byte_pos - packet_length;
											}
											else
											{
												section_start_pos = -1;
											}
											section_end_pos = -1;
											catchedCount = 0;
										}

										SectionSplicer.Reset();
									}
								} while (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START);

								if (pSectionTrigger->IsFull())
								{
									break;
								}
							}
						}
					}
					else
					{
						//解析TS包语法发现错误，为什么发生错误？？？？
						assert(0);
					}
				}
				else if (rtcode == ETR290_TS_SYNC_LOSS)
				{
					//读取TS包发生错误——同步丢失
					stream_synced = 0;

					sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉<ID:0x%08X>: 丢失同步（文件位置：0x%llx）\n", randNum, read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;
				}
				else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
				{
					SectionSplicer.Reset();
					pSectionTrigger->Reset();

					stream_synced = 0;

					sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉<ID:0x%08X>: 到达文件尾！（文件位置：0x%llx）\n", randNum, read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;

					ptransport_stream->SeekToBegin();
				}
				else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
				{
				}
				else
				{
					break;
				}
			}
		}

		ptransport_stream->StopGetData();

		//释放sectionsplicer的资源
		SectionSplicer.Reset();

		pThreadParams->section_trigger_thread_running = 0;
//		pThreadParams->section_trigger_thread_stopped = 1;

		uint32_t existTickCount = ::GetTickCount();
		uint32_t diff_tickcount = existTickCount - tickCount;

		sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉<ID:0x%08X>: 结束, 共抓%d个TS包，耗时%dms", randNum, catchedCount, diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		LOG(INFO) << pszDebug;
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "SECTION捕捉: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
#endif
}

uint32_t TSMagic_section_trigger_thread(LPVOID lpParam)
{
	ts_thread_params_t*	pThreadParams = (ts_thread_params_t*)lpParam;
	ts_section_trigger_loop(pThreadParams);

	return 0;
}



