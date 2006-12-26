// non client area draw window

#pragma once

#include "SkinCtrl.h"
#include "menubar.h"

namespace Skin {

using WTL::CDCHandle;
using WTL::CMenuHandle;

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
#endif // #ifndef MSG_WM_NCMOUSELEAVE

// Menu 扩充的 part and property 
// BEGIN_TM_CLASS_PARTS(WINDOW)
#define WP_SYSTEM_LAST  WP_SMALLFRAMEBOTTOMSIZINGTEMPLATE
#define WP_MENUBAR      WP_SYSTEM_LAST + 1  // WP_MENU 已经被使用了

// TMT_MENU             ? menu item background
// TMT_MENUTEXT         The color of text drawn on a menu.
// TMT_MENUHILIGHT      The color of highlight drawn on a menu item when the 
//                      user moves the mouse over it.
// TMT_MENUBAR          The color of the menu bar. 
//                      ? bar background
#define TMT_MENU_BORDER         TMT_MENUBAR + 1     // item border
#define TMT_MENUBORDER_HILIGHT  TMT_MENUBAR + 2     // hilight border


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Fuck! MenuBar item 鼠标移动上去MenuBar Item 变化的效果似乎不可阻挡!
// I Hate MS!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



template<class ControlT, class WindowT = CWindow>
class SkinFrameImpl : public WindowT
{
protected:
    
    SkinFrameImpl() 
        // 注意：在 WM_NCCALCSIZE 消息前就要初始化
        : _frame_state(FS_ACTIVE)
        , _fTrackNcMouseLeave(false) 
        , _anchorDown(0) , _anchorHover(0)
        , _hoverMenuItem(-1), _selectedMenuItem(-1)
        , _rgn(0)
    {
		m_bIgnoreAlt = FALSE;
	}

    // TODO: 似乎应该缓存 border 宽度作为类成员
    // 只用在： CalcSysButtonRect
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

            if (hasmin() && !hasmax() && !hasrestore())
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
    RECT CalcSysButtonRect(const WTL::CRect& rcw, int index)
    {
        ControlT * pT = static_cast<ControlT*>(this);
        CSize z = pT->GetSchemeSize(WP_SYSBUTTON);

        WTL::CRect rc = rcw;
        rc.top += BorderThickness;
        rc.bottom = rc.top + z.cy;

        rc.right = rc.right - (index + 1) * BorderThickness - index * z.cx;
        rc.left = rc.right - z.cx;
        return rc;
    }

    
    // 计算 menu bar 高度
    // 1 MenuInfo
    // 2 SystemMetrics TODO: from skin.cfg

	// 需要处理菜单显示不全的情况???
    int CalcMenuBarHeight()
    {
		return m_MenuBar.CalcMenuBarHeight();

		/*
        HMENU h = m_hMenu;
        ASSERT(h);
        MENUINFO mi;
        mi.cbSize = sizeof(MENUINFO);
        mi.fMask = MIM_MAXHEIGHT;
        GetMenuInfo(h, &mi);

        if (0 == mi.cyMax)
        {
            // ASSERT(false && "这样的情况会发生吗?");
            return GetSystemMetrics(SM_CYMENU);
        }
        else
            return mi.cyMax;
		*/
    }

	static HFONT GetCtrlFont(HWND hwnd)
	{
		HFONT hFont;
		//如果sendmessage得到的是NULL,则使用的是系统字体
		if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
			hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

		return hFont;
	}

	void InitMenu( CMenuHandle menu )
	{
		//得到Menu的各个菜单
		if ( !menu )
			return;

		ControlT * pT = static_cast<ControlT*>(this);

		int border_left_width = pT->GetSchemeWidth(WP_FRAMELEFT, _frame_state);

		m_MenuBar.InitMenu( m_hWnd, menu, border_left_width, CaptionHeight());
	}


    //! MenuBar 缺省位置就在 caption 下面
    RECT CalcMenuBarRect(const WTL::CRect& rcw, FRAMESTATES fs)
    {
        return m_MenuBar.CalcMenuBarRect( rcw, fs );
    }


    RECT CalcMenuItemRect(int nItemIndex, const WTL::CRect& rcmb, FRAMESTATES fs)
    {
		return m_MenuBar.CalcMenuItemRect( nItemIndex );
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

    // DrawFrame = Draw nonClient area
    // 1 draw border = DrawFrameBorder
    // 2 draw caption, include system button = DrawCaption
    // 3 MenuBar = DrawMenuBar
    // 4 TODO: resize anchor = 
    // 所有坐标相对于本窗口
    void DrawFrame(HDC hdc, WTL::CRect& rcw, WTL::CRect& rcc, DWORD dwStyle, FRAMESTATES _frame_state)
    {
        _ASSERTE(_CrtCheckMemory());

        WTL::CDC dc(hdc);
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

        if ( m_MenuBar.GetMenu() )
		{
            m_MenuBar.DrawMenuBar(dcMem, m_MenuBar.GetMenu(), rcw, -1, MS_NORMAL );
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

        nRet = ::ExcludeClipRect(dc, 0, 0, 0, 0);
        ASSERT(ERROR != nRet);
    }



    // TODO: 是否需要 windows style 呢？
    void DrawFrameBorder(HDC hdc, const WTL::CRect& rcw, FRAMESTATES _frame_state)
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
        WTL::CRect rc(rcw.left, rcw.top + caption_height,
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

    void DrawCaption(HDC hdc, WTL::CRect& rcw, DWORD dwStyle, CAPTIONSTATES caption_state,
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

    void DrawSysButton(HDC hdc, const WTL::CRect& rcw, const SystemButtonState& sysbtn_state, DWORD dwStyle)
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
    void EtchedSysButton(CDCHandle dc, const WTL::CRect& rcw, const SystemButtonState& sysbtn_state)
    {
        // 先把图绘制在 dcmem 上，然后再绘制到 dc 上
        WTL::CDC dcmem;
        dcmem.CreateCompatibleDC(dc);
        WTL::CBitmap bmpbg;
        bmpbg.CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
        HBITMAP bmpold = dcmem.SelectBitmap(bmpbg);

        ControlT * pT = static_cast<ControlT*>(this);

        WTL::CRect rc;
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

        // WTL::CWindowDC dct(0);
        // dct.BitBlt(0, 0, rcw.Width(), rcw.Height(), dcmem, 0, 0, SRCCOPY);

        // 
        dcmem.SelectBitmap(bmpold);
    }

    BEGIN_MSG_MAP_EX(SkinFrameImpl)
    //    if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
    //        && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
    //        ATLTRACE("%04x frame\n", uMsg);
    // if (uMsg == WM_COMMAND)
    //    __asm nop;
        MSG_WM_NCACTIVATE(OnNcActivate)
        MSG_WM_NCPAINT(OnNcPaint)
		//MSG_WM_PAINT(OnPaint)
		MSG_WM_SETCURSOR( OnSetCursor )
		//MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
        MSG_WM_NCCALCSIZE(OnNcCalcSize)
		
#if 1
        MSG_WM_NCHITTEST(OnNcHitTest)
#else
        if (uMsg == WM_NCHITTEST) 
        {
            SetMsgHandled(TRUE); 
            lResult = (LRESULT)OnNcHitTest(WTL::CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); 
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
        // MSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
        MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
		//MSG_WM_MOUSEMOVE(OnMouseMove)

        MSG_WM_MENUSELECT(OnMenuSelect)
		/*
		MESSAGE_HANDLER( WM_SYSKEYDOWN, OnKey)
		MESSAGE_HANDLER( WM_SYSKEYUP, OnKey)
		MESSAGE_HANDLER( WM_KEYDOWN, OnKey)
		*/

		if ( uMsg == WM_KEYDOWN )
		{
			int i ;
			i = 0;
		}
		//MESSAGE_HANDLER( WM_SYSCOMMAND, OnSysCommand)
    END_MSG_MAP()

    BOOL OnNcActivate(BOOL bActive)
    {
        TRACE("WM_NCACTIVATE %d\n", bActive);
        _frame_state = bActive ? FS_ACTIVE : FS_INACTIVE;

        WTL::CWindowDC dc(m_hWnd);

        WTL::CRect rcw, rcc;
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
        WTL::CRect rc1 = lpncsp_->rgrc[0];
        
        ControlT * pT1 = static_cast<ControlT*>(this);
        LRESULT ret = pT1->DefWindowProc();
        WTL::CRect rc2 = lpncsp_->rgrc[0];
        RECT & new_rcc = lpncsp_->rgrc[0];
        
    //    TraceRect("before", &rc1);
    //    TraceRect("after ", &rc2);
    //    TRACE("return %d\n", ret);

        DWORD dwStyle = GetStyle();

        int c_top = 0;
        if (dwStyle & WS_DLGFRAME)
            c_top += CaptionHeight();
        if ( m_MenuBar.GetMenu() )
            c_top += CalcMenuBarHeight();

        int c_left = 0, c_right = 0, c_bottom = 0;

        if (dwStyle & WS_BORDER)
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
            
            WTL::CRect rcw;
            GetWindowRect(&rcw);

            new_rcc = rcw;

            new_rcc.top = rcw.top + CaptionHeight();

            if ( m_MenuBar.GetMenu() )
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
                TRACE("Attention: wParam=%d IsIconic\n", bCalcValidRects);
            }
            return 0;
        }
#endif
    }

    UINT OnNcHitTest(WTL::CPoint point)
    {
        ControlT * pT = static_cast<ControlT*>(this);
        // return pT->DefWindowProc();
		//UINT lr = pT->DefWindowProc();

        // 所有的计算都是相对于本窗口的坐标,所以先转换
        WTL::CRect rcw, rcc;
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

        if (rcc.PtInRect(point))
            return HTCLIENT;

        WTL::CRect rc_caption = pT->GetSchemeRect(WP_CAPTION, _frame_state);

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
            else if (point.y > rcw.bottom - bottom_height) // TODO: 系统的区域似乎到了 12
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
                WTL::CRect rc = CalcSysButtonRect(rcw, i);
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

            // TODO: GetIcon() return 0, 应该是用窗口风格判断。
            // 至少 tool window没有 htsysmenu
            //if (GetIcon())
            {
                WTL::CRect rc = rc_caption;
                rc.bottom = rc.top + 16;
                rc.right = rc.left + 16;
                if (rc.PtInRect(point))
                    return HTSYSMENU;
            }

            return HTCAPTION;
        }

		
        if ( m_MenuBar.GetMenu() )
        {
            int bar_height = CalcMenuBarHeight();
            rc_caption.top = rc_caption.bottom;
            rc_caption.bottom += bar_height;
            if (rc_caption.PtInRect(point))
                return HTMENU; // TODO: 
        }
		
		
		//return lr;
        // TODO: menu 后半部分 返回啥？
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
        //  HTSYSMENU In a window menu or in a Close button in a child window.
        //  HTTOP In the upper-horizontal border of a window.
        //  HTTOPLEFT In the upper-left corner of a window border.
        //  HTTOPRIGHT In the upper-right corner of a window border.
        // HTTRANSPARENT In a window currently covered by another window in the same thread (the message will be sent to underlying windows in the same thread until one of them returns a code that is not HTTRANSPARENT).
        // HTVSCROLL In the vertical scroll bar.
        // HTZOOM In a Maximize button.
    }
    // TODO: Read http://www.codeproject.com/menu/newmenuxpstyle.asp


	
	LRESULT OnKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bAlt = HIWORD( lParam) & KF_ALTDOWN;	// Alt key presed?
		TCHAR vkey = wParam;						// + X key

		//对F10的支持还没有

		if (( uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYDOWN)) 
		{
			// Alt + X pressed
			if ((bAlt || _hoverMenuItem != -1 ) && _istalnum(vkey)) 
			{
				int nIndex;
				if ( m_MenuBar.MapAccessKey(vkey, nIndex) == TRUE ) 
				{
					_selectedMenuItem = nIndex;
					m_MenuBar.TrackPopup( _selectedMenuItem );
					return TRUE;		// eat it!
				}
				else if ( _hoverMenuItem != -1 && !bAlt)
				{
					//					MessageBeep(0);		// if you want
					return TRUE;
				}
			}
		}
		ControlT * pT = static_cast<ControlT*>(this);
		LRESULT lRet = pT->DefWindowProc();
		return lRet;
	}

    BOOL OnCreate(LPCREATESTRUCT)
    {
		m_MenuBar.SetMenu( GetMenu() );

        SetMsgHandled(FALSE);
        
#if 0
        if(GetStyle() & WS_DLGFRAME)
        {
            ControlT * pT = static_cast<ControlT*>(this);
            _rgn = pT->GetSchemeRegion(0, 0);
            ASSERT(OBJ_REGION == ::GetObjectType(_rgn));
        }
#else
        if(GetStyle() & WS_DLGFRAME)
        {
            CRect rcw;
            GetWindowRect(&rcw);
            rcw.OffsetRect(-rcw.left, -rcw.top);
            _rgn = CreateRoundRectRgn(rcw.left, rcw.top, rcw.right, rcw.bottom, 10, 10);
            ASSERT(OBJ_REGION == ::GetObjectType(_rgn));
        }
#endif  
		
		

		SetMenu ( 0 );

		InitMenu( m_MenuBar.GetMenu() );
		m_MenuBar.SetFrameState( _frame_state );
		m_MenuBar.SethWnd( m_hWnd );
		m_MenuBar.m_pselectedMenuItem = &_selectedMenuItem;
        return 0;
    }

    void OnDestroy()
    {
        if (_rgn)
            DeleteObject(_rgn);
        
        HRGN hrgnPrev = ::CreateRectRgn(0, 0, 0, 0);
        ::GetWindowRgn(m_hWnd, hrgnPrev);
        int nRet = ::DeleteObject(hrgnPrev);
        
		//是否要删除,m_hMenu
		ASSERT(nRet);
    }

//    void SetWindowRgnEx(HRGN hRgn)
//    {
//        ASSERT( ::IsWindow(m_hWnd) );
//
//        int nRet = 0;
//
//        // delete previous region object
//        HRGN hrgnPrev = ::CreateRectRgn(0, 0, 0, 0);
//        ::GetWindowRgn(m_hWnd, hrgnPrev);
//        nRet = ::DeleteObject(hrgnPrev);
//        ASSERT( nRet );
//
//        if( hRgn )
//        {
////            RECT rc; ::GetWindowRect(m_hWnd, &rc);
////            OffsetRect(&rc, -rc.left, -rc.top);
////
////            ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);
//        }
//
//        return ::SetWindowRgn( m_hWnd, hRgn, TRUE );
//    }
//
//    HRGN ResizeRegion()
//    {
//        if(_rgn)
//        {
//            RECT rc; GetWindowRect(&rc);
//            OffsetRect(&rc, -rc.left, -rc.top);
//            SIZE sz = {rc.right, rc.bottom};
//
//            _rgn = SkinScheme::RegulateRegion( _rgnSource, &sz );
//        }
//        return _rgn;
//    }

    void OnSize(UINT nType, CSize)
    {
        // 注意Region是 Windows's Rect，而不是 Client's Rect
        SetMsgHandled(FALSE);
        
        if(SIZE_MAXIMIZED == nType && _rgn)
        {
            // SetWindowRgn(0);
            // 填充边角
            SetWindowRgn(NULL);
        }
        else if(_rgn)
        {
            WTL::CRect rcw;
            GetWindowRect(&rcw);

            ASSERT(OBJ_REGION == ::GetObjectType(_rgn));
            HRGN rgn_new = RegulateRegion(_rgn, &rcw.Size());
            ASSERT(OBJ_REGION == ::GetObjectType(rgn_new));
            
            // delete previous region object
            HRGN hrgnPrev = ::CreateRectRgn(0, 0, 0, 0);
            ::GetWindowRgn(m_hWnd, hrgnPrev);
            int nRet = ::DeleteObject(hrgnPrev);
            ASSERT(nRet);

            SetWindowRgn(rgn_new, TRUE);
        }

		InitMenu( m_MenuBar.GetMenu() );

        // TODO: 没有绘制正确
        // TODO: 没有得到正确的 Region in OnCreate
        // RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW
        RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
    }

	LRESULT OnEraseBkgnd(HDC hdc)
	{
		WTL::CRect rcw, rcc;

		GetWindowRect(&rcw);
		GetClientRect(&rcc);

		ClientToScreen(&rcc);
		rcc.OffsetRect(-rcc.left, -rcc.top);

		rcw.OffsetRect(-rcw.left, -rcw.top);

		FillRect(hdc, rcc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		return 1;
	}

	void OnPaint(HDC hdc)
	{
		WTL::CRect rcw, rcc;

		GetWindowRect(&rcw);
		GetClientRect(&rcc);

		ClientToScreen(&rcc);
		rcc.OffsetRect(-rcc.left, -rcc.top);

		rcw.OffsetRect(-rcw.left, -rcw.top);

		FillRect(hdc, rcc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}

	LRESULT OnSetCursor( HWND hWnd, UINT x, UINT y )
	{
		ModifyStyle( WS_VISIBLE, 0 );

		ControlT * pT = static_cast<ControlT*>(this);
		LRESULT lRet = pT->DefWindowProc();
		
		ModifyStyle( 0, WS_VISIBLE );

		return lRet;
	}

    void OnNcPaint(HRGN)
    {
        WTL::CWindowDC dc(m_hWnd);

        WTL::CRect rcw, rcc;

        GetWindowRect(&rcw);
        GetClientRect(&rcc);

        ClientToScreen(&rcc);
        rcc.OffsetRect(-rcw.left, -rcw.top);

        rcw.OffsetRect(-rcw.left, -rcw.top);

		//dc.FillSolidRect( rcw, RGB( 255, 0, 5));
		//return;
        DrawFrame(dc, rcw, rcc, GetStyle(), _frame_state);
    }

    void OnNcMouseMove(UINT nHitTest, WTL::CPoint point)
    {
        if(nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE
            || HTMENU == nHitTest )
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
            return;

        if ( HTMENU == nHitTest )
        {
			//
            int n = m_MenuBar.PtInMenu( point );

            if (n != _hoverMenuItem)
            {
                WTL::CWindowDC dc(m_hWnd);

                if (-1 != n)
                    m_MenuBar.EtchedMenuBarItem( (HDC)dc, m_MenuBar.GetMenu(), n, MS_SELECTED);

                if (_hoverMenuItem != -1)
                    m_MenuBar.EtchedMenuBarItem( (HDC)dc, m_MenuBar.GetMenu(), _hoverMenuItem, MS_NORMAL);

                _hoverMenuItem = n;
            }
        }
        else if (_hoverMenuItem != -1)
        {
            WTL::CWindowDC dc(m_hWnd);
            m_MenuBar.EtchedMenuBarItem( (HDC)dc, m_MenuBar.GetMenu(), _hoverMenuItem, MS_NORMAL);
            _hoverMenuItem = -1;
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

            //WTL::CWindowDC dc(m_hWnd);
            //WTL::CRect rcw;
            //GetWindowRect(&rcw);
            //rcw.OffsetRect(-rcw.left, -rcw.top);
            //EtchedSysButton((HDC)dc, rcw, sbState);
        }
        
        // if(HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest)
        // {
        //    return FALSE;
        // }
        // return TRUE;
    }
    
    void OnNcLButtonDown(UINT nHitTest, WTL::CPoint point)
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

            if ( HTMENU == nHitTest )
            {
                // sure ?
                _selectedMenuItem = m_MenuBar.PtInMenu( point );
				if (-1 != _selectedMenuItem)
                {
                    WTL::CWindowDC dc(m_hWnd);
                    m_MenuBar.EtchedMenuBarItem( (HDC)dc, m_MenuBar.GetMenu(), _selectedMenuItem, MS_SELECTED);
					m_MenuBar.TrackPopup( _selectedMenuItem );
                }
				
                return;
            }
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

            WTL::CWindowDC dc(m_hWnd);
            WTL::CRect rcw;
            GetWindowRect(&rcw);
            rcw.OffsetRect(-rcw.left, -rcw.top);

            EtchedSysButton((HDC)dc, rcw, sysbtn_state);
        }
        if (HTCAPTION != nHitTest && HTCLOSE != nHitTest && HTMAXBUTTON != nHitTest && HTMINBUTTON != nHitTest)
        {
            SetMsgHandled(FALSE);
            //CallWindowProc(gDialogProc, hWnd, WM_NCLBUTTONDOWN, (WPARAM)(UINT)(nHitTest),
             // MAKELPARAM((x), (y)));
        }
    }
    
    void OnNcLButtonUp(UINT nHitTest, WTL::CPoint point)
    {
        if (0 == _anchorDown)
            return;

        // TRACE("trace OnNcLButtonUp\n");

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

#if 0 // TODO: 被注释的代码似乎有错误，没有验证所有风格的窗口
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
#endif
        if( -1 != _hoverMenuItem)
        {
            HMENU hm = m_MenuBar.GetMenu();

            WTL::CWindowDC dc(m_hWnd);
            MENUSTATES ms = MS_NORMAL;
            if (_hoverMenuItem == _selectedMenuItem)
                ms = MS_DEMOTED;
            m_MenuBar.EtchedMenuBarItem((HDC)dc, hm, _hoverMenuItem, ms);

            _hoverMenuItem = -1;
        }
        
        // TODO: 设置sysbutton的专用标志
        {
            WTL::CWindowDC dc(m_hWnd);
            WTL::CRect rcw;
            GetWindowRect(&rcw);
            rcw.OffsetRect(-rcw.left, -rcw.top);
            EtchedSysButton((HDC)dc, rcw, sbState);
        }
    }

    void OnNcLButtonDblClk(UINT nHitTest, WTL::CPoint point)
    {
        if (HTCAPTION == nHitTest)
        {
            HICON hIcon = (HICON)SendMessage(WM_GETICON, ICON_SMALL, 0);
            WTL::CPoint pt = point;
            ScreenToClient(&pt);
            if (hIcon && pt.x < 16 + 9)
            {
                SendMessage(WM_SYSCOMMAND, SC_CLOSE, -1);
            }
        }
    }

    void OnMenuSelect(UINT nItem, UINT nFlag, HMENU menu)
    {
        // menu maybe popupmenu or sysmenu
        
        // TRACE("Frame::OnMenuSelect %d %x %p\n", nItem, nFlag, menu);

        HMENU hm = m_MenuBar.GetMenu();

        if (nFlag == 0xFFFF && menu == 0 && _selectedMenuItem != -1)
        {
            WTL::CWindowDC dc(m_hWnd);            

            m_MenuBar.EtchedMenuBarItem((HDC)dc, hm, _selectedMenuItem, MS_NORMAL);
            _selectedMenuItem = -1;
        }
        
        if (nFlag & MF_HILITE && menu == hm)
        {
            WTL::CWindowDC dc(m_hWnd);

            if (_selectedMenuItem != -1)
            {
                m_MenuBar.EtchedMenuBarItem((HDC)dc, hm, _selectedMenuItem, MS_NORMAL);
            }
            _selectedMenuItem = nItem;
            m_MenuBar.EtchedMenuBarItem((HDC)dc, hm, _selectedMenuItem, MS_DEMOTED);
        }
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
    //! 在NcMouseMove中设置/取消
    //! 在NcMouseLeave中取消
    UINT _hoverMenuItem;    // 鼠标曾经在过的 menu bar 上
    //! 在NcLButtonDown中设置
    UINT _selectedMenuItem; // 鼠标点击该 item 或者**曾经收到 WM_MENUSELECT 消息
    unsigned long _fTrackNcMouseLeave : 1;
    HRGN _rgn;

	//HMENU m_hMenu;		//把菜单记录下来 
	//std::vector<WTL::CRect>		m_MenuRectLst;//记录菜单的位置
	//BOOL m_bLoop;
	//static HHOOK m_hMenuMsgHook;
	//BOOL	m_bProcessLeftArrow;
	//BOOL	m_bProcessRightArrow;

	CSkinMenuBar	m_MenuBar;
	BOOL			m_bIgnoreAlt;
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


// ????????????????????????????????????????????????????????????????????
// 需要记录 placement 的元素：
// menu, system buttons
//
// ????????????????????????????????????????????????????????????????????
// part 相对于 parent window 的位置
// 或者说 windows 内，每个child part所处的位置
// WTL::CRect SkinScheme::GetPlacement(int part, WTL::CRect& rcparent);
//
// <area state="" placement="left, top, right, bottom" />
// eg: placement="0, 0, -10, -10" 表示靠右上
//     placement="10, 0, -10, 0"  表示靠左下
//
