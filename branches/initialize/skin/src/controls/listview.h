#pragma once

#include "../base/skinctrl.h"

namespace Skin {
	template<class BaseT = WTL::CListViewCtrl>
	struct SkinListViewCtrl : public SkinControlImpl<SkinListViewCtrl, BaseT>
	{
		enum { class_id = LISTVIEW };
		SkinListViewCtrl()
		{
			m_nPart = LVP_LISTITEM;
		}
		typedef SkinListViewCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinListViewCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
			MSG_WM_CREATE(OnCreate)
		END_MSG_MAP()

		BOOL OnCreate(LPCREATESTRUCT)
		{
			InstallScrollBar();
			SetMsgHandled(FALSE);
			return TRUE;
		}

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LONG lExStyle;
			lExStyle = GetExStyle();
			if (( GetStyle() & WS_BORDER) ||
				(lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
			{		
				WTL::CRect rcw;
				GetWindowRect(&rcw);
				rcw.right -= rcw.left;
				rcw.bottom -= rcw.top;
				rcw.top = rcw.left = 0;

				HDC hdc = GetWindowDC(  );

				// 绘制外框
				WTL::CBrush brBorder;
				int nState = GetState();
				COLORREF cr;
				_scheme->GetColor(class_id, m_nPart, nState, TMT_BORDERCOLOR, &cr);
				brBorder.CreateSolidBrush( cr ); 
				FrameRect(hdc, WTL::CRect(0, 0, rcw.Width(), rcw.Height()), (HBRUSH)brBorder);
				brBorder.DeleteObject();

				// 绘制内框
				if ((lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
				{
					InflateRect(&rcw, -1, -1);
					LONG lStyle = GetStyle();			
					WTL::CBrush brBorder;
					_scheme->GetColor(class_id, m_nPart, nState, TMT_TEXTBORDERCOLOR, &cr);
					brBorder.CreateSolidBrush( cr ); 
					FrameRect(hdc, &rcw, (HBRUSH) brBorder);			
					if ((lExStyle & WS_EX_CLIENTEDGE) && (lExStyle & WS_EX_STATICEDGE))
					{
						InflateRect(&rcw, -1, -1);
						FrameRect(hdc, &rcw, (HBRUSH)brBorder);
					}
					brBorder.DeleteObject();
				}
				ReleaseDC( hdc );
			}
			else
			{
				LRESULT lRet = DefWindowProc();
				return lRet;
			}

			return 0;
		}

		int  GetState()
		{
			return LIS_NORMAL;
		}
	private:
		int m_nPart;
	};

}; // namespace 