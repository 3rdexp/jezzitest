// blank_win32app.cpp : ����Ӧ�ó������ڵ㡣
//

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlwin.h>

#pragma comment(lib, "comctl32.lib")

#include "resource.h"
#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
HWND hWndMain;

// �˴���ģ���а����ĺ�����ǰ������:
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

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BLANK_WIN32APP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_BLANK_WIN32APP);

	// ����Ϣѭ��:
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��: 
//
//    ����ϣ��������ӵ� Windows 95 ��
//    ��RegisterClassEx������֮ǰ�˴����� Win32 ϵͳ����ʱ��
//    ����Ҫ�˺��������÷������ô˺���
//    ʮ����Ҫ������Ӧ�ó���Ϳ��Ի�ù�����
//   ����ʽ��ȷ�ġ�Сͼ�ꡣ
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
//   ����: InitInstance(HANDLE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, unsigned, WORD, LONG)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// �����˵�ѡ��:
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
		// TODO: �ڴ���������ͼ����...
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

// �����ڡ������Ϣ�������
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
