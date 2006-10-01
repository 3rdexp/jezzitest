#pragma once

#include "../base/skinctrl.h"

namespace Skin {
	template<class BaseT = CTrackBarCtrl>
	struct SkinTrackBarCtrl : public SkinControlImpl<SkinTrackBarCtrl, BaseT>
	{
		enum { class_id = SPIN };

		typedef SkinTrackBarCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinTrackBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			REFLECTED_NOTIFY_CODE_HANDLER_EX( NM_CUSTOMDRAW, OnCustomDraw )
		END_MSG_MAP()

		LRESULT OnCustomDraw(NMHDR* pNMHDR) 
		{
			NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNMHDR;
			

			if ( nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
			{
				if ( nmcd.dwItemSpec == TBCD_THUMB )
				{
					// .. now draw a diamond-shaped thumb
					CDC cdc = GetDC();
					int xx, yy, dx, dy, cx, cy;
					xx = nmcd.rc.left;
					yy = nmcd.rc.top;
					dx = 2;
					dy = 2;
					cx = nmcd.rc.right - xx - 1;
					cy = nmcd.rc.bottom - yy - 1;
					POINT pts[8] = { {xx+dx, yy}, 
					{xx, yy+dy},
					{xx, yy+cy-dy}, 
					{xx+dx, yy+cy},
					{xx+cx-dx, yy+cy}, 
					{xx+cx, yy+cy-dy}, 
					{xx+cx, yy+dy}, 
					{xx+cx-dx, yy} };

					cdc.Polygon( pts, 8 );  // diamond shape

					cdc.Detach();

					return CDRF_SKIPDEFAULT;
				}
			}
			SetMsgHandled(FALSE);
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

			return 0;
		}

	private:
		int m_nPart;
	};

}; // namespace 