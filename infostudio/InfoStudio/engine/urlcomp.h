#pragma once

#include <wininet.h>

// 可能占用内存多了一些，不要保存之, sizeof = 4736
class UrlComponet : public URL_COMPONENTSW
{
public:
	UrlComponet()
	{
		ZeroMemory(this, sizeof URL_COMPONENTSW);
		dwStructSize = sizeof URL_COMPONENTSW;

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
	BOOL Crack(LPCWSTR lpszUrl, DWORD dwUrlLength = 0, DWORD dwFlags = ICU_ESCAPE)
	{
		lstrcpynW(_szUrl, lpszUrl, sizeof _szUrl);
		BOOL f = InternetCrackUrlW(lpszUrl, dwUrlLength, dwFlags, this);
		return f;
	}
	BOOL Create(INTERNET_SCHEME nScheme, LPCWSTR lpszHostName, INTERNET_PORT nPort, 
		LPCWSTR lpszUserName, LPCWSTR lpszPassword,
		LPCWSTR lpszUrlPath, LPCWSTR lpszExtraInfo)
	{
		// this->lpszHostName = (LPTSTR)lpszHostName;
		return 0;
	}
	LPCWSTR GetFileName() const
	{
		int n = lstrlenW(_szPath);
		if (n > 0)
		{
			LPCWSTR p = _szPath + (n - 1);
			while ( *p != L'\\' && *p != L'/' )
				p --;
			return p + 1;
		}
		return 0; 
	}
private:
	wchar_t _szUrl[INTERNET_MAX_URL_LENGTH];
	wchar_t _szHostName[INTERNET_MAX_HOST_NAME_LENGTH];
	wchar_t _szUserName[INTERNET_MAX_USER_NAME_LENGTH];
	wchar_t _szPassword[INTERNET_MAX_PASSWORD_LENGTH];
	wchar_t _szPath[INTERNET_MAX_PATH_LENGTH];
	wchar_t _szScheme[INTERNET_MAX_SCHEME_LENGTH];
	wchar_t _szExtraInfo[INTERNET_MAX_PATH_LENGTH];
};
