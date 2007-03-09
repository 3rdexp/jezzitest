#pragma once

#include <atlwin.h>
#include "../base/skinctrl.h"
#include "../base/wclassdefines.h"

namespace Skin {

	template<class BaseT = WTL::CComboBox>
	struct SkinComboBox : public SkinControlImpl<SkinComboBox, BaseT>
	{
		SkinComboBox()
		{
			_nPart		= CP_DROPDOWNBUTTON;
			_bLBtnDown	= FALSE;
			_classid	= COMBOBOX;
		}

		void OnFirstMessage()
		{
		}

		typedef SkinComboBox<BaseT> this_type;
		typedef SkinControlImpl<SkinComboBox, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		END_MSG_MAP()


		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CRect rc ;
			GetClientRect( rc );

			WTL::CClientDC	dc( m_hWnd );
		
			int nState = GetState();

			LRESULT lRet = DefWindowProc();
			//if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, _nPart, nState))
			//	_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

			DoPaint(dc, nState, rc);
			
			return lRet;
		}

		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = 0;
			
			_bLBtnDown = TRUE;
			
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
			
			_bLBtnDown = FALSE;
			
			Invalidate();
			
			if( IsWindowEnabled() ) 
				lRet = DefWindowProc();
			
			return lRet;
		}

		void DoPaint(HDC hdc, int nState, RECT *pRect)
		{
			const int EDGE = 1;

			WTL::CDC dc;
			dc.Attach(hdc);

			WTL::CRect rcItem(*pRect);
			
			// Cover up dark 3D shadow.
			
			COLORREF cr;
			_scheme->GetColor(_classid, _nPart, nState, TMT_BORDERCOLOR, &cr);
			
			int nType = (GetStyle() & 0xf);

			if ( nType == CBS_SIMPLE )
			{
				CWindow wndLBox = GetChildWnd(WC_COMBOLBOX);

				if ( wndLBox.m_hWnd )
				{
					WTL::CRect rLBox;
					wndLBox.GetWindowRect( rLBox );
					wndLBox.ScreenToClient( rLBox );
					wndLBox.MapWindowPoints( m_hWnd, rLBox );
					
					rcItem.bottom = rLBox.top - EDGE * 2;
					//dc.ExcludeClipRect(rcItem.left, rLBox.top, rcItem.right, EDGE * 2);
				}
			}

			dc.Draw3dRect(rcItem, cr, cr);

			rcItem.DeflateRect(1,1);

			if (nState == CBXS_DISABLED) 
			{
				//2003-07-11
				dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),::GetSysColor(COLOR_BTNHIGHLIGHT));
			}
			else 
			{
				_scheme->GetColor(_classid, _nPart, nState, TMT_TEXTBORDERCOLOR, &cr);
				dc.Draw3dRect(rcItem, cr, cr); 
			}

			if ( nType != CBS_SIMPLE )
			{
				WTL::CRect rcButton;
				rcButton.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB) - 2;
				rcButton.right = rcItem.right;
				rcButton.top = rcItem.top + 1;
				rcButton.bottom = rcItem.bottom - 1;

				// fix icon combox draw
				FillRect(dc.m_hDC, &rcButton, (HBRUSH)GetStockObject(WHITE_BRUSH));

				rcButton.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB) - 1;
				rcButton.right = rcItem.right - 1;
				rcButton.top = rcItem.top + 1;
				rcButton.bottom = rcItem.bottom - 1;

				if (_scheme)
					_scheme->DrawBackground(dc, _classid, _nPart, nState, &rcButton, NULL );
			}
			
			dc.Detach();

		}

	
		int  GetState()
		{
			int nState = CBXS_NORMAL;
			if (!IsWindowEnabled())
				nState = CBXS_DISABLED;
			else
			{
				if (_bLBtnDown)
					nState = CBXS_PRESSED;
			}

			return nState;
		}

	private:
		int		_nPart;
		BOOL	_bLBtnDown;
	};

}; // namespace 