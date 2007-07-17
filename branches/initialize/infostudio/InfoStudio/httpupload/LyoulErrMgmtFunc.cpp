#include "stdafx.h"
#include "LyoulErrMgmtFunc.h"

// $LastChangedRevision: 7 $
// $LastChangedDate: 2004-05-03 01:23:34 +0900 (Mon, 03 May 2004) $

namespace Lyoul {


/************************************************************************************
 * 사용자 에러메세지에 표준 시스템 에러메세지를 덧붙인 메세지를 생성한다. 
 * 만약 버퍼의 크기가 모든 에러메세지를 담을 정도로 충분하지 않는다면 시스템 
 * 에러메세지는 덧붙여지지 않으며 (잘려진 에러코드가 덧붙여짐) 버퍼의 크기가 
 * 사용자 에러메세지보다 작다면 잘려진 사용자 에러메세지만 복사된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] dwErrCode     : 시스템 에러 코드. bUseLastErr 가 TRUE 일 경우 무시됨.
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 시스템 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 * [in] bUseLastErr   : TRUE 일 경우 dwErrCode 대신 GetLastError () 로 얻어진 에러코드를
 *                      사용함. FALSE 일 경우 dwErrCode 를 사용.
 * [in] pszModuleName : 메세지 테이블을 얻어올 모듈 이름. NULL 일 경우 사용안함
 */
static void
GetSystemErrMsgA_Impl (LPSTR pszBuff, DWORD cchBuff, DWORD dwErrCode
							, LPCSTR pszErrDesc, BOOL bUseLastErr, HMODULE hModule)
{
	// 파라미터 검사
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		return ;
	}
	
	if ( bUseLastErr )
		dwErrCode = ::GetLastError () ;

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// 사용자 메세지 복사
	if ( (pszErrDesc != NULL) && (::strlen (pszErrDesc) > 0) ) {
		::_mbsnbcpy (
			(unsigned char *) (pszBuff + cchUsedBuff)
			, (unsigned char *) (pszErrDesc)
			, cchBuff - cchUsedBuff - 1) ;
		pszBuff[cchBuff - 1] = NULL ;
		cchUsedBuff = ::strlen (pszBuff) ;
	}

	// 모든 버퍼를 전부 사용했다면 종료
	if ( cchUsedBuff >= cchBuff - 1 )
		return ;

	// 메세지 테이블이 저장된 모듈 사용 여부에 따라서
	// FormatMessage 파라미터 설정
	DWORD			dwFmtMsgFlag ;
	dwFmtMsgFlag = FORMAT_MESSAGE_FROM_SYSTEM ;

	if ( hModule )
		dwFmtMsgFlag |= FORMAT_MESSAGE_FROM_HMODULE ;

	// 시스템 에러메세지 복사
	// 버퍼가 에러메시지를 담을 정도로 충분하지 않다면 시스템 
	// 에러메시지는 복사되지 않음
	DWORD			dwWrittenChrs ;
	dwWrittenChrs = ::FormatMessageA (
		dwFmtMsgFlag
		, hModule						// lpSource
		, dwErrCode						// Error Code
		, 0								// dwLanguageID
		, pszBuff + cchUsedBuff			// lpBuffer
		, cchBuff - cchUsedBuff			// nSize
		, NULL) ;						// Arguments


	// 에러메세지를 출력할 수 없다면 (에러등의 이유로)
	// 에러번호 출력
	// StringCbPrintf 자체 에러는 무시함
	if ( dwWrittenChrs == 0 ) {
		::StringCbPrintfA (
			pszBuff + cchUsedBuff
			, (cchBuff - cchUsedBuff) * sizeof (CHAR)
			, "Error Code is %u, FormatMessage Error Code is %u"
			, dwErrCode, ::GetLastError ()
		) ;
	}

}

static void
GetSystemErrMsgW_Impl (LPWSTR pszBuff, DWORD cchBuff, DWORD dwErrCode
							, LPCWSTR pszErrDesc, BOOL bUseLastErr, HMODULE hModule)
{
	// 파라미터 검사
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		return ;
	}
	
	if ( bUseLastErr )
		dwErrCode = ::GetLastError () ;

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// 사용자 메세지 복사
	if ( (pszErrDesc != NULL) && (::wcslen (pszErrDesc) > 0) ) {
		::StringCbCopyW (
			pszBuff + cchUsedBuff
			, (cchBuff - cchUsedBuff) * sizeof (WCHAR)
			, pszErrDesc) ;
		cchUsedBuff = ::wcslen (pszBuff) ;
	}

	// 모든 버퍼를 전부 사용했다면 종료
	if ( cchUsedBuff >= cchBuff - 1 )
		return ;


	// 메세지 테이블이 저장된 모듈 사용 여부에 따라서
	// FormatMessage 파라미터 설정
	DWORD			dwFmtMsgFlag ;
	dwFmtMsgFlag = FORMAT_MESSAGE_FROM_SYSTEM ;

	if ( hModule )
		dwFmtMsgFlag |= FORMAT_MESSAGE_FROM_HMODULE ;

	// 시스템 에러메세지 복사
	// 버퍼가 에러메시지를 담을 정도로 충분하지 않다면 시스템 
	// 에러메시지는 복사되지 않음
	DWORD			dwWrittenChrs ;
	dwWrittenChrs = ::FormatMessageW (
		dwFmtMsgFlag
		, hModule						// lpSource
		, dwErrCode						// Error Code
		, 0								// dwLanguageID
		, pszBuff + cchUsedBuff			// lpBuffer
		, cchBuff - cchUsedBuff			// nSize
		, NULL) ;						// Arguments


	// 에러메세지를 출력할 수 없다면 (에러등의 이유로)
	// 에러번호 출력
	// StringCbPrintf 자체 에러는 무시함
	if ( dwWrittenChrs == 0 ) {
		::StringCbPrintfW (
			pszBuff + cchUsedBuff
			, (cchBuff - cchUsedBuff) * sizeof (WCHAR)
			, L"Error Code is %u, FormatMessage Error Code is %u"
			, dwErrCode, ::GetLastError ()
		) ;
	}

}





/************************************************************************************
 * 사용자 에러메세지에 마지막 에러 코드에 해당하는 표준 시스템 에러메세지를 덧붙인
 * 메세지를 생성한다. 만약 버퍼의 크기가 모든 에러메세지를 담을 정도로 충분하지 않는다면
 * 시스템 에러메세지는 덧붙여지지 않으며 버퍼의 크기가 사용자 에러메세지보다 작다면 
 * 잘려진 사용자 에러메세지만 복사된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 시스템 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 */
void
GetSystemErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc)
{
	GetSystemErrMsgA_Impl (pszBuff, cchBuff, 0, pszErrDesc, TRUE, NULL) ;
}

void
GetSystemErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc)
{
	GetSystemErrMsgW_Impl (pszBuff, cchBuff, 0, pszErrDesc, TRUE, NULL) ;
}


/************************************************************************************
 * 사용자 에러메세지에 에러 코드에 해당하는 표준 시스템 에러메세지를 덧붙인 메세지를 
 * 생성한다. 만약 버퍼의 크기가 모든 에러메세지를 담을 정도로 충분하지 않는다면 시스템 
 * 에러메세지는 덧붙여지지 않으며 버퍼의 크기가 사용자 에러메세지보다 작다면 잘려진
 * 사용자 에러메세지만 복사된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] dwErrCode     : 시스템 에러 코드.
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 시스템 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 */
void
GetSystemErrMsgA (LPSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCSTR pszErrDesc)
{
	GetSystemErrMsgA_Impl (pszBuff, cchBuff, dwErrCode, pszErrDesc, FALSE, NULL) ;
}

void
GetSystemErrMsgW (LPWSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCWSTR pszErrDesc)
{
	GetSystemErrMsgW_Impl (pszBuff, cchBuff, dwErrCode, pszErrDesc, FALSE, NULL) ;
}


/************************************************************************************
 * 사용자 에러메세지에 마지막 에러 코드에 해당하는 wininet.dll 모듈 또는 표준 시스템으로
 * 부터의 에러메세지를 덧붙인 메세지를 생성한다. 만약 버퍼의 크기가 모든 에러메세지를
 * 담을 정도로 충분하지 않는다면 시스템 에러메세지는 덧붙여지지 않으며 버퍼의 크기가
 * 사용자 에러메세지보다 작다면 잘려진 사용자 에러메세지만 복사된다.
 * wininet.dll 모듈의 핸들을 얻어오는데 실패했을 경우 WinInet API의 에러는 에러코드만
 * 표시된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 시스템 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 */
void
GetWinInetErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc)
{
	// 에러 코드 캐슁
	DWORD		dwLastErrCode = ::GetLastError () ;
	GetSystemErrMsgA_Impl (pszBuff, cchBuff, dwLastErrCode, pszErrDesc, FALSE
		, ::GetModuleHandle (TEXT ("wininet.dll"))) ;
}

void
GetWinInetErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc)
{
	// 에러 코드 캐슁
	DWORD		dwLastErrCode = ::GetLastError () ;
	GetSystemErrMsgW_Impl (pszBuff, cchBuff, dwLastErrCode, pszErrDesc, FALSE
		, ::GetModuleHandle (TEXT ("wininet.dll"))) ;
}

/************************************************************************************
 * 사용자 에러메세지에 에러 코드에 해당하는 wininet.dll 모듈 또는 표준 시스템으로
 * 부터의 에러메세지를 덧붙인 메세지를 생성한다. 만약 버퍼의 크기가 모든 에러메세지를
 * 담을 정도로 충분하지 않는다면 시스템 에러메세지는 덧붙여지지 않으며 버퍼의 크기가
 * 사용자 에러메세지보다 작다면 잘려진 사용자 에러메세지만 복사된다.
 * wininet.dll 모듈의 핸들을 얻어오는데 실패했을 경우 WinInet API의 에러는 에러코드만
 * 표시된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] dwErrCode     : 시스템 에러 코드.
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 시스템 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 */
void
GetWinInetErrMsgA (LPSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCSTR pszErrDesc)
{
	GetSystemErrMsgA_Impl (pszBuff, cchBuff, dwErrCode, pszErrDesc, FALSE
		, ::GetModuleHandle (TEXT ("wininet.dll"))) ;
}

void
GetWinInetErrMsgW (LPWSTR pszBuff, DWORD cchBuff, DWORD dwErrCode, LPCWSTR pszErrDesc)
{
	GetSystemErrMsgW_Impl (pszBuff, cchBuff, dwErrCode, pszErrDesc, FALSE
		, ::GetModuleHandle (TEXT ("wininet.dll"))) ;
}







/************************************************************************************
 * 사용자 에러메세지에 표준 COM 에러메세지를 덧붙인 메세지를 생성한다. 
 * 만약 버퍼의 크기가 모든 에러메세지를 담을 정도로 충분하지 않는다면 COM 
 * 에러메세지는 덧붙여지지 않으며 버퍼의 크기가 사용자 에러메세지보다 작다면
 * 잘려진 사용자 에러메세지만 복사된다. IErrorInfo 인터페이스를 얻어오는데
 * 실패했거나 IErrorInfo로부터 메세지를 얻어오는데 실패했을 경우 함수는 
 * FALSE를 반환하지만, 실패 이유는 버퍼에 복사된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] pIErrorInfo   : COM 에러 메세지를 얻어오는데 사용할 IErrorInfo. NULL일 경우
 *                      GetErrorInfo를 사용하여 얻어옴.
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 COM 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 *
 * return             : 성공적으로 IErrorInfo로부터 에러메세지를 얻어왔는지 여부.
 */
static BOOL
GetCOMErrMsgA_Impl (LPSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo, LPCSTR pszErrDesc)
{
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	CComBSTR			bstrErrMsg ;
	LPSTR				szErrMsg = NULL ;
	BOOL				bReturn = FALSE ;

	// pIErrorInfo 가 NULL일 경우 구해오기
	if ( pIErrorInfo == NULL ) {
		HRESULT				hResult ;
		hResult = ::GetErrorInfo (NULL, &pIErrorInfo) ;

		if ( FAILED (hResult) )
			szErrMsg = "Failed to get a IErrorInfo interface." ;

		if ( hResult == S_FALSE )
			szErrMsg = "Object does not support the ISupportErrorInfo interface." ;

	} else {
		pIErrorInfo->AddRef () ;
	}

	// COM 에러메세지 구하기
	if ( szErrMsg == NULL ) {
		if ( FAILED (pIErrorInfo->GetDescription (&bstrErrMsg)) )
			szErrMsg = "Failed to get a error message from IErrorInfo" ;

		// 인터페이스 해제
		pIErrorInfo->Release () ;
	}

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		// 여태까지 에러가 없었다면
		if ( szErrMsg == NULL )
			return TRUE ;
		return bReturn ;
	}

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// 사용자 메세지 복사
	if ( (pszErrDesc != NULL) && (::strlen (pszErrDesc) > 0) ) {
		::_mbsnbcpy (
			(unsigned char *) (pszBuff + cchUsedBuff)
			, (unsigned char *) (pszErrDesc)
			, cchBuff - cchUsedBuff - 1) ;
		pszBuff[cchBuff - 1] = NULL ;
		cchUsedBuff = ::strlen (pszBuff) ;
	}

	// 모든 버퍼를 전부 사용했다면 종료
	if ( cchUsedBuff >= cchBuff - 1 ) {
		// 여태까지 에러가 없었다면
		if ( szErrMsg == NULL )
			return TRUE ;
		return bReturn ;
	}

	LPSTR				szUsingBuff = NULL ;
	BOOL				bFreeUsingBuff = FALSE ;

	if ( szErrMsg == NULL && bstrErrMsg.m_str != NULL ) {
		int			cbNeeded ;

		// 필요한 메모리 양 구하기
		if ( 0 == (cbNeeded = ::WideCharToMultiByte (
				CP_ACP
				, 0
				, static_cast <LPWSTR> (bstrErrMsg.m_str)
				, -1
				, NULL
				, 0
				, NULL
				, NULL)) )
			szErrMsg = "This COM error message is a invalid unicode string." ;

		if ( szErrMsg == NULL ) {
			if ( ((cchBuff - cchUsedBuff) * sizeof (CHAR)) >= static_cast<DWORD> (cbNeeded) )
				szUsingBuff = pszBuff + cchUsedBuff ;
			else {
				// 메모리 할당
				szUsingBuff = (LPSTR) ::malloc (cbNeeded) ;
				if ( szUsingBuff == NULL )
					szErrMsg = "Failed to allocate memory" ;
				else
					bFreeUsingBuff = TRUE ;
			}
		}

		if ( szErrMsg == NULL ) {
			// ANSI 포멧으로 변환
			if ( 0 == ::WideCharToMultiByte (
					CP_ACP
					, 0
					, static_cast <LPWSTR> (bstrErrMsg.m_str)
					, -1
					, szUsingBuff
					, cbNeeded
					, NULL
					, NULL) )
				szErrMsg = "Failed to convert a COM error message to ANSI" ;
			else {
				bReturn = TRUE ;
				szErrMsg = szUsingBuff ;
			}
		}
	}

	// 에러메세지 복사
	if ( szErrMsg ) {
		::_mbsnbcpy (
			(unsigned char *) (pszBuff + cchUsedBuff)
			, (unsigned char *) szErrMsg
			, cchBuff - cchUsedBuff - 1
		) ;
		pszBuff[cchBuff - 1] = NULL ;
	}

	if ( bFreeUsingBuff )
		::free (szUsingBuff) ;

	return bReturn ;
}

static BOOL
GetCOMErrMsgW_Impl (LPWSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo, LPCWSTR pszErrDesc)
{
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	CComBSTR			bstrErrMsg ;
	LPCWSTR				szErrMsg = NULL ;
	BOOL				bReturn = FALSE ;

	// pIErrorInfo 가 NULL일 경우 구해오기
	if ( pIErrorInfo == NULL ) {
		HRESULT				hResult ;
		hResult = ::GetErrorInfo (NULL, &pIErrorInfo) ;

		if ( FAILED (hResult) )
			szErrMsg = L"Failed to get a IErrorInfo interface." ;

		if ( hResult == S_FALSE )
			szErrMsg = L"Object does not support the ISupportErrorInfo interface." ;

	} else {
		pIErrorInfo->AddRef () ;
	}

	// COM 에러메세지 구하기
	if ( szErrMsg == NULL ) {
		if ( FAILED (pIErrorInfo->GetDescription (&bstrErrMsg)) )
			szErrMsg = L"Failed to get a error message from IErrorInfo" ;	
		else {
			szErrMsg = static_cast<LPWSTR> (bstrErrMsg.m_str) ;
			bReturn = TRUE ;
		}
		pIErrorInfo->Release () ;
	}

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		return bReturn ;
	}

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// 사용자 메세지 복사
	if ( (pszErrDesc != NULL) && (::wcslen (pszErrDesc) > 0) ) {
		::StringCbCopyW (
			pszBuff + cchUsedBuff
			, (cchBuff - cchUsedBuff) * sizeof (WCHAR)
			, pszErrDesc) ;
		cchUsedBuff = ::wcslen (pszBuff) ;
	}

	// 모든 버퍼를 전부 사용했다면 종료
	if ( cchUsedBuff >= cchBuff - 1 )
		return bReturn ;

	// 에러메세지 복사
	if ( szErrMsg )
		::StringCchCopyW (pszBuff + cchUsedBuff
				, cchBuff - cchUsedBuff
				, szErrMsg) ;

	return bReturn ;
}


/************************************************************************************
 * 사용자 에러메세지에 표준 COM 에러메세지를 덧붙인 메세지를 생성한다. 
 * 만약 버퍼의 크기가 모든 에러메세지를 담을 정도로 충분하지 않는다면 COM 
 * 에러메세지는 덧붙여지지 않으며 버퍼의 크기가 사용자 에러메세지보다 작다면
 * 잘려진 사용자 에러메세지만 복사된다. IErrorInfo 인터페이스를 얻어오는데
 * 실패했거나 IErrorInfo로부터 메세지를 얻어오는데 실패했을 경우 함수는 
 * FALSE를 반환하지만, 실패 이유는 버퍼에 복사된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] pIErrorInfo   : COM 에러 메세지를 얻어오는데 사용할 IErrorInfo. NULL일 경우
 *                      GetErrorInfo를 사용하여 얻어옴.
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 COM 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 *
 * return             : 성공적으로 IErrorInfo로부터 에러메세지를 얻어왔는지 여부.
 */
BOOL
GetCOMErrMsgA (LPSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo, LPCSTR pszErrDesc)
{
	return GetCOMErrMsgA_Impl (pszBuff, cchBuff, pIErrorInfo, pszErrDesc) ;
}

BOOL
GetCOMErrMsgW (LPWSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo, LPCWSTR pszErrDesc)
{
	return GetCOMErrMsgW_Impl (pszBuff, cchBuff, pIErrorInfo, pszErrDesc) ;
}


/************************************************************************************
 * 사용자 에러메세지에 마지막 표준 COM 에러메세지를 덧붙인 메세지를 생성한다. 
 * 만약 버퍼의 크기가 모든 에러메세지를 담을 정도로 충분하지 않는다면 COM 
 * 에러메세지는 덧붙여지지 않으며 버퍼의 크기가 사용자 에러메세지보다 작다면
 * 잘려진 사용자 에러메세지만 복사된다. IErrorInfo 인터페이스를 얻어오는데
 * 실패했거나 IErrorInfo로부터 메세지를 얻어오는데 실패했을 경우 함수는 
 * FALSE를 반환하지만, 실패 이유는 버퍼에 복사된다.
 * 
 * [in, out] pszBuff  : 에러메세지를 받아올 버퍼. (미리 할당되어 있어야 함)
 * [in] cchBuff       : pszBuff 의 크기 (NULL 포함 CHAR or WCHAR 단위)
 * [in] pszErrDesc    : 사용자 에러 메세지. 사용자 에러메세지 다음에 COM 에러메세지가
 *                      덧붙여짐. NULL 경우 사용안함.
 *
 * return             : 성공적으로 IErrorInfo로부터 에러메세지를 얻어왔는지 여부.
 */
BOOL
GetCOMErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc)
{
	return GetCOMErrMsgA_Impl (pszBuff, cchBuff, NULL, pszErrDesc) ;
}

BOOL
GetCOMErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc)
{
	return GetCOMErrMsgW_Impl (pszBuff, cchBuff, NULL, pszErrDesc) ;
}






static HRESULT
SetCOMErrInfo_Impl (GUID * pGUID, LPCOLESTR szSource, LPCOLESTR szDescription, LPCOLESTR szHelpFile, DWORD dwHelpContext)
{
	CComPtr<ICreateErrorInfo>	spCEI ;
	HRESULT						hResult ;

	hResult = ::CreateErrorInfo (&spCEI) ;
	if ( FAILED (hResult) )
		return hResult ;

	if ( pGUID && FAILED (hResult = spCEI->SetGUID (*pGUID)) )
		return hResult ;

	if ( szSource && FAILED (hResult = spCEI->SetSource (const_cast <LPOLESTR> (szSource))) )
		return hResult ;

	if ( szDescription && FAILED (hResult = spCEI->SetDescription (const_cast <LPOLESTR> (szDescription))) )
		return hResult ;

	if ( szHelpFile && FAILED (hResult = spCEI->SetHelpFile (const_cast <LPOLESTR> (szHelpFile))) )
		return hResult ;

	if ( FAILED (hResult = spCEI->SetHelpContext (dwHelpContext)) )
		return hResult ;

	CComPtr<IErrorInfo>			spEI ;
	if ( FAILED (hResult = spCEI->QueryInterface (IID_IErrorInfo, (void **) (&spEI))) )
		return hResult ;

	if ( FAILED (hResult = ::SetErrorInfo (0, spEI)) )
		return hResult ;

	return S_OK ;
}

HRESULT
SetCOMErrInfoA (GUID * pGUID, LPCSTR szSource, LPCSTR szDescription, LPCSTR szHelpFile, DWORD dwHelpContext)
{
	HANDLE			hProcHeap ;

	if ( NULL == (hProcHeap = ::GetProcessHeap ()) )
		return E_FAIL ;

	LPWSTR			wszSource = NULL ;
	LPWSTR			wszDescription = NULL ;
	LPWSTR			wszHelpFile = NULL ;
	HRESULT			hResult ;

	if ( szSource && 
			(FAILED (hResult = Ansi2Unicode (&wszSource, szSource, hProcHeap))) )
		return hResult ;

	if ( szDescription && 
		(FAILED (hResult = Ansi2Unicode (&wszDescription, szDescription, hProcHeap))) ) {
		::HeapFree (hProcHeap, 0, wszSource) ;
		return hResult ;
	}

	if ( szHelpFile && 
		(FAILED (hResult = Ansi2Unicode (&wszHelpFile, szHelpFile, hProcHeap))) ) {
		::HeapFree (hProcHeap, 0, wszSource) ;
		::HeapFree (hProcHeap, 0, wszDescription) ;
		return hResult ;
	}

	hResult = SetCOMErrInfo_Impl (pGUID, wszSource, wszDescription, wszHelpFile, dwHelpContext) ;
	::HeapFree (hProcHeap, 0, wszSource) ;
	::HeapFree (hProcHeap, 0, wszDescription) ;
	::HeapFree (hProcHeap, 0, wszHelpFile) ;

	return hResult ;
}

HRESULT
SetCOMErrInfoW (GUID * pGUID, LPCWSTR szSource, LPCWSTR szDescription, LPCWSTR wszHelpFile, DWORD dwHelpContext)
{
	return SetCOMErrInfo_Impl (pGUID, szSource, szDescription, wszHelpFile, dwHelpContext) ;
}



}
