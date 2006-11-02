// ShapeWindowDemo.h : main header file for the SHAPEWINDOWDEMO application
//

#if !defined(AFX_SHAPEWINDOWDEMO_H__AF513C52_313D_4B30_8A56_EBEC498B0A94__INCLUDED_)
#define AFX_SHAPEWINDOWDEMO_H__AF513C52_313D_4B30_8A56_EBEC498B0A94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoApp:
// See ShapeWindowDemo.cpp for the implementation of this class
//

class CShapeWindowDemoApp : public CWinApp
{
public:
	CShapeWindowDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShapeWindowDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CShapeWindowDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPEWINDOWDEMO_H__AF513C52_313D_4B30_8A56_EBEC498B0A94__INCLUDED_)
