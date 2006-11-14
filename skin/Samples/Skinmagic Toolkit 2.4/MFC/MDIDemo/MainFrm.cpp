// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MDIDemo.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDM_LOADSKIN, OnLoadskin)
	ON_COMMAND(IDM_DISABLE, OnDisable)
	//}}AFX_MSG_MAP
	ON_COMMAND( 32773 , OnButton )
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
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//CBRS_TOOLTIPS
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER |  CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
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

	//m_wndStatusBar.ModifyStyle( 0 , WS_CLIPCHILDREN );
	//m_SButton.Create("X",WS_CHILD|WS_VISIBLE,CRect(10,1,40,18),&m_wndStatusBar , 100 );
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	
	CToolBar* pToolbar = NULL;
	pToolbar = (CToolBar*)CWnd::FromHandle(m_wndToolBar.GetSafeHwnd()); 

	/*
	HMODULE  hLibSkin = LoadLibrary("skin.dll");
	typedef BOOL (WINAPI *InitToolBar)(HWND hWnd);
	InitToolBar pf = (InitToolBar)GetProcAddress(hLibSkin, "InitToolBar");
	ASSERT(pf);
	if(pf)
	{
		pf(m_wndToolBar.GetSafeHwnd());
	}
	*/
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
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
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnLoadskin() 
{
	// TODO: Add your command handler code here
	TCHAR strExt[]= _T("SkinMagic Files(*.smf)|*.smf||");
	CFileDialog dlg( TRUE, _T("*.smf"), _T("*.smf") , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		             , strExt ,  this );

	if( dlg.DoModal() == IDOK  )
	{
		//LoadSkinFile( (LPCTSTR)(dlg.GetPathName()) );
	}
}

void CMainFrame::OnButton()
{
   static bb = FALSE;
   bb = ! bb;
}

void CMainFrame::OnDisable() 
{
	// TODO: Add your command handler code here
	//HMENU  hmenu = GetSkinMenu(m_hWnd);

}
