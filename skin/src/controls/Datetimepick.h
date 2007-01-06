#pragma once

#include <atlwin.h>
#include "../base/skinctrl.h"
#include "../base/wclassdefines.h"

namespace Skin {
	
	const UINT TIMER_DROP = 1;

	template<class BaseT = WTL::CDateTimePickerCtrl>
	struct SkinDataTimePickerCtrl : public SkinControlImpl<SkinDataTimePickerCtrl, BaseT>
	{
		enum { class_id = DATATIMEPICKER };

		SkinDataTimePickerCtrl()
		{
			m_nPart		= CP_DROPDOWNBUTTON;
			m_bLBtnDown	= FALSE;
			m_bRefresh  = FALSE;
		}

		void OnFirstMessage()
		{
			int i = 0;
		}

		typedef SkinDataTimePickerCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinDataTimePickerCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
		//	if ( hWnd == m_hWnd )
		//		ATLTRACE("datatimepick msg is %04x \n", uMsg);
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)

			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			
			//NOTIFY_CODE_HANDLER(DTN_DROPDOWN, OnDropDown)
			//NOTIFY_CODE_HANDLER(DTN_CLOSEUP, OnDropClose)
			//NOTIFY_CODE_HANDLER(DTN_DATETIMECHANGE, OnDataTimeChange)

		END_MSG_MAP()


		LRESULT OnTimer( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{
			if ( wParam == TIMER_DROP )
			{
				//SetRedraw(TRUE);

				WTL::CClientDC dc( m_hWnd );
				CRect rcItem;
				GetClientRect( rcItem );
				int nState = GetState();
				DrawDropButton(dc, nState, rcItem );
			}
			

			bHandled = FALSE;
			return 0;
		}

		LRESULT OnDropDown( int wParam, LPNMHDR lParam, BOOL& bHandled)
		{
			m_bLBtnDown = TRUE;
			SendMessage(WM_NCPAINT);
			SetRedraw(FALSE);
			SetTimer(TIMER_DROP, 10, NULL);
			Invalidate();
			return 0;
		}
		
		LRESULT OnDropClose( int wParam, LPNMHDR lParam, BOOL& bHandled)
		{
			m_bLBtnDown = FALSE;
			SetRedraw(TRUE);
			KillTimer(TIMER_DROP);
			Invalidate();
			return 0;
		}

		LRESULT OnDataTimeChange( int wParam, LPNMHDR lParam, BOOL& bHandled)
		{
			Invalidate(FALSE);
			return 0;
		}

		WTL::CRect getDropButtonRect()
		{
			WTL::CRect rcItem;
			GetClientRect(&rcItem);

			WTL::CRect rcButton;
			rcButton.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB) - 1;
			rcButton.right = rcItem.right - 1;
			rcButton.top = rcItem.top + 1;
			rcButton.bottom = rcItem.bottom - 1;

			return rcButton;
		}

		void DrawDropButton( HDC hdc, int nState, WTL::CRect rcItem )
		{
			WTL::CRect rcButton;
			rcButton.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB) - 2;
			rcButton.right = rcItem.right;
			rcButton.top = rcItem.top + 1;
			rcButton.bottom = rcItem.bottom - 1;

			// fix icon combox draw
			FillRect(hdc, &rcButton, (HBRUSH)GetStockObject(WHITE_BRUSH));

			rcButton =  getDropButtonRect();

			if (_scheme)
				_scheme->DrawBackground(hdc, class_id, m_nPart, nState, &rcButton, NULL );
		}

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 0;
		}

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{

			HDC hdc = GetWindowDC( );
			WTL::CDC dc;
			dc.Attach(hdc);

			WTL::CRect rcItem;
			GetWindowRect(&rcItem);
			rcItem.right -= rcItem.left;
			rcItem.bottom -= rcItem.top;
			rcItem.top = rcItem.left = 0;

			int nState = GetState();

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
/*
			if (!(GetStyle() & DTS_UPDOWN))
			{
				DrawDropButton( dc, nState, rcItem );
			}	
*/
			dc.Detach();

			ReleaseDC( hdc );
			return 0;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CRect rc ;
			GetClientRect( rc );

			WTL::CPaintDC	dc( m_hWnd );
			WTL::CMemoryDC memdc( dc, rc );

			int nState = GetState();

			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, m_nPart, nState))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

			DoPaint(memdc, nState, rc);

			return 0;
		}

		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = 0;

			m_bLBtnDown = TRUE;

			if( IsWindowEnabled() )
			{

				//SetRedraw(FALSE);
				SetTimer(TIMER_DROP, 10, NULL);

				lRet = DefWindowProc();

				//SetRedraw(TRUE);
				KillTimer(TIMER_DROP);

				return lRet;


				WTL::CRect rcItem;
				GetClientRect( rcItem );

				WTL::CClientDC dc( m_hWnd );

				int nState = GetState();
				DrawDropButton(dc, nState, rcItem );

				return lRet;

				WTL::CRect rcButton;
				rcButton = getDropButtonRect();


				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

				if ( rcButton.PtInRect(pt) )
				{
					WTL::CMonthCalendarCtrl hMonthCal = GetMonthCal();

					RECT rcMonthCal;
					hMonthCal.SendMessage(MCM_GETMINREQRECT, 0, (LPARAM)&rcMonthCal);
					
					POINT monthcal_pos;
					if(GetStyle() & DTS_RIGHTALIGN)
						monthcal_pos.x = rcButton.left - 
						(rcMonthCal.right - rcMonthCal.left);
					else
						/* FIXME: this should be after the area reserved for the checkbox */
						monthcal_pos.x = rcItem.left;

					monthcal_pos.y = rcItem.bottom;
					ClientToScreen ( &monthcal_pos );

					
					hMonthCal.SetWindowPos(0, monthcal_pos.x,
						monthcal_pos.y, rcMonthCal.right - rcMonthCal.left,
						rcMonthCal.bottom - rcMonthCal.top, 0);

					if(hMonthCal.IsWindowVisible()) 
					{
						hMonthCal.ShowWindow( SW_HIDE );
					}
					else
					{
						SYSTEMTIME SysTime;
						GetSystemTime(&SysTime); 

						TRACE("update calendar %04d/%02d/%02d\n", 
							SysTime.wYear, SysTime.wMonth, SysTime.wDay);
						
						hMonthCal.SendMessage(MCM_SETCURSEL, 0, (LPARAM)(&SysTime));
						hMonthCal.ShowWindow(SW_SHOW);
					}

					Invalidate();
				}
				
				
			}
			
			return lRet;
		}

		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = 0;
			m_bLBtnDown = FALSE;
			//Invalidate();
			if( IsWindowEnabled() ) 
				lRet = DefWindowProc();
			
			WTL::CClientDC dc( m_hWnd );

			WTL::CRect rcItem;
			GetClientRect( rcItem );

			int nState = GetState();
			DrawDropButton(dc, nState, rcItem );

			//InvalidateRect(NULL, FALSE);
			return lRet;
		}



		void DoPaint(HDC hdc, int nState, RECT *pRect)
		{
			const int EDGE = 1;

			WTL::CDC dc;
			dc.Attach(hdc);

			WTL::CRect rcItem(*pRect);
			
			dc.FillSolidRect( rcItem, RGB( 255, 255, 255));

			WTL::CRect  rText;
			rText = rcItem;
			rText.right = rText.right - GetSystemMetrics(SM_CXHTHUMB) - 2;
			
			const int nLen = GetWindowTextLength();
			TCHAR * szText = new TCHAR[nLen + 1];
			int nTextLen = GetWindowText(szText, nLen + 1);

			HFONT hOldFont = dc.SelectFont(GetCtrlFont(m_hWnd));
			if (_scheme)
				_scheme->DrawText(dc, class_id, m_nPart, nState, szText, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS, 0, &rText);

			delete [] szText;

			if (!(GetStyle() & DTS_UPDOWN))
			{
				DrawDropButton(dc, nState, rcItem );
			}	

			dc.Detach();

		}

		static HFONT GetCtrlFont(HWND hwnd)
		{
			HFONT hFont;
			//如果sendmessage得到的是NULL,则使用的是系统字体
			if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
				hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

			return hFont;
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
		BOOL	m_bRefresh;

		enum
		{
			ICON_SPACE = 2,
		};
	};

}; // namespace 