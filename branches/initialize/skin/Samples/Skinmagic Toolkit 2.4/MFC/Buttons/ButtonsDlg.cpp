// ButtonsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Buttons.h"
#include "ButtonsDlg.h"
#include <winuser.h>
#include "skinmsg.h"
//#define COMPILE_MULTIMON_STUBS
//#include "multimon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_EASYSIZE_MAP(CButtonsDlg)
	EASYSIZE(IDC_BUTTON1,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP
/////////////////////////////////////////////////////////////////////////////
// CButtonsDlg dialog

CButtonsDlg::CButtonsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CButtonsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CButtonsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CButtonsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonsDlg)
	DDX_Control(pDX, IDC_BUTTON2, m_Btn2);
	DDX_Control(pDX, IDC_BUTTON1, m_Btn1);
	DDX_Control(pDX, IDC_BUTTON3, m_Btn3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CButtonsDlg, CDialog)
	//{{AFX_MSG_MAP(CButtonsDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	//}}AFX_MSG_MAP
	ON_COMMAND(5001 , OnButton1 )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsDlg message handlers

BOOL CButtonsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_Btn3.SetIcon( m_hIcon );

	
	bmp.LoadBitmap( IDB_BITMAP1 );
	m_Btn2.SetBitmap( bmp );
	//m_Btn2.SetIcon( m_hIcon );

	// TODO: Add extra initialization here
		LOGFONT Font;
		Font.lfHeight = 18; 
		Font.lfWidth = 0; 
		Font.lfEscapement = 0; 
		Font.lfOrientation = 0; 
		Font.lfWeight = 400; 
		Font.lfItalic = 0; 
		Font.lfUnderline = 0; 
		Font.lfStrikeOut = 0; 
		Font.lfCharSet = 0; 
		Font.lfOutPrecision = 3; 
		Font.lfClipPrecision = 2; 
		Font.lfQuality = 1; 
		Font.lfPitchAndFamily = 0; 
		strcpy( Font.lfFaceName , "New Times" ); 

	
	if( font.CreateFontIndirect( &Font ) )
	{
		m_Btn1.SetFont( &font );
	}

	INIT_EASYSIZE;

	CString strTB1;
	strTB1.LoadString(IDS_BUTTON1TOOLTIP);

	strTB1 = _T("<font face=""Times New Roman"" color=""green"" size=""24"" style=""bi"" >Hello, Tooltip</font>");
	//SetControlTooltip( m_Btn1.m_hWnd , strTB1 );

	strTB1.LoadString(IDS_BUTTON2);
	//SetControlTooltip( m_Btn2.m_hWnd , strTB1 );

	strTB1.LoadString(IDS_BUTTON3 );
	//SetControlTooltip( m_Btn3.m_hWnd , strTB1 );

	//CBitmap bmp;
	//bmp.LoadBitmap( IDB_BITMAP1 );
	//BITMAP bm;
	//bmp.GetBitmap( &bm );


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CButtonsDlg::OnPaint() 
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
HCURSOR CButtonsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CButtonsDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("Long string demo! Long string demo! Long string demo! Long string demo! Long string demo! Long string demo! Long string demo! Long string demo!Long string demo! Long string demo! Long string demo! Long string demo! Long string demo! " );
}

void CButtonsDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	m_Btn3.EnableWindow(FALSE);
	m_Btn2.EnableWindow(TRUE);
	MessageBox("You click button 3");
}

void CButtonsDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	m_Btn2.EnableWindow(FALSE);
	m_Btn3.EnableWindow(TRUE);
	MessageBox("You click button 2");
}

void CButtonsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    UPDATE_EASYSIZE;
	RedrawWindow( NULL,NULL ,RDW_UPDATENOW|RDW_ERASENOW|RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN );
}

void CButtonsDlg::OnButton4() 
{
    //MainWnd is the owner of Button1 in skin file
	static bFlag = TRUE;
	if( bFlag )
	{
		::SendMessage( GetDlgItem(	IDC_BUTTON4 )->m_hWnd, WMS_SETSTYLE, 51, 0 );
		//SetControlSkin( GetDlgItem(	IDC_BUTTON4 )->m_hWnd , "Button1" , "Dialog" );
	}
	else
	{
		::SendMessage( GetDlgItem(	IDC_BUTTON4 )->m_hWnd, WMS_SETSTYLE, 1, 0 );
		//SetControlSkin( GetDlgItem(	IDC_BUTTON4 )->m_hWnd , "ButtonTemplate" , NULL );
	}
	bFlag = !bFlag ;
}
