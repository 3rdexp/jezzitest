#pragma once

#include "../base/skinctrl.h"
/*
BEGIN_TM_CLASS_PARTS(TAB)
TM_PART(1, TABP, TABITEM)
TM_PART(2, TABP, TABITEMLEFTEDGE)
TM_PART(3, TABP, TABITEMRIGHTEDGE)
TM_PART(4, TABP, TABITEMBOTHEDGE)
TM_PART(5, TABP, TOPTABITEM)
TM_PART(6, TABP, TOPTABITEMLEFTEDGE)
TM_PART(7, TABP, TOPTABITEMRIGHTEDGE)
TM_PART(8, TABP, TOPTABITEMBOTHEDGE)
TM_PART(9, TABP, PANE)
TM_PART(10, TABP, BODY)
END_TM_CLASS_PARTS()
*/
namespace Skin {

	template<class BaseT = WTL::CTabCtrl>
	struct SkinTabCtrl : public SkinControlImpl<SkinTabCtrl, BaseT>
	{
		//enum { class_id = TAB };

		SkinTabCtrl()
		{
			_classid	= TAB;	
			_nOverIndex	= -1;
			_fMouseOver = 0;
		}

		void OnFirstMessage()
		{
			int i = 0;
		}

		typedef SkinTabCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinTabCtrl, BaseT> base_type;


		void GetDrawRect ( LPRECT pWindow, LPRECT pClient = NULL )
		{
			WTL::CRect rWindow;
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

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
			//MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		END_MSG_MAP()

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 1;
		}

		LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			TAB_Refresh( (HDC)wParam );
			return 0;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CRect rc;
			GetClientRect( &rc );
			WTL::CPaintDC dc(m_hWnd);
			WTL::CMemoryDC memdc ( dc, rc );
			TAB_Refresh( memdc );
			return 0;
		}

		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			//if(GetCapture() == m_hWnd)
			{
				POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				TCHITTESTINFO tinfo;
				tinfo.pt = ptCursor;
				tinfo.flags = TCHT_ONITEM | TCHT_ONITEMICON | TCHT_ONITEMLABEL;
				int nIndex = HitTest ( &tinfo );
				if ( _nOverIndex != nIndex )
				{
					WTL::CRect rc1;
					BOOL bRet = GetItemRect( _nOverIndex, &rc1 );

					WTL::CRect rc2;
					GetItemRect( nIndex, &rc2 );
					
					_nOverIndex = nIndex;

					InvalidateRect( rc1 );
					InvalidateRect( rc2 );
					//InvalidateRect(NULL, TRUE);
					//UpdateWindow();
				}
			}
			if( _fMouseOver == 0 )
			{
				_fMouseOver = 1;
				//InvalidateRect(NULL, TRUE);
				//UpdateWindow();
				StartTrackMouseLeave();
			}
			return DefWindowProc();
		}

		LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if(_fMouseOver == 1)
			{
				WTL::CRect rc1;
				BOOL bRet = GetItemRect( _nOverIndex, &rc1 );

				_fMouseOver = 0;
				_nOverIndex = -1;
				//InvalidateRect(NULL, TRUE);
				//UpdateWindow();
				InvalidateRect( rc1 );
			}
			return 0;
		}

		BOOL StartTrackMouseLeave()
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			return _TrackMouseEvent(&tme);
		}

		static HFONT GetCtrlFont(HWND hwnd)
		{
			HFONT hFont;
			//如果sendmessage得到的是NULL,则使用的是系统字体
			if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
				hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

			return hFont;
		}
		
		void TAB_DrawItem( HDC  hdc, INT  iItem)
		{
			const int PADDING = 2;

			WTL::CRect rcItem;
			BOOL bRet = GetItemRect( iItem, &rcItem );
			if ( !bRet )
				return;

			//
			int nState;
			// DWORD dwMask;
			TCITEM tcItem;

			BOOL bVert = FALSE;
			if ( GetStyle() & TCS_VERTICAL )
				bVert = TRUE;
			
			tcItem.mask = TCIF_STATE;
			tcItem.dwState = 0;
			GetItem(iItem, &tcItem);
			
			if ( !IsWindowEnabled() )
				nState = bVert ? TTIS_DISABLED : TIS_DISABLED;
			else if ( GetCurSel() == iItem )
				nState = bVert ? TTIS_SELECTED : TIS_SELECTED;
			else if ( tcItem.dwState & TCIS_HIGHLIGHTED || _nOverIndex == iItem )
				nState = bVert ? TTIS_HOT : TIS_HOT;
			else if ( GetCurFocus() == iItem )
				nState = bVert ? TTIS_FOCUSED : TIS_FOCUSED;
			else
				nState = bVert ? TTIS_NORMAL : TIS_NORMAL;
			
			// draw item background
			if ( _scheme )
				_scheme->DrawBackground(hdc, _classid, bVert ? TABP_TOPTABITEM : TABP_TABITEM, nState, &rcItem, NULL );

			// draw icon and text
			TCHAR szText[256] = {0};
			tcItem.mask = TCIF_TEXT | TCIF_IMAGE;
			tcItem.pszText = szText;
			tcItem.cchTextMax = 256;
			GetItem(iItem, &tcItem);

			WTL::CImageList imgList = GetImageList();
			if ( !imgList.IsNull() )
			{
				WTL::CSize szImg;
				imgList.GetIconSize(szImg);
				if ( bVert )
				{
					BOOL bRet = imgList.Draw(hdc, tcItem.iImage, WTL::CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
					if ( bRet )
						rcItem.bottom = rcItem.bottom - szImg.cy - PADDING;
				}
				else
				{
					BOOL bRet = imgList.Draw(hdc, tcItem.iImage, WTL::CPoint(rcItem.left, rcItem.bottom - szImg.cy ), ILD_TRANSPARENT);
					if ( bRet )
						rcItem.left = rcItem.left + szImg.cx + PADDING;
					
				}
				
			}

			HFONT hOldFont = (HFONT)SelectObject(hdc, GetCtrlFont(m_hWnd));
			if (_scheme)
				_scheme->DrawText(hdc, _classid, bVert ? TABP_TOPTABITEM : TABP_TABITEM, nState, tcItem.pszText, DT_VCENTER | DT_SINGLELINE | DT_CENTER , 0, &rcItem);

			SelectObject(hdc, hOldFont);

		}	

		void TAB_DrawBorder ( HDC hdc )
		{
			WTL::CRect rc;
			GetClientRect(&rc);
			
			BOOL bVert = FALSE;
			if ( GetStyle() & TCS_VERTICAL )
				bVert = TRUE;

			if ( GetItemCount() > 0 )
			{
				int nMax = 0;
				for ( int i = 0; i < GetItemCount(); i++ )
				{
					WTL::CRect rcItem;
					GetItemRect( i, &rcItem );
					if ( bVert )
					{
						if ( rcItem.right > nMax )
							nMax = rcItem.right;
					}
					else
					{
						if ( rcItem.bottom > nMax )
							nMax = rcItem.bottom;
					}
				}
				
				if ( bVert )
				{
					rc.left += nMax;
					rc.left --;
				}
				else
				{
					rc.top += nMax;
					rc.top --;
				}
			}
			
			if ( _scheme )
				_scheme->DrawBackground(hdc, _classid, TABP_PANE, TIS_NORMAL, &rc, NULL );
			
		}

		LRESULT TAB_Refresh( HDC hdc )
		{
			WTL::CRect rc;
			GetClientRect(&rc);

			WTL::CMemoryDC memdc(hdc, rc);
			
			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(_classid, TABP_TABITEM, TIS_NORMAL))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

			int i = 0;
			
			if ( GetStyle() & TCS_BUTTONS )
			{
				for ( i = 0; i < GetItemCount(); i++ )
					TAB_DrawItem ( memdc, i );
			}
			else
			{
				/* Draw all the non selected item first */
				for (i = 0; i < GetItemCount(); i++)
				{
					if ( i != GetCurSel() )
						TAB_DrawItem ( memdc, i );
				}

				/* Now, draw the border, draw it before the selected item
				* since the selected item overwrites part of the border. */
				TAB_DrawBorder ( memdc );

				/* Then, draw the selected item */
				TAB_DrawItem ( memdc, GetCurSel() );

				/* If we haven't set the current focus yet, set it now.
				* Only happens when we first paint the tab controls */
				//if ( GetCurFocus()  == -1)
				//	TAB_SetCurFocus( GetCurFocus() );
			}
			return 0;
		}
	
		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CRect rWindow, rClient;
			GetDrawRect(rWindow, rClient);

			BOOL bHot = FALSE;//IsHot(rWindow);

			// draw the rest of the border
			WTL::CRect rPage;
			GetClientRect(rPage);
			AdjustRect(FALSE, rPage);
			rPage.top -= 2;
			rClient.top += rPage.top;

			COLORREF crBkgnd = RGB(255, 125, 125);
			const int EDGE = 1;
			HDC hdc = GetWindowDC();
			WTL::CDC dc;
			dc.Attach(hdc);
			dc.FillSolidRect(rWindow.left, rWindow.top, EDGE, rClient.top - rWindow.top, crBkgnd);
			//dc.FillSolidRect(rWindow.left, rWindow.top, rWindow.right - rWindow.left, EDGE, crBkgnd);
			//dc.FillSolidRect(rWindow.right - EDGE, rWindow.top, EDGE, rClient.top - rWindow.top, crBkgnd);

			rWindow.top = rClient.top;

			dc.Draw3dRect(rWindow, crBkgnd, crBkgnd);
			//Draw3dEdge(dc, rWindow, 4, 0x0002, IM_COLD);

			// rub out the base line of the selected tab if enough of the tab is visible
			int nSel = GetCurSel();

			if (nSel != -1)
			{
				WTL::CRect rTab;
				GetItemRect(nSel, rTab);

				if (rTab.right > 4)
				{
					//ClientToScreen(rTab);
					//dc.FillSolidRect(rTab.left + 2, rWindow.top, rTab.Width() - 2, 2, crBkgnd);
				}
			}

			ReleaseDC( hdc );
			dc.Detach();

			return 0;
		}

		int  GetState()
		{
			return ETS_NORMAL;
		}
	private:
		int			_nOverIndex;
		unsigned	_fMouseOver  : 1;
	};

}; // namespace 
