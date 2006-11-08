#pragma once

#include "../base/skinctrl.h"

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
			LRESULT lRet = DefWindowProc();

			RECT rc = {0};
			GetClientRect( &rc );

			WTL::CClientDC	dc( m_hWnd );
			
			WTL::CMemoryDC memdc(dc, rc);

			int nState = GetState();

			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, m_nPart, nState))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);
			
			DoPaint(memdc, nState, &rc);
			
			return 0;
		}

		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = DefWindowProc();
			m_bLBtnDown = TRUE;
			return lRet;
		}

		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = DefWindowProc();
			m_bLBtnDown = FALSE;
			return lRet;
		}

		void DoPaint(HDC hdc, int nState, RECT *pRect)
		{
			WTL::CDC dc;
			dc.Attach(hdc);

			WTL::CRect rcFill(*pRect);

			FillRect(dc.m_hDC, &rcFill, (HBRUSH)GetStockObject(WHITE_BRUSH));

			rcFill.left = rcFill.right - GetSystemMetrics(SM_CXHTHUMB) - ICON_SPACE;
			rcFill.right = rcFill.left + ICON_SPACE;

			

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
			// Cover up dark 3D shadow on drop arrow.
			rcItem.DeflateRect(1,1);
			rcItem.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB);
			rcItem.DeflateRect(1,1);

			WTL::CRect rcClient(*pRect);			
			WTL::CRect rcButton(rcClient.right - GetSystemMetrics(SM_CXHTHUMB) , rcClient.top + ICON_SPACE, rcClient.right - 1, rcClient.bottom - 1);

			// fix icon combox draw
			FillRect(dc.m_hDC, &rcButton, (HBRUSH)GetStockObject(WHITE_BRUSH));
			rcButton.left += 1;

			int nImgWidth =  GetSchemeWidth( m_nPart, nState );
			int nImgkHeight = GetSchemeHeight( m_nPart, nState );
			
			WTL::CRect rcbmp;
			rcbmp.left = rcButton.right - nImgWidth - 1;
			rcbmp.top = rcButton.bottom - nImgkHeight - 1;
			rcbmp.right = rcbmp.left + nImgWidth;
			rcbmp.bottom = rcbmp.top + nImgkHeight;
			if (_scheme)
				_scheme->DrawBackground(dc, class_id, m_nPart, nState, &rcbmp, NULL );

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