// SkinSplitterDemo.h : main header file for the SKINSPLITTERDEMO application
//

#if !defined(AFX_SKINSPLITTERDEMO_H__D8A88672_E7D1_4E51_B5BC_CAD8B17659AA__INCLUDED_)
#define AFX_SKINSPLITTERDEMO_H__D8A88672_E7D1_4E51_B5BC_CAD8B17659AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoApp:
// See SkinSplitterDemo.cpp for the implementation of this class
//

class CSkinSplitterDemoApp : public CWinApp
{
public:
	CSkinSplitterDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSplitterDemoApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSkinSplitterDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSPLITTERDEMO_H__D8A88672_E7D1_4E51_B5BC_CAD8B17659AA__INCLUDED_)
