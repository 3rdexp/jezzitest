// TestDll.h : main header file for the TESTDLL DLL
//

#if !defined(AFX_TESTDLL_H__FD9EA01F_5F7A_4A82_A2C5_736E3A552F75__INCLUDED_)
#define AFX_TESTDLL_H__FD9EA01F_5F7A_4A82_A2C5_736E3A552F75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestDllApp
// See TestDll.cpp for the implementation of this class
//
extern "C" __declspec(dllexport) BOOL ScanAdware(HWND hDlg, DWORD iProcessBar);

class CTestDllApp : public CWinApp
{
public:
	CTestDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTestDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLL_H__FD9EA01F_5F7A_4A82_A2C5_736E3A552F75__INCLUDED_)
