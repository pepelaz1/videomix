
// sampledlg.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "sampledlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 #pragma warning( disable : 4995)

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSampleDlg dialog

CSampleDlg::CSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSampleDlg::IDD, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAY_BTN, m_btnPlay);
	DDX_Control(pDX, IDC_PAUSE_BTN, m_btnPause);
	DDX_Control(pDX, IDC_STOP_BTN, m_btnStop);
	DDX_Control(pDX, IDC_FILE_STATIC, m_stcFile1);
	DDX_Control(pDX, IDC_FILE_EDIT, m_edtFile1);
	DDX_Control(pDX, IDC_BROWSE_BTN, m_btnBrowse1);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	DDX_Control(pDX, IDC_STEP_BTN, m_btnStep);
	DDX_Control(pDX, IDC_FILE_STATIC2, m_stcFile2);
	DDX_Control(pDX, IDC_FILE_EDIT2, m_edtFile2);
	DDX_Control(pDX, IDC_BROWSE_BTN2, m_btnBrowse2);
	DDX_Control(pDX, IDC_FILE_STATIC3, m_stcBBox);
	DDX_Control(pDX, IDC_FILE_STATIC4, m_stcBBoxX);
	DDX_Control(pDX, IDC_BBOX_X_EDIT, m_edtBBoxX);
	DDX_Control(pDX, IDC_FILE_STATIC5, m_stcBBoxY);
	DDX_Control(pDX, IDC_BBOX_Y_EDIT, m_edtBBoxY);
	DDX_Control(pDX, IDC_FILE_STATIC6, m_stcBBoxWidth);
	DDX_Control(pDX, IDC_BBOX_WIDTH_EDIT, m_edtBBoxWidth);
	DDX_Control(pDX, IDC_FILE_STATIC7, m_stcBBoxHeight);
	DDX_Control(pDX, IDC_BBOX_HEIGHT_EDIT, m_edtBBoxHeight);
	DDX_Control(pDX, IDC_STEP_BTN2, m_btnBBoxSet);
}

BEGIN_MESSAGE_MAP(CSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY_BTN, &CSampleDlg::OnBnClickedPlayBtn)
	ON_BN_CLICKED(IDC_PAUSE_BTN, &CSampleDlg::OnBnClickedPauseBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CSampleDlg::OnBnClickedStopBtn)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BROWSE_BTN, &CSampleDlg::OnBnClickedBrowseBtn)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CSampleDlg::OnNMReleasedcaptureSlider1)
ON_BN_CLICKED(IDC_BROWSE_BTN2, &CSampleDlg::OnBnClickedBrowseBtn2)
ON_BN_CLICKED(IDC_STEP_BTN, &CSampleDlg::OnBnClickedStepBtn)
ON_BN_CLICKED(IDC_STEP_BTN2, &CSampleDlg::OnBnClickedStepBtn2)
END_MESSAGE_MAP()


// CSampleDlg message handlers

BOOL CSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CoInitialize(NULL);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect clrect;
	GetClientRect(&clrect);
	m_playerWnd.Create(_T("STATIC"), L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY,
		CRect(clrect.left+10, clrect.top+10, clrect.right-10, clrect.bottom-125), this, 1234);

	m_playerWnd.SetPlayer(&m_player);

	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);
	m_btnStep.EnableWindow(FALSE);
	m_edtFile1.EnableWindow(TRUE);
	m_edtFile2.EnableWindow(TRUE);
	m_btnBrowse1.EnableWindow(TRUE);

	m_edtBBoxX.SetWindowText(L"10");
	m_edtBBoxY.SetWindowText(L"10");
	m_edtBBoxWidth.SetWindowText(L"100");
	m_edtBBoxHeight.SetWindowText(L"100");


	//InitPlayer();

	m_slider.SetRange(0,100);
	SetTimer(1,1000, NULL);
	SetTimer(2,500, NULL);
	SetTimer(3,5000, NULL);

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleDlg::InitPlayer()
{
	CString filename1;
	GetDlgItemText(IDC_FILE_EDIT, filename1);
	
	CString filename2;
	GetDlgItemText(IDC_FILE_EDIT2, filename2);

//	filename1 = "D:\\torrents\\video\\Kiteshop_9Mbps.mp4";
//	filename2 = "D:\\torrents\\video\\Kiteshop_1Mbps.mp4";
//	filename1 = "D:\\video\\Kiteshop_9Mbps.mp4";
//	filename2 = "D:\\video\\Kiteshop_1Mbps.mp4";

	m_player.SetFile(filename1.GetBuffer(), filename2.GetBuffer());
	HRESULT hr = m_player.Init(m_playerWnd.m_hWnd);
	if (FAILED(hr))
	{
		CString errmsg = L"Failed build graph ";
		MessageBox(errmsg.GetBuffer(),L"Error", MB_ICONERROR);
	}

	m_btnBBoxSet.EnableWindow(TRUE);
}


void CSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		m_player.RepaintVW();
	}
}


HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CSampleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CSampleDlg::OnBnClickedPlayBtn()
{
	m_player.Play();
	m_btnPlay.EnableWindow(FALSE);
	m_btnPause.EnableWindow(TRUE);
	m_btnStop.EnableWindow(TRUE);
	m_btnStep.EnableWindow(TRUE);
	m_edtFile1.EnableWindow(FALSE);
	m_edtFile2.EnableWindow(FALSE);
	m_btnBrowse1.EnableWindow(FALSE);
	m_btnBrowse2.EnableWindow(FALSE);
}


void CSampleDlg::OnBnClickedPauseBtn()
{
	m_player.Pause();
	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStep.EnableWindow(TRUE);
	m_btnStop.EnableWindow(TRUE);
	m_edtFile1.EnableWindow(FALSE);
	m_edtFile2.EnableWindow(FALSE);
	m_btnBrowse1.EnableWindow(FALSE);
	m_btnBrowse2.EnableWindow(FALSE);
}



void CSampleDlg::OnBnClickedStopBtn()
{
	m_player.Stop();
	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);
	m_btnStep.EnableWindow(FALSE);
	m_edtFile1.EnableWindow(TRUE);
	m_edtFile2.EnableWindow(TRUE);
	m_btnBrowse1.EnableWindow(TRUE);
	m_btnBrowse2.EnableWindow(TRUE);
}


void CSampleDlg::OnBnClickedStepBtn()
{
	m_player.StepForward();
	m_btnPlay.EnableWindow(TRUE);
}


void CSampleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	MoveControls();	
}


void CSampleDlg::MoveControls()
{
	CRect r;
	GetClientRect(&r);

	if (m_playerWnd.m_hWnd)
	{
		m_playerWnd.MoveWindow(r.left+10, r.top+10, r.right-20, r.bottom-135);
	//	m_playerWndveWindow(r.left+10, r.top+10, r.right-20, r.bottom-135);

	}
	m_player.UpdateVWPos();
	
	CRect r1;
	if (m_btnPlay.m_hWnd != NULL)
	{
		m_btnPlay.GetClientRect(&r1);
		r1.MoveToY(r.bottom-117);
		r1.MoveToX(9);
		m_btnPlay.MoveWindow(&r1);
	}

	if (m_btnPause.m_hWnd != NULL)
	{
		m_btnPause.GetClientRect(&r1);
		r1.MoveToY(r.bottom-117);
		r1.MoveToX(75);
		m_btnPause.MoveWindow(&r1);
	}

	if (m_btnStop.m_hWnd != NULL)
	{
		m_btnStop.GetClientRect(&r1);
		r1.MoveToY(r.bottom-117);
		r1.MoveToX(141);
		m_btnStop.MoveWindow(&r1);
	}

	if (m_btnStep.m_hWnd != NULL)
	{
		m_btnStep.GetClientRect(&r1);
		r1.MoveToY(r.bottom-117);
		r1.MoveToX(201);
		m_btnStep.MoveWindow(&r1);
	}

	if (m_stcFile1.m_hWnd)
	{
		m_stcFile1.GetClientRect(&r1);
		r1.SetRect(12,r.bottom - 83, 45, r.bottom - 67);
		m_stcFile1.MoveWindow(&r1);
	}

	if (m_stcFile2.m_hWnd)
	{
		m_stcFile2.GetClientRect(&r1);
		r1.SetRect(12,r.bottom - 55, 45, r.bottom - 39);
		m_stcFile2.MoveWindow(&r1);
	}	

	int nn = 35;
	if (m_edtFile1.m_hWnd)
	{
		m_edtFile1.GetClientRect(&r1);
		r1.SetRect(45,r.bottom - 86, r.right-nn, r.bottom - 66);
		m_edtFile1.MoveWindow(&r1);
	}		

	if (m_edtFile2.m_hWnd)
	{
		m_edtFile2.GetClientRect(&r1);
		r1.SetRect(45,r.bottom - 58, r.right-nn, r.bottom - 38);
		m_edtFile2.MoveWindow(&r1);
	}		

	if (m_btnBrowse1.m_hWnd)
	{
		m_btnBrowse1.GetClientRect(&r1);
		r1.SetRect(r.right-nn-1,r.bottom - 87, r.right-nn-1+26, r.bottom-67);
		m_btnBrowse1.MoveWindow(&r1);
	}

	if (m_btnBrowse2.m_hWnd)
	{
		m_btnBrowse2.GetClientRect(&r1);
		r1.SetRect(r.right-nn-1,r.bottom - 59, r.right-nn-1+26, r.bottom-39);
		m_btnBrowse2.MoveWindow(&r1);
	}

	nn = 2;
	if (m_slider.m_hWnd)
	{
		m_slider.GetClientRect(&r1);
		r1.SetRect(260,r.bottom - 118, r.right-nn, r.bottom - 93);
		m_slider.MoveWindow(&r1);
	}
	
	if (m_stcBBox.m_hWnd)
	{
		m_stcBBox.GetClientRect(&r1);
		r1.SetRect(12,r.bottom - 26, 100, r.bottom - 9);
		m_stcBBox.MoveWindow(&r1);
	}
	//-------------------------
	if (m_stcBBoxX.m_hWnd)
	{
		m_stcBBoxX.GetClientRect(&r1);
		r1.SetRect(95,r.bottom - 26, 100, r.bottom - 9);
		m_stcBBoxX.MoveWindow(&r1);
	}

	if (m_edtBBoxX.m_hWnd)
	{
		m_stcBBoxX.GetClientRect(&r1);
		r1.SetRect(110,r.bottom - 29, 154, r.bottom - 9);
		m_edtBBoxX.MoveWindow(&r1);
	}	
	//-------------------------
	if (m_stcBBoxY.m_hWnd)
	{
		m_stcBBoxY.GetClientRect(&r1);
		r1.SetRect(168,r.bottom - 26, 175, r.bottom - 9);
		m_stcBBoxY.MoveWindow(&r1);
	}

	if (m_edtBBoxY.m_hWnd)
	{
		m_stcBBoxY.GetClientRect(&r1);
		r1.SetRect(183,r.bottom - 29, 227, r.bottom - 9);
		m_edtBBoxY.MoveWindow(&r1);
	}	
	//-------------------------
	if (m_stcBBoxWidth.m_hWnd)
	{
		m_stcBBoxWidth.GetClientRect(&r1);
		r1.SetRect(243,r.bottom - 26, 300, r.bottom - 9);
		m_stcBBoxWidth.MoveWindow(&r1);
	}

	if (m_edtBBoxWidth.m_hWnd)
	{
		m_edtBBoxWidth.GetClientRect(&r1);
		r1.SetRect(276,r.bottom - 29, 320, r.bottom - 9);
		m_edtBBoxWidth.MoveWindow(&r1);
	}	
	//-------------------------
	if (m_stcBBoxHeight.m_hWnd)
	{
		m_stcBBoxHeight.GetClientRect(&r1);
		r1.SetRect(330,r.bottom - 26, 370, r.bottom - 9);
		m_stcBBoxHeight.MoveWindow(&r1);
	}

	if (m_edtBBoxHeight.m_hWnd)
	{
		m_edtBBoxHeight.GetClientRect(&r1);
		r1.SetRect(368,r.bottom - 29, 415, r.bottom - 9);
		m_edtBBoxHeight.MoveWindow(&r1);
	}	
	//-------------------------
	if (m_btnBBoxSet.m_hWnd)
	{
		m_btnBBoxSet.GetClientRect(&r1);
		r1.SetRect(425,r.bottom - 31, 478, r.bottom-8);
		m_btnBBoxSet.MoveWindow(&r1);
	}
	RedrawWindow();
}

void CSampleDlg::Finalize()
{
	m_player.Reset();
}

void CSampleDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	m_player.UpdateVWPos();
}

void CSampleDlg::OnBnClickedBrowseBtn()
{
	WCHAR filter[] = { L"All Files (*.*)|*.*||" };

	CFileDialog dlg(TRUE, L"", NULL, 0, filter);

	if( dlg.DoModal() == IDOK )
	{
		m_edtFile1.SetWindowText(dlg.GetFolderPath() + "\\"+ dlg.GetFileName());
		
		CString secondFile;
		m_edtFile2.GetWindowText(secondFile);
		if (secondFile != L"")
			InitPlayer();		
	}
}

void CSampleDlg::OnBnClickedBrowseBtn2()
{
WCHAR filter[] = { L"All Files (*.*)|*.*||" };

	CFileDialog dlg(TRUE, L"", NULL, 0, filter);

	if( dlg.DoModal() == IDOK )
	{
		m_edtFile2.SetWindowText(dlg.GetFolderPath() + "\\"+ dlg.GetFileName());
		
		CString firstFile;
		m_edtFile1.GetWindowText(firstFile);
		if (firstFile != L"")
			InitPlayer();	
	}
}


void CSampleDlg::OnClose()
{
	m_player.Reset();
	CDialogEx::OnClose();
}


void CSampleDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CSampleDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDialogEx::OnRButtonUp(nFlags, point);
}

void CSampleDlg::OnTimer(UINT_PTR nTimerID)
{
	LONGLONG duration = m_player.GetDuration();
	if (duration > 0 && !GetAsyncKeyState(VK_LBUTTON) )
	{		
		if ( nTimerID == 1 )
		{
			/*int acnt = m_player.GetAudioTracksCount();
			if (acnt > 0 && m_cmbAudio.GetCount() == 0)
			{
				PopulateComboboxes();
				UpdateComboboxes();
			}*/

			LONGLONG position = m_player.GetPosition();
			m_slider.SetPos( (int)(position * 100.0f / duration) );

			char str[MAX_PATH];
			sprintf_s(str,MAX_PATH, "CSampleDlg::OnTimer position = %I64d, duration = %I64d lbutton state=%d\n", position , duration);
			//OutputDebugStringA(str);
		}

	/*	if ( nTimerID == 2 )
		{
			LONGLONG newpos = rand() * duration / RAND_MAX;
			m_player.SetPosition(newpos);
		}

		if ( nTimerID == 3 )
		{
			m_player.Stop();
			m_player.Play();
		}*/
	}
}

void CSampleDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LONGLONG duration = m_player.GetDuration();
	if (duration > 0 )
	{
		int pos = m_slider.GetPos();
		m_player.SetPosition((LONGLONG)(pos *  duration / 100.0f));
	}
	*pResult = 0;
}



void CSampleDlg::OnBnClickedStepBtn2()
{
	m_player.SetBoundingBox(GetDlgItemInt(IDC_BBOX_X_EDIT), GetDlgItemInt(IDC_BBOX_Y_EDIT),
		GetDlgItemInt(IDC_BBOX_WIDTH_EDIT), GetDlgItemInt(IDC_BBOX_HEIGHT_EDIT));
}
