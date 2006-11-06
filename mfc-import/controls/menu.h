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

// 1 要让系统处理，MN_SELECTITEM,不然会死得很难看
// GetCurrentSelectedIndex

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
        , m_fSysMenu(FALSE)
        , m_fPopup(FALSE)
    {}

    BEGIN_MSG_MAP(this_type)
        ATLASSERT(::IsWindow(m_hWnd));
        //if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
        //        && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
        //    TRACE("Menu: %p %08x %08x %08x\n", m_hWnd, uMsg, wParam, lParam);

        // WM_PAINT 似乎是第一个设置了 HMENU 后的消息，还有一个更早的 MN_SIZEWINDOW，没有测试过
//        if (uMsg == WM_PAINT && m_menu.IsNull())
//        {
//            m_menu = GetHMenu();
//        }

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
            rcc.Width(), rcc.Height(), 
            GetSysColor(COLOR_MENU) // RGB(212, 208, 200)
            );

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

        if (m_nUpdateItem != nUpdateItem)
        {
            // new item
            if (nUpdateItem != -1)
                InvalidItem(nUpdateItem);

            // old item
            if (-1 != m_nUpdateItem)
                InvalidItem(m_nUpdateItem);

            m_nUpdateItem = nUpdateItem;
        }
        SetMsgHandled(FALSE);
        return 0;
    }

private:
    int GetCurrentSelectedIndex() const
    {
        int cItems = m_menu.GetMenuItemCount();
        int i;
        CMenuItemInfo mii;
        mii.fMask = MIIM_STATE;
        for (i=0;i<cItems;i++)
        {
            m_menu.GetMenuItemInfo(i, TRUE, &mii);
            if (mii.fState & MFS_HILITE)
                return i;
        }
        return -1;
    }

    void InvalidItem(int idItem)
    {
        CRect rc;
        BOOL r = m_menu.GetMenuItemRect(NULL, idItem, &rc);
        ScreenToClient(&rc);
        InvalidateRect(&rc);
    }

private:
    int m_nUpdateItem;
    int m_nSelectedItem;
    CMenuHandle m_menu;
    BOOL m_fSysMenu;
    BOOL m_fPopup;
};

}; // namespace
