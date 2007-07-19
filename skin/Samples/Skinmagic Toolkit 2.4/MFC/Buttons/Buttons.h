// Buttons.h : main header file for the BUTTONS application
//

#if !defined(AFX_BUTTONS_H__4289D8F9_A7A1_474C_9D93_9F3201A0E769__INCLUDED_)
#define AFX_BUTTONS_H__4289D8F9_A7A1_474C_9D93_9F3201A0E769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CButtonsApp:
// See Buttons.cpp for the implementation of this class
//

class CButtonsApp : public CWinApp
{
public:
	CButtonsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CButtonsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONS_H__4289D8F9_A7A1_474C_9D93_9F3201A0E769__INCLUDED_)