/*
 Copyright (c) 1999, kSet Lab 
 Author: Konstantin Bukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 12/27/1999 2:19:37 PM
 Version: 1.0.0

 Parts of code by:
	Ferdinand Prantl (prantl@ff.cuni.cz) and MFC Godeguru Site (http://www.codeguru.com)
*/

#ifndef _kHyperLink_93785d76_6b70_4912_a306_6abc33fc7f92
#define _kHyperLink_93785d76_6b70_4912_a306_6abc33fc7f92

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WININET_NEED_
#include <Wininet.h>
#include <winnetwk.h>
#include <winnls.h>
#endif

#include <intshcut.h>

class kHyperLink
{
public:
	enum {OPEN, EDIT, PRINT};	
public:
	static bool Open (TCHAR* pLink, int how, HWND hWnd = GetActiveWindow());
	static bool OpenUsingCom (TCHAR* pLink, int how, HWND hWnd = GetActiveWindow());
	static bool OpenUsingShellExecute (TCHAR* pLink, int how, HWND hWnd = GetActiveWindow());
	#ifdef _WININET_NEED_
	static bool ParseURL(TCHAR* pURL, URL_COMPONENTS& urlComponents, DWORD dwFlags = 0);
	static bool GetInternetScheme (TCHAR* pURL, INTERNET_SCHEME& nScheme, DWORD dwFlags = 0);
	#endif
};

#endif //_kHyperLink_93785d76_6b70_4912_a306_6abc33fc7f92

