/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 11.07.2000 16:03:14
 Version: 1.0.0

 based on CJColorPicker class 
	Chris Maunder (chrismaunder@codeguru.com), 
	Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
	Copyright (c) 1998.
*/

#ifndef _ColorPicker_a66cf345_8aa8_4959_9d27_29d44e9dddcf
#define _ColorPicker_a66cf345_8aa8_4959_9d27_29d44e9dddcf

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#define OCM_COMMAND_CODE_HANDLER(code, func) \
	if(uMsg == OCM_COMMAND && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

template <class T>
class kColorPickerButtonT  : 	
	public CWindowImpl<kColorPickerButtonT, T>
{
public:
	OLE_COLOR m_clr;
	bool m_bActive;
		
public:
	kColorPickerButtonT ()
	{
		m_clr = 0x80000000 | COLOR_WINDOW;
		m_bActive = false;
	}

	DECLARE_WND_SUPERCLASS(0, T::GetWndClassName())

	BEGIN_MSG_MAP(kColorPickerButtonT)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)		
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem);
		OCM_COMMAND_CODE_HANDLER(BN_CLICKED, OnClicked)	
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);				
		return lRet;
	}
		
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		DRAWITEMSTRUCT *lpDIS = reinterpret_cast<DRAWITEMSTRUCT *>(lParam);

		ATLASSERT(lpDIS->CtlType == ODT_BUTTON); 
	
		CDCHandle dc = lpDIS->hDC;		
		
		UINT	itemState	= lpDIS->itemState;
		RECT	rcItem		= lpDIS->rcItem;
		DWORD	dwState		= EDGE_RAISED;
		DWORD	dwArrow		= DFCS_SCROLLDOWN;
	
		// erase everything....
		dc.FillRect( &rcItem, ::GetSysColorBrush( COLOR_3DFACE ));
	
		// set display flags based on state.
		if((itemState & ODS_SELECTED) || m_bActive) 
		{
			dwState = EDGE_SUNKEN;
			dwArrow = DFCS_SCROLLDOWN|DFCS_PUSHED;
		}
		if(itemState & ODS_DISABLED) 
		{
			dwArrow = DFCS_SCROLLDOWN|DFCS_INACTIVE;
		}
	
		// Draw the drop arrow.
		RECT rcArrow =  rcItem;
		rcArrow.left = rcArrow.right - rcArrow.left - ::GetSystemMetrics( SM_CXHTHUMB );

		dc.DrawFrameControl( &rcArrow, DFC_SCROLL, dwArrow );
		dc.Draw3dRect( &rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
		::InflateRect( &rcArrow, -1, -1 );
		dc.Draw3dRect( &rcArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ));
	
		if((itemState & ODS_SELECTED) || m_bActive)
			::OffsetRect( &rcArrow, 1, 1 );
	
		// draw the seperator line.
		CPen penShadow;
		penShadow.CreatePen( PS_SOLID, 1, ::GetSysColor( COLOR_3DSHADOW ));
		dc.SelectPen( penShadow );
		dc.MoveTo( rcArrow.left - 1, rcArrow.top + 2 );
		dc.LineTo( rcArrow.left - 1, rcArrow.bottom - 2 );

		CPen penHilite;
		penHilite.CreatePen( PS_SOLID, 1, ::GetSysColor( COLOR_3DHILIGHT ));
		dc.SelectPen( penHilite );
		dc.MoveTo( rcArrow.left, rcArrow.top + 2 );
		dc.LineTo( rcArrow.left, rcArrow.bottom - 2 );
	
		// draw the control border.
		dc.DrawEdge( &rcItem, dwState, BF_RECT );
	
		// draw the focus rect.
		InflateRect( &rcItem, -2, -2 );
		dc.Draw3dRect( &rcItem, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DFACE ) );
		if( itemState & ODS_FOCUS ) 
		{
			dc.DrawFocusRect( &rcItem );
		}
	
		// draw the color box.
		if( (itemState & ODS_SELECTED) || m_bActive)
			::OffsetRect( &rcItem, 1, 1 );
	
		::InflateRect( &rcItem, -2, -2 );
		rcItem.right = rcArrow.left - 4;
	
		COLORREF clr = 0;
		OleTranslateColor(m_clr, 0, &clr);

		CBrush br;
		br.CreateSolidBrush( ( itemState & ODS_DISABLED ) ? 
			::GetSysColor( COLOR_3DFACE ) : clr);
		HBRUSH hOldBrush = dc.SelectBrush(br); 
		dc.SelectStockPen( (itemState & ODS_DISABLED) ? WHITE_PEN : BLACK_PEN );
		dc.Rectangle( &rcItem );
		dc.SelectBrush( hOldBrush );
	    
		return 0;
	}

	LRESULT OnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_bActive = true;
		Invalidate();
		UpdateWindow();

		RECT rc;
		GetWindowRect(&rc);
				
		kColorDlg dlg;
		dlg.Pick(m_hWnd, rc.left, rc.bottom, m_clr, &m_clr);
	
		m_bActive = false;
		Invalidate();
		UpdateWindow();

		return 0;
	}	
};

typedef kColorPickerButtonT<CButton> kColorPickerButton;

#endif //_ColorPicker_a66cf345_8aa8_4959_9d27_29d44e9dddcf

