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

	template<class BaseT = CTabCtrl>
	struct SkinTabCtrl : public SkinControlImpl<SkinTabCtrl, BaseT>
	{
		enum { class_id = TAB };
		SkinTabCtrl()
		{
			m_nPart = TABP_TABITEM;
		}
		typedef SkinTabCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinTabCtrl, BaseT> base_type;


		void GetDrawRect ( LPRECT pWindow, LPRECT pClient = NULL )
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

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		END_MSG_MAP()

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CPaintDC dc(m_hWnd);
			//DoPaint(dc);
			return 0;
		}

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CRect rWindow, rClient;
			GetDrawRect(rWindow, rClient);

			BOOL bHot = FALSE;//IsHot(rWindow);

			// draw the rest of the border
			CRect rPage;
			GetClientRect(rPage);
			AdjustRect(FALSE, rPage);
			rPage.top -= 2;
			rClient.top += rPage.top;

			COLORREF crBkgnd = RGB(255, 125, 125);
			const int EDGE = 1;
			HDC hdc = GetWindowDC();
			CDC dc;
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
				CRect rTab;
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
		int m_nPart;
	};

}; // namespace 