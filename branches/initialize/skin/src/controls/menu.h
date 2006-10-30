#pragma once

#include "../base/skinctrl.h"

extern long FindItemIDThatOwnsThisMenu (HMENU hMenuOwned,HWND* phwndOwner,
                                        BOOL* pfPopup,BOOL *pfSysMenu);

namespace Skin {

//#ifndef MSG_MN_.............
//
//#define MSG_WM_NCMOUSELEAVE(func) \
//    if (uMsg == WM_NCMOUSELEAVE) \
//{ \
//    SetMsgHandled(TRUE); \
//    func(); \
//    lResult = 0; \
//    if (IsMsgHandled()) \
//    return TRUE; \
//}
//
//#endif


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
#endif


template<class BaseT = CWindow>
class SkinMenu : public SkinControlImpl<SkinMenu, BaseT>
{
public:
    enum { class_id = MENU };
    static LPCTSTR GetWndClassName()
    {
        return _T("#32768");
    }

    typedef SkinMenu<BaseT> this_type;
    typedef SkinControlImpl<SkinMenu, BaseT> base_type;

    SkinMenu()
        : m_nUpdateItem(-1)
        , m_nSelectedItem(-1)
        // , m_hWndOwner(0)
    {}

    BEGIN_MSG_MAP(this_type)
        if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST) 
                && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
            TRACE("Menu: %08x %08x %08x\n", uMsg, wParam, lParam);
//        MSG_WM_NCPAINT(OnNcPaint)
//        MSG_WM_PAINT(OnPaint)
//        MSG_WM_PRINT()
//        MSG_WM_PRINTCLIENT()
        if (uMsg == MN_SELECTITEM)
        {
            SetMsgHandled(TRUE);
            lResult = OnSelectItem((int)wParam);
            if (IsMsgHandled())
                return TRUE;
        }
        MSG_WM_KEYDOWN(OnKeyDown)
//        MSG_WM_NCCALCSIZE
//        MSG_WM_WINDOWPOSCHANGING
//        MSG_WM_ERASEBKGND
//        MSG_WM_CREATE(OnCreate)
    END_MSG_MAP()

private:
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

public:
#if 0
    LRESULT OnNcCreate(LPCREATESTRUCT lpcs)
    {
        TRACE("nc lpcs->lpCreateParams : %x hmenu: %p, hwndParent: %p\n", lpcs->lpCreateParams, lpcs->hMenu, lpcs->hwndParent);

        if (lpcs->hMenu)
        {
            CMenuHandle mh(lpcs->hMenu);
            int c = mh.GetMenuItemCount();
            TRACE("  count: %d\n", c);
            for (int i=0; i<c; ++i)
            {
                char sz[32] = {0};
                mh.GetMenuString(i, sz, 32, MF_BYPOSITION);
                TRACE("   %d %s\n", i, sz);
            }
        }
        SetMsgHandled(FALSE);
        return 0;
    }
    LRESULT OnCreate(LPCREATESTRUCT lpcs)
    {
        SetMsgHandled(FALSE);
        m_hMenu = GetHMenu(); // ! NOT WORKED
        return 0;
    }
#endif

    void OnNcPaint(HRGN)
    {
        CRect rcw, rcc;
        GetWindowRect(&rcw);
        GetClientRect(&rcc);
        ClientToScreen(&rcc);
        rcc.OffsetRect(-rcw.left, -rcw.top);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        CWindowDC dc(m_hWnd);

//        CRect rcclip;
//        dc.GetClipBox(&rcclip);

        // memory dc
        CDC dcMem;
        dcMem.CreateCompatibleDC(dc);
        ASSERT( dcMem );
        HBITMAP bmpMemBg = ::CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
        ASSERT( bmpMemBg );
        HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
        ASSERT( pOldBmp );

        // dcMem.FillSolidRect(&rcw, 0x0000ff00);

        CPen pen;
        pen.CreatePen(PS_SOLID, 1, GetSchemeColor(0, 0, TMT_MENU));
        HPEN old_pen = dcMem.SelectPen(pen);
        dcMem.Rectangle(rcw);
        dcMem.SelectPen(old_pen);


        // draw back
        int n = dc.SaveDC();
        dc.ExcludeClipRect(&rcc);

        ::BitBlt(dc, 0, 0, rcw.Width(), rcw.Height(), dcMem, rcw.left, rcw.top, SRCCOPY);
        
        dc.RestoreDC(n);

        // memory dc
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);
    }

    void OnPaint(HDC)
    {
        CPaintDC dc(m_hWnd);

        CRect rcc;
        GetClientRect(rcc);

        CDC dcm;
        dcm.CreateCompatibleDC(dc);
        CBitmap bg;
        bg.CreateCompatibleBitmap(dc, rcc.Width(), rcc.Height());
        HBITMAP oldbmp = dcm.SelectBitmap(bg);

        DefWindowProc(WM_ERASEBKGND, (WPARAM)(HDC)dcm, 0); // TODO: ??
        DefWindowProc(WM_PRINTCLIENT, (WPARAM)(HDC)dcm, PRF_ERASEBKGND | PRF_CLIENT);

        CDC dcw;
        dcw.CreateCompatibleDC(dc);
        CBitmap bgw;
        bgw.CreateCompatibleBitmap(dc, rcc.Width(), rcc.Height());
        HBITMAP oldbmpw = dcw.SelectBitmap(bgw);

        // draw my background
        dcw.FillSolidRect(rcc, RGB(0xff, 0, 0));

        // TransBlt dcm to dcw
        dcw.TransparentBlt(0, 0, rcc.Width(), rcc.Height(), dcm, 0, 0, 
            rcc.Width(), rcc.Height(), RGB(212, 208, 200));

#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcc.Width(), rcc.Height(), dcw, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif

        // draw back
        dc.BitBlt(0, 0, rcc.Width(), rcc.Height(), dcw, 0, 0, SRCCOPY);

        // release
        dcw.SelectBitmap(oldbmpw);
        dcm.SelectBitmap(oldbmp);
    }

    // func((TCHAR)wParam, (UINT)lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16));
    void OnKeyDown(TCHAR, UINT, UINT)
    {
        // TODO:
        // 0 call DefWindowProc
        // 1 GetCurSelectedItem
        // 2 update the item
        SetMsgHandled(FALSE);
        return;
        
#if 0
        if (s1 != -1 || s2 != -1)
        {
            CMenuHandle hm = GetHMenu();
            int c = hm.GetMenuItemCount();

            CRect rcc;
            GetClientRect(&rcc);

            int h = rcc.Height() / c;

            if (s1 != -1)
            {
                rcc.top = h * s1;
                rcc.bottom = rcc.top + h;

                InvalidateRect(&rcc);
            }

            if (s2 != -1)
            {
                rcc.top = h * s2;
                rcc.bottom = rcc.top + h;

                InvalidateRect(&rcc);
            }
        }
#endif
    }

    LRESULT OnSelectItem(int nUpdateItem)
    {
        if (m_nUpdateItem != nUpdateItem)
        {
            // new item
            if (nUpdateItem != -1)
                InvalidItem(nUpdateItem);

            if (-1 != m_nUpdateItem) // old item
                InvalidItem(m_nUpdateItem);
            
            m_nUpdateItem = nUpdateItem;
        }
        return 0;
    }

private:
    int GetCurSelectedItem()
    {
        int cItems = GetMenuItemCount (hMenu);
        int i;
        MENUITEMINFO mii;
        mii.cbSize = sizeof(MENUITEMINFO);
        for (i=0;i<cItems;i++)
        {
            ::GetMenuItemInfo(GetHMenu(), i, TRUE, &mii);
            if (mii.fState & MFS_HILITE)
                return i;
        }
        return -1;
    }

    void InvalidItem(int idItem)
    {
        // TODO: fPopup =0 & fSysMenu = 0
        HWND m_hWndOwner = 0;
        
        BOOL fPopup, fSysMenu;
        ::FindItemIDThatOwnsThisMenu(GetHMenu(), &m_hWndOwner, &fPopup, &fSysMenu);
        TRACE("%d-%d\n", fPopup, fSysMenu);

        ASSERT(m_hWndOwner);
        // ASSERT(fPopup || fSysMenu); // other menu ?
        LONG objid = fPopup ? OBJID_MENU : OBJID_SYSMENU;
        MENUBARINFO barinfo = {0};
        barinfo.cbSize = sizeof(MENUBARINFO);
        int r = GetMenuBarInfo(m_hWndOwner, objid, idItem + 1, &barinfo);
        ASSERT(r);

         TRACE("focus: %d (%d %d %d %d)", barinfo.fFocused, 
            barinfo.rcBar.left, barinfo.rcBar.top, barinfo.rcBar.right, barinfo.rcBar.bottom);

        ScreenToClient(&barinfo.rcBar);
        InvalidateRect(&barinfo.rcBar);
    }
    
private:
    int m_nUpdateItem;
    int m_nSelectedItem;
    // HWND m_hWndOwner;
};

}; // namespace 
