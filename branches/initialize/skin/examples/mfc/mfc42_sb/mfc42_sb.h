// mfc42_sb.h : main header file for the MFC42_SB application
//

#if !defined(AFX_MFC42_SB_H__9966F754_5C8E_4923_90DB_84E8B05714D7__INCLUDED_)
#define AFX_MFC42_SB_H__9966F754_5C8E_4923_90DB_84E8B05714D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbApp:
// See mfc42_sb.cpp for the implementation of this class
//

class CMfc42_sbApp : public CWinApp
{
public:
	CMfc42_sbApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc42_sbApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMfc42_sbApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC42_SB_H__9966F754_5C8E_4923_90DB_84E8B05714D7__INCLUDED_)
