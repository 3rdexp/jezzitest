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

    // ע�⣺�� wm_nccalcsize ��Ϣǰ��Ҫ��ʼ��
    SkinFrameImpl() : _frame_state(FS_ACTIVE) {}

    // TODO: �ƺ�Ӧ�û��� border ������Ϊ���Ա
    // ��Ҫ�� WM_NCCALCSIZE ʱ�õ�
    enum Metics
    {
        BorderThickness = 4,
    };

    // �� 12 ��SysButton, ÿ�� 6 ��״̬
    // tmschema.h ��ֻ������4�֣����� 0 ������ʹ�ö��ѣ�5 ��ʾ���ڲ�����active״̬
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

            // ������С����
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

            // �����
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

    // �̶�system button�� caption�ϵ�λ��
    // TODO: ������Ԫ�� placement �Ļ���
    // ������ʹ�� CalcSysButtonRect(index) ����
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

    // ���� menu bar �߶�
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
    // ������������ڱ�����
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

#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width(), rcw.Height(), dcMem, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif

        // memory dc
        ::BitBlt(dc, 0, 0, rcw.Width(), rcw.Height(), dcMem, rcw.left, rcw.top, SRCCOPY);
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);

        nRet = ::ExcludeClipRect( dc, 0, 0, 0, 0 );
        ASSERT( ERROR != nRet );
    }

    void DrawMenuBar(HDC hdc, CRect& rc)
    {
        HMENU hMenu = GetMenu();

        // xp only
        BOOL flat_menu = FALSE;
        SystemParametersInfoW (SPI_GETFLATMENU, 0, &flat_menu, 0);

        // font
        // 
        // FillRect(hdc, &rc, GetSysColorBrush(flat_menu ? COLOR_MENUBAR : COLOR_MENU) );
        // CPen pen;
        // pen.CreatePen(PS_SOLID, 1, 0x00cc00);
        // ::SelectObject(hdc, pen);
        HBRUSH br = CreateSolidBrush(0x00cc00);
        FillRect(hdc, &rc, br);
        DeleteObject(br);

        MENUINFO mi;
        mi.cbSize = sizeof(MENUINFO);
        mi.fMask = MIM_MAXHEIGHT;
        GetMenuInfo(hMenu, &mi);
    }


    // TODO: �Ƿ���Ҫ windows style �أ�
    void DrawFrameBorder(HDC hdc, CRect& rcw, FRAMESTATES _frame_state)
    {
        // FS_INACTIVE FS_ACTIVE
        ASSERT( hdc );
        ControlT * pT = static_cast<ControlT*>(this);

        // ����border���ȱ���һ�£������±߸߶Ȼ᲻һ��
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

        // 1 SysButton �����ͼ�����û�п��ܻ��������
        // WP_SYSBUTTON ����˼��ɶ�� ����ΪSysButton����ĵ�ͼ��
        RECT rc;
        BOOL bRet = FALSE;

        // CalcXXXButtonRect ���� CalcSysButtonRect(int index)�������߼����˺ܶ�
        // index ���ҵ�������

        int count_sys_button = 0;

        // 2 Button ����
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
        ControlT * pT1 = static_cast<ControlT*>(this);
        return pT1->DefWindowProc();
        // TODO: the return value ����Ҫ��������
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
#if 0
        // if (!bCalcValidRects) DebugBreak();

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
        return WVR_ALIGNBOTTOM;
#endif

#if 0
        NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS *)lParam;
        RECT* winRect = &lpncsp->rgrc[0];

        RECT tmpRect = { 0, 0, 0, 0 };
        LRESULT result = 0;
        LONG cls_style = GetClassLong(m_hWnd, GCL_STYLE);
        LONG style = GetWindowLong(GWL_STYLE );
        LONG exStyle = GetWindowLong( GWL_EXSTYLE );

        if (cls_style & CS_VREDRAW) result |= WVR_VREDRAW;
        if (cls_style & CS_HREDRAW) result |= WVR_HREDRAW;

        if (!IsIconic())
        {
            NC_AdjustRectOuter( &tmpRect, style, FALSE, exStyle );

            winRect->left   -= tmpRect.left;
            winRect->top    -= tmpRect.top;
            winRect->right  -= tmpRect.right;
            winRect->bottom -= tmpRect.bottom;

            if (((style & (WS_CHILD | WS_POPUP)) != WS_CHILD) && GetMenu())
            {
                TRACE("Calling GetMenuBarHeight with hwnd %p, width %ld, at (%ld, %ld).\n",
                    m_hWnd, winRect->right - winRect->left, -tmpRect.left, -tmpRect.top );

                winRect->top += CalcMenuBarHeight();
//                    MENU_GetMenuBarHeight( hwnd,
//                    winRect->right - winRect->left,
//                    -tmpRect.left, -tmpRect.top );
            }

            if( exStyle & WS_EX_CLIENTEDGE)
                if( winRect->right - winRect->left > 2 * GetSystemMetrics(SM_CXEDGE) &&
                    winRect->bottom - winRect->top > 2 * GetSystemMetrics(SM_CYEDGE))
                    InflateRect( winRect, - GetSystemMetrics(SM_CXEDGE),
                    - GetSystemMetrics(SM_CYEDGE));

            if (style & WS_VSCROLL)
                if( winRect->right - winRect->left >= GetSystemMetrics(SM_CXVSCROLL)){
                    if((exStyle & WS_EX_LEFTSCROLLBAR) != 0)
                        winRect->left  += GetSystemMetrics(SM_CXVSCROLL);
                    else
                        winRect->right -= GetSystemMetrics(SM_CXVSCROLL);
                }

                if (style & WS_HSCROLL)
                    if( winRect->bottom - winRect->top > GetSystemMetrics(SM_CYHSCROLL))
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
            adjust +=  ( GetSystemMetrics (SM_CXFRAME)
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
        // ���еļ��㶼������ڱ����ڵ�����,������ת��
        CRect rcw, rcc;
        GetWindowRect(&rcw);

        point.x -= rcw.left;
        point.y -= rcw.top;

        GetClientRect(&rcc);
        ClientToScreen(&rcc);
        rcc.OffsetRect( -rcw.left, -rcw.top );
        rcw.OffsetRect( -rcw.left, -rcw.top );

        // �ĸ����
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

        // ʹ�ñ�־����ֱ�ǰ���ʯͷ���Լ���ͷ
        if( GetStyle() & WS_DLGFRAME ) // �Ƿ��� TitleBar
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
// ��Ҫ��¼ placement ��Ԫ�أ�
// menu, system buttons
//
//
// part ����� parent window ��λ��
// CRect SkinScheme::GetPlacement(int part, CRect& rcparent);
//
// <area state="" placement="left, top, right, bottom" />
// eg: placement="0, 0, -10, -10" ��ʾ������
//     placement="10, 0, -10, 0"  ��ʾ������
//