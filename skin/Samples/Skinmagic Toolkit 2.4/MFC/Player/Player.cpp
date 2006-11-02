// Player.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayerApp

BEGIN_MESSAGE_MAP(CPlayerApp, CWinApp)
	//{{AFX_MSG_MAP(CPlayerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerApp construction

CPlayerApp::CPlayerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlayerApp object

CPlayerApp theApp;
CPlayerDlg* glpMainWnd = NULL;
/////////////////////////////////////////////////////////////////////////////
// CPlayerApp initialization

BOOL CPlayerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	VERIFY( 1 == InitSkinMagicLib( AfxGetInstanceHandle(), "Demo" , 
					  NULL,
					  NULL ) );

	if( 1 != LoadSkinFile("..\\Samples\\MFC\\Player\\Player.smf") )
	{
		VERIFY( 1 != LoadSkinFile("Player.smf") );
	}

	m_pMainWnd = new CPlayerDlg;
	m_pMainWnd->ShowWindow( SW_SHOW );

	glpMainWnd = (CPlayerDlg*)m_pMainWnd;

	return TRUE;
}

int CPlayerApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete glpMainWnd;
	return CWinApp::ExitInstance();
}
