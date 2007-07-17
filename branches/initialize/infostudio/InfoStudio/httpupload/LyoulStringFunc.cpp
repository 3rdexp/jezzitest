#include "stdafx.h"
#include "LyoulStringFunc.h"

// $LastChangedRevision: 7 $
// $LastChangedDate: 2004-05-03 01:23:34 +0900 (Mon, 03 May 2004) $

namespace Lyoul {


/************************************************************************************
 * 유니코드 문자열을 Ansi 문자열로 변환하는데 필요한 문자수를 구한다. 문자수에는
 * NULL문자는 포함되지 않는다. 에러시 GetLastError 함수를 사용하여 에러를 
 * 확인할 수 있다.
 * 
 * [in] wszSrc        : Ansi로 변환할 Unicode 문자열
 * [out] pcCHNeeded   : 변환하는데 필요한 문자수 (NULL 포함 않함)
 */
HRESULT
NeededCharsForUnicode2Ansi (LPCWSTR wszSrc, int * pcchNeeded)
{
	// 파라미터 검사
	_ASSERTE ( (wszSrc != NULL) && (pcchNeeded != NULL) ) ;

	size_t		cchSrc ;
	if ( FAILED (::StringCchLengthW (wszSrc, STRSAFE_MAX_CCH, &cchSrc)) ) {
		::SetLastError (ERROR_INVALID_PARAMETER) ;
		return E_INVALIDARG ;
	}

	// 문자열 길이가 0일 경우
	if ( cchSrc == 0 ) {
		*pcchNeeded = 0 ;
		return S_OK ;
	}
	
	// 필요한 메모리 양 구하기
	if ( 0 == (*pcchNeeded = ::WideCharToMultiByte (
			CP_ACP
			, 0
			, wszSrc
			, static_cast<int> (cchSrc + 1)
			, NULL
			, 0
			, NULL
			, NULL)) )
		return E_FAIL ;

	*pcchNeeded -= 1 ;
	return S_OK ;
}

/************************************************************************************
 * 유니코드 문자열을 Ansi 문자열로 변환한다. 에러시 GetLastError 함수를 사용하여
 * 에러를 확인할 수 있다. szDest의 메모리는 cbDest만큼 할당되어 있어야 한다.
 * 
 * [out] szDest       : Ansi문자열로 변환된 문자열을 받아올 버퍼
 * [in] cchDest       : szDest 버퍼에 할당된 문자 수
 * [in] wszSrc        : Ansi로 변환할 Unicode 문자열
 */
HRESULT
Unicode2Ansi (LPSTR szDest, int cchDest, LPCWSTR wszSrc)
{
	_ASSERTE ( !(szDest == NULL || cchDest == 0 || wszSrc == NULL) ) ;

	// ANSI 포멧으로 변환
	if ( 0 == ::WideCharToMultiByte (
			CP_ACP
			, 0
			, wszSrc
			, -1
			, szDest
			, cchDest
			, NULL
			, NULL) )
			return E_FAIL ;
	return S_OK ;
}

/************************************************************************************
 * 유니코드 문자열을 Ansi 문자열로 변환한다. 에러시 GetLastError 함수를 사용하여
 * 에러를 확인할 수 있다. 메모리는 HeapFree 함수를 사용하여 해제해야 한다.
 * 
 * [out] pszDest      : Ansi문자열로 변환된 문자열을 받아올 포인터
 * [in] wszSrc        : Ansi로 변환할 Unicode 문자열
 * [in] hHeap         : 메모리를 할당하는데 사용할 힙. NULL 경우 프로세스의 힙 사용.
 */
HRESULT
Unicode2Ansi (LPSTR * pszDest, LPCWSTR wszSrc, HANDLE hHeap)
{
	// 파라미터 검사
	_ASSERTE ( !(pszDest == NULL || wszSrc == NULL) ) ;

	*pszDest = NULL ;

	if ( hHeap == NULL && (hHeap = ::GetProcessHeap ()) == NULL )
		return E_FAIL ;

	HRESULT			hResult ;

	int				cchNeeded ;
	if ( FAILED (hResult = NeededCharsForUnicode2Ansi  (wszSrc, &cchNeeded)) )
		return hResult ;

	// 메모리 할당
	*pszDest = static_cast<LPSTR> (::HeapAlloc (hHeap, 0, cchNeeded + 1)) ;
	if ( *pszDest == NULL ) {
		::SetLastError (ERROR_NOT_ENOUGH_MEMORY) ;
		return E_OUTOFMEMORY ;
	}

	// ANSI 포멧으로 변환
	if ( FAILED (hResult = Unicode2Ansi (*pszDest, cchNeeded + 1, wszSrc)) ) {
		DWORD		dwLastErr = ::GetLastError () ;
		::HeapFree (hHeap, 0, *pszDest) ;
		::SetLastError (dwLastErr) ;
		return hResult ;
	}

	return S_OK ;
}


/************************************************************************************
 * 유니코드 문자열을 UTF-8 문자열로 변환하는데 필요한 문자수를 구한다. 문자수에는
 * NULL문자는 포함되지 않는다. 에러시 GetLastError 함수를 사용하여 에러를 
 * 확인할 수 있다.
 * 
 * [in] wszSrc        : UTF-8로 변환할 Unicode 문자열
 * [out] pcchNeeded    : 변환하는데 필요한 문자수 (NULL 포함 않함)
 */
HRESULT
NeededCharsForUnicode2UTF8 (LPCWSTR wszSrc, int * pcchNeeded)
{
	// 파라미터 검사
	_ASSERTE ( (wszSrc != NULL) && (pcchNeeded != NULL) ) ;

	size_t		cchSrc ;
	if ( FAILED (::StringCchLengthW (wszSrc, STRSAFE_MAX_CCH, &cchSrc)) ) {
		::SetLastError (ERROR_INVALID_PARAMETER) ;
		return E_INVALIDARG ;
	}

	// 문자열 길이가 0일 경우
	if ( cchSrc == 0 ) {
		*pcchNeeded = 0 ;
		return S_OK ;
	}

	// 필요한 메모리 양 구하기
	if ( 0 == (*pcchNeeded = ::WideCharToMultiByte (
			CP_UTF8
			, 0
			, wszSrc
			, static_cast<int> (cchSrc + 1)
			, NULL
			, 0
			, NULL
			, NULL)) )
		return E_FAIL ;

	*pcchNeeded -= 1 ;
	return S_OK ;
}

/************************************************************************************
 * 유니코드 문자열을 UTF-8 문자열로 변환한다. 에러시 GetLastError 함수를 사용하여
 * 에러를 확인할 수 있다. szDest의 메모리는 cchDest만큼 할당되어 있어야 한다.
 * 
 * [out] szDest       : UTF-8문자열로 변환된 문자열을 받아올 버퍼
 * [in] cchDest       : szDest 버퍼에 할당된 문자 수
 * [in] wszSrc        : UTF-8로 변환할 Unicode 문자열
 */
HRESULT
Unicode2UTF8 (LPSTR szDest, int cchDest, LPCWSTR wszSrc)
{
	_ASSERTE ( !(szDest == NULL || cchDest == 0 || wszSrc == NULL) ) ;

	// UTF-8 포멧으로 변환
	if ( 0 == ::WideCharToMultiByte (
			CP_UTF8
			, 0
			, wszSrc
			, -1
			, szDest
			, cchDest
			, NULL
			, NULL) )
			return E_FAIL ;
	return S_OK ;
}

/************************************************************************************
 * 유니코드 문자열을 UTF-8 문자열로 변환한다. 에러시 GetLastError 함수를 사용하여
 * 에러를 확인할 수 있다. 메모리는 HeapFree 함수를 사용하여 해제해야 한다.
 * 
 * [out] pszDest      : UTF-8 문자열로 변환된 문자열을 받아올 포인터
 * [in] wszSrc        : UTF-8로 변환할 Unicode 문자열
 * [in] hHeap         : 메모리를 할당하는데 사용할 힙. NULL 경우 프로세스의 힙 사용.
 */
HRESULT
Unicode2UTF8 (LPSTR * pszDest, LPCWSTR wszSrc, HANDLE hHeap)
{
	// 파라미터 검사
	_ASSERTE ( !(pszDest == NULL || wszSrc == NULL) ) ;

	*pszDest = NULL ;

	if ( hHeap == NULL && (hHeap = ::GetProcessHeap ()) == NULL )
		return E_FAIL ;

	HRESULT			hResult ;

	int				cchNeeded ;
	if ( FAILED (hResult = NeededCharsForUnicode2UTF8  (wszSrc, &cchNeeded)) )
		return hResult ;

	// 메모리 할당
	*pszDest = static_cast<LPSTR> (::HeapAlloc (hHeap, 0, cchNeeded + 1)) ;
	if ( *pszDest == NULL ) {
		::SetLastError (ERROR_NOT_ENOUGH_MEMORY) ;
		return E_OUTOFMEMORY ;
	}

	// ANSI 포멧으로 변환
	if ( FAILED (hResult = Unicode2UTF8 (*pszDest, cchNeeded + 1, wszSrc)) ) {
		DWORD		dwLastErr = ::GetLastError () ;
		::HeapFree (hHeap, 0, *pszDest) ;
		::SetLastError (dwLastErr) ;
		return hResult ;
	}

	return S_OK ;
}


/************************************************************************************
 * 유니코드 문자열을 Ansi 문자열로 변환하는데 필요한 문자수를 구한다. 문자수에는
 * NULL문자는 포함되지 않는다. 에러시 GetLastError 함수를 사용하여 에러를 
 * 확인할 수 있다.
 * 
 * [in] szSrc        : Unicode로 변환할 Ansi 문자열
 * [out] pcchNeeded  : 변환하는데 필요한 WCHAR 문자수 (NULL 문자 포함 안함)
 */
HRESULT
NeededCharsForAnsi2Unicode (LPCSTR szSrc, int * pcchNeeded)
{
	// 파라미터 검사
	_ASSERTE ( (szSrc != NULL) && (pcchNeeded != NULL) ) ;

	size_t		cchSrc ;
	if ( FAILED (::StringCchLengthA (szSrc, STRSAFE_MAX_CCH, &cchSrc)) ) {
		::SetLastError (ERROR_INVALID_PARAMETER) ;
		return E_INVALIDARG ;
	}

	// 문자열 길이가 0일 경우
	if ( cchSrc == 0 ) {
		*pcchNeeded = 0 ;
		return S_OK ;
	}

	// 필요한 메모리 양 구하기
	if ( 0 == (*pcchNeeded = ::MultiByteToWideChar (
			CP_ACP
			, 0
			, szSrc
			, static_cast<int> (cchSrc + 1)
			, NULL
			, 0)) )
		return E_FAIL ;

	*pcchNeeded -= 1 ;
	return S_OK ;
}

/************************************************************************************
 * 유니코드 문자열을 Ansi 문자열로 변환한다. 에러시 GetLastError 함수를 사용하여
 * 에러를 확인할 수 있다. wszDest의 메모리는 cchDest개의 WCHAR문자를 담을 수 있을
 * 만큼 할당되어 있어야 한다.
 * 
 * [out] wszDest      : Unicode 문자열로 변환된 문자열을 받아올 포인터
 * [in] cchDest       : wszDest에 할당할 수 있는 WCHAR문자 수
 * [in] szSrc         : Unicode로 변환할 Ansi 문자열
 */
HRESULT
Ansi2Unicode (LPWSTR wszDest, int cchDest, LPCSTR szSrc)
{
	_ASSERTE ( !(wszDest == NULL || cchDest == 0 || szSrc == NULL) ) ;

	// Unicode 포멧으로 변환
	if ( 0 == ::MultiByteToWideChar (
			CP_ACP
			, 0
			, szSrc
			, -1
			, wszDest
			, cchDest
			) )
		return E_FAIL ;

	return S_OK ;
}

/************************************************************************************
 * 유니코드 문자열을 Ansi 문자열로 변환한다. 에러시 GetLastError 함수를 사용하여
 * 에러를 확인할 수 있다. 메모리는 HeapFree 함수를 사용하여 해제해야 한다.
 * 
 * [out] pwszDest     : Unicode 문자열로 변환된 문자열을 받아올 포인터
 * [in] szSrc         : Unicode로 변환할 Ansi 문자열
 * [in] hHeap         : 메모리를 할당하는데 사용할 힙. NULL 경우 프로세스의 힙 사용.
 */
HRESULT
Ansi2Unicode (LPWSTR * pwszDest, LPCSTR szSrc, HANDLE hHeap)
{
	// 파라미터 검사
	_ASSERTE ( !(pwszDest == NULL || szSrc == NULL) ) ;

	*pwszDest = NULL ;

	if ( hHeap == NULL && (hHeap = ::GetProcessHeap ()) == NULL )
		return E_FAIL ;

	HRESULT		hResult ;
	int			cchNeeded ;

	if ( FAILED (hResult = NeededCharsForAnsi2Unicode (szSrc, &cchNeeded)) )
		return hResult ;

	// 메모리 할당
	*pwszDest = static_cast<LPWSTR> (::HeapAlloc (hHeap, 0, (cchNeeded + 1) * sizeof (WCHAR))) ;
	if ( *pwszDest == NULL ) {
		::SetLastError (ERROR_NOT_ENOUGH_MEMORY) ;
		return E_OUTOFMEMORY ;
	}

	// Unicode 포멧으로 변환
	if ( FAILED (hResult = Ansi2Unicode (*pwszDest, cchNeeded + 1, szSrc)) ) {
		DWORD		dwLastErr = ::GetLastError () ;
		::HeapFree (hHeap, 0, *pwszDest) ;
		::SetLastError (dwLastErr) ;
		return E_FAIL ;
	}

	return S_OK ;
}


}
