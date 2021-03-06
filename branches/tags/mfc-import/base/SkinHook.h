#pragma once
#include "wclassdefines.h"
#include "../controls/controlbar.h"

namespace Skin {

class CSkinHook
{
public:
	CSkinHook()
	{
		_hCallWndHook  = NULL;
	}

	~CSkinHook()
	{
		ReleaseHooks();
	}

	static CSkinHook& GetInstance()
	{
		static CSkinHook manager; 
		return manager; 
	}
	
	static BOOL Initialize()
	{
		return GetInstance().InitHooks();
	}

	BOOL InitHooks()
	{
		_hCallWndHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, NULL, GetCurrentThreadId()); 
		return TRUE;
	}

	void ReleaseHooks()
	{
		if ( _hCallWndHook ) 
			UnhookWindowsHookEx( _hCallWndHook );
		_hCallWndHook  = NULL;
	}
	
	CString GetClass(HWND hWnd)
	{
		static char szWndClass[128] = "";

		if (hWnd)
			::GetClassName(hWnd, szWndClass, 127);

		return CString(szWndClass);
	}

	CString GetClassEx(HWND hWnd)
	{
		CString sClass = GetClass(hWnd);

		if (sClass.Find("Afx") == 0) // its an mfc framework base or derived class
		{
			// can do the check if pWnd is permanent else mfc will not yet
			// have hooked up
			CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

			if (pWnd)
			{
				// must do the check in order of most derived class first
				if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
					return WC_MFCVIEW;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
					return WC_MFCMDIFRAME;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
					return WC_MFCMDICHILD;

//				else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
//					return WC_MFCMINIDOCKFRAME;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
					return WC_MFCMINIFRAME;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) // this is the catch all for frame wnds
					return WC_MFCFRAME;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
					return WC_MFCSPLITTER;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
					return WC_MFCDIALOGBAR;

				else if (pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)))
					return WC_CONTROLBAR;

				else 
					return WC_MFCWND; // catch all for all window classes
			}
		}

		return sClass;
	}


	SkinHookBase* GetSkinCtrl( HWND hWnd )
	{
		SkinHookBase* pSkin = NULL;

		SKINHOOK_ITERATOR it = _handle_maps.find( hWnd );
		if( it != _handle_maps.end() )
		{
			pSkin = it->second;
			return pSkin;
		}

		return NULL;
	}

	SkinHookBase* NewSkinCtrl( const CString& sClass, HWND hWnd )
	{
		if ( sClass.Find(WC_CONTROLBAR) == 0 )
			return new SkinControlBar();
		else if ( sClass.CompareNoCase(WC_TOOLBAR) == 0 )
			return new SkinToolBarCtrl();

		return NULL;//new CSkinOther(0);
	}


	BOOL InitSkin( HWND hWnd )
	{
		// do what
		CString sClass( GetClass( hWnd ) );
		SkinHookBase* pSkinCtrl = GetSkinCtrl( hWnd );
		if ( pSkinCtrl )
			return TRUE;

		// finally do it ourselves
		if (!pSkinCtrl)
		{
			CString sClass( GetClassEx( hWnd ) ); // converts Afx class names into something more useful

			pSkinCtrl = NewSkinCtrl( sClass, hWnd );
		}

		if ( pSkinCtrl )
		{
			pSkinCtrl->Attach( hWnd );
			pSkinCtrl->InstallHook( hWnd );
			_handle_maps.insert( std::make_pair(hWnd, pSkinCtrl) );	
			return TRUE;
		}

		// else
		delete pSkinCtrl;

		return TRUE;
	}


	BOOL  UnInstallSkin( HWND hWnd )
	{
		SKINHOOK_ITERATOR it = _handle_maps.find( hWnd );
		if( it != _handle_maps.end() )
		{

			SkinHookBase* pCtrl = it->second;
			_handle_maps.erase(it);
			//pCtrl->UninstallHook( hWnd );
			pCtrl->Detach();
			delete pCtrl;
		}
		// else
		return FALSE;
	}

	// global app hooks
	// WH_CALLWNDPROC
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
#ifdef _USRDLL
		// If this is a DLL, need to set up MFC state
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

		if (nCode == HC_ACTION)
		{
			CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;

			MSG msg = { pwp->hwnd, pwp->message, pwp->wParam, pwp->lParam, 0, { 0, 0 } };
			GetInstance().OnCallWndProc(msg);
		}

		return CallNextHookEx(GetInstance()._hCallWndHook, nCode, wParam, lParam);
	}

	void OnCallWndProc(const MSG& msg)
	{   
		switch (msg.message)
		{
		case WM_STYLECHANGED:
			if (msg.wParam == GWL_STYLE)
			{
				LPSTYLESTRUCT lpSS = (LPSTYLESTRUCT)msg.lParam;

				if (lpSS->styleNew & WS_VISIBLE)
				{
					BOOL bRes = InitSkin(msg.hwnd);
				}
			}
			break;

		case WM_PARENTNOTIFY:
			{
				if (LOWORD(msg.wParam) == WM_CREATE)
				{
					BOOL bRes = InitSkin((HWND)msg.lParam);
				}
			}
			break;

		case WM_WINDOWPOSCHANGED: // alternative to WM_SHOWWINDOW
			if (msg.wParam)
			{
				WINDOWPOS* pWPos = (WINDOWPOS*)msg.lParam;

				if (pWPos->flags & SWP_SHOWWINDOW)
					BOOL bRes = InitSkin(msg.hwnd);
			}
			break;

		case WM_SHOWWINDOW:
			if (msg.wParam)
			{
				BOOL bRes = InitSkin(msg.hwnd);
			}
			break;
		case WM_NCDESTROY:
			{
				BOOL bRes = UnInstallSkin(msg.hwnd);
			}
			break;
		}
	}

public:
	HHOOK _hCallWndHook;
	std::map< HWND , SkinHookBase* > _handle_maps;
	typedef std::map<HWND, SkinHookBase* >::iterator SKINHOOK_ITERATOR;

};

};// namespace Skin