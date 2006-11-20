#pragma once

#include "../base/skinctrl.h"
#include <atlwin.h>

namespace Skin {

	template<class BaseT = WTL::CComboBox>
	struct SkinComboBox : public SkinControlImpl<SkinComboBox, BaseT>
	{
		enum { class_id = COMBOBOX };
		SkinComboBox()
		{
			m_nPart		= CP_DROPDOWNBUTTON;
			m_bLBtnDown	= FALSE;
		}
		typedef SkinComboBox<BaseT> this_type;
		typedef SkinControlImpl<SkinComboBox, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)

		END_MSG_MAP()


		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CRect rc ;
			GetClientRect( rc );

			WTL::CClientDC	dc( m_hWnd );
		
			int nState = GetState();

			LRESULT lRet = DefWindowProc();
			//if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, m_nPart, nState))
			//	_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

			DoPaint(dc, nState, rc);
			
			return 0;
		}

		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = 0;
			//LRESULT lRet = DefWindowProc();
			m_bLBtnDown = TRUE;
			if( IsWindowEnabled() )
			{
				lRet = DefWindowProc();
				Invalidate();
			}
			return lRet;
		}

		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = 0;
			m_bLBtnDown = FALSE;
			Invalidate();
			if( IsWindowEnabled() ) 
				lRet = DefWindowProc();
			
			return lRet;
		}

		

		void DoPaint(HDC hdc, int nState, RECT *pRect)
		{
			WTL::CDC dc;
			dc.Attach(hdc);

			WTL::CRect rcItem(*pRect);
			// Cover up dark 3D shadow.
			COLORREF cr;
			_scheme->GetColor(class_id, m_nPart, nState, TMT_BORDERCOLOR, &cr);

			dc.Draw3dRect(rcItem, cr, cr);

			rcItem.DeflateRect(1,1);

			if (nState == CBXS_DISABLED) 
			{
				//2003-07-11
				dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),::GetSysColor(COLOR_BTNHIGHLIGHT));
			}
			else 
			{
				_scheme->GetColor(class_id, m_nPart, nState, TMT_TEXTBORDERCOLOR, &cr);
				dc.Draw3dRect(rcItem, cr, cr); 
			}

			WTL::CRect rcButton;
			rcButton.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB) - 2;
			rcButton.right = rcItem.right;
			rcButton.top = rcItem.top + 1;
			rcButton.bottom = rcItem.bottom - 1;

			// fix icon combox draw
			FillRect(dc.m_hDC, &rcButton, (HBRUSH)GetStockObject(WHITE_BRUSH));
			
			rcButton.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB) - 1;
			rcButton.right = rcItem.right - 1;
			rcButton.top = rcItem.bottom - ::GetSystemMetrics(SM_CYVTHUMB) - 1;
			rcButton.bottom = rcItem.bottom - 1;

			if (_scheme)
				_scheme->DrawBackground(dc, class_id, m_nPart, nState, &rcButton, NULL );

			dc.Detach();

		}

	
		int  GetState()
		{
			int nState = CBXS_NORMAL;
			if (!IsWindowEnabled())
				nState = CBXS_DISABLED;
			else
			{
				if (m_bLBtnDown)
					nState = CBXS_PRESSED;
			}

			return nState;
		}
	private:
		int		m_nPart;
		BOOL	m_bLBtnDown;

		enum
		{
			ICON_SPACE = 2,
		};
	};

}; // namespace 