// TransparentDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TransparentDemo.h"
#include "TransparentDemoDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
MENUIMAGEINFO MenuImage[]={
	{IDM_FILE_OPEN , 0 },
	{IDM_FILE_SAVE , 1 },
	{IDM_FILE_SAVEAS , 2 },
	{IDM_FILE_PRINT , 3 },
	{IDM_HELP_ABOUT , 4 },
	{IDM_FILE_PRINTPREVIEW , 5}
};
*/
/////////////////////////////////////////////////////////////////////////////
// CTransparentDemoDlg dialog

CTransparentDemoDlg::CTransparentDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransparentDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransparentDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTransparentDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransparentDemoDlg)
	DDX_Control(pDX, IDC_ANIMATE1, m_ctrlAnimate);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTransparentDemoDlg, CDialog)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CTransparentDemoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_COMMAND(IDM_FILE_EXIT, OnFileExit)
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransparentDemoDlg message handlers

BOOL CTransparentDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_TabCtrl.InsertItem( 0 , "Page1" , -1 );
	m_TabCtrl.InsertItem( 1 , "Page2" , -1 );
	m_TabCtrl.InsertItem( 2 , "Page3" , -1 );


	SetTimer(1 , 1000 , NULL );

	m_ctrlAnimate.Open( IDR_AVI1 );
	m_ctrlAnimate.Play(0,-1, 3 );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTransparentDemoDlg::OnPaint() 
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
HCURSOR CTransparentDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CTransparentDemoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*
	// TODO: Add your specialized creation code here
	SetSingleDialogSkin(m_hWnd , "MainFrame" );
	//SetShapeWindowSkin(m_hWnd,"Shape2");
       

	m_hMenuImage = CreateImageList( "MenuImageList" );
	if( m_hMenuImage )
	{
		SetWindowMainMenuImage( m_hWnd , MenuImage ,6 ,m_hMenuImage , NULL,NULL );
	}
	*/
	return 0;
}

void CTransparentDemoDlg::OnFileExit() 
{
	// TODO: Add your command handler code here
	//PostMessage( WM_COMMAND , IDOK , 0 );
	OnOK();
}



void CTransparentDemoDlg::OnContextMenu(CWnd*, CPoint point)
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
		VERIFY(menu.LoadMenu(IDR_POUPMENU));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		/*
		//DWORD dwCmd = pPopup->TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		//	pWndPopupOwner);
		DWORD dwCmd = TrackSkinPopupMenu( pPopup->m_hMenu , TPM_RETURNCMD|TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			point.x, point.y, pWndPopupOwner->m_hWnd );
		//DWORD dwCmd = TrackSkinPopupMenuEx( pPopup->m_hMenu , NULL, TPM_RETURNCMD|TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		//	point.x, point.y, pWndPopupOwner->m_hWnd ,
		//	MenuImage , 6 ,m_hMenuImage , NULL,NULL );

		if( dwCmd != 0 )
		{
			CString strR;
			strR.Format("%d" , dwCmd ); 
			AfxMessageBox( strR , MB_OK );
		}
		*/
	}
}

//CRect r;

void CTransparentDemoDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDCtl == 5000 )
	{
		char szTime[100];
		SYSTEMTIME st;
		GetSystemTime( &st );
		wsprintf( szTime , "%02d:%02d:%02d" , st.wHour ,st.wMinute,st.wSecond );
		int nSave = SaveDC( lpDrawItemStruct->hDC );
		SetBkMode( lpDrawItemStruct->hDC , 1 );
		SetTextColor( lpDrawItemStruct->hDC , RGB(255,255,255) );
		DrawText( lpDrawItemStruct->hDC , szTime , lstrlen(szTime),&lpDrawItemStruct->rcItem,DT_SINGLELINE|DT_LEFT );
		RestoreDC(lpDrawItemStruct->hDC,nSave );
		//r = lpDrawItemStruct->rcItem;
		

		return;
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
/*void NcInvalidateRect(HWND hwnd, RECT *rcRect)
{
	RECT rc;
	HRGN hrgn;

	GetWindowRect(hwnd, &rc);

	OffsetRect(rcRect, rc.left, rc.top);

	hrgn = CreateRectRgnIndirect(rcRect);

	SendMessage(hwnd, WM_NCPAINT, (WPARAM)hrgn, 0);

	DeleteObject(hrgn);
}*/
void CTransparentDemoDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//SetWindowPos( NULL , 0 , 0 , 0 , 0 , SWP_DRAWFRAME|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER );
	//RedrawCaptionStatic( m_hWnd , "OD1" );
	//NcInvalidateRect( m_hWnd , r );

	CDialog::OnTimer(nIDEvent);
}



void CTransparentDemoDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	ImageList_Destroy( m_hMenuImage );
}
