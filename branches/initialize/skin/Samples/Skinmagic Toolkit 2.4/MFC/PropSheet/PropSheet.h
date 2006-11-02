// PropSheet.h : main header file for the PROPSHEET application
//

#if !defined(AFX_PROPSHEET_H__B83C5C98_60A7_47B3_A444_CC92AB5A5440__INCLUDED_)
#define AFX_PROPSHEET_H__B83C5C98_60A7_47B3_A444_CC92AB5A5440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPropSheetApp:
// See PropSheet.cpp for the implementation of this class
//

class CPropSheetApp : public CWinApp
{
public:
	CPropSheetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropSheetApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPropSheetApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPSHEET_H__B83C5C98_60A7_47B3_A444_CC92AB5A5440__INCLUDED_)
