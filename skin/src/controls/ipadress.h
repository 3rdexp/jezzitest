#pragma once

#include "../base/skinctrl.h"

namespace Skin {

	template<class BaseT = CIPAddressCtrl>
	struct SkinIpAddress : public SkinControlImpl<SkinIpAddress, BaseT>
	{
		enum { class_id = IPADDRESS };

		SkinIpAddress()
		{
			m_nPart = EP_EDITTEXT;
		}
		typedef SkinIpAddress<BaseT> this_type;
		typedef SkinControlImpl<SkinIpAddress, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		END_MSG_MAP()

		

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			LONG lExStyle;
			lExStyle = GetExStyle();
			if (( GetStyle() & WS_BORDER) ||
				(lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
			{		
				CRect rcw;
				GetWindowRect(&rcw);
				rcw.right -= rcw.left;
				rcw.bottom -= rcw.top;
				rcw.top = rcw.left = 0;

				HDC hdc = GetWindowDC(  );

				// �������
				CBrush brBorder;
				int nState = GetState();
				COLORREF cr;
				_scheme->GetColor(class_id, m_nPart, nState, TMT_BORDERCOLOR, &cr);
				brBorder.CreateSolidBrush( cr ); 
				FrameRect(hdc, CRect(0, 0, rcw.Width(), rcw.Height()), (HBRUSH)brBorder);
				brBorder.DeleteObject();

				// �����ڿ�

				if ((lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
				{
					InflateRect(&rcw, -1, -1);
					LONG lStyle = GetStyle();			
					CBrush brBorder;
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
			return ETS_NORMAL;
		}
	private:
		int m_nPart;
	};

}; // namespace 