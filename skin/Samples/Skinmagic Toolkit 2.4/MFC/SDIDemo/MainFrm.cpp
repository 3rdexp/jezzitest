// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SDIDemo.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND(5000, OnOpenSystemMenu )
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDM_NOSKIN, OnNoskin)
	ON_COMMAND(IDM_LOADSKIN, OnLoadskin)
	ON_COMMAND(IDM_DEFAULT, OnDefault)
	ON_UPDATE_COMMAND_UI(IDM_DEFAULT, OnUpdateDefault)
	ON_UPDATE_COMMAND_UI(IDM_NOSKIN, OnUpdateNoskin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bSkinned = FALSE;
	m_nIndex = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnNoskin() 
{
	// TODO: Add your command handler code here
//	RemoveWindowSkin( m_hWnd );
//	RemoveDialogSkin();
	m_bSkinned = FALSE;
	m_nIndex = 1 ;

}

void CMainFrame::OnLoadskin() 
{
	// TODO: Add your command handler code here
	char strExt[]= "SkinMagic Files(*.smf)|*.smf||";
	CFileDialog dlg( TRUE, "*.smf", "*.smf" , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		             , strExt ,  this );

	if( dlg.DoModal() == IDOK  )
	{
		/*
		LoadSkinFile( (char*)(LPCSTR)(dlg.GetPathName()) );
		if( !m_bSkinned )
		{
			SetWindowSkin( m_hWnd , "MainFrame" );
			SetDialogSkin( "Dialog" );
		}
		*/
		m_nIndex = 2 ;
	}
	
}

void CMainFrame::OnDefault() 
{
	// TODO: Add your command handler code here
//	VERIFY( 1 == LoadSkinFromResource( AfxGetInstanceHandle()  , "DEFAULT" ,"SKINMAGIC") ); 
	if( !m_bSkinned )
	{
//		VERIFY( 1 == SetWindowSkin( m_hWnd , "MainFrame" ));
//		VERIFY( 1 == SetDialogSkin( "Dialog" ) );
		m_nIndex = 0 ;
	}	
}

void CMainFrame::OnUpdateDefault(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_nIndex == 0 );
}

void CMainFrame::OnUpdateNoskin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_nIndex == 1 );
}

void CMainFrame::OnOpenSystemMenu()
{
	
	HMENU hSysMenu = ::GetSystemMenu(m_hWnd,FALSE);
	if( hSysMenu )
	{
		POINT point;
		GetCursorPos( &point );
		DWORD dwCmd = TrackPopupMenu( hSysMenu,TPM_RETURNCMD|TPM_LEFTALIGN ,point.x,point.y,0,m_hWnd,NULL);
		SendMessage( WM_SYSCOMMAND , dwCmd , 0 );
	}
}
