#pragma once

#include "../base/skinctrl.h"

namespace Skin {

	template<class BaseT = CEdit>
	struct SkinEdit : public SkinControlImpl<SkinEdit, BaseT>
	{
		//enum { class_id = EDIT };
		SkinEdit()
		{
			_nPart		= EP_EDITTEXT;
			_classid	= EDIT; 
		}

		void OnFirstMessage()
		{
		}

		typedef SkinEdit<BaseT> this_type;
		typedef SkinControlImpl<SkinEdit, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
			MSG_WM_CREATE(OnCreate)
		END_MSG_MAP()

		BOOL OnCreate(LPCREATESTRUCT)
		{
			_nPart = EP_EDITTEXT;

			TCHAR className[256];
			GetClassName(GetParent(), className, sizeof(className));
			if(lstrcmpi(className, "COMBOBOX") != 0) 
			{ 
				//Fix Combobox bug
				InstallScrollBar();
			}

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
				rcw.OffsetRect( -rcw.left, -rcw.top );

				HDC hdc = GetWindowDC(  );

				// �������
				WTL::CBrush brBorder;
				int nState = GetState();
				COLORREF cr;
				_scheme->GetColor(_classid, _nPart, nState, TMT_BORDERCOLOR, &cr);
				brBorder.CreateSolidBrush( cr ); 
				FrameRect(hdc, WTL::CRect(0, 0, rcw.Width(), rcw.Height()), (HBRUSH)brBorder);
				brBorder.DeleteObject();

				// �����ڿ�
				
				if ((lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
				{
					InflateRect(&rcw, -1, -1);
					LONG lStyle = GetStyle();			
					WTL::CBrush brBorder;
					_scheme->GetColor(_classid, _nPart, nState, TMT_TEXTBORDERCOLOR, &cr);
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
		int _nPart;
	};

}; // namespace 