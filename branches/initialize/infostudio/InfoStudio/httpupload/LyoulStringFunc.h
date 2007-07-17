#pragma once

/************************************************************************************
 * �ۼ���       : ������
 * �����ۼ���   : 2003�� 11�� 12��
 * 
 * �� ������ ���ڿ��� �ٷ�� �Լ����� �����ϰ� �ִ�.
 *
 * $LastChangedRevision: 7 $
 * $LastChangedDate: 2004-05-03 01:23:34 +0900 (Mon, 03 May 2004) $
 *
 */

#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include "strsafe.h"			// for String... functions
#include <crtdbg.h>				// for _ASSERTE 


namespace Lyoul {

	HRESULT NeededCharsForUnicode2Ansi (LPCWSTR wszSrc, int * pcchNeeded) ;
	HRESULT Unicode2Ansi (LPSTR szDest, int cchDest, LPCWSTR wszSrc) ;
	HRESULT Unicode2Ansi (LPSTR * pszDest, LPCWSTR wszSrc, HANDLE hHeap) ;

	HRESULT NeededCharsForUnicode2UTF8 (LPCWSTR wszSrc, int * pcchNeeded) ;
	HRESULT Unicode2UTF8 (LPSTR szDest, int cchDest, LPCWSTR wszSrc) ;
    HRESULT Unicode2UTF8 (LPSTR * pszDest, LPCWSTR wszSrc, HANDLE hHeap) ;

	HRESULT NeededCharsForAnsi2Unicode (LPCSTR szSrc, int * pcchNeeded) ;
	HRESULT Ansi2Unicode (LPWSTR wszDest, int cchDest, LPCSTR szSrc) ;
	HRESULT Ansi2Unicode (LPWSTR * pwszDest, LPCSTR szSrc, HANDLE hHeap) ;

}

