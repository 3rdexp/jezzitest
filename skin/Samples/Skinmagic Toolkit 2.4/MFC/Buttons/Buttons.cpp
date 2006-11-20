// Buttons.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Buttons.h"
#include "ButtonsDlg.h"

#include "skinitf.h"
#include "skinlib.h"
#pragma comment(lib, "../../../../samples/Skinmagic Toolkit 2.4/bin/skin.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonsApp

BEGIN_MESSAGE_MAP(CButtonsApp, CWinApp)
	//{{AFX_MSG_MAP(CButtonsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonsApp construction

CButtonsApp::CButtonsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CButtonsApp object

CButtonsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CButtonsApp initialization

BOOL CButtonsApp::InitInstance()
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

	LoadSkinDll();

	CButtonsDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CButtonsApp::ExitInstance() 
{
	UnloadSkinDll();
	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::ExitInstance();
}
