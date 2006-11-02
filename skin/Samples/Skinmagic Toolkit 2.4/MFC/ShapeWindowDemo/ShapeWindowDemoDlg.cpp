// ShapeWindowDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShapeWindowDemo.h"
#include "ShapeWindowDemoDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoDlg dialog

CShapeWindowDemoDlg::CShapeWindowDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeWindowDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShapeWindowDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nPos = 0;
}

void CShapeWindowDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeWindowDemoDlg)
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_MonthCalCtrl);
	DDX_Control(pDX, IDC_BUTTON2, m_Button2);
	DDX_Control(pDX, IDC_BUTTON1, m_Button1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShapeWindowDemoDlg, CDialog)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CShapeWindowDemoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_ITEM1, OnItem1)
	ON_COMMAND(IDM_ITEM2, OnItem2)
	ON_COMMAND(IDM_ITEM9, OnItem9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoDlg message handlers

BOOL CShapeWindowDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetShapeWindowSkin( m_hWnd , "ShapeWnd2" );
	
	// TODO: Add extra initialization here
	//m_myProg.SetRange32( 0 , 100 );
	//m_myScroll.SetScrollRange( 0 , 100 );
	//SetTimer( 1000 , 500 , NULL );
	
	SetDialogSkin("Dialog");

	CRect rc;
	GetSkinClientRect( m_hWnd , rc );
	m_MonthCalCtrl.MoveWindow( rc );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShapeWindowDemoDlg::OnPaint() 
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
HCURSOR CShapeWindowDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CShapeWindowDemoDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	/*m_nPos+=5;
	m_nPos = m_nPos % 101;
	m_myProg.SetPos( m_nPos );
	m_myScroll.SetScrollPos( m_nPos );

	CString str;
	str.Format("%02d:%02d" , m_nPos/60 , m_nPos%60 );
	m_myLabel.SetWindowText( str );*/

	FlashWindow(TRUE);
	//KillTimer(1);

	CDialog::OnTimer(nIDEvent);
}

void CShapeWindowDemoDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CShapeWindowDemoDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox("Hello.................................., World!");
	//SetTimer( 1 , 2000 , NULL );
	//ShowWindow( SW_MINIMIZE );
	static b = TRUE;
	m_Button1.SetState( b );
	b=!b;
}

void CShapeWindowDemoDlg::OnContextMenu(CWnd*, CPoint point)
{

	// CG: This block was added by the Pop-up Menu component
	{
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(CG_IDR_POPUP_SHAPE_WINDOW_DEMO_DLG));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		//pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		//	pWndPopupOwner);
		TrackSkinPopupMenu( pPopup->m_hMenu , TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x, point.y , m_hWnd );
	}
}

void CShapeWindowDemoDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//OnContextMenu( this , point );
	CDialog::OnRButtonDown(nFlags, point);
}

void CShapeWindowDemoDlg::OnItem1() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("popup 1" );
}

void CShapeWindowDemoDlg::OnItem2() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("popup 2" );
}

void CShapeWindowDemoDlg::OnItem9() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("popup 3" );
}
