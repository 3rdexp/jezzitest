// PlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "VideoWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//皮肤文件中定义的按钮的ID
#define IDM_PLAY          5000
#define IDM_VOICE_UP      4000
#define IDM_VOICE_DOWN    4001
 

/////////////////////////////////////////////////////////////////////////////
// CPlayerDlg dialog

CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlayerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Create( CPlayerDlg::IDD, pParent );
}

void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayerDlg)
	DDX_Control(pDX, IDC_BMPSLIDER, m_bmpSlider);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CPlayerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND( IDM_PLAY , OnDemo)
	ON_COMMAND( IDM_VOICE_UP , OnVoiceUp)
	ON_COMMAND( IDM_VOICE_DOWN , OnVoiceDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerDlg message handlers

BOOL CPlayerDlg::OnInitDialog()
{
	BOOL bAutoPlace;
	CRect rcObj;

	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetWindowText("King Player");
	SetShapeWindowSkin(m_hWnd,"PlayerPanel" );

	SetCaptionButtonTooltip( m_hWnd , "Play" , _T("<table><tr><td valign=vcenter><icon idres=128 width=32 height=32></td><td><center><h2>Play</h2><br><hr color=blue></center><br>Click to play current file</td></tr></table>") );
	SetCaptionButtonTooltip( m_hWnd , "cb1" , _T("CaptionButton 1") );
	SetCaptionButtonTooltip( m_hWnd , "cb2" , _T("<table><tr><td valign=vcenter><icon idres=128 width=32 height=32></td><td><center><h2>CaptionButton 2</h2><br><hr color=blue></center><br>This is a caption button with tooltip</td></tr></table>") );
	SetCaptionButtonTooltip( m_hWnd , "Close" , _T("<table><tr><td valign=vcenter><icon idres=128 width=32 height=32></td><td><center><h2>Close</h2><br><hr color=blue></center><br>Click to close the window</td></tr></table>") );
	SetCaptionButtonTooltip( m_hWnd , "Min" , _T("<table><tr><td valign=vcenter><icon idres=128 width=32 height=32></td><td><center><h2>Minimize</h2><br><hr color=blue></center><br>Click to minimize the window</td></tr></table>") );
    
	m_pVideo = new CVideoWnd( NULL );

	//we use a CaptionStatic object as a place holder for voice slider control
	HSKIN hSkin = OpenSkinData( _T("CS2") );
	VERIFY( hSkin != NULL );

	int nRet = GetSkinControlRect( hSkin , rcObj , &bAutoPlace );
	VERIFY( nRet == 1 );

	CloseSkinData( hSkin );
	
	m_bmpSlider.MoveWindow( &rcObj , TRUE );
	m_bmpSlider.SetValue( 50 );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlayerDlg::OnPaint() 
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
HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPlayerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_pVideo->DestroyWindow();
	delete m_pVideo;
}

void CPlayerDlg::OnDemo() 
{
	SetSkinObjectText( m_hWnd , "CS1" , "Gone with wind..." );
	//m_pVideo->Create( CVideoWnd::IDD , this );
	m_pVideo->ShowWindow( SW_SHOW );
}

void CPlayerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	DestroyWindow();
	CDialog::OnClose();
}

void CPlayerDlg::OnVoiceUp()
{
	m_bmpSlider.SetValue( m_bmpSlider.GetValue() + 5 ); 
}

void CPlayerDlg::OnVoiceDown()
{
	m_bmpSlider.SetValue( m_bmpSlider.GetValue() - 5 );
}
