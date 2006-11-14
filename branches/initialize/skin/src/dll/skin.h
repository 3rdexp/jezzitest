// skin.h : main header file for the skin DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CSkinApp
// See skin.cpp for the implementation of this class
//

class CSkinApp : public CWinApp
{
public:
	CSkinApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	static bool init_library(HINSTANCE hInst);
	static bool uninit_library(HINSTANCE hInst);
	DECLARE_MESSAGE_MAP()
};
