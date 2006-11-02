// MFCOutlookBar.h : main header file for the MFCOUTLOOKBAR application
//

#if !defined(AFX_MFCOUTLOOKBAR_H__4FA1BC91_275A_43EF_B921_A8B899BF126E__INCLUDED_)
#define AFX_MFCOUTLOOKBAR_H__4FA1BC91_275A_43EF_B921_A8B899BF126E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarApp:
// See MFCOutlookBar.cpp for the implementation of this class
//

class CMFCOutlookBarApp : public CWinApp
{
public:
	CMFCOutlookBarApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCOutlookBarApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMFCOutlookBarApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCOUTLOOKBAR_H__4FA1BC91_275A_43EF_B921_A8B899BF126E__INCLUDED_)
