// FormViewDemo.h : main header file for the FORMVIEWDEMO application
//

#if !defined(AFX_FORMVIEWDEMO_H__A9DBFBD3_2523_4520_B2A0_F52392FDDF5D__INCLUDED_)
#define AFX_FORMVIEWDEMO_H__A9DBFBD3_2523_4520_B2A0_F52392FDDF5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoApp:
// See FormViewDemo.cpp for the implementation of this class
//

class CFormViewDemoApp : public CWinApp
{
public:
	CFormViewDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormViewDemoApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFormViewDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMVIEWDEMO_H__A9DBFBD3_2523_4520_B2A0_F52392FDDF5D__INCLUDED_)
