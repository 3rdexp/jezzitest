// MFCControlsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCControls.h"
#include "MFCControlsDlg.h"
#include "../About/AboutDlg.h"
#include "../TestDll/TestDll.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CMFCControlsDlg dialog

CMFCControlsDlg::CMFCControlsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMFCControlsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMFCControlsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCControlsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMFCControlsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
    DDX_Control(pDX, IDC_LIST1, m_List);
    DDX_Control(pDX, IDC_SCROLLBAR1, m_HorzScroll);
    DDX_Control(pDX, IDC_SCROLLBAR2, m_VertScroll);
    DDX_Control(pDX, IDC_COMBO1, m_Combo);
    DDX_Control(pDX, IDC_PAUSEPLAY, m_PausePlayButton);
    DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
    DDX_Control(pDX, IDC_TAB1, m_Tab);
    DDX_Text(pDX, IDC_EDIT1, m_Edit);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_SLIDER1, m_Slider1);
    DDX_Control(pDX, IDC_SLIDER2, m_Slider2);
    DDX_Control(pDX, IDC_SLIDER3, m_Slider3);
    DDX_Control(pDX, IDC_SLIDER4, m_Slider4);
    DDX_Control(pDX, IDC_SLIDER5, m_Slider5);
    DDX_Control(pDX, IDC_SLIDER6, m_Slider6);
}

BEGIN_MESSAGE_MAP(CMFCControlsDlg, CDialog)
	//{{AFX_MSG_MAP(CMFCControlsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PAUSEPLAY, OnPauseplay)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_BN_CLICKED(IDC_SKINMAGIC, OnSkinmagic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCControlsDlg message handlers

BOOL CMFCControlsDlg::OnInitDialog()
{
	CFont* pFont = GetFont();

	LOGFONT logfont;

	pFont->GetLogFont(&logfont);

	logfont.lfWeight = 700;

	CFont myFont;
	myFont.CreateFontIndirect(&logfont);

	SetFont(&myFont,TRUE);

	myFont.Detach();

	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	
	// Setup demo controls - no more skinning-specific code in this method
    for (int i = 0; i < 5; i ++)
    {
        TCHAR szTab[20] = {0};
        _stprintf(szTab, _T("Tab %d"), (i+1));
        m_Tab.InsertItem(i, szTab); 
    }
	
    for (int j = 0; j < 10; j++) {
        m_List.InsertString(j, _T("SkinMagic Toolkit!"));
    }
	
    m_bPlaying = TRUE;
    
    m_Progress.SetRange(0, 1000);
    m_Progress.SetPos(0);
    m_VertScroll.SetScrollRange(0, 1000, TRUE);
    m_HorzScroll.SetScrollRange(0, 1000, TRUE);
    SendDlgItemMessage(IDC_RADIO3, BM_SETCHECK, 1, 0);
    SendDlgItemMessage(IDC_CHECK3, BM_SETCHECK, 1, 0);
	
    m_Slider1.SetRange(0,100,0);
    m_Slider2.SetRange(0,100,0);
    m_Slider3.SetRange(0,100,0);
    m_Slider4.SetRange(0,100,0);
    m_Slider5.SetRange(0,100,0);
    m_Slider6.SetRange(0,100,0);
	
    m_Slider1.SetPos(0);
    m_Slider2.SetPos(50);
    m_Slider3.SetPos(100);
    m_Slider4.SetPos(0);
    m_Slider5.SetPos(50);
    m_Slider6.SetPos(100);
	
    // Set a timer to update
    //SetTimer(1000, 250, NULL);


	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCControlsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCControlsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCControlsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMFCControlsDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
	
	if(nIDEvent == 1000)
	{
		if(m_VertScroll.GetScrollPos() == 1000)
			m_VertScroll.SetScrollPos(0, TRUE);
		else
			m_VertScroll.SetScrollPos(m_VertScroll.GetScrollPos() + 50, TRUE);
		if(m_HorzScroll.GetScrollPos() == 1000)
			m_HorzScroll.SetScrollPos(0, TRUE);
		else
			m_HorzScroll.SetScrollPos(m_HorzScroll.GetScrollPos() + 50, TRUE);
		
		if(m_Tab.GetCurSel() == 4)
			m_Tab.SetCurSel(0);
		else
			m_Tab.SetCurSel(m_Tab.GetCurSel() + 1);

		  m_Tab.Invalidate();
	
		m_Slider1.SetPos(rand() % 101);
		m_Slider2.SetPos(rand() % 101);
		m_Slider3.SetPos(rand() % 101);
		m_Slider4.SetPos(rand() % 101);
		m_Slider5.SetPos(rand() % 101);
		m_Slider6.SetPos(rand() % 101);

	}
}

static DWORD WINAPI _ScoreThreadProc(LPVOID lpParameter)
{
	ASSERT(lpParameter);
	CMFCControlsDlg * pThis = (CMFCControlsDlg*)lpParameter;
	pThis->ScoreThreadProc();
	return 0;
}

void CMFCControlsDlg::ScoreThreadProc ()
{
	_hWait = CreateEvent(NULL, TRUE, FALSE, NULL);

	while(1)
	{
		
		int nPos = 0;
		int cur = m_Progress.GetPos();

		if(cur >= 1000)
			m_Progress.SetPos(0);
		else
			m_Progress.SetPos(cur + 200);
		// 1个小时报告一次

		//SendDlgItemMessage(IDC_PROGRESS1, WM_USER+2, nPos, 0);

		//SendDlgItemMessage( IDC_PROGRESS1, )
		DWORD dwWait = WaitForSingleObject(_hWait, 1000);
		if(dwWait == WAIT_OBJECT_0)
			break;
		
	}	
}
void CMFCControlsDlg::OnPauseplay() 
{
	if(m_bPlaying)
	{
		m_bPlaying = FALSE;
		KillTimer(1000);
		m_PausePlayButton.SetWindowText(_T("Play"));
	}
	else
	{
		m_bPlaying = TRUE;
		SetTimer(1000, 250, NULL);
		m_PausePlayButton.SetWindowText(_T("Pause"));
	}
}

int CMFCControlsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CMFCControlsDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}


void CMFCControlsDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
    m_Tab.Invalidate();

	*pResult = 0;
}

void CMFCControlsDlg::OnSkinmagic() 
{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();	
}
