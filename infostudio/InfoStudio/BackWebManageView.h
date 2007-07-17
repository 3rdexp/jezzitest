#pragma once

#include <atlctrls.h>
#include "commonctl/myatlsplit.h"
#include "BackWebListView.h"
#include "webinfoDlg.h"

//这个是显示全部的后台管理界面

class CBackWebManageView : public CMySplitterWindowImpl<CBackWebManageView, false>
{
public:
	CBackWebManageView(void)
	{
		
	}
	virtual ~CBackWebManageView(void)
	{
		
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{

		return FALSE;

	}

	CBackWebListView	_WebListView;
	CWebInfoDlg			_webInfoDlg;

	typedef CMySplitterWindowImpl<CBackWebManageView, false> baseClass;

	BEGIN_MSG_MAP(CBackWebManageView)		
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBk)					
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnEraseBk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	HWND Create(HWND hWndParent)
	{
		RECT rc = {0, 0,400,400};
		baseClass::Create(hWndParent, rc, NULL, 
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
		ATLASSERT(m_hWnd != NULL);
		m_cxyMinLeftPos = 140;
		//m_cxyMinRightPos = 180;
		m_bFullDrag = false;
		m_cxySplitBar = 2;

		_webInfoDlg.Create( m_hWnd, rc );
		::ShowWindow( _webInfoDlg.m_hWnd, SW_SHOW );

		_WebListView.Create( m_hWnd );
		::ShowWindow( _WebListView.m_hWnd, SW_SHOW );

		SetSplitterExtendedStyle(SPLIT_LEFTALIGNED);
		SetSplitterPos(250, false);
		SetSplitterPanes( _WebListView, _webInfoDlg );

		_WebListView.SetWebInfoDlg(  &_webInfoDlg );

		return m_hWnd;
	}
};
