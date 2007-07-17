#pragma once

/************************************************************************************
 * 작성자       : 조형렬
 * 최초작성일   : 2003년 07월 07일
 * 
 * 이 파일은 에러 처리 관련 함수들을 포함하고 있다.
 *
 * $LastChangedRevision: 7 $
 * $LastChangedDate: 2004-05-03 01:23:34 +0900 (Mon, 03 May 2004) $
 *
 */

#include <windows.h>
#include <atlbase.h>
#include <mbstring.h>			// for mbsnbcpy 
#define STRSAFE_NO_DEPRECATE
#include "strsafe.h"			// for String.. functions
#include <crtdbg.h>				// for _ASSERTE 

#include "LyoulStringFunc.h"	// for Ansi2Unicode function

namespace Lyoul {

	void GetSystemErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc = NULL) ;
	void GetSystemErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc = NULL) ;
	inline
	void GetSystemErrMsg (LPTSTR pszBuff, DWORD cchBuff, LPCTSTR pszErrDesc = NULL) {

#ifdef UNICODE
		GetSystemErrMsgW (pszBuff, cchBuff, pszErrDesc) ;
#else
		GetSystemErrMsgA (pszBuff, cchBuff, pszErrDesc) ;
#endif

	}

	void GetSystemErrMsgA (LPSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCSTR pszErrDesc = NULL) ;
	void GetSystemErrMsgW (LPWSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCWSTR pszErrDesc = NULL) ;
	inline
	void GetSystemErrMsg (LPTSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCTSTR pszErrDesc = NULL) {

#ifdef UNICODE
		GetSystemErrMsgW (pszBuff, cchBuff, dwErrCode, pszErrDesc) ;
#else
		GetSystemErrMsgA (pszBuff, cchBuff, dwErrCode, pszErrDesc) ;
#endif

	}


	void GetWinInetErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc = NULL) ;
	void GetWinInetErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc = NULL) ;
	inline
	void GetWinInetErrMsg (LPTSTR pszBuff, DWORD cchBuff, LPCTSTR pszErrDesc = NULL) {

#ifdef UNICODE
		GetWinInetErrMsgW (pszBuff, cchBuff, pszErrDesc) ;
#else
		GetWinInetErrMsgA (pszBuff, cchBuff, pszErrDesc) ;
#endif

	}

	void GetWinInetErrMsgA (LPSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCSTR pszErrDesc = NULL) ;
	void GetWinInetErrMsgW (LPWSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCWSTR pszErrDesc = NULL) ;
	inline
	void GetWinInetErrMsg (LPTSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCTSTR pszErrDesc = NULL) {

#ifdef UNICODE
		GetWinInetErrMsgW (pszBuff, cchBuff, dwErrCode, pszErrDesc) ;
#else
		GetWinInetErrMsgA (pszBuff, cchBuff, dwErrCode, pszErrDesc) ;
#endif

	}


	BOOL GetCOMErrMsgA (LPSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo = NULL, LPCSTR pszErrDesc = NULL) ;
	BOOL GetCOMErrMsgW (LPWSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo = NULL, LPCWSTR pszErrDesc = NULL) ;
	inline
	BOOL GetCOMErrMsg (LPTSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo = NULL, LPCTSTR pszErrDesc = NULL) {

#ifdef UNICODE
		return GetCOMErrMsgW (pszBuff, cchBuff, pIErrorInfo, pszErrDesc) ;
#else
		return GetCOMErrMsgA (pszBuff, cchBuff, pIErrorInfo, pszErrDesc) ;
#endif

	}

	BOOL GetCOMErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc = NULL) ;
	BOOL GetCOMErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc = NULL) ;
	inline
	BOOL GetCOMErrMsg (LPTSTR pszBuff, DWORD cchBuff, LPCTSTR pszErrDesc = NULL) {

#ifdef UNICODE
		return GetCOMErrMsgW (pszBuff, cchBuff, pszErrDesc) ;
#else
		return GetCOMErrMsgA (pszBuff, cchBuff, pszErrDesc) ;
#endif

	}



	HRESULT SetCOMErrInfoA (GUID * pGUID, LPCSTR szSource, LPCSTR szDescription, LPCSTR szHelpFile, DWORD dwHelpContext) ;
	HRESULT SetCOMErrInfoW (GUID * pGUID, LPCWSTR szSource, LPCWSTR szDescription, LPCWSTR szHelpFile, DWORD dwHelpContext) ;
	inline
	HRESULT	SetCOMErrInfo (GUID * pGUID, LPCTSTR szSource, LPCTSTR szDescription, LPCTSTR szHelpFile, DWORD dwHelpContext) {

#ifdef UNICODE
		return SetCOMErrInfoW (pGUID, szSource, szDescription, szHelpFile, dwHelpContext) ;
#else
		return SetCOMErrInfoA (pGUID, szSource, szDescription, szHelpFile, dwHelpContext) ;
#endif

	}

	inline
	HRESULT	SetCOMErrMsgA (LPCSTR szDescription) {
		return SetCOMErrInfoA (NULL, NULL, szDescription, NULL, 0) ;
	}

	inline
	HRESULT	SetCOMErrMsgW (LPCWSTR wszDescription) {
		return SetCOMErrInfoW (NULL, NULL, wszDescription, NULL, 0) ;
	}

	inline
	HRESULT	SetCOMErrMsg (LPCTSTR szDescription) {

#ifdef UNICODE
		return SetCOMErrMsgW (szDescription) ;
#else
		return SetCOMErrMsgA (szDescription) ;
#endif

	}


#ifndef SAFEFREE
#	define SAFEFREE(x) if(x){ free((void *)x); x = NULL; }
#endif

#ifndef INVALID_SET_FILE_POINTER
#	define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

}

