#pragma once

#include <atlctrls.h>
#include "commonctl/myatlsplit.h"

#include "leftsortdlg.h"
#include "weblistDlg.h"
#include "webinfoDlg.h"

//这个是显示分类和网站列表的界面

class CBackWebListView : public CMySplitterWindowImpl<CBackWebListView, true>
{
public:
	CBackWebListView(void)
	{
		_pWebInfoDlg = NULL;
	}
	virtual ~CBackWebListView(void)
	{

	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{

		return FALSE;

	}

	CLeftSortDlg	_sortDlg;		//左侧的行业分类树
	CWebListDlg		_webListDlg;	//右侧的网站信息列表

	CWebInfoDlg*	_pWebInfoDlg;	//下面的显示具体现在的网站的信息

	typedef CMySplitterWindowImpl<CBackWebListView, true> baseClass;

	BEGIN_MSG_MAP(CBackWebListView)		
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

		_sortDlg.Create( m_hWnd, rc );
		::ShowWindow( _sortDlg.m_hWnd, SW_SHOW );

		_webListDlg.Create( m_hWnd, rc );
		::ShowWindow( _webListDlg.m_hWnd, SW_SHOW );

		SetSplitterExtendedStyle(SPLIT_LEFTALIGNED);
		SetSplitterPos(250, false);
		SetSplitterPanes( _sortDlg, _webListDlg );


		return m_hWnd;
	}

	void SetWebInfoDlg( CWebInfoDlg* pWebInfoDlg )
	{
		_pWebInfoDlg = pWebInfoDlg;
		_webListDlg._pWebInfoDlg = pWebInfoDlg;

		pWebInfoDlg->_pWebListDlg = &_webListDlg;
	}
};
