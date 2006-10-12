// non client area draw window

#pragma once

#include "SkinCtrl.h"

namespace Skin {

#ifndef MSG_WM_NCMOUSELEAVE

#define MSG_WM_NCMOUSELEAVE(func) \
    if (uMsg == WM_NCMOUSELEAVE) \
    { \
        SetMsgHandled(TRUE); \
        func(); \
        lResult = 0; \
        if (IsMsgHandled()) \
        return TRUE; \
    }

#endif


template<class ControlT, class WindowT = CWindow>
class SkinFrameImpl : public WindowT
{
protected:
    // replace by _frame_state;
//    enum ActiveState
//    {
//        _Active   = 1,
//        _Inactive = 2,
//        _Disable  = 3
//    };

    // 注意：在 wm_nccalcsize 消息前就要初始化
    SkinFrameImpl() 
        : _frame_state(FS_ACTIVE)
        , _fTrackNcMouseLeave(false) 
        , _anchorDown(0)
        , _anchorHover(0)
    {}

    // TODO: 似乎应该缓存 border 宽度作为类成员
    // 主要在 WM_NCCALCSIZE 时用到
    enum Metics
    {
        BorderThickness = 4,
    };

    // 共 12 种SysButton, 每种 6 种状态
    // tmschema.h 中只定义了4种，加入 0 程序中使用而已，5 表示窗口不处于active状态
    struct SystemButtonState
    {
        enum StateT
        {
            hidden  = 0,
            normal    = 1,
            hot    = 2,
            pushed    = 3,
            disabled = 4,
            inactive = 5
        };
        unsigned long _close    : 4;
        unsigned long _max        : 4;
        unsigned long _restore    : 4;
        unsigned long _min        : 4;
        unsigned long _help        : 4;

        SystemButtonState() : _close(hidden), _max(hidden), _restore(hidden), _min(hidden), _help(hidden) {}
        void initFromWindow(DWORD dwStyle, bool fWinActivate)
        {
            // close button always exist
            _close = normal;
            _restore = hidden;
            _max = hidden;

            // 窗口最小化了
            if (dwStyle & WS_MINIMIZE)
            {
                _min = hidden;
                _restore = normal;
            }
            else
            {
                if (dwStyle & WS_MINIMIZEBOX)
                    _min = normal;
            }

            // 最大化了
            if (dwStyle & WS_MAXIMIZE)
            {
                _restore = normal;
            }
            else
            {
                if (dwStyle & WS_MAXIMIZEBOX)
                    _max = normal;
            }

            if (hasmin() && !hasmax())
            {
                _max = disabled;
            }

            if (!fWinActivate)
            {
                if (hasclose())
                    _close = inactive;
                if (hasmax() && _max != disabled)
                    _max = inactive;
                if (hasrestore())
                    _restore = inactive;
                if (hasmin())
                    _min = inactive;
                if (hashelp())
                    _help = inactive;
            }
        }

        bool hasclose() const    { return(_close != hidden); }
        bool hasmax() const        { return(_max != hidden); }
        bool hasrestore() const { return(_restore != hidden); }
        bool hasmin() const        { return(_min != hidden); }
        bool hashelp() const    { return(_help != hidden); }
    }; // SystemButtonState


    int CaptionHeight()
    {
        ControlT * pT = static_cast<ControlT*>(this);
        return pT->GetSchemeSize(WP_CAPTION, CS_ACTIVE).cy;
    }

    // title    ?  _ []  X
    //
    // index:   3  2  1  0
    RECT CalcSysButtonRect(const CRect& rcw, int index)
    {
        ControlT * pT = static_cast<ControlT*>(this);
        CSize z = pT->GetSchemeSize(WP_SYSBUTTON);

        CRect rc = rcw;
        rc.top += BorderThickness;
        rc.bottom = rc.top + z.cy;

        rc.right = rc.right - (index + 1) * BorderThickness - index * z.cx;
        rc.left = rc.right - z.cx;
        return rc;
    }

    // 固定system button在 caption上的位置
    // TODO: 建立子元素 placement 的机制
    // 现在先使用 CalcSysButtonRect(index) 函数
#if 0
    RECT CalcCloseButtonRect()
    {
        return CalcSysButtonRect(0);
    }

    RECT CalcMinButtonRect()
    {
        return CalcSysButtonRect(2);
    }

    RECT CalcMaxButtonRect()
    {
        return CalcSysButtonRect(1);
    }
#endif

    // 计算 menu bar 高度
    // 1 MenuInfo
    // 2 SystemMetrics TODO: from skin.cfg
    int CalcMenuBarHeight()
    {
        HMENU h = GetMenu();
        ASSERT(h);
        MENUINFO mi;
        mi.cbSize = sizeof(MENUINFO);
        mi.fMask = MIM_MAXHEIGHT;
        GetMenuInfo(h, &mi);

        if (0 == mi.cyMax)
            return GetSystemMetrics(SM_CYMENU);
        else
            return mi.cyMax;
    }


    // DrawFrame = Draw nonClient area
    // 1 draw border
    // 2 draw caption, include system button
    // 3 include resize anchor
    // 所有坐标相对于本窗口
    void DrawFrame(HDC hdc, CRect& rcw, CRect& rcc, DWORD dwStyle, FRAMESTATES _frame_state)
    {
        _ASSERTE(_CrtCheckMemory());

        CDC dc(hdc);
        // exclude the client area
        // ! The lower and right edges of the specified rectangle are not excluded from the clipping region.
        int nRet = ::ExcludeClipRect(dc, rcc.left, rcc.top, rcc.right, rcc.bottom);

        // memory dc
        HDC dcMem = ::CreateCompatibleDC(dc);
        ASSERT(dcMem);
        HBITMAP bmpMemBg = ::CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
        ASSERT(bmpMemBg);
        HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
        ASSERT(pOldBmp);

        DrawFrameBorder(dcMem, rcw, _frame_state);

        SystemButtonState sysbtn_state;
        sysbtn_state.initFromWindow(dwStyle, _frame_state == FS_ACTIVE);
        CAPTIONSTATES caption_state = ((_frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE);
        
        DrawCaption(dcMem, rcw, dwStyle, caption_state, sysbtn_state);

        if (GetMenu())
        {
            ControlT * pT = static_cast<ControlT*>(this);

            CAPTIONSTATES caption_state = (_frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE;
            int caption_height = pT->GetSchemeHeight(WP_CAPTION, caption_state);

            int bottom_height = pT->GetSchemeHeight(WP_FRAMEBOTTOM, _frame_state);
            int border_left_width = pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);
            int border_right_width = pT->GetSchemeWidth(WP_FRAMERIGHT, _frame_state);

            CRect rc = rcw;
            rc.top += caption_height;
            rc.left += border_left_width;
            rc.right -= border_right_width;
            rc.bottom = rc.top + CalcMenuBarHeight();
            DrawMenuBar(dcMem, rc);
        }

#if 1
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width(), rcw.Height(), dcMem, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif

        // memory dc
        ::BitBlt(dc, 0, 0, rcw.Width(), rcw.Height(), dcMem, rcw.left, rcw.top, SRCCOPY);
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);

        nRet = ::ExcludeClipRect(dc, 0, 0, 0, 0);
        ASSERT(ERROR != nRet);
    }

    void DrawMenuBar(HDC hdc, CRect& rc)
    {
        CMenuHandle menu(GetMenu());
        ASSERT(!menu.IsNull());

        CDCHandle dc(hdc);
        int r = dc.SaveDC();

        // xp only
        BOOL flat_menu = FALSE;
        SystemParametersInfoW (SPI_GETFLATMENU, 0, &flat_menu, 0);

        // font
        CFont fontMenu;
        LOGFONT logFontMenu;

        NONCLIENTMETRICS nm = {0};
        nm.cbSize = sizeof (NONCLIENTMETRICS);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0);
        logFontMenu = nm.lfMenuFont;

        fontMenu.CreateFontIndirect(&logFontMenu);
        dc.SelectFont(fontMenu);

        int c = menu.GetMenuItemCount();
        MENUBARINFO barinfo = {0};
        barinfo.cbSize = sizeof(MENUBARINFO);
        
        // TRACE("menu %p\n", menu.m_hMenu);
        TraceRect("calc ", &rc);

        // bar selft
        ::GetMenuBarInfo(m_hWnd, OBJID_MENU, 0, &barinfo);
        // TRACE("bar %p\n", barinfo.hMenu);
        TraceRect("bar ", &barinfo.rcBar);
        ControlT * pT = static_cast<ControlT*>(this);
        COLORREF cr = pT->GetSchemeColor(WP_MENUBAR, 0, TMT_MENU);
        CBrush br;

        br.CreateSolidBrush(cr);
        dc.FillRect(&rc, br);

        CRect rcw;
        GetWindowRect(&rcw);

        // bar item
        for (int i=1; i<=c; ++i)
        {
            ::GetMenuBarInfo(m_hWnd, OBJID_MENU, i, &barinfo);
            // TRACE("item %p\n", barinfo.hMenu);
            
            OffsetRect(&barinfo.rcBar, -rcw.left, -rcw.top);
            TraceRect("item ", &barinfo.rcBar);
            // DrawMenuBarItem(dc, menu, i, 0);
            char sz[0x16];
            int n = menu.GetMenuString(i - 1, sz, 16, MF_BYPOSITION);
            dc.DrawText(sz, n, &barinfo.rcBar, DT_LEFT);
        }

        // release
        dc.RestoreDC(r);
    }

    void DrawMenuBarItem(CDCHandle dc, CMenuHandle menu, UINT nItemIndex, UINT nState)
    {
        CRect itemRect;
        if (nItemIndex!=UINT(-1) && GetMenuItemRect(m_hWnd, menu, nItemIndex, &itemRect))
        {
            MENUITEMINFO menuInfo = {0};
            menuInfo.cbSize = sizeof(menuInfo);
            menuInfo.fMask = MIIM_DATA|MIIM_TYPE|MIIM_ID;
            if (menu.GetMenuItemInfo(nItemIndex, TRUE, &menuInfo))
            {
                if(menuInfo.fType&MF_OWNERDRAW)
                {
                    DebugBreak();
                }
            }
        }
    }


    // TODO: 是否需要 windows style 呢？
    void DrawFrameBorder(HDC hdc, CRect& rcw, FRAMESTATES _frame_state)
    {
        // FS_INACTIVE FS_ACTIVE
        ASSERT(hdc);
        ControlT * pT = static_cast<ControlT*>(this);

        // 左右border宽度保持一致，可能下边高度会不一样
        // 1 Caption Height, Border Width
        CAPTIONSTATES caption_state = (_frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE;
        int caption_height = pT->GetSchemeHeight(WP_CAPTION, caption_state);

        int bottom_height = pT->GetSchemeHeight(WP_FRAMEBOTTOM, _frame_state);
        int border_left_width = pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);
        int border_right_width = pT->GetSchemeWidth(WP_FRAMERIGHT, _frame_state);

        // 2 Left
        CRect rc(rcw.left, rcw.top + caption_height,
            rcw.left + border_left_width, rcw.bottom - bottom_height);
        pT->Draw(hdc, WP_FRAMELEFT, _frame_state, rc.left, rc.top, 0, rc.Height());

        // Right
        rc.left = rcw.right - border_right_width;
        rc.right = rcw.right;
        pT->Draw(hdc, WP_FRAMERIGHT, _frame_state, rc.left, rc.top, 0, rc.Height());

        // Bottom
        rc.left = rcw.left;
        rc.top = rcw.bottom - bottom_height;
        rc.bottom = rcw.bottom;
        pT->Draw(hdc, WP_FRAMEBOTTOM, _frame_state, rc.left, rc.top, rc.Width(), 0);

#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width() + 10, rcw.Height() + 10, hdc, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif
    }

    void DrawCaption(HDC hdc, CRect& rcw, DWORD dwStyle, CAPTIONSTATES caption_state,
        SystemButtonState& sysbtn_state)
    {
        ASSERT(hdc);
        ControlT * pT = static_cast<ControlT*>(this);

        pT->Draw(hdc, WP_CAPTION, caption_state, rcw.left, rcw.top, rcw.Width(), 0);
        DrawSysButton(hdc, rcw, sysbtn_state, dwStyle);
#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width() + 10, rcw.Height() + 10, hdc, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif
    }

    void DrawSysButton(HDC hdc, const CRect& rcw, const SystemButtonState& sysbtn_state, DWORD dwStyle)
    {
        ASSERT(hdc);
        ControlT * pT = static_cast<ControlT*>(this);

        // 1 SysButton 区域底图，如果没有可能会造成问题
        // WP_SYSBUTTON 的意思是啥？ 能作为SysButton区域的底图吗？
        RECT rc;
        BOOL bRet = FALSE;

        // CalcXXXButtonRect 换成 CalcSysButtonRect(int index)，代码逻辑简单了很多
        // index 重右到左的序号

        int count_sys_button = 0;

        // 2 Button 表面
        if (sysbtn_state.hasclose())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(hdc, WP_CLOSEBUTTON, sysbtn_state._close, rc.left, rc.top);
            ATLASSERT(bRet);
        }

        if (sysbtn_state.hasmax())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(hdc, WP_MAXBUTTON, sysbtn_state._max, rc.left, rc.top);
            ATLASSERT(bRet);
        }

        if (sysbtn_state.hasrestore())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(hdc, WP_RESTOREBUTTON, sysbtn_state._restore, rc.left, rc.top);
            ATLASSERT(bRet);
        }

        if (sysbtn_state.hasmin())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(hdc, WP_MINBUTTON, sysbtn_state._min, rc.left, rc.top);
            ATLASSERT(bRet);
        }

        if (sysbtn_state.hashelp())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(hdc, WP_HELPBUTTON, sysbtn_state._help, rc.left, rc.top);
            ATLASSERT(bRet);
        }
    }

    // 使用memory DC, 只绘制几个button部分
    void EtchedSysButton(CDCHandle dc, const CRect& rcw, const SystemButtonState& sysbtn_state)
    {
        // 先把图绘制在 dcmem 上，然后再绘制到 dc 上
        CDC dcmem;
        dcmem.CreateCompatibleDC(dc);
        CBitmap bmpbg;
        bmpbg.CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
        HBITMAP bmpold = dcmem.SelectBitmap(bmpbg);

        ControlT * pT = static_cast<ControlT*>(this);

        CRect rc;
        BOOL bRet = FALSE;

        // CalcXXXButtonRect 换成 CalcSysButtonRect(int index)，代码逻辑简单了很多
        // index 重右到左的序号

        int count_sys_button = 0;

        // 2 Button 表面
        if (sysbtn_state.hasclose())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(dcmem, WP_CLOSEBUTTON, sysbtn_state._close, rc.left, rc.top);
            ATLASSERT(bRet);

            dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcmem, rc.left, rc.top, SRCCOPY);
        }

        if (sysbtn_state.hasmax())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(dcmem, WP_MAXBUTTON, sysbtn_state._max, rc.left, rc.top);
            ATLASSERT(bRet);

            dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcmem, rc.left, rc.top, SRCCOPY);
        }

        if (sysbtn_state.hasrestore())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(dcmem, WP_RESTOREBUTTON, sysbtn_state._restore, rc.left, rc.top);
            ATLASSERT(bRet);

            dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcmem, rc.left, rc.top, SRCCOPY);
        }

        if (sysbtn_state.hasmin())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(dcmem, WP_MINBUTTON, sysbtn_state._min, rc.left, rc.top);
            ATLASSERT(bRet);

            dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcmem, rc.left, rc.top, SRCCOPY);
        }

        if (sysbtn_state.hashelp())
        {
            rc = CalcSysButtonRect(rcw, count_sys_button++);
            bRet = pT->Draw(dcmem, WP_HELPBUTTON, sysbtn_state._help, rc.left, rc.top);
            ATLASSERT(bRet);

            dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcmem, rc.left, rc.top, SRCCOPY);
        }

        // CWindowDC dct(0);
        // dct.BitBlt(0, 0, rcw.Width(), rcw.Height(), dcmem, 0, 0, SRCCOPY);

        // 
        dcmem.SelectBitmap(bmpold);
    }

    BEGIN_MSG_MAP_EX(SkinFrameImpl)
        MSG_WM_NCACTIVATE(OnNcActivate)
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_NCCALCSIZE(OnNcCalcSize)
#if 0
        MSG_WM_NCHITTEST(OnNcHitTest)
#else
        if (uMsg == WM_NCHITTEST) 
        {
            SetMsgHandled(TRUE); 
            lResult = (LRESULT)OnNcHitTest(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); 
            if(IsMsgHandled()) 
            {
                ATLTRACE("nchit: %d\n", lResult);
                return TRUE; 
            }
        }
#endif
        MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
        MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
        MSG_WM_NCMOUSELEAVE(OnNcMouseLeave)
        MSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
        MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
    END_MSG_MAP()

    BOOL OnNcActivate(BOOL bActive)
    {
        _frame_state = bActive ? FS_ACTIVE : FS_INACTIVE;

        CWindowDC dc(m_hWnd);

        CRect rcw, rcc;
        GetWindowRect(&rcw);
        GetClientRect(&rcc);
        ClientToScreen(&rcc);
        rcc.OffsetRect(-rcw.left, -rcw.top);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        DrawFrame(dc, rcw, rcc, GetStyle(), _frame_state);
        return TRUE;
    }

    void TraceRect(const char* name, RECT* prc)
    {
        TRACE("%s (%d,%d)-(%d,%d)\n", name, prc->left, prc->top, prc->right, prc->bottom);
    }

    LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
    {
        /*
        // MSDN 2001 ------------------------------------------
        if bValidateClient is 1
        #1 Rect[0] is the proposed new client position

        #2 Rect[1] is the source rectangle or the current window in case you want to
        preserve the graphics that are already drawn there.

        #3 Rect[2] is the destination rectangle where the source graphics will be
        copied to. If this rectangle is a different size to the source the top and
        left wil be copied but the graphics will be clipped, not resized. You can
        for example copy only a relavent subset of the current client to the new
        place.


        // MSDN January 2006 --------------------------------------
        Specifies an array of rectangles. 
        The first contains the new coordinates of a window that has been moved or resized, 
        that is, it is the proposed new window coordinates. 

        The second contains the coordinates of the window before it was moved or resized. 

        The third contains the coordinates of the window's client area before the window was 
        moved or resized. If the window is a child window, the coordinates are relative to 
        the client area of the parent window. If the window is a top-level window, the 
        coordinates are relative to the screen origin. 

        // TODO: read <<Windows_Graphics_Programming;_Win32_GDI_and_DirectDraw_(2000)>>
        //       find out which value should return!!!
        */

#if 1
        NCCALCSIZE_PARAMS FAR* lpncsp_ = (NCCALCSIZE_PARAMS *)lParam;
        CRect rc1 = lpncsp_->rgrc[0];
        
        ControlT * pT1 = static_cast<ControlT*>(this);
        LRESULT ret = pT1->DefWindowProc();
        CRect rc2 = lpncsp_->rgrc[0];
        RECT & new_rcc = lpncsp_->rgrc[0];
        
    //    TraceRect("before", &rc1);
    //    TraceRect("after ", &rc2);
    //    TRACE("return %d\n", ret);

        int c_top = CaptionHeight();
        if (GetMenu())
            c_top += CalcMenuBarHeight();

        int c_left = 0, c_right = 0, c_bottom = 0;

        if (GetStyle() & WS_BORDER)
        {
            ControlT * pT = static_cast<ControlT*>(this);

            c_left = pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);
            c_right = pT->GetSchemeWidth(WP_FRAMERIGHT, _frame_state);
            c_bottom = pT->GetSchemeHeight(WP_FRAMEBOTTOM, _frame_state);
        }

     //   TRACE("l  : %d - %d\n", rc2.left, rc1.left + c_left);
     //   TRACE("top: %d - %d\n", rc2.top, rc1.top + c_top);
        
     //   TRACE("r  : %d - %d\n", rc2.right, rc1.right - c_right);
     //   TRACE("b  : %d - %d\n\n", rc2.bottom, rc1.bottom - c_bottom);

        new_rcc.left = rc1.left + c_left;
        new_rcc.top = rc1.top + c_top;
        new_rcc.right = rc1.right - c_right;
        new_rcc.bottom = rc1.bottom - c_bottom;

        return ret;
#else
        // If wParam is FALSE, lParam points to a RECT structure. On entry, the structure contains 
        // the proposed window rectangle for the window. On exit, the structure should contain the 
        // screen coordinates of the corresponding window client area.
        if (!bCalcValidRects)
        {
            NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS *)lParam;
            RECT& new_rcc = lpncsp->rgrc[0];
            
            CRect rcw;
            GetWindowRect(&rcw);

            new_rcc = rcw;

            new_rcc.top = rcw.top + CaptionHeight();

            if (GetMenu())
            {
                new_rcc.top += CalcMenuBarHeight();
            }

            if (GetStyle() & WS_BORDER)
            {
                ControlT * pT = static_cast<ControlT*>(this);

                new_rcc.left += pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);
                new_rcc.right -= pT->GetSchemeWidth(WP_FRAMERIGHT, _frame_state);
                new_rcc.bottom -= pT->GetSchemeHeight(WP_FRAMEBOTTOM, _frame_state);
            }
            return 0;
        }
        else
        {
            if (!IsIconic())
            {
                TRACE("wParam=%d IsIconic\n", bCalcValidRects);
            }
            return 0;
        }
#endif

#if 0 // wine implement
        NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS *)lParam;
        RECT* winRect = &lpncsp->rgrc[0];

        RECT tmpRect = { 0, 0, 0, 0 };
        LRESULT result = 0;
        LONG cls_style = GetClassLong(m_hWnd, GCL_STYLE);
        LONG style = GetWindowLong(GWL_STYLE);
        LONG exStyle = GetWindowLong(GWL_EXSTYLE);

        if (cls_style & CS_VREDRAW) result |= WVR_VREDRAW;
        if (cls_style & CS_HREDRAW) result |= WVR_HREDRAW;

        if (!IsIconic())
        {
            NC_AdjustRectOuter(&tmpRect, style, FALSE, exStyle);

            winRect->left   -= tmpRect.left;
            winRect->top    -= tmpRect.top;
            winRect->right  -= tmpRect.right;
            winRect->bottom -= tmpRect.bottom;

            if (((style & (WS_CHILD | WS_POPUP)) != WS_CHILD) && GetMenu())
            {
                TRACE("Calling GetMenuBarHeight with hwnd %p, width %ld, at (%ld, %ld).\n",
                    m_hWnd, winRect->right - winRect->left, -tmpRect.left, -tmpRect.top);

                winRect->top += CalcMenuBarHeight();
//                    MENU_GetMenuBarHeight(hwnd,
//                    winRect->right - winRect->left,
//                    -tmpRect.left, -tmpRect.top);
            }

            if(exStyle & WS_EX_CLIENTEDGE)
                if(winRect->right - winRect->left > 2 * GetSystemMetrics(SM_CXEDGE) &&
                    winRect->bottom - winRect->top > 2 * GetSystemMetrics(SM_CYEDGE))
                    InflateRect(winRect, - GetSystemMetrics(SM_CXEDGE),
                    - GetSystemMetrics(SM_CYEDGE));

            if (style & WS_VSCROLL)
                if(winRect->right - winRect->left >= GetSystemMetrics(SM_CXVSCROLL)){
                    if((exStyle & WS_EX_LEFTSCROLLBAR) != 0)
                        winRect->left  += GetSystemMetrics(SM_CXVSCROLL);
                    else
                        winRect->right -= GetSystemMetrics(SM_CXVSCROLL);
                }

                if (style & WS_HSCROLL)
                    if(winRect->bottom - winRect->top > GetSystemMetrics(SM_CYHSCROLL))
                        winRect->bottom -= GetSystemMetrics(SM_CYHSCROLL);

                if (winRect->top > winRect->bottom)
                    winRect->bottom = winRect->top;

                if (winRect->left > winRect->right)
                    winRect->right = winRect->left;
        }
        return result;
#endif
    }

    void NC_AdjustRectOuter (LPRECT rect, DWORD style, BOOL menu, DWORD exStyle)
    {
        int adjust;
        if(style & WS_ICONIC) return;

        if ((exStyle & (WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)) ==
            WS_EX_STATICEDGE)
        {
            adjust = 1; /* for the outer frame always present */
        }
        else
        {
            adjust = 0;
            if ((exStyle & WS_EX_DLGMODALFRAME) ||
                (style & (WS_THICKFRAME|WS_DLGFRAME))) adjust = 2; /* outer */
        }
        if (style & WS_THICKFRAME)
            adjust +=  (GetSystemMetrics (SM_CXFRAME)
            - GetSystemMetrics (SM_CXDLGFRAME)); /* The resize border */
        if ((style & (WS_BORDER|WS_DLGFRAME)) ||
            (exStyle & WS_EX_DLGMODALFRAME))
            adjust++; /* The other border */

        InflateRect (rect, adjust, adjust);

        if ((style & WS_CAPTION) == WS_CAPTION)
        {
            if (exStyle & WS_EX_TOOLWINDOW)
                rect->top -= GetSystemMetrics(SM_CYSMCAPTION);
            else
                rect->top -= GetSystemMetrics(SM_CYCAPTION);
        }
        if (menu) rect->top -= GetSystemMetrics(SM_CYMENU);
    }


    UINT OnNcHitTest(CPoint point)
    {
        // 所有的计算都是相对于本窗口的坐标,所以先转换
        CRect rcw, rcc;
        GetWindowRect(&rcw);

        point.x -= rcw.left;
        point.y -= rcw.top;

        GetClientRect(&rcc);
        ClientToScreen(&rcc);
        rcc.OffsetRect(-rcw.left, -rcw.top);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        // 四个大块
        // 1 border
        // 2 caption
        // 3 menu
        // 4 client

        DWORD dwStyle = GetStyle();
        ControlT * pT = static_cast<ControlT*>(this);

        if (rcc.PtInRect(point))
            return HTCLIENT;

        CRect rc_caption = pT->GetSchemeRect(WP_CAPTION, _frame_state);

        int bottom_height = pT->GetSchemeHeight(WP_FRAMEBOTTOM, _frame_state);        
        int border_left_width = pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);
        int border_right_width = pT->GetSchemeWidth(WP_FRAMERIGHT, _frame_state);

        BOOL can_resize = (WS_THICKFRAME & dwStyle);
        
        // left
        if (point.x < bottom_height)
        {
            if (!can_resize)
                return HTBORDER;

            if (point.y < bottom_height)
                return HTTOPLEFT;
            else if (point.y > rcw.bottom - bottom_height)
                return HTBOTTOMLEFT;
            else 
                return HTLEFT;
        }

        // right
        else if (point.x >= rcw.right - border_right_width)
        {
            if (!can_resize)
                return HTBORDER;

            if (point.y < bottom_height)
                return HTTOPRIGHT;
            else if (point.y > rcw.bottom - bottom_height)
                return HTBOTTOMRIGHT;
            else 
                return HTRIGHT;
        }

        // bottom
        else if (point.y >= rcw.bottom - bottom_height)
        {
            if (!can_resize)
                return HTBORDER;

            return HTBOTTOM;
        }

        // top
        else if(point.y < bottom_height)
        {
            if (!can_resize)
                return HTBORDER;

            return HTTOP;
        }

        rc_caption.right = rcw.right - border_right_width;
        rc_caption.left = rcw.left + border_left_width;
        rc_caption.top = rcw.top + bottom_height;

        // caption
        // rc_caption.DeflateRect(border_left_width, bottom_height, border_right_width, 0);
        if (rc_caption.PtInRect(point))
        {
            // system button
            int index = -1;
            for (int i=0; i<4; i++)
            {
                CRect rc = CalcSysButtonRect(rcw, i);
                if (rc.PtInRect(point))
                    index = i;
            }

            if (index != -1)
            {
                SystemButtonState sysbtn_state;
                sysbtn_state.initFromWindow(dwStyle, _frame_state == FS_ACTIVE);

                if (index == 0 && sysbtn_state.hasclose())
                    return HTCLOSE;

                if (index == 1 && (sysbtn_state.hasmax() || sysbtn_state.hasrestore()))
                    return HTMAXBUTTON;

                if ((index == 1 || index == 2) && sysbtn_state.hasmin())
                    return HTMINBUTTON;

                if ((index == 1 || index == 2 || index == 3) 
                        && sysbtn_state.hashelp())
                    return HTHELP;
            }

            return HTCAPTION;
        }

        if (GetMenu())
        {
            int bar_height = CalcMenuBarHeight();
            rc_caption.top = rc_caption.bottom;
            rc_caption.bottom += bar_height;
            if (rc_caption.PtInRect(point))
                return HTMENU; // TODO: 
        }

        ASSERT(false);
        return HTNOWHERE;
        // HTBORDER In the border of a window that does not have a sizing border.
        //  HTBOTTOM In the lower-horizontal border of a resizable window (the user can click the mouse to resize the window vertically).
        //  HTBOTTOMLEFT In the lower-left corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).
        //  HTBOTTOMRIGHT In the lower-right corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).
        //  HTCAPTION In a title bar.
        //  HTCLIENT In a client area.
        //  HTCLOSE In a Close button.
        // HTERROR On the screen background or on a dividing line between windows (same as HTNOWHERE, except that the DefWindowProc function produces a system beep to indicate an error).
        // HTGROWBOX In a size box (same as HTSIZE).
        // HTHELP In a Help button.
        // HTHSCROLL In a horizontal scroll bar.
        //  HTLEFT In the left border of a resizable window (the user can click the mouse to resize the window horizontally).
        // HTMENU In a menu.
        //  HTMAXBUTTON In a Maximize button.
        //  HTMINBUTTON In a Minimize button.
        // HTNOWHERE On the screen background or on a dividing line between windows.
        // HTREDUCE In a Minimize button.
        //  HTRIGHT In the right border of a resizable window (the user can click the mouse to resize the window horizontally).
        // HTSIZE In a size box (same as HTGROWBOX).
        // HTSYSMENU In a window menu or in a Close button in a child window.
        //  HTTOP In the upper-horizontal border of a window.
        //  HTTOPLEFT In the upper-left corner of a window border.
        //  HTTOPRIGHT In the upper-right corner of a window border.
        // HTTRANSPARENT In a window currently covered by another window in the same thread (the message will be sent to underlying windows in the same thread until one of them returns a code that is not HTTRANSPARENT).
        // HTVSCROLL In the vertical scroll bar.
        // HTZOOM In a Maximize button.
    }
    // TODO: Read http://www.codeproject.com/menu/newmenuxpstyle.asp

    void OnNcPaint(HRGN)
    {
        CWindowDC dc(m_hWnd);

        CRect rcw, rcc;

        GetWindowRect(&rcw);
        GetClientRect(&rcc);

        ClientToScreen(&rcc);
        rcc.OffsetRect(-rcw.left, -rcw.top);

        rcw.OffsetRect(-rcw.left, -rcw.top);

        DrawFrame(dc, rcw, rcc, GetStyle(), _frame_state);
    }

    BOOL OnNcMouseMove(UINT nHitTest, CPoint point)
    {
        if(nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE)
        {
            if(_anchorDown && _anchorDown != nHitTest)
                _anchorDown = 0;			

            if(_anchorHover != nHitTest)
                _anchorHover = nHitTest;

            if(!_fTrackNcMouseLeave)
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_NONCLIENT | TME_LEAVE;			
                tme.hwndTrack = m_hWnd;
                tme.dwHoverTime = HOVER_DEFAULT;
                _TrackMouseEvent(&tme);
                _fTrackNcMouseLeave = true;		
            }
        }		
        else
        {
            return FALSE;
        }

        // 使用标志，简直是搬起石头砸自己的头
        if(GetStyle() & WS_DLGFRAME) // 是否有 TitleBar
        {
            SystemButtonState sbState;
            DWORD dwStyle = GetStyle();
            sbState.initFromWindow(dwStyle, (_frame_state == FS_ACTIVE));
            if(HTCLOSE == nHitTest)
                sbState._close = SystemButtonState::hot;
            else if(HTMAXBUTTON == nHitTest)
            {
                if(dwStyle & WS_MAXIMIZE)
                    sbState._restore = SystemButtonState::hot;
                else
                    sbState._max = SystemButtonState::hot;
            }
            else if(HTMINBUTTON == nHitTest)
            {
                if(sbState._min != SystemButtonState::hidden)
                    sbState._min = SystemButtonState::hot;
            }

            CWindowDC dc(m_hWnd);
            CRect rcw;
            GetWindowRect(&rcw);
            rcw.OffsetRect(-rcw.left, -rcw.top);
            EtchedSysButton((HDC)dc, rcw, sbState);
        }
        
        if(HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest)
        {
            return FALSE;
            //CallWindowProc(gDialogProc, hWnd, WM_NCMOUSEMOVE, (WPARAM)(UINT)(nHitTest), MAKELPARAM((x), (y)));
        }
        return TRUE;
    }
    
    void OnNcLButtonDown(UINT nHitTest, CPoint point)
    {
        if (nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON 
            || nHitTest == HTCLOSE || nHitTest == HTHELP)
        {
            _anchorDown  = nHitTest;
            _anchorHover = nHitTest;
            
        }
        else
        {
            _anchorDown  = 0;
            _anchorHover = 0;
        }

        if (GetStyle() & WS_DLGFRAME)
        {
            SystemButtonState sysbtn_state;
            sysbtn_state.initFromWindow(GetStyle(), (_frame_state == FS_ACTIVE));
            if (HTCLOSE == nHitTest)
                sysbtn_state._close = SystemButtonState::pushed;
            else if (HTMAXBUTTON == nHitTest)
            {
                if (GetStyle() & WS_MAXIMIZE)
                    sysbtn_state._restore = SystemButtonState::pushed;
                else
                    sysbtn_state._max = SystemButtonState::pushed;
            }
            else if (HTMINBUTTON == nHitTest)
            {
                if (GetStyle() & WS_MINIMIZE)
                    sysbtn_state._restore = SystemButtonState::pushed;
                else
                    sysbtn_state._min = SystemButtonState::pushed;
            }

#if 1
            CWindowDC dc(m_hWnd);
            CRect rcw;
            GetWindowRect(&rcw);
            rcw.OffsetRect(-rcw.left, -rcw.top);
#else
            CWindowDC dc(0);
            CRect rcw(0, 40, 200, 80);
#endif
            EtchedSysButton((HDC)dc, rcw, sysbtn_state);
        }
        if (HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest)
        {
            SetMsgHandled(FALSE);
            //CallWindowProc(gDialogProc, hWnd, WM_NCLBUTTONDOWN, (WPARAM)(UINT)(nHitTest),
             // MAKELPARAM((x), (y)));
        }
    }
    
    void OnNcLButtonUp(UINT nHitTest, CPoint point)
    {
        if (0 == _anchorDown)
            return;

        SetMsgHandled(FALSE);
        return;

        ControlT * pT = static_cast<ControlT*>(this);
        pT->DefWindowProc();

        if (HTMAXBUTTON == nHitTest && (GetStyle() & WS_MAXIMIZEBOX))
        {
            // ShowWindow(SW_MAXIMIZE);
            if (IsZoomed())
                SendMessage(WM_SYSCOMMAND, SC_RESTORE, -1);
            else
                SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, -1);
        }
        else if (HTMINBUTTON == nHitTest)
        {
            if (IsIconic())
                SendMessage(WM_SYSCOMMAND, SC_RESTORE, -1);
            else
                SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, -1);
        }
        else if (HTCLOSE == nHitTest)
            PostMessage(WM_SYSCOMMAND, SC_CLOSE, -1);
    }

    void OnNcMouseLeave()
    {
        _fTrackNcMouseLeave = false;
        _anchorHover = 0;

        SystemButtonState sbState;
        sbState.initFromWindow(GetStyle(), (_frame_state == FS_ACTIVE));
        sbState._close = SystemButtonState::normal;
        {
            if (GetStyle() & WS_MAXIMIZE)
                sbState._restore = SystemButtonState::normal;
            else
                sbState._max = SystemButtonState::normal;
        }
        {
            if (GetStyle() & WS_MINIMIZE)
                sbState._restore = SystemButtonState::normal;
            else
                sbState._min = SystemButtonState::normal;
        }

        CWindowDC dc(m_hWnd);
        CRect rcw;
        GetWindowRect(&rcw);
        rcw.OffsetRect(-rcw.left, -rcw.top);
        EtchedSysButton((HDC)dc, rcw, sbState);
    }

    BOOL OnNcLButtonDblClk(UINT nHitTest, CPoint point)
    {
        if (HTCAPTION == nHitTest)
        {
            HICON hIcon = (HICON)SendMessage(WM_GETICON, ICON_SMALL, 0);
            CPoint pt = point;
            ScreenToClient(&pt);
            if (hIcon && pt.x < 16 + 9)
            {
                SendMessage(WM_SYSCOMMAND, SC_CLOSE, -1);
                return TRUE;
            }
        }
        return FALSE;
    }

    
    LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hWndChild = NULL;

        switch(uMsg)
        {
        case WM_COMMAND:
            if(lParam != NULL)	// not from a menu
                hWndChild = (HWND)lParam;
            break;
        case WM_NOTIFY:
            hWndChild = ((LPNMHDR)lParam)->hwndFrom;
            break;
        case WM_PARENTNOTIFY:
            switch(LOWORD(wParam))
            {
            case WM_CREATE:
            case WM_DESTROY:
                hWndChild = (HWND)lParam;
                break;
            default:
                hWndChild = GetDlgItem(HIWORD(wParam));
                break;
            }
            break;
        case WM_DRAWITEM:
            if(wParam)	// not from a menu
                hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
            break;
        case WM_MEASUREITEM:
            if(wParam)	// not from a menu
                hWndChild = GetDlgItem(((LPMEASUREITEMSTRUCT)lParam)->CtlID);
            break;
        case WM_COMPAREITEM:
            if(wParam)	// not from a menu
                hWndChild = GetDlgItem(((LPCOMPAREITEMSTRUCT)lParam)->CtlID);
            break;
        case WM_DELETEITEM:
            if(wParam)	// not from a menu
                hWndChild = GetDlgItem(((LPDELETEITEMSTRUCT)lParam)->CtlID);
            break;
        case WM_VKEYTOITEM:
        case WM_CHARTOITEM:
        case WM_HSCROLL:
        case WM_VSCROLL:
            hWndChild = (HWND)lParam;
            break;
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            hWndChild = (HWND)lParam;
            break;
        default:
            break;
        }

        if(hWndChild == NULL)
        {
            bHandled = FALSE;
            return 1;
        }

        ATLASSERT(::IsWindow(hWndChild));
        return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
    }
private:
    FRAMESTATES    _frame_state;
    UINT _anchorDown, _anchorHover;
    unsigned long _fTrackNcMouseLeave : 1;
};

class SkinFrame : public SkinControlImpl<SkinFrame, SkinFrameImpl<SkinFrame>,
            RegisterPolicy>
{
public:
    typedef SkinFrameImpl<SkinFrame> framebase_type;
    static LPCTSTR GetWndClassName()
    {
        return _T("SKINFRAME");
    }
    enum { class_id = WINDOW };

    BEGIN_MSG_MAP(SkinFrame)
        CHAIN_MSG_MAP(framebase_type)
    END_MSG_MAP()
};

LRESULT WINAPI SkinFrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return SkinFrame::ControlProc(hWnd, uMsg, wParam, lParam);
}

} // namespace Skin


//
// 需要记录 placement 的元素：
// menu, system buttons
//
//
// part 相对于 parent window 的位置
// CRect SkinScheme::GetPlacement(int part, CRect& rcparent);
//
// <area state="" placement="left, top, right, bottom" />
// eg: placement="0, 0, -10, -10" 表示靠右上
//     placement="10, 0, -10, 0"  表示靠左下
//