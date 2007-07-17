#pragma once

#include <atlctrls.h>
#include "commonctl/myatlsplit.h"

#include "leftsortdlg.h"
#include "weblistDlg.h"
#include "webinfoDlg.h"

//�������ʾ�������վ�б�Ľ���

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

	CLeftSortDlg	_sortDlg;		//������ҵ������
	CWebListDlg		_webListDlg;	//�Ҳ����վ��Ϣ�б�

	CWebInfoDlg*	_pWebInfoDlg;	//�������ʾ�������ڵ���վ����Ϣ

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
