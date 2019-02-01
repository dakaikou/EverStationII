// Dlg_YUVPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_YUVPreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVPreview dialog
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_Video.h"

#include "..\MainFrm.h"
#include "..\Common\Dlg_ShowVideo.h"
#include "..\Magic_YUV\GuiApi_YUV.h"

CDlg_YUVPreview::CDlg_YUVPreview(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_YUVPreview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_YUVPreview)
	//}}AFX_DATA_INIT
}


void CDlg_YUVPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_YUVPreview)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_YUVPreview, CDialog)
	//{{AFX_MSG_MAP(CDlg_YUVPreview)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, OnBtnPreview)
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpen)
	ON_CBN_SELCHANGE(IDC_CMB_FOURCC, OnSelchangeCmbFourcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_YUVPreview message handlers

void CDlg_YUVPreview::Reset(void)
{
	CButton* pBtn = NULL;
	CWnd* pWnd = NULL;

	//源文件设置
	pWnd = GetDlgItem(IDC_EDIT_YUV);
	pWnd->SetWindowText("");

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	pWnd->EnableWindow(TRUE);
	pWnd->SetWindowText("打开");

	pWnd = GetDlgItem(IDC_BTN_PREVIEW);
	pWnd->EnableWindow(FALSE);
//	pWnd->SetWindowText("预览");

	UpdateData(FALSE);
}

BOOL CDlg_YUVPreview::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int			i;
	int			nSel;
	char		pszItem[128];
	char		pszFourCC[5];
	CComboBox*  pCmbBox;
	CWnd*	    pWnd;
	CRect		rect(0, 0, 720, 576);

	m_dlgVideo.Create(IDD_SHOW_VIDEO, this);
	m_dlgVideo.ShowWindow(SW_HIDE);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_WH);
	pCmbBox->ResetContent();
	
	for (i = 0; i < VIDEO_FORMAT_MAX; i++)
	{
		VIDEO_get_width_and_height_info(i, NULL, pszItem, sizeof(pszItem));
		pCmbBox->AddString(pszItem);
	}
	pCmbBox->SetCurSel(VIDEO_4CIF);

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
	pCmbBox->ResetContent();
//	pCmbBox->AddString("IYUV");
//	pCmbBox->AddString("YV16");
	pCmbBox->AddString("YV12");
	pCmbBox->AddString("YUY2");
	pCmbBox->SetCurSel(0);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		pCmbBox->GetLBText(nSel, pszFourCC);
		DecodeFourCC2Text(pszFourCC, pszItem, sizeof(pszItem));
	}

	//设置播放帧率
	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FRAMERATE);
	pCmbBox->ResetContent();

	pCmbBox->AddString("7.5P");
	pCmbBox->AddString("15P");
	pCmbBox->AddString("25P");
	pCmbBox->AddString("30P");
	pCmbBox->AddString("60P");
	pCmbBox->SetCurSel(3);

	pWnd = GetDlgItem(IDC_STATIC_FORMAT);
	pWnd->SetWindowText(pszItem);

	m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
	m_dlgProgress.ShowWindow(SW_HIDE);

	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_YUVPreview::OnBtnPreview() 
{
	// TODO: Add your control notification handler code here
	int			nSel;
	CString		strPath;
	CString		strBtn;
	CWnd*		pWnd = NULL;

	WIDTH_HEIGHT_t			wh;
	Video_decode_info_t		decode_info;
	CComboBox*				pCmbBox = NULL;

	if (!m_dlgVideo.IsWindowVisible())
	{
		pWnd = GetDlgItem(IDC_EDIT_YUV);
		pWnd->GetWindowText(strPath);

		pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_WH);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			VIDEO_get_width_and_height_info(nSel, &wh, NULL, 0);
		}
		else
		{
			wh.width = 352;
			wh.height = 288;
		}

		//获得参数
		memset(&decode_info, 0x00, sizeof(Video_decode_info_t));

		pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			pCmbBox->GetLBText(nSel, decode_info.pszFourCC);
		}
		else
		{
			strcpy_s(decode_info.pszFourCC, sizeof(decode_info.pszFourCC), "YV12");
		}

		char strFrameRate[16];
		pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FRAMERATE);
		nSel = pCmbBox->GetCurSel();
		if (nSel != CB_ERR)
		{
			pCmbBox->GetLBText(nSel, strFrameRate);
		}

		decode_info.luma_width = wh.width;
		decode_info.luma_height = wh.height;
		decode_info.luma_pix_count = decode_info.luma_width * decode_info.luma_height; 
		decode_info.luma_buf_size = decode_info.luma_pix_count;

		decode_info.display_width = decode_info.luma_width;
		decode_info.display_height = decode_info.luma_height;

		if (strcmp(strFrameRate, "7.5P") == 0)
		{
			decode_info.framerate = 7.0;
		}
		else if (strcmp(strFrameRate, "15P") == 0)
		{
			decode_info.framerate = 15.0;
		}
		else if (strcmp(strFrameRate, "25P") == 0)
		{
			decode_info.framerate = 25.0;
		}
		else if (strcmp(strFrameRate, "30P") == 0)
		{
			decode_info.framerate = 30.0;
		}
		else if (strcmp(strFrameRate, "60P") == 0)
		{
			decode_info.framerate = 60.0;
		}
		else
		{
			decode_info.framerate = 60.0;
		}

//		if (strcmp(decode_info.pszFourCC, "IYUV") == 0)
//		{
//			decode_info.chroma_width = (wh.width >> 1);
//			decode_info.chroma_height = (wh.height >> 1);
//			decode_info.chroma_pix_count = decode_info.chroma_width * decode_info.chroma_height;
//			decode_info.chroma_buf_size = decode_info.chroma_pix_count;
//			decode_info.chroma_format = CHROMA_FORMAT_4_2_0;
//		}
//		else if (strcmp(decode_info.pszFourCC, "YV16") == 0)
//		{
//			decode_info.chroma_width = (wh.width >> 1);
//			decode_info.chroma_height = wh.height;
//			decode_info.chroma_pix_count = decode_info.chroma_width * decode_info.chroma_height;
//			decode_info.chroma_buf_size = decode_info.chroma_pix_count;
//			decode_info.chroma_format = CHROMA_FORMAT_4_2_2;
//		}

		if (strcmp(decode_info.pszFourCC, "YUY2") == 0)
		{
			decode_info.chroma_width = (wh.width >> 1);
			decode_info.chroma_height = wh.height;
			decode_info.chroma_pix_count = decode_info.chroma_width * decode_info.chroma_height;
			decode_info.chroma_buf_size = decode_info.chroma_pix_count;
			decode_info.chroma_format = CHROMA_FORMAT_4_2_2;
		}
		else
		{
			decode_info.chroma_width = (wh.width >> 1);
			decode_info.chroma_height = (wh.height >> 1);
			decode_info.chroma_pix_count = decode_info.chroma_width * decode_info.chroma_height;
			decode_info.chroma_buf_size = decode_info.chroma_pix_count;
			decode_info.chroma_format = CHROMA_FORMAT_4_2_0;
		}

		decode_info.frame_buf_size = decode_info.luma_buf_size + decode_info.chroma_buf_size + decode_info.chroma_buf_size;

		//if (m_YUVDecoder.IsOpened())
		//{
		//	m_YUVDecoder.Close();
		//}

		m_YUVDecoder.Open((STREAM_FILE | YUV_FILE_YUV), strPath.GetBuffer(128), &decode_info);

		m_dlgVideo.AttachVideoDecoder(&m_YUVDecoder);
		m_dlgVideo.ShowWindow(SW_SHOW);

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame->m_rectSecondDesktop.right > pMainFrame->m_rectSecondDesktop.left)
		{
			m_dlgVideo.MoveWindow(&(pMainFrame->m_rectSecondDesktop));
		}
		else
		{
			m_dlgVideo.MoveWindow(&(pMainFrame->m_rectPrimaryDesktop));
		}

		m_dlgVideo.EnlargeClientAreaToFullScreen();
	}
}

void CDlg_YUVPreview::OnBtnOpen() 
{
	// TODO: Add your control notification handler code here
	char szFilter[128];
	CWnd*	pWnd = NULL;
	CString strBtn;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_BTN_OPEN);
	pWnd->GetWindowText(strBtn);

	if (strBtn == "打开")
	{
//		switch (m_nFileFormat)
//		{
//		case YUV_FILE_DY_DU_DV:
//			strcpy(szFilter, "Y Files (*.y)|*.y|U Files (*.u)|*.u|V Files (*.v)|*.v||");
//			break;
//		case YUV_FILE_DYUV:
//		case YUV_FILE_YUV:
		strcpy_s(szFilter, sizeof(szFilter), "All Files (*.yuv)|*.yuv||");
//			break;
//		}

		CFileDialog dlg(TRUE, "yuv", NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);

		if (dlg.DoModal() == IDOK)
		{
			CString strPath = dlg.GetPathName();

			pWnd = GetDlgItem(IDC_EDIT_YUV);
			pWnd->SetWindowText(strPath);

			pWnd = GetDlgItem(IDC_BTN_OPEN);
			pWnd->SetWindowText("关闭");

			pWnd = GetDlgItem(IDC_BTN_PREVIEW);
			pWnd->EnableWindow(TRUE);
		}
	}
	else
	{
		pWnd->SetWindowText("打开");

		pWnd = GetDlgItem(IDC_BTN_PREVIEW);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_YUV);
		pWnd->SetWindowText("");

		m_YUVDecoder.Close();
	}
}

/*
void CDlg_YUVPreview::OnBtnExchange() 
{
	// TODO: Add your control notification handler code here
	FILE* fp_src;
	FILE* fp_dst;
	WIDTH_HEIGHT_t	wh;
	int file_size;
	int rdcount;
//	int src_chroma_size;
	int src_chroma_width;
	int src_chroma_height;
//	int dst_chroma_size;
	int dst_chroma_width;
	int dst_chroma_height;
	int nSel;
	int nSrcChroma;
	int nDstChroma;
	int w, h;
//	int rdsize;
	unsigned char*	srcbuf;
	unsigned char*  dstbuf;
	CString			strSrcFile;
	CString			strDstFile;

	CWnd*		pWnd;
	CComboBox*  pCmbBox;

	UpdateData(TRUE);

	pWnd = GetDlgItem(IDC_BTN_EXCHANGE);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_RESET);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_EDIT_SRC_YUV);
	pWnd->GetWindowText(strSrcFile);

	if (strSrcFile != "")
	{
		fp_src = fopen(strSrcFile.GetBuffer(128), "rb");
		if (fp_src)
		{
			fseek(fp_src, 0, SEEK_END);
			file_size = ftell(fp_src);
			fseek(fp_src, 0, SEEK_SET);

			pWnd = GetDlgItem(IDC_EDIT_DST_YUV);
			pWnd->GetWindowText(strDstFile);

			if (strDstFile != "")
			{
				fp_dst = fopen(strDstFile.GetBuffer(128), "wb");
				if (fp_dst)
				{
					pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_SRC_WH);
					nSel = pCmbBox->GetCurSel();
					VIDEO_get_width_and_height_info(nSel, &wh, NULL);

					pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_SRC_CHROMA);
					nSrcChroma = pCmbBox->GetCurSel();
					switch (nSrcChroma)
					{
					case CHROMA_FORMAT_4_2_0:		//4:2:0
						src_chroma_width = wh.width / 2;
						src_chroma_height = wh.height / 2;
						break;
					case CHROMA_FORMAT_4_2_2:		//4:2:2
						src_chroma_width = wh.width / 2;
						src_chroma_height = wh.height;
						break;
					case CHROMA_FORMAT_4_4_4:		//4:4:4
						src_chroma_width = wh.width;
						src_chroma_height = wh.height;
						break;
					}
								
					pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_DST_CHROMA);
					nDstChroma = pCmbBox->GetCurSel();

					switch (nDstChroma)
					{
					case CHROMA_FORMAT_4_2_0:		//4:2:0
						dst_chroma_width = wh.width / 2;
						dst_chroma_height = wh.height / 2;
						break;
					case CHROMA_FORMAT_4_2_2:		//4:2:2
						dst_chroma_width = wh.width / 2;
						dst_chroma_height = wh.height;
						break;
					case CHROMA_FORMAT_4_4_4:		//4:4:4
						dst_chroma_width = wh.width;
						dst_chroma_height = wh.height;
						break;
					}

					srcbuf = (unsigned char*)malloc(wh.width);
					dstbuf = (unsigned char*)malloc(wh.width);

					m_dlgProgress.SetWindowText("视频序列色度格式转换");
					m_dlgProgress.ShowWindow(SW_SHOW);

					rdcount = 0;
					do
					{
						//Y
						for (h = 0; h < wh.height; h++)
						{
							rdcount += fread(srcbuf, wh.width, sizeof(char), fp_src);
							file_size -= wh.width;

							fwrite(srcbuf, wh.width, sizeof(char), fp_dst);
						}

						//U & V
						for (h = 0; h < (src_chroma_height * 2); h++)
						{
							rdcount += fread(srcbuf, src_chroma_width, sizeof(char), fp_src);
							file_size -= src_chroma_width;

							if ((nSrcChroma == CHROMA_FORMAT_4_2_0) && (nDstChroma == CHROMA_FORMAT_4_2_2))				//4:2:0 -> 4:2:2
							{
								fwrite(srcbuf, src_chroma_width, sizeof(char), fp_dst);
								fwrite(srcbuf, src_chroma_width, sizeof(char), fp_dst);
							}
							else if ((nSrcChroma == CHROMA_FORMAT_4_2_0) && (nDstChroma == CHROMA_FORMAT_4_4_4))				//4:2:0 -> 4:4:4
							{
								for (w = 0; w < src_chroma_width; w++)
								{
									dstbuf[w*2 + 0] = srcbuf[w];
									dstbuf[w*2 + 1] = srcbuf[w];
								}

								fwrite(dstbuf, dst_chroma_width, sizeof(char), fp_dst);
								fwrite(dstbuf, dst_chroma_width, sizeof(char), fp_dst);
							}
							else if ((nSrcChroma == CHROMA_FORMAT_4_2_2) && (nDstChroma == CHROMA_FORMAT_4_4_4))				//4:2:2 -> 4:4:4
							{
								for (w = 0; w < src_chroma_width; w++)
								{
									dstbuf[w*2 + 0] = srcbuf[w];
									dstbuf[w*2 + 1] = srcbuf[w];
								}

								fwrite(dstbuf, dst_chroma_width, sizeof(char), fp_dst);
							}
							else if ((nSrcChroma == CHROMA_FORMAT_4_2_2) && (nDstChroma == CHROMA_FORMAT_4_2_0))				//4:2:2 -> 4:2:0
							{
								if ((h % 2) == 0)
								{
									fwrite(srcbuf, dst_chroma_width, sizeof(char), fp_dst);
								}
							}
							else if ((nSrcChroma == CHROMA_FORMAT_4_4_4) && (nDstChroma == CHROMA_FORMAT_4_2_0))				//4:4:4 -> 4:2:0
							{
								if ((h % 2) == 0)
								{
									for (w = 0; w < src_chroma_width; w++)
									{
										if ((w % 2) == 0)
										{
											dstbuf[w/2] = srcbuf[w];
										}
									}
									fwrite(dstbuf, dst_chroma_width, sizeof(char), fp_dst);
								}
							}
							else if ((nSrcChroma == CHROMA_FORMAT_4_4_4) && (nDstChroma == CHROMA_FORMAT_4_2_2))				//4:4:4 -> 4:2:2
							{
								for (w = 0; w < src_chroma_width; w++)
								{
									if ((w % 2) == 0)
									{
										dstbuf[w/2] = srcbuf[w];
									}
								}
								fwrite(dstbuf, dst_chroma_width, sizeof(char), fp_dst);
							}
							else
							{
								fwrite(srcbuf, src_chroma_width, sizeof(char), fp_dst);
							}
						}

					} while (file_size > 0);

					m_dlgProgress.ShowWindow(SW_HIDE);

					fclose(fp_dst);
				}
			}
			else
			{
				MessageBox("请选择一个目标文件!", "视频序列色度格式转换", MB_OK | MB_ICONEXCLAMATION);
			}

			fclose(fp_src);
		}
	}
	else
	{
		MessageBox("请选择一个源文件!", "视频序列色度格式转换", MB_OK | MB_ICONEXCLAMATION);
	}

	pWnd = GetDlgItem(IDC_BTN_EXCHANGE);
	pWnd->EnableWindow(TRUE);

	pWnd = GetDlgItem(IDC_BTN_RESET);
	pWnd->EnableWindow(TRUE);
}
*/


void CDlg_YUVPreview::OnSelchangeCmbFourcc() 
{
	// TODO: Add your control notification handler code here
	int			nSel;
	CComboBox*	pCmbBox;
	CWnd*		pWnd;
	char		pszFourCC[5];
	char		pszItem[128];

	pCmbBox = (CComboBox*)GetDlgItem(IDC_CMB_FOURCC);
	nSel = pCmbBox->GetCurSel();
	if (nSel != CB_ERR)
	{
		pCmbBox->GetLBText(nSel, pszFourCC);
		DecodeFourCC2Text(pszFourCC, pszItem, sizeof(pszItem));
	}

	pWnd = GetDlgItem(IDC_STATIC_FORMAT);
	pWnd->SetWindowText(pszItem);
	
}
