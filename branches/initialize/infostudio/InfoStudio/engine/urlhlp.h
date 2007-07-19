#pragma once

#include <wininet.h>

// 可能占用内存多了一些，不要保存之, sizeof = 4736
class UrlComponet : public URL_COMPONENTS
{
public:
	UrlComponet()
	{
		ZeroMemory(this, sizeof URL_COMPONENTS);
		dwStructSize = sizeof URL_COMPONENTS;

		dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
		dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
		dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
		dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
		dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;
		dwExtraInfoLength = INTERNET_MAX_PATH_LENGTH; // 0
		lpszScheme = _szScheme;
		lpszHostName = _szHostName;
		lpszUserName = _szUserName;
		lpszPassword = _szPassword;
		lpszUrlPath = _szPath;
		lpszExtraInfo = _szExtraInfo;
	}
	BOOL Crack(LPCTSTR lpszUrl, DWORD dwUrlLength = 0, DWORD dwFlags = ICU_ESCAPE)
	{
		lstrcpyn(_szUrl, lpszUrl, sizeof _szUrl);
		BOOL f = InternetCrackUrl(lpszUrl, dwUrlLength, dwFlags, this);
		return f;
	}
	BOOL Create(INTERNET_SCHEME nScheme, LPCSTR lpszHostName, INTERNET_PORT nPort, 
		LPCSTR lpszUserName, LPCSTR lpszPassword,
		LPCSTR lpszUrlPath, LPCSTR lpszExtraInfo)
	{
		this->lpszHostName = (LPTSTR)lpszHostName;
		return 0;
	}
	LPCTSTR GetFileName() const
	{
		int n = lstrlen(_szPath);
		if(n>0)
		{
			LPCTSTR p = _szPath + (n - 1);
			while( *p != _T('\\') && *p != _T('/') )
				p --;
			return p + 1;
		}
		return 0; 
	}
private:
	TCHAR _szUrl[INTERNET_MAX_URL_LENGTH];
	TCHAR _szHostName[INTERNET_MAX_HOST_NAME_LENGTH];
	TCHAR _szUserName[INTERNET_MAX_USER_NAME_LENGTH];
	TCHAR _szPassword[INTERNET_MAX_PASSWORD_LENGTH];
	TCHAR _szPath[INTERNET_MAX_PATH_LENGTH];
	TCHAR _szScheme[INTERNET_MAX_SCHEME_LENGTH];
	TCHAR _szExtraInfo[INTERNET_MAX_PATH_LENGTH];
};