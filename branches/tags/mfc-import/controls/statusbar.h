#pragma once

#include "../base/skinctrl.h"

namespace Skin {
	template<class BaseT = CStatusBarCtrl>
	struct SkinStatusBarCtrl : public SkinControlImpl<SkinStatusBarCtrl, BaseT>
	{
		enum { class_id = STATUS };
		
		SkinStatusBarCtrl()
		{
		}
		
		typedef SkinStatusBarCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinStatusBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)	
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)	
		END_MSG_MAP()

		
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CPaintDC dc(m_hWnd);
			STATUSBAR_Refresh(dc);
			return 0;
		}

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 1;
		}	

		static HFONT GetCtrlFont(HWND hwnd)
		{
			HFONT hFont;
			//如果sendmessage得到的是NULL,则使用的是系统字体
			if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
				hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

			return hFont;
		}

		void STATUSBAR_DrawPart ( HDC hdc, int itemID, RECT& r, BOOL bSimple)
		{
			UINT border = BDR_SUNKENOUTER;
			int themePart = SP_PANE;

			//TRACE("part bound %ld,%ld - %ld,%ld\n", r.left, r.top, r.right, r.bottom);

			TCHAR	szText[ 512 ] = {0};
			int nType;
			if ( bSimple )
			{
				GetText( itemID, szText, &nType );
			}
			else
				GetText( itemID, szText, &nType );

			// 绘制了背景图,所以不绘制了.
			/*
			if (nType & SBT_POPOUT)
				border = BDR_RAISEDOUTER;
			else if (nType & SBT_NOBORDERS)
				border = 0;
			
			
			DrawEdge(hdc, &r, border, BF_RECT|BF_ADJUST);
			*/
			if (nType & SBT_OWNERDRAW) 
			{
				DRAWITEMSTRUCT dis;

				dis.CtlID = (UINT) GetWindowLongPtr( GWLP_ID );
				dis.itemID = itemID;
				dis.hwndItem = m_hWnd;
				dis.hDC = hdc;
				dis.rcItem = r;
				dis.itemData = (ULONG_PTR)szText;
				SendMessageW ( GetParent(), WM_DRAWITEM, (WPARAM)dis.CtlID, (LPARAM)&dis);
			}
			else
			{
				HICON hIcon = GetIcon(bSimple ? -1 : itemID); 
				if ( hIcon ) 
				{
					INT cy = r.bottom - r.top;
					r.left += 2;
					DrawIconEx (hdc, r.left, r.top, hIcon, cy, cy, 0, 0, DI_NORMAL);
					r.left += cy;
				}
				//DrawStatusText (hdc, &r, szText, SBT_NOBORDERS);
				SetBkMode( hdc, TRANSPARENT );
				DrawText(hdc, szText, -1, &r, DT_SINGLELINE  | DT_END_ELLIPSIS | DT_VCENTER | DT_LEFT);
			}
		}

		void STATUSBAR_RefreshPart( HDC hdc, int itemID, BOOL bSimple )
		{
			if (!IsWindowVisible ())
				return;

			CRect rectPane;
			BOOL bRet = GetRect( itemID, &rectPane );
			if ( !bRet && !bSimple )
				return;

			if ( rectPane.right < rectPane.left )
				return;

			// draw back
			int nPart;
			int nState;
			nPart = SP_GRIPPERPANE; // 需要确认是否是这个????//
			nState = 1; // 没有状态,统一搞1吧
			if ( _scheme )
				_scheme->DrawBackground(hdc, class_id, nPart, nState, &rectPane, NULL);

			if ( bSimple )
			{
				GetClientRect( rectPane );
				rectPane.left += 5;
			}
			STATUSBAR_DrawPart ( hdc, itemID, rectPane, bSimple);
		}

		void STATUSBAR_DrawSizeGrip (HDC hdc, LPRECT lpRect)
		{
			

			//TRACE("draw size grip %ld,%ld - %ld,%ld\n", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);

			int nPart;
			int nState;
			nPart = SP_GRIPPER; 
			nState = 1; // 没有状态,统一搞1吧
			
			RECT rcClient = *lpRect;
			int nWidth = GetSchemeWidth( nPart, nState );
			int nHeight = GetSchemeHeight( nPart, nState );

			rcClient.left = rcClient.right - nWidth;
			rcClient.top = rcClient.bottom - nHeight;

			if ( _scheme )
				_scheme->DrawBackground(hdc, class_id, nPart, nState, &rcClient, NULL);

			/*

			HPEN hPenFace, hPenShadow, hPenHighlight, hOldPen;
			POINT pt;
			INT i;

			pt.x = lpRect->right - 1;
			pt.y = lpRect->bottom - 1;

			hPenFace = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DFACE ));
			hOldPen = (HPEN)SelectObject( hdc, hPenFace );
			MoveToEx (hdc, pt.x - 12, pt.y, NULL);
			LineTo (hdc, pt.x, pt.y);
			LineTo (hdc, pt.x, pt.y - 13);

			pt.x--;
			pt.y--;

			hPenShadow = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ));
			SelectObject( hdc, hPenShadow );
			for (i = 1; i < 11; i += 4) {
				MoveToEx (hdc, pt.x - i, pt.y, NULL);
				LineTo (hdc, pt.x + 1, pt.y - i - 1);

				MoveToEx (hdc, pt.x - i - 1, pt.y, NULL);
				LineTo (hdc, pt.x + 1, pt.y - i - 2);
			}

			hPenHighlight = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DHIGHLIGHT ));
			SelectObject( hdc, hPenHighlight );
			for (i = 3; i < 13; i += 4) {
				MoveToEx (hdc, pt.x - i, pt.y, NULL);
				LineTo (hdc, pt.x + 1, pt.y - i - 1);
			}

			SelectObject (hdc, hOldPen);
			DeleteObject( hPenFace );
			DeleteObject( hPenShadow );
			DeleteObject( hPenHighlight );
			*/
		}

		LRESULT STATUSBAR_Refresh ( HDC hdc )
		{
			CRect rc;
			GetClientRect(&rc);
			CMemoryDC memdc(hdc, rc);


			int nPart;
			int nState;
			nPart = SP_PANE;
			nState = 1; // 没有状态,统一搞1吧

			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, nPart, nState))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);


			// draw background
			if ( _scheme )
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rc, NULL);

			// draw part
			HGDIOBJ  hOldFont = SelectObject (memdc, GetCtrlFont(m_hWnd) );
			
			if ( IsSimple() )
			{
				STATUSBAR_RefreshPart ( memdc, 0, TRUE);
			}
			else
			{
				int pParts[1];
				int nCount = GetParts(1, pParts);
				for ( int i = 0; i < nCount; i++)
				{
					STATUSBAR_RefreshPart (memdc, i, FALSE);
				}
				
			}

			SelectObject (hdc, hOldFont);

			if (GetWindowLong( GWL_STYLE ) & SBARS_SIZEGRIP)
				STATUSBAR_DrawSizeGrip (memdc, &rc);
			return 0;
		}
	
	private:

	};

}; // namespace 