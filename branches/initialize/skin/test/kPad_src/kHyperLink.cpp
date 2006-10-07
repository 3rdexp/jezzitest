/*
 Copyright (c) 1999, kSet Lab 
 Author: Konstantin Bukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 12/27/1999 2:22:55 PM
 Version: 1.0.0
*/

#include "stdafx.h"
#include "kHyperLink.h"

bool kHyperLink::Open (TCHAR* pLink, int how, HWND hWnd)
{  
	HCURSOR hCursor = SetCursor(LoadCursor(0, IDC_APPSTARTING));

	bool bSuccess = false;
	bSuccess = OpenUsingCom (pLink, how, hWnd);  
	if (!bSuccess)
		bSuccess = OpenUsingShellExecute (pLink, how, hWnd);
	SetCursor(hCursor);
	return bSuccess;
}

bool kHyperLink::OpenUsingCom (TCHAR* pLink, int how, HWND hWnd)
{  
	CComPtr<IUniformResourceLocator> pURL;	
	HRESULT hr = ::CoCreateInstance(CLSID_InternetShortcut, 0, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (void**)&pURL);
	if (SUCCEEDED(hr))
	{
		hr = pURL->SetURL (pLink, IURL_SETURL_FL_GUESS_PROTOCOL);
		if (SUCCEEDED(hr)) 
		{  
			URLINVOKECOMMANDINFO ivci;
			ivci.dwcbSize = sizeof (URLINVOKECOMMANDINFO);
			ivci.dwFlags = 0;//IURL_INVOKECOMMAND_FL_ALLOW_UI;
			ivci.hwndParent = hWnd;
			
			switch (how)
			{
				case OPEN: ivci.pcszVerb = _T ("open");  break;
				case EDIT: ivci.pcszVerb = _T ("edit");  break;
				case PRINT:ivci.pcszVerb = _T ("print"); break;
				default:  ATLASSERT (0);
			}
			hr = pURL->InvokeCommand (&ivci);
		}
   }	
	return SUCCEEDED(hr);
}

bool kHyperLink::OpenUsingShellExecute (TCHAR* pLink, int how, HWND hWnd)
{
	LPCTSTR mode = 0;
	switch (how)
	{
		case OPEN: mode = _T ("open");  break;
		case EDIT: mode = _T ("edit");  break;
		case PRINT:  mode = _T ("print");  break;
		default: ATLASSERT (false);      
	}

	SHELLEXECUTEINFO sei; memset(&sei, 0, sizeof SHELLEXECUTEINFO);
	sei.cbSize = sizeof SHELLEXECUTEINFO;
	sei.fMask = SEE_MASK_FLAG_NO_UI ;
	sei.hwnd = hWnd;
	sei.lpVerb = mode;
	sei.lpFile = pLink;
	sei.nShow = SW_SHOW;

	ShellExecuteEx(&sei);
	if ((int)sei.hInstApp > HINSTANCE_ERROR)
		return true;
	else
	{
		TCHAR pResult[MAX_PATH]; memset(pResult, 0, MAX_PATH * sizeof TCHAR);
		if ((int)FindExecutable(pLink, 0, pResult) <= HINSTANCE_ERROR)
			return false;
		
		return (int)ShellExecute(hWnd, mode, pResult, pLink, 0, SW_SHOW) > HINSTANCE_ERROR;
	}  
}

#ifdef _WININET_NEED_
bool kHyperLink::ParseURL(TCHAR* pURL,	URL_COMPONENTS& urlComponents, DWORD dwFlags)
{	
	TCHAR* pstrCanonicalizedURL = 0;
	TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	bool bRetVal; 
	bool bMustFree = false;

	DWORD dwCanonicalizeFlags = dwFlags & (ICU_NO_ENCODE | ICU_DECODE | ICU_NO_META | ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);
	DWORD dwCrackFlags = dwFlags & (ICU_ESCAPE | ICU_USERNAME);

	bRetVal = (InternetCanonicalizeUrl(pURL, szCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags) != FALSE);
	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)	
			return false;
			
		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		bMustFree = true;
		bRetVal = (InternetCanonicalizeUrl(pURL, pstrCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags) != FALSE);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return false;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;
	
	bRetVal = (InternetCrackUrl(pstrCanonicalizedURL, 0, dwCrackFlags, &urlComponents) != FALSE);
	if (bMustFree) delete [] pstrCanonicalizedURL;	
	return bRetVal;
}

bool kHyperLink::GetInternetScheme (TCHAR* pURL, INTERNET_SCHEME& nScheme, DWORD dwFlags)
{
	if (!pURL) return false;
	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
	bool bRetVal = ParseURL(pURL, urlComponents);
	nScheme = urlComponents.nScheme;	
	return bRetVal;
}
#endif
