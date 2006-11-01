// mfc42_sr.h : main header file for the MFC42_SR application
//

#if !defined(AFX_MFC42_SR_H__CA9D52F3_0CE6_477D_843E_1685397F9A15__INCLUDED_)
#define AFX_MFC42_SR_H__CA9D52F3_0CE6_477D_843E_1685397F9A15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srApp:
// See mfc42_sr.cpp for the implementation of this class
//

class CMfc42_srApp : public CWinApp
{
public:
	CMfc42_srApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc42_srApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMfc42_srApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC42_SR_H__CA9D52F3_0CE6_477D_843E_1685397F9A15__INCLUDED_)
