#pragma once
#include "wclassdefines.h"
#include "../controls/controlbar.h"
#include "../controls/toolbar.h"
//#include "../controls/skindialog.h"
#include "../controls/frame.h"
#include "../controls/menu.h"
#include "../controls/Datetimepick.h"
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

	static void RefreshUI()
	{
		GetInstance().UpdateUI();
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


	
	CSkinHookBase* GetSkinCtrl( HWND hWnd )
	{
		CSkinHookBase* pSkin = NULL;

		SKINHOOK_ITERATOR it = _handle_maps.find( hWnd );
		if( it != _handle_maps.end() )
		{
			pSkin = it->second;
			return pSkin;
		}

		return NULL;
	}

	/*
	CSkinHookBase* NewSkinCtrl( const CString& sClass, HWND hWnd )
	{
		if ( sClass.Find(WC_CONTROLBAR) == 0 )
			return new SkinControlBar();
		else if ( sClass.CompareNoCase(WC_TOOLBAR) == 0 )
		{
			return new SkinToolBarCtrl();
		}
		//else if ( sClass.CompareNoCase(WC_DIALOGBOX) == 0 )
		//{
		//	return new SkinHookDialog();
		//}
		//else if ( sClass.CompareNoCase(WC_SCROLLBAR) == 0 )
		//{
		//	return new SkinScrollBar();
		//}
		return NULL;//new CSkinOther(0);
	}
	*/
	
	BOOL InitSkin( HWND hWnd )
	{
		CString sClass( GetClass( hWnd ) ); // converts Afx class names into something more useful
	/*
		if ( sClass.CompareNoCase(WC_TOOLBAR) == 0 )
		{
			CSkinHookBase* pSkinCtrl = GetSkinCtrl( hWnd );
			if ( pSkinCtrl )
				return TRUE;
			SkinToolBarCtrl* pSkinToolBar = new SkinToolBarCtrl();
			if ( pSkinToolBar )
			{
				pSkinToolBar->Install( hWnd );
				_handle_maps.insert( std::make_pair(hWnd, pSkinToolBar) );	
				return TRUE;
			}
		} 
		else
		if( sClass.Find(WC_CONTROLBAR) == 0 )
		{
			SkinControlBar<ATL::CWindow>::InstallHook( hWnd );
		}
		*/
		
		return TRUE;
		/*

		// do what

		CString sClass( GetClass( hWnd ) );
		CSkinHookBase* pSkinCtrl = GetSkinCtrl( hWnd );
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
			pSkinCtrl->Install( hWnd );
			_handle_maps.insert( std::make_pair(hWnd, pSkinCtrl) );	
			return TRUE;
		}

		// else
		delete pSkinCtrl;

		return TRUE;
		*/
	}

	
	LRESULT  UnInstallSkin( HWND hWnd, const MSG& msg)
	{
		SKINHOOK_ITERATOR it = _handle_maps.find( hWnd );
		if( it != _handle_maps.end() )
		{

			//SkinHookBase* pCtrl = it->second;
			_handle_maps.erase(it);
			//pCtrl->UninstallHook( hWnd );
			//WNDPROC defaultproc = pCtrl->getDefaultProc();
			//pCtrl->Detach();
			//delete pCtrl;

			//return CallWindowProc(defaultproc, hWnd, msg.message, msg.wParam, msg.lParam);

		}
		// else
		return 0;
	}
	
	void AddhWnd(HWND hWnd )
	{
		SKINHWND_ITERATOR it = _hwnd_maps.find( hWnd );
		if( it == _hwnd_maps.end() )
		{
			_hwnd_maps.insert( std::make_pair(hWnd, hWnd) );	
		}
	}

	void RemovehWnd( HWND hWnd )
	{
		SKINHWND_ITERATOR it = _hwnd_maps.find( hWnd );
		if( it != _hwnd_maps.end() )
		{
			_hwnd_maps.erase(it);
		}
	}

	void UpdateUI()
	{
		SkinSDIFrame::UpdateWindowRgn();
		SkinMDIFrame::UpdateWindowRgn();
		SkinFrame::UpdateWindowRgn();
		SkinDialog::UpdateWindowRgn();

		SKINHWND_ITERATOR it = _hwnd_maps.begin();
		for ( it = _hwnd_maps.begin(); it != _hwnd_maps.end(); it++)
		{
			//需要把Dialog, Frame都处理一下
			::SendMessage ( it->first, WM_NCACTIVATE, 1, 0 );
			//::InvalidateRect( it->first, NULL, TRUE);
			RedrawWindow(it->first, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
		}
	}

	// global app hooks
	// WH_CALLWNDPROC
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
#ifdef _USRDLL
		// If this is a DLL, need to set up MFC state
		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

		if (nCode == HC_ACTION)
		{
			CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;

			MSG msg = { pwp->hwnd, pwp->message, pwp->wParam, pwp->lParam, 0, { 0, 0 } };
			LRESULT lReturn = 0;
			BOOL bRet = GetInstance().OnCallWndProc(msg, lReturn);
			if ( bRet )
			{
				//CallNextHookEx(GetInstance()._hCallWndHook, nCode, wParam, lParam);
				return lReturn;
			}
		}

		return CallNextHookEx(GetInstance()._hCallWndHook, nCode, wParam, lParam);
	}

	bool IgnoreThisMenu(HMENU hMenu)
	{
		// if we receive a menu which has separators which have a non-zero value, then its
		// a menu we should not make ownerdrawm
		// determine whether this is such a menu
		bool ignoreMenu = false;

		int itemCount = ::GetMenuItemCount(hMenu);
		for (int item = 0; item < itemCount && !ignoreMenu; item++)
		{
			MENUITEMINFO    itemInfo;    
			memset(&itemInfo, 0, sizeof(MENUITEMINFO));
			itemInfo.cbSize = sizeof(MENUITEMINFO);

			itemInfo.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
			::GetMenuItemInfo(hMenu, item, TRUE, &itemInfo);

			int itemID = itemInfo.wID;

			if ((itemInfo.fType & MFT_SEPARATOR) != 0 && itemID != 0)
			{
				// this is a menu type we need to ignore
				ignoreMenu = true;
			}
			if (itemInfo.hSubMenu != NULL)
			{
				// do a recursive call on this popup menu
				ignoreMenu = IgnoreThisMenu(itemInfo.hSubMenu);
			}
		}
		return ignoreMenu;
	}

	BOOL OnCallWndProc(const MSG& msg, LRESULT& lReturn)
	{   
		switch (msg.message)
		{
		case WM_NOTIFY:
			{
				//搞这个消息是为了CDateTimePickerCtrl 所以就判断这个就可以了
				NMHDR* pNMHDR = (NMHDR*)msg.lParam;
				CString strClass = GetClass( pNMHDR->hwndFrom );
				if ( strClass.CompareNoCase( WC_DATETIMEPICK ) == 0 )
				{
					
					if (pNMHDR->code != NM_CUSTOMDRAW)
					{
						//SkinDateTimePickerCtrl<WTL::CDateTimePickerCtrl>* pSkin = NULL;

						SkinDateTimePickerCtrl<WTL::CDateTimePickerCtrl> skindata;
						LRESULT lRet;
						skindata.NotifyReflect( pNMHDR->hwndFrom, pNMHDR, lRet );
						
					}
				}

				/*
				if ( ((LPNMHDR)msg.lParam)->code == NM_COOLSB_CUSTOMDRAW )
				{
					lReturn =  HandleCustomDraw(msg.wParam, (NMCSBCUSTOMDRAW *)msg.lParam);
					return TRUE;
				}
				*/
				

				break;
			}
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
		
		case WM_CREATE:
			{
				AddhWnd(msg.hwnd);
			}
			break;
		case WM_NCDESTROY:
			{
				UnInstallSkin(msg.hwnd, msg);
				RemovehWnd( msg.hwnd );
				//return TRUE;
				break;
			}
			break;
		case WM_NCPAINT:
			{
				//TRACE("hook WM_NCPAINT  %p %d\n", msg.hwnd, msg.message);
				break;
			}
		case WMS_SETSTYLE: //修改style
			{
				CSkinHookBase* pSkinCtrl = GetSkinCtrl( msg.hwnd );
				if ( pSkinCtrl )
				{
					if ( pSkinCtrl->_classid != msg.wParam )
					{
						pSkinCtrl->_classid == msg.wParam;
						::InvalidateRect( msg.hwnd, NULL, TRUE );
					}

				}
			}
		
		}
		return FALSE;
	}

public:
	HHOOK _hCallWndHook;
	std::map< HWND , CSkinHookBase* > _handle_maps;
	typedef std::map<HWND, CSkinHookBase* >::iterator SKINHOOK_ITERATOR;

	std::map< HWND , HWND > _hwnd_maps;
	typedef std::map<HWND, HWND >::iterator SKINHWND_ITERATOR;

};

};// namespace Skin