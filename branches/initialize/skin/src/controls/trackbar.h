#pragma once

#include "../base/skinctrl.h"

namespace Skin {

	/* Used by TRACKBAR_Refresh to find out which parts of the control
	need to be recalculated */

#define TB_THUMBPOSCHANGED      1
#define TB_THUMBSIZECHANGED     2
#define TB_THUMBCHANGED 	(TB_THUMBPOSCHANGED | TB_THUMBSIZECHANGED)
#define TB_SELECTIONCHANGED     4
#define TB_DRAG_MODE            8     /* we're dragging the slider */
#define TB_AUTO_PAGE_LEFT	16
#define TB_AUTO_PAGE_RIGHT	32
#define TB_AUTO_PAGE		(TB_AUTO_PAGE_LEFT | TB_AUTO_PAGE_RIGHT)
#define TB_THUMB_HOT            64    /* mouse hovers above thumb */

	template<class BaseT = WTL::CTrackBarCtrl>
	struct SkinTrackBarCtrl : public SkinControlImpl<SkinTrackBarCtrl, BaseT>
	{
		enum { class_id = TRACKBAR };

		SkinTrackBarCtrl()
		{
			m_flags = 0;
		}

		void OnFirstMessage()
		{
			int i = 0;
		}

		typedef SkinTrackBarCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinTrackBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			//REFLECTED_NOTIFY_CODE_HANDLER_EX( NM_CUSTOMDRAW, OnCustomDraw )
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		END_MSG_MAP()

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 0 ;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			TRACKBAR_Paint ( (HDC)wParam );
			return 0;
		}

		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			LRESULT lRet = DefWindowProc();	
			if (!(m_flags & TB_DRAG_MODE)) 
			{
				RECT rcThumb;
				GetThumbRect( & rcThumb );
				DWORD oldFlags = m_flags;

				if (PtInRect (&rcThumb, pt))
				{
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof( tme );
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = m_hWnd;
					TrackMouseEvent( &tme );
					m_flags |= TB_THUMB_HOT;
				}
				else
				{
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof( tme );
					tme.dwFlags = TME_CANCEL;
					tme.hwndTrack = m_hWnd;
					TrackMouseEvent( &tme );
					m_flags &= ~TB_THUMB_HOT; 
				}
				if (oldFlags != m_flags) 
					InvalidateRect ( &rcThumb, FALSE);

			}

			return lRet;
		}
		LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = DefWindowProc();	
			m_flags &= ~TB_THUMB_HOT; 
			return lRet;
		}

		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			LRESULT lRet = DefWindowProc();	
			RECT rcThumb;
			GetThumbRect( &rcThumb );
			if (PtInRect(&rcThumb, pt)) 
			{
				m_flags |= TB_DRAG_MODE;
				InvalidateRect ( &rcThumb, FALSE);
			}
			return lRet;
		}

		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LRESULT lRet = DefWindowProc();	
			if (m_flags & TB_DRAG_MODE)
			{
				m_flags &= ~TB_DRAG_MODE; 
				RECT rcThumb;
				GetThumbRect( &rcThumb );
				InvalidateRect ( &rcThumb, FALSE);
			}
			return lRet;
		}
		
		int GetThumbState(BOOL bVert)
		{
			int nState = TUS_NORMAL;
			if ( !IsWindowEnabled() )
			{
				nState = bVert ? TUVS_DISABLED : TUS_DISABLED;
			}
			else
			{
				if (m_flags & TB_DRAG_MODE)
				{
					nState = bVert ? TUVS_PRESSED : TUS_PRESSED;
				}
				else
				{
					if (m_flags & TB_THUMB_HOT)
					{
						nState = bVert ? TUVS_HOT : TUS_HOT;
					}
					else
					{
						if ( GetFocus() == m_hWnd )
							nState = bVert ? TUVS_FOCUSED : TUS_FOCUSED;
						else
							nState = bVert ? TUVS_NORMAL : TUS_NORMAL;
					}
				}
			}
			return nState;
		}

		LRESULT	TRACKBAR_Paint ( HDC hdc )
		{
			if (hdc) 
			{
				TRACKBAR_Refresh( hdc );
			}
			else
			{
				WTL::CPaintDC dc(m_hWnd);
				TRACKBAR_Refresh( dc );
			}
			return 0;
		}
		
		int notify_customdraw( NMCUSTOMDRAW *pnmcd, int stage)
		{
			pnmcd->dwDrawStage = stage;
			return (int)SendMessageW (GetParent(), WM_NOTIFY, 
				pnmcd->hdr.idFrom, (LPARAM)pnmcd);
		}

		void TRACKBAR_DrawChannel( HDC hdc )
		{
			int nPart = TKP_TRACK;// 取得一个就可以了
			int nState = TRS_NORMAL;
			DWORD dwStyle	= GetStyle();
			if( (dwStyle&TBS_VERT) )
			{
				nPart = TKP_TRACKVERT;
				nState = TRVS_NORMAL;
			}
			RECT	rc;
			GetChannelRect( &rc );
			if (_scheme)
				_scheme->DrawBackground(hdc, class_id, nPart, nState, &rc, NULL );			
		}

		void TRACKBAR_DrawThumb( HDC hdc )
		{
			int nPart = TKP_THUMB;// 取得一个就可以了
			DWORD dwStyle	= GetStyle();
			if( (dwStyle&TBS_VERT) )
			{
				nPart = TKP_THUMBVERT;
			}
			int nState = GetThumbState(dwStyle&TBS_VERT);
			RECT	rc;
			GetThumbRect( &rc );
			if (_scheme)
				_scheme->DrawBackground(hdc, class_id, nPart, nState, &rc, NULL );			

		}
		void TRACKBAR_Refresh ( HDC hdcDst )
		{
			WTL::CRect rc;
			GetClientRect(&rc);
			WTL::CMemoryDC memdc(hdcDst, rc);
			
			int nPart = TKP_TRACK;// 取得一个就可以了
			int nState = TKS_NORMAL;

			DWORD dwStyle = GetWindowLongW (m_hWnd, GWL_STYLE);

			NMCUSTOMDRAW nmcd;
			int gcdrf, icdrf;

			ZeroMemory(&nmcd, sizeof(nmcd));
			nmcd.hdr.hwndFrom = m_hWnd;
			nmcd.hdr.idFrom = GetWindowLongPtrW (m_hWnd, GWLP_ID);
			nmcd.hdr.code = NM_CUSTOMDRAW;
			nmcd.hdc = memdc;


			/* start the paint cycle */
			nmcd.rc = rc;
			gcdrf = notify_customdraw( &nmcd, CDDS_PREPAINT);
			if (gcdrf & CDRF_SKIPDEFAULT) 
				return;

			/* Erase backbround */
			if (gcdrf == CDRF_DODEFAULT ||
				notify_customdraw( &nmcd, CDDS_PREERASE ) != CDRF_SKIPDEFAULT) 
			{
				//背景图的绘制,需要在想想.
				if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, nPart, nState))
					_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

				if (gcdrf != CDRF_DODEFAULT)
					notify_customdraw( &nmcd, CDDS_POSTERASE );
			}

			/* draw channel */
			if (gcdrf & CDRF_NOTIFYITEMDRAW) 
			{
				nmcd.dwItemSpec = TBCD_CHANNEL;
				nmcd.uItemState = CDIS_DEFAULT;
				GetChannelRect( &nmcd.rc );
				icdrf = notify_customdraw( &nmcd, CDDS_ITEMPREPAINT );
			}
			else
				icdrf = CDRF_DODEFAULT;

			if ( !(icdrf & CDRF_SKIPDEFAULT) ) 
			{
				TRACKBAR_DrawChannel ( memdc );
				if (icdrf & CDRF_NOTIFYPOSTPAINT)
					notify_customdraw( &nmcd, CDDS_ITEMPOSTPAINT);
			}

			//后面做
			/* draw tics */
			/*
			if (!(dwStyle & TBS_NOTICKS))
			{
				if (gcdrf & CDRF_NOTIFYITEMDRAW) 
				{
					nmcd.dwItemSpec = TBCD_TICS;
					nmcd.uItemState = CDIS_DEFAULT;
					nmcd.rc = rc;
					icdrf = notify_customdraw( &nmcd, CDDS_ITEMPREPAINT );
				} 
				else
					icdrf = CDRF_DODEFAULT;
				if ( !(icdrf & CDRF_SKIPDEFAULT) ) 
				{
					TRACKBAR_DrawTics (infoPtr, hdc, dwStyle);
					if (icdrf & CDRF_NOTIFYPOSTPAINT)
						notify_customdraw( &nmcd, CDDS_ITEMPOSTPAINT);
				}
			}
			*/

			/* draw thumb */
			if (!(dwStyle & TBS_NOTHUMB))
			{
				if (gcdrf & CDRF_NOTIFYITEMDRAW)
				{
					nmcd.dwItemSpec = TBCD_THUMB;
					nmcd.uItemState = m_flags & TB_DRAG_MODE ? CDIS_HOT : CDIS_DEFAULT;
					GetThumbRect( &nmcd.rc );// = infoPtr->rcThumb;
					icdrf = notify_customdraw( &nmcd, CDDS_ITEMPREPAINT);
				}
				else
					icdrf = CDRF_DODEFAULT;
				if ( !(icdrf & CDRF_SKIPDEFAULT) ) 
				{
					TRACKBAR_DrawThumb( memdc );
					if (icdrf & CDRF_NOTIFYPOSTPAINT)
						notify_customdraw( &nmcd, CDDS_ITEMPOSTPAINT);
				}
			}

			/* draw focus rectangle */
			if ( GetFocus() == m_hWnd )
			{
				DrawFocusRect(memdc, &rc);
			}

			/* finish up the painting */
			if (gcdrf & CDRF_NOTIFYPOSTPAINT)
				notify_customdraw( &nmcd, CDDS_POSTPAINT);
		}

private:

	int	m_flags;


	};

}; // namespace 
