/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 07.07.2000 18:21:26
 Version: 1.0.0

*/

#ifndef _Common_e438bc1b_8363_444d_b569_4cacac3129b7
#define _Common_e438bc1b_8363_444d_b569_4cacac3129b7

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

struct IMainFrame
{
	virtual HWND GetWindow() = 0;
	virtual CProgressBarCtrl* GetProgressBar() = 0;
	virtual void Send_RefreshStatusBar(const TCHAR*, ...) = 0;
	virtual bool DoFileOpen (const TCHAR* sPath, bool bUpdateRecent = true) = 0;
};

struct _Context 
{
	IMainFrame *m_frame;
};

extern __declspec( thread ) _Context g_Context;

inline void  debug_print (char* pStr, ...)
{
	char buffer[512] = {0};
	
	va_list args;	
	va_start(args, pStr);

	wvsprintfA(buffer, pStr, args);
	OutputDebugStringA(buffer);

	va_end(args);
}
inline void  debug_print (wchar_t* pStr, ...)
{
	wchar_t buffer[512] = {0};
	
	va_list args;	
	va_start(args, pStr);

	wvsprintfW(buffer, pStr, args);
	OutputDebugStringW(buffer);

	va_end(args);
}

#endif //_Common_e438bc1b_8363_444d_b569_4cacac3129b7

