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

    enum Metics
    {
        BorderThickness = 4,
    };

    struct SystemButtonState
    {
        enum StateT
        {
            hidden  = 0,
            normal	= 1,
            hover	= 2,
            down	= 3,
            disable = 4,
            inactive = 5
        };
        unsigned long _close	: 4;
        unsigned long _max		: 4;
        unsigned long _restore	: 4;
        unsigned long _min		: 4;
        unsigned long _help		: 4;

        SystemButtonState() : _close(hidden), _max(hidden), _restore(hidden), _min(hidden), _help(hidden) {}
        void initNormal()
        {
            _close = normal;
            _min = normal;
            _max = normal;
        }
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
                _max = disable;
            }

            if( !fWinActivate )
            {
                if( hasclose() )
                    _close = inactive;
                if( hasmax() && _max != disable )
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
    }

    // 固定system button在 caption上的位置
    // TODO: 建立子元素 placement 的机制
    RECT CalcCloseButtonRect()
    {
        ControlT * pT = static_cast<ControlT*>(this);
        CSize z = pT->GetSchemeSize(WP_CLOSEBUTTON);

        CRect rcw;
        GetWindowRect(&rcw);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        rcw.right -= BorderThickness;
        rcw.left = rcw.right - z.cx;
        rcw.top += BorderThickness;
        rcw.bottom = rcw.top + z.cy;
        return rcw;
    }

    RECT CalcMinButtonRect()
    {
        // TODO: 逻辑还不对
        ControlT * pT = static_cast<ControlT*>(this);
        CSize z = pT->GetSchemeSize(WP_MINBUTTON);

        CRect rcw;
        GetWindowRect(&rcw);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        rcw.right -= BorderThickness;
        rcw.left = rcw.right - z.cx;
        rcw.top += BorderThickness;
        rcw.bottom = rcw.top + z.cy;
        return rcw;
    }

    RECT CalcMaxButtonRect()
    {
        ControlT * pT = static_cast<ControlT*>(this);
        CSize z = pT->GetSchemeSize(WP_MAXBUTTON);

        CRect rcw;
        GetWindowRect(&rcw);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        rcw.right -= BorderThickness;
        rcw.left = rcw.right - z.cx;
        rcw.top += BorderThickness;
        rcw.bottom = rcw.top + z.cy;
        return rcw;
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
    void DrawFrame(HDC hdc, CRect& rcw, CRect& rcc, DWORD dwStyle)
    {
        CDC dc(hdc);
        // exclude the clienet area
        // ! The lower and right edges of the specified rectangle are not excluded from the clipping region.
        int nRet = ::ExcludeClipRect( dc, rcc.left, rcc.top, rcc.right, rcc.bottom );

        // memory dc
        HDC dcMem = ::CreateCompatibleDC(dc);
        ASSERT( dcMem );
        HBITMAP bmpMemBg = ::CreateCompatibleBitmap(dc, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top);
        ASSERT( bmpMemBg );
        HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
        ASSERT( pOldBmp );

        // 所有坐标相对于本窗口
        CRect rcw; GetWindowRect(&rcw);
        rcw.OffsetRect(rcw.left, rcw.top);

        DrawFrameBorder(dcMem, );

        _ASSERTE( _CrtCheckMemory( ) );

        if( dwStyle & WS_DLGFRAME )
        {
         //   RECT rc = rcWnd; rc.bottom = CaptionHeight(m_hWnd);

            // 计算 caption 的位置
            // TODO: 可能要加上左边的异型区域的宽度
         //   DrawTitleBar(m_hWnd, dcMem, &rc, _state);

            // draw system button
         //   SystemButtonState sbState;
         //   sbState.initFromWindow( dwStyle, _state == _Active );
         //   DrawSystemButton(m_hWnd, dcMem, sbState, _state );
        }
        else if( dwStyle & WS_BORDER )
        {
            TRACE("SkinFrmImpl Warning: Ttitle Bar 和 Border 必须同时设置，如果仅设置 WS_BORDER 会不正常的, 有时间会实现的. :( \n");
            // !!! Ttitle Bar 和 Border 必须同时设置，如果仅设置 WS_BORDER 会不正常的 !!!
            // TODO: 仅画出 top border 
        }

        _ASSERTE( _CrtCheckMemory( ) );

        // memory dc
        ::BitBlt(dc, 0, 0, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, dcMem, rcWnd.left, rcWnd.top, SRCCOPY);
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);

        nRet = ::ExcludeClipRect( dc, 0, 0, 0, 0 );
        ASSERT( ERROR != nRet );
    }

/*
    BEGIN_TM_PART_STATES(FRAME)
        TM_STATE(1, FS, ACTIVE)
        TM_STATE(2, FS, INACTIVE)
    END_TM_PART_STATES()


#define BEGIN_TM_PART_STATES(name)          enum name##STATES { name##StateFiller0,
#define TM_STATE(val, prefix, name)         prefix##_##name = val, 
#define END_TM_PART_STATES()                };
    enum FRAMESTATES
*/
    void DrawFrameBorder(HDC hdc, CRect& rcw, FRAMESTATES frame_state)
    {
        // FS_INACTIVE FS_ACTIVE
        ASSERT( hdc );
        ControlT * pT = static_cast<ControlT*>(this);

        // 左右border宽度保持一致，可能下边高度会不一样
        // 1 Caption Height, Border Width        
        CAPTIONSTATE caption_state = (frame_state == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE;
        int caption_height = pT->GetSchemeHeight(WP_CAPTION, caption_state);

        int bottom_height = pT->GetSchemeHeight(WP_FRAMEBOTTOM, frame_state);
        int border_width = pT->GetSchemeWidth(WP_FRAMELEFT, frame_state);

        // 2 Left, Right, Bottom
        CRect rc(rcw.left, rcw.top + caption_height, rcw.right, rcw.bottom - bottom_height);
        pT->Draw(hdc, WP_FRAMELEFT, frame_state, rc.left, rc.top, 0, rc.Height());

        rc.left = rcw.right - border_width;
        rc.right = rcw.right;
        pT->Draw(hdc, WP_FRAMERIGHT, frame_state, rc.left, rc.top, 0, rc.Height());

        rc.left = rcw.left;
        rc.top = rcw.bottom - bottom_height;
        pT->Draw(hdc, WP_FRAMEBOTTOM, frame_state, rc.left, rc.top, rc.Width(), 0);
    }

    void DrawCaption(HDC hdc, CRect& rcw) // frame state + system button state
    {
        // 1 calc caption area
    }
    
    void DrawSystemButton(HDC hdc, SystemButtonState& state, DWORD dwStyle)
    {
        ASSERT( hdc );
        ControlT * pT = static_cast<ControlT*>(this);
    
        // 1 绘制 caption 右边的底图
        std::string strCaptionRight("caption_right");
        if( _Inactive == dwWinState )
            strCaptionRight = "caption_right_inactive";

        CRect rcw; GetWindowRect(&rcw);
        rcw.OffsetRect( -rcw.left, -rcw.top );
        CRect rcBmp = pT->GetSchemeRect(0, 0); // TODO:
        int nSystemBtnWidth = CalcSystemButtonAreaWidth(hWnd);
        Draw( hdc, strCaptionRight.c_str(), rcw.right - nSystemBtnWidth, rcw.top, nSystemBtnWidth, rcBmp.Height(),
            rcBmp.Width() - nSystemBtnWidth, 0, nSystemBtnWidth, rcBmp.Height() );

        // 2 draw buttons
        RECT rc;
        BOOL bRet = FALSE;

        if( state.hasclose() )
        {
            rc = CalcCloseButtonRect(hWnd, &rc);
            bRet = pScheme->Draw( hdc, state.close_string(), rc.left, rc.top );
            ATLASSERT( bRet );
        }

        if( state.hasmax() )
        {
            rc = CalcMaxButtonRect(&rc);
            bRet = pScheme->Draw( hdc, state.max_string(), rc.left, rc.top );
            ATLASSERT( bRet );
        }

        if( state.hasrestore() )
        {
            if( state.hasmin() )
                rc = CalcMaxButtonRect(hWnd, &rc);
            else
                rc = CalcMinButtonRect(hWnd, &rc);
            bRet = pScheme->Draw( hdc, state.restore_string(), rc.left, rc.top );
            ATLASSERT( bRet );
        }

        if( state.hasmin() )
        {
            GetCaptionMinRect(&rc);
            bRet = pScheme->Draw( hdc, state.min_string(), rc.left, rc.top );
            ATLASSERT( bRet );
        }

        // TODO: help button
    }


    void DrawFrame(HWND hWnd, HDC hdc, const RECT* prc, ActiveState state)
    {
        ASSERT( hdc && prc );


//        CComPtr<ISkinScheme>	pScheme;
  //      LRESULT hr = GetCurrentScheme(&pScheme);
    //    if (!SUCCEEDED(hr) || !pScheme)
        {
            return ;	
        }

#if 0
        // 使用 GetNearestColor，为了在用户使用256色的情况下也能够绘制出正确的风格
        // TODO: use drawing function to paint the frame
        COLORREF crDark = ::GetNearestColor( hdc, pScheme->BorderDarkColor() );
        COLORREF crLight = ::GetNearestColor( hdc, pScheme->BorderDarkColor() );
        if( _rgn )
        {
            HPEN pen = CreatePen( PS_SOLID, 1, crDark );
            HBRUSH br = CreateSolidBrush( crLight );
            ASSERT( pen && br );

            HGDIOBJ oldPen = ::SelectObject( hdc, pen ):
            HGDIOBJ oldBrush = ::SelectObject( hdc, br );
            ASSERT( oldPen && oldBrush );

            // ::RoundRect(hdc, 
        }
        else
        {
        }
#else
        // FillRect(hdc, prc, (HBRUSH)GetStockObject(GRAY_BRUSH) );

/*
                    CRect rc = pScheme->ExtractRect("border_left");
                    int PART_LEN  = rc.Height() / 3; // 分段绘制, 该值似乎应该取 border_left 的高度， border_bottom, 50 三者中的最小值
                    const int bw = BorderWidth(hWnd);
            
                    // 1 left border 上半段-----------------------
                    pScheme->Draw( hdc, "border_left",  prc->left, prc->top, bw, PART_LEN,
                        0, 0, rc.Width(), PART_LEN );
            
                    // 2 下半段
                    pScheme->Draw( hdc, "border_left", prc->left, prc->bottom - PART_LEN, bw, PART_LEN,
                        0, rc.Height() - PART_LEN, rc.Width(), PART_LEN );
            
                    // 3 缩放中间部分
                    pScheme->Draw( hdc, "border_left", prc->left, prc->top + PART_LEN, bw, prc->bottom-prc->top - 2 * PART_LEN, 
                        0, PART_LEN, rc.Width(), rc.Height() - 2 * PART_LEN );
            
                    // right border--------------------------------
                    rc = pScheme->ExtractRect("border_right");
                    // 1上半段
                    pScheme->Draw( hdc, "border_right",  prc->right-bw, prc->top, bw, PART_LEN,
                        0, 0, rc.Width(), PART_LEN );
            
                    // 2 下半段
                    pScheme->Draw( hdc, "border_right", prc->right-bw, prc->bottom - PART_LEN, bw, PART_LEN,
                        0, rc.Height() - PART_LEN, rc.Width(), PART_LEN );
            
                    // 3 缩放中间部分
                    pScheme->Draw( hdc, "border_right", prc->right-bw, prc->top + PART_LEN, bw, prc->bottom-prc->top - 2 * PART_LEN, 
                        0, PART_LEN, rc.Width(), rc.Height() - 2 * PART_LEN );
            
                    // bottom border ------------------------------
                    rc = pScheme->ExtractRect("border_bottom");
                    const int bh = BorderHeight(hWnd);
                    PART_LEN = rc.Width() / 3;
                    // 1 left
                    pScheme->Draw( hdc, "border_bottom", prc->left, prc->bottom - bh, PART_LEN, bh, 
                        0, 0, PART_LEN, rc.Height() );
                    // 2 right
                    pScheme->Draw( hdc, "border_bottom", prc->right-PART_LEN, prc->bottom - bh, PART_LEN, bh, 
                        rc.Width() - PART_LEN, 0, PART_LEN, rc.Height() );
                    // 3 middle
                    pScheme->Draw( hdc, "border_bottom", prc->left + PART_LEN, prc->bottom - bh, prc->right-prc->left - 2 * PART_LEN, bh, 
                        PART_LEN, 0, rc.Width() - 2 * PART_LEN, rc.Height() );*/
            
#endif
    }

    BEGIN_MSG_MAP_EX(SkinFrameImpl)
        MSG_WM_NCACTIVATE(OnNcActivate)
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_NCHITTEST(OnNcHitTest)
        MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
    END_MSG_MAP()

    BOOL OnNcActivate(BOOL bActive)
    {
        _state = bActive ? _Active : _Inactive;
        CWindowDC dc(m_hWnd);
        // Paint(m_hWnd, dc, TRUE, pState);
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
        // 所有的计算都是相对于本窗口的坐标,所以先全部转换成窗口坐标
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

        //if (point.y > CaptionHeight(hWnd) && point.y <= CaptionHeight(hWnd) + 20)
        //return HTMENU;
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

        ControlT * pT = static_cast<ControlT*>(this);
        CRect rc_caption = pT->GetSchemeRect(GetPartType());
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
        RECT rcWnd;	GetWindowRect(&rcWnd);
        ::OffsetRect(&rcWnd, -rcWnd.left, -rcWnd.top);

        dc.FillSolidRect(&rcWnd, RGB(0xff,0,0));
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
            SystemButtonState sbState;
            ActiveState _state = _state;
            sbState.initFromWindow( GetStyle(), (_state == _Active) );
            if( HTCLOSE == nHitTest )
                sbState._close = SystemButtonState::down;
            else if( HTMAXBUTTON == nHitTest )
            {
                if( GetStyle() & WS_MAXIMIZE )
                    sbState._restore = SystemButtonState::down;
                else
                    sbState._max = SystemButtonState::down;
            }
            else if( HTMINBUTTON == nHitTest )
            {
                if( GetStyle() & WS_MINIMIZE )
                    sbState._restore = SystemButtonState::down;
                else
                    sbState._min = SystemButtonState::down;
            }

            HDC hdc = ::GetWindowDC(m_hWnd);
            // TODO: DrawSystemButton2( hWnd, hdc, sbState, _state );
            DrawSystemButton( hdc, sbState, _state );
            ::ReleaseDC(m_hWnd, hdc);
        }
        if( HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest )
        {
            SetMsgHandled(FALSE);
            //CallWindowProc(gDialogProc, hWnd, WM_NCLBUTTONDOWN, (WPARAM)(UINT)(nHitTest), MAKELPARAM((x), (y)));
        }
    }
private:
    ActiveState	_state;
    UINT _anchorDown, _anchorHover;
};

class SkinFrame : public SkinControlImpl<SkinFrame, SkinFrameImpl<SkinFrame>, RegisterPolicy>
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
