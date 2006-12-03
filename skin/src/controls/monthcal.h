#pragma once

#include <atlwin.h>
#include "../base/skinctrl.h"
#include "../base/wclassdefines.h"



namespace Skin {

	template<class BaseT = WTL::CMonthCalendarCtrl>
	struct SkinMonthCal: public SkinControlImpl<SkinMonthCal, BaseT>
	{
		enum { class_id = MONTHCALCTL };

		enum
		{
			BUTTON_LEFT = 5,
			BUTTON_TOP  = 6,
		};

		SkinMonthCal()
		{
			m_nPart		= 1;
			m_bLBtnDown	= FALSE;
		}
		typedef SkinMonthCal<BaseT> this_type;
		typedef SkinControlImpl<SkinMonthCal, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		END_MSG_MAP()

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LONG lExStyle;
			lExStyle = GetExStyle();
			if (( GetStyle() & WS_BORDER) ||
				(lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
			{		
				CRect rcw;
				GetWindowRect(&rcw);
				rcw.right -= rcw.left;
				rcw.bottom -= rcw.top;
				rcw.top = rcw.left = 0;

				HDC hdc = GetWindowDC(  );

				// 绘制外框
				CBrush brBorder;
				int nState = GetState();
				COLORREF cr;
				_scheme->GetColor(class_id, m_nPart, nState, TMT_BORDERCOLOR, &cr);
				brBorder.CreateSolidBrush( cr ); 
				FrameRect(hdc, CRect(0, 0, rcw.Width(), rcw.Height()), (HBRUSH)brBorder);
				brBorder.DeleteObject();

				// 绘制内框

				if ((lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
				{
					InflateRect(&rcw, -1, -1);
					LONG lStyle = GetStyle();			
					CBrush brBorder;
					_scheme->GetColor(class_id, m_nPart, nState, TMT_TEXTBORDERCOLOR, &cr);
					brBorder.CreateSolidBrush( cr ); 
					FrameRect(hdc, &rcw, (HBRUSH) brBorder);			
					if ((lExStyle & WS_EX_CLIENTEDGE) && (lExStyle & WS_EX_STATICEDGE))
					{
						InflateRect(&rcw, -1, -1);
						FrameRect(hdc, &rcw, (HBRUSH)brBorder);
					}
					brBorder.DeleteObject();
				}

				ReleaseDC( hdc );
			}
			else
			{
				LRESULT lRet = DefWindowProc();
				return lRet;
			}

			return 0;
		}


		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CRect rc ;
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
				/*
				WTL::CClientDC	dc( m_hWnd );

				WTL::CRect rc ;
				GetClientRect( rc );

				int nState = GetState();

				//lRet = DefWindowProc();

				DoPaint(dc, nState, rc);
				*/
				//InvalidateRect(rc);
			}
			return lRet;
		}

		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = 0;
			m_bLBtnDown = FALSE;
			if( IsWindowEnabled() )
			{
				lRet = DefWindowProc();
				Invalidate();
				/*

				WTL::CClientDC	dc( m_hWnd );

				WTL::CRect rc ;
				GetClientRect( rc );

				int nState = GetState();

				DoPaint(dc, nState, rc);
				*/
				//InvalidateRect(rc);
			}
			return lRet;
		}

		WTL::CRect GetLeftButtonRect()
		{


			WTL::CRect rcItem;
			GetClientRect( rcItem );
	
			WTL::CRect rcReqRect;
			GetMinReqRect(rcReqRect);

			
			for ( int i = 0; i < 12; i++ )
			{
				if ( rcReqRect.Width() * ( i + 1 ) + i * 7 > rcItem.Width() )
					break;
			}
			int nCol = i + 1;


			for ( i = 0; i < 12; i++ )
			{
				if ( rcReqRect.Height() * ( i + 1 ) + i * 7 > rcItem.Height() )
					break;
			}
			int nRow = i + 1;
			
			while ((nRow * nCol) > 12)
			{
				if (nRow > nCol)
					nRow--;
				else
					nCol--;
			}

			rcReqRect.right = rcReqRect.Width() * nCol + ( nCol - 1 ) * 7 + rcReqRect.left;
			rcReqRect.bottom = rcReqRect.Height() * nRow + ( nRow - 1 ) * 7 + rcReqRect.top;

			WTL::CRect rcButton;
			if ( rcItem.Width() > rcReqRect.Width() )
				rcButton.left = ( rcItem.Width() - rcReqRect.Width() ) / 2 + BUTTON_LEFT;
			else
				rcButton.left = BUTTON_LEFT;

			WTL::CClientDC	dc( m_hWnd );
			CSize sz;
			dc.GetTextExtent("2006", 4, &sz);

			if ( rcItem.Height() > rcReqRect.Height() )
			{
				if ( GetStyle() & MCS_NOTODAY )
					rcButton.top = ( rcItem.Height() - rcReqRect.Height() ) / 2 + BUTTON_TOP - sz.cy / 2;
				else
					rcButton.top = ( rcItem.Height() - rcReqRect.Height() ) / 2 + BUTTON_TOP;

			}
			else
				rcButton.top = BUTTON_TOP;

			NONCLIENTMETRICS ncm;
			ncm.cbSize = sizeof(ncm);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
			rcButton.right = rcButton.left + (int)ncm.iScrollWidth * 4 / 3;
			rcButton.bottom = rcButton.top + (int)ncm.iScrollWidth ;
			return rcButton;

		}

		WTL::CRect GetRightButtonRect()
		{


			WTL::CRect rcItem;
			GetClientRect( rcItem );

			WTL::CRect rcReqRect;
			GetMinReqRect(rcReqRect);

			for ( int i = 0; i < 12; i++ )
			{
				if ( rcReqRect.Width() * ( i + 1 ) + i * 6 > rcItem.Width() )
					break;
			}
			int nCol = i + 1;


			for ( i = 0; i < 12; i++ )
			{
				if ( rcReqRect.Height() * ( i + 1 ) + i * 6 > rcItem.Height() )
					break;
			}
			int nRow = i + 1;

			while ((nRow * nCol) > 12)
			{
				if (nRow > nCol)
					nRow--;
				else
					nCol--;
			}

			rcReqRect.right = rcReqRect.Width() * nCol + ( nCol - 1 ) * 6 + rcReqRect.left;
			rcReqRect.bottom = rcReqRect.Height() * nRow + ( nRow - 1 ) * 6 + rcReqRect.top;

			WTL::CRect rcButton;
			if ( rcItem.Width() + 4 >= rcReqRect.Width()  )
				rcButton.right =  ( rcItem.Width() + rcReqRect.Width() ) / 2 - BUTTON_LEFT;
			else
				rcButton.right = rcReqRect.Width() - BUTTON_LEFT;

			WTL::CClientDC	dc( m_hWnd );
			CSize sz;
			dc.GetTextExtent("2006", 4, &sz);

			if ( rcItem.Height() > rcReqRect.Height() )
				if ( GetStyle() & MCS_NOTODAY )
					rcButton.top = ( rcItem.Height() - rcReqRect.Height() ) / 2 + BUTTON_TOP - sz.cy / 2;
				else
					rcButton.top = ( rcItem.Height() - rcReqRect.Height() ) / 2 + BUTTON_TOP;
			else
				rcButton.top = BUTTON_TOP;

			NONCLIENTMETRICS ncm;
			ncm.cbSize = sizeof(ncm);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
			rcButton.left = rcButton.right - (int)ncm.iScrollWidth * 4 / 3;
			rcButton.bottom = rcButton.top + (int)ncm.iScrollWidth ;
			return rcButton;

		}

		void DoPaint(HDC hdc, int nState, RECT *pRect)
		{
			const int EDGE = 1;

			WTL::CDC dc;
			dc.Attach(hdc);

			WTL::CRect rcButton;
			//left
			rcButton = GetLeftButtonRect();

			if (_scheme)
					_scheme->DrawBackground(dc, class_id, m_nPart, ABS_LEFTNORMAL, &rcButton, NULL );

			rcButton = GetRightButtonRect();

			if (_scheme)
				_scheme->DrawBackground(dc, class_id, m_nPart, ABS_RIGHTNORMAL, &rcButton, NULL );

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