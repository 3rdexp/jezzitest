// kPad.cpp : main source file for kPad.exe
//

#include "stdafx.h"

#define SIDEBAR_WIDTH 21
#define SIDEBAR_FONT  "Comic Sans MS"

// font size array
const static int font_size[] = 
		{6, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <AtlCtrlx.h>
#include <atlprint.h>

#include "resource.h"
#include "AtlAux2.h"

#include "common.h"
#include "option.h"
#include "CmdBar.h"
#include "kFlatCombo.h"
#include "kFontCombo.h"
#include "kColorPickerButton.h"
#include "OptionDlg.h"
#include "kSheetView.h"
#include "kSheet.h"
#include "aboutdlg.h"
#include "MainFrm.h"

#include "skinitf.h"
#include "skinlib.h"
#include <tmschema.h>

#pragma comment(lib, "skin.lib")

CAppModule _Module;

__declspec( thread ) _Context g_Context = {0};

class CMessageLoop2 : public CMessageLoop
{
public:
	int Run()
	{
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bRet;

		for(;;)
		{
			while(!::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE) && bDoIdle)
			{
				if(!OnIdle(nIdleCount++))
					bDoIdle = FALSE;
			}

			bRet = ::GetMessage(&m_msg, NULL, 0, 0);

			if(bRet == -1)
			{
				ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;	// error, don't process
			}
			else if(!bRet)
			{
				ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
				break;		// WM_QUIT, exit message loop
			}

			if(!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			// ! change
			if(IsIdleMessage2(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return (int)m_msg.wParam;
	}

	static BOOL IsIdleMessage2(MSG* pMsg)
	{
		// These messages should NOT cause idle processing
		switch(pMsg->message)
		{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_PAINT:
		case 0x0118:	// WM_SYSTIMER (caret blink)
		// ! change
		case WM_TIMER:
			return FALSE;
		}
		return TRUE;
	}	
};

class CKPadThreadManager
{
public:
	// thread init param
	struct _RunData
	{
		LPTSTR lpstrCmdLine;
		int nCmdShow;
	};

	// thread proc
	static DWORD WINAPI RunThread(LPVOID lpData)
	{
		// !
		CMessageLoop2 theLoop;
		_Module.AddMessageLoop(&theLoop);

		_RunData* pData = (_RunData*)lpData;
		CMainFrame wndFrame;

		g_Context.m_frame = &wndFrame;

		RECT *prc = 0;
		RECT rc = {0, 0, 0, 0};		

		{
			CRegKey key;			
			if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, kPadRegKey))
			{						
				DWORD dwType = REG_BINARY;
				DWORD dwSize = sizeof RECT;
				if ((ERROR_SUCCESS == RegQueryValueEx(key, _T("WindowRect"), 0, &dwType, (BYTE *)&rc, &dwSize))
						&& (REG_BINARY == dwType) && (sizeof(RECT) == dwSize) && !IsRectEmpty(&rc)) 
					prc = &rc;
			}
		}
		
		if(wndFrame.CreateEx(0, prc) == NULL)
		{
			ATLTRACE(_T("Frame window creation failed!\n"));
			return 0;
		}

		///////////////////////
		
		if(pData->lpstrCmdLine[0] == 0)
		{		
		}
		else	// file name specified at the command line
		{
			// strip quotes (if any)
			TCHAR* pCmd = pData->lpstrCmdLine;
			if(pCmd[0] == '"')
			{
				pCmd++;
				for(int i = 0; i < lstrlen(pCmd); i++)
				{
					if(pCmd[i] == '"')
					{
						pCmd[i] = 0;
						break;
					}
				}
			}

			//	get full path and file name
			TCHAR sPath[MAX_PATH];
			LPTSTR sFile = NULL;
			::GetFullPathName(pCmd, MAX_PATH, sPath, &sFile);
			
			// open file
			wndFrame.DoFileOpen(sPath);	
		}

		///////////////////////

		wndFrame.ShowWindow(pData->nCmdShow);
		::SetForegroundWindow(wndFrame);	// Win95 needs this
		delete pData;

		int nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
		return nRet;
	}

	DWORD m_dwCount;
	HANDLE m_arrThreadHandles[MAXIMUM_WAIT_OBJECTS - 1];

	CKPadThreadManager() : m_dwCount(0)
	{ }

// Operations
	DWORD AddThread(LPTSTR lpstrCmdLine, int nCmdShow)
	{
		if(m_dwCount == (MAXIMUM_WAIT_OBJECTS - 1))
		{
			::MessageBox(NULL, _T("ERROR: Cannot create ANY MORE threads!!!"), _T("kPad"), MB_OK);
			return 0;
		}

		_RunData* pData = new _RunData;
		pData->lpstrCmdLine = lpstrCmdLine;
		pData->nCmdShow = nCmdShow;
		DWORD dwThreadID;
		HANDLE hThread = ::CreateThread(NULL, 0, RunThread, pData, 0, &dwThreadID);
		if(hThread == NULL)
		{
			::MessageBox(NULL, _T("ERROR: Cannot create thread!!!"), _T("kPad"), MB_OK);
			return 0;
		}

		m_arrThreadHandles[m_dwCount] = hThread;
		m_dwCount++;
		return dwThreadID;
	}

	void RemoveThread(DWORD dwIndex)
	{
		::CloseHandle(m_arrThreadHandles[dwIndex]);
		if(dwIndex != (m_dwCount - 1))
			m_arrThreadHandles[dwIndex] = m_arrThreadHandles[m_dwCount - 1];
		m_dwCount--;
	}

	int Run(LPTSTR lpstrCmdLine, int nCmdShow)
	{
		MSG msg;
		// force message queue to be created
		::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

		AddThread(lpstrCmdLine, nCmdShow);

		int nRet = m_dwCount;
		DWORD dwRet;
		while(m_dwCount > 0)
		{
			dwRet = ::MsgWaitForMultipleObjects(m_dwCount, m_arrThreadHandles, FALSE, INFINITE, QS_ALLINPUT);

			if(dwRet == 0xFFFFFFFF)
				::MessageBox(NULL, _T("ERROR: Wait for multiple objects failed!!!"), _T("kPad"), MB_OK);
			else if(dwRet >= WAIT_OBJECT_0 && dwRet <= (WAIT_OBJECT_0 + m_dwCount - 1))
				RemoveThread(dwRet - WAIT_OBJECT_0);
			else if(dwRet == (WAIT_OBJECT_0 + m_dwCount))
			{
				::GetMessage(&msg, NULL, 0, 0);
				if(msg.message == WM_USER)
					AddThread("", SW_SHOWNORMAL);
				else
					::MessageBeep((UINT)-1);
			}
			else
				::MessageBeep((UINT)-1);
		}

		return nRet;
	}
};

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	LoadSkinDll();

#if (_WIN32_IE >= 0x0300)
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(iccx);
	iccx.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES; 
	BOOL bRet = ::InitCommonControlsEx(&iccx);
	bRet;
	ATLASSERT(bRet);
#else
	::InitCommonControls();
#endif

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	HINSTANCE hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
	ATLASSERT(hInstRich != NULL);

	CKPadThreadManager mgr;
	int nRet = mgr.Run(lpstrCmdLine, nCmdShow);

	::FreeLibrary(hInstRich);

	UnloadSkinDll();

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
