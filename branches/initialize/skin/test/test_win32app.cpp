// blank_win32app.cpp : 定义应用程序的入口点。
//

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlwin.h>

#pragma comment(lib, "comctl32.lib")

#include "resource.h"
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND hWndMain;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	CoInitialize(0);
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BLANK_WIN32APP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_BLANK_WIN32APP);

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CoUninitialize();

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释: 
//
//    仅当希望在已添加到 Windows 95 的
//    “RegisterClassEx”函数之前此代码与 Win32 系统兼容时，
//    才需要此函数及其用法。调用此函数
//    十分重要，这样应用程序就可以获得关联的
//   “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_BLANK_WIN32APP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_BLANK_WIN32APP;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HANDLE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 320, 240, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hWndMain = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   INITCOMMONCONTROLSEX cs = {sizeof INITCOMMONCONTROLSEX, ICC_TAB_CLASSES};
   InitCommonControlsEx(&cs);

   return TRUE;
}


//////////////////////////////////////////////////////////////////////////
HMODULE hLibSkin = 0;

#include "../src/skinitf.h"
using namespace Skin;

typedef HRESULT (WINAPI *fGetSkinMgr)(ISkinMgr **);
//////////////////////////////////////////////////////////////////////////


//
//  函数: WndProc(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_ASSERTE( _CrtCheckMemory( ) );
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_SKIN_INSTALL:
			{
				if(!hLibSkin)
				    hLibSkin = LoadLibrary("Skin.dll");

				fGetSkinMgr pfGetSkinMgr = 0;

				if(hLibSkin)
				{
					pfGetSkinMgr = (fGetSkinMgr)GetProcAddress(hLibSkin, "GetSkinMgr");
				}

				if(pfGetSkinMgr)
				{
					ISkinMgr* pm = 0;
					pfGetSkinMgr(&pm);

					if(pm)
					{
						pm->InitControls(hInst, SKINCTL_BUTTON);
						pm->Release();
					}
				}
			}
			break;
		case ID_SKIN_UNINSTALL:
			if(0){
				if(hLibSkin)
				{
					fGetSkinMgr pfGetSkinMgr = (fGetSkinMgr)GetProcAddress(hLibSkin, "GetSkinMgr");
					if(pfGetSkinMgr)
					{
						ISkinMgr* pm = 0;
						pfGetSkinMgr(&pm);

						if(pm)
						{
							pm->UninitControls(hInst, SKINCTL_ALL);
							pm->Release();
						}
					}

					FreeLibrary(hLibSkin);
					hLibSkin = 0;
				}
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	_ASSERTE( _CrtCheckMemory( ) );
	return 0;
}

// “关于”框的消息处理程序。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	_ASSERTE( _CrtCheckMemory( ) );
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			if(hLibSkin)
			{
				fGetSkinMgr pfGetSkinMgr = (fGetSkinMgr)GetProcAddress(hLibSkin, "GetSkinMgr");
				if(pfGetSkinMgr)
				{
					ISkinMgr* pm = 0;
					pfGetSkinMgr(&pm);

					if(pm)
					{
						pm->UninitControls(hInst, SKINCTL_ALL);
						pm->Release();
					}
				}

				FreeLibrary(hLibSkin);
				hLibSkin = 0;
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			RECT rc;

			CWindow m_wndTab(GetDlgItem(hDlg, IDC_TAB1));

			// Get tab control dimensions
			m_wndTab.GetClientRect(&rc);

			// Get the tab control DC
			HDC hDC = m_wndTab.GetDC();

			// Create a compatible DC
			HDC hDCMem = ::CreateCompatibleDC(hDC);
			HBITMAP hBmp = ::CreateCompatibleBitmap(hDC, 
				rc.right - rc.left, rc.bottom - rc.top);
			HBITMAP hBmpOld = (HBITMAP)(::SelectObject(hDCMem, hBmp));

			// Tell the tab control to paint in our DC
			m_wndTab.SendMessage(WM_PRINTCLIENT, (WPARAM)(hDCMem), 
				(LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));

			// Create a pattern brush from the bitmap selected in our DC
			HBRUSH m_hBrush = ::CreatePatternBrush(hBmp);

			// Restore the bitmap
			::SelectObject(hDCMem, hBmpOld);

			// Cleanup
			::DeleteObject(hBmp);
			::DeleteDC(hDCMem);
			m_wndTab.ReleaseDC(hDC);


			HDC h = GetDC(0);
			FillRect(h, &rc, m_hBrush);
			ReleaseDC(0, h);

#if 0



			HDC h = GetDC(0);

			HWND hWnd = hDlg; // GetDlgItem(hDlg, IDC_CHECK1);
			RECT rcparent;
			GetClientRect(hWnd, &rcparent);
			rcparent.right += 200;
			rcparent.bottom += 200;

			HDC dcMem = ::CreateCompatibleDC(h);
			HBITMAP bmpMemBg = ::CreateCompatibleBitmap(h, rcparent.right - rcparent.left, rcparent.bottom - rcparent.top);
			HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);

			SendMessage(hWnd, WM_PRINTCLIENT, (WPARAM)dcMem, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT );

			HBRUSH m_hBrush = ::CreatePatternBrush(bmpMemBg);

			
			BitBlt(h, 10, 10, rcparent.right - rcparent.left, rcparent.bottom - rcparent.top, dcMem, 0, 0, SRCCOPY);
			// FillRect(h, &rcparent, m_hBrush);

			ReleaseDC(0, h);

			SelectObject(dcMem, pOldBmp);
			DeleteObject(bmpMemBg);
			DeleteDC(dcMem);
#endif
		}
		break;
	}
	_ASSERTE( _CrtCheckMemory( ) );
	return FALSE;
}
