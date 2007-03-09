#pragma once

#include "../base/skinctrl.h"
//#include "../base/skinhookbase.h"

namespace Skin {

template<class BaseT = ATL::CWindow>
//struct SkinControlBar : public SkinControlImpl<SkinControlBar, BaseT, HookPolicy>
struct SkinControlBar : public SkinControlImpl<SkinControlBar, BaseT, HookPolicy>
{
public:
	SkinControlBar()
	{
		_classid = CONTROLBAR;
	}

	static LPCTSTR GetWndClassName()
	{
		return _T(WC_CONTROLBAR);
	}

	~SkinControlBar()
	{
		//UnInstallHook( m_hWnd );
	}

	void OnFirstMessage()
	{
		int i = 0;
	}

	//enum { class_id = CONTROLBAR };

	typedef SkinControlBar<BaseT> this_type;
	typedef SkinControlImpl<SkinControlBar, BaseT> base_type;

	BEGIN_MSG_MAP(this_type)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)	
		MESSAGE_HANDLER(WM_PAINT, OnPaint)	
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)	
	END_MSG_MAP()

	
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CWindowDC dc( m_hWnd );
		CRect rectClient;
		GetClientRect(rectClient);
		CRect rectWindow;
		GetWindowRect(rectWindow);
		ScreenToClient(rectWindow);
		rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
		dc.ExcludeClipRect(rectClient);     // draw borders in non-client area
		rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
		
		if (_scheme)
			_scheme->DrawBackground(dc, _classid, 1, 1, &rectWindow, NULL );

		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}	

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CRect rect;
		
		GetClientRect(&rect);
		
		WTL::CPaintDC dc(m_hWnd);
		
		if (_scheme)
			_scheme->DrawBackground(dc, _classid, 1, 1, &rect, NULL );

		return 0;
	}

};

}; // namespace 