#pragma once

#include "../base/skinctrl.h"


namespace Skin {

class WndTypeMenu : public CMenu
{
public:
};

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

    BEGIN_MSG_MAP(this_type)
        MSG_WM_NCPAINT(OnNcPaint)
//        MSG_WM_PRINT
//        MSG_WM_PRINTCLIENT
        MSG_WM_PAINT(OnPaint)
//        MSG_WM_KEYDOWN
//        MSG_WM_NCCALCSIZE
//        MSG_WM_WINDOWPOSCHANGING
//        MSG_WM_ERASEBKGND
    END_MSG_MAP()

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

        dcMem.FillSolidRect(&rcw, 0x00ff0000);

        // draw back
        int n = dc.SaveDC();
        // dc.ExcludeClipRect(&rcc);

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
    }
};

}; // namespace 