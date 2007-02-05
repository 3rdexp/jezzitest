#pragma once

#include "../base/skinctrl.h"
#include "../base/skinhookbase.h"

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

template<class BaseT = CWindow>
class SkinMenu : public SkinControlImpl<SkinMenu, BaseT>
//class SkinMenu : public CSkinHookImpl<SkinMenu>
//template<class BaseT = ATL::CWindow>
//struct SkinMenu : public SkinControlImpl<SkinMenu, BaseT, HookPolicy>
//class SkinMenu : public CSkinHookImpl<SkinMenu>
{
public:
    //enum { class_id = MENU };

	enum { REDRAWALL = -2 };

    static LPCTSTR GetWndClassName()
    {
        return _T("#32768");
    }

	void OnFirstMessage()
	{
		int i = 0;
	}


    typedef SkinMenu<BaseT> this_type;
    typedef SkinControlImpl<SkinMenu, BaseT> base_type;

    SkinMenu()
        : m_nUpdateItem(-1)
        , m_nSelectedItem(-1)
        , m_fSysMenu(FALSE)
        , m_fPopup(FALSE)
    {
		_classid = MENU;
		m_bFirstRedraw = TRUE;
		m_bAnimatedMenus = FALSE;
		SystemParametersInfo(SPI_GETMENUANIMATION, 0, &m_bAnimatedMenus, 0);

		NONCLIENTMETRICS info; info.cbSize = sizeof(info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
		_fontHorzMenu.CreateFontIndirect(&info.lfMenuFont);

		_spaceIcon = 4;
		_spaceText = 32;

	}

    BEGIN_MSG_MAP(/*SkinMenu*/this_type)
//        ATLASSERT(::IsWindow(m_hWnd));
//        if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
//              && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
//            TRACE("Menu: %p %08x %08x %08x\n", m_hWnd, uMsg, wParam, lParam);
//        WM_PAINT 似乎是第一个设置了 HMENU 后的消息，还有一个更早的 MN_SIZEWINDOW，没有测试过
//        if (uMsg == WM_PAINT && m_menu.IsNull())
//        {
//          m_menu = GetHMenu();
//        }

		//ATLTRACE("%04x menu\n", uMsg);
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_PAINT(OnPaint)
        MSG_MN_SELECTITEM(OnSelectItem)
        //MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_PRINT(OnPrint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PRINTCLIENT(OnPrintClient)
		//MSG_WM_SIZE(OnSize)
//        MSG_WM_PRINT()
//        MSG_WM_PRINTCLIENT()
//        MSG_WM_NCCALCSIZE
//        MSG_WM_WINDOWPOSCHANGING
//        MSG_WM_ERASEBKGND
//        MSG_WM_CREATE(OnCreate)
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

public:

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



	void DoPaint( HDC hdc, BOOL bClient = TRUE )
	{
		if (m_menu.IsNull() && m_hWnd)
			m_menu = GetHMenu();

		CRect rcw;
		GetWindowRect( rcw );
		rcw.OffsetRect( -rcw.left, -rcw.top );
		CRect rcc;
		GetClientRect( rcc );
		int nTop = (rcw.Height() - rcc.Height()) / 2;
		int nLeft = (rcw.Width() - rcc.Width() ) / 2;

		CDCHandle dc;
		dc.Attach( hdc );

		int cItems = m_menu.GetMenuItemCount();

		int mode = SetBkMode(dc.m_hDC, TRANSPARENT);
		for ( int i = 0; i < cItems ;i++ )
		{
			WTL::CRect rcItem;
			BOOL b = FALSE;
			if ( bClient )
			{
				b = GetMenuItemRect(NULL, m_menu.m_hMenu, i, &rcItem);
				if ( !b )
				{
					TRACE("rect is %d,%d,%d,%d\r\n", rcItem.left, rcItem.top, rcItem.right, rcItem.bottom );
					m_menu.GetMenuItemRect(m_hWnd, i, &rcItem);
					TRACE("rect is %d,%d,%d,%d\r\n", rcItem.left, rcItem.top, rcItem.right, rcItem.bottom );
					rcItem.OffsetRect( nLeft, nTop );
				}
			}
			else
			{
				m_menu.GetMenuItemRect(m_hWnd, i, &rcItem);
				TRACE("rect is %d,%d,%d,%d\r\n", rcItem.left, rcItem.top, rcItem.right, rcItem.bottom );
				rcItem.OffsetRect( nLeft, nTop );
			}
			
			ScreenToClient(&rcItem);
			TRACE("rect is %d,%d,%d,%d\r\n", rcItem.left, rcItem.top, rcItem.right, rcItem.bottom );
			
			// 0 判断状态
			int nState = 1;

			UINT  nType = GetMenuState( m_menu.m_hMenu, i, MF_BYPOSITION );
			if ( (nType & MF_DISABLED ) )
			{
				nState = 4;
			}
			else if ( (nType & MF_HILITE ) )
			{
				nState = 2;
			}

			if (_scheme)
				_scheme->DrawBackground(dc, _classid, 1, nState, &rcItem, NULL );

			// 1 判断类型
			MENUITEMINFO mii = { sizeof MENUITEMINFO, MIIM_TYPE };
			::GetMenuItemInfo (m_menu.m_hMenu, i, TRUE, &mii);
			
			if ( (mii.fType & MFT_SEPARATOR) )
			{
				int nSepHeight = GetSchemeHeight( 2, 1 );
				WTL::CRect rcSep = rcItem;
				rcSep.top = rcItem.top + ( rcItem.Height() - nSepHeight ) / 2 ;
				rcSep.bottom = rcSep.top + nSepHeight;
				if (_scheme)
					_scheme->DrawBackground(dc, _classid, 2, 1, &rcSep, NULL );
			}
			else
			{
				HFONT hOldFont = 0;
				WTL::CFont fontBold;
				// 判断是否是黑体
				if( (nType & MFS_DEFAULT) != 0 ) 
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
				if ( ( nType & MFS_CHECKED ) )
				{
					MENUITEMINFO   mbmp;   
					ZeroMemory(&mbmp,sizeof(mbmp));   
					mbmp.cbSize=sizeof(mbmp);   
					mbmp.fMask=MIIM_CHECKMARKS;
					::GetMenuItemInfo (m_menu.m_hMenu, i, TRUE, &mbmp);
					if ( mbmp.hbmpChecked )
					{
						hBitmap.Attach( mbmp.hbmpChecked );
					}
					else
					{
						if ( (mii.fType & MFT_RADIOCHECK) )
						{
							// todo raido
						}
					}
				}
				else if ( ( nType & MFS_UNCHECKED ) )
				{
					MENUITEMINFO mbmp = { sizeof MENUITEMINFO, MIIM_CHECKMARKS | MIIM_TYPE  };
					::GetMenuItemInfo (m_menu.m_hMenu, i, TRUE, &mbmp);
					if ( mbmp.hbmpUnchecked )
					{
						hBitmap.Attach( mbmp.hbmpUnchecked );
					}
				}
				else
				{
					MENUITEMINFO mbmp = { sizeof MENUITEMINFO, 0x00000080 | MIIM_TYPE  };
					::GetMenuItemInfo (m_menu.m_hMenu, i, TRUE, &mbmp);
					if ( mbmp.hbmpItem )
					{
						hBitmap.Attach( mbmp.hbmpItem );
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

				//else if ( (mii.fType & MFT_STRING ) || (mii.fType & MIIM_STRING ))
				const int string_size = m_menu.GetMenuString(i, 0, 0, MF_BYPOSITION);
				if (string_size > 0)
				{
					char* sz = new char[string_size + 1];
					int n = m_menu.GetMenuString(i, sz, string_size  + 1, MF_BYPOSITION);
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

						UINT uFlags = DT_SINGLELINE | DT_VCENTER;
						//uFlags |= DT_HIDEPREFIX;

						rcItem.left += _spaceText;
						rcItem.right -= _spaceText;

						dc.DrawText(sz, nTab, &rcItem, DT_LEFT | uFlags);
						if( nTab >= 0 ) 
							dc.DrawText(&sz[nTab + 1], -1, &rcItem, DT_RIGHT | uFlags);

						::SetTextColor( dc.m_hDC, clrOldText );

						//dc.DrawText(sz, n, (LPRECT)&rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
					delete [] sz;
				}	

				dc.SelectFont(hOldFont);

			}
		}

		
		SetBkMode(dc.m_hDC, mode);
		return;
	}


	void DoPrint( HDC hdc )
	{
		CRect rClient;
		GetClientRect( rClient );

		CRect rcw;
		GetWindowRect( rcw );
		rcw.OffsetRect( -rcw.left, -rcw.top );

		WTL::CDCHandle dc ;
		dc.Attach( hdc );

		if (_scheme)
			_scheme->DrawBackground(dc, _classid, 0, 1, &rcw, NULL );
		//dc.FillSolidRect( rcw, RGB( 255, 0, 5));

		DoPaint( dc.m_hDC, FALSE );
		return;
	}


	LRESULT OnEraseBkgnd( HDC hdc )
	{
		//DoPaint( hdc );

		return 1;
	}

	void OnPrintClient( HDC hdc, UINT lParam )
	{
		DoPaint( hdc );
	}


    void OnNcPaint(HRGN)
    {
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
    }


	void OnPaint(HDC)
    {
		
        WTL::CPaintDC paintdc(m_hWnd);

		DoPaint( paintdc.m_hDC );
		return;
    }


	void OnPrint( HDC wParam, UINT lParam )
	{

		DoPrint( wParam );
		return;

		if ( lParam & PRF_CLIENT  )
		{
			DoPrint( wParam );
		}
		else if ( lParam & PRF_NONCLIENT )
			DoNcPaint( wParam );
		else
			DoPrint( wParam );
	}

    void OnKeyDown(TCHAR, UINT, UINT)
    {
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

        return;
    }

    LRESULT OnSelectItem(int nUpdateItem)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        if (m_menu.IsNull())
            m_menu = GetHMenu();

		
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
    int GetCurrentSelectedIndex() const
    {
        int cItems = m_menu.GetMenuItemCount();
        int i;
        WTL::CMenuItemInfo mii;
        mii.fMask = MIIM_STATE;
        for (i=0;i<cItems;i++)
        {
            m_menu.GetMenuItemInfo(i, TRUE, &mii);
            if (mii.fState & MFS_HILITE)
                return i;
        }
        return -1;
    }
/*
    void InvalidItem(int idItem)
    {
		WTL::CRect rc;
		if ( m_menu.GetMenuItemCount() == 1 )
		{
			return;
			GetWindowRect(rc);
			rc.OffsetRect( -rc.left, -rc.top );
			InvalidateRect(&rc, TRUE);
			UpdateWindow();
			
		}
		
		//return;
        
        BOOL r = m_menu.GetMenuItemRect(m_hWnd, idItem, &rc);
		//r = m_menu.GetMenuItemRect(GetParent(), idItem, &rc);
		r = m_menu.GetMenuItemRect(NULL, idItem, &rc);
        ScreenToClient(&rc);

		TRACE("InvalidItem is %d,%d,%d,%d\r\n", rc.left, rc.top, rc.right, rc.bottom );
	
        InvalidateRect(&rc, FALSE);
    }
*/
private:
    int m_nUpdateItem;
    int m_nSelectedItem;
    WTL::CMenuHandle m_menu;
    BOOL m_fSysMenu;
    BOOL m_fPopup;
	BOOL	m_bFirstRedraw;
	BOOL	m_bAnimatedMenus;

	WTL::CFont _fontHorzMenu;
	int	 _spaceIcon;
	int  _spaceText;
};

}; // namespace
