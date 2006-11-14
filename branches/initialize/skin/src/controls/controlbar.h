#pragma once

#include "../base/skinctrl.h"

namespace Skin {

class SkinControlBar : public SkinControlImpl<SkinControlBar, SkinHookBase,
	HookPolicy>
{
public:
	
	~SkinControlBar()
	{
		UnInstallHook( m_hWnd );
	}
	enum { class_id = CONTROLBAR };

	BEGIN_MSG_MAP(CWindow)
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
		dc.FillSolidRect(rectWindow, RGB( 0, 0, 125));
		dc.IntersectClipRect(rectWindow);
		SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
		dc.SetBkColor(::GetSysColor(COLOR_3DFACE));
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect rect;
		GetClientRect(&rect);
		CDCHandle dc = (HDC)wParam;
		//CMemoryDC dc ( (HDC)wParam, rect);
		dc.FillSolidRect (rect, RGB( 255, 0, 255));
		return 1;
	}	

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect rect;
		GetClientRect(&rect);
		WTL::CPaintDC dc(m_hWnd);
		dc.FillSolidRect (rect, RGB( 255, 0, 255));
		return 0;
	}

};

}; // namespace 