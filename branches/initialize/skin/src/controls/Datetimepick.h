#pragma once

#include <atlwin.h>
#include "../base/skinctrl.h"
#include "../base/wclassdefines.h"

namespace Skin {
	
	const UINT TIMER_DROP = 1;
	const int SCROLLCX = ::GetSystemMetrics(SM_CXHSCROLL);

	template<class BaseT = WTL::CDateTimePickerCtrl>
	struct SkinDateTimePickerCtrl : public SkinControlImpl<SkinDateTimePickerCtrl, BaseT>
	{
		//enum { class_id = DATATIMEPICKER };
		
		SkinDateTimePickerCtrl()
		{
			m_nPart		= CP_DROPDOWNBUTTON;
			m_bLBtnDown	= FALSE;
			m_bRefresh  = FALSE;
			_classid	= DATETIMEPICKER;
			m_nMess     = 0;
		}

		void OnFirstMessage()
		{
			int i = 0;
		}

		typedef SkinDateTimePickerCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinDateTimePickerCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)

			MESSAGE_HANDLER(WM_PAINT, OnPaint)

			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			MESSAGE_HANDLER( WM_SETFOCUS, OnSetFocus )
		END_MSG_MAP()

		void GetDrawRect( LPRECT pWindow, LPRECT pClient )
		{
			CRect rWindow;
			GetWindowRect(rWindow);

			if (pClient)
			{
				GetClientRect(pClient);
				ClientToScreen(pClient);
				::OffsetRect(pClient, -rWindow.left, -rWindow.top);
			}

			if (pWindow)
			{
				rWindow.OffsetRect(-rWindow.TopLeft());
				*pWindow = rWindow;
			}
		}

		LRESULT OnSetFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = DefWindowProc();
			Invalidate();
			return lRet;
		}
		
		void	OnNotifyReflect( NMHDR* pNMHDR, LRESULT& lrParent )
		{

			switch (pNMHDR->code)
			{
			case DTN_DROPDOWN:
				m_bLBtnDown = TRUE;
				//SendMessage(WM_PAINT);
				SetRedraw(FALSE);
				SetTimer(TIMER_DROP, 10, NULL);
				break;

			case DTN_CLOSEUP:
				m_bLBtnDown = FALSE;
				SetRedraw(TRUE);
				KillTimer(TIMER_DROP);
				//DelayRedraw(200);
				break;

			case DTN_DATETIMECHANGE:
				Invalidate(FALSE);
				break;
			}

			return;
		}

		
		LRESULT OnTimer( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{
			if (wParam == TIMER_DROP)
			{
				if (m_bLBtnDown)
				{
					SetRedraw(TRUE);

					WTL::CWindowDC dc( m_hWnd );

					WTL::CRect rcWindow, rClient;
					GetDrawRect(rcWindow, rClient);

					int nState = GetState();
					
					DrawDropButton(dc, nState, rClient );
				}
				else
				{
					KillTimer(TIMER_DROP);
					SetRedraw(TRUE);
				}
			}

			bHandled = FALSE;
			return 0;
		}

		
		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 0;
		}



	

		void DrawDropButton( HDC hdc, int nState, WTL::CRect& rClient )
		{
			rClient.left = rClient.right - ::GetSystemMetrics(SM_CXHTHUMB) - 2;
			rClient.right = rClient.right;
			
			
			// fix icon combox draw
			FillRect(hdc, &rClient, (HBRUSH)GetStockObject(WHITE_BRUSH));

			rClient.top = rClient.top + 1;
			rClient.bottom = rClient.bottom - 1;

			if (_scheme)
				_scheme->DrawBackground(hdc, _classid, m_nPart, nState, &rClient, NULL );

		}

		

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			HDC hdc = GetWindowDC( );

			WTL::CDCHandle dc;
			dc.Attach(hdc);

			WTL::CRect rcItem, rClient;
			GetDrawRect(rcItem, rClient);

			int nSaveDC = dc.SaveDC( ); // must restore dc to original state
			dc.ExcludeClipRect(rClient);

			int nState = GetState();

			// Cover up dark 3D shadow.

			COLORREF cr;
			_scheme->GetColor(_classid, m_nPart, nState, TMT_BORDERCOLOR, &cr);


			dc.Draw3dRect(rcItem, cr, cr);

			rcItem.DeflateRect(1,1);

			if (nState == CBXS_DISABLED) 
			{
				//2003-07-11
				dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),::GetSysColor(COLOR_BTNHIGHLIGHT));
			}
			else 
			{
				_scheme->GetColor(_classid, m_nPart, nState, TMT_TEXTBORDERCOLOR, &cr);
				dc.Draw3dRect(rcItem, cr, cr); 
			}

			if (!(GetStyle() & DTS_UPDOWN))
			{
				DrawDropButton( dc, nState, rClient );
			}	

			dc.RestoreDC(nSaveDC);

			ReleaseDC( hdc );
			return 0;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CRect rc ;
			GetClientRect( rc );

			WTL::CPaintDC	dc( m_hWnd );
			WTL::CMemoryDC memdc( dc, rc );

			int nState = GetState();

			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(_classid, m_nPart, nState))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

			DoPaint(memdc, nState, rc);

			return 0;
		}

		void DoPaint(HDC hdc, int nState, RECT *pRect)
		{
			const int EDGE = 1;

			WTL::CDCHandle dc;
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
				_scheme->DrawText(dc, _classid, m_nPart, nState, szText, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS, 0, &rText);

			delete [] szText;

			if (!(GetStyle() & DTS_UPDOWN))
			{
				DrawDropButton(dc, nState, rcItem );
			}	
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

		int m_nMess;
		enum
		{
			ICON_SPACE = 2,
		};
	};

}; // namespace 