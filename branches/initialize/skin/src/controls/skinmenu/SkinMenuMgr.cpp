// SkinMenuMgr.cpp: implementation of the CSkinMenuMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

using namespace ATL;

#include "SkinMenuMgr.h"
#include "skinmenu.h"
#include "../../base/wclassdefines.h"
#include "skinbase.h"
#include "winclasses.h"
//#include "../../base/SkinCtrl.h"
//#include "../../base/SkinMgr.h"
#include "../../skinitf.h"
#include "../../controls/controlbar.h"

#ifndef NO_SKIN_INI
#include "skininifile.h"
#endif




namespace Skin {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinMenuMgr::CSkinMenuMgr()
{
	m_dwMenuStyle = SKMS_SIDEBAR;
	m_nSidebarWidth = 10;
	m_pCurSkinMenu = NULL;
	m_hCurContextWnd = NULL;
	m_hCurMenu = NULL;
}

CSkinMenuMgr::~CSkinMenuMgr()
{
	// cleanup any remaining windows
	//ASSERT (!m_mapMenus.GetCount());

	MENUHWND_ITERATOR it = m_mapMenus.begin();

	while( it != m_mapMenus.end() )
	{
		ASSERT(it->first);
		CSkinMenu* pSkin = it->second; 
		if ( pSkin )
			delete pSkin;
		it = m_mapMenus.erase(it);
	}

	m_skGlobals.Reset();
}

/////////////////////////////////////////////////////////////////////////////////
// static methods start

BOOL CSkinMenuMgr::Initialize(DWORD dwMenuStyle, int nSBWidth, BOOL bNotXP)
{
	//if (bNotXP && CSkinBase::GetOS() >= SBOS_XP)
	//{
	//	return FALSE;
	//}

	// only initialize once
	VERIFY(GetInstance().InitHooks(HM_CALLWNDPROC | HM_CBT));

	GetInstance().m_dwMenuStyle = dwMenuStyle;
	GetInstance().m_nSidebarWidth = nSBWidth;

	// Set Color
	//InitColor();
	return TRUE;
}

BOOL CSkinMenuMgr::InitColor()
{
	return TRUE;
	/*
	CComPtr<ISkinScheme> scheme;
	CComPtr<ISkinMgr> mgr;
	if (S_OK == GetSkinMgr(&mgr) && mgr)
		mgr->GetCurentScheme(&scheme);

	if ( scheme )
	{
		COLORREF ret;
		BOOL f = scheme->GetColor(MENU, 0, 0, TMT_MENU, &ret);
		if ( f )
			SetColor(COLOR_MENU, ret);

		f = scheme->GetColor(MENU, 0, 0, TMT_MENUTEXT, &ret);
		if ( f )
			SetColor(COLOR_WINDOWTEXT, ret);

		f = scheme->GetColor(MENU, 0, 0, TMT_MENUHILIGHT, &ret);
		if ( f )
			SetColor(COLOR_HIGHLIGHTTEXT, ret);

		f = scheme->GetColor(MENU, 0, 0, TMT_DKSHADOW3D, &ret);
		if ( f )
			SetColor(COLOR_3DSHADOW, ret);
	}
	*/

	return TRUE;
}

#ifndef NO_SKIN_INI
void CSkinMenuMgr::UnloadSkin()
{
	GetInstance().m_skGlobals.UnloadSkin();
}

BOOL CSkinMenuMgr::LoadSkin(const CSkinIniGlobalsFile* pIniFile)
{
	if (!pIniFile)
		return FALSE;

	GetInstance().m_skGlobals.LoadSkin(pIniFile);

	return TRUE;
}
#endif

void CSkinMenuMgr::SetColor(int nColor, COLORREF color)
{
	int nOS = CSkinBase::GetOS();

	if (nOS == SBOS_95 || nOS == SBOS_NT4)
		return;

	GetInstance().m_skGlobals.SetColor(nColor, color);
}

COLORREF CSkinMenuMgr::GetColor(int nColor)
{
	return GetInstance().m_skGlobals.GetColor(nColor);
}

void CSkinMenuMgr::ClearColors()
{
	GetInstance().m_skGlobals.Reset();
}

// static methods end
///////////////////////////////////////////////////////////////////////////////////

//#define NO_SKINNING
//把WC_CONTROLBAR和WC_MENU都处理吧,还有就是把换肤的处理加上

void CSkinMenuMgr::OnCallWndProc(const MSG& msg)
{   
	static BOOL bSkinning = FALSE; // initialized first time only but persistent

	if (!bSkinning)
	{
		bSkinning = TRUE;

#ifndef NO_SKINNING

		// skin/unskin menus at each and every opportunity 
		switch (msg.message)
		{
		case WM_CREATE:

			AddhWnd(msg.hwnd);
			
			if (CSkinMenu::IsMenuWnd(msg.hwnd))
			{
				BOOL bRes = Skin(msg.hwnd);
			}

			break;
			
		case WM_WINDOWPOSCHANGING: 
			if (CSkinMenu::IsMenuWnd(msg.hwnd)  )
			{
				if ( msg.lParam )
				{
					WINDOWPOS* pWPos = (WINDOWPOS*)msg.lParam;

					if (pWPos->flags & SWP_SHOWWINDOW)
					{
						BOOL bRes = Skin(msg.hwnd);
					}
					else if (pWPos->flags & SWP_HIDEWINDOW)
					{
						BOOL bRes = Unskin(msg.hwnd);
					}
				}
			}
			else
			{
				if (msg.wParam)
				{
					WINDOWPOS* pWPos = (WINDOWPOS*)msg.lParam;

					if (pWPos->flags & SWP_SHOWWINDOW)
						BOOL bRes = InitSkin(msg.hwnd);
				}
			}
			break;

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

		case 0x1e2:
			if (CSkinMenu::IsMenuWnd(msg.hwnd) && msg.wParam)
			{
				BOOL bRes = Skin(msg.hwnd);

				if (!bRes)
					TRACE("Skin failed on 0x1e2");
			}
			break;
			
		case WM_SHOWWINDOW: 
			if (CSkinMenu::IsMenuWnd(msg.hwnd))
			{
				if (msg.wParam)
				{
					BOOL bRes = Skin(msg.hwnd);
				}
				else // if (!msg.wParam)
				{
					BOOL bRes = Unskin(msg.hwnd);
				}
			}
			else
			{
				if (msg.wParam)
				{
					BOOL bRes = InitSkin(msg.hwnd);
				}
			}
			break;

		case WM_DESTROY:
		case WM_NCDESTROY:
			if (CSkinMenu::IsMenuWnd(msg.hwnd))
			{
				BOOL bRes = Unskin(msg.hwnd);
			}
			
			RemovehWnd( msg.hwnd );

			break;

			// grab the menu handle at each and every opportunity
			//
			// notes:
			//
			// 1. menu bars generate a WM_INITMENUPOPUP prior to showing their menus
			//
			// 2. edit windows do not generate WM_INITMENUPOPUP prior to showing
			//    their context menu so the best we can do is grab their window handle
			//
			// 3. other controls display their context menus prior to generating
			//    a WM_INITMENUPOPUP

		case WM_CONTEXTMENU: // means a menu may be about to appear
			{
				m_pCurSkinMenu = NULL;
				m_hCurContextWnd = msg.hwnd;

				TRACE ("WM_CONTEXTMENU sent to window of type '%s'\n", CWinClasses::GetClass(msg.hwnd));
			}
			break;

		case WM_INITMENUPOPUP:

			if (m_pCurSkinMenu && !m_pCurSkinMenu->GetMenu())
			{
				m_pCurSkinMenu->SetContextWnd(NULL);
				m_pCurSkinMenu->SetMenu((HMENU)msg.wParam, GetParentSkinMenu((HMENU)msg.wParam));

				m_hCurMenu = NULL;
				m_pCurSkinMenu = NULL;
				m_hCurContextWnd = NULL;
			}
			else // save for menu skinning
				m_hCurMenu = (HMENU)msg.wParam;
		}
#endif

		bSkinning = FALSE;
	}
}

BOOL CSkinMenuMgr::OnCbt(int nCode, WPARAM wParam, LPARAM lParam)
{   
	if (nCode == HCBT_SYSCOMMAND)
	{
		switch (wParam)
		{
		case SC_MOUSEMENU:
			if (lParam)
			{
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);

				HWND hwnd = WindowFromPoint(CPoint(xPos, yPos));

				if (hwnd && (GetWindowLong(hwnd, GWL_STYLE) & WS_SYSMENU))
				{
					// convert to window coords
					CRect rWindow;
					GetWindowRect(hwnd, rWindow);

					CPoint ptScreen(xPos, yPos);

					xPos -= rWindow.left;
					yPos -= rWindow.top;

					if (xPos > 0 && xPos < 16 && yPos > 0 && yPos < 16)
					{
						CSkinBase::DoSysMenu(CWnd::FromHandle(hwnd), ptScreen, NULL, TRUE);
						return TRUE; // handled
					}
				}
			}
			break;
		}
	}

	return FALSE;
}

CSkinMenu* CSkinMenuMgr::GetSkinMenu(HWND hWnd)
{
	CSkinMenu* pSkin = NULL;
	
	MENUHWND_ITERATOR it = m_mapMenus.find( hWnd );

	if( it != m_mapMenus.end() )
	{
		pSkin = it->second;
	}

	return pSkin;
}

BOOL CSkinMenuMgr::Unskin(HWND hWnd)
{
	m_pCurSkinMenu = NULL;
	m_hCurContextWnd = NULL;
	m_hCurMenu = NULL;

	ASSERT (CSkinMenu::IsMenuWnd(hWnd));

	CSkinMenu* pSkinMenu = GetSkinMenu(hWnd);
	
	if (!pSkinMenu)
		return TRUE; // already done
	
	TRACE ("menu unskinned (%08X)\n", (UINT)hWnd);
	MENUHWND_ITERATOR it = m_mapMenus.find( hWnd );
	if( it != m_mapMenus.end() )
	{
		m_mapMenus.erase(it);
	}

	pSkinMenu->DetachWindow();
	delete pSkinMenu;

	return TRUE;
}

BOOL CSkinMenuMgr::Skin(HWND hWnd)
{
	ASSERT (CSkinMenu::IsMenuWnd(hWnd));

	CSkinMenu* pSkinMenu = GetSkinMenu(hWnd);
	
	if (pSkinMenu)
		return TRUE; // already done

	pSkinMenu = new CSkinMenu(&m_skGlobals, m_dwMenuStyle, m_nSidebarWidth);
	
	if (pSkinMenu && pSkinMenu->AttachWindow(hWnd))
	{
		TRACE ("menu skinned (%08X)\n", (UINT)hWnd);

		m_pCurSkinMenu = pSkinMenu;
		m_pCurSkinMenu->SetContextWnd(m_hCurContextWnd);
		m_pCurSkinMenu->SetMenu(m_hCurMenu, GetParentSkinMenu(m_hCurMenu));

		m_mapMenus.insert( std::make_pair(hWnd, pSkinMenu) );
		//m_mapMenus[hWnd] = pSkinMenu;

		return TRUE;
	}
	
	// else
	delete pSkinMenu;
	return FALSE;
}

CSkinMenu* CSkinMenuMgr::GetParentSkinMenu(HMENU hMenu)
{
	if (!hMenu)
		return NULL;

	// search the map if CSkinMenus looking for a menu
	// having this menu as a popup
	HWND hwnd;
	CSkinMenu* pSkin;

	MENUHWND_ITERATOR it = m_mapMenus.begin();
	for ( it = m_mapMenus.begin(); it != m_mapMenus.end(); it++)
	{
		pSkin = it->second;

		const HMENU hOther = pSkin->GetMenu();

		if (hOther && hOther != hMenu)
		{
			// iterate the items looking for submenus
			int nMenu = GetMenuItemCount(hOther);

			while (nMenu--)
			{
				if (GetSubMenu(hOther, nMenu) == hMenu) // submenu
					return pSkin;
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 全部是为了修改皮肤和WC_CONTROLBAR
void CSkinMenuMgr::AddhWnd(HWND hWnd )
{
	SKINHWND_ITERATOR it = _hwnd_maps.find( hWnd );
	if( it == _hwnd_maps.end() )
	{
		_hwnd_maps.insert( std::make_pair(hWnd, hWnd) );	
	}
}

void CSkinMenuMgr::RemovehWnd( HWND hWnd )
{
	SKINHWND_ITERATOR it = _hwnd_maps.find( hWnd );
	if( it == _hwnd_maps.end() )
	{
		_hwnd_maps.erase(it);
	}
}


BOOL CSkinMenuMgr::InitSkin( HWND hWnd )
{
	if( CWinClasses::IsClass(hWnd, WC_CONTROLBAR) )
	{
		//SkinControlBar<ATL::CWindow>::InstallHook( hWnd );
	}

	return TRUE;
}

void CSkinMenuMgr::UpdateUI()
{
	SKINHWND_ITERATOR it = _hwnd_maps.begin();
	for ( it = _hwnd_maps.begin(); it != _hwnd_maps.end(); it++)
	{
		::SendMessage ( it->first, WM_NCACTIVATE, 1, 0 );
		//::InvalidateRect( it->first, NULL, TRUE);
		RedrawWindow(it->first, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
	}
}

void CSkinMenuMgr::RefreshSkin()
{
	GetInstance().UpdateUI();
}
} // namespace Skin