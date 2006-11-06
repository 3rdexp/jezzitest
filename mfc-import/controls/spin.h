#pragma once

#include "../base/skinctrl.h"


namespace Skin {
	template<class BaseT = CUpDownCtrlT>
	struct SkinUpDownCtrl : public SkinControlImpl<SkinUpDownCtrl, BaseT>
	{
		enum { class_id = SPIN };

		typedef SkinUpDownCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinUpDownCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
		END_MSG_MAP()

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

			// pressed?
			CPoint ptCursor;

			BOOL bMouseDown = (GetAsyncKeyState(MK_LBUTTON) & 0x8000);

			GetCursorPos(&ptCursor);

			CRect rMem;
			GetWindowRect(&rMem);
			ptCursor.Offset(-rMem.TopLeft());
			rMem.OffsetRect(-rMem.TopLeft());

			BOOL bVert = ! (GetStyle() & UDS_HORZ);
			int nPart;
			int nState;
			
			// draw left/up arrow
			CRect rHalf = rMem;
			if (bVert)
				rHalf.bottom = (rHalf.top + rHalf.bottom) / 2;
			else
				rHalf.right = (rHalf.left + rHalf.right) / 2;

			nState = GetState( bMouseDown, rHalf, ptCursor, bVert, TRUE);
			nPart = bVert ? SPNP_UP  : SPNP_UPHORZ;

			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rHalf, NULL );

			// draw right/down arrow
			if (bVert)
			{
				rHalf.top = rHalf.bottom;
				rHalf.bottom = rMem.bottom;
			}
			else
			{
				rHalf.left = rHalf.right;
				rHalf.right = rMem.right;
			}
			nState = GetState( bMouseDown, rHalf, ptCursor, bVert, FALSE);
			nPart = bVert ? SPNP_DOWN  : SPNP_DOWNHORZ;

			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rHalf, NULL );
			return 0;
		}

		int GetState(const BOOL& bMouseDown, const CRect& rHalf, CPoint& ptCursor, const BOOL& bVert, const BOOL bLeftUp)
		{
			int nState;
			if ( bVert )
			{
				// ´¹Ö±
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
				//Ë®Æ½
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
		int m_nPart;
	};

}; // namespace 