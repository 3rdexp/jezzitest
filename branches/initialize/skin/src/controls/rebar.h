#pragma once

#include "../base/skinctrl.h"

namespace Skin {
	template<class BaseT = WTL::CReBarCtrl>
	struct SkinReBarCtrl : public SkinControlImpl<SkinReBarCtrl, BaseT>
	{
		enum { class_id = REBAR };
		
		SkinReBarCtrl()
		{
			
		}
		typedef SkinReBarCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinReBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			//MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		END_MSG_MAP()

	
		void DrawGripper(CWindowDC *pDC, CRect& rectWindow)
		{
			// get the gripper rect (1 pixel smaller than toolbar)
			CRect gripper = rectWindow;
			//Disable these line to disable the XP Gripper
			

			pDC->FillSolidRect( gripper, RGB( 125, 125, 125) );//Enable this to get the classic color

			gripper.right = gripper.left+3;
			//pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));//Enable this to get the classic gripper
			gripper.OffsetRect(+4,0);
			gripper.DeflateRect(0,6);
			CBrush brGripperHorz;
			brGripperHorz.CreateSolidBrush(RGB(0, 255, 0));
			pDC->FillRect (gripper,brGripperHorz);//Disable This to disable the XP Gripper
			rectWindow.left += 8; 

			/*
			if (m_dwStyle & CBRS_FLOATING)
			{          // no grippers
			}
			else if (m_dwStyle & CBRS_ORIENT_HORZ)
			{          // gripper at left
				gripper.right = gripper.left+3;
				//pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));//Enable this to get the classic gripper
				gripper.OffsetRect(+4,0);
				gripper.DeflateRect(0,6);
				pDC->FillRect (gripper,&m_brGripperHorz);//Disable This to disable the XP Gripper
				rectWindow.left += 8; 
			} else
			{          // gripper at top
				gripper.bottom = gripper.top+3;
				//pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));//Enable this to get the classic look
				gripper.OffsetRect(0,+4);
				gripper.DeflateRect(6,0);
				pDC->FillRect (gripper,&m_brGripperVert);//Disable this to disable the XP Gripper
				rectWindow.top += 8;
			}
			*/
		}

		void EraseNonClientEx()
		{
			// get window DC that is clipped to the non-client area
			CWindowDC dc(m_hWnd);
			CRect rectClient;
			GetClientRect(rectClient);
			CRect rectWindow;
			GetWindowRect(rectWindow);
			ScreenToClient(rectWindow);
			rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
			dc.ExcludeClipRect(rectClient);
			//dc.SetBkColor( RGB(255, 0, 0 ) );

			// draw borders in non-client area
			rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);

			//if (m_dwExStyle & CBRS_RAISEDBORDER)
			//DrawRaisedBorders(&dc, rectWindow);
			//else
			//	DrawBorders(&dc, rectWindow);

			// draw backimg
			if( _scheme )
				_scheme->DrawBackground(dc, class_id, 8, 1, &rectWindow, NULL );

			// erase parts not drawn
			dc.IntersectClipRect(rectWindow);
			//SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
			//DefWindowProc(WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0);
			// draw gripper in non-client area
			DrawGripper(&dc, rectWindow);
		}

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			EraseNonClientEx();
			return 0;
		}	

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CPaintDC dc(m_hWnd);
			CRect rcClip;
			dc.GetClipBox (rcClip);
			
			// draw backimg
			if( _scheme )
				_scheme->DrawBackground(dc, class_id, 1, 1, &rcClip, NULL );

			//dc.FillSolidRect (rcClip, RGB( 255, 0, 0 ));//Enable This to get the Classic Color
			return 0;
		}

		BOOL IsHiddenBand( LPREBARBANDINFO lprbbi )
		{
			return ( ( lprbbi->fStyle & RBBS_HIDDEN ) || (( GetStyle() & CCS_VERT )
					&& (lprbbi->fStyle & RBBS_NOVERT) ) );
		}

	
		
	private:
		
	};

}; // namespace 