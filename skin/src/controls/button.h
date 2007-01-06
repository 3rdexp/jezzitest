#pragma once

#include "../base/skinctrl.h"


namespace Skin {

#define GROUPBOX_TEXT_SPACE                7    //groupbox���������ң��߿����

template<class BaseT = WTL::CButton>
struct SkinButton : public SkinControlImpl<SkinButton, BaseT>
{
    enum { class_id = BUTTON };

    SkinButton()
    {
        m_fMouseOver	= 0;
        m_fFocus		= 0;
        m_fPressed		= 0;

        m_hIcon         = 0;
		_classid		= class_id;
    }

	void OnFirstMessage()
	{
		int i = 0;
	}

    typedef SkinButton<BaseT> this_type;
    typedef SkinControlImpl<SkinButton, BaseT> base_type;

    BEGIN_MSG_MAP(this_type)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
        MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
        MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
        MESSAGE_HANDLER(WM_ENABLE, OnEnable)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
        MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
        MESSAGE_HANDLER(BM_SETCHECK, OnStateChange)
        MESSAGE_HANDLER(BM_SETSTATE, OnStateChange)
    END_MSG_MAP()

    BOOL OnCreate(LPCREATESTRUCT)
    {
        m_nPart = GetButtonPart();
        SetMsgHandled(FALSE);
        return TRUE;
    }

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0 ;
	}
	
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ( BP_USERBUTTON == m_nPart )
			return DefWindowProc( WM_PAINT, wParam, lParam );

		WTL::CPaintDC dc(m_hWnd);
		DoPaint( dc );
		return 0;
	}

	LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DoPaint( (HDC) wParam );
		return 0;
	}

    LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_fFocus = (uMsg == WM_SETFOCUS) ? 1 : 0;
        InvalidateRect(NULL, TRUE);
        UpdateWindow();
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_nPart == BP_PUSHBUTTON)
        {
            LRESULT lRet = 0;
            
			if(IsHoverMode())
                SetCapture();
            else
                lRet = DefWindowProc();

            if(GetCapture() == m_hWnd)
            {
                m_fPressed = 1;
                InvalidateRect(NULL, TRUE);
                UpdateWindow();
            }
            return lRet;
        }
        else
        {
            LRESULT lRet = 0;
            
			SetCapture();

            if(GetCapture() == m_hWnd)
            {
                lRet = DefWindowProc();
                m_fPressed = 1;
                InvalidateRect(NULL, TRUE);
                UpdateWindow();
            }
            
			return lRet;
        }
    }

    LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_nPart == BP_PUSHBUTTON)
        {
            LRESULT lRet = 0;

            if(!IsHoverMode())
                lRet = DefWindowProc();
            
			if(GetCapture() != m_hWnd)
                SetCapture();
            
			if(m_fPressed == 0)
            {
                m_fPressed = 1;
                InvalidateRect(NULL, TRUE);
                UpdateWindow();
            }
            
			return lRet;
        }
        else
        {
            LRESULT lRet = 0;
            
			if(GetCapture() != m_hWnd)
                SetCapture();
            
			if(m_fPressed == 0)
            {
                lRet = DefWindowProc();
                m_fPressed = 1;
                InvalidateRect(NULL, TRUE);
                UpdateWindow();
            }
            
			return lRet;
        }

    }
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_nPart == BP_PUSHBUTTON)
        {
            LRESULT lRet = 0;
            
			bool bHover = IsHoverMode();
            
			if(!bHover)
                lRet = DefWindowProc();
            
			if(GetCapture() == m_hWnd)
            {
                if(bHover && m_fPressed == 1)
                    ::PostMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
                ::ReleaseCapture();
            }
            
			return lRet;
        }
        else
        {
            LRESULT lRet = 0;
            
			if(GetCapture() == m_hWnd)
            {
                lRet = DefWindowProc();
                InvalidateRect(NULL, TRUE);
                UpdateWindow();
                ::ReleaseCapture();
            }
            
			return lRet;

        }
    }

    LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(m_fPressed == 1)
        {
            m_fPressed = 0;
            InvalidateRect(NULL, TRUE);
            UpdateWindow();
        }
       
		return DefWindowProc();
    }

    LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        BOOL bVisible = IsWindowVisible();
        if ( bVisible )
            ModifyStyle( WS_VISIBLE, 0 );

        LRESULT lRet = DefWindowProc();

        if ( bVisible )
            ModifyStyle( 0, WS_VISIBLE );

        Invalidate();
        return lRet;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(GetCapture() == m_hWnd)
        {
            POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ClientToScreen(&ptCursor);
            RECT rect = { 0 };
            GetWindowRect(&rect);
            unsigned int uPressed = ::PtInRect(&rect, ptCursor) ? 1 : 0;
            if(m_fPressed != uPressed)
            {
                m_fPressed = uPressed;
                InvalidateRect(NULL, TRUE);
                UpdateWindow();
            }
        }
        else if(IsHoverMode() && m_fMouseOver == 0)
        {
            m_fMouseOver = 1;
            InvalidateRect(NULL, TRUE);
            UpdateWindow();
            StartTrackMouseLeave();
        }
        return DefWindowProc();
    }

    LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(m_fMouseOver == 1)
        {
            m_fMouseOver = 0;
            InvalidateRect(NULL, TRUE);
            UpdateWindow();
        }
        return 0;
    }

    LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lRes = DefWindowProc();
        InvalidateRect(NULL, TRUE);
        UpdateWindow();
        return lRes;
    }

    LRESULT OnStateChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        Invalidate( TRUE );
        return DefWindowProc();
    }

    //////////////////////////////////////////////////////////////////////////
    SIZE SizeIcon(HICON hIcon)
    {
        SIZE resSize = {0,0};
        ICONINFO iconInfo;
        if (::GetIconInfo(hIcon, &iconInfo))
        {
            resSize.cx = iconInfo.xHotspot;
            resSize.cy = iconInfo.yHotspot;
        }
        return resSize;
    }

    /********************************************************************
    * ��  ����GetButtonTextFormat()                                        *
    * ��  �ܣ��õ�Button���ֵĶ��뷽ʽ(��DrawText()���ʱ�ĸ�ʽ��        *
    * ��  ����lStyle �ؼ����                                            *
    * ����ֵ����DrawText()���ʱ�ĸ�ʽ                                    *
    * ˵  ��: button�ϵ��ֱ�����һ��                                    *
    ********************************************************************/
    //////////////////////////////////////////////////////////////////////////
    // ˵�� ����Button�����ֵĸ�ʽ,��Ҫ���Ӹ����������

    UINT GetButtonTextFormat(const LONG lStyle )
    {


        UINT uFormat = 0; // = DT_SINGLELINE;//button�ϵ��ֱ�����һ��

        if ( (lStyle & BS_MULTILINE) != BS_MULTILINE )
            uFormat = DT_SINGLELINE;

        // 1 ����Y����Ķ��뷽ʽ,
        //y����
        if ( (lStyle & BS_VCENTER ) == BS_VCENTER )//y����
            uFormat |= DT_VCENTER;
        else if ( (lStyle & BS_TOP)==BS_TOP )//y������
            uFormat |= DT_TOP;
        else if ( (lStyle & BS_BOTTOM)==BS_BOTTOM )//y������
            uFormat |= DT_BOTTOM;
        else//ȱʡ��y��
            uFormat |= DT_VCENTER;

        // 2 ����X����Ķ��뷽ʽ,��Ҫ��Right Align Text ���Խ������
        //x����

        BOOL bLeftText = GetLeftTextFormat(lStyle);
        BOOL bRightAlignText = GetRightAlignTextFormat();

        if ( bRightAlignText )
        {
            if ( (lStyle & BS_CENTER)==BS_CENTER )//x������
                uFormat |= DT_CENTER;
            else if ( (lStyle & BS_RIGHT)==BS_RIGHT )//x������
                uFormat |= DT_RIGHT;
            else if ( (lStyle & BS_LEFT) == BS_LEFT )//x������
                uFormat |= DT_CENTER;
            else//ȱʡ��x��
                uFormat |= DT_RIGHT;
        }
        else
        {
            if ( (lStyle & BS_CENTER)==BS_CENTER )//x������
                uFormat |= DT_CENTER;
            else if ( (lStyle & BS_RIGHT)==BS_RIGHT )//x������
                uFormat |= DT_RIGHT;
            else if ( (lStyle & BS_LEFT) == BS_LEFT )//x������
                uFormat |= DT_LEFT;
            else//ȱʡ��x��
            {
                if ( m_nPart == BP_PUSHBUTTON )
                    uFormat |= DT_CENTER;
                else
                    uFormat |= BS_LEFT;
            }
        }

        return uFormat;
    }

    /********************************************************************
    * ��  ����GetVertAlignmentFormat()                                    *
    * ��  �ܣ��õ�Button��Vertical�Ķ��뷽ʽ                            *
    * ��  ����lStyle �ؼ����                                            *
    * ����ֵ������Check,Radioǰ���Сͼ��λ����                            *
    ********************************************************************/
    UINT GetVertAlignmentFormat(const LONG lStyle)
    {
        //y����
        if ( (lStyle & BS_VCENTER ) == BS_VCENTER )//y����
            return DT_VCENTER;
        else if ( (lStyle & BS_TOP)==BS_TOP )//y������
            return DT_TOP;
        else if ( (lStyle & BS_BOTTOM)==BS_BOTTOM )//y������
            return  DT_BOTTOM;
        else//ȱʡ��y��
            return  DT_VCENTER;
    }

    /********************************************************************
    * ��  ����GetLeftTextFormat()                                    *
    * ��  �ܣ��õ�Check Radio��Left Text�Ķ��뷽ʽ                *
    * ��  ����lStyle �ؼ����                                            *
    * ����ֵ������Check,Radioǰ���Сͼ������������뻹���Ҷ���            *
    ********************************************************************/
    BOOL GetLeftTextFormat(const LONG lStyle)
    {
        //y����
        if ( (lStyle & BS_LEFTTEXT ) == BS_LEFTTEXT )//y����
            return TRUE;
        else
            return  FALSE;
    }

    /********************************************************************
    * ��  ����GetRightAlignTextFormat()                                    *
    * ��  �ܣ��õ�Right Align Text�Ķ��뷽ʽ                *
    * ��  ����lStyle �ؼ����                                            *
    * ����ֵ������Check,Radioǰ���Сͼ������������뻹���Ҷ���            *
    ********************************************************************/
    BOOL GetRightAlignTextFormat( )
    {
        LONG lStyle = GetExStyle();
        //y����
        if ( (lStyle & WS_EX_RIGHT ) == WS_EX_RIGHT )//y����
            return TRUE;
        else
            return  FALSE;
    }



    /********************************************************************
    * ��  ����GetButtonPart(HWND hWnd)                                    *
    * ��  �ܣ��õ�button�Ļ������group��check��radio����ͨbutton)    *
    * ��  ������                                                        *
    * ����ֵ�������ownerdraw����    �򷵻�BASETYPE_OWNERDRAW        *
    *           �����groupbox            �򷵻�BASETYPE_GROUPBOX            *
    *           �����radiobutton        �򷵻�BASETYPE_RADIOBUTTON        *
    *           �����checkbox            �򷵻�BASETYPE_CHECKBOX            *
    *           �������ͨ��ť            �򷵻�BASETYPE_BUTTON            *
    * ˵  ��:                                                            *
    ********************************************************************/
    int    GetButtonPart()
    {
        long lStyle = GetStyle();
        if ( (lStyle & BS_OWNERDRAW) == BS_OWNERDRAW )//ownerdraw
            return BP_USERBUTTON;

        if ((lStyle & BS_GROUPBOX)==BS_GROUPBOX)//groupbox
            return BP_GROUPBOX;
        else if ((lStyle & BS_CHECKBOX)==BS_CHECKBOX
            || (lStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX  )//checkbox
            return BP_CHECKBOX;
        else if ((lStyle & BS_AUTORADIOBUTTON)==BS_AUTORADIOBUTTON
            || (lStyle & BS_RADIOBUTTON)==BS_RADIOBUTTON)//radiobox
            return BP_RADIOBUTTON;

        //��ͨ��ť
        return BP_PUSHBUTTON;
    }

    void GetCheckImgRect(const WTL::CRect& rcClient, int nState,  WTL::CRect& rcImg, WTL::CRect& rcText)
    {
        long lStyle = GetStyle();

        int nCheckWidth =  GetSchemeWidth( m_nPart, nState );
        int nCheckHeight = GetSchemeHeight( m_nPart, nState );

        UINT nVertical = GetVertAlignmentFormat(lStyle);
        //y����

        rcText.top = rcClient.top;
        rcText.bottom = rcClient.bottom;

        if ( nCheckHeight > rcClient.Height() )
        {
            rcImg.top = rcClient.top;
            rcImg.bottom = rcClient.bottom;
        }
        else
        {
            if ( nVertical ==  DT_VCENTER)//y����
            {
                rcImg.top = rcClient.top + ( rcClient.Height() - nCheckHeight ) / 2;
                rcImg.bottom = rcImg.top + nCheckHeight;
            }
            else if ( nVertical ==  DT_TOP )//y������
            {
                rcImg.top = rcClient.top ;
                rcImg.bottom = rcImg.top + nCheckHeight;
            }
            else if ( nVertical ==  DT_BOTTOM )//y������
            {
                rcImg.top = rcClient.bottom - nCheckHeight;
                rcImg.bottom = rcClient.bottom;
            }
        }

        // x ����
        BOOL bLeft = GetLeftTextFormat( lStyle );

        if ( nCheckWidth > rcClient.Width() )
        {
            rcImg.left = rcClient.left;
            rcImg.right = rcClient.right;

            rcText.left = rcText.right = rcText.top = rcText.bottom = 0;
        }
        else
        {
            if ( !bLeft )
            {
                rcImg.left = rcClient.left;
                rcImg.right = rcImg.left + nCheckWidth;

                rcText.left = rcImg.right + ICON_LEFT;
                rcText.right = rcClient.right;
            }
            else
            {
                rcImg.left = rcClient.right - nCheckWidth;
                rcImg.right = rcClient.right ;

                rcText.left = rcClient.left;
                rcText.right = rcImg.left - ICON_LEFT;
            }
        }
    }

    BOOL StartTrackMouseLeave()
    {
        TRACKMOUSEEVENT tme = { 0 };
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        return _TrackMouseEvent(&tme);
    }

    bool IsHoverMode()
    {
        return true;
    }

    int  GetState()
    {
        if (m_nPart == BP_PUSHBUTTON)
        {
            LONG lStyle = GetStyle();
            BOOL bEnabled = !(lStyle & WS_DISABLED);//�Ƿ񱻽�ֹ
            BOOL bDefault = (lStyle & BS_DEFPUSHBUTTON);

            bool bHover = IsHoverMode();

            if (m_fPressed == 1)
                return PBS_PRESSED;
            else if((!bHover && m_fFocus == 1) || (bHover && m_fMouseOver == 1))
                return PBS_HOT;
            else if(!bEnabled)
                return PBS_DISABLED;
            else if (bDefault)
                return    PBS_DEFAULTED;
            else
                return PBS_NORMAL;
        }
        else if (m_nPart == BP_CHECKBOX)
        {
            LONG lStyle = GetStyle();
            BOOL bEnabled = !(lStyle & WS_DISABLED);//�Ƿ񱻽�ֹ
            BOOL bChecked = GetCheck();
            bool bHover = IsHoverMode();
            if (!bEnabled)
            {
                return bChecked ?  CBS_CHECKEDDISABLED:CBS_UNCHECKEDDISABLED;
            }
            else
            {
                if (m_fPressed == 1)
                    return bChecked ?  CBS_CHECKEDPRESSED:CBS_UNCHECKEDPRESSED;
                else if((!bHover && m_fFocus == 1) || (bHover && m_fMouseOver == 1))
                    return bChecked ?  CBS_CHECKEDHOT:CBS_UNCHECKEDHOT;
                else
                    return bChecked ?  CBS_CHECKEDNORMAL:CBS_UNCHECKEDNORMAL;
            }
        }
        else if (m_nPart == BP_GROUPBOX)
        {
            LONG lStyle = GetStyle();
            BOOL bEnabled = !(lStyle & WS_DISABLED);//�Ƿ񱻽�ֹ
            if(!bEnabled)
                return  GBS_DISABLED ;
            else
                return GBS_NORMAL;
        }
        else if (m_nPart == BP_RADIOBUTTON)
        {
            LONG lStyle = GetStyle();
            BOOL bEnabled = !(lStyle & WS_DISABLED);//�Ƿ񱻽�ֹ
            BOOL bChecked = GetCheck();
            bool bHover = IsHoverMode();
            if (!bEnabled)
            {
                return bChecked ?  RBS_CHECKEDDISABLED:RBS_UNCHECKEDDISABLED;
            }
            else
            {
                if (m_fPressed == 1)
                    return bChecked ?  RBS_CHECKEDPRESSED:RBS_UNCHECKEDPRESSED;
                else if((!bHover && m_fFocus == 1) || (bHover && m_fMouseOver == 1))
                    return bChecked ?  RBS_CHECKEDHOT:RBS_UNCHECKEDHOT;
                else
                    return bChecked ?  RBS_CHECKEDNORMAL:RBS_UNCHECKEDNORMAL;
            }
        }
        return 0;
    }

    static HFONT GetCtrlFont(HWND hwnd)
    {
        HFONT hFont;
        //���sendmessage�õ�����NULL,��ʹ�õ���ϵͳ����
        if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
            hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

        return hFont;
    }

    void DrawGroup(HDC hdc)
    {
        WTL::CDC dc;
        dc.Attach(hdc);
        WTL::CRect rect;//�ؼ�����
        GetClientRect(&rect);

        rect.bottom --;
        rect.right --;
        WTL::CPen pen;
        pen.CreatePen(PS_SOLID,1,0xBFD0D0);//�߿��
        HPEN hOldPen = dc.SelectPen(pen.m_hPen);

        char sCaption[256];//�ؼ�����
        GetWindowText(sCaption,256);
        if ( strlen(sCaption) == 0 )//û�����֣�ֻ�����߿�
        {
			if (_scheme)
				_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rect );

			/*
            dc.MoveTo(rect.left,rect.top);
            dc.LineTo(rect.right,rect.top);
            dc.LineTo(rect.right,rect.bottom);
            dc.LineTo(rect.left,rect.bottom);
            dc.LineTo(rect.left,rect.top);

            dc.SelectPen(hOldPen);
            dc.Detach();
            */
			return;
        }

        //����
        HFONT hOldFont = dc.SelectFont(GetCtrlFont(m_hWnd));
        WTL::CSize Extent;
        if (!dc.GetTextExtent(sCaption, lstrlen(sCaption), &Extent))
            return;//�õ�����ռ�ռ����

        rect.top += Extent.cy / 2;
        WTL::CPoint ptText;//�������Ͻ�λ��
        ptText.y = 0 ;//rect.top + Extent.cy/2;//y�����ǲ����

        LONG lStyle = GetWindowLong(GWL_STYLE);
        //���£�GROUPBOX_TEXT_SPACE��ʾ������ߣ��ұߣ���ؼ����ң��ľ���
        if ( (lStyle & BS_CENTER) == BS_CENTER )//x�������־���
        {
            ptText.x = rect.Width() / 2 - Extent.cx / 2;//�������Ͻ�x����

            if ( ptText.x > rect.left )//����û�г���rect��߽�
            {
				dc.ExcludeClipRect( ptText.x - 1, 0, ptText.x + Extent.cx + 1, Extent.cy);

				if (_scheme)
					_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rect );
				
				dc.SelectClipRgn( NULL );
				/*
                dc.MoveTo(ptText.x-1, rect.top);
                dc.LineTo(rect.left,rect.top);
                dc.LineTo(rect.left,rect.bottom);
                dc.LineTo(rect.right,rect.bottom);
                dc.LineTo(rect.right,rect.top);
                //�����Ǿ��У���Ȼû������߽磬��һ��Ҳû�г����ұ߽�
                dc.LineTo(ptText.x+Extent.cx,rect.top);
				*/
            }
            else//���ֳ�������߽磬���ұ߽�����ֵ��¿�ʼ��
            {
                int nYTop = ptText.y + Extent.cy;
				WTL::CRect rectDraw = rect;
				rectDraw.top = nYTop + 1;
				if (_scheme)
					_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rectDraw );
				/*
                dc.MoveTo(rect.left,nYTop+1);
                dc.LineTo(rect.left,rect.bottom);
                dc.LineTo(rect.right,rect.bottom);
                dc.LineTo(rect.right,nYTop);
				*/
            }
        }
        else if ( (lStyle & BS_RIGHT)==BS_RIGHT )//x���������Ҷ���
        {
            ptText.x = rect.right - GROUPBOX_TEXT_SPACE - Extent.cx;//�������Ͻ�x����

			/*
            //�Ҷ��룬��֤�ұߵ�����
            dc.MoveTo(rect.right-GROUPBOX_TEXT_SPACE,rect.top);
            dc.LineTo(rect.right,rect.top);
            dc.LineTo(rect.right,rect.bottom);
            dc.LineTo(rect.left,rect.bottom);
			*/

            if ( ptText.x > rect.left )//�������û�����ؼ���߽�
            {
				dc.ExcludeClipRect( ptText.x - 1, 0, rect.right - GROUPBOX_TEXT_SPACE, Extent.cy);

				if (_scheme)
					_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rect );

				dc.SelectClipRgn( NULL );
				/*
                dc.LineTo(rect.left,rect.top);
                dc.LineTo(ptText.x,rect.top);
				*/
            }
            else//������߳����˿ؼ���߽�
			{
				dc.ExcludeClipRect( GROUPBOX_TEXT_SPACE, 0, rect.right - GROUPBOX_TEXT_SPACE, Extent.cy);

				if (_scheme)
					_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rect );

				dc.SelectClipRgn( NULL );

				/*
                dc.LineTo(rect.left,ptText.y+Extent.cy);
				*/
			}
        }
        else//x������������루ȱʡ��
        {
            ptText.x = rect.left + GROUPBOX_TEXT_SPACE;//�������Ͻ�x����

			/*
            //��������룬��֤�������������
            dc.MoveTo(rect.left+GROUPBOX_TEXT_SPACE-1,rect.top);
            dc.LineTo(rect.left,rect.top);
            dc.LineTo(rect.left,rect.bottom);
            dc.LineTo(rect.right,rect.bottom);
			*/

            if ( ptText.x+Extent.cx < rect.right)//����û�����ؼ��ұ߽�
            {
				dc.ExcludeClipRect( rect.left+GROUPBOX_TEXT_SPACE - 1, 0, ptText.x + Extent.cx + 1, Extent.cy);

				if (_scheme)
					_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rect );

				dc.SelectClipRgn( NULL );
				/*
                dc.LineTo(rect.right,rect.top);
                dc.LineTo(ptText.x + Extent.cx,rect.top);
				*/
            }
            else//���ֳ����ؼ��ұ߽�
			{

				dc.ExcludeClipRect( GROUPBOX_TEXT_SPACE, 0, rect.right - GROUPBOX_TEXT_SPACE, Extent.cy);

				if (_scheme)
					_scheme->TransparentDraw(dc, class_id, BP_GROUPBOX, GBS_NORMAL, &rect );

				dc.SelectClipRgn( NULL );

				/*
                dc.LineTo(rect.right,ptText.y+Extent.cy);
				*/
			}
        }

        WTL::CRect rcText;//�������ھ���
        rcText.left = ptText.x;
        rcText.top = ptText.y;
        rcText.right = rcText.left+Extent.cx;
        rcText.bottom = rcText.top+Extent.cy;

        dc.DrawText(sCaption, lstrlen(sCaption),rcText,DT_LEFT|DT_TOP|DT_SINGLELINE);

        dc.SelectFont(hOldFont);
        dc.SelectPen(hOldPen);
        dc.Detach();
    }
    
    LRESULT DoPaint( HDC dc )
    {
        WTL::CRect rc;
        GetClientRect(&rc);
  
		WTL::CMemoryDC memdc(dc, rc);

		int nState = GetState();

		if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, m_nPart, nState))
			_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

		if ( BP_GROUPBOX == m_nPart )
			BitBlt( memdc, 0, 0, rc.Width(), rc.Height(), dc, 0, 0, SRCCOPY);

        LONG lStyle = GetStyle();

        if (m_nPart == BP_PUSHBUTTON)
        {
            if (_scheme)
                _scheme->DrawBackground(memdc, class_id, m_nPart, nState, &rc, NULL );

            // icon / bitmap ( TODO: button should have bitmap itself
            // text

			// �ж��Ƿ���ϵͳ��icon
			HICON hIcon = (HICON)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_ICON, 0L);
			if ( hIcon )
			{
				SIZE szIcon = SizeIcon(hIcon);
				int nIconX = (rc.right - rc.left - szIcon.cx) / 2;
				int nIconY = (rc.bottom - rc.top - szIcon.cy) / 2;
				BOOL bRet = DrawIconEx(memdc, nIconX, nIconY, hIcon, szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);
				return S_OK;
			}

			WTL::CBitmapHandle hBitmap = WTL::CBitmapHandle((HBITMAP)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0L));
			if ( hBitmap )
			{
				SIZE sz;
				hBitmap.GetSize(sz);
				int nX = (rc.right - rc.left - sz.cx) / 2;
				int nY = (rc.bottom - rc.top - sz.cy) / 2;

				WTL::CClientDC cdc(m_hWnd);
				HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
				HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC, hBitmap);
				::BitBlt(memdc, nX, nY, sz.cx, sz.cy, hDC, 0, 0, SRCCOPY);
				SelectObject(hDC, pOldBitmapImage);
				::DeleteObject(hDC);
				::ReleaseDC(m_hWnd, cdc);
				return S_OK;
			}
            // if exist icon
			/*
            if (m_hIcon)
            {
                SIZE szIcon = SizeIcon(m_hIcon);
                int nIconX = rc.left + ICON_LEFT;
                int nIconY = (rc.bottom - rc.top - szIcon.cy) / 2;
                BOOL bRet = DrawIconEx(memdc, nIconX, nIconY, m_hIcon, szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);
                rc.left = rc.left + szIcon.cx + ICON_LEFT;
            }
			*/

			const int nLen = GetWindowTextLength();
			TCHAR * szText = new TCHAR[nLen + 1];
			int nTextLen = GetWindowText(szText, nLen + 1);

			HFONT hOldFont = memdc.SelectFont(GetCtrlFont(m_hWnd));
			if (_scheme)
				_scheme->DrawText(memdc, class_id, m_nPart, nState, szText, GetButtonTextFormat(lStyle), 0, &rc);

			_ASSERTE( _CrtCheckMemory( ) );
			delete [] szText;
			_ASSERTE( _CrtCheckMemory( ) );

			memdc.SelectFont(hOldFont);

        }
        else if ( m_nPart == BP_CHECKBOX || m_nPart == BP_RADIOBUTTON )
        {
            // 1 ����ǰ��[o]��λ�ú����ֵ�λ��
            WTL::CRect rcImg;
            WTL::CRect rcText;
            GetCheckImgRect( rc, nState, rcImg, rcText );

            if (_scheme)
                _scheme->TransparentDraw(memdc, class_id, m_nPart, nState, rcImg.left, rcImg.top);

            rc = rcText;


			const int nLen = GetWindowTextLength();
			TCHAR * szText = new TCHAR[nLen + 1];
			int nTextLen = GetWindowText(szText, nLen + 1);

			HFONT hOldFont = memdc.SelectFont(GetCtrlFont(m_hWnd));
			if (_scheme)
				_scheme->DrawText(memdc, class_id, m_nPart, nState, szText, GetButtonTextFormat(lStyle), 0, &rc);

			_ASSERTE( _CrtCheckMemory( ) );
			delete [] szText;
			_ASSERTE( _CrtCheckMemory( ) );

			memdc.SelectFont(hOldFont);

        }
        else if ( m_nPart == BP_GROUPBOX )
        {
            DrawGroup(memdc);
			//DefWindowProc();
        }
#if 0
        HDC d = ::GetDC(0);
        BitBlt(d, 0, 0, 100, 100, memdc, 0, 0, SRCCOPY);
        ::ReleaseDC(0, d);
#endif
        return S_OK;
    }

private:
    unsigned m_fMouseOver  : 1;
    unsigned m_fFocus      : 1;
    unsigned m_fPressed       : 1;
    HICON m_hIcon;
    unsigned m_uFormat;
    int m_nPart;

    enum
    {
        ICON_LEFT = 5,
    };
};

}; // namespace

