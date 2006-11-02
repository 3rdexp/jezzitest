// MFCControls.h : main header file for the MFCCONTROLS application
//

#if !defined(AFX_MFCCONTROLS_H__3BF7A328_29DF_49E5_8568_E20BBFE63749__INCLUDED_)
#define AFX_MFCCONTROLS_H__3BF7A328_29DF_49E5_8568_E20BBFE63749__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCControlsApp:
// See MFCControls.cpp for the implementation of this class
//

class CMFCControlsApp : public CWinApp
{
public:
	CMFCControlsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCControlsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMFCControlsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCCONTROLS_H__3BF7A328_29DF_49E5_8568_E20BBFE63749__INCLUDED_)
