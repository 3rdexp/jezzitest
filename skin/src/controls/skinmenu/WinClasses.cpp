// WinClasses.cpp: implementation of the CWinClasses class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinClasses.h"
#include "../../base/wclassdefines.h"

// helper function for everyone to use
void TRACEWND(LPCTSTR szFunctionName, HWND hWnd)
{
#ifdef _DEBUG
	if (hWnd)
	{
		CString sText; 
		//		pWnd->GetWindowText(sText); 
		TRACE ("%s(%s, %s, id = %d)\n", szFunctionName, CWinClasses::GetClass(hWnd), sText, GetDlgCtrlID(hWnd)); 
	}
#endif
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CString CWinClasses::GetClass(HWND hWnd)
{
	static char szWndClass[128] = "";
	
	if (hWnd)
		::GetClassName(hWnd, szWndClass, 127);
	
	return CString(szWndClass);
}

BOOL CWinClasses::IsClass(HWND hWnd, LPCTSTR szClass)
{
	if (hWnd)
	{
		char szWndClass[128] = "";

		::GetClassName(hWnd, szWndClass, 127);
		return (lstrcmpi(szClass, szWndClass) == 0);
	}

	return FALSE;
}

CString CWinClasses::GetClassEx(HWND hWnd)
{
	CString sClass = GetClass(hWnd);
	
	return sClass;
}


