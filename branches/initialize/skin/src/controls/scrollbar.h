#pragma once

#include "../base/skinctrl.h"


namespace Skin {

	template<class BaseT = CScrollBar>
	struct SkinScrollBar : public SkinControlImpl<SkinScrollBar, BaseT>
	{
		enum { class_id = SCROLLBAR };

		enum
		{
			SCROLL_UP,
			SCROLL_DOWN,
			SCROLL_THUMB
		};

		SkinScrollBar()
		{
			
		}
		typedef SkinScrollBar<BaseT> this_type;
		typedef SkinControlImpl<SkinScrollBar, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		END_MSG_MAP()

		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			//InstallScrollBar();
			bHandled = FALSE;
			return 0;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CPaintDC dc(m_hWnd);
			DoPaint(dc);	
			return 0;
		}

		LRESULT DoPaint(HDC dc)
		{
			CRect rc;
			GetClientRect(&rc);

			CMemoryDC memdc(dc, rc);

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


			int nWidth = GetSchemeWidth(SBP_ARROWBTN, bVert ? ABS_UPNORMAL : ABS_LEFTNORMAL);
			int nHeight = GetSchemeHeight(SBP_ARROWBTN, bVert ? ABS_UPNORMAL : ABS_LEFTNORMAL);

			CRect rcDraw;
			rcDraw = rc;
			if ( bVert )
			{
				rcDraw.bottom = rcDraw.top + nHeight;
			}
			else
			{
				rcDraw.right = rcDraw.left + nWidth;
			}
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

			rcDraw = rc;
			if ( bVert )
			{
				rcDraw.top = rcDraw.bottom - nHeight;
			}
			else
			{
				rcDraw.left = rcDraw.right - nWidth;
			}
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcDraw, NULL );

			SCROLLINFO si;
			si.cbSize = sizeof (si);
			si.fMask = SIF_ALL;
			
			GetScrollInfo(&si);
			
			// draw thumb
			rcDraw = rc;
			if ( bVert )
			{
				int nThumbSpace = rc.Height() - 2 * nHeight;
				int nh = (int)(nThumbSpace * si.nPage * 1.0/(si.nMax-si.nMin+1));
				rcDraw.top = rcDraw.top + nHeight + ( nThumbSpace - nh ) / 2;
				rcDraw.bottom = rcDraw.top + nh;
			}
			else
			{
				int nThumbSpace = rc.Width() - 2 * nWidth;
				int nh = (int)(nThumbSpace * si.nPage * 1.0/(si.nMax-si.nMin+1));
				rcDraw.left = rcDraw.left + nHeight + ( nThumbSpace - nh ) / 2;
				rcDraw.right = rcDraw.left + nh;
			}


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
			CRect rcGripper = rcDraw;
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
			CRect rcThumbSpace;
			if ( bVert )
			{
				nPart = SBP_LOWERTRACKVERT;
				rcThumbSpace.left = rcDraw.left;
				rcThumbSpace.right = rcDraw.right;
				rcThumbSpace.top =  nHeight;
				rcThumbSpace.bottom = rcDraw.top;
			}
			else
			{
				nPart = SBP_LOWERTRACKHORZ;
				rcThumbSpace.left = nWidth;
				rcThumbSpace.right = rcDraw.left;
				rcThumbSpace.top =  rcDraw.top;
				rcThumbSpace.bottom = rcDraw.bottom;
			}
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, SCRBS_NORMAL, &rcThumbSpace, NULL );


			if ( bVert )
			{
				nPart = SBP_UPPERTRACKVERT;
				rcThumbSpace.left = rcDraw.left;
				rcThumbSpace.right = rcDraw.right;
				rcThumbSpace.top =  rcDraw.bottom;
				rcThumbSpace.bottom = rc.bottom - nHeight;
			}
			else
			{
				nPart = SBP_UPPERTRACKHORZ;
				rcThumbSpace.left = rcDraw.right;
				rcThumbSpace.right = rc.right - nWidth;
				rcThumbSpace.top =  rcDraw.top;
				rcThumbSpace.bottom = rcDraw.bottom;
			}
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, SCRBS_NORMAL, &rcThumbSpace, NULL );

			return 0;
		}
		
		int HitTest(CPoint& pt, BOOL bVer)
		{
			CRect rc;
			GetClientRect(&rc);
			if ( bVer ) //垂直
			{
				
			}
			else
			{

			}

		}

		int GetState(const BOOL& bMouseDown, const CRect& rHalf, CPoint& ptCursor, const BOOL& bVert, const BOOL bLeftUp)
		{
			int nState;
			if ( bVert )
			{
				// 垂直
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
				//水平
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
	};

}; // namespace 