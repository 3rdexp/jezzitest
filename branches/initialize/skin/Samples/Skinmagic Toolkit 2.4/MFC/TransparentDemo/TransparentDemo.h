// TransparentDemo.h : main header file for the TRANSPARENTDEMO application
//

#if !defined(AFX_TRANSPARENTDEMO_H__5EACA130_2CF1_4B3E_A755_6CAF7E37D507__INCLUDED_)
#define AFX_TRANSPARENTDEMO_H__5EACA130_2CF1_4B3E_A755_6CAF7E37D507__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTransparentDemoApp:
// See TransparentDemo.cpp for the implementation of this class
//

class CTransparentDemoApp : public CWinApp
{
public:
	CTransparentDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentDemoApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTransparentDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTDEMO_H__5EACA130_2CF1_4B3E_A755_6CAF7E37D507__INCLUDED_)
