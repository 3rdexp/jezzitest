#pragma once

#include "../base/skinctrl.h"
#include "../base/skinhookbase.h"
namespace Skin {

	template<class BaseT = WTL::CScrollBar>
	struct SkinScrollBar : public SkinControlImpl<SkinScrollBar, BaseT>
	{
		enum { class_id = SCROLLBAR };

		enum
		{
			SCROLL_UP,
			SCROLL_DOWN,
			SCROLL_THUMB
		};

		enum
		{
			TIMERID_NOTIFY	= 100,
			TIMERINTER		= 100,
			THUMB_BORDER	= 3,
			THUMB_MINSIZE	= 6
		};


		SkinScrollBar()
		{
			m_bDrag			=	 FALSE;
			m_uClicked		=	 -1;
			m_bNotify		=	FALSE;
			m_uHtPrev		=	-1;
			m_bMouseHover	=	FALSE;
			memset(&m_si,0,sizeof(SCROLLINFO));
			m_si.nTrackPos	=	-1;
		}
		
		typedef SkinScrollBar<BaseT> this_type;
		typedef SkinControlImpl<SkinScrollBar, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)		
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
			MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnDbLButtonDown)
			MESSAGE_HANDLER(SBM_SETSCROLLINFO, OnSetScrollInfo)
			MESSAGE_HANDLER(SBM_GETSCROLLINFO, OnGetScrollInfo)
			
			MESSAGE_HANDLER(SBM_SETPOS, OnSetPos)
			MESSAGE_HANDLER(SBM_GETPOS, OnGetPos)

			MESSAGE_HANDLER(SBM_SETRANGE, OnSetRange)
			MESSAGE_HANDLER(SBM_GETRANGE, OnGetRange)
			
			MESSAGE_HANDLER(SBM_SETRANGEREDRAW, OnSetRangeDraw)
			
			//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		END_MSG_MAP()


		BOOL IsVertical()
		{
			DWORD dwStyle = GetStyle();
			return dwStyle&SBS_VERT;
		}

		UINT HitTest(CPoint pt)
		{
			WTL::CRect rcClient;
			GetClientRect( rcClient );

			WTL::CScrollBar scrollbar;
			scrollbar = m_hWnd;

			SCROLLBARINFO sbar;
			sbar.cbSize = sizeof( sbar );
			scrollbar.GetScrollBarInfo( &sbar );

			BOOL bVert =  IsVertical();

			WTL::CRect rc;

			UINT uHit = SB_LINEUP;

			rc = GetLineUpRect( rcClient, bVert, sbar );
			if ( rc.PtInRect(pt) )
				return SB_LINEUP;

			rc = GetPageUpRect( rcClient, bVert, sbar );
			if ( rc.PtInRect(pt) )
				return SB_PAGEUP;
			
			rc = GetThumbRect( rcClient, bVert, sbar );
			if ( rc.PtInRect(pt) )
				return SB_THUMBTRACK;

			rc = GetPageDownRect( rcClient, bVert, sbar );
			if ( rc.PtInRect(pt) )
				return SB_PAGEDOWN;

			rc = GetLineDownRect( rcClient, bVert, sbar );
			if ( rc.PtInRect(pt) )
				return SB_LINEDOWN;

			return uHit;
		}


		
		LRESULT OnSetRangeDraw(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			m_si.nMin = wParam;
			m_si.nMax = lParam;
			
			if ( m_si.nPos < m_si.nMin )
			{
				int nOld = m_si.nPos;
				m_si.nPos = m_si.nMin;
				return nOld;
			}

			if ( m_si.nPos > m_si.nMax )
			{
				int nOld = m_si.nMax;
				m_si.nPos = m_si.nMax;
				return nOld;
			}
			InvalidateRect(NULL, FALSE);
			return 0;
		}

		LRESULT OnSetRange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			m_si.nMin = wParam;
			m_si.nMax = lParam;

			if ( m_si.nPos < m_si.nMin )
			{
				int nOld = m_si.nPos;
				m_si.nPos = m_si.nMin;
				return nOld;
			}

			if ( m_si.nPos > m_si.nMax )
			{
				int nOld = m_si.nMax;
				m_si.nPos = m_si.nMax;
				return nOld;
			}
			return 0;

		}
		LRESULT OnGetRange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			wParam = m_si.nMin;
			lParam = m_si.nMax;
			return 0;
		}
		

		LRESULT OnSetPos(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			BOOL bRedraw = (BOOL)lParam;
			m_si.nPos = wParam;
			if(bRedraw)
			{
				InvalidateRect(NULL,FALSE);
			}
			return 0;
		}

		LRESULT OnGetPos(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return m_si.nPos;
		}

		LRESULT OnGetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LPSCROLLINFO lpScrollInfo=(LPSCROLLINFO)lParam;
			int nMask=lpScrollInfo->fMask;
			if(nMask&SIF_PAGE)
				lpScrollInfo->nPage = m_si.nPage;
			if(nMask&SIF_POS)
				lpScrollInfo->nPos = m_si.nPos;
			if(nMask&SIF_TRACKPOS) 
				lpScrollInfo->nTrackPos = m_si.nTrackPos;
			if(nMask&SIF_RANGE)
			{
				lpScrollInfo->nMin=m_si.nMin;
				lpScrollInfo->nMax=m_si.nMax;
			}
			return 0;	
		}

		LRESULT OnSetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			BOOL bRedraw = (BOOL)wParam;
			LPSCROLLINFO lpScrollInfo=(LPSCROLLINFO)lParam;
			if(lpScrollInfo->fMask&SIF_PAGE) m_si.nPage = lpScrollInfo->nPage;
			if(lpScrollInfo->fMask&SIF_POS) m_si.nPos = lpScrollInfo->nPos;
			if(lpScrollInfo->fMask&SIF_RANGE)
			{
				m_si.nMin = lpScrollInfo->nMin;
				m_si.nMax = lpScrollInfo->nMax;
			}
			if(bRedraw)
			{
				InvalidateRect(NULL,FALSE);
			}
			return 0;
		}

		LRESULT OnDbLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 0;
		}

		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if ( wParam == TIMERINTER )
			{
				ASSERT(m_uClicked!=-1 && m_uClicked!=SB_THUMBTRACK);
				::SendMessage( GetParent(), IsVertical() ? WM_VSCROLL : WM_HSCROLL, MAKELONG(m_uClicked,0),(LPARAM)m_hWnd );
			}
			bHandled = FALSE;
			return 0 ;
		}

		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetCapture();

			CPoint pt;
			pt.x = (short)LOWORD(lParam);
			pt.y = (short)HIWORD(lParam);

			UINT uHit= HitTest (pt);

			if(uHit == SB_THUMBTRACK)
			{
				m_bDrag = TRUE;
				m_ptDrag = pt;
				m_si.nTrackPos = m_si.nPos;
				m_nDragPos = m_si.nPos;
				//::SendMessage( GetParent(), WM_SYSCOMMAND,IsVertical() ? SC_VSCROLL : SC_HSCROLL, 0 );
				InvalidateRect( GetRect( uHit ), FALSE );
			}
			else
			{
				m_uClicked = uHit;
				m_bNotify = TRUE;
				BOOL b;
				OnTimer(WM_TIMER, TIMERID_NOTIFY, 0, b);
				SetTimer(TIMERID_NOTIFY, TIMERINTER, NULL);
				if( uHit==SB_LINEUP||uHit==SB_LINEDOWN) 
					InvalidateRect( GetRect( uHit ), FALSE );
			}
			return 0;
		}

		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			ReleaseCapture();
			if(m_bDrag)
			{
				m_bDrag=FALSE;
				::SendMessage(GetParent(), IsVertical() ? WM_VSCROLL : WM_HSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nTrackPos), (LPARAM)m_hWnd);
				::SendMessage(GetParent(), IsVertical() ? WM_VSCROLL: WM_HSCROLL, SB_ENDSCROLL,(LPARAM)m_hWnd);

				//InvalidateRect( GetRect(SB_THUMBTRACK), FALSE );

				m_si.nTrackPos	= -1;

				InvalidateRect( NULL, FALSE );

			}
			else if( m_uClicked != -1 )
			{
				if(m_bNotify)
				{
					KillTimer(TIMERID_NOTIFY);
					m_bNotify=FALSE;
					if( m_uClicked==SB_LINEUP || m_uClicked==SB_LINEDOWN ) 
						InvalidateRect( GetRect(m_uClicked), FALSE );
				}
				m_uClicked = -1;
			}
			return 0;
		}

		
		LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			m_bMouseHover = FALSE;
			BOOL b;
			OnMouseMove(WM_MOUSEMOVE, -1, MAKELONG( -1, -1 ), b);
			return 0;
		}
		
		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CPoint point;
			point.x = (short)LOWORD(lParam);
			point.y = (short)HIWORD(lParam);

			if( !m_bMouseHover && wParam != -1)
			{
				m_bMouseHover=TRUE;
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = 1;
				m_bMouseHover = _TrackMouseEvent(&tme);
			}

			if(m_bDrag)
			{
				WTL::CScrollBar scrollbar;
				scrollbar = m_hWnd;

				SCROLLBARINFO sbar;
				sbar.cbSize = sizeof( sbar );
				scrollbar.GetScrollBarInfo( &sbar );

				int nNewTrackPos = IsVertical() ? point.y - m_ptDrag.y : point.x - m_ptDrag.x;
				TRACE( "nNewTrackPos is %d \r\n", nNewTrackPos );
				int nMin = sbar.dxyLineButton; //IsVertical() ? sbar.rcScrollBar.top : sbar.rcScrollBar.left;
				int nMax = IsVertical() ? sbar.rcScrollBar.bottom - sbar.rcScrollBar.top - sbar.dxyLineButton : sbar.rcScrollBar.right - sbar.rcScrollBar.left - sbar.dxyLineButton;
				if( nNewTrackPos + sbar.xyThumbTop < nMin )
				{
					nNewTrackPos = nMin - sbar.xyThumbTop;
				}
				else if( nNewTrackPos + sbar.xyThumbBottom > nMax )
				{
					nNewTrackPos = nMax - sbar.xyThumbBottom;
				}

				if(nNewTrackPos != m_si.nTrackPos)
				{
					//WTL::CRect rcOld = GetRect( SB_THUMBTRACK );

					m_si.nTrackPos=nNewTrackPos;
					
					//InvalidateRect( rcOld, FALSE );
					//InvalidateRect( GetRect( SB_THUMBTRACK ), FALSE );
					InvalidateRect( NULL, FALSE );
					::SendMessage( GetParent(), IsVertical() ? WM_VSCROLL : WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_si.nTrackPos),(LPARAM)m_hWnd);
				}
			}
			else if(m_uClicked!=-1)
			{
				CRect rc = GetRect(m_uClicked);
				if(!rc.PtInRect(point))
				{
					if(m_bNotify)
					{
						KillTimer(TIMERID_NOTIFY);
						m_bNotify=FALSE;
						if(m_uClicked==SB_LINEUP||m_uClicked==SB_LINEDOWN) 
							InvalidateRect( GetRect(m_uClicked), FALSE );
					}
				}else
				{
					if(!m_bNotify)
					{
						BOOL b;
						OnTimer(WM_TIMER, TIMERID_NOTIFY, 0, b);
						SetTimer(TIMERID_NOTIFY,TIMERINTER,NULL);
						m_bNotify=TRUE;
						if(m_uClicked==SB_LINEUP||m_uClicked==SB_LINEDOWN)
							InvalidateRect( GetRect(m_uClicked), FALSE );
					}
				}
			}
			else
			{
				UINT uHit=HitTest(point);
				if(uHit!=m_uHtPrev)
				{
					if(m_uHtPrev!=-1)
					{
						if(m_uHtPrev==SB_THUMBTRACK)
						{
							InvalidateRect( GetRect(SB_THUMBTRACK), FALSE );
						}else if(m_uHtPrev==SB_LINEUP||m_uHtPrev==SB_LINEDOWN)
						{
							InvalidateRect( GetRect(m_uHtPrev), FALSE );
						}
					}
					if(uHit!=-1)
					{
						if(uHit==SB_THUMBTRACK)
						{
							InvalidateRect( GetRect(SB_THUMBTRACK), FALSE );
						}else if(uHit==SB_LINEUP||uHit==SB_LINEDOWN)
						{
							InvalidateRect( GetRect(uHit), FALSE );
						}
					}
					m_uHtPrev=uHit;
				}
			}
			return 0;
		}

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 0 ;
		}

		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			//InstallScrollBar();
			bHandled = FALSE;
			return 0;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CPaintDC dc(m_hWnd);
			DoPaint(dc);	
			return 0;
		}

		WTL::CRect GetRect( int nHit )
		{
			WTL::CRect rc;
			GetClientRect( rc );
			
			WTL::CScrollBar scrollbar;
			scrollbar = m_hWnd;

			SCROLLBARINFO sbar;
			sbar.cbSize = sizeof( sbar );
			scrollbar.GetScrollBarInfo( &sbar );

			BOOL bVert =  IsVertical();
		
			if ( nHit == SB_LINEUP )
				return GetLineUpRect( rc, bVert, sbar );
			else if ( nHit == SB_PAGEUP )
				return GetPageUpRect( rc, bVert, sbar );
			else if ( nHit == SB_THUMBTRACK )
				return GetThumbRect( rc, bVert, sbar );
			else if ( nHit == SB_PAGEDOWN )
				return GetPageDownRect( rc, bVert, sbar );
			else if ( nHit == SB_LINEDOWN )
				return GetLineDownRect( rc, bVert, sbar );
			else
				return rc;
		}

		WTL::CRect GetLineUpRect( WTL::CRect& rc, BOOL bVert, SCROLLBARINFO& sbar)
		{
			WTL::CRect rcDraw;
			rcDraw = rc;
			if ( bVert )
			{
				rcDraw.bottom = rcDraw.top + sbar.dxyLineButton;
			}
			else
			{
				rcDraw.right = rcDraw.left + sbar.dxyLineButton;
			}

			return rcDraw;
		}

		WTL::CRect GetLineDownRect( WTL::CRect& rc, BOOL bVert , SCROLLBARINFO& sbar)
		{
			WTL::CRect rcDraw;
			rcDraw = rc;
			if ( bVert )
			{
				rcDraw.top = rcDraw.bottom - sbar.dxyLineButton;
			}
			else
			{
				rcDraw.left = rcDraw.right - sbar.dxyLineButton;
			}
			return rcDraw;
		}

		WTL::CRect GetPageUpRect( WTL::CRect& rc, BOOL bVert , SCROLLBARINFO& sbar)
		{
			WTL::CRect rcDraw;
			WTL::CRect rcThumb = GetThumbRect ( rc, bVert, sbar );

			rcDraw = rc;
			if ( bVert )
			{
				rcDraw.top = rcDraw.top + sbar.dxyLineButton;
				rcDraw.bottom = rcThumb.top;
			}
			else
			{
				rcDraw.left = rcDraw.left + sbar.dxyLineButton;
				rcDraw.right = rcThumb.left;
			}
			return rcDraw;
		}

		WTL::CRect GetPageDownRect( WTL::CRect& rc, BOOL bVert , SCROLLBARINFO& sbar)
		{
			WTL::CRect rcDraw;
			WTL::CRect rcThumb = GetThumbRect ( rc, bVert, sbar );

			rcDraw = rc;
			if ( bVert )
			{
				rcDraw.top = rcThumb.bottom;
				rcDraw.bottom = rcDraw.bottom - sbar.dxyLineButton;
			}
			else
			{
				rcDraw.left = rcThumb.right;
				rcDraw.right = rcDraw.right - sbar.dxyLineButton;
			}
			return rcDraw;
		}

		WTL::CRect GetThumbRect( WTL::CRect& rc, BOOL bVert , SCROLLBARINFO& sbar)
		{
			WTL::CRect rcDraw;
			rcDraw = rc;
			if ( !m_bDrag )
			{
				if ( m_si.nMax - m_si.nMin <= 0 )
				{
					if ( bVert )
					{
						rcDraw.top = sbar.xyThumbTop;
						rcDraw.bottom = sbar.xyThumbBottom;
					}
					else
					{
						rcDraw.left = sbar.xyThumbTop;
						rcDraw.right = sbar.xyThumbBottom;
					}
				}
				else
				{
					if ( bVert )
					{
						rcDraw.top = sbar.dxyLineButton + (rc.Height() - 2 * sbar.dxyLineButton - sbar.xyThumbBottom + sbar.xyThumbTop) * m_si.nPos / (m_si.nMax - m_si.nMin);
						rcDraw.bottom = rcDraw.top + sbar.xyThumbBottom - sbar.xyThumbTop;
					}
					else
					{
						rcDraw.left = sbar.dxyLineButton + (rc.Width() - 2 * sbar.dxyLineButton - sbar.xyThumbBottom + sbar.xyThumbTop) * m_si.nPos / (m_si.nMax - m_si.nMin);
						rcDraw.right = rcDraw.left + sbar.xyThumbBottom - sbar.xyThumbTop;
					}
				}
				
			}
			else
			{
				if ( bVert )
				{
					rcDraw.top = sbar.xyThumbTop + m_si.nTrackPos;
					rcDraw.bottom = sbar.xyThumbBottom + m_si.nTrackPos;
				}
				else
				{
					rcDraw.left = sbar.xyThumbTop + m_si.nTrackPos;
					rcDraw.right = sbar.xyThumbBottom + m_si.nTrackPos;
				}
			}
			
			return rcDraw;
		}


		LRESULT DoPaint(HDC dc)
		{
			WTL::CRect rc;
			GetClientRect(&rc);

			WTL::CScrollBar scrollbar;
			scrollbar = m_hWnd;

			SCROLLBARINFO sbar;
			sbar.cbSize = sizeof( sbar );
			scrollbar.GetScrollBarInfo( &sbar );

			WTL::CMemoryDC memdc(dc, rc);

			int nPart;
			int nState;

			BOOL bVert =  (GetStyle() & SBS_VERT );
		
			nPart = SBP_ARROWBTN;
			if ( !IsWindowEnabled() )
			{
				nState = bVert ? ABS_UPDISABLED : ABS_LEFTDISABLED; 
			}
			else if ( SCROLL_UP == m_nPress )
			{
				nState = bVert ? ABS_UPPRESSED : ABS_LEFTPRESSED; 
			}
			else if ( SCROLL_UP == m_nHot )
			{
				nState = bVert ? ABS_UPHOT : ABS_LEFTHOT; 
			}
			else
				nState = bVert ? ABS_UPNORMAL : ABS_LEFTNORMAL; 


			//int nWidth = GetSchemeWidth(SBP_ARROWBTN, bVert ? ABS_UPNORMAL : ABS_LEFTNORMAL);
			//int nHeight = GetSchemeHeight(SBP_ARROWBTN, bVert ? ABS_UPNORMAL : ABS_LEFTNORMAL);

			WTL::CRect rcDraw;

			rcDraw = GetLineUpRect( rc, bVert, sbar );

			// draw up arrow
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcDraw, NULL );


			// draw down arrow
			
			if ( !IsWindowEnabled() )
			{
				nState = bVert ? ABS_DOWNDISABLED : ABS_RIGHTDISABLED; 
			}
			else if ( SCROLL_DOWN == m_nPress )
			{
				nState = bVert ? ABS_DOWNPRESSED : ABS_RIGHTPRESSED; 
			}
			else if ( SCROLL_DOWN == m_nHot )
			{
				nState = bVert ? ABS_DOWNHOT : ABS_RIGHTHOT; 
			}
			else
				nState = bVert ? ABS_DOWNNORMAL : ABS_RIGHTNORMAL; 

			rcDraw = GetLineDownRect ( rc, bVert, sbar);
		
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcDraw, NULL );

			

			//SCROLLINFO si;
			//si.cbSize = sizeof (si);
			//si.fMask = SIF_ALL;
			
			//GetScrollInfo(&si);
			
			// draw thumb
			rcDraw = GetThumbRect( rc, bVert, sbar );


			if ( !IsWindowEnabled() )
			{
				nState = SCRBS_DISABLED; 
			}
			else if ( SCROLL_THUMB == m_nPress )
			{
				nState = SCRBS_PRESSED; 
			}
			else if ( SCROLL_THUMB == m_nHot )
			{
				nState = SCRBS_HOT; 
			}
			else
				nState = SCRBS_NORMAL; 

			nPart = bVert ? SBP_THUMBBTNVERT : SBP_THUMBBTNHORZ;

			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcDraw, NULL );

			// draw gripper
			WTL::CRect rcGripper = rcDraw;
			int nGripperWidth = GetSchemeWidth(bVert ? SBP_GRIPPERVERT : SBP_GRIPPERHORZ, SCRBS_NORMAL);
			int nGripperHeight = GetSchemeHeight(bVert ? SBP_GRIPPERVERT : SBP_GRIPPERHORZ, SCRBS_NORMAL);

			if ( bVert )
			{
				rcGripper.top = rcGripper.top + ( rcDraw.Height() - nGripperHeight ) / 2 ;
				rcGripper.bottom = 	rcGripper.top + nGripperHeight;
				rcGripper.left = rcGripper.left + ( rcDraw.Width() - nGripperWidth ) / 2;
				rcGripper.right =  rcGripper.left + nGripperWidth;		
			}
			else
			{
				rcGripper.left= rcGripper.left + ( rcDraw.Width() - nGripperWidth ) / 2 ;
				rcGripper.right = 	rcGripper.left + nGripperWidth;
				rcGripper.top = rcGripper.top + ( rcDraw.Height() - nGripperHeight ) / 2;
				rcGripper.bottom =  rcGripper.top + nGripperHeight;	
			}

			nPart = bVert ? SBP_GRIPPERVERT : SBP_GRIPPERHORZ;
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcGripper, NULL );

			// draw up 
			WTL::CRect rcThumbSpace;
			if ( bVert )
			{
				nPart = SBP_LOWERTRACKVERT;
//				rcThumbSpace.left = rcDraw.left;
//				rcThumbSpace.right = rcDraw.right;
//				rcThumbSpace.top =  sbar.dxyLineButton;
//				rcThumbSpace.bottom = rcDraw.top;
			}
			else
			{
				nPart = SBP_LOWERTRACKHORZ;
//				rcThumbSpace.left = sbar.dxyLineButton;
//				rcThumbSpace.right = rcDraw.left;
//				rcThumbSpace.top =  rcDraw.top;
//				rcThumbSpace.bottom = rcDraw.bottom;
			}

			rcThumbSpace = GetPageUpRect( rc, bVert, sbar );

			if ( _scheme && rcThumbSpace.right > rcThumbSpace.left && rcThumbSpace.bottom > rcThumbSpace.top )
				_scheme->DrawBackground(memdc, class_id, nPart, SCRBS_NORMAL, &rcThumbSpace, NULL );


			if ( bVert )
			{
				nPart = SBP_UPPERTRACKVERT;
//				rcThumbSpace.left = rcDraw.left;
//				rcThumbSpace.right = rcDraw.right;
//				rcThumbSpace.top =  rcDraw.bottom;
//				rcThumbSpace.bottom = rc.bottom - sbar.dxyLineButton;
			}
			else
			{
				nPart = SBP_UPPERTRACKHORZ;
//				rcThumbSpace.left = rcDraw.right;
//				rcThumbSpace.right = rc.right - sbar.dxyLineButton;
//				rcThumbSpace.top =  rcDraw.top;
//				rcThumbSpace.bottom = rcDraw.bottom;
			}

			rcThumbSpace = GetPageDownRect( rc, bVert, sbar );

			if ( _scheme && rcThumbSpace.right > rcThumbSpace.left && rcThumbSpace.bottom > rcThumbSpace.top )
				_scheme->DrawBackground(memdc, class_id, nPart, SCRBS_NORMAL, &rcThumbSpace, NULL );

			return 0;
		}
		
		int HitTest(WTL::CPoint& pt, BOOL bVer)
		{
			WTL::CRect rc;
			GetClientRect(&rc);
			if ( bVer ) //ֱ
			{
				
			}
			else
			{

			}

		}

		int GetState(const BOOL& bMouseDown, const WTL::CRect& rHalf, WTL::CPoint& ptCursor, const BOOL& bVert, const BOOL bLeftUp)
		{
			int nState;
			if ( bVert )
			{
				// ֱ
				if ( !IsWindowEnabled() )
				{
					nState = bLeftUp ? UPS_DISABLED : DNS_DISABLED;
				}
				else
				{
					if ( !rHalf.PtInRect(ptCursor) )
					{
						nState = bLeftUp ? UPS_NORMAL : DNS_NORMAL;
					}
					else
					{
						if ( bMouseDown )
							nState = bLeftUp ? UPS_PRESSED : DNS_PRESSED;
						else
							nState = bLeftUp ? UPS_HOT : DNS_HOT;
					}
				}
			}
			else
			{
				//ˮƽ
				if ( !IsWindowEnabled() )
				{
					nState = bLeftUp ? UPHZS_DISABLED : DNHZS_DISABLED;
				}
				else
				{
					if ( !rHalf.PtInRect(ptCursor) )
					{
						nState = bLeftUp ? UPHZS_NORMAL : DNHZS_NORMAL;
					}
					else
					{
						if ( bMouseDown )
							nState = bLeftUp ? UPHZS_PRESSED : DNHZS_PRESSED;
						else
							nState = bLeftUp ? UPHZS_HOT : DNHZS_HOT;
					}
				}
			}
			return nState;
		}

	private:
	
		int m_nPress;
		int	m_nHot;


		BOOL		m_bDrag;
		CPoint		m_ptDrag;
		int			m_nDragPos;

		UINT		m_uClicked;
		BOOL		m_bNotify;
		UINT		m_uHtPrev;
		BOOL		m_bMouseHover;
		SCROLLINFO	m_si;

	};

}; // namespace 
