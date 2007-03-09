#pragma once

#include "../base/skinctrl.h"
#include "../base/skinhookbase.h"

/*
CPlugInApp *pApp = static_cast<CPlugInApp*>(AfxGetApp());
if (pApp)
{
TRACE("Enumerating application\n");
EnumResourceNames(pApp->m_hInstance, RT_TOOLBAR,(ENUMRESNAMEPROC)EnumResNameProc, 0);

// now enumerate all the plug-in DLL toolbars
int dllCount = pApp->GetPlugInDLLCount();
for(int dll = 0 ; dll < dllCount; dll++)
{
TRACE("Enumerating DLL %1d\n", dll);
EnumResourceNames(pApp->GetDLL(dll)->GetHInstance(), RT_TOOLBAR,(ENUMRESNAMEPROC)EnumResNameProc, 0);
}
}

*/

/*
0081 menu	WM_NCCREATE
0083 menu	WM_NCCALCSIZE
0001 menu	WM_CREATE
0005 menu	WM_SIZE
0003 menu	WM_MOVE
01e2 menu	MN_SIZEWINDOW
0046 menu	WM_WINDOWPOSCHANGING
0083 menu	WM_NCCALCSIZE
0047 menu	WM_WINDOWPOSCHANGED
0005 menu	WM_SIZE
0046 menu	WM_WINDOWPOSCHANGING
0317 menu	WM_PRINT
00af menu
0014 menu	WM_ERASEBKGND
0318 menu	WM_PRINTCLIENT
0047 menu	WM_WINDOWPOSCHANGED
01e5 menu	MN_SELECTITEM
01e5 menu	MN_SELECTITEM
01e5 menu	MN_SELECTITEM
01e5 menu	MN_SELECTITEM
01e5 menu	MN_SELECTITEM
0046 menu	WM_WINDOWPOSCHANGING
0047 menu	WM_WINDOWPOSCHANGED
0002 menu	WM_DESTROY
0082 menu	WM_NCDESTROY
*/
extern long FindItemIDThatOwnsThisMenu (HMENU hMenuOwned,HWND* phwndOwner,
                                        BOOL* pfPopup,BOOL *pfSysMenu);

namespace Skin {



#define IMGWIDTH 16
#define IMGHEIGHT 16
#define IMGPADDING 6
#define TEXTPADDING 8
#define TEXTPADDING_MNUBR 4
#define SM_CXSHADOW 4

#ifndef MN_SETHMENU

// copy from windows\media\avi\inc.16\windows.inc(1500)

#define MN_SETHMENU              0x01E0
#define MN_GETHMENU              0x01E1
#define MN_SIZEWINDOW            0x01E2
#define MN_OPENHIERARCHY         0x01E3
#define MN_CLOSEHIERARCHY        0x01E4
#define MN_SELECTITEM            0x01E5
#define MN_CANCELMENUS           0x01E6
#define MN_SELECTFIRSTVALIDITEM  0x01E7

#define MN_BUTTONUP				 0x01EF

//;MN_GETPPOPUPMENU       = 01EAh ;Win32
//;MN_FINDMENUWINDOWFROMPOINT = 01EBh ;Win32
//;MN_SHOWPOPUPWINDOW     = 01ECh ;Win32
//;MN_BUTTONDOWN          = 01EDh ;Win32
//;MN_MOUSEMOVE           = 01EEh ;Win32
//;MN_BUTTONUP            = 01EFh ;Win32
//;MN_SETTIMERTOOPENHIERARCHY = 01F0 ;Win32

#endif // #ifndef MN_SETHMENU

#ifndef MSG_MN_SELECTITEM
#define MSG_MN_SELECTITEM(func) \
    if (uMsg == MN_SELECTITEM) \
    { \
        SetMsgHandled(TRUE); \
        func((int)wParam); \
        lResult = 0; \
        if (IsMsgHandled()) \
        return TRUE; \
    }
#endif // #ifndef MSG_MN_SELECTITEM



// 1 要让系统处理，MN_SELECTITEM,不然会死得很难看
// GetCurrentSelectedIndex

//template<class BaseT = CWindow>
//class SkinMenu : public SkinControlImpl<SkinMenu, BaseT, HookPolicy>
//class SkinMenu : public CSkinHookImpl<SkinMenu>
template<class BaseT = ATL::CWindow>
struct SkinMenu : public SkinControlImpl<SkinMenu, BaseT>
//class SkinMenu : public CSkinHookImpl<SkinMenu>
{
public:
    //enum { class_id = MENU };

	typedef struct
	{
		HMENU hMenu;
		int nID;
		UINT nType;
	} OLDMENUITEM; 

	enum { REDRAWALL = -2 };

    static LPCTSTR GetWndClassName()
    {
        return _T("#32768");
    }

	void OnFirstMessage()
	{
		int i = 0;
	}


    //typedef SkinMenu<BaseT> this_type;
    //typedef SkinControlImpl<SkinMenu, BaseT> base_type;

    SkinMenu()
    {
		_classid = MENU;
		m_bFirstRedraw = TRUE;
		m_bAnimatedMenus = FALSE;
		SystemParametersInfo(SPI_GETMENUANIMATION, 0, &m_bAnimatedMenus, 0);

		NONCLIENTMETRICS info; info.cbSize = sizeof(info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
		_fontHorzMenu.CreateFontIndirect(&info.lfMenuFont);

		_spaceIcon = 4;
		_spaceText = 20;
		m_bSysMenu = FALSE;
		m_minSystemMenuWidth = 0;

		m_nSelIndex = REDRAWALL;
		m_nUpdateItem = -1;
		m_bLButtonUp = FALSE;

	}

    BEGIN_MSG_MAP(SkinMenu /*this_type*/)
//        ATLASSERT(::IsWindow(m_hWnd));
//        if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
//              && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
//            TRACE("Menu: %p %08x %08x %08x\n", m_hWnd, uMsg, wParam, lParam);
//        WM_PAINT 似乎是第一个设置了 HMENU 后的消息，还有一个更早的 MN_SIZEWINDOW，没有测试过
//        if (uMsg == WM_PAINT && m_menu.IsNull())
//        {
//          m_menu = GetHMenu();
//        }
/*
		if ( m_menu.IsNull() && m_hWnd )
		{
			HMENU hm = GetHMenu();
			if ( hm )
				m_menu = hm;
			if ( !m_menu.IsNull() )
			{
				InitMenu();
			}
		}
*/	
		ATLTRACE("%04x menu\n", uMsg);
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_PAINT(OnPaint)
        MSG_MN_SELECTITEM(OnSelectItem)
        MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_PRINT(OnPrint)
	//	MSG_WM_ERASEBKGND(OnEraseBkgnd)

		//MESSAGE_HANDLER( MN_BUTTONUP, OnLButtonUp)

		//MSG_WM_WINDOWPOSCHANGING( OnWindowPosChange )
		//MSG_WM_WINDOWPOSCHANGED( OnWindowPosChanged )

//		MSG_WM_PRINTCLIENT(OnPrintClient)
//		MSG_WM_SIZE(OnSize)
//        MSG_WM_PRINT()
//        MSG_WM_PRINTCLIENT()
//        MSG_WM_NCCALCSIZE
//        MSG_WM_WINDOWPOSCHANGING
//        MSG_WM_ERASEBKGND
      //  MSG_WM_CREATE(OnCreate)
//		MSG_WM_DRAWITEM( OnDrawItem )
//		MSG_WM_MEASUREITEM( OnMeasureItem )
    END_MSG_MAP()

private:
    // ! 不知是何时 m_hWnd 和 Menu 才关联起来
    // 注意此函数调用可能失败
    HMENU GetHMenu()
    {
        return (HMENU)SendMessage(MN_GETHMENU, 0, 0);
    }

    SIZE GetSizeWindow()
    {
        DWORD dwSize = (DWORD)SendMessage(MN_SIZEWINDOW, 0, 0);
        SIZE sizeRet = { GET_X_LPARAM(dwSize), GET_Y_LPARAM(dwSize) };
        return sizeRet;
    }

	HFONT GetCtrlFont(HWND hwnd)
	{
		//HFONT hFont;
		//如果sendmessage得到的是NULL,则使用的是系统字体
		//if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
		//hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

		//return hFont;
		return _fontHorzMenu.m_hFont;
	}

	void GetInvalidRect(int nCurSel, int nPrevSel, LPRECT lpRect)
	{
		ASSERT (lpRect);

		if ( m_menu.IsNull() || nCurSel == REDRAWALL || nPrevSel == REDRAWALL)
			GetClientRect(lpRect);
		else if ( !m_menu.IsNull() )
		{
			::SetRectEmpty(lpRect);

			if (nCurSel >= 0 || nPrevSel >= 0)
			{
				if (nCurSel >= 0)
				{
					GetMenuItemRect(NULL, m_menu.m_hMenu, nCurSel, lpRect);
				}

				if (nPrevSel >= 0)
				{
					CRect rTemp;

					GetMenuItemRect(NULL, m_menu.m_hMenu, nPrevSel, rTemp);
					::UnionRect(lpRect, lpRect, rTemp);
				}

				// convert to client coords
				ScreenToClient(lpRect);
			}
		}
	}

	void GetDrawRect( LPRECT pWindow, LPRECT pClient )
	{
		CRect rWindow;
		GetWindowRect(rWindow);

		if (pClient)
		{
			GetClientRect(pClient);
			ClientToScreen(pClient);
			::OffsetRect(pClient, -rWindow.left, -rWindow.top);
		}

		if (pWindow)
		{
			rWindow.OffsetRect(-rWindow.TopLeft());
			*pWindow = rWindow;
		}
	}


	int GetCurSel()
	{
		int nItem = GetMenuItemCount( m_menu.m_hMenu );

		while (nItem--)
		{
			if (GetMenuState(m_menu.m_hMenu, nItem, MF_BYPOSITION) & MF_HILITE)
				return nItem;
		}

		return -1; // nothing selected
	}
public:

	// 绘制每一个item
	void DrawItem( HDC hdc, int nID, CRect& rcItem )
	{
		CDCHandle dc;
		dc.Attach( hdc );
		
			// 0 判断状态
		int nState = 1;

		UINT  nMenuState = GetMenuState( m_menu.m_hMenu, nID, MF_BYPOSITION );

		if ( (nMenuState & MF_DISABLED ) || (nMenuState & MF_GRAYED ) )
		{
			nState = 4;
		}
		else if ( (nMenuState & MF_HILITE ) )
		{
			nState = 2;
		}

		// 1 判断类型
		MENUITEMINFO mii = { sizeof MENUITEMINFO, MIIM_TYPE | MIIM_SUBMENU };
		::GetMenuItemInfo (m_menu.m_hMenu, nID, TRUE, &mii);
		if ( (mii.fType & MFT_SEPARATOR) )
		{
			if (_scheme)
				_scheme->DrawBackground(dc, _classid, 1, nState, &rcItem, NULL );

			int nSepHeight = GetSchemeHeight( 2, 1 );
			WTL::CRect rcSep = rcItem;
			rcSep.top = rcItem.top + ( rcItem.Height() - nSepHeight ) / 2 ;
			rcSep.bottom = rcSep.top + nSepHeight;
			if (_scheme)
				_scheme->DrawBackground(dc, _classid, 2, 1, &rcSep, NULL );
		}
		/*else if ( (mii.fType & MFT_OWNERDRAW) )
		{
			TRACE( "DrawItem MFT_OWNERDRAW \r\n ");
		}*/
		else
		{
			const int string_size = m_menu.GetMenuString(nID, 0, 0, MF_BYPOSITION);
			if ( string_size == 0 && (mii.fType & MFT_OWNERDRAW) )
			{
				TRACE( "DrawItem MFT_OWNERDRAW \r\n ");
			}
			else
			{
				if (_scheme)
					_scheme->DrawBackground(dc, _classid, 1, nState, &rcItem, NULL );

				if ( mii.hSubMenu )
				{
					// 绘制
					// 如果有 hSubMenu  那么就绘制右箭头
					WTL::CRect rcIcon;
					if (_scheme)
					{
						rcIcon = GetSchemeRect( 8, 1 );

						int nX = _spaceIcon + rcIcon.Width() ;
						int nY = (rcItem.Height() - rcIcon.Height()) / 2;
						rcIcon.OffsetRect(  rcItem.right - nX - rcIcon.left, nY - rcIcon.top + rcItem.top ); 

						_scheme->TransparentDraw(dc, _classid, 8, 1, &rcIcon );
					}
				}
			}
		
			HFONT hOldFont = 0;
			WTL::CFont fontBold;
			// 判断是否是黑体
			if( (nMenuState & MFS_DEFAULT) != 0 ) 
			{
				// Need bold version of font
				LOGFONT lf;
				_fontHorzMenu.GetLogFont(lf);
				lf.lfWeight += 200;
				fontBold.CreateFontIndirect(&lf);
				ATLASSERT(!fontBold.IsNull());
				hOldFont = dc.SelectFont(fontBold);
			}
			else
			{
				hOldFont = dc.SelectFont( _fontHorzMenu.m_hFont );
			}

			// 绘制图片 radio/check/hbitmap
			WTL::CBitmapHandle hBitmap;
			if ( ( nMenuState & MFS_CHECKED ) )
			{
				MENUITEMINFO   mbmp;   
				ZeroMemory(&mbmp,sizeof(mbmp));   
				mbmp.cbSize=sizeof(mbmp);   
				mbmp.fMask=MIIM_CHECKMARKS;
				::GetMenuItemInfo (m_menu.m_hMenu, nID, TRUE, &mbmp);
				if ( mbmp.hbmpChecked )
				{
					hBitmap.Attach( mbmp.hbmpChecked );
				}
				else
				{
					if ( (mii.fType & MFT_RADIOCHECK) )
					{
						// todo raido
						WTL::CRect rcIcon;
						if (_scheme)
						{
							rcIcon = GetSchemeRect( 7, 1 );
							rcIcon.OffsetRect( -rcIcon.left, -rcIcon.top );

							int nX = _spaceIcon;
							int nY = (rcItem.Height() - rcIcon.Height()) / 2;
							rcIcon.OffsetRect( nX, nY ); 
					
							_scheme->TransparentDraw(dc, _classid, 7, 1, &rcIcon );
						}
					}
					else
					{
						WTL::CRect rcIcon;
						if (_scheme)
						{
							rcIcon = GetSchemeRect( 6, 1 );

							int nX = _spaceIcon;
							int nY = (rcItem.Height() - rcIcon.Height()) / 2;
							rcIcon.OffsetRect( nX -rcIcon.left + rcItem.left , nY - rcIcon.top + rcItem.top ); 

							_scheme->TransparentDraw(dc, _classid, 6, 1, &rcIcon );
						}
					}
				}
			}
			else if ( ( nMenuState & MFS_UNCHECKED ) )
			{
				MENUITEMINFO mbmp = { sizeof MENUITEMINFO, MIIM_CHECKMARKS | MIIM_TYPE  };
				::GetMenuItemInfo (m_menu.m_hMenu, nID, TRUE, &mbmp);
				if ( mbmp.hbmpUnchecked )
				{
					hBitmap.Attach( mbmp.hbmpUnchecked );
				}
				else
				{
					WTL::CRect rcIcon;
					if (_scheme)
					{
						rcIcon = GetSchemeRect( 6, 2 );
						rcIcon.OffsetRect( -rcIcon.left, -rcIcon.top );

						int nX = _spaceIcon;
						int nY = (rcItem.Height() - rcIcon.Height()) / 2;
						rcIcon.OffsetRect( nX, nY ); 

						_scheme->TransparentDraw(dc, _classid, 6, 2, &rcIcon );
					}
				}
			}
			else if ( ( mii.fType & MFT_BITMAP ) )
			{
				MENUITEMINFO mbmp ;//= { sizeof MENUITEMINFO,  MIIM_TYPE | MIIM_BITMAP  }; 
				ZeroMemory(&mbmp, sizeof(mbmp));  
				mbmp.cbSize = sizeof(mbmp);
				mbmp.fMask =  MIIM_BITMAP ;
				m_menu.GetMenuItemInfo ( nID, TRUE, &mbmp );
				if ( mbmp.fType & MFT_BITMAP)
				{
					if ( mbmp.hbmpItem )
					{
						hBitmap.Attach( mbmp.hbmpItem );
					}
				}
			}

			// draw icon
			if ( !hBitmap.IsNull() )
			{
				SIZE sz;
				hBitmap.GetSize(sz);
				int nX = _spaceIcon;
				int nY = (rcItem.Height() - sz.cy) / 2;

				WTL::CClientDC cdc(m_hWnd);
				HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
				HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC, hBitmap);
				::BitBlt(dc.m_hDC, nX, nY, sz.cx, sz.cy, hDC, 0, 0, SRCCOPY);
				SelectObject(hDC, pOldBitmapImage);
				::DeleteObject(hDC);
				::ReleaseDC(m_hWnd, cdc);
			}

			
			
			
			
			if (string_size > 0)
			{
				char* sz = new char[string_size + 1];
				int n = m_menu.GetMenuString(nID, sz, string_size  + 1, MF_BYPOSITION);
				ASSERT(n == string_size);
				if (n)
				{
					int nTab = -1;
					for( int j = 0; j < ::lstrlen(sz); j++ ) 
					{
						if( sz[j] == _T('\t')) 
						{
							nTab = j;
							break;
						}
					}

					COLORREF clr =  GetSchemeColor( 1, nState, TMT_TEXTCOLOR );

					COLORREF clrOldText = ::SetTextColor( dc.m_hDC, clr );

					int mode = SetBkMode(dc.m_hDC, TRANSPARENT);

					UINT uFlags = DT_SINGLELINE | DT_VCENTER;
					//uFlags |= DT_HIDEPREFIX;

					rcItem.left += _spaceText;
					rcItem.right -= _spaceText;
					
					dc.DrawText(sz, nTab, &rcItem, DT_LEFT | uFlags);
					if( nTab >= 0 ) 
						dc.DrawText(&sz[nTab + 1], -1, &rcItem, DT_RIGHT | uFlags);

					::SetTextColor( dc.m_hDC, clrOldText );
					SetBkMode(dc.m_hDC, mode);
					//dc.DrawText(sz, n, (LPRECT)&rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				delete [] sz;
			}	

			dc.SelectFont(hOldFont);

		}

	}
	void SetMenu( HMENU hMenu )
	{
		m_menu = hMenu;
	}

	void DoNcPaint(HDC hdc)
	{
		CRect rWindow, rClient;
		GetDrawRect(rWindow, rClient);

		CDCHandle dc;
		dc.Attach( hdc );
		int nSaveDC = dc.SaveDC( ); // must restore dc to original state

		dc.ExcludeClipRect(rClient);
		
		if (_scheme)
			_scheme->DrawBackground(dc, _classid, 0, 1, &rWindow, NULL );

		dc.RestoreDC(nSaveDC);
	}

	LRESULT OnEraseBkgnd( HDC hdc )
	{
		return 1;
	}

    void OnNcPaint(HRGN)
    {
		TRACE( "OnNcPaint \r\n ");

		WTL::CWindowDC dc(m_hWnd);

		DoNcPaint( dc.m_hDC );

		return;
		/*
		if (!m_bAnimatedMenus || !m_bFirstRedraw)
		{
			WTL::CWindowDC dc(m_hWnd);

			DoNcPaint( dc.m_hDC );

			return;
		}
		else
		{
			DefWindowProc();
		}
		*/
    }


	LRESULT OnLButtonUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		m_bLButtonUp = TRUE;
		//ShowWindow( SW_HIDE );
		LRESULT lRet = DefWindowProc();
		return lRet;
	}


	void OnWindowPosChange( LPWINDOWPOS lp )
	{
		if ( m_bLButtonUp )
			ShowWindow( SW_HIDE );

		DefWindowProc();
/*
		if ( m_bLButtonUp )
		{
			WTL::CRect rc;
			WTL::CWindowDC dc ( m_hWnd );
			WTL::CMemoryDC memdc ( dc.m_hDC, rc );

			int cItems = m_menu.GetMenuItemCount();
			for ( int i = 0; i < cItems ;i++ )
			{
				WTL::CRect rcItem;
				BOOL bRet = GetMenuItemRect(NULL, m_menu.m_hMenu, i, &rcItem);
				//ASSERT ( bRet );
				ScreenToClient(&rcItem);
				DrawItem( memdc.m_hDC, i, rcItem );
			}
		}
*/		
	}

	
	void OnWindowPosChanged( LPWINDOWPOS lParam )
	{
		if ( m_bLButtonUp )
			ShowWindow( SW_HIDE );

		DefWindowProc();
/*
		if ( m_bLButtonUp )
		{
			WTL::CRect rc;
			WTL::CWindowDC dc ( m_hWnd );
			WTL::CMemoryDC memdc ( dc.m_hDC, rc );

			int cItems = m_menu.GetMenuItemCount();
			for ( int i = 0; i < cItems ;i++ )
			{
				WTL::CRect rcItem;
				BOOL bRet = GetMenuItemRect(NULL, m_menu.m_hMenu, i, &rcItem);
				//ASSERT ( bRet );
				ScreenToClient(&rcItem);
				DrawItem( memdc.m_hDC, i, rcItem );
			}
		}
*/
	}

	void OnPaint(HDC)
    {
		TRACE( "OnPaint  \r\n ");
		if (m_menu.IsNull())
			m_menu = GetHMenu();

		WTL::CRect rc;
		GetClientRect( rc );

		WTL::CPaintDC paintdc(m_hWnd);
		
		WTL::CMemoryDC dc ( paintdc.m_hDC, rc );

		DefWindowProc( WM_PRINTCLIENT,(WPARAM)dc.m_hDC, (LPARAM)PRF_CLIENT ); //| WM_PRINTCLIENT | PRF_OWNED );

		int cItems = m_menu.GetMenuItemCount();
		for ( int i = 0; i < cItems ;i++ )
		{
			WTL::CRect rcItem;
			BOOL bRet = GetMenuItemRect(NULL, m_menu.m_hMenu, i, &rcItem);
			ASSERT ( bRet );
			ScreenToClient(&rcItem);
			DrawItem( dc.m_hDC, i, rcItem );
		}
		return;
    }


	void OnPrint( HDC wParam, UINT lParam )
	{
		if (m_menu.IsNull())
			m_menu = GetHMenu();
		//DefWindowProc();
		TRACE( "OnPrint lParam is %d \r\n ", lParam);

		//DoNcPaint( wParam );
		
		WTL::CRect rect;
		GetWindowRect( rect );

		WTL::CRect rcw = rect;
		rcw.OffsetRect( - rcw.left, -rcw.top );

		WTL::CMemoryDC dc ( wParam, rcw );

		WTL::CRect rc;
		GetClientRect( rc );

		if (_scheme)
			_scheme->DrawBackground(dc.m_hDC, _classid, 0, 1, &rcw, NULL );

		HDC dcMem = ::CreateCompatibleDC(dc.m_hDC);
		ASSERT(dcMem);
		HBITMAP bmpMemBg = ::CreateCompatibleBitmap(dc.m_hDC, rc.Width(), rc.Height());
		ASSERT(bmpMemBg);
		HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
		ASSERT(pOldBmp);

		DefWindowProc( WM_PRINTCLIENT,(WPARAM)dcMem, (LPARAM)lParam );

		CPoint wndOffset(0,0);
		ClientToScreen( &wndOffset );
		wndOffset -= rect.TopLeft();

		int cItems = m_menu.GetMenuItemCount();

		for ( int i = 0; i < cItems ;i++ )
		{
			WTL::CRect rcItem;
			BOOL bRet = GetMenuItemRect(m_hWnd, m_menu.m_hMenu, i, &rcItem);
			ASSERT ( bRet );
			ScreenToClient(&rcItem);
			DrawItem( dcMem, i, rcItem );
		}

		::BitBlt( dc.m_hDC, wndOffset.x, wndOffset.y, rc.Width(), rc.Height(), dcMem,0, 0, SRCCOPY);	

		::SelectObject(dcMem, pOldBmp);
		::DeleteObject(bmpMemBg);
		::DeleteDC(dcMem);

		
		return;
	}


    void OnKeyDown(TCHAR wp, UINT, UINT)
    {
		TRACE( "OnKeyDown char is %d \r\n ", wp);
		LRESULT lr = 0;
		switch (wp)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_RIGHT:
			// left is much trickier because if the currently selected item
			// has a popup menu then left will close that submenu, and if
			// we prevent the default redrawing then the submenu is not correctly
			// removed from the screen.
			// so we must always do the default drawing and follow it up with our own.
		case VK_LEFT:
			if ( !m_menu.IsMenu() )
			{
				if (wp != VK_LEFT) 
					SetRedraw(FALSE);

				lr = DefWindowProc();

				if (wp != VK_LEFT) 
					SetRedraw(TRUE);

				//					TRACE ("Invalidating entire menu in response to a cursor keypress\n");

				m_nSelIndex = -1; // reset current selection because its too risky to 
				// try to figure it out for ourselves
				Invalidate(FALSE);
				UpdateWindow( );

				m_bFirstRedraw = FALSE;
			}
			else // have menu handle
			{
				int nPrevSel = GetCurSel();

				if (wp != VK_LEFT) 
					SetRedraw(FALSE);

				lr = DefWindowProc();

				if (wp != VK_LEFT) 
					SetRedraw(TRUE);

				// if we have the handle of the menu then 
				// we can do a selective redraw else we must redraw all
				m_nSelIndex = GetCurSel();

				if (m_nSelIndex != nPrevSel)
				{
					CRect rInvalid;
					GetInvalidRect(m_nSelIndex, nPrevSel, rInvalid);

					//						TRACE ("Invalidating menu items %d & %d in response to a cursor keypress\n", m_nSelIndex, nPrevSel);

					InvalidateRect( rInvalid, FALSE );
					//UpdateWindow( );

					m_bFirstRedraw = FALSE;
				}
			}
			if (m_bAnimatedMenus && m_bFirstRedraw)
			{
				CWindowDC dc( m_hWnd );
				DoNcPaint( dc.m_hDC );
			}
			return ;
		}
		DefWindowProc();
		/*
        ATLASSERT(::IsWindow(m_hWnd));
        DefWindowProc();

        if (m_menu.IsNull() && m_hWnd)
            m_menu = GetHMenu();

        if (m_hWnd) // 防止窗口已经被销毁
        {
            int nUpdateItem = GetCurrentSelectedIndex();
            if (m_nUpdateItem != nUpdateItem)
            {
                // new item
                if (nUpdateItem != -1)
                    InvalidItem(nUpdateItem);

                if (-1 != m_nUpdateItem) // old item
                    InvalidItem(m_nUpdateItem);

                m_nUpdateItem = nUpdateItem;
            }
        }
		*/

        return;
    }

    LRESULT OnSelectItem(int nUpdateItem)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        if (m_menu.IsNull())
            m_menu = GetHMenu();

		TRACE("OnSelectItem \r\n");
		
		//CClientDC dc ( m_hWnd );
		//DoPaint( dc.m_hDC );
		
		LRESULT lRet = 0;
		

        if (m_nUpdateItem != nUpdateItem)
        {
            // new item
			CRect rInvalid;

			if ( !m_menu.IsNull() )
				GetInvalidRect((int)nUpdateItem, m_nUpdateItem, rInvalid);
			else
				GetClientRect(rInvalid);

			SetRedraw(FALSE);
			lRet = DefWindowProc();
			SetRedraw(TRUE);

            m_nUpdateItem = nUpdateItem;

			InvalidateRect( rInvalid, FALSE );

			if (!m_bFirstRedraw)
				UpdateWindow();
        }

		// special fix for animated menus
		if (m_bAnimatedMenus && m_bFirstRedraw)
		{
			CWindowDC dc( m_hWnd );
			DoNcPaint( dc.m_hDC );
		}

		m_bFirstRedraw = FALSE;
		
		return lRet;
        //SetMsgHandled(FALSE);
        return 0;
    }


private:
   
    
    WTL::CMenuHandle m_menu;
    
	BOOL	m_bFirstRedraw;
	BOOL	m_bAnimatedMenus;
	int		m_nUpdateItem;
	WTL::CFont _fontHorzMenu;
	int		_spaceIcon;
	int		_spaceText;
	BOOL	m_bSysMenu;
	int		m_minSystemMenuWidth;
	int		m_nSelIndex;

	BOOL	m_bLButtonUp;
	//typedef std::map< OLDMENUITEM* , HMENU >::iterator OWNER_ITERATOR;
};

}; // namespace
