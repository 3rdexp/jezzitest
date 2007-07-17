#pragma once

#include <atlctrlx.h>
#include "Resource.h"
#include "condef.h"

class CListViewDlg : public CDialogImpl<CListViewDlg>
{
public:

	CListViewDlg()
	{

	}

	~CListViewDlg()
	{

	}
	enum {IDD = IDD_LISTVIEWDLG};

	BEGIN_MSG_MAP( CListViewDlg )
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)

	END_MSG_MAP()

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == VK_RETURN)
			{
				if(::IsDialogMessage(m_hWnd, pMsg))
					return TRUE;
			}				
		}	
		return FALSE;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Init();		
		return 0;
	}

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	void OnSize(UINT wParam, CSize size)
	{
		DefWindowProc();
		int cx = size.cx;
		int cy = size.cy;

		if (_listView.m_hWnd)
			_listView.SetWindowPos(NULL, 0, 0, cx , cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	}

	//funciton
	void Init()
	{
		_listView.Create(m_hWnd,rcDefault,NULL,
			WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS | WS_VSCROLL | WS_HSCROLL);		

		_listView.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES );

		for (int i = 0; i< sizeof(LOGINFIELD); i++)
		{
			_listView.InsertColumn(i, LOGINFIELD[i]);
			_listView.SetColumnWidth(i, LOGINFIELDSIZE[i]);
		}
	}

public:

	CSortListViewCtrl	_listView;
};