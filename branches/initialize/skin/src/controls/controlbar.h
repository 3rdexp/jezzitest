#pragma once

#include "../base/skinctrl.h"

namespace Skin {

class SkinHookBase : public CWindow
{
public:
	virtual bool UninstallHook( HWND hWnd ) = 0;
	virtual bool InstallHook( HWND hWnd ) = 0;
};

class SkinControlBar : public SkinControlImpl<SkinControlBar, SkinHookBase,
	HookPolicy>
{
public:
	
	~SkinControlBar()
	{
		UninstallHook( m_hWnd );
	}
	enum { class_id = CONTROLBAR };

	BEGIN_MSG_MAP(CWindow)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)	
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect rect;
		GetClientRect(&rect);
		CDCHandle dc = (HDC)wParam;
		//CMemoryDC dc ( (HDC)wParam, rect);
		dc.FillSolidRect (rect, RGB( 255, 0, 255));
		return 1;
	}	

};

}; // namespace 