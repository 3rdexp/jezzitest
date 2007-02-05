#pragma once

#include "../base/skinctrl.h"

namespace Skin {

	class SkinComboLbox : public SkinControlImpl<SkinComboLbox, ATL::CWindow,
		ClassPolicy>
	{
	public:
		BOOL bPrint;

		SkinComboLbox()
		{
			bPrint		= FALSE;
			_classid	= COMBOBOX;
		}

		void OnFirstMessage()
		{
			int i = 0;
		}


		static LPCTSTR GetWndClassName()
		{
			return WC_COMBOLBOX;
		}
		//enum { class_id = COMBOBOX };

		BEGIN_MSG_MAP(ATL::CWindow)
			//if ( hWnd == m_hWnd && bPrint)
			//	ATLTRACE("combolbox msg is %04x \n", uMsg);
			MSG_WM_CREATE(OnCreate)
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
			MESSAGE_HANDLER(WM_PRINT, OnPrint)
		END_MSG_MAP()

		BOOL OnCreate(LPCREATESTRUCT)
		{
			InstallScrollBar();
			SetMsgHandled(FALSE);
			return TRUE;
		}

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if ( !IsWindowVisible() )
			{
				bHandled = FALSE;
				return 0;
			}

			LONG lExStyle;
			lExStyle = GetExStyle();
			if (( GetStyle() & WS_BORDER) ||
				(lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
			{		
				HDC hdc = GetWindowDC(  );

				OnNcPaint( hdc );
				
				ReleaseDC( hdc );
			}
			else
			{
				LRESULT lRet = DefWindowProc();
				return lRet;
			}

			return 0;
		}

		LRESULT OnPrint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{
			if (lParam & PRF_NONCLIENT)
			{
				bPrint = TRUE;
				// base class first
				LRESULT lRet = DefWindowProc();

				if (IsPopup())
				{
					OnNcPaint((HDC)wParam);					
					return lRet;
				}

			}
			bHandled = FALSE;
			return 0 ;
		}

		void OnNcPaint( HDC hdc )
		{	
			LONG lExStyle;
			lExStyle = GetExStyle();

			WTL::CRect rcw;
			GetWindowRect(&rcw);
			rcw.right -= rcw.left;
			rcw.bottom -= rcw.top;
			rcw.top = rcw.left = 0;

			// 绘制外框
			WTL::CBrush brBorder;
			int nState = GetState();
			COLORREF cr;
			_scheme->GetColor(_classid, CP_DROPDOWNBUTTON, nState, TMT_BORDERCOLOR, &cr);
			brBorder.CreateSolidBrush( cr ); 
			FrameRect(hdc, WTL::CRect(0, 0, rcw.Width(), rcw.Height()), (HBRUSH)brBorder);
			brBorder.DeleteObject();

			// 绘制内框

			if ((lExStyle & WS_EX_CLIENTEDGE) || (lExStyle & WS_EX_STATICEDGE))
			{
				InflateRect(&rcw, -1, -1);
				LONG lStyle = GetStyle();			
				WTL::CBrush brBorder;
				_scheme->GetColor(_classid, CP_DROPDOWNBUTTON, nState, TMT_TEXTBORDERCOLOR, &cr);
				brBorder.CreateSolidBrush( cr ); 
				FrameRect(hdc, &rcw, (HBRUSH) brBorder);			
				if ((lExStyle & WS_EX_CLIENTEDGE) && (lExStyle & WS_EX_STATICEDGE))
				{
					InflateRect(&rcw, -1, -1);
					FrameRect(hdc, &rcw, (HBRUSH)brBorder);
				}
				brBorder.DeleteObject();
			}
		}

		int  GetState()
		{
			return CBXS_NORMAL;
		}

		BOOL IsPopup()
		{
			// else
			return (GetParent().m_hWnd == ::GetDesktopWindow());
		}

	private:
		
	};

}; // namespace 