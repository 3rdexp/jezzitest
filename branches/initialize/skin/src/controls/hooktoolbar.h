#pragma once

#include "../base/skinctrl.h"
//#include "../base/skinhookbase.h"
#include "toolbar.h"
#include <atlctrls.h>
#include <atlwin.h>

namespace Skin {

	template<class BaseT = WTL::CToolBarCtrl>
	struct SkinHookToolBarCtrl : public SkinControlImpl<SkinHookToolBarCtrl, BaseT>
	{
	public:
		SkinHookToolBarCtrl()
		{
			_classid	= TOOLBAR;
			_pSkinToolBar = NULL;
		}

		void OnFirstMessage()
		{	
		}

		~SkinHookToolBarCtrl()
		{
		}

		//typedef SkinToolBarCtrl<BaseT> this_type;
		//typedef SkinControlImpl<SkinToolBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(SkinHookToolBarCtrl)
			MESSAGE_HANDLER(WM_CREATE, OnCreate)	
			MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)		
		END_MSG_MAP()

		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			_pSkinToolBar = new SkinToolBarCtrl();
			if ( _pSkinToolBar )
				_pSkinToolBar->Install( m_hWnd );
			return 0;
		}

		LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetMsgHandled(FALSE);
			return 0;
		}

	private:
		SkinToolBarCtrl* _pSkinToolBar;
	};

}; // namespace 
