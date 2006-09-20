// non client area draw window

#pragma once

#include "SkinCtrl.h"

namespace Skin {

template<class ControlT, class WindowT = CWindow>
class SkinFrameImpl : public WindowT
{
protected:
    enum ActiveState
    {
        _Active   = 1,
        _Inactive = 2,
        _Disable  = 3
    };

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
            normal	= 1,
            hot	= 2,
            pushed	= 3,
            disabled = 4,
            inactive = 5
        };
        unsigned long _close	: 4;
        unsigned long _max		: 4;
        unsigned long _restore	: 4;
        unsigned long _min		: 4;
        unsigned long _help		: 4;

        SystemButtonState() : _close(hidden), _max(hidden), _restore(hidden), _min(hidden), _help(hidden) {}
        void initFromWindow(DWORD dwStyle, bool fWinActivate)
        {
            // close button always exist
            _close = normal;
            _restore = hidden;
            _max = hidden;

            // 窗口最小化了
            if( dwStyle & WS_MINIMIZE )
            {
                _min = hidden;
                _restore = normal;
            }
            else
            {
                if( dwStyle & WS_MINIMIZEBOX )
                    _min = normal;
            }

            // 最大化了
            if( dwStyle & WS_MAXIMIZE )
            {
                _restore = normal;
            }
            else
            {
                if( dwStyle & WS_MAXIMIZEBOX )
                    _max = normal;
            }

            if( hasmin() && !hasmax() )
            {
                _max = disabled;
            }

            if( !fWinActivate )
            {
                if( hasclose() )
                    _close = inactive;
                if( hasmax() && _max != disabled )
                    _max = inactive;
                if( hasrestore() )
                    _restore = inactive;
                if( hasmin() )
                    _min = inactive;
                if( hashelp() )
                    _help = inactive;
            }
        }

        bool hasclose() const	{ return( _close != hidden ); }
        bool hasmax() const		{ return( _max != hidden ); }
        bool hasrestore() const { return( _restore != hidden ); }
        bool hasmin() const		{ return( _min != hidden ); }
        bool hashelp() const	{ return( _help != hidden ); }
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

    int CalcSystemButtonAreaWidth()
    {
        // TODO:
        ASSERT(false);
        return 0;
    }


    // DrawFrame = Draw nonClient area
    // 1 draw border
    // 2 draw caption, include system button
    // 3 include resize anchor
    // 所有坐标相对于本窗口
    void DrawFrame(HDC hdc, CRect& rcw, CRect& rcc, DWORD dwStyle, FRAMESTATES frame_state)
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

        // DrawFrameBorder(dcMem, rcw, frame_state);

#if 0
        HDC dct = ::GetDC(0);
        BitBlt(dct, 0, 0, rcw.Width(), rcw.Height(), dcMem, 0, 0, SRCCOPY);
        ::ReleaseDC(0, dct);
#endif

        SystemButtonState sysbtn_state;
        sysbtn_state.initFromWindow(dwStyle, frame_state == FS_ACTIVE);
        CAPTIONSTATES caption_state = ((frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE);
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
    void DrawFrameBorder(HDC hdc, CRect& rcw, FRAMESTATES frame_state)
    {
        // FS_INACTIVE FS_ACTIVE
        ASSERT( hdc );
        ControlT * pT = static_cast<ControlT*>(this);

        // 左右border宽度保持一致，可能下边高度会不一样
        // 1 Caption Height, Border Width        
        CAPTIONSTATES caption_state = (frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE;
        int caption_height = pT->GetSchemeHeight(WP_CAPTION, caption_state);

        int bottom_height = pT->GetSchemeHeight(WP_FRAMEBOTTOM, frame_state);
        int border_left_width = pT->GetSchemeWidth(WP_FRAMELEFT, frame_state);
        int border_right_width = pT->GetSchemeWidth(WP_FRAMERIGHT, frame_state);

        // 2 Left
        CRect rc(rcw.left, rcw.top + caption_height, 
            rcw.left + border_left_width, rcw.bottom - bottom_height);
        pT->Draw(hdc, WP_FRAMELEFT, frame_state, rc.left, rc.top, 0, rc.Height());

        // Right
        rc.left = rcw.right - border_right_width;
        rc.right = rcw.right;
        pT->Draw(hdc, WP_FRAMERIGHT, frame_state, rc.left, rc.top, 0, rc.Height());

        // Bottom
        rc.left = rcw.left;
        rc.top = rcw.bottom - bottom_height;
        rc.bottom = rcw.bottom;
        pT->Draw(hdc, WP_FRAMEBOTTOM, frame_state, rc.left, rc.top, rc.Width(), 0);

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
        // use CalcSystemButtonAreaWidth

        // CalcXXXButtonRect 换成 CalcSysButtonRect(int index)，代码逻辑简单了很多
        // index 重右到左的序号

        int count_sys_button = 0;

        // 2 Button 表面
        if( sysbtn_state.hasclose() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_CLOSEBUTTON, sysbtn_state._close, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if( sysbtn_state.hasmax() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_MAXBUTTON, sysbtn_state._max, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if( sysbtn_state.hasrestore() )
        {
            rc = CalcSysButtonRect(count_sys_button++);
            bRet = pT->Draw(hdc, WP_RESTOREBUTTON, sysbtn_state._restore, rc.left, rc.top);
            ATLASSERT( bRet );
        }

        if( sysbtn_state.hasmin() )
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
        MSG_WM_NCHITTEST(OnNcHitTest)
        MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
    END_MSG_MAP()

    BOOL OnNcActivate(BOOL bActive)
    {
        _frame_state = bActive ? FS_ACTIVE : FS_INACTIVE;

        CWindowDC dc(m_hWnd);
        
        CRect rcw, rcc;	
        GetClientRect(&rcc);
        GetWindowRect(&rcw);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        // DrawFrame(dc, rcw, rcc, GetStyle(), _frame_state);
        return TRUE;
    }

    void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
    {
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
        RECT* pRect = &lpncsp->rgrc[0];
        if( GetStyle() & WS_DLGFRAME )
            pRect->top += GetSchemeHeight(GetPartType()); // CaptionHeight(hWnd);

        if( GetStyle() & WS_BORDER )
        {
            pRect->left += BorderThickness; // BorderWidth(hWnd);
            pRect->right -= BorderThickness; // BorderWidth(hWnd);
            pRect->bottom -= BorderThickness; // BorderHeight(hWnd);
        }
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

#define _RSBOX 15 // resize 的范围 resize box

        DWORD dwStyle = GetStyle();
        ControlT * pT = static_cast<ControlT*>(this);
        CRect rc_caption = pT->GetSchemeRect(WP_CAPTION, _frame_state);

        if (GetMenu() && point.y > rc_caption.Height() 
                && point.y <= rc_caption.Height() + 20) // TODO: menu height
            return HTMENU;
#if 1 // HTGROWBOX 没有用，使用 HTBOTTOMRIGHT
        // 优先对 HTGROWBOX 进行判断, 可能有部分 grow box 在客户区中
        if( dwStyle & WS_THICKFRAME )
        {
            if( point.x > rcw.right - _RSBOX && point.y > rcw.bottom - _RSBOX )
                return HTBOTTOMRIGHT;
        }
#endif

        // client area
        rcc.DeflateRect( 2, 2 );// 把rcc缩小一些，鼠标响应会灵敏一些
        if( rcc.PtInRect(point) )
            return HTCLIENT;

        int border_width = BorderThickness;
        int border_height = BorderThickness;

        
        if( rc_caption.PtInRect(point) )
        {
            CRect rcclose = CalcCloseButtonRect();
            if( rcclose.PtInRect(point) )
                return HTCLOSE;
            if( WS_MINIMIZEBOX & dwStyle )
            {
                CRect rcmin = CalcMinButtonRect();
                if( rcmin.PtInRect( point ) )
                    return HTMINBUTTON;
            }
            if( WS_MAXIMIZEBOX & dwStyle )
            {
                CRect rcmax = CalcMaxButtonRect();
                if( rcmax.PtInRect( point ) )
                    return HTMAXBUTTON;
            }

            return HTCAPTION;
        }

        if( !(dwStyle & WS_THICKFRAME) )
            if( rcw.PtInRect(point) )
                return HTBORDER;

        if( point.x < border_width ) // left
        {
            // 优先判断角
            if( point.y > rcw.bottom - _RSBOX )
                return HTBOTTOMLEFT;

            if( point.y < _RSBOX )
                return HTTOPLEFT;

            return HTLEFT;
        }
        if( point.x > rcc.right ) // right border
        {
            if( point.y > rcw.bottom - _RSBOX )
                return HTBOTTOMRIGHT;

            if( point.y < _RSBOX )
                return HTTOPRIGHT;

            return HTRIGHT;
        }

        if( point.y < border_height ) // top border
        {
            if( point.x < _RSBOX )
                return HTTOPLEFT;
            if( point.x > rcw.right - _RSBOX )
                return HTTOPRIGHT;
            else
                return HTTOP;
        }
        if( point.y > rcc.bottom ) // bottom border
        {
            if( point.x < _RSBOX )
                return HTBOTTOMLEFT;
            if( point.x > rcw.right - _RSBOX )
                return HTBOTTOMRIGHT;
            else
                return HTBOTTOM;
        }
        return HTERROR;
#undef _RSBOX
        //  HTBORDER In the border of a window that does not have a sizing border. 
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
    void OnNcLButtonDown(UINT nHitTest, CPoint point)
    {
        if( nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE )
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
        if( GetStyle() & WS_DLGFRAME )
        {
            SystemButtonState sysbtn_state;
            sysbtn_state.initFromWindow( GetStyle(), (_frame_state == FS_ACTIVE) );
            if( HTCLOSE == nHitTest )
                sysbtn_state._close = SystemButtonState::pushed;
            else if( HTMAXBUTTON == nHitTest )
            {
                if( GetStyle() & WS_MAXIMIZE )
                    sysbtn_state._restore = SystemButtonState::pushed;
                else
                    sysbtn_state._max = SystemButtonState::pushed;
            }
            else if( HTMINBUTTON == nHitTest )
            {
                if( GetStyle() & WS_MINIMIZE )
                    sysbtn_state._restore = SystemButtonState::pushed;
                else
                    sysbtn_state._min = SystemButtonState::pushed;
            }

            HDC hdc = ::GetWindowDC(m_hWnd);
            // TODO: DrawSystemButton2( hWnd, hdc, sysbtn_state, _state );
            // DrawSysButton( hdc, sysbtn_state, _state );
            ::ReleaseDC(m_hWnd, hdc);
        }
        if( HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest )
        {
            SetMsgHandled(FALSE);
            //CallWindowProc(gDialogProc, hWnd, WM_NCLBUTTONDOWN, (WPARAM)(UINT)(nHitTest), 
             // MAKELPARAM((x), (y)));
        }
    }
private:
    FRAMESTATES	_frame_state;
    UINT _anchorDown, _anchorHover;
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