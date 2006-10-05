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
    SkinFrameImpl() : _frame_state(FS_ACTIVE) {}

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
            if ( dwStyle & WS_MINIMIZE )
            {
                _min = hidden;
                _restore = normal;
            }
            else
            {
                if ( dwStyle & WS_MINIMIZEBOX )
                    _min = normal;
            }

            // 最大化了
            if ( dwStyle & WS_MAXIMIZE )
            {
                _restore = normal;
            }
            else
            {
                if ( dwStyle & WS_MAXIMIZEBOX )
                    _max = normal;
            }

            if ( hasmin() && !hasmax() )
            {
                _max = disabled;
            }

            if ( !fWinActivate )
            {
                if ( hasclose() )
                    _close = inactive;
                if ( hasmax() && _max != disabled )
                    _max = inactive;
                if ( hasrestore() )
                    _restore = inactive;
                if ( hasmin() )
                    _min = inactive;
                if ( hashelp() )
                    _help = inactive;
            }
        }

        bool hasclose() const    { return( _close != hidden ); }
        bool hasmax() const        { return( _max != hidden ); }
        bool hasrestore() const { return( _restore != hidden ); }
        bool hasmin() const        { return( _min != hidden ); }
        bool hashelp() const    { return( _help != hidden ); }
    }; // SystemButtonState

    int GetPartType()
    {
        return WP_CAPTION;
        // TODO: WP_SMALLCAPTION
        // TODO: remove GetPartType, no sense get PartType here
    }

    int CaptionHeight()
    {
        ControlT * pT = static_cast<ControlT*>(this);
        return pT->GetSchemeSize(WP_CAPTION, CS_ACTIVE).cy;
    }

    // title    ?  _ []  X
    //
    // index:   3  2  1  0
    RECT CalcSysButtonRect(int index)
    {
        ControlT * pT = static_cast<ControlT*>(this);
        CSize z = pT->GetSchemeSize(WP_SYSBUTTON);

        CRect rcw;
        GetWindowRect(&rcw);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        rcw.top += BorderThickness;
        rcw.bottom = rcw.top + z.cy;

        rcw.right = rcw.right - (index + 1) * BorderThickness - index * z.cx;
        rcw.left = rcw.right - z.cx;
        return rcw;
    }

    // 固定system button在 caption上的位置
    // TODO: 建立子元素 placement 的机制
    // 现在先使用 CalcSysButtonRect(index) 函数
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
        _ASSERTE( _CrtCheckMemory( ) );

        CDC dc(hdc);
        // exclude the client area
        // ! The lower and right edges of the specified rectangle are not excluded from the clipping region.
        int nRet = ::ExcludeClipRect( dc, rcc.left, rcc.top, rcc.right, rcc.bottom );

        // memory dc
        HDC dcMem = ::CreateCompatibleDC(dc);
        ASSERT( dcMem );
        HBITMAP bmpMemBg = ::CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
        ASSERT( bmpMemBg );
        HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
        ASSERT( pOldBmp );

        DrawFrameBorder(dcMem, rcw, _frame_state);

#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width(), rcw.Height(), dcMem, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif

        SystemButtonState sysbtn_state;
        sysbtn_state.initFromWindow(dwStyle, _frame_state == FS_ACTIVE);
        CAPTIONSTATES caption_state = ((_frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE);
        // TODO:
        DrawCaption(dcMem, rcw, dwStyle, caption_state, sysbtn_state);

        // memory dc
        ::BitBlt(dc, 0, 0, rcw.Width(), rcw.Height(), dcMem, rcw.left, rcw.top, SRCCOPY);
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);

        nRet = ::ExcludeClipRect( dc, 0, 0, 0, 0 );
        ASSERT( ERROR != nRet );
    }


    // TODO: 是否需要 windows style 呢？
    void DrawFrameBorder(HDC hdc, CRect& rcw, FRAMESTATES _frame_state)
    {
        // FS_INACTIVE FS_ACTIVE
        ASSERT( hdc );
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
        ASSERT( hdc );
        ControlT * pT = static_cast<ControlT*>(this);

        pT->Draw(hdc, WP_CAPTION, caption_state, rcw.left, rcw.top, rcw.Width(), 0);
        DrawSysButton(hdc, rcw, sysbtn_state, dwStyle);
#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width() + 10, rcw.Height() + 10, hdc, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif
    }

    void DrawSysButton(HDC hdc, CRect& rcw, SystemButtonState& sysbtn_state, DWORD dwStyle)
    {
        ASSERT( hdc );
        ControlT * pT = static_cast<ControlT*>(this);

        // 1 SysButton 区域底图，如果没有可能会造成问题
        // WP_SYSBUTTON 的意思是啥？ 能作为SysButton区域的底图吗？
        RECT rc;
        BOOL bRet = FALSE;

        // CalcXXXButtonRect 换成 CalcSysButtonRect(int index)，代码逻辑简单了很多
        // index 重右到左的序号

        int count_sys_button = 0;

        // 2 Button 表面
        if ( sysbtn_state.hasclose() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_CLOSEBUTTON, sysbtn_state._close, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if ( sysbtn_state.hasmax() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_MAXBUTTON, sysbtn_state._max, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if ( sysbtn_state.hasrestore() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_RESTOREBUTTON, sysbtn_state._restore, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if ( sysbtn_state.hasmin() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_MINBUTTON, sysbtn_state._min, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if (sysbtn_state.hashelp())
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_HELPBUTTON, sysbtn_state._help, rc.left, rc.top);
            ATLASSERT( bRet );
        }
    }

    BEGIN_MSG_MAP_EX(SkinFrameImpl)
        MSG_WM_NCACTIVATE(OnNcActivate)
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_NCCALCSIZE(OnNcCalcSize)
        // MSG_WM_NCHITTEST(OnNcHitTest)
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
        // MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
        // MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
        // MSG_WM_NCMOUSELEAVE(OnNcMouseLeave)
        // MSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
        // MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
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

    BOOL OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
    {
        // TODO: the return value 很重要。。。。
        // TODO: read <<Windows_Graphics_Programming;_Win32_GDI_and_DirectDraw_(2000)>>
        //       find out which value should return!!!

        /*
        if bValidateClient is 1
        #1 Rect[0] is the proposed new client position

        #2 Rect[1] is the source rectangle or the current window in case you want to
        preserve the graphics that are already drawn there.

        #3 Rect[2] is the destination rectangle where the source graphics will be
        copied to. If this rectangle is a different size to the source the top and
        left wil be copied but the graphics will be clipped, not resized. You can
        for example copy only a relavent subset of the current client to the new
        place.
        */
        NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS *)lParam;
        RECT* pRect = &lpncsp->rgrc[0];
        if ( GetStyle() & WS_DLGFRAME )
            pRect->top += CaptionHeight();

        if (GetMenu())
        {
            pRect->top += CalcMenuBarHeight();
        }

        if ( GetStyle() & WS_BORDER )
        {
            ControlT * pT = static_cast<ControlT*>(this);

            pRect->left += pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);
            pRect->right -= pT->GetSchemeWidth(WP_FRAMERIGHT, _frame_state);
            pRect->bottom -= pT->GetSchemeHeight(WP_FRAMEBOTTOM, _frame_state);
        }
        return TRUE;
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
        rcc.OffsetRect( -rcw.left, -rcw.top );
        rcw.OffsetRect( -rcw.left, -rcw.top );

        // 四个大块
        // 1 border
        // 2 caption
        // 3 menu
        // 4 client

        DWORD dwStyle = GetStyle();
        ControlT * pT = static_cast<ControlT*>(this);

        if ( rcc.PtInRect(point) )
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
                CRect rc = CalcSysButtonRect(i);
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
                return HTMENU;
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
        if( nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE )
        {
            if( _anchorDown && _anchorDown != nHitTest )
                _anchorDown = 0;			

            if( _anchorHover != nHitTest )
                _anchorHover = nHitTest;

            if( !_fTrackNcMouseLeave)
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
        if( GetStyle() & WS_DLGFRAME ) // 是否有 TitleBar
        {
            SystemButtonState sbState;
            DWORD dwStyle = GetStyle();
            sbState.initFromWindow( dwStyle, (_frame_state == FS_ACTIVE) );
            if( HTCLOSE == nHitTest )
                sbState._close = SystemButtonState::hot;
            else if( HTMAXBUTTON == nHitTest )
            {
                if( dwStyle & WS_MAXIMIZE )
                    sbState._restore = SystemButtonState::hot;
                else
                    sbState._max = SystemButtonState::hot;
            }
            else if( HTMINBUTTON == nHitTest )
            {
                if( sbState._min != SystemButtonState::hidden )
                    sbState._min = SystemButtonState::hot;
            }

            //CWindowDC dc(CWnd::FromHandle(hWnd));
            //Paint( hWnd, dc, TRUE, pState );

//            HDC hdc = ::GetWindowDC(hWnd);
//            DrawSystemButton2( hWnd, hdc, sbState, _frame_state );
//            ::ReleaseDC(hWnd, hdc);
        }
        
        if( HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest )
        {
            return FALSE;
            //CallWindowProc(gDialogProc, hWnd, WM_NCMOUSEMOVE, (WPARAM)(UINT)(nHitTest), MAKELPARAM((x), (y)));
        }
        return TRUE;
    }
    
    void OnNcLButtonDown(UINT nHitTest, CPoint point)
    {
        if ( nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE )
        {
            _anchorDown  = nHitTest;
            _anchorHover = nHitTest;
            SetMsgHandled(FALSE);
        }
        else
        {
            _anchorDown  = 0;
            _anchorHover = 0;
        }
        if ( GetStyle() & WS_DLGFRAME )
        {
            SystemButtonState sysbtn_state;
            sysbtn_state.initFromWindow( GetStyle(), (_frame_state == FS_ACTIVE) );
            if ( HTCLOSE == nHitTest )
                sysbtn_state._close = SystemButtonState::pushed;
            else if ( HTMAXBUTTON == nHitTest )
            {
                if ( GetStyle() & WS_MAXIMIZE )
                    sysbtn_state._restore = SystemButtonState::pushed;
                else
                    sysbtn_state._max = SystemButtonState::pushed;
            }
            else if ( HTMINBUTTON == nHitTest )
            {
                if ( GetStyle() & WS_MINIMIZE )
                    sysbtn_state._restore = SystemButtonState::pushed;
                else
                    sysbtn_state._min = SystemButtonState::pushed;
            }

            HDC hdc = ::GetWindowDC(m_hWnd);
            // TODO: DrawSystemButton2( hWnd, hdc, sysbtn_state, _frame_state );
            // DrawSysButton( hdc, sysbtn_state, _frame_state );
            ::ReleaseDC(m_hWnd, hdc);
        }
        if ( HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest )
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

        ControlT * pT = static_cast<ControlT*>(this);
        pT->DefWindowProc();

        if (HTMAXBUTTON == nHitTest && (GetStyle() & WS_MAXIMIZEBOX))
        {
            // ShowWindow(SW_MAXIMIZE);
            if ( IsZoomed() )
                SendMessage(WM_SYSCOMMAND, SC_RESTORE, -1 );
            else
                SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, -1 );
        }
        else if (HTMINBUTTON == nHitTest )
        {
            if ( IsIconic() )
                SendMessage(WM_SYSCOMMAND, SC_RESTORE, -1 );
            else
                SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, -1 );
        }
        else if (HTCLOSE == nHitTest )
            SendMessage(WM_SYSCOMMAND, SC_CLOSE, -1 );
    }

    void OnNcMouseLeave()
    {
        _fTrackNcMouseLeave = false;
        _anchorHover = 0;

        SystemButtonState sbState;
        sbState.initFromWindow(GetStyle(), (_frame_state == FS_ACTIVE));
        sbState._close = SystemButtonState::normal;
        {
            if ( GetStyle() & WS_MAXIMIZE )
                sbState._restore = SystemButtonState::normal;
            else
                sbState._max = SystemButtonState::normal;
        }
        {
            if ( GetStyle() & WS_MINIMIZE )
                sbState._restore = SystemButtonState::normal;
            else
                sbState._min = SystemButtonState::normal;
        }

        // HDC hdc = ::GetWindowDC(m_hWnd);
        // DrawSystemButton2( hWnd, hdc, sbState, _frame_state );
        // ::ReleaseDC(hWnd, hdc);
    }

    BOOL OnNcLButtonDblClk(UINT nHitTest, CPoint point)
    {
        if ( HTCAPTION == nHitTest )
        {
            HICON hIcon = (HICON)SendMessage(WM_GETICON, ICON_SMALL, 0);
            CPoint pt = point;
            ScreenToClient(&pt);
            if ( hIcon && pt.x < 16 + 9 )
            {
                SendMessage( WM_SYSCOMMAND, SC_CLOSE, -1 );
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