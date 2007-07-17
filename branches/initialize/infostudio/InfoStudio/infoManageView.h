#pragma once

#include <atlctrls.h>
#include "commonctl/myatlsplit.h"
#include "commonctl/myatlcontainer.h"
#include "leftDlg.h"
#include "listViewDlg.h"
#include "infoDlg.h"
#include "yellowpageDlg.h"
#include "BackWebManageView.h"

class CInfoManageView : public CMySplitterWindowImpl<CInfoManageView, true>
{
public:
	CInfoManageView(void)
	{

	}
	virtual ~CInfoManageView(void)
	{

	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
	
		return FALSE;

	}

	CMyContainerWindow  _container;
	CLeftDlg			_leftDlg;
	CListViewDlg		_loginView;
	CInfoDlg			_infoDlg;
	CYellowPageDlg		_yellowDlg;
	//CLeftDlg			_rightDlg;
	CBackWebManageView	_backWebManageView;

	typedef CMySplitterWindowImpl<CInfoManageView, true> baseClass;

	BEGIN_MSG_MAP(CInfoManageView)		
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBk)					
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnEraseBk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	HWND CInfoManageView::Create(HWND hWndParent)
	{
		RECT rc = {0, 0,400,400};
		baseClass::Create(hWndParent, rc, NULL, 
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
		ATLASSERT(m_hWnd != NULL);
		m_cxyMinLeftPos = 140;
		//m_cxyMinRightPos = 180;
		m_bFullDrag = false;
		m_cxySplitBar = 2;
		_leftDlg.Create( m_hWnd, rc );
		::ShowWindow( _leftDlg.m_hWnd, SW_SHOW );

		//_rightDlg.Create( m_hWnd, rc );
		//::ShowWindow( _rightDlg.m_hWnd, SW_SHOW );

		_container.Create( m_hWnd, rc, NULL,  WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );		
	
		SetSplitterExtendedStyle(SPLIT_LEFTALIGNED);
		SetSplitterPos(250, false);
		SetSplitterPanes( _leftDlg, _container );

		//_loginView.Create( m_hWnd, rc );
		//_container.SetClient( _loginView );

		//_infoDlg.Create( m_hWnd, rc );
		//_container.SetClient( _infoDlg );

		_yellowDlg.Create( m_hWnd, rc );
		_backWebManageView.Create( m_hWnd );
		_container.SetClient( _backWebManageView );

		return m_hWnd;
	}
};
