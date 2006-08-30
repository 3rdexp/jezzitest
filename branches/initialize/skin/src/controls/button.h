#pragma once

#include "../base/skinctrl.h"


namespace Skin {

template<class BaseT = CButton>
struct SkinButton : public SkinControlImpl<SkinButton, BaseT>
{
    enum { class_id = BUTTON };

	typedef SkinButton<BaseT> this_type;
	typedef SkinControlImpl<SkinButton, BaseT> base_type;

	BEGIN_MSG_MAP(this_type)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
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
		MSG_WM_CREATE(OnCreate)
	END_MSG_MAP()

	BOOL OnCreate(LPCREATESTRUCT)
	{
		m_nPart = GetButtonPart();
		SetMsgHandled(FALSE);
		return TRUE;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CPaintDC dc(m_hWnd);
		DoPaint(dc);
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
					::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);		
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
	* ��  ����GetButtonTextFormat()									*
	* ��  �ܣ��õ�Button���ֵĶ��뷽ʽ(��DrawText()���ʱ�ĸ�ʽ��		*
	* ��  ����lStyle �ؼ����											*
	* ����ֵ����DrawText()���ʱ�ĸ�ʽ									*
	* ˵  ��: button�ϵ��ֱ�����һ��									*
	********************************************************************/
	UINT GetButtonTextFormat(const LONG lStyle)
	{
		UINT uFormat = DT_SINGLELINE;//button�ϵ��ֱ�����һ��

		//x����
		if ( (lStyle & BS_CENTER)==BS_CENTER )//x������
			uFormat |= DT_CENTER;
		else if ( (lStyle & BS_RIGHT)==BS_RIGHT )//x������
			uFormat |= DT_RIGHT;
		else if ( (lStyle & BS_LEFT) == BS_LEFT )//x������
			uFormat |= DT_LEFT;
		else//ȱʡ��x��
			uFormat |= DT_CENTER;

		//y����
		if ( (lStyle & BS_VCENTER ) == BS_VCENTER )//y����
			uFormat |= DT_VCENTER;
		else if ( (lStyle & BS_TOP)==BS_TOP )//y������
			uFormat |= DT_TOP;
		else if ( (lStyle & BS_BOTTOM)==BS_BOTTOM )//y������
			uFormat |= DT_BOTTOM;
		else//ȱʡ��y��
			uFormat |= DT_VCENTER;

		return uFormat;
	}
	/********************************************************************
	* ��  ����GetButtonPart(HWND hWnd)									*
	* ��  �ܣ��õ�button�Ļ������group��check��radio����ͨbutton)	*
	* ��  ������														*
	* ����ֵ�������ownerdraw����	�򷵻�BASETYPE_OWNERDRAW		*
	*		   �����groupbox			�򷵻�BASETYPE_GROUPBOX			*
	*		   �����radiobutton		�򷵻�BASETYPE_RADIOBUTTON		*
	*		   �����checkbox			�򷵻�BASETYPE_CHECKBOX			*
	*		   �������ͨ��ť			�򷵻�BASETYPE_BUTTON			*
	* ˵  ��:															*
	********************************************************************/
	int	GetButtonPart()
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
				return	PBS_DEFAULTED;
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

	LRESULT DoPaint(HDC dc)
	{
		CRect rc;
		GetClientRect(&rc);

		CMemoryDC memdc(dc, rc);
		int nState = GetState();
		if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, m_nPart, nState))
			_scheme->DrawParentBackground(m_hWnd, memdc, &rc);
		
		if (_scheme)
            _scheme->DrawBackground(memdc, class_id, m_nPart, nState, &rc, NULL );

		const int nLen = GetWindowTextLength();
		TCHAR * szText = new TCHAR[nLen + 1];
		int nTextLen = GetWindowText(szText, nLen + 1);

        // icon / bitmap ( TODO: button should have bitmap itself
		// text
		
        if (m_nPart == BP_PUSHBUTTON)
		{
			// if exist icon
			if (m_hIcon)
			{
				SIZE szIcon = SizeIcon(m_hIcon);			
				int nIconX = rc.left + ICON_LEFT;
				int nIconY = (rc.bottom - rc.top - szIcon.cy) / 2;
				BOOL bRet = DrawIconEx(memdc, nIconX, nIconY, m_hIcon, szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);
				rc.left = rc.left + szIcon.cx + ICON_LEFT; 
			}
		}

		LONG lStyle = GetStyle();
		if (_scheme)
            _scheme->DrawText(memdc, class_id, m_nPart, nState, szText, GetButtonTextFormat(lStyle), 0, &rc);

		_ASSERTE( _CrtCheckMemory( ) );
		delete [] szText;
		_ASSERTE( _CrtCheckMemory( ) );

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
	unsigned m_fPressed	   : 1;
	HICON m_hIcon;
	unsigned m_uFormat;
	int m_nPart;

	enum
	{
		ICON_LEFT = 5,
	};
};

}; // namespace 