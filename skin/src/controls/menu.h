#pragma once

#include "../base/skinctrl.h"


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

#define MN_SETHMENU              0x01E0
#define MN_GETHMENU              0x01E1
#define MN_SIZEWINDOW            0x01E2
#define MN_OPENHIERARCHY         0x01E3
#define MN_CLOSEHIERARCHY        0x01E4
#define MN_SELECTITEM            0x01E5
#define MN_CANCELMENUS           0x01E6
#define MN_SELECTFIRSTVALIDITEM  0x01E7

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

    SkinMenu() :
        m_nUpdateItem(-1)
    {}

    BEGIN_MSG_MAP(this_type)
        if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST) 
                && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
            TRACE("Menu: %08x %08x %08x\n", uMsg, wParam, lParam);
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_PAINT(OnPaint)
//        MSG_WM_PRINT()
//        MSG_WM_PRINTCLIENT()
        if (uMsg == MN_SELECTITEM)
        {
            SetMsgHandled(TRUE);
            lResult = OnSelectItem((int)wParam);
            if (IsMsgHandled())
                return TRUE;
        }
        // MSG_WM_KEYDOWN(OnKeyDown)
//        MSG_WM_NCCALCSIZE
//        MSG_WM_WINDOWPOSCHANGING
//        MSG_WM_ERASEBKGND
//        MSG_WM_CREATE(OnCreate)
    END_MSG_MAP()

    // TODO: µÃµ½ HMENU !!!
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

    int GetSelectItem()
    {
        return (int)SendMessage(MN_SELECTITEM, 0, 0);
    }

public:
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
        TRACE("lpcs->lpCreateParams : %x hmenu: %p, hwndParent: %p\n", lpcs->lpCreateParams, lpcs->hMenu, lpcs->hwndParent);

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

        LRESULT r = SendMessage(0x01E0, 0, 0);
        TRACE("MN_GETHMENU: %p\n", r);
        return 0;
    }

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
        // MN_1: 08F703F1
        // MN_2: 0036003E
        // MN_5: 00000080
//        for(int i=1; i<6; i++)
//            TRACE("MN_%d: %p\n", i, SendMessage(i + 0x01E0, 0, 0));

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
        int s1 = GetSelectItem();
        DefWindowProc();
        if (!::IsWindow(m_hWnd))
            return;
        int s2 = GetSelectItem();

        TRACE("OnKeyDown: %d %d\n", s1, s2);

        return;
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
    }

    LRESULT OnSelectItem(int nUpdateItem)
    {
        TRACE("OnMenuSelect: %d\n", nUpdateItem);
        SetRedraw(FALSE);
        LRESULT lr = DefWindowProc();
        SetRedraw(TRUE);

        if (m_nUpdateItem != nUpdateItem)
        {
            CMenuHandle hm = GetHMenu();
            if (!hm.IsNull())
            {
                // SIZE size_menu = GetSizeWindow();

                int c = hm.GetMenuItemCount();
                CRect rcc;
                GetClientRect(&rcc);

                int h = rcc.Height() / c;

                // new item
                if (nUpdateItem != -1)
                {
                    rcc.top = h * nUpdateItem;
                    rcc.bottom = rcc.top + h;

                    InvalidateRect(&rcc);
                }

                if (-1 != m_nUpdateItem) // old item
                {
                    rcc.top = h * m_nUpdateItem;
                    rcc.bottom = rcc.top + h;

                    InvalidateRect(&rcc);
                }
                
                m_nUpdateItem = nUpdateItem;
            }
        }
        return lr;
    }
private:
    int m_nUpdateItem;
};

}; // namespace 