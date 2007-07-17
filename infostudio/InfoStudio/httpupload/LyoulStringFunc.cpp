#include "stdafx.h"
#include "LyoulStringFunc.h"

// $LastChangedRevision: 7 $
// $LastChangedDate: 2004-05-03 01:23:34 +0900 (Mon, 03 May 2004) $

namespace Lyoul {


/************************************************************************************
 * �����ڵ� ���ڿ��� Ansi ���ڿ��� ��ȯ�ϴµ� �ʿ��� ���ڼ��� ���Ѵ�. ���ڼ�����
 * NULL���ڴ� ���Ե��� �ʴ´�. ������ GetLastError �Լ��� ����Ͽ� ������ 
 * Ȯ���� �� �ִ�.
 * 
 * [in] wszSrc        : Ansi�� ��ȯ�� Unicode ���ڿ�
 * [out] pcCHNeeded   : ��ȯ�ϴµ� �ʿ��� ���ڼ� (NULL ���� ����)
 */
HRESULT
NeededCharsForUnicode2Ansi (LPCWSTR wszSrc, int * pcchNeeded)
{
	// �Ķ���� �˻�
	_ASSERTE ( (wszSrc != NULL) && (pcchNeeded != NULL) ) ;

	size_t		cchSrc ;
	if ( FAILED (::StringCchLengthW (wszSrc, STRSAFE_MAX_CCH, &cchSrc)) ) {
		::SetLastError (ERROR_INVALID_PARAMETER) ;
		return E_INVALIDARG ;
	}

	// ���ڿ� ���̰� 0�� ���
	if ( cchSrc == 0 ) {
		*pcchNeeded = 0 ;
		return S_OK ;
	}
	
	// �ʿ��� �޸� �� ���ϱ�
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
 * �����ڵ� ���ڿ��� Ansi ���ڿ��� ��ȯ�Ѵ�. ������ GetLastError �Լ��� ����Ͽ�
 * ������ Ȯ���� �� �ִ�. szDest�� �޸𸮴� cbDest��ŭ �Ҵ�Ǿ� �־�� �Ѵ�.
 * 
 * [out] szDest       : Ansi���ڿ��� ��ȯ�� ���ڿ��� �޾ƿ� ����
 * [in] cchDest       : szDest ���ۿ� �Ҵ�� ���� ��
 * [in] wszSrc        : Ansi�� ��ȯ�� Unicode ���ڿ�
 */
HRESULT
Unicode2Ansi (LPSTR szDest, int cchDest, LPCWSTR wszSrc)
{
	_ASSERTE ( !(szDest == NULL || cchDest == 0 || wszSrc == NULL) ) ;

	// ANSI �������� ��ȯ
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
 * �����ڵ� ���ڿ��� Ansi ���ڿ��� ��ȯ�Ѵ�. ������ GetLastError �Լ��� ����Ͽ�
 * ������ Ȯ���� �� �ִ�. �޸𸮴� HeapFree �Լ��� ����Ͽ� �����ؾ� �Ѵ�.
 * 
 * [out] pszDest      : Ansi���ڿ��� ��ȯ�� ���ڿ��� �޾ƿ� ������
 * [in] wszSrc        : Ansi�� ��ȯ�� Unicode ���ڿ�
 * [in] hHeap         : �޸𸮸� �Ҵ��ϴµ� ����� ��. NULL ��� ���μ����� �� ���.
 */
HRESULT
Unicode2Ansi (LPSTR * pszDest, LPCWSTR wszSrc, HANDLE hHeap)
{
	// �Ķ���� �˻�
	_ASSERTE ( !(pszDest == NULL || wszSrc == NULL) ) ;

	*pszDest = NULL ;

	if ( hHeap == NULL && (hHeap = ::GetProcessHeap ()) == NULL )
		return E_FAIL ;

	HRESULT			hResult ;

	int				cchNeeded ;
	if ( FAILED (hResult = NeededCharsForUnicode2Ansi  (wszSrc, &cchNeeded)) )
		return hResult ;

	// �޸� �Ҵ�
	*pszDest = static_cast<LPSTR> (::HeapAlloc (hHeap, 0, cchNeeded + 1)) ;
	if ( *pszDest == NULL ) {
		::SetLastError (ERROR_NOT_ENOUGH_MEMORY) ;
		return E_OUTOFMEMORY ;
	}

	// ANSI �������� ��ȯ
	if ( FAILED (hResult = Unicode2Ansi (*pszDest, cchNeeded + 1, wszSrc)) ) {
		DWORD		dwLastErr = ::GetLastError () ;
		::HeapFree (hHeap, 0, *pszDest) ;
		::SetLastError (dwLastErr) ;
		return hResult ;
	}

	return S_OK ;
}


/************************************************************************************
 * �����ڵ� ���ڿ��� UTF-8 ���ڿ��� ��ȯ�ϴµ� �ʿ��� ���ڼ��� ���Ѵ�. ���ڼ�����
 * NULL���ڴ� ���Ե��� �ʴ´�. ������ GetLastError �Լ��� ����Ͽ� ������ 
 * Ȯ���� �� �ִ�.
 * 
 * [in] wszSrc        : UTF-8�� ��ȯ�� Unicode ���ڿ�
 * [out] pcchNeeded    : ��ȯ�ϴµ� �ʿ��� ���ڼ� (NULL ���� ����)
 */
HRESULT
NeededCharsForUnicode2UTF8 (LPCWSTR wszSrc, int * pcchNeeded)
{
	// �Ķ���� �˻�
	_ASSERTE ( (wszSrc != NULL) && (pcchNeeded != NULL) ) ;

	size_t		cchSrc ;
	if ( FAILED (::StringCchLengthW (wszSrc, STRSAFE_MAX_CCH, &cchSrc)) ) {
		::SetLastError (ERROR_INVALID_PARAMETER) ;
		return E_INVALIDARG ;
	}

	// ���ڿ� ���̰� 0�� ���
	if ( cchSrc == 0 ) {
		*pcchNeeded = 0 ;
		return S_OK ;
	}

	// �ʿ��� �޸� �� ���ϱ�
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
 * �����ڵ� ���ڿ��� UTF-8 ���ڿ��� ��ȯ�Ѵ�. ������ GetLastError �Լ��� ����Ͽ�
 * ������ Ȯ���� �� �ִ�. szDest�� �޸𸮴� cchDest��ŭ �Ҵ�Ǿ� �־�� �Ѵ�.
 * 
 * [out] szDest       : UTF-8���ڿ��� ��ȯ�� ���ڿ��� �޾ƿ� ����
 * [in] cchDest       : szDest ���ۿ� �Ҵ�� ���� ��
 * [in] wszSrc        : UTF-8�� ��ȯ�� Unicode ���ڿ�
 */
HRESULT
Unicode2UTF8 (LPSTR szDest, int cchDest, LPCWSTR wszSrc)
{
	_ASSERTE ( !(szDest == NULL || cchDest == 0 || wszSrc == NULL) ) ;

	// UTF-8 �������� ��ȯ
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
 * �����ڵ� ���ڿ��� UTF-8 ���ڿ��� ��ȯ�Ѵ�. ������ GetLastError �Լ��� ����Ͽ�
 * ������ Ȯ���� �� �ִ�. �޸𸮴� HeapFree �Լ��� ����Ͽ� �����ؾ� �Ѵ�.
 * 
 * [out] pszDest      : UTF-8 ���ڿ��� ��ȯ�� ���ڿ��� �޾ƿ� ������
 * [in] wszSrc        : UTF-8�� ��ȯ�� Unicode ���ڿ�
 * [in] hHeap         : �޸𸮸� �Ҵ��ϴµ� ����� ��. NULL ��� ���μ����� �� ���.
 */
HRESULT
Unicode2UTF8 (LPSTR * pszDest, LPCWSTR wszSrc, HANDLE hHeap)
{
	// �Ķ���� �˻�
	_ASSERTE ( !(pszDest == NULL || wszSrc == NULL) ) ;

	*pszDest = NULL ;

	if ( hHeap == NULL && (hHeap = ::GetProcessHeap ()) == NULL )
		return E_FAIL ;

	HRESULT			hResult ;

	int				cchNeeded ;
	if ( FAILED (hResult = NeededCharsForUnicode2UTF8  (wszSrc, &cchNeeded)) )
		return hResult ;

	// �޸� �Ҵ�
	*pszDest = static_cast<LPSTR> (::HeapAlloc (hHeap, 0, cchNeeded + 1)) ;
	if ( *pszDest == NULL ) {
		::SetLastError (ERROR_NOT_ENOUGH_MEMORY) ;
		return E_OUTOFMEMORY ;
	}

	// ANSI �������� ��ȯ
	if ( FAILED (hResult = Unicode2UTF8 (*pszDest, cchNeeded + 1, wszSrc)) ) {
		DWORD		dwLastErr = ::GetLastError () ;
		::HeapFree (hHeap, 0, *pszDest) ;
		::SetLastError (dwLastErr) ;
		return hResult ;
	}

	return S_OK ;
}


/************************************************************************************
 * �����ڵ� ���ڿ��� Ansi ���ڿ��� ��ȯ�ϴµ� �ʿ��� ���ڼ��� ���Ѵ�. ���ڼ�����
 * NULL���ڴ� ���Ե��� �ʴ´�. ������ GetLastError �Լ��� ����Ͽ� ������ 
 * Ȯ���� �� �ִ�.
 * 
 * [in] szSrc        : Unicode�� ��ȯ�� Ansi ���ڿ�
 * [out] pcchNeeded  : ��ȯ�ϴµ� �ʿ��� WCHAR ���ڼ� (NULL ���� ���� ����)
 */
HRESULT
NeededCharsForAnsi2Unicode (LPCSTR szSrc, int * pcchNeeded)
{
	// �Ķ���� �˻�
	_ASSERTE ( (szSrc != NULL) && (pcchNeeded != NULL) ) ;

	size_t		cchSrc ;
	if ( FAILED (::StringCchLengthA (szSrc, STRSAFE_MAX_CCH, &cchSrc)) ) {
		::SetLastError (ERROR_INVALID_PARAMETER) ;
		return E_INVALIDARG ;
	}

	// ���ڿ� ���̰� 0�� ���
	if ( cchSrc == 0 ) {
		*pcchNeeded = 0 ;
		return S_OK ;
	}

	// �ʿ��� �޸� �� ���ϱ�
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
 * �����ڵ� ���ڿ��� Ansi ���ڿ��� ��ȯ�Ѵ�. ������ GetLastError �Լ��� ����Ͽ�
 * ������ Ȯ���� �� �ִ�. wszDest�� �޸𸮴� cchDest���� WCHAR���ڸ� ���� �� ����
 * ��ŭ �Ҵ�Ǿ� �־�� �Ѵ�.
 * 
 * [out] wszDest      : Unicode ���ڿ��� ��ȯ�� ���ڿ��� �޾ƿ� ������
 * [in] cchDest       : wszDest�� �Ҵ��� �� �ִ� WCHAR���� ��
 * [in] szSrc         : Unicode�� ��ȯ�� Ansi ���ڿ�
 */
HRESULT
Ansi2Unicode (LPWSTR wszDest, int cchDest, LPCSTR szSrc)
{
	_ASSERTE ( !(wszDest == NULL || cchDest == 0 || szSrc == NULL) ) ;

	// Unicode �������� ��ȯ
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
 * �����ڵ� ���ڿ��� Ansi ���ڿ��� ��ȯ�Ѵ�. ������ GetLastError �Լ��� ����Ͽ�
 * ������ Ȯ���� �� �ִ�. �޸𸮴� HeapFree �Լ��� ����Ͽ� �����ؾ� �Ѵ�.
 * 
 * [out] pwszDest     : Unicode ���ڿ��� ��ȯ�� ���ڿ��� �޾ƿ� ������
 * [in] szSrc         : Unicode�� ��ȯ�� Ansi ���ڿ�
 * [in] hHeap         : �޸𸮸� �Ҵ��ϴµ� ����� ��. NULL ��� ���μ����� �� ���.
 */
HRESULT
Ansi2Unicode (LPWSTR * pwszDest, LPCSTR szSrc, HANDLE hHeap)
{
	// �Ķ���� �˻�
	_ASSERTE ( !(pwszDest == NULL || szSrc == NULL) ) ;

	*pwszDest = NULL ;

	if ( hHeap == NULL && (hHeap = ::GetProcessHeap ()) == NULL )
		return E_FAIL ;

	HRESULT		hResult ;
	int			cchNeeded ;

	if ( FAILED (hResult = NeededCharsForAnsi2Unicode (szSrc, &cchNeeded)) )
		return hResult ;

	// �޸� �Ҵ�
	*pwszDest = static_cast<LPWSTR> (::HeapAlloc (hHeap, 0, (cchNeeded + 1) * sizeof (WCHAR))) ;
	if ( *pwszDest == NULL ) {
		::SetLastError (ERROR_NOT_ENOUGH_MEMORY) ;
		return E_OUTOFMEMORY ;
	}

	// Unicode �������� ��ȯ
	if ( FAILED (hResult = Ansi2Unicode (*pwszDest, cchNeeded + 1, szSrc)) ) {
		DWORD		dwLastErr = ::GetLastError () ;
		::HeapFree (hHeap, 0, *pwszDest) ;
		::SetLastError (dwLastErr) ;
		return E_FAIL ;
	}

	return S_OK ;
}


}
