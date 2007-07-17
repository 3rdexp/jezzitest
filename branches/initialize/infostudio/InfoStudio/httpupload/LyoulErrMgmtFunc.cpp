#include "stdafx.h"
#include "LyoulErrMgmtFunc.h"

// $LastChangedRevision: 7 $
// $LastChangedDate: 2004-05-03 01:23:34 +0900 (Mon, 03 May 2004) $

namespace Lyoul {


/************************************************************************************
 * ����� �����޼����� ǥ�� �ý��� �����޼����� ������ �޼����� �����Ѵ�. 
 * ���� ������ ũ�Ⱑ ��� �����޼����� ���� ������ ������� �ʴ´ٸ� �ý��� 
 * �����޼����� ���ٿ����� ������ (�߷��� �����ڵ尡 ���ٿ���) ������ ũ�Ⱑ 
 * ����� �����޼������� �۴ٸ� �߷��� ����� �����޼����� ����ȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] dwErrCode     : �ý��� ���� �ڵ�. bUseLastErr �� TRUE �� ��� ���õ�.
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ �ý��� �����޼�����
 *                      ���ٿ���. NULL ��� ������.
 * [in] bUseLastErr   : TRUE �� ��� dwErrCode ��� GetLastError () �� ����� �����ڵ带
 *                      �����. FALSE �� ��� dwErrCode �� ���.
 * [in] pszModuleName : �޼��� ���̺��� ���� ��� �̸�. NULL �� ��� ������
 */
static void
GetSystemErrMsgA_Impl (LPSTR pszBuff, DWORD cchBuff, DWORD dwErrCode
							, LPCSTR pszErrDesc, BOOL bUseLastErr, HMODULE hModule)
{
	// �Ķ���� �˻�
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		return ;
	}
	
	if ( bUseLastErr )
		dwErrCode = ::GetLastError () ;

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// ����� �޼��� ����
	if ( (pszErrDesc != NULL) && (::strlen (pszErrDesc) > 0) ) {
		::_mbsnbcpy (
			(unsigned char *) (pszBuff + cchUsedBuff)
			, (unsigned char *) (pszErrDesc)
			, cchBuff - cchUsedBuff - 1) ;
		pszBuff[cchBuff - 1] = NULL ;
		cchUsedBuff = ::strlen (pszBuff) ;
	}

	// ��� ���۸� ���� ����ߴٸ� ����
	if ( cchUsedBuff >= cchBuff - 1 )
		return ;

	// �޼��� ���̺��� ����� ��� ��� ���ο� ����
	// FormatMessage �Ķ���� ����
	DWORD			dwFmtMsgFlag ;
	dwFmtMsgFlag = FORMAT_MESSAGE_FROM_SYSTEM ;

	if ( hModule )
		dwFmtMsgFlag |= FORMAT_MESSAGE_FROM_HMODULE ;

	// �ý��� �����޼��� ����
	// ���۰� �����޽����� ���� ������ ������� �ʴٸ� �ý��� 
	// �����޽����� ������� ����
	DWORD			dwWrittenChrs ;
	dwWrittenChrs = ::FormatMessageA (
		dwFmtMsgFlag
		, hModule						// lpSource
		, dwErrCode						// Error Code
		, 0								// dwLanguageID
		, pszBuff + cchUsedBuff			// lpBuffer
		, cchBuff - cchUsedBuff			// nSize
		, NULL) ;						// Arguments


	// �����޼����� ����� �� ���ٸ� (�������� ������)
	// ������ȣ ���
	// StringCbPrintf ��ü ������ ������
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
	// �Ķ���� �˻�
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		return ;
	}
	
	if ( bUseLastErr )
		dwErrCode = ::GetLastError () ;

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// ����� �޼��� ����
	if ( (pszErrDesc != NULL) && (::wcslen (pszErrDesc) > 0) ) {
		::StringCbCopyW (
			pszBuff + cchUsedBuff
			, (cchBuff - cchUsedBuff) * sizeof (WCHAR)
			, pszErrDesc) ;
		cchUsedBuff = ::wcslen (pszBuff) ;
	}

	// ��� ���۸� ���� ����ߴٸ� ����
	if ( cchUsedBuff >= cchBuff - 1 )
		return ;


	// �޼��� ���̺��� ����� ��� ��� ���ο� ����
	// FormatMessage �Ķ���� ����
	DWORD			dwFmtMsgFlag ;
	dwFmtMsgFlag = FORMAT_MESSAGE_FROM_SYSTEM ;

	if ( hModule )
		dwFmtMsgFlag |= FORMAT_MESSAGE_FROM_HMODULE ;

	// �ý��� �����޼��� ����
	// ���۰� �����޽����� ���� ������ ������� �ʴٸ� �ý��� 
	// �����޽����� ������� ����
	DWORD			dwWrittenChrs ;
	dwWrittenChrs = ::FormatMessageW (
		dwFmtMsgFlag
		, hModule						// lpSource
		, dwErrCode						// Error Code
		, 0								// dwLanguageID
		, pszBuff + cchUsedBuff			// lpBuffer
		, cchBuff - cchUsedBuff			// nSize
		, NULL) ;						// Arguments


	// �����޼����� ����� �� ���ٸ� (�������� ������)
	// ������ȣ ���
	// StringCbPrintf ��ü ������ ������
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
 * ����� �����޼����� ������ ���� �ڵ忡 �ش��ϴ� ǥ�� �ý��� �����޼����� ������
 * �޼����� �����Ѵ�. ���� ������ ũ�Ⱑ ��� �����޼����� ���� ������ ������� �ʴ´ٸ�
 * �ý��� �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ ����� �����޼������� �۴ٸ� 
 * �߷��� ����� �����޼����� ����ȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ �ý��� �����޼�����
 *                      ���ٿ���. NULL ��� ������.
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
 * ����� �����޼����� ���� �ڵ忡 �ش��ϴ� ǥ�� �ý��� �����޼����� ������ �޼����� 
 * �����Ѵ�. ���� ������ ũ�Ⱑ ��� �����޼����� ���� ������ ������� �ʴ´ٸ� �ý��� 
 * �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ ����� �����޼������� �۴ٸ� �߷���
 * ����� �����޼����� ����ȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] dwErrCode     : �ý��� ���� �ڵ�.
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ �ý��� �����޼�����
 *                      ���ٿ���. NULL ��� ������.
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
 * ����� �����޼����� ������ ���� �ڵ忡 �ش��ϴ� wininet.dll ��� �Ǵ� ǥ�� �ý�������
 * ������ �����޼����� ������ �޼����� �����Ѵ�. ���� ������ ũ�Ⱑ ��� �����޼�����
 * ���� ������ ������� �ʴ´ٸ� �ý��� �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ
 * ����� �����޼������� �۴ٸ� �߷��� ����� �����޼����� ����ȴ�.
 * wininet.dll ����� �ڵ��� �����µ� �������� ��� WinInet API�� ������ �����ڵ常
 * ǥ�õȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ �ý��� �����޼�����
 *                      ���ٿ���. NULL ��� ������.
 */
void
GetWinInetErrMsgA (LPSTR pszBuff, DWORD cchBuff, LPCSTR pszErrDesc)
{
	// ���� �ڵ� ĳ��
	DWORD		dwLastErrCode = ::GetLastError () ;
	GetSystemErrMsgA_Impl (pszBuff, cchBuff, dwLastErrCode, pszErrDesc, FALSE
		, ::GetModuleHandle (TEXT ("wininet.dll"))) ;
}

void
GetWinInetErrMsgW (LPWSTR pszBuff, DWORD cchBuff, LPCWSTR pszErrDesc)
{
	// ���� �ڵ� ĳ��
	DWORD		dwLastErrCode = ::GetLastError () ;
	GetSystemErrMsgW_Impl (pszBuff, cchBuff, dwLastErrCode, pszErrDesc, FALSE
		, ::GetModuleHandle (TEXT ("wininet.dll"))) ;
}

/************************************************************************************
 * ����� �����޼����� ���� �ڵ忡 �ش��ϴ� wininet.dll ��� �Ǵ� ǥ�� �ý�������
 * ������ �����޼����� ������ �޼����� �����Ѵ�. ���� ������ ũ�Ⱑ ��� �����޼�����
 * ���� ������ ������� �ʴ´ٸ� �ý��� �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ
 * ����� �����޼������� �۴ٸ� �߷��� ����� �����޼����� ����ȴ�.
 * wininet.dll ����� �ڵ��� �����µ� �������� ��� WinInet API�� ������ �����ڵ常
 * ǥ�õȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] dwErrCode     : �ý��� ���� �ڵ�.
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ �ý��� �����޼�����
 *                      ���ٿ���. NULL ��� ������.
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
 * ����� �����޼����� ǥ�� COM �����޼����� ������ �޼����� �����Ѵ�. 
 * ���� ������ ũ�Ⱑ ��� �����޼����� ���� ������ ������� �ʴ´ٸ� COM 
 * �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ ����� �����޼������� �۴ٸ�
 * �߷��� ����� �����޼����� ����ȴ�. IErrorInfo �������̽��� �����µ�
 * �����߰ų� IErrorInfo�κ��� �޼����� �����µ� �������� ��� �Լ��� 
 * FALSE�� ��ȯ������, ���� ������ ���ۿ� ����ȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] pIErrorInfo   : COM ���� �޼����� �����µ� ����� IErrorInfo. NULL�� ���
 *                      GetErrorInfo�� ����Ͽ� ����.
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ COM �����޼�����
 *                      ���ٿ���. NULL ��� ������.
 *
 * return             : ���������� IErrorInfo�κ��� �����޼����� ���Դ��� ����.
 */
static BOOL
GetCOMErrMsgA_Impl (LPSTR pszBuff, DWORD cchBuff, IErrorInfo * pIErrorInfo, LPCSTR pszErrDesc)
{
	_ASSERTE ( !(pszBuff == NULL || cchBuff == 0) ) ;

	CComBSTR			bstrErrMsg ;
	LPSTR				szErrMsg = NULL ;
	BOOL				bReturn = FALSE ;

	// pIErrorInfo �� NULL�� ��� ���ؿ���
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

	// COM �����޼��� ���ϱ�
	if ( szErrMsg == NULL ) {
		if ( FAILED (pIErrorInfo->GetDescription (&bstrErrMsg)) )
			szErrMsg = "Failed to get a error message from IErrorInfo" ;

		// �������̽� ����
		pIErrorInfo->Release () ;
	}

	if ( cchBuff == 1 ) {
		pszBuff[0] = NULL ;
		// ���±��� ������ �����ٸ�
		if ( szErrMsg == NULL )
			return TRUE ;
		return bReturn ;
	}

	size_t		cchUsedBuff = 0 ;
	pszBuff[0] = NULL ;

	// ����� �޼��� ����
	if ( (pszErrDesc != NULL) && (::strlen (pszErrDesc) > 0) ) {
		::_mbsnbcpy (
			(unsigned char *) (pszBuff + cchUsedBuff)
			, (unsigned char *) (pszErrDesc)
			, cchBuff - cchUsedBuff - 1) ;
		pszBuff[cchBuff - 1] = NULL ;
		cchUsedBuff = ::strlen (pszBuff) ;
	}

	// ��� ���۸� ���� ����ߴٸ� ����
	if ( cchUsedBuff >= cchBuff - 1 ) {
		// ���±��� ������ �����ٸ�
		if ( szErrMsg == NULL )
			return TRUE ;
		return bReturn ;
	}

	LPSTR				szUsingBuff = NULL ;
	BOOL				bFreeUsingBuff = FALSE ;

	if ( szErrMsg == NULL && bstrErrMsg.m_str != NULL ) {
		int			cbNeeded ;

		// �ʿ��� �޸� �� ���ϱ�
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
				// �޸� �Ҵ�
				szUsingBuff = (LPSTR) ::malloc (cbNeeded) ;
				if ( szUsingBuff == NULL )
					szErrMsg = "Failed to allocate memory" ;
				else
					bFreeUsingBuff = TRUE ;
			}
		}

		if ( szErrMsg == NULL ) {
			// ANSI �������� ��ȯ
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

	// �����޼��� ����
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

	// pIErrorInfo �� NULL�� ��� ���ؿ���
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

	// COM �����޼��� ���ϱ�
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

	// ����� �޼��� ����
	if ( (pszErrDesc != NULL) && (::wcslen (pszErrDesc) > 0) ) {
		::StringCbCopyW (
			pszBuff + cchUsedBuff
			, (cchBuff - cchUsedBuff) * sizeof (WCHAR)
			, pszErrDesc) ;
		cchUsedBuff = ::wcslen (pszBuff) ;
	}

	// ��� ���۸� ���� ����ߴٸ� ����
	if ( cchUsedBuff >= cchBuff - 1 )
		return bReturn ;

	// �����޼��� ����
	if ( szErrMsg )
		::StringCchCopyW (pszBuff + cchUsedBuff
				, cchBuff - cchUsedBuff
				, szErrMsg) ;

	return bReturn ;
}


/************************************************************************************
 * ����� �����޼����� ǥ�� COM �����޼����� ������ �޼����� �����Ѵ�. 
 * ���� ������ ũ�Ⱑ ��� �����޼����� ���� ������ ������� �ʴ´ٸ� COM 
 * �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ ����� �����޼������� �۴ٸ�
 * �߷��� ����� �����޼����� ����ȴ�. IErrorInfo �������̽��� �����µ�
 * �����߰ų� IErrorInfo�κ��� �޼����� �����µ� �������� ��� �Լ��� 
 * FALSE�� ��ȯ������, ���� ������ ���ۿ� ����ȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] pIErrorInfo   : COM ���� �޼����� �����µ� ����� IErrorInfo. NULL�� ���
 *                      GetErrorInfo�� ����Ͽ� ����.
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ COM �����޼�����
 *                      ���ٿ���. NULL ��� ������.
 *
 * return             : ���������� IErrorInfo�κ��� �����޼����� ���Դ��� ����.
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
 * ����� �����޼����� ������ ǥ�� COM �����޼����� ������ �޼����� �����Ѵ�. 
 * ���� ������ ũ�Ⱑ ��� �����޼����� ���� ������ ������� �ʴ´ٸ� COM 
 * �����޼����� ���ٿ����� ������ ������ ũ�Ⱑ ����� �����޼������� �۴ٸ�
 * �߷��� ����� �����޼����� ����ȴ�. IErrorInfo �������̽��� �����µ�
 * �����߰ų� IErrorInfo�κ��� �޼����� �����µ� �������� ��� �Լ��� 
 * FALSE�� ��ȯ������, ���� ������ ���ۿ� ����ȴ�.
 * 
 * [in, out] pszBuff  : �����޼����� �޾ƿ� ����. (�̸� �Ҵ�Ǿ� �־�� ��)
 * [in] cchBuff       : pszBuff �� ũ�� (NULL ���� CHAR or WCHAR ����)
 * [in] pszErrDesc    : ����� ���� �޼���. ����� �����޼��� ������ COM �����޼�����
 *                      ���ٿ���. NULL ��� ������.
 *
 * return             : ���������� IErrorInfo�κ��� �����޼����� ���Դ��� ����.
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
