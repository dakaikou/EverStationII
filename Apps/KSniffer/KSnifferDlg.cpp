
// KSnifferDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KSniffer.h"
#include "KSnifferDlg.h"
#include "afxdialogex.h"
#include "GlobalVar.h"
#include <vector>
#include <iterator>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::vector<PacketNode *> g_vPacketChain;
extern WCHAR g_szIcmpType[42][40];
extern WCHAR g_szIgmpType[34][40];

// CKSnifferDlg �Ի���
CKSnifferDlg::CKSnifferDlg(CWnd* pParent /*=NULL*/)
	: m_pAllDevs(NULL), m_tCapThread(NULL), m_tShowThread(NULL),
	CDialogEx(CKSnifferDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKSnifferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PACKETS, m_cPacketsList);
	DDX_Control(pDX, IDC_STATIC_ADAPTER, m_cStatusCurAdapter);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_cPacketNumBar);
	DDX_Control(pDX, IDC_EDIT_HEXDATA, m_cHexView);
	DDX_Control(pDX, IDC_TREE_PACKETS, m_cTreeView);
}

BEGIN_MESSAGE_MAP(CKSnifferDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_COMMAND(ID_32774, &CKSnifferDlg::OnSelectAdapters)
	ON_COMMAND(ID_32775, &CKSnifferDlg::OnSetCapFilter)
	ON_COMMAND(IDM_OPT_STARTCAP, &CKSnifferDlg::OnOptStartcap)
	ON_COMMAND(IDM_OPT_STOPCAP, &CKSnifferDlg::OnOptStopcap)
	ON_COMMAND(IDM_HELP_ABOUT, &CKSnifferDlg::OnHelpAbout)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PACKETS, &CKSnifferDlg::OnNMClickListPackets)
//	ON_NOTIFY(NM_CLICK, IDC_TREE_PACKETS, &CKSnifferDlg::OnNMClickTreePackets)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PACKETS, &CKSnifferDlg::OnTvnSelchangedTreePackets)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_PACKETS, &CKSnifferDlg::OnTvnDeleteitemTreePackets)
	ON_COMMAND(IDM_FILE_CLOSE, &CKSnifferDlg::OnFileClose)
END_MESSAGE_MAP()


// CKSnifferDlg ��Ϣ�������

BOOL CKSnifferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ��ʼ���ؼ�
	InitControls();
	InitWinpcap();
	// ��vector����һ���Ŀռ�
	g_vPacketChain.reserve(10*1024*1024);
	// ���ݰ�ͳ��
	m_dwTcp = m_dwUdp = m_dwArp = m_dwIgmp = m_dwIcmp = 0;
	// ��ʾ�߳�
	SAFE_DELETE(m_tShowThread);
	m_tShowThread = new CShowThread(m_dwTcp, m_dwUdp, 
		m_dwArp, m_dwIgmp, m_dwIcmp, this);
    m_tShowThread->ResumeThread();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

//  �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CKSnifferDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CKSnifferDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ��ʼ���ؼ�
BOOL CKSnifferDlg::InitControls(void)
{
	// �б���ͼ�ؼ� ��������
	DWORD dwStyle = m_cPacketsList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_cPacketsList.SetExtendedStyle(dwStyle);
	// �б���ͼ�ؼ� �����ͷ
	CString csHeadStr;
	DWORD dwCol = 0;
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_NO);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 60);	// ��һ��ʵ���ϲ������Ҷ���
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_TIME);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 120);	// ���������
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_SRCHOST);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 120);
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_DSTHOST);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 120);
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_PROTOCOL);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 50);
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_LENGTH);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 50);
	csHeadStr.LoadString(AfxGetInstanceHandle(), IDS_LH_DETAIL);
	m_cPacketsList.InsertColumn(dwCol++, csHeadStr, LVCFMT_LEFT, 400);

	// ʮ�����ƿؼ�
	m_cHexView.m_pData = NULL;
	m_cHexView.m_length = 0;
	m_cHexView.m_bpr = 16;		// ÿ��16���ֽ�
	m_cHexView.m_lpp = 6;		// ÿҳ06������
	m_cHexView.SetReadOnly();

	// �������οؼ�����
	dwStyle = GetWindowLong(m_cTreeView.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_LINESATROOT | TVS_HASBUTTONS | TVS_FULLROWSELECT;//| TVS_HASLINES;
	SetWindowLong(m_cTreeView.m_hWnd, GWL_STYLE, dwStyle);

	return 0;
}

// ��ȡm_cPacketsList���ʿ���
CListCtrl* CKSnifferDlg::GetPacketsListWnd(void)
{
	return &m_cPacketsList;
}

// ��ȡm_cPacketNumBar���ʿ���
CStatic* CKSnifferDlg::GetPacketNumBarWnd(void)
{
	return &m_cPacketNumBar;
}

void CKSnifferDlg::UpdatePackNum(DWORD dwTcp, DWORD dwUdp, 
		DWORD dwArp, DWORD dwIgmp, DWORD dwIcmp)
{
	m_dwTcp = dwTcp;
	m_dwUdp = dwUdp;
	m_dwArp = dwArp;
	m_dwIgmp = dwIgmp;
	m_dwIcmp = dwIcmp;

	WCHAR szBuffer[64] = {0};
	wsprintf(szBuffer, _T("TCP %d UDP %d ARP %d IGMP %d ICMP %d"),
		m_dwTcp, m_dwUdp, m_dwArp, m_dwIgmp, m_dwIcmp);
	m_cPacketNumBar.SetWindowText(szBuffer);
}

// ��ʼ��WinPcap
BOOL CKSnifferDlg::InitWinpcap(void)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	USES_CONVERSION;

	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &m_pAllDevs, errbuf) == -1)
	{
		WCHAR *pszErrBuf = A2W(errbuf);
		MessageBox(pszErrBuf, _T("WinPcap��ʼ������"), MB_ICONERROR);
		return FALSE;
	}

	m_nCurAdapter = -1;
	ZeroMemory(&m_sCapFilter, sizeof(CaptureFilter)); // ������
	m_sCapFilter.dwProtocol = 31; // ȫ����λ
	m_bCapWorking = FALSE;

	return TRUE;
}

// ����ʼ��WinPcap
void CKSnifferDlg::UnInitWinpcap(void)
{
	if (m_pAllDevs != NULL)
	{
		pcap_freealldevs(m_pAllDevs);
	}
}

// �ͷ���Դ
void CKSnifferDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	// ����ʼ��WinPcap
	UnInitWinpcap();
	// �ͷŷ���Ľڵ��ڴ�
	DeletePacket();

	CDialogEx::OnClose();
}

pcap_if_t* CKSnifferDlg::GetAllDevs(void)
{
	return m_pAllDevs;
}

void CKSnifferDlg::SetCurAdapter(int nIndex)
{
	m_nCurAdapter = nIndex;
}

void CKSnifferDlg::SetCurAdapter(CString strInfo)
{
	m_strCurAdapter = strInfo;
}

// ����ѡ�������Ի���
void CKSnifferDlg::OnSelectAdapters()
{
	if (m_bCapWorking)
	{
		MessageBox(_T("�Ѿ�����ץ��״̬�У����������������������ֹͣ����ץ����"),
			_T("��ʾ��Ϣ"), MB_ICONWARNING);
		return ;
	}

	CAdaptersDlg dlg(this);
	dlg.DoModal();

	m_cStatusCurAdapter.SetWindowText(m_strCurAdapter);
}

// �����������
void CKSnifferDlg::OnSetCapFilter()
{
	if (m_bCapWorking)
	{
		MessageBox(_T("�Ѿ�����ץ��״̬�У��������ù��˹�������ֹͣ����ץ����"),
			_T("��ʾ��Ϣ"), MB_ICONWARNING);
		return ;
	}

	CCapFilterDlg dlg(this, m_sCapFilter);
	dlg.DoModal();
}

void CKSnifferDlg::SetCapFilter(CaptureFilter sCapFilter)
{
	m_sCapFilter = sCapFilter;
}


void CKSnifferDlg::OnOptStartcap()
{
	if (m_bCapWorking)
	{
		MessageBox(_T("�Ѿ�����ץ��״̬�У��������¿�ʼ����ֹͣ����ץ����"),
			_T("��ʾ��Ϣ"), MB_ICONWARNING);
		return ;
	}

	int i = 0;
	pcap_if_t *d = m_pAllDevs;
	while (i < m_nCurAdapter)
	{
		++i;
		d = d->next;
	}

	// �������߳̿�ʼץ������
	SAFE_DELETE(m_tCapThread);
	m_tCapThread = new CCapThread(d, m_sCapFilter);
    if (-1 != m_tCapThread->ResumeThread())
	{
		m_bCapWorking = TRUE;
	}
}

// ֹͣץ���߳�
void CKSnifferDlg::OnOptStopcap()
{
	if (!m_bCapWorking)
	{
		MessageBox(_T("��ǰδ����ץ��״̬�����β������ԣ�"),
			_T("��ʾ��Ϣ"), MB_ICONWARNING);
		return ;
	}

	SAFE_DELETE(m_tCapThread);
	m_bCapWorking = FALSE;
}

// ����CListCtrl�е�һ��ʱ��ʾ��Ӧ����ϸ����
void CKSnifferDlg::OnNMClickListPackets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwIndex = pNMItemActivate->iItem;
	DWORD dwSize = g_vPacketChain.size();

	if (dwIndex >= 0 && dwIndex < dwSize)
	{
		PacketNode *pNode = g_vPacketChain[dwIndex];
		// ���οؼ�������ʾ
		UpdateTreeViewData(dwIndex);

		// BugFix: ʮ�����ƿؼ��Ĵ�ֱ�������е�����
		// �����������β�����
		m_cHexView.SetData(pNode->pData, pNode->pHeader->caplen);
		m_cHexView.SetData(pNode->pData, pNode->pHeader->caplen);
		m_cHexView.Invalidate();
	}

	*pResult = 0;
}

void CKSnifferDlg::UpdateTreeViewData(DWORD dwIndex)
{
	PacketNode *pNode = g_vPacketChain[dwIndex];
	
	EthernetHeader *pEthdr = (EthernetHeader *)(pNode->pData); 
	static TCHAR szSrcMac[64] = {0};
	static TCHAR szDstMac[64] = {0};
	static TCHAR szMacFmt[64] = _T("%s: %02X-%02X-%02X-%02X-%02X-%02X");
	static TCHAR szProtocolType[32] = {0};
	
	// ɾ���ڵ�ʱ�������Ӧ��ɾ�������������ͷſռ�
	m_cTreeView.DeleteAllItems();		// ������нڵ�

	wsprintf(szSrcMac, szMacFmt, _T("Source"),
		pEthdr->sSrcMac.a, pEthdr->sSrcMac.b,
		pEthdr->sSrcMac.c, pEthdr->sSrcMac.d, 
		pEthdr->sSrcMac.e, pEthdr->sSrcMac.f);
	wsprintf(szDstMac, szMacFmt, _T("Destination"),
		pEthdr->sDstMac.a, pEthdr->sDstMac.b,
		pEthdr->sDstMac.c, pEthdr->sDstMac.d, 
		pEthdr->sDstMac.e, pEthdr->sDstMac.f);

	// [�ڵ�] Ethernetͷ������
	HTREEITEM hEthernet = m_cTreeView.InsertItem(_T("Ethernet"));
	TreeNodeData *pTndEthernet = (TreeNodeData *)malloc(sizeof(TreeNodeData));
	pTndEthernet->dwStartPos = 0;
	pTndEthernet->dwEndPos = pTndEthernet->dwStartPos + sizeof(EthernetHeader);
	m_cTreeView.SetItemData(hEthernet, (DWORD_PTR)pTndEthernet);

	// [�ڵ�] Ŀ��MAC��ַ
	HTREEITEM hEthDstMac = m_cTreeView.InsertItem(szDstMac, hEthernet);
	TreeNodeData *pTndEthDstMac = (TreeNodeData *)malloc(sizeof(TreeNodeData));
	pTndEthDstMac->dwStartPos = 0;
	pTndEthDstMac->dwEndPos = pTndEthDstMac->dwStartPos + sizeof(MacAddr);
	m_cTreeView.SetItemData(hEthDstMac, (DWORD_PTR)pTndEthDstMac);

	// [�ڵ�] ԴMAC��ַ
	HTREEITEM hEthSrcMac = m_cTreeView.InsertItem(szSrcMac, hEthernet);
	TreeNodeData *pTndEthSrcMac = (TreeNodeData *)malloc(sizeof(TreeNodeData));
	pTndEthSrcMac->dwStartPos = pTndEthDstMac->dwEndPos;
	pTndEthSrcMac->dwEndPos = pTndEthSrcMac->dwStartPos + sizeof(MacAddr);
	m_cTreeView.SetItemData(hEthSrcMac, (DWORD_PTR)pTndEthSrcMac);

	// Э�����͸�������
	TreeNodeData *pTndEthProtoType = (TreeNodeData *)malloc(sizeof(TreeNodeData));
	pTndEthProtoType->dwStartPos = pTndEthSrcMac->dwEndPos;
	pTndEthProtoType->dwEndPos = pTndEthProtoType->dwStartPos + 2;
	
	// ==========================================================================
	// ARP���ݰ�����
	// ==========================================================================
	if (ntohs(pEthdr->nEthType) == 0x0806)	// ARP
	{
		static TCHAR szSrcIp[128];
		static TCHAR szSrcMac[128];
		static TCHAR szDstIp[128];
		static TCHAR szDstMac[128];
		static TCHAR szHardwareType[32];
		// static TCHAR szProtocolType[32];
		static TCHAR szHardwareSize[32];
		static TCHAR szProtocolSize[32];
		static TCHAR szOpcode[32];
		static TCHAR szOpString[3][16] = {_T("Not defined"), _T("Request"), _T("Response")};

		// ARPͷ����ʽ����
		ArpHeader *pArpHdr = (ArpHeader *)(pNode->pData + sizeof(EthernetHeader));

		// ���ͷ�MAC��ַ��IP��ַ
		wsprintf(szSrcMac, _T("Sender MAC address: %02X-%02X-%02X-%02X-%02X-%02X"),
				pArpHdr->sSrcMac.a, pArpHdr->sSrcMac.b, pArpHdr->sSrcMac.c,
				pArpHdr->sSrcMac.d, pArpHdr->sSrcMac.e, pArpHdr->sSrcMac.f);
		wsprintf(szSrcIp, _T("Sender IP address: %d.%d.%d.%d"), pArpHdr->sSrcIp.a, 
			pArpHdr->sSrcIp.b, pArpHdr->sSrcIp.c, pArpHdr->sSrcIp.d);

		// ���շ�MAC��ַ��IP��ַ
		wsprintf(szDstMac, _T("Target MAC address: %02X-%02X-%02X-%02X-%02X-%02X"),
				pArpHdr->sDstMac.a, pArpHdr->sDstMac.b, pArpHdr->sDstMac.c,
				pArpHdr->sDstMac.d, pArpHdr->sDstMac.e, pArpHdr->sDstMac.f);
		wsprintf(szDstIp, _T("Target IP address: %d.%d.%d.%d"), pArpHdr->sDstIp.a, 
			pArpHdr->sDstIp.b, pArpHdr->sDstIp.c, pArpHdr->sDstIp.d);
		
		// ARP������
		unsigned short uOpcode = ntohs(pArpHdr->nOpCode);
		if (uOpcode > 2) uOpcode = 0; // δ����

		// [�ڵ�] EthernetЭ���ֶ�: Э������
		wcscpy(szProtocolType, _T("Type: ARP (0x0806)"));
		HTREEITEM hEthProtoType = m_cTreeView.InsertItem(szProtocolType, hEthernet);
		m_cTreeView.SetItemData(hEthProtoType, (DWORD_PTR)pTndEthProtoType);

		// [�ڵ�] ARPͷ������
		HTREEITEM hArp = m_cTreeView.InsertItem(_T("Address Resolution Protocol"));
		TreeNodeData *pTndArp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndArp->dwStartPos = pTndEthProtoType->dwEndPos;
		pTndArp->dwEndPos = pTndArp->dwStartPos + sizeof(ArpHeader);
		m_cTreeView.SetItemData(hArp, (DWORD_PTR)pTndArp);

		// [�ڵ�] Ӳ������
		wsprintf(szHardwareType, _T("Hardware type: 0x%04X"), 
			ntohs(pArpHdr->nHardType));
		HTREEITEM hHardwareType = m_cTreeView.InsertItem(szHardwareType, hArp);
		TreeNodeData *pTndHardwareType = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndHardwareType->dwStartPos = pTndArp->dwStartPos;
		pTndHardwareType->dwEndPos = pTndHardwareType->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hHardwareType, (DWORD_PTR)pTndHardwareType);

		// [�ڵ�] Э������
		wsprintf(szProtocolType, _T("Protocol type: 0x%04X"), 
			ntohs(pArpHdr->nProtoType));
		HTREEITEM hProtocolType = m_cTreeView.InsertItem(szProtocolType, hArp);
		TreeNodeData *pTndProtocolType = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndProtocolType->dwStartPos = pTndHardwareType->dwEndPos;
		pTndProtocolType->dwEndPos = pTndProtocolType->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hProtocolType, (DWORD_PTR)pTndProtocolType);

		// [�ڵ�] Ӳ����С
		wsprintf(szHardwareSize, _T("Hardware size: 0x%02X"), 
			pArpHdr->nMacLen);
		HTREEITEM hHardwareSize = m_cTreeView.InsertItem(szHardwareSize, hArp);
		TreeNodeData *pTndHardwareSize = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndHardwareSize->dwStartPos = pTndProtocolType->dwEndPos;
		pTndHardwareSize->dwEndPos = pTndHardwareSize->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hHardwareSize, (DWORD_PTR)pTndHardwareSize);

		// [�ڵ�] Э���С
		wsprintf(szProtocolSize, _T("Protocol size: 0x%02X"), 
			pArpHdr->nProtoLen);
		HTREEITEM hProtocolSize = m_cTreeView.InsertItem(szProtocolSize, hArp);
		TreeNodeData *pTndProtocolSize = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndProtocolSize->dwStartPos = pTndHardwareSize->dwEndPos;
		pTndProtocolSize->dwEndPos = pTndProtocolSize->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hProtocolSize, (DWORD_PTR)pTndProtocolSize);

		// [�ڵ�] ������
		wsprintf(szOpcode, _T("Opcode: 0x%04X (%s)"), 
			ntohs(pArpHdr->nOpCode), szOpString[uOpcode]);
		HTREEITEM hOpcode = m_cTreeView.InsertItem(szOpcode, hArp);
		TreeNodeData *pTndOpcode = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndOpcode->dwStartPos = pTndProtocolSize->dwEndPos;
		pTndOpcode->dwEndPos = pTndOpcode->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hOpcode, (DWORD_PTR)pTndOpcode);

		// [�ڵ�] ���ͷ�MAC��ַ
		HTREEITEM hSrcMac = m_cTreeView.InsertItem(szSrcMac, hArp);
		TreeNodeData *pTndSrcMac = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndSrcMac->dwStartPos = pTndOpcode->dwEndPos;
		pTndSrcMac->dwEndPos = pTndSrcMac->dwStartPos + sizeof(MacAddr);
		m_cTreeView.SetItemData(hSrcMac, (DWORD_PTR)pTndSrcMac);

		// [�ڵ�] ���ͷ�IP��ַ
		HTREEITEM hSrcIp = m_cTreeView.InsertItem(szSrcIp, hArp);
		TreeNodeData *pTndSrcIp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndSrcIp->dwStartPos = pTndSrcMac->dwEndPos;
		pTndSrcIp->dwEndPos = pTndSrcIp->dwStartPos + sizeof(IpAddr);
		m_cTreeView.SetItemData(hSrcIp, (DWORD_PTR)pTndSrcIp);

		// [�ڵ�] ���շ�MAC��ַ
		HTREEITEM hDstMac = m_cTreeView.InsertItem(szDstMac, hArp);
		TreeNodeData *pTndDstMac = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndDstMac->dwStartPos = pTndSrcIp->dwEndPos;
		pTndDstMac->dwEndPos = pTndDstMac->dwStartPos + sizeof(MacAddr);
		m_cTreeView.SetItemData(hDstMac, (DWORD_PTR)pTndDstMac);

		// [�ڵ�] ���շ�IP��ַ
		HTREEITEM hDstIp = m_cTreeView.InsertItem(szDstIp, hArp);
		TreeNodeData *pTndDstIp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndDstIp->dwStartPos = pTndDstMac->dwEndPos;
		pTndDstIp->dwEndPos = pTndDstIp->dwStartPos + sizeof(IpAddr);
		m_cTreeView.SetItemData(hDstIp, (DWORD_PTR)pTndDstIp);
	}
	// ==========================================================================
	// IP���ݰ�����
	// ==========================================================================
	else if (ntohs(pEthdr->nEthType) == 0x0800)
	{
		IpHeader *pIpHdr = (IpHeader *)(pNode->pData + sizeof(EthernetHeader));
		DWORD dwIpHdrLen = pIpHdr->nHeaderLen * 4;	// һ��Ҫ����4

		// [�ڵ�] EthernetЭ���ֶ�: Э������
		wcscpy(szProtocolType, _T("Type: IP (0x0800)"));
		HTREEITEM hEthProtoType = m_cTreeView.InsertItem(szProtocolType, hEthernet);
		m_cTreeView.SetItemData(hEthProtoType, (DWORD_PTR)pTndEthProtoType);

		// [�ڵ�] IPͷ������
		HTREEITEM hIp = m_cTreeView.InsertItem(_T("Internet Protocol"));
		TreeNodeData *pTndIp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndIp->dwStartPos = pTndEthProtoType->dwEndPos;
		pTndIp->dwEndPos = pTndIp->dwStartPos + sizeof(IpHeader);
		m_cTreeView.SetItemData(hIp, (DWORD_PTR)pTndIp);

		// [�ڵ�] IP�汾
		static WCHAR szIpVer[16];
		wsprintf(szIpVer, _T("Version: %u"), pIpHdr->nVer);
		HTREEITEM hIpVer = m_cTreeView.InsertItem(szIpVer, hIp);
		TreeNodeData *pTndIpVer = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndIpVer->dwStartPos = pTndIp->dwStartPos;
		pTndIpVer->dwEndPos = pTndIpVer->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hIpVer, (DWORD_PTR)pTndIpVer);

		// [�ڵ�] IP ͷ������
		static WCHAR szIpHdrLen[32];
		wsprintf(szIpHdrLen, _T("Header length: %u bytes"), dwIpHdrLen);
		HTREEITEM hIpHdrLen = m_cTreeView.InsertItem(szIpHdrLen, hIp);
		TreeNodeData *pTndIpHdrLen = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndIpHdrLen->dwStartPos = pTndIp->dwStartPos;
		pTndIpHdrLen->dwEndPos = pTndIpHdrLen->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hIpHdrLen, (DWORD_PTR)pTndIpHdrLen);

		// [�ڵ�] IP ��������
		static WCHAR szIpTos[64];
		wsprintf(szIpTos, _T("Differentiated Services Field: 0x%02X"), pIpHdr->nTos);
		HTREEITEM hIpTos = m_cTreeView.InsertItem(szIpTos, hIp);
		TreeNodeData *pTndIpTos = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndIpTos->dwStartPos = pTndIpHdrLen->dwEndPos;
		pTndIpTos->dwEndPos = pTndIpTos->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hIpTos, (DWORD_PTR)pTndIpTos);

		// [�ڵ�] IP �ܳ���
		static WCHAR szIpTotalLen[64];
		wsprintf(szIpTotalLen, _T("Total length: 0x%04X"), ntohs(pIpHdr->nTotalLen));
		HTREEITEM hIpTotalLen = m_cTreeView.InsertItem(szIpTotalLen, hIp);
		TreeNodeData *pTndTotalLen = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndTotalLen->dwStartPos = pTndIpTos->dwEndPos;
		pTndTotalLen->dwEndPos = pTndTotalLen->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hIpTotalLen, (DWORD_PTR)pTndTotalLen);

		// [�ڵ�] IP ��ʶ
		static WCHAR szIpIdent[64];
		wsprintf(szIpIdent, _T("Identification: 0x%04X"), ntohs(pIpHdr->nIdent));
		HTREEITEM hIpIdent = m_cTreeView.InsertItem(szIpIdent, hIp);
		TreeNodeData *pTndIdent = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndIdent->dwStartPos = pTndTotalLen->dwEndPos;
		pTndIdent->dwEndPos = pTndIdent->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hIpIdent, (DWORD_PTR)pTndIdent);

		// [�ڵ�] IP ��Ƭƫ��
		static WCHAR szIpFragOff[64];
		wsprintf(szIpFragOff, _T("Fragment offset: %u"), ntohs(pIpHdr->nFragOff));
		HTREEITEM hIpFragOff = m_cTreeView.InsertItem(szIpFragOff, hIp);
		TreeNodeData *pTndFragOff = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndFragOff->dwStartPos = pTndIdent->dwEndPos;
		pTndFragOff->dwEndPos = pTndFragOff->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hIpFragOff, (DWORD_PTR)pTndFragOff);

		// [�ڵ�] IP TTL
		static WCHAR szIpTtl[64];
		wsprintf(szIpTtl, _T("Time to live: %u"), pIpHdr->nTtl);
		HTREEITEM hIpTtl = m_cTreeView.InsertItem(szIpTtl, hIp);
		TreeNodeData *pTndTtl = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndTtl->dwStartPos = pTndFragOff->dwEndPos;
		pTndTtl->dwEndPos = pTndTtl->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hIpTtl, (DWORD_PTR)pTndTtl);

		// [�ڵ�] IP Э������
		static WCHAR szIpProto[64];
		static WCHAR szIpProtoType[16] = {_T("Unknown")};
		if (pIpHdr->nProtocol == 1) wcscpy(szIpProtoType, _T("ICMP"));
		else if (pIpHdr->nProtocol == 2) wcscpy(szIpProtoType, _T("IGMP"));
		else if (pIpHdr->nProtocol == 6) wcscpy(szIpProtoType, _T("TCP"));
		else if (pIpHdr->nProtocol == 17) wcscpy(szIpProtoType, _T("UDP"));
		wsprintf(szIpProto, _T("Protocol: %s (%u)"), szIpProtoType, pIpHdr->nProtocol);
		HTREEITEM hIpProto = m_cTreeView.InsertItem(szIpProto, hIp);
		TreeNodeData *pTndProto = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndProto->dwStartPos = pTndTtl->dwEndPos;
		pTndProto->dwEndPos = pTndProto->dwStartPos + sizeof(unsigned char);
		m_cTreeView.SetItemData(hIpProto, (DWORD_PTR)pTndProto);

		// [�ڵ�] IP ͷ��У��
		static WCHAR szIpCrc[64];
		wsprintf(szIpCrc, _T("Header checksum: 0x%04X"), ntohs(pIpHdr->nCrc));
		HTREEITEM hIpCrc = m_cTreeView.InsertItem(szIpCrc, hIp);
		TreeNodeData *pTndCrc = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndCrc->dwStartPos = pTndProto->dwEndPos;
		pTndCrc->dwEndPos = pTndCrc->dwStartPos + sizeof(unsigned short);
		m_cTreeView.SetItemData(hIpCrc, (DWORD_PTR)pTndCrc);

		// [�ڵ�] IP ���ͷ�IP��ַ
		static WCHAR szIpSrcIp[64];
		wsprintf(szIpSrcIp, _T("Source: %d.%d.%d.%d"), pIpHdr->sSrcIp.a,
			pIpHdr->sSrcIp.b, pIpHdr->sSrcIp.c, pIpHdr->sSrcIp.d);
		HTREEITEM hIpSrcIp = m_cTreeView.InsertItem(szIpSrcIp, hIp);
		TreeNodeData *pTndSrcIp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndSrcIp->dwStartPos = pTndCrc->dwEndPos;
		pTndSrcIp->dwEndPos = pTndSrcIp->dwStartPos + sizeof(IpAddr);
		m_cTreeView.SetItemData(hIpSrcIp, (DWORD_PTR)pTndSrcIp);

		// [�ڵ�] IP ���շ�IP��ַ
		static WCHAR szIpDstIp[64];
		wsprintf(szIpDstIp, _T("Source: %d.%d.%d.%d"), pIpHdr->sDstIp.a,
			pIpHdr->sDstIp.b, pIpHdr->sDstIp.c, pIpHdr->sDstIp.d);
		HTREEITEM hIpDstIp = m_cTreeView.InsertItem(szIpDstIp, hIp);
		TreeNodeData *pTndDstIp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
		pTndDstIp->dwStartPos = pTndSrcIp->dwEndPos;
		pTndDstIp->dwEndPos = pTndDstIp->dwStartPos + sizeof(IpAddr);
		m_cTreeView.SetItemData(hIpDstIp, (DWORD_PTR)pTndDstIp);

		// ======================================================================
		// ICMP���ݰ�����
		// ======================================================================
		if (pIpHdr->nProtocol == 1)
		{
			// [�ڵ�] ICMPͷ������
			HTREEITEM hIcmp = m_cTreeView.InsertItem(_T("Internet Control Message Protocol"));
			TreeNodeData *pTndIcmp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIcmp->dwStartPos = pTndIp->dwStartPos + dwIpHdrLen;
			pTndIcmp->dwEndPos = pNode->pHeader->caplen;
			m_cTreeView.SetItemData(hIcmp, (DWORD_PTR)pTndIcmp);

			// [�ڵ�] ICMP ����
			static WCHAR szIcmpType[128];
			IcmpHeader *pIcmpHdr = (IcmpHeader *)((BYTE *)pIpHdr + dwIpHdrLen);
			if (pIcmpHdr->nType >= 0 && pIcmpHdr->nType <= ICMP_TYPE_VALUE_MAX)
			{
				wsprintf(szIcmpType, _T("Type: %d (%s)"),
					pIcmpHdr->nType,
					g_szIcmpType[pIcmpHdr->nType]);
			}
			else
			{
				wsprintf(szIcmpType, _T("Type: %d (Type not defined)"),
					pIcmpHdr->nType);
			}
			HTREEITEM hIcmpType = m_cTreeView.InsertItem(szIcmpType, hIcmp);
			TreeNodeData *pTndIcmpType = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIcmpType->dwStartPos = pTndIcmp->dwStartPos;
			pTndIcmpType->dwEndPos = pTndIcmpType->dwStartPos + sizeof(unsigned char);
			m_cTreeView.SetItemData(hIcmpType, (DWORD_PTR)pTndIcmpType);
			
			// [�ڵ�] ICMP Code
			static WCHAR szIcmpCode[16];
			wsprintf(szIcmpCode, _T("Code: %d"), pIcmpHdr->nCode);
			HTREEITEM hIcmpCode = m_cTreeView.InsertItem(szIcmpCode, hIcmp);
			TreeNodeData *pTndIcmpCode = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIcmpCode->dwStartPos = pTndIcmpType->dwEndPos;
			pTndIcmpCode->dwEndPos = pTndIcmpCode->dwStartPos + sizeof(unsigned char);
			m_cTreeView.SetItemData(hIcmpCode, (DWORD_PTR)pTndIcmpCode);

			// [�ڵ�] ICMP У��
			static WCHAR szIcmpCrc[32];
			wsprintf(szIcmpCrc, _T("Checksum: 0x%04X"), ntohs(pIcmpHdr->nCheckSum));
			HTREEITEM hIcmpCrc = m_cTreeView.InsertItem(szIcmpCrc, hIcmp);
			TreeNodeData *pTndIcmpCrc = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIcmpCrc->dwStartPos = pTndIcmpCode->dwEndPos;
			pTndIcmpCrc->dwEndPos = pTndIcmpCrc->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hIcmpCrc, (DWORD_PTR)pTndIcmpCrc);
		}
		// ======================================================================
		// IGMP���ݰ�����
		// ======================================================================
		else if (pIpHdr->nProtocol == 2)
		{
			// [�ڵ�] IGMPͷ������
			HTREEITEM hIgmp = m_cTreeView.InsertItem(_T("Internet Group Management Protocol"));
			TreeNodeData *pTndIgmp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIgmp->dwStartPos = pTndIp->dwStartPos + dwIpHdrLen;
			pTndIgmp->dwEndPos = pNode->pHeader->caplen;
			m_cTreeView.SetItemData(hIgmp, (DWORD_PTR)pTndIgmp);

			// [�ڵ�] IGMP ����
			static WCHAR szIgmpType[128];
			IgmpHeader *pIgmpHdr = (IgmpHeader *)((BYTE *)pIpHdr + dwIpHdrLen);
			if (pIgmpHdr->nType >= 0 && pIgmpHdr->nType <= IGMP_TYPE_VALUE_MAX)
			{
				wsprintf(szIgmpType, _T("Type: %d (%s)"),
					pIgmpHdr->nType,
					g_szIgmpType[pIgmpHdr->nType]);
			}
			else
			{
				wsprintf(szIgmpType, _T("Type: %d (Type not defined)"),
					pIgmpHdr->nType);
			}
			HTREEITEM hIgmpType = m_cTreeView.InsertItem(szIgmpType, hIgmp);
			TreeNodeData *pTndIgmpType = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIgmpType->dwStartPos = pTndIgmp->dwStartPos;
			pTndIgmpType->dwEndPos = pTndIgmpType->dwStartPos + sizeof(unsigned char);
			m_cTreeView.SetItemData(hIgmpType, (DWORD_PTR)pTndIgmpType);
			
			// [�ڵ�] IGMP Code
			static WCHAR szIgmpCode[16];
			wsprintf(szIgmpCode, _T("Code: %d"), pIgmpHdr->nCode);
			HTREEITEM hIgmpCode = m_cTreeView.InsertItem(szIgmpCode, hIgmp);
			TreeNodeData *pTndIgmpCode = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIgmpCode->dwStartPos = pTndIgmpType->dwEndPos;
			pTndIgmpCode->dwEndPos = pTndIgmpCode->dwStartPos + sizeof(unsigned char);
			m_cTreeView.SetItemData(hIgmpCode, (DWORD_PTR)pTndIgmpCode);

			// [�ڵ�] IGMP У��
			static WCHAR szIgmpCrc[32];
			wsprintf(szIgmpCrc, _T("Checksum: 0x%04X"), ntohs(pIgmpHdr->nCheckSum));
			HTREEITEM hIgmpCrc = m_cTreeView.InsertItem(szIgmpCrc, hIgmp);
			TreeNodeData *pTndIcmpCrc = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndIcmpCrc->dwStartPos = pTndIgmpCode->dwEndPos;
			pTndIcmpCrc->dwEndPos = pTndIcmpCrc->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hIgmpCrc, (DWORD_PTR)hIgmpCrc);
		}
		// ======================================================================
		// TCP���ݰ�����
		// ======================================================================
		else if (pIpHdr->nProtocol == 6)
		{
			TcpHeader *pTcpHdr = (TcpHeader *)((BYTE*)pIpHdr + dwIpHdrLen);

			// [�ڵ�] TCPͷ������
			HTREEITEM hTcp= m_cTreeView.InsertItem(_T("Transmission Control Protocol"));
			TreeNodeData *pTndTcp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcp->dwStartPos = pTndIp->dwStartPos + dwIpHdrLen;
			pTndTcp->dwEndPos = pTndTcp->dwStartPos + sizeof(TcpHeader);
			m_cTreeView.SetItemData(hTcp, (DWORD_PTR)pTndTcp);

			// [�ڵ�] TCP ���ͷ��˿�
			static WCHAR szTcpSrcPort[32];
			wsprintf(szTcpSrcPort, _T("Source port: %d"), ntohs(pTcpHdr->nSrcPort));
			HTREEITEM hTcpSrcPort = m_cTreeView.InsertItem(szTcpSrcPort, hTcp);
			TreeNodeData *pTndTcpSrcPort = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpSrcPort->dwStartPos = pTndTcp->dwStartPos;
			pTndTcpSrcPort->dwEndPos = pTndTcpSrcPort->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hTcpSrcPort, (DWORD_PTR)pTndTcpSrcPort);

			// [�ڵ�] TCP ���շ��˿�
			static WCHAR szTcpDstPort[32];
			wsprintf(szTcpDstPort, _T("Destination port: %d"), ntohs(pTcpHdr->nDstPort));
			HTREEITEM hTcpDstPort = m_cTreeView.InsertItem(szTcpDstPort, hTcp);
			TreeNodeData *pTndTcpDstPort = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpDstPort->dwStartPos = pTndTcpSrcPort->dwEndPos;
			pTndTcpDstPort->dwEndPos = pTndTcpDstPort->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hTcpDstPort, (DWORD_PTR)pTndTcpDstPort);

			// [�ڵ�] TCP SeqNum
			static WCHAR szTcpSeqNum[32];
			wsprintf(szTcpSeqNum, _T("SeqNum: %lu"), ntohl(pTcpHdr->nSeqNum));
			HTREEITEM hTcpSeqNum = m_cTreeView.InsertItem(szTcpSeqNum, hTcp);
			TreeNodeData *pTndTcpSeqNum = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpSeqNum->dwStartPos = pTndTcpDstPort->dwEndPos;
			pTndTcpSeqNum->dwEndPos = pTndTcpSeqNum->dwStartPos + sizeof(unsigned long);
			m_cTreeView.SetItemData(hTcpSeqNum, (DWORD_PTR)pTndTcpSeqNum);

			// [�ڵ�] TCP AckNum
			static WCHAR szTcpAckNum[32];
			wsprintf(szTcpAckNum, _T("AckNum: %lu"), ntohl(pTcpHdr->nAckNum));
			HTREEITEM hTcpAckNum = m_cTreeView.InsertItem(szTcpAckNum, hTcp);
			TreeNodeData *pTndTcpAckNum = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpAckNum->dwStartPos = pTndTcpSeqNum->dwEndPos;
			pTndTcpAckNum->dwEndPos = pTndTcpAckNum->dwStartPos + sizeof(unsigned long);
			m_cTreeView.SetItemData(hTcpAckNum, (DWORD_PTR)pTndTcpAckNum);

			// [�ڵ�] TCP HeaderLen
			static WCHAR szTcpHeaderLen[32];
			wsprintf(szTcpHeaderLen, _T("Header length: %d"), pTcpHdr->nHeaderLen);
			HTREEITEM hTcpHeaderLen = m_cTreeView.InsertItem(szTcpHeaderLen, hTcp);
			TreeNodeData *pTndTcpHeaderLen = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpHeaderLen->dwStartPos = pTndTcpAckNum->dwEndPos;
			pTndTcpHeaderLen->dwEndPos = pTndTcpHeaderLen->dwStartPos + sizeof(unsigned char);
			m_cTreeView.SetItemData(hTcpHeaderLen, (DWORD_PTR)pTndTcpHeaderLen);

			// [�ڵ�] TCP Flags
			WCHAR szFlags[32] = {0};
			if (pTcpHdr->bFin) wcscat(szFlags, _T("FIN,"));
			if (pTcpHdr->bSyn) wcscat(szFlags, _T("SYN,"));
			if (pTcpHdr->bRst) wcscat(szFlags, _T("RST,"));
			if (pTcpHdr->bPsh) wcscat(szFlags, _T("PSH,"));
			if (pTcpHdr->bAck) wcscat(szFlags, _T("ACK,"));
			if (pTcpHdr->bUgr) wcscat(szFlags, _T("UGR,"));
			if (wcslen(szFlags) != 0)
			{
				szFlags[wcslen(szFlags)-1] = _T('\0');
			}
			static WCHAR szTcpFlags[32];
			wsprintf(szTcpFlags, _T("Flags: %s"), szFlags);
			HTREEITEM hTcpFlags = m_cTreeView.InsertItem(szTcpFlags, hTcp);
			TreeNodeData *pTndTcpFlags = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpFlags->dwStartPos = pTndTcpHeaderLen->dwEndPos;
			pTndTcpFlags->dwEndPos = pTndTcpFlags->dwStartPos + sizeof(unsigned char);
			m_cTreeView.SetItemData(hTcpFlags, (DWORD_PTR)pTndTcpFlags);
			
			// [�ڵ�] TCP WinSize
			static WCHAR szTcpWinSize[32];
			wsprintf(szTcpWinSize, _T("Window size value: %u"), ntohl(pTcpHdr->nWinSize));
			HTREEITEM hTcpWinSize = m_cTreeView.InsertItem(szTcpWinSize, hTcp);
			TreeNodeData *pTndTcpWinSize = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpWinSize->dwStartPos = pTndTcpFlags->dwEndPos;
			pTndTcpWinSize->dwEndPos = pTndTcpWinSize->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hTcpWinSize, (DWORD_PTR)pTndTcpWinSize);
			
			// [�ڵ�] TCP CheckSum
			static WCHAR szTcpCheckSum[32];
			wsprintf(szTcpCheckSum, _T("Checksum: 0x%02X"), ntohl(pTcpHdr->nCheckSum));
			HTREEITEM hTcpCheckSum = m_cTreeView.InsertItem(szTcpCheckSum, hTcp);
			TreeNodeData *pTndTcpCheckSum = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpCheckSum->dwStartPos = pTndTcpWinSize->dwEndPos;
			pTndTcpCheckSum->dwEndPos = pTndTcpCheckSum->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hTcpCheckSum, (DWORD_PTR)pTndTcpCheckSum);

			// [�ڵ�] TCP UrgPtr
			static WCHAR szTcpUrgPtr[32];
			wsprintf(szTcpUrgPtr, _T("UrgPtr: 0x%02X"), ntohl(pTcpHdr->nUrgPtr));
			HTREEITEM hTcpUrgPtr = m_cTreeView.InsertItem(szTcpUrgPtr, hTcp);
			TreeNodeData *pTndTcpUrgPtr = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndTcpUrgPtr->dwStartPos = pTndTcpCheckSum->dwEndPos;
			pTndTcpUrgPtr->dwEndPos = pTndTcpUrgPtr->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hTcpUrgPtr, (DWORD_PTR)pTndTcpUrgPtr);

			// [�ڵ�] TCP ����
			if (pTndTcpUrgPtr->dwEndPos < pNode->pHeader->caplen)
			{
				HTREEITEM hTcpData = m_cTreeView.InsertItem(_T("Data"), hTcp);
				TreeNodeData *pTndTcpData = (TreeNodeData *)malloc(sizeof(TreeNodeData));
				pTndTcpData->dwStartPos = pTndTcpUrgPtr->dwEndPos;
				pTndTcpData->dwEndPos = pNode->pHeader->caplen;
				m_cTreeView.SetItemData(hTcpData, (DWORD_PTR)pTndTcpData);
			}
		}
		// ======================================================================
		// UDP���ݰ�����
		// ======================================================================
		else if (pIpHdr->nProtocol == 17)
		{
			UdpHeader *pUdpHdr = (UdpHeader *)((BYTE*)pIpHdr + dwIpHdrLen);
			// [�ڵ�] UDPͷ������
			HTREEITEM hUdp = m_cTreeView.InsertItem(_T("User Datagram Protocol"));
			TreeNodeData *pTndUdp = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndUdp->dwStartPos = pTndIp->dwStartPos + dwIpHdrLen;
			pTndUdp->dwEndPos = pTndUdp->dwStartPos + sizeof(UdpHeader);
			m_cTreeView.SetItemData(hUdp, (DWORD_PTR)pTndUdp);

			// [�ڵ�] UDP ���ͷ��˿�
			static WCHAR szUdpSrcPort[32];
			wsprintf(szUdpSrcPort, _T("Source port: %d"), ntohs(pUdpHdr->nSrcPort));
			HTREEITEM hUdpSrcPort = m_cTreeView.InsertItem(szUdpSrcPort, hUdp);
			TreeNodeData *pTndUdpSrcPort = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndUdpSrcPort->dwStartPos = pTndUdp->dwStartPos;
			pTndUdpSrcPort->dwEndPos = pTndUdpSrcPort->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hUdpSrcPort, (DWORD_PTR)pTndUdpSrcPort);

			// [�ڵ�] UDP ���շ��˿�
			static WCHAR szUdpDstPort[32];
			wsprintf(szUdpDstPort, _T("Destination port: %d"), ntohs(pUdpHdr->nDstPort));
			HTREEITEM hUdpDstPort = m_cTreeView.InsertItem(szUdpDstPort, hUdp);
			TreeNodeData *pTndUdpDstPort = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndUdpDstPort->dwStartPos = pTndUdpSrcPort->dwEndPos;
			pTndUdpDstPort->dwEndPos = pTndUdpDstPort->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hUdpDstPort, (DWORD_PTR)pTndUdpDstPort);

			// [�ڵ�] UDP ����
			static WCHAR szUdpLength[32];
			wsprintf(szUdpLength, _T("Length: %u"), ntohs(pUdpHdr->nLen));
			HTREEITEM hUdpLength = m_cTreeView.InsertItem(szUdpLength, hUdp);
			TreeNodeData *pTndUdpLength = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndUdpLength->dwStartPos = pTndUdpDstPort->dwEndPos;
			pTndUdpLength->dwEndPos = pTndUdpLength->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hUdpLength, (DWORD_PTR)pTndUdpLength);

			// [�ڵ�] UDP У���
			static WCHAR szUdpCrc[32];
			wsprintf(szUdpCrc, _T("Checksum: %u"), ntohs(pUdpHdr->nCrc));
			HTREEITEM hUdpCrc = m_cTreeView.InsertItem(szUdpCrc, hUdp);
			TreeNodeData *pTndUdpCrc = (TreeNodeData *)malloc(sizeof(TreeNodeData));
			pTndUdpCrc->dwStartPos = pTndUdpLength->dwEndPos;
			pTndUdpCrc->dwEndPos = pTndUdpCrc->dwStartPos + sizeof(unsigned short);
			m_cTreeView.SetItemData(hUdpCrc, (DWORD_PTR)pTndUdpCrc);

			// [�ڵ�] UDP ����
			if (pTndUdpCrc->dwEndPos < pNode->pHeader->caplen)
			{
				HTREEITEM hUdpData = m_cTreeView.InsertItem(_T("Data"), hUdp);
				TreeNodeData *pTndUdpData = (TreeNodeData *)malloc(sizeof(TreeNodeData));
				pTndUdpData->dwStartPos = pTndUdpCrc->dwEndPos;
				pTndUdpData->dwEndPos = pNode->pHeader->caplen;
				m_cTreeView.SetItemData(hUdpData, (DWORD_PTR)pTndUdpData);
			}
		}
	}
}

void CKSnifferDlg::OnTvnSelchangedTreePackets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HTREEITEM hCurNode = m_cTreeView.GetSelectedItem();
	if (hCurNode == NULL)
	{
		return ;
	}
	//AfxMessageBox(m_cTreeView.GetItemText(hCurNode));
	TreeNodeData *pTnd = (TreeNodeData *)(m_cTreeView.GetItemData(hCurNode));
	m_cHexView.SetSel(pTnd->dwStartPos, pTnd->dwEndPos);

	*pResult = 0;
}

// ����Ŀռ�������ɾ��
void CKSnifferDlg::OnTvnDeleteitemTreePackets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (pNMTreeView->itemOld.lParam != NULL)
	{
		free((TreeNodeData *)(pNMTreeView->itemOld.lParam));
	}

	*pResult = 0;
}

// �رճ���
void CKSnifferDlg::OnFileClose()
{
	// TODO: �ڴ���������������
	SendMessage(WM_CLOSE, 0, 0);
}

void CKSnifferDlg::OnHelpAbout()
{
	// TODO: �ڴ���������������
	MessageBox(
		_T("--------------------------------------------------\r\n")
		_T("���ߣ��������\r\n")
		_T("���ͣ�http://www.programlife.net/ \r\n")
		_T("--------------------------------------------------\r\n")
		_T("���ƣ�KSniffer(����WinPcap�ļ���̽��)\r\n")
		_T("�汾��V1.0\r\n")
		_T("--------------------------------------------------"),
		_T("About KSniffer V1.0"),
		MB_ICONINFORMATION);
}
