// ShapeWindowDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ShapeWindowDemo.h"
#include "ShapeWindowDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoApp

BEGIN_MESSAGE_MAP(CShapeWindowDemoApp, CWinApp)
	//{{AFX_MSG_MAP(CShapeWindowDemoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoApp construction

CShapeWindowDemoApp::CShapeWindowDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CShapeWindowDemoApp object

CShapeWindowDemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoApp initialization

BOOL CShapeWindowDemoApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	VERIFY( 1 == InitSkinMagicLib( AfxGetInstanceHandle(), "Demo" , 
					  NULL,
					  NULL ) );

	VERIFY( 1 == LoadSkinFromResource( AfxGetInstanceHandle()  , "SKIN" ,"SKINMAGIC" ) ); 
 
	CShapeWindowDemoDlg dlg;

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
